#include "StdAfx.h"
#include "AnimationClipManager.h"
#include "NodeExporter.h"
#include "ControllerExporter.h"
#include "Common/ConversionFunctions.h"
#include "ICAnimationSelector.h"
#include "ICBillboardController.h"
#include "MAXHelpers.h"

#include <CS/BipedAPI.h>
#include <CS/IMixer.h>
#include <notetrck.h>
#include <sstream>

extern ICAnimationSelector g_animationSelector;

#define MAX_ANIM_CLIP_NAME 256

AnimationClipManager *AnimationClipManager::s_pAnimationClipManager = NULL;

AnimationClipManager::~AnimationClipManager()
{
	
}

DefNoteTrack *AnimationClipManager::GetNoteTrack( INode *pNode )
{
	if ( pNode->NumNoteTracks() == 0 )
	{
		return NULL;
	}
	
	return ( DefNoteTrack * )pNode->GetNoteTrack( 0 );	
}

int AnimationClipManager::GetNoteKeyIndexByName( INode *pNode, const char *name )
{
	DefNoteTrack *pNoteTrack = GetNoteTrack( pNode );

	int keyIdx = -1;
	for ( int index = 0; index < pNoteTrack->NumKeys(); index++ )
	{
		if ( !strcmp( pNoteTrack->keys[index]->note.data(), name ))
		{
			keyIdx = index;
			break;
		}
	}
	
	return keyIdx;
}

const char *AnimationClipManager::GetAnimClipListBoxName( int clipId )
{
	return m_animClipListBoxNames[clipId].c_str();
}

int AnimationClipManager::GetAnimClipStartNoteKeyIndex( INode *pNode, int clipId )
{
	DefNoteTrack *pNoteTrack = GetNoteTrack( pNode );
	int clipIndex = 0;

	int numKeys = pNoteTrack->NumKeys();
	int noteKeyIndex = -1;
	for ( noteKeyIndex = 0; noteKeyIndex < numKeys; noteKeyIndex++ )
	{
		const char *pNoteKeyName = pNoteTrack->keys[noteKeyIndex]->note.data();
		const char *pPrefixFirstChar = strstr( pNoteKeyName, ANIMCLIP_END_NOTEKEY_PREFIX );
		if ( pPrefixFirstChar != pNoteKeyName )
		{
			if ( clipIndex == clipId )
			{
				break;
			}
			clipIndex++;
		}
	}

	return noteKeyIndex == numKeys ? -1 : noteKeyIndex;
}

int AnimationClipManager::GetNumAnimClips()
{
	return m_animClips.size();
}

const char *AnimationClipManager::GetAnimClipName( int clipId )
{
	return m_animClips[clipId].GetName().c_str();
}

int AnimationClipManager::GetAnimClipStart( int clipId )
{
	return m_animClips[clipId].GetStart();
}

int AnimationClipManager::GetAnimClipEnd( int clipId )
{
	return m_animClips[clipId].GetEnd();
}

int AnimationClipManager::GetAnimClipFlags( int clipId )
{
	return m_animClips[clipId].GetFlags();
}

void AnimationClipManager::SetAnimClipFlags( int clipId, int flags )
{
	m_animClips[clipId].SetFlags( flags );
}

int AnimationClipManager::GetAnimClipId( const char *name )
{
	int animClipId = -1;

	int numAnimClips = GetNumAnimClips();
	for ( int i = 0; i < numAnimClips; ++i )
	{
		const char *currentName = GetAnimClipName( i );
		if ( !_stricmp( name, currentName ) )
		{
			animClipId = i;
			break;
		}
	}

	return animClipId;
}

void AnimationClipManager::PrepareAnimClipListBoxNames()
{
	int numClips = GetNumAnimClips();
	std::stringstream stream;

	m_animClipListBoxNames.clear();
	for ( int i = 0; i < numClips; i++ )
	{
		stream.str("");
		stream << GetAnimClipName( i ) << "  [ " << TicksToFrames( GetAnimClipStart( i ) ) << " , " << TicksToFrames( GetAnimClipEnd( i ) ) << " ]";
		m_animClipListBoxNames.push_back( stream.str() );
	}
}

bool AnimationClipManager::ValidateAnimRange( Interval animRange )
{
	return animRange.Start() != INT_MAX && 
		animRange.End() != INT_MIN &&
		animRange.Start() < animRange.End();
}

void AnimationClipManager::InitAnimClipsToExport()
{
	m_animClipsToExport.clear();
}

Interval AnimationClipManager::GetMixerKeyTimes( Animatable *pAnimatable, TicksToSecsMap &keyTimesMap )
{
	Interval interval;
	RESET_ANIM_RANGE( interval );

	AnimatableKeyTimeMap::iterator animatableIter = m_mixerAnimatableKeyTimes.find( pAnimatable );
	if ( animatableIter == m_mixerAnimatableKeyTimes.end() )
	{
		return interval;
	}

	keyTimesMap.merge( animatableIter->second );

	const TicksToSecsMap &mixerKeyTimesMap = animatableIter->second;
	//size_t keyCount = mixerKeyTimesMap.size();
	//for ( size_t keyId = 0; keyId < keyCount; ++keyId )
	for ( TicksToSecsMap::const_iterator iter = mixerKeyTimesMap.begin(); iter != mixerKeyTimesMap.end(); ++iter )
	{
		const TimeValue &keyTime = iter->first;
		keyTimesMap[keyTime] = iter->second;

		interval.SetStart( interval.Start() > keyTime ? keyTime : interval.Start() );
		interval.SetEnd( interval.End() < keyTime ? keyTime : interval.End() );
	}

	return interval;
}

Interval AnimationClipManager::GetSubAnimatableKeyTimes( Animatable *pAnimatable, TicksToSecsMap &keyTimesMap, int flags )
{
	Interval interval;
	RESET_ANIM_RANGE( interval );

	if ( !pAnimatable )
	{
		return interval;
	}

	if ( TEST_FLAG( flags, GAKTF_INCLUDE_MIXER_KEY_TIMES ) )
	{
		interval = GetMixerKeyTimes( pAnimatable, keyTimesMap );
	}

	IKeyControl *iKeys = GetKeyControlInterface( pAnimatable );
	if ( !iKeys )
	{
		return interval;
	}

	int numKeys = iKeys->GetNumKeys();
	if ( numKeys <= 0 )
	{
		return interval;
	}

	int keySize = iKeys->GetKeySize();
	void *p = malloc( keySize );
	IKey *pKey = ( IKey * )p;

	for ( int i = 0; i < numKeys; i++ )
	{
		iKeys->GetKey( i, pKey );
		if ( pKey == NULL )
		{
			continue;
		}

		const TimeValue &keyTime = pKey->time;
		keyTimesMap[ keyTime ] = TEST_FLAG( flags, GAKTF_CONVERT_KEYTIMES_TOSECS ) ? FSConvert::MaxTickToSecs( keyTime ) : FLT_MIN;

		interval.SetStart( interval.Start() > keyTime ? keyTime : interval.Start() );
		interval.SetEnd( interval.End() < keyTime ? keyTime : interval.End() );
	}

	free( p );

	return interval;
}

Interval AnimationClipManager::GetAnimatableKeyTimes( Animatable *pAnimatable, TicksToSecsMap &keyTimesMap, int flags )
{
	Interval animatableAnimRange;
	RESET_ANIM_RANGE( animatableAnimRange );

	if ( !pAnimatable )
	{
		return animatableAnimRange;
	}

	if ( TEST_FLAG( flags, GAKTF_IGNORE_BILLBOARD_CONTROLLER ) 
		&& pAnimatable->ClassID() == BILLBOARD_CONTROLLER_CLASS_ID )
	{
		return animatableAnimRange;
	}

	animatableAnimRange = GetSubAnimatableKeyTimes( pAnimatable, keyTimesMap, flags );

	int nSubs = pAnimatable->NumSubs();
	for (int i = 0; i < nSubs; i++)
	{
		Animatable* sub = pAnimatable->SubAnim(i);
		const Interval subAnimatableAnimRange = GetAnimatableKeyTimes( sub, keyTimesMap, flags );
		
		animatableAnimRange.SetStart( animatableAnimRange.Start() > subAnimatableAnimRange.Start() ? subAnimatableAnimRange.Start() : animatableAnimRange.Start() );
		animatableAnimRange.SetEnd( animatableAnimRange.End() < subAnimatableAnimRange.End() ? subAnimatableAnimRange.End() : animatableAnimRange.End() );
	}

	return animatableAnimRange;
}

void AnimationClipManager::GetBipedNodeKeyTimes( INode *pNode, AnimatableKeyTimeMap &animatableKeyTimes )
{
	if ( !pNode )
	{
		return;
	}

	Control *pController = pNode->GetTMController();
	if ( !ControllerExporter::IsBipedController( pController ) )
	{
		return;
	}

	TicksToSecsMap &keyTimesMap = animatableKeyTimes[pController];
	GetAnimatableKeyTimes( pController, keyTimesMap, GAKTF_NONE );

	int childrenCount = pNode->NumberOfChildren();
	for (int j = 0; j < childrenCount; j++)
	{
		GetBipedNodeKeyTimes( pNode->GetChildNode( j ), animatableKeyTimes );
	}
}

void AnimationClipManager::CalculateSceneMixerKeyTimes( ExportNodeMap &exportNodes )
{
	m_mixerAnimatableKeyTimes.clear();

	for ( ExportNodeMap::iterator it = exportNodes.begin(); it != exportNodes.end(); ++it )
	{
		INode *pNode = it->first;
		
		Control *pController = pNode->GetTMController();
		if ( !pController ||
			pController->ClassID() != BIPBODY_CONTROL_CLASS_ID )
		{
			continue;
		}

		IBipMaster *pBipMaster = GetBipMasterInterface( pController );
		DWORD activeModes = pBipMaster->GetActiveModes();
		if ( !TEST_FLAG( activeModes, BMODE_MIXER ) )
		{
			continue;
		}

		IMixer *pMixer = pBipMaster->GetMixer();
		FUAssert( pMixer, continue );

		int trackGroupCount = pMixer->NumTrackgroups();
		if ( trackGroupCount <= 0 )
		{
			continue;
		}

		// get tmp directory path
		fm::string tmpDirectory;
		{
			FUStringBuilder builder;
			builder.append( MAXHelpers::GetTmpDir() );
			builder.append( "\\" );
			builder.append( GetString( IDS_LIBDESCRIPTION ) );

			tmpDirectory = builder.ToString();
		}

		// get tmp file path
		fm::string tmpFilepath;
		{
			FUStringBuilder builder;

			builder.append( tmpDirectory );
			builder.append( "\\" );
			builder.append( pNode->GetName() );
			builder.append( "_" );
			builder.append( pNode->GetHandle() );
			builder.append( "_backup.bip" );

			tmpFilepath = builder.ToString();
		}

		// backup current biped anims
		//g_warningLog.append( builder.ToCharPtr() );
		CreateDirectory( tmpDirectory.c_str(), NULL );
		pBipMaster->EndModes( BMODE_MIXER, FALSE );
		pBipMaster->SaveBipfile( const_cast<CHAR *>( tmpFilepath.c_str() ), false, false );

		// get all key times from Mixer clips
		for ( size_t trackGroupId = 0; trackGroupId < trackGroupCount; ++trackGroupId )
		{
			IMXtrackgroup *pTrackGroup = pMixer->GetTrackgroup( trackGroupId );
			//const TCHAR *pTrackGroupName = pTrackGroup->GetName();
			int trackCount = pTrackGroup->NumTracks();
			for ( size_t trackId = 0; trackId < trackCount; ++trackId )
			{
				IMXtrack *pTrack = pTrackGroup->GetTrack( trackId );
				int trackType = pTrack->GetTrackType();
				if ( trackType == LAYERTRACK )
				{
					int clipCount = pTrack->NumClips( BOT_ROW );
					for ( int clipId = 0; clipId < clipCount; ++clipId )
					{
						IMXclip *pClip = pTrack->GetClip( clipId, BOT_ROW );
						TCHAR *pClipFile = pClip->GetFilename();

						int globalStart, globalEnd;
						pClip->GetGlobalBounds( &globalStart, &globalEnd );

						pMixer->CopyClipSourceToBiped( pClip );
						
						AnimatableKeyTimeMap animatableKeyTimeMap;
						GetBipedNodeKeyTimes( pNode, animatableKeyTimeMap );

						// apply offset and copy in global animatable key time map
						for ( AnimatableKeyTimeMap::iterator animatableKeyTimeMapIter = animatableKeyTimeMap.begin(); animatableKeyTimeMapIter != animatableKeyTimeMap.end(); ++animatableKeyTimeMapIter )
						{
							TicksToSecsMap &globalKeyTimeMap = m_mixerAnimatableKeyTimes[animatableKeyTimeMapIter->first];
							TicksToSecsMap &currentKeyTimeMap = animatableKeyTimeMapIter->second;
							
							for ( TicksToSecsMap::iterator keyTimeMapIter = currentKeyTimeMap.begin(); keyTimeMapIter != currentKeyTimeMap.end(); ++keyTimeMapIter )
							{
								TimeValue offsetTime = keyTimeMapIter->first + globalStart;
								globalKeyTimeMap[offsetTime] = FSConvert::MaxTickToSecs( offsetTime );
							}
						}
					}
				}
				else
				{
					// TODO for TRANSTRACK if necessary
				}
			}
		}

		// restore biped state
		pBipMaster->LoadBipStpfile( const_cast<CHAR *>( tmpFilepath.c_str() ), false, true, false, true, false, false, false, false, false, NULL, NULL );
		DeleteFile( tmpFilepath.c_str() );
		RemoveDirectory( tmpDirectory.c_str() );
		pBipMaster->BeginModes( BMODE_MIXER, FALSE );
	}
}

Interval AnimationClipManager::CalculateSceneAnimationKeyTimes( ExportNodeMap &exportNodes )
{
	RESET_ANIM_RANGE( m_sceneAnimRange );
	m_sceneKeyTimesMap.clear();

	for ( ExportNodeMap::iterator it = exportNodes.begin(); it != exportNodes.end(); ++it )
	{
		INode *pNode = it->first;

		Interval nodeBoundaryKeyTimes = GetAnimatableKeyTimes( pNode, m_sceneKeyTimesMap );

		int minKeyFrameTime = nodeBoundaryKeyTimes.Start();
		int maxKeyFrameTime = nodeBoundaryKeyTimes.End();
		
		m_sceneAnimRange.SetStart( m_sceneAnimRange.Start() > minKeyFrameTime ? minKeyFrameTime : m_sceneAnimRange.Start() );
		m_sceneAnimRange.SetEnd( m_sceneAnimRange.End() < maxKeyFrameTime ? maxKeyFrameTime : m_sceneAnimRange.End() );
	}

	return m_sceneAnimRange;
}

Interval AnimationClipManager::GetUserDefinedAnimClipsRange()
{
	return m_userDefinedAnimClipsRange;
}

Interval AnimationClipManager::GetExportedAnimClipsRange()
{
	return m_exportedAnimClipsRange;
}

Interval AnimationClipManager::GetSceneAnimRange()
{
	return m_sceneAnimRange;
}

Interval AnimationClipManager::GetTotalAnimRange()
{
	return m_totalAnimRange;
}

void AnimationClipManager::AddValidAnimClips( INode *pNode )
{
	DefNoteTrack *pNoteTrack = GetNoteTrack( pNode );
	if ( !pNoteTrack )
	{
		return;
	}

	int numKeys = pNoteTrack->keys.Count();
	if ( numKeys <= 0 || numKeys % 2 == 1 )
	{
		return;
	}

	int clipCount = numKeys >> 1;
	std::stringstream stream;
	for ( int clipId = 0; clipId < clipCount; clipId++ )
	{
		int startNoteKeyIndex = GetAnimClipStartNoteKeyIndex( pNode, clipId );
		if ( startNoteKeyIndex == -1 )
		{
			continue;
		}

		if ( pNoteTrack->keys[startNoteKeyIndex]->note.data() == NULL ||
			!strcmp( "", pNoteTrack->keys[startNoteKeyIndex]->note.data() ))
		{
			continue;
		}

		stream.str( "" );
		stream << ANIMCLIP_END_NOTEKEY_PREFIX << pNoteTrack->keys[startNoteKeyIndex]->note.data();
		
		int endNoteKeyIndex = GetNoteKeyIndexByName( pNode, stream.str().c_str() );
		if ( endNoteKeyIndex == -1 )
		{
			continue;
		}

		if ( pNoteTrack->keys[startNoteKeyIndex]->time > pNoteTrack->keys[endNoteKeyIndex]->time )
		{
			continue;
		}

		m_animClips.push_back( AnimClip_Ticks( 
			pNoteTrack->keys[startNoteKeyIndex]->time, 
			pNoteTrack->keys[endNoteKeyIndex]->time, 
			fm::string( pNoteTrack->keys[startNoteKeyIndex]->note.data() )
			));

		m_userDefinedAnimClipsRange.SetStart( m_userDefinedAnimClipsRange.Start() > pNoteTrack->keys[startNoteKeyIndex]->time ? pNoteTrack->keys[startNoteKeyIndex]->time : m_userDefinedAnimClipsRange.Start() );
		m_userDefinedAnimClipsRange.SetEnd( m_userDefinedAnimClipsRange.End() < pNoteTrack->keys[endNoteKeyIndex]->time ? pNoteTrack->keys[endNoteKeyIndex]->time : m_userDefinedAnimClipsRange.End() );
	}
}

	//m_animClips.insert( m_animClips.begin(), AnimClip_Ticks( animRange.Start(), animRange.End(), fm::string( name ))); \

#define ADD_STD_ANIM_CLIP( animRange, name, totalAnimRange ) \
if ( ValidateAnimRange( animRange ) ) \
{ \
	m_animClips.push_back( AnimClip_Ticks( animRange.Start(), animRange.End(), name, AnimClip_Ticks::STANDARD_ANIM_CLIP )); \
	totalAnimRange.SetStart( totalAnimRange.Start() > animRange.Start() ? animRange.Start() : totalAnimRange.Start() ); \
	totalAnimRange.SetEnd( totalAnimRange.End() < animRange.End() ? animRange.End() : totalAnimRange.End() ); \
}

void AnimationClipManager::GetSceneAnimationClips( ExportNodeMap &exportNodes )
{
	// scene anim clips
	int numAnimationClips = g_animationSelector.GetNumAnimationClips();
	if ( numAnimationClips < 1 )
	{
		// backward compatibility with old animation clip storage system using notetracks
		for ( ExportNodeMap::iterator it = exportNodes.begin(); it != exportNodes.end(); ++it )
		{
			if ( it->second->exportObject )
			{
				INode *pNode = it->first;
				AddValidAnimClips( pNode );
			}
		}
	}
	else
	{
		for ( int clipId = 0; clipId < numAnimationClips; ++clipId )
		{
			m_animClips.push_back( 
				AnimClip_Ticks( 
					FramesToTicks( g_animationSelector.GetAnimationClipStart( clipId ) )
					, FramesToTicks( g_animationSelector.GetAnimationClipEnd( clipId ) )
					, g_animationSelector.GetAnimationClipName( clipId )
				));
		}
	}

	// standard anim clips
	Interval currentAnimRange = GetCOREInterface()->GetAnimRange();

	ADD_STD_ANIM_CLIP( m_userDefinedAnimClipsRange, GenerateStandardAnimClipName( "AnimClips" ), m_totalAnimRange );
	ADD_STD_ANIM_CLIP( m_sceneAnimRange, GenerateStandardAnimClipName( "SceneAnims" ), m_totalAnimRange );
	ADD_STD_ANIM_CLIP( currentAnimRange, GenerateStandardAnimClipName( "Current" ), m_totalAnimRange );
	ADD_STD_ANIM_CLIP( m_totalAnimRange, GenerateStandardAnimClipName( "Total" ), m_totalAnimRange );
}

void AnimationClipManager::CalculateExportedAnimClipsRange()
{
	RESET_ANIM_RANGE( m_exportedAnimClipsRange );
	
	int numAnimClipsToExport = m_animClipsToExport.size();
	if ( numAnimClipsToExport <= 0 )
	{
		return;
	}

	for ( int i = 0; i < numAnimClipsToExport; ++i )
	{
		int clipId = m_animClipsToExport[i];
		int animClipStart = GetAnimClipStart( clipId );
		int animClipEnd = GetAnimClipEnd( clipId );

		m_exportedAnimClipsRange.SetStart( m_exportedAnimClipsRange.Start() > animClipStart ? animClipStart : m_exportedAnimClipsRange.Start() );
		m_exportedAnimClipsRange.SetEnd( m_exportedAnimClipsRange.End() < animClipEnd ? animClipEnd : m_exportedAnimClipsRange.End() );
	}
}

fm::string AnimationClipManager::GenerateStandardAnimClipName( fm::string name )
{
	return fm::string( ANIMCLIP_STD_PREFIX ) + name + fm::string( ANIMCLIP_STD_SUFFIX );
}

template <class T>
bool operator<( const AnimClip<T> &clipA, const AnimClip<T> &clipB )
{
	FUFail( return false );
}

// Providing explicit template instantiation because implicit fails for some reason
bool operator<( const AnimClip_Secs &clipA, const AnimClip_Secs &clipB )
{
	FUFail( return false );
}

template <class T>
bool IsEquivalent( const AnimClip<T> &clipA, const AnimClip<T> &clipB )
{
	return IsEquivalent( clipA.GetName(), clipB.GetName() )
		&& IsEquivalent( clipA.GetStart(), clipB.GetStart() )
		&& IsEquivalent( clipA.GetEnd(), clipB.GetEnd() )
		&& IsEquivalent( clipA.GetFlags(), clipB.GetFlags() )
		;
}

// Providing explicit template instantiation because implicit fails for some reason
bool IsEquivalent( const AnimClip_Secs &clipA, const AnimClip_Secs &clipB )
{
	return IsEquivalent( clipA.GetName(), clipB.GetName() )
		&& IsEquivalent( clipA.GetStart(), clipB.GetStart() )
		&& IsEquivalent( clipA.GetEnd(), clipB.GetEnd() )
		&& IsEquivalent( clipA.GetFlags(), clipB.GetFlags() )
		;
}