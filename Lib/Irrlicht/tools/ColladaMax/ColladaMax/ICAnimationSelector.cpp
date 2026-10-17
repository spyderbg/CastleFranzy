#include "StdAfx.h"
#include "ICAnimationSelector.h"
#include "MAXHelpers.h"
#include "ICAppDataChunkMemManager.h"
#include "ICBinaryIO.h"

AnimationSelectorClassDesc g_animationSelectorClassDesc;
ClassDesc2* GetAnimationSelectorClassDesc()
{
	return &g_animationSelectorClassDesc;
}

ICAnimationSelector g_animationSelector;
void *AnimationSelectorClassDesc::Create(BOOL isLoading)
{
	return &g_animationSelector;
}

void ICAnimationSelector::SaveAnimationSelectorVersion()
{
	INode *pNode = GetCOREInterface()->GetRootNode();

	// precalculate buffer size
	int bufSize = 0;
	bufSize += sizeof( int );

	// allocate buffer
	char *buffer = ( char * )MAX_MALLOC( bufSize );
	char *pBuffer = buffer;

	// version
	BWriteInt( pBuffer, ICAnimationSelectorBackwardCompatibility::ASV_CURRENT_VERSION );

	// Remove Current Data Chunk
	pNode->RemoveAppDataChunk( g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_ANIMATION_SELECTOR_VERSION
		);

	// Add Data Chunk
	pNode->AddAppDataChunk( g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_ANIMATION_SELECTOR_VERSION
		, bufSize
		, buffer 
		);
}

void ICAnimationSelector::LoadAnimationSelectorVersion()
{
	INode *pNode = GetCOREInterface()->GetRootNode();

	AppDataChunk *pDataChunk = MAXHelpers::GetAppDataChunk( pNode
		, g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_ANIMATION_SELECTOR_VERSION
		, -1
		, NULL
		);

	if ( !pDataChunk )
	{
		return;
	}

	char *pBuffer = ( char * )pDataChunk->data;

	// version
	int animationSelectorVersion = 0;
	BReadInt( pBuffer, &animationSelectorVersion );
}

void ICAnimationSelector::SaveAnimationClipsVersion()
{
	INode *pNode = GetCOREInterface()->GetRootNode();

	// precalculate buffer size
	int bufSize = 0;
	bufSize += sizeof( int );

	// allocate buffer
	char *buffer = ( char * )MAX_MALLOC( bufSize );
	char *pBuffer = buffer;

	// version
	BWriteInt( pBuffer, ICAnimationSelectorBackwardCompatibility::ACV_CURRENT_VERSION );

	// Remove Current Data Chunk
	pNode->RemoveAppDataChunk( g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_ANIMATION_CLIPS_VERSION
		);

	// Add Data Chunk
	pNode->AddAppDataChunk( g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_ANIMATION_CLIPS_VERSION
		, bufSize
		, buffer 
		);
}

void ICAnimationSelector::LoadAnimationClipsVersion()
{
	INode *pNode = GetCOREInterface()->GetRootNode();

	AppDataChunk *pDataChunk = MAXHelpers::GetAppDataChunk( pNode
		, g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_ANIMATION_CLIPS_VERSION
		, -1
		, NULL
		);

	if ( !pDataChunk )
	{
		return;
	}

	char *pBuffer = ( char * )pDataChunk->data;

	// version
	int animationClipsVersion = 0;
	BReadInt( pBuffer, &animationClipsVersion );
}

void ICAnimationSelector::SaveAnimationClips()
{
	INode *pNode = GetCOREInterface()->GetRootNode();

	// precalculate buffer size
	int bufSize = 0;
	
	int numClips = GetNumAnimationClips();
	bufSize += sizeof( int );

	for ( int clipId = 0; clipId < numClips; ++clipId )
	{
		AnimClip_Frames &animationClip = GetAnimationClip( clipId );

		// clip start, clip end
		bufSize += 2 * sizeof( int );

		// clip name
		bufSize += ICBinaryIO::GetFStringSize( animationClip.GetName() );
	}

	// allocate buffer
	char *buffer = ( char * )MAX_MALLOC( bufSize );
	char *pBuffer = buffer;

	BWriteInt( pBuffer, numClips );
	for ( int clipId = 0; clipId < numClips; ++clipId )
	{
		AnimClip_Frames &animationClip = GetAnimationClip( clipId );

		// clip start
		int clipStart = animationClip.GetStart();
		BWriteInt( pBuffer, clipStart );

		// clip end
		int clipEnd = animationClip.GetEnd();
		BWriteInt( pBuffer, clipEnd );

		// clip name
		const fm::string &animClipName = animationClip.GetName();
		ICBinaryIO::WriteFString( pBuffer, animClipName );
	}

	// Remove Current Data Chunk
	pNode->RemoveAppDataChunk( g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_ANIMATION_CLIPS
		);

	// Add Data Chunk
	pNode->AddAppDataChunk( g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_ANIMATION_CLIPS
		, bufSize
		, buffer 
		);
}

void ICAnimationSelector::LoadAnimationClips()
{
	INode *pNode = GetCOREInterface()->GetRootNode();

	AppDataChunk *pDataChunk = MAXHelpers::GetAppDataChunk( pNode
		, g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_ANIMATION_CLIPS
		, -1
		, NULL
		);

	if ( !pDataChunk )
	{
		return;
	}

	char *pBuffer = ( char * )pDataChunk->data;

	int numClips = 0;
	BReadInt( pBuffer, &numClips );
	for ( int clipId = 0; clipId < numClips; ++clipId )
	{
		int clipStart;
		int clipEnd;
		fm::string clipName;
		int numEventGroups;
		
		BReadInt( pBuffer, &clipStart );
		BReadInt( pBuffer, &clipEnd );
		ICBinaryIO::ReadFString( pBuffer, clipName );

		AddAnimationClip( clipName, clipStart, clipEnd );
		AnimClip_Frames &animationClip = GetAnimationClip( clipId );
	}
}

void ICAnimationSelector::SaveEventGroupsVersion()
{
	INode *pNode = GetCOREInterface()->GetRootNode();

	// precalculate buffer size
	int bufSize = 0;
	bufSize += sizeof( int );

	// allocate buffer
	char *buffer = ( char * )MAX_MALLOC( bufSize );
	char *pBuffer = buffer;

	// version
	BWriteInt( pBuffer, ICAnimationSelectorBackwardCompatibility::EGV_CURRENT_VERSION );

	// Remove Current Data Chunk
	pNode->RemoveAppDataChunk( g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_EVENT_GROUPS_VERSION
		);

	// Add Data Chunk
	pNode->AddAppDataChunk( g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_EVENT_GROUPS_VERSION
		, bufSize
		, buffer 
		);
}

void ICAnimationSelector::LoadEventGroupsVersion()
{
	INode *pNode = GetCOREInterface()->GetRootNode();

	AppDataChunk *pDataChunk = MAXHelpers::GetAppDataChunk( pNode
		, g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_EVENT_GROUPS_VERSION
		, -1
		, NULL
		);

	if ( !pDataChunk )
	{
		return;
	}

	char *pBuffer = ( char * )pDataChunk->data;

	// version
	int eventGroupsVersion = 0;
	BReadInt( pBuffer, &eventGroupsVersion );
}

void ICAnimationSelector::SaveEventGroups()
{
	INode *pNode = GetCOREInterface()->GetRootNode();

	// precalculate buffer size
	int bufSize = 0;
	
	// event groups
	int numEventGroups = GetNumEventGroups();
	bufSize += sizeof( int );

	for ( int eventGroupId = 0; eventGroupId < numEventGroups; ++eventGroupId )
	{
		const FCDEventGroup<int> &eventGroup = GetEventGroup( eventGroupId );

		// event group time
		bufSize += sizeof( int );

		// events
		bufSize += sizeof( int );
		int numEvents = ( int )eventGroup.GetNumEvents();
		for ( int eventId = 0; eventId < numEvents; ++eventId )
		{
			bufSize += ICBinaryIO::GetFStringSize( eventGroup.GetEvent( eventId ) );
		}
	}

	// allocate buffer
	char *buffer = ( char * )MAX_MALLOC( bufSize );
	char *pBuffer = buffer;

	// event groups
	BWriteInt( pBuffer, numEventGroups );
	for ( int eventGroupId = 0; eventGroupId < numEventGroups; ++eventGroupId )
	{
		const FCDEventGroup<int> &eventGroup = GetEventGroup( eventGroupId );

		// event group time
		int eventGroupTime = eventGroup.GetTime();
		BWriteInt( pBuffer, eventGroupTime );

		// events
		int numEvents = ( int )eventGroup.GetNumEvents();
		BWriteInt( pBuffer, numEvents );
		
		for ( int eventId = 0; eventId < numEvents; ++eventId )
		{
			const fm::string &event = eventGroup.GetEvent( eventId );
			ICBinaryIO::WriteFString( pBuffer, event );
		}
	}

	// Remove Current Data Chunk
	pNode->RemoveAppDataChunk( g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_EVENT_GROUPS
		);

	// Add Data Chunk
	pNode->AddAppDataChunk( g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_EVENT_GROUPS
		, bufSize
		, buffer 
		);
}

void ICAnimationSelector::LoadEventGroups()
{
	INode *pNode = GetCOREInterface()->GetRootNode();

	AppDataChunk *pDataChunk = MAXHelpers::GetAppDataChunk( pNode
		, g_animationSelectorClassDesc.ClassID()
		, g_animationSelectorClassDesc.SuperClassID()
		, ICAnimationSelector::CID_EVENT_GROUPS
		, -1
		, NULL
		);

	if ( !pDataChunk )
	{
		return;
	}

	char *pBuffer = ( char * )pDataChunk->data;

#if EVENT_GROUP_RECOVERY_BUILD
	{
		int version = 0;
		BReadInt( pBuffer, &version );
	}
#endif // EVENT_GROUP_RECOVERY_BUILD

	// events groups
	int numEventGroups = 0;
	BReadInt( pBuffer, &numEventGroups );
	for ( int eventGroupId = 0; eventGroupId < numEventGroups; ++eventGroupId )
	{
		int eventGroupTime;
		int numEvents;

		BReadInt( pBuffer, &eventGroupTime );
		FCDEventGroup<int> eventGroup( eventGroupTime );

		BReadInt( pBuffer, &numEvents );
		for ( int eventId = 0; eventId < numEvents; ++eventId )
		{
			fstring event;
			ICBinaryIO::ReadFString( pBuffer, event );
			eventGroup.AddEvent( event );
		}

		AddEventGroup( eventGroup );
	}
}

void ICAnimationSelector::Save()
{
	SaveAnimationSelectorVersion();
	SaveAnimationClipsVersion();
	SaveAnimationClips();
	SaveEventGroupsVersion();
	SaveEventGroups();
}

void ICAnimationSelector::Load()
{
	Reset();

	LoadAnimationSelectorVersion();
	LoadAnimationClipsVersion();
	LoadAnimationClips();
	LoadEventGroupsVersion();
	LoadEventGroups();
}

void ICAnimationSelector::NotificationCallback( void *param, NotifyInfo *info )
{
	switch ( info->intcode )
	{
		case NOTIFY_SYSTEM_POST_RESET :
		case NOTIFY_SYSTEM_POST_NEW :
		{
			g_animationSelector.Reset();
			break;
		}

		case NOTIFY_FILE_POST_OPEN :
		{
			g_animationSelector.Load();
			break;
		}

		case NOTIFY_FILE_PRE_SAVE :
		{
			g_animationSelector.Save();
			break;
		}
	}
}

ICAnimationSelector::ICAnimationSelector(BOOL isLoading)
{
	Reset();

	// Register Notification Callbacks
	UnRegisterNotification( &ICAnimationSelector::NotificationCallback, NULL );
	RegisterNotification( &ICAnimationSelector::NotificationCallback, NULL, NOTIFY_SYSTEM_POST_RESET );
	RegisterNotification( &ICAnimationSelector::NotificationCallback, NULL, NOTIFY_SYSTEM_POST_NEW );
	RegisterNotification( &ICAnimationSelector::NotificationCallback, NULL, NOTIFY_FILE_POST_OPEN );
	RegisterNotification( &ICAnimationSelector::NotificationCallback, NULL, NOTIFY_FILE_PRE_SAVE );
}

ICAnimationSelector::~ICAnimationSelector()
{
}

void ICAnimationSelector::Reset()
{
	m_animationClips.clear();
	m_eventGroups.clear();
}

int ICAnimationSelector::AddAnimationClip( fm::string name, int startTime, int endTime )
{
	m_animationClips.push_back( AnimClip_Frames( startTime, endTime, name.ToLower() ) );
	return IC_OK;
}

int ICAnimationSelector::DeleteAnimationClip( size_t index )
{
	if ( index >= m_animationClips.size() )
	{
		return IC_ERROR;
	}

	m_animationClips.erase( index );
	return IC_OK;
}

size_t ICAnimationSelector::GetNumAnimationClips() const
{
	return m_animationClips.size();
}

AnimClip_Frames &ICAnimationSelector::GetAnimationClip( size_t index )
{
	return m_animationClips[index];
}

const AnimClip_Frames &ICAnimationSelector::GetAnimationClip( size_t index ) const
{
	return m_animationClips[index];
}

int ICAnimationSelector::GetAnimationClipStart( size_t index ) const
{
	if ( index >= m_animationClips.size() )
	{
		return -1;
	}

	return m_animationClips[index].GetStart();
}

int ICAnimationSelector::GetAnimationClipEnd( size_t index ) const
{
	if ( index >= m_animationClips.size() )
	{
		return -1;
	}

	return m_animationClips[index].GetEnd();
}

const fm::string &ICAnimationSelector::GetAnimationClipName( size_t index ) const
{
	FUAssert( index < m_animationClips.size(), );
	return m_animationClips[index].GetName();
}

int ICAnimationSelector::GetAnimationClipIndexByName( const fm::string clipName ) const
{
	int index = -1;

	int numClips = ( int )m_animationClips.size();
	for ( int clipId = 0; clipId < numClips; ++clipId )
	{
		if ( !_stricmp( m_animationClips[clipId].GetName().c_str(), clipName.c_str() ) )
		{
			index = clipId;
			break;
		}
	}

	return index;
}

int ICAnimationSelector::SetAnimationClipStart( size_t index, int start )
{
	if ( index >= m_animationClips.size() )
	{
		return IC_ERROR;
	}	

	m_animationClips[index].SetStart( start );
	return IC_OK;
}

int ICAnimationSelector::SetAnimationClipEnd( size_t index, int end )
{
	if ( index >= m_animationClips.size() )
	{
		return IC_ERROR;
	}

	m_animationClips[index].SetEnd( end );
	return IC_OK;
}

int ICAnimationSelector::SetAnimationClipName( size_t index, fm::string name )
{
	if ( index >= m_animationClips.size() )
	{
		return IC_ERROR;
	}

	m_animationClips[index].SetName( name.ToLower() );
	return IC_OK;
}