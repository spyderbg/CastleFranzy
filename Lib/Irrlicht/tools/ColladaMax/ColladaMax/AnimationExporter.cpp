/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the 3dsMax COLLADA Tools:
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "AnimationExporter.h"
#include "AnimationImporter.h"
#include "DocumentExporter.h"
#include "ControllerExporter.h"
#include "NodeExporter.h"
#include "ColladaOptions.h"
#include "Common/ConversionFunctions.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDAnimated.h"
#include "FCDocument/FCDAnimation.h"
#include "FCDocument/FCDAnimationClip.h"
#include "FCDocument/FCDAnimationChannel.h"
#include "FCDocument/FCDAnimationCurve.h"
#include "FCDocument/FCDAnimationKey.h"
#include "FCDocument/FCDExtra.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDTransform.h"
#include "FUtils/FUCrc32.h"
#include "ICAnimationSelector.h"
#include "ICBillboardController.h"
#include "Common/StandardMtl.h"
#include "CS/BipedAPI.h"
#include "CS/Tracks.h"
//#include "CS/IMixer.h"
//#include "CS/OurExp.h"
//
// Static Macros
//

//extern FUStringBuilder g_warningLog;
extern ICAnimationSelector g_animationSelector;

#define GET_TM_CONTROLLER_SAMPLING_INTERPOLATION( c ) \
( \
	ControllerExporter::FindConstraint( c ) \
		? FUDaeInterpolation::LINEAR \
		: GetAnimatableSamplingInterpolation( c, FUDaeInterpolation::STEP, FUDaeInterpolation::LINEAR ) \
)

#define FLOATWAVE_CONTROL_CLASS_ID	0x930Abc78

#define VALIDATE_ANIM_CLIPS( clips, statementIfInvalid ) \
if ( clips.begin() == clips.end() ) \
{ \
	statementIfInvalid; \
}

#define ADD_ANIMATION_KEY_PTR( __keyPtr, __keyBuffer, __interpolation, __input, __output ) \
			{ \
				__keyPtr = FCDAnimationCurve::CreateKey( __interpolation ); \
				__keyPtr->input = __input; \
				__keyPtr->output = __output; \
				__keyBuffer.push_back( __keyPtr ); \
			}

#define ADD_ANIMATION_KEY( __keyBuffer, __interpolation, __input, __output ) \
			{ \
				FCDAnimationKey* __pKey; \
				ADD_ANIMATION_KEY_PTR( __pKey, __keyBuffer, __interpolation, __input, __output ); \
			}

#define VALIDATE_ANIM_KEY_OPTIMIZATION( bCompressKeys, bOneChannelPerCurve, curveInterpolation, bIsSampled ) \
( (bCompressKeys) && (bOneChannelPerCurve) && ( (curveInterpolation) == FUDaeInterpolation::LINEAR || (curveInterpolation) == FUDaeInterpolation::STEP ) && bIsSampled )

#define IS_FORCE_ANIM_EXPORT( pController )( OPTS->ExportDegenerateAnimationCurves() && ControllerExporter::IsValidAnimationController( pController ) )

#define TRANSLATION_ANIM 0
#define ROTATION_ANIM 1
#define SCALE_ANIM 2
#define NUM_ANIMATIONS 3
#define EULER_ROTATION_ANGLE_ID 3
#define TRANSLATION_NUM_CURVES 3
#define SCALE_NUM_CURVES 3
#define EULER_ROT_NUM_CURVES 3
#define ANGLE_AXIS_ROT_NUM_CURVES 4 // used for QUATERNION as well
#define IS_EULER_ROT_ANIM( rotationFormat, animId )( (rotationFormat) == ColladaOptions::EULER_XYZ && (animId) == ROTATION_ANIM )
#define IS_TRANSFORM_ANIM_ONE_CHANNEL_PER_CURVE( bOneChannelPerCurve, rotationFormat, bIsRotation )( (bOneChannelPerCurve) || IS_EULER_ROT_ANIM( rotationFormat, bIsRotation ) )

#define GET_NODE_TM( nodeTM, tmpTM, parentNode, t ) \
{ \
	nodeTM = node->GetNodeTM(t); \
	\
	if ( bIsWSMPivot ) \
	{ \
		tmpTM = node->GetObjTMAfterWSM(t); \
		nodeTM.Invert(); \
		nodeTM = tmpTM * nodeTM; \
	} \
	else if ( parentNode != NULL) \
	{ \
		/* export a relative TM */  \
		/* We have to use whatever value we exported for this parent */ \
		/* in order to remain consistent in collada */ \
		tmpTM = parentNode->GetNodeTM(t); \
		tmpTM.Invert(); \
		nodeTM = nodeTM * tmpTM; \
	} \
}

#define GET_ROTATION_FROM_AFFINE_PARTS( rotation, affineParts, rotationFormat ) \
{ \
	if ( rotationFormat == ColladaOptions::EULER_XYZ ) \
	{ \
		Point3 angles; \
		QuatToEuler(affineParts.q, ( float * )&angles, EULERTYPE_XYZ); \
		 \
		for ( int el = 0; el < 3; el++ ) \
		{ \
			angles[el] = FMath::RadToDeg(angles[el]); \
		} \
		 \
		fm::swap( angles[0], angles[2] ); \
		 \
		rotation = Point4( angles ); \
	} \
	else if ( rotationFormat == ColladaOptions::ANGLE_AXIS ) \
	{ \
		Point3 axis; \
		float angle; \
		AngAxisFromQ( affineParts.q, &angle, axis ); \
		angle = -FMath::RadToDeg(angle); \
		 \
		rotation = Point4( axis[0], axis[1], axis[2], angle ); \
	} \
	else \
	{ \
		rotation = Point4( ( float * )&affineParts.q ); \
	} \
}

#define DECOMPOSED_ANIMATED_TRANSFORM_PRE_EXPORT \
	ClipList clips = GetAnimationClips(NULL); \
	VALIDATE_ANIM_CLIPS( clips, return ); \
	Clip** it = clips.begin(); \
	FCDAnimationCurveTrackList addedCurves; \
	 \
	int rotationFormat = OPTS->RotationFormat(); \
	int rotationNumCurves = rotationFormat != ColladaOptions::EULER_XYZ ? ANGLE_AXIS_ROT_NUM_CURVES : EULER_ROT_NUM_CURVES; \
	 \
	fm::string animNames[NUM_ANIMATIONS]; \
	animNames[TRANSLATION_ANIM] = id + "-translation"; \
	animNames[ROTATION_ANIM] = id + "-rotation"; \
	animNames[SCALE_ANIM] = id + "-scale"; \
	 \
	FCDAnimation *animations[NUM_ANIMATIONS]; \
	 \
	/* Create the needed curves */  \
	FCDAnimationCurveList curves[NUM_ANIMATIONS]; \
	int numCurves[NUM_ANIMATIONS]; \
	numCurves[TRANSLATION_ANIM] = TRANSLATION_NUM_CURVES; \
	numCurves[ROTATION_ANIM] = rotationNumCurves; \
	numCurves[SCALE_ANIM] = SCALE_NUM_CURVES; \
	 \
	/* key buffers */ \
	FCDAnimationKeyList *keyBuffers[NUM_ANIMATIONS]; \
	for ( int animId = 0; animId < NUM_ANIMATIONS; ++animId ) \
	{ \
		int curveCount = numCurves[animId]; \
		 \
		keyBuffers[animId] = new FCDAnimationKeyList[curveCount]; \
		for ( int curveId = 0; curveId < curveCount; ++curveId ) \
		{ \
			keyBuffers[animId][curveId].reserve( 1024 ); \
		} \
	} \
	 \
	fm::vector<fm::pvector<FCDAnimated>> animateds; \
	animateds.resize( NUM_ANIMATIONS ); \
	animateds[TRANSLATION_ANIM].push_back( tColladaValue.GetAnimated() ); \
	animateds[SCALE_ANIM].push_back( kColladaValue.GetAnimated() ); \
	for ( size_t curveId = 0; curveId < rotations.size(); ++curveId ) \
	{ \
		animateds[ROTATION_ANIM].push_back( rotations[curveId]->GetAngleAxis().GetAnimated() ); \
	} \
	 \
	FCDAnimationChannel *channel = NULL; \
	for ( int animId = 0; animId < NUM_ANIMATIONS; animId++ ) \
	{ \
		animations[animId] = CreateAnimation((*it), animNames[animId].c_str() ); \
		curves[animId].reserve( numCurves[animId] ); \
		if ( !IS_TRANSFORM_ANIM_ONE_CHANNEL_PER_CURVE( bOneChannelPerCurve, rotationFormat, animId == ROTATION_ANIM ) ) \
		{ \
			channel = animations[animId]->AddChannel(); \
		} \
		for ( int curveId = 0; curveId < numCurves[animId]; curveId++ ) \
		{ \
			if ( IS_TRANSFORM_ANIM_ONE_CHANNEL_PER_CURVE( bOneChannelPerCurve, rotationFormat, animId == ROTATION_ANIM ) ) \
			{ \
				channel = animations[animId]->AddChannel(); \
			} \
			 \
			FCDAnimationCurve* curve = CreateCurve(channel); \
			 \
			if ( IS_EULER_ROT_ANIM( rotationFormat, animId ) ) \
			{ \
				animateds[animId][curveId]->AddCurve( EULER_ROTATION_ANGLE_ID, curve); \
			} \
			else \
			{ \
				animateds[animId][0]->AddCurve( curveId, curve); \
			} \
			 \
			curves[animId].push_back(curve); \
			if ((*it)->colladaClip != NULL) (*it)->colladaClip->AddClipCurve(curve); \
			addedCurves.push_back( curve );	 \
		} \
	} \
	 \
	{ \
		/* TODO : case when rotations.size() > 1 */ \
		animations[TRANSLATION_ANIM]->SetDefaultAnimated( tColladaValue.GetAnimated() ); \
		animations[ROTATION_ANIM]->SetDefaultAnimated( rotations[0]->GetAngleAxis().GetAnimated() ); \
		animations[SCALE_ANIM]->SetDefaultAnimated( kColladaValue.GetAnimated() ); \
	} \
	 \
	AddCurvesToClips( addedCurves, it + 1, clips.end() ); \
	\
	/* Actual sampling */ \
	Matrix3 nodeTM, tmpTM; \
	INode* parentNode = NULL; \
	 \
	if ( !bIsWSMPivot ) \
	{ \
		parentNode = node->GetParentNode(); \
		if (parentNode != NULL && parentNode->IsRootNode()) parentNode = NULL; \
	}

#define DECOMPOSED_ANIMATED_TRANSFORM_CLEANUP \
	/* Delete key buffers */ \
	for ( int animId = 0; animId < NUM_ANIMATIONS; ++animId ) \
	{ \
		SAFE_DELETE_ARRAY( keyBuffers[animId] ); \
	} \
	 \
	/* Delete empty animations */ \
	for ( size_t animId = 0; animId < animateds.size(); ++animId ) \
	{ \
		bool bValidAnimation = false; \
		for ( size_t animatedId = 0; animatedId < animateds[animId].size(); ++animatedId ) \
		{ \
			bool bValidAnimated = animateds[animId][animatedId]->HasCurve(); \
			bValidAnimation |= bValidAnimated; \
			 \
			if (!bValidAnimated) \
			{ \
				SAFE_RELEASE(animateds[animId][animatedId]); \
			} \
		} \
		 \
		if ( !bValidAnimation ) \
		{ \
			SAFE_RELEASE(animations[animId]); \
		} \
	}

static const char *k_eulerRotationSubIds [] = { "rotation_z", "rotation_y", "rotation_x" };

inline FUDaeInfinity::Infinity ConvertORT(int ort)
{
	switch (ort)
	{
		case ORT_CONSTANT: return FUDaeInfinity::CONSTANT;
		case ORT_CYCLE: return FUDaeInfinity::CYCLE;
		case ORT_LOOP: return FUDaeInfinity::CYCLE;
		case ORT_OSCILLATE: return FUDaeInfinity::OSCILLATE;
		case ORT_LINEAR: return FUDaeInfinity::LINEAR;
		case ORT_RELATIVE_REPEAT: return FUDaeInfinity::CYCLE_RELATIVE;
		default: return FUDaeInfinity::CONSTANT;
	}
}

template <class T>
FUDaeInterpolation::Interpolation GetBezierKeyInterpolation( T &k, FUDaeInterpolation::Interpolation defaultInterp )
{
	FUDaeInterpolation::Interpolation interp = defaultInterp;
	
	// may need to cover these types at some point : BEZKEY_SMOOTH, BEZKEY_FAST, BEZKEY_SLOW, BEZKEY_USER, BEZKEY_FLAT
	if ( GetOutTanType(k.flags) == BEZKEY_STEP )
	{
		interp = FUDaeInterpolation::STEP;
	}
	else if ( GetOutTanType(k.flags) == BEZKEY_LINEAR )
	{
		interp = FUDaeInterpolation::LINEAR;
	}
	
	return interp;
}

// TODO : Should I use the ticks per frame corresponding to the animation sampling rate or the scene's current frame rate?
template <class T, class F, class L>
inline FUDaeInterpolation::Interpolation PatchBezierKey(T& k, const F& UNUSED(flat), const L& length, TimeValue previousTime, TimeValue nextTime, FUDaeInterpolation::Interpolation defaultInterp = FUDaeInterpolation::BEZIER)
{
	if (k.flags & BEZKEY_UNCONSTRAINHANDLE) 
	{
		k.inLength *= GetTicksPerFrame() / (float) (k.time - previousTime); 
		k.outLength *= GetTicksPerFrame() / (float) (nextTime - k.time); 
	}
	if (GetInTanType(k.flags) != BEZKEY_USER) k.inLength = length;
	if (GetOutTanType(k.flags) != BEZKEY_USER) k.outLength = length;
	return defaultInterp;
}

void GetControlInterpolationMap( Animatable *pAnimatable, FUDaeInterpolation::Interpolation defaultInterp, InterpolationMap &interps )
{
	IKeyControl *pKeyControl = pAnimatable ? GetKeyControlInterface( pAnimatable ) : NULL;
	int numKeys = pKeyControl ? pKeyControl->GetNumKeys() : 0;
	int keySize = pKeyControl ? pKeyControl->GetKeySize() : 0;
	for ( int keyId = 0; keyId < numKeys; ++keyId )
	{
		Class_ID classId = pAnimatable->ClassID();
		ULONG partA = classId.PartA();
		if ( partA == HYBRIDINTERP_FLOAT_CLASS_ID
			|| partA == HYBRIDINTERP_POSITION_CLASS_ID
			|| partA == HYBRIDINTERP_ROTATION_CLASS_ID
			|| partA == HYBRIDINTERP_POINT3_CLASS_ID
			|| partA == HYBRIDINTERP_SCALE_CLASS_ID
			|| partA == HYBRIDINTERP_COLOR_CLASS_ID
			|| partA == HYBRIDINTERP_POINT4_CLASS_ID
			|| partA == HYBRIDINTERP_FRGBA_CLASS_ID
			)
		{
			void *p = malloc( keySize );
			IKey *pKey = ( IKey * )p;

			pKeyControl->GetKey( keyId, pKey );
			interps.insert( GetBezierKeyInterpolation( *pKey, defaultInterp ), false );
			free( p );
		}
	}
}

FUDaeInterpolation::Interpolation GetControlInterpolation( Animatable *pAnimatable, FUDaeInterpolation::Interpolation defaultInterp )
{
	InterpolationMap interps;
	GetControlInterpolationMap( pAnimatable, defaultInterp, interps );
	
	if ( interps.size() <= 0 )
	{
		return defaultInterp;
	}
	
	static fm::vector<FUDaeInterpolation::Interpolation> interpolationPriorityList;
	if ( interpolationPriorityList.empty() )
	{
		interpolationPriorityList.push_back( FUDaeInterpolation::STEP );
		interpolationPriorityList.push_back( FUDaeInterpolation::LINEAR );
		interpolationPriorityList.push_back( FUDaeInterpolation::BEZIER );
		interpolationPriorityList.push_back( FUDaeInterpolation::HERMITE );
	}

	int interpPriority = 0;
	for ( InterpolationMap::iterator interpIter = interps.begin(); interpIter != interps.end(); ++interpIter )
	{
		FUDaeInterpolation::Interpolation interp = interpIter->first;
		int currentPriority = interpolationPriorityList.find_index( interp );
		//FUAssert( currentPriority != -1, continue );
		interpPriority = std::max( interpPriority, currentPriority );
	}

	return *interpolationPriorityList.find( interpPriority );
}

void GetAnimatableValidControllers( Animatable *pAnimatable, fm::pvector<Animatable> &controllers )
{
	if ( !pAnimatable )
	{
		return;
	}

	IKeyControl *pKeyInterface = NULL;
	int numKeys = ControllerExporter::GetNumControllerKeys( pAnimatable, pKeyInterface );

	if ( numKeys > 0 )
	{
		controllers.push_back( pAnimatable );
	}

	int nSubs = pAnimatable->NumSubs();
	for (int i = 0; i < nSubs; i++)
	{
		Animatable* sub = pAnimatable->SubAnim(i);
		GetAnimatableValidControllers( sub, controllers );
	}
}

FUDaeInterpolation::Interpolation GetAnimatableSamplingInterpolation( Animatable *pAnimatable, FUDaeInterpolation::Interpolation preferredInterp, FUDaeInterpolation::Interpolation fallbackInterp )
{
	fm::pvector<Animatable> controllers;
	GetAnimatableValidControllers( pAnimatable, controllers );

	for ( fm::pvector<Animatable>::iterator ctrlIter = controllers.begin(); ctrlIter != controllers.end(); ++ctrlIter )
	{
		FUDaeInterpolation::Interpolation interp = GetControlInterpolation( *ctrlIter, fallbackInterp );
		if ( interp != preferredInterp )
		{
			return fallbackInterp;
		}
	}

	// cannot seem to get controllers ( or keys ) for animated .BIPs
	if ( controllers.size() <= 0 )
	{
		return fallbackInterp;
	}

	return preferredInterp;
}

AnimationExporter::Clip::Clip()
:	colladaClip(NULL), colladaContainer(NULL), controller(NULL)
{
}

//
// AnimationExporter
//

AnimationExporter::AnimationExporter(DocumentExporter* _document)
:	BaseExporter(_document)
,	isRelativeAnimation(false), controlType(ColladaMaxAnimatable::ROTATION_X)
{
	for (size_t i = 0; i < maxInitialValues; ++i) initialValue[i] = 0.0f;
}

AnimationExporter::~AnimationExporter()
{
	clips.clear();
}

void AnimationExporter::FinalizeExport()
{
	for (ClipMap::iterator it = clips.begin(); it != clips.end(); ++it)
	{
		if (it->second.colladaClip != NULL)
		{
			// If the clip is empty, remove it because empty clips are not valid COLLADA.
			if (it->second.colladaClip->GetClipCurves().empty())
			{
				SAFE_RELEASE(it->second.colladaClip);
				SAFE_RELEASE(it->second.colladaContainer);
			}
		}
	}
}

AnimationExporter::ClipList AnimationExporter::GetAnimationClips(Control* controller)
{
	int numAnimClipsToExport = AnimationClipManager::GetInst()->GetNumAnimClipsToExport();

	// Create the global clip, if it does not yet exists.
	if (clips.empty())
	{
		clips.insert(0, Clip());

		Interval animRange;
		for ( int i = 1; i < numAnimClipsToExport; i++ )
		{
			clips.insert(i, Clip());
		}

		for ( int i = 0; i < numAnimClipsToExport; i++ )
		{
			ClipMap::iterator it = clips.find( i );
			it->second.colladaClip = CDOC->GetAnimationClipLibrary()->AddEntity();

			int clipIndex = AnimationClipManager::GetInst()->GetAnimClipsToExport()[i];
			animRange.Set( AnimationClipManager::GetInst()->GetAnimClipStart( clipIndex ), AnimationClipManager::GetInst()->GetAnimClipEnd( clipIndex ) );
			it->second.colladaClip->SetDaeId( AnimationClipManager::GetInst()->GetAnimClipName( clipIndex ) );

			it->second.colladaClip->SetStart( FSConvert::MaxTickToSecs( animRange.Start() ) );
			it->second.colladaClip->SetEnd( FSConvert::MaxTickToSecs( animRange.End() ) );
		}
	}

	ClipList out;

#if MAX_VERSION_MAJOR >= 9
	Class_ID cid = (controller != NULL) ? controller->ClassID() : Class_ID(0, 0);
	if (cid == FLOATLAYER_CONTROL_CLASS_ID || cid == POINT3LAYER_CONTROL_CLASS_ID || cid == POSLAYER_CONTROL_CLASS_ID || cid == ROTLAYER_CONTROL_CLASS_ID || cid == SCALELAYER_CONTROL_CLASS_ID || cid == POINT4LAYER_CONTROL_CLASS_ID || cid == LAYEROUTPUT_CONTROL_CLASS_ID)
	{
		// Retrieve the animation layer controller interface in order to extract the animation clip information.
		ILayerControl* layerController = GetILayerControlInterface(controller);
		FUAssert(layerController != NULL, return out);
		int layerCount = layerController->GetLayerCount();
		for (int i = 0; i < layerCount; ++i)
		{
			TSTR name = layerController->GetLayerName(i);
			uint32 crc = FUCrc32::CRC32(name.data());
			ClipMap::iterator it = clips.find(crc);
			if (it == clips.end())
			{
				// This is a new animation layer.
				it = clips.insert(crc, Clip());
				it->second.colladaClip = CDOC->GetAnimationClipLibrary()->AddEntity();
				it->second.colladaClip->SetStart( FSConvert::MaxTickToSecs( OPTS->AnimStart() ) );
				it->second.colladaClip->SetEnd( FSConvert::MaxTickToSecs( OPTS->AnimEnd() ) );
				it->second.colladaClip->SetDaeId(name.data());
				it->second.colladaContainer = CDOC->GetAnimationLibrary()->AddEntity();
				it->second.colladaContainer->SetDaeId(fm::string(name.data()) + "-clip");
			}
			it->second.controller = layerController->GetSubCtrl(i);
			if (it->second.controller != NULL)
			{
				// Add this sub-controller to the list of controllers to attempt to export.
				out.push_back(&it->second);
			}
		}
	}
	else
#endif // Max 9.0+
	{
		for ( int i = 0; i < ( numAnimClipsToExport > 0 ? numAnimClipsToExport : 1 ); i++ )
		{
			// Attempt to export this standard controller.
			Clip* baseClip = &clips.find(i)->second;
			baseClip->controller = controller;
			out.push_back(baseClip);
		}
	}
	return out;
}

FCDAnimation* AnimationExporter::CreateAnimation(Clip* clip, const fchar* id)
{
	FCDAnimation* animation;
	if (clip == NULL || clip->colladaContainer == NULL)
	{
		animation = CDOC->GetAnimationLibrary()->AddEntity();
	}
	else
	{
		animation = clip->colladaContainer->AddChild();
	}
	animation->SetDaeId(TO_STRING(id));
	return animation;
}

FCDAnimationCurve* AnimationExporter::CreateCurve(FCDAnimationChannel* colladaChannel, Control* controller)
{
	FCDAnimationCurve* colladaCurve = colladaChannel->AddCurve();
	if (controller != NULL)
	{
		colladaCurve->SetPreInfinity(ConvertORT(controller->GetORT(ORT_BEFORE)));
		colladaCurve->SetPostInfinity(ConvertORT(controller->GetORT(ORT_AFTER)));
	}
	return colladaCurve;
}

FCDAnimated* AnimationExporter::ExportAnimatedPoint3(const fm::string& id, Control* controller, FCDParameterAnimatableVector3& colladaValue, int UNUSED(arrayIndex), FCDConversionFunctor* conversion, bool forceSampling, int baseType )
{
	if (!OPTS->ExportAnimations()) return NULL;
	FCDAnimated* animated = colladaValue.GetAnimated();
	ExportAnimatedPoint3(id, controller, animated, conversion, forceSampling, baseType);
	if (!animated->HasCurve()) SAFE_RELEASE(animated);
	return animated;
}

void AnimationExporter::ExportAnimatedPoint3(const fm::string& id, Control* controller, FCDAnimated* animated, FCDConversionFunctor* conversion, bool forceSampling, int baseType )
{
	if (controller == NULL || animated == NULL || animated->GetValueCount() < 3) return; 

	// Create the animation clip iterator.
	ClipList clips = GetAnimationClips(controller);
	VALIDATE_ANIM_CLIPS( clips, return );
	Clip** it = clips.begin();
	FCDAnimationCurveTrackList addedCurves;

	controller = (*it)->controller;
	FCDAnimation* animation = CreateAnimation(*it, id.c_str());

#define NUM_CURVES 3

	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	int numChannels = bOneChannelPerCurve ? NUM_CURVES : 1 ;

	// Hack for animation on material color + maybe other types of anims
	// Have to fake a 4th channel
	if ( bOneChannelPerCurve && 
		( baseType == TYPE_POINT3 || baseType == TYPE_RGBA ))
	{
		++numChannels;
	}

	FCDAnimationChannel **channels = new FCDAnimationChannel*[numChannels];
	for ( int i = 0; i < numChannels; ++i )
	{
		channels[i] =  animation->AddChannel();
	}
	//FCDAnimationChannel* channel = animation->AddChannel();

	// Relative animation support
	if (isRelativeAnimation)
	{
		SClass_ID ctrlSuperId = controller->SuperClassID();
		Point3 v;
		if (ctrlSuperId == CTRL_POINT3_CLASS_ID || ctrlSuperId == CTRL_POSITION_CLASS_ID) 
		{
			// Get our initial value
			controller->GetValue(OPTS->AnimStart(), &v, FOREVER, CTRL_ABSOLUTE);
		}
		else if (ctrlSuperId == CTRL_SCALE_CLASS_ID)
		{
			// For COLLADA, a scale is just another P3
			ScaleValue sv;	// A scale value holds a scale and orientation
			controller->GetValue(OPTS->AnimStart(), &sv, FOREVER, CTRL_ABSOLUTE);
			v = sv.s;		// Keep the scale, we dont care about the orientation.
		}
		else
		{
			FUFail(return);
		}
		
		initialValue[0] = v[0];
		initialValue[1] = v[1];
		initialValue[2] = v[2];
	}

	Control* subControllers[3] = { controller->GetXController(), controller->GetYController(), controller->GetZController() };

	// First, Try to extract animations from the component controllers
	bool animationValid = false;
	if (subControllers[0] != NULL && subControllers[1] != NULL && subControllers[2] != NULL)
	{
		for (size_t i = 0; i < NUM_CURVES; ++i)
		{
			FCDAnimationChannel *channel = bOneChannelPerCurve ? channels[i] : channels[0];
			FCDAnimationCurve* curve = CreateCurve(channel, subControllers[i]);
			ExportCurve(subControllers[i], curve, animated, conversion, forceSampling);
			if (curve != NULL)
			{
				animated->AddCurve(i, curve);
				if ((*it)->colladaClip != NULL) (*it)->colladaClip->AddClipCurve(curve);
				animationValid = true;
				addedCurves.push_back( curve );
			}
		}
	}
	else 
	{
		// Else, with no subs, try and export ourselves as keyframes
		animationValid = ExportAnimation(*it, ColladaMaxAnimatable::FLOAT3, animated, channels, addedCurves, conversion, forceSampling);
	}

	if (!animationValid) SAFE_RELEASE(animation);

	AddCurvesToClips( addedCurves, it + 1, clips.end() );

	SAFE_DELETE_ARRAY( channels );

#undef NUM_CURVES
}

void AnimationExporter::ExportAnimatedPoint3(const fm::string& id, Control* controllerX, Control* controllerY, Control* controllerZ, FCDAnimated* animated, FCDConversionFunctor* conversion, bool forceSampling, int baseType )
{
	if (animated == NULL || animated->GetValueCount() < 3) return; 

	// Create the animation clip iterator.
	ClipList clips = GetAnimationClips(NULL);
	VALIDATE_ANIM_CLIPS( clips, return );
	Clip** it = clips.begin();
	FCDAnimationCurveTrackList addedCurves;

	FCDAnimation* animation = CreateAnimation(*it, id.c_str());

#define NUM_CURVES 3

	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	int numChannels = bOneChannelPerCurve ? NUM_CURVES : 1 ;

	FCDAnimationChannel **channels = new FCDAnimationChannel*[numChannels];
	for ( int i = 0; i < numChannels; ++i )
	{
		channels[i] =  animation->AddChannel();
	}

	Control* subControllers[3] = { controllerX, controllerY, controllerZ };

	// Relative animation support
	for ( int controllerId = 0; controllerId < 3; ++ controllerId )
	{
		Control *pController = subControllers[controllerId];
		if ( isRelativeAnimation && pController )
		{
			SClass_ID ctrlSuperId = pController->SuperClassID();
			if (ctrlSuperId == CTRL_FLOAT_CLASS_ID)
			{
				pController->GetValue(OPTS->AnimStart(), &initialValue[controllerId], FOREVER, CTRL_ABSOLUTE);
			}
			else
			{
				FUFail(return);
			}
		}
	}

	// First, Try to extract animations from the component controllers
	bool animationValid = false;
	for (size_t i = 0; i < NUM_CURVES; ++i)
	{
		FCDAnimationChannel *channel = bOneChannelPerCurve ? channels[i] : channels[0];
		FCDAnimationCurve* curve = CreateCurve(channel, subControllers[i]);
		ExportCurve(subControllers[i], curve, animated, conversion, forceSampling);
		if (curve != NULL)
		{
			animated->AddCurve(i, curve);
			if ((*it)->colladaClip != NULL) (*it)->colladaClip->AddClipCurve(curve);
			animationValid = true;
			addedCurves.push_back( curve );
		}
	}

	if (!animationValid) SAFE_RELEASE(animation);

	AddCurvesToClips( addedCurves, it + 1, clips.end() );

	SAFE_DELETE_ARRAY( channels );

#undef NUM_CURVES
}

FCDAnimated* AnimationExporter::ExportAnimatedPoint4(const fm::string& id, Control* controller, FCDParameterAnimatableVector4& colladaValue, int UNUSED(arrayIndex), FCDConversionFunctor* conversion, bool forceSampling)
{
	if (!OPTS->ExportAnimations()) return NULL;
	FCDAnimated* animated = colladaValue.GetAnimated();
	ExportAnimatedPoint4(id, controller, animated, conversion, forceSampling);
	if (!animated->HasCurve()) SAFE_RELEASE(animated);
	return animated;
}

void AnimationExporter::ExportAnimatedPoint4(const fm::string& id, Control* controller, FCDAnimated* animated, FCDConversionFunctor* conversion, bool forceSampling)
{
	if (!OPTS->ExportAnimations() || controller == NULL || animated == NULL || animated->GetValueCount() < 4)
		return;
	
	SClass_ID superId = controller->SuperClassID();
	FUAssert(superId == CTRL_POINT4_CLASS_ID, return);

	// treat it as a RGBA color
	ClipList clips = GetAnimationClips(controller);
	VALIDATE_ANIM_CLIPS( clips, return );
	Clip** it = clips.begin();
	FCDAnimationCurveTrackList addedCurves;

	controller = (*it)->controller;
	FCDAnimation* animation = CreateAnimation(*it, id.c_str());

#define NUM_CURVES 4

	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	int numChannels = bOneChannelPerCurve ? NUM_CURVES : 1 ;

	FCDAnimationChannel **channels = new FCDAnimationChannel*[numChannels];
	for ( int i = 0; i < numChannels; ++i )
	{
		channels[i] =  animation->AddChannel();
	}
	//FCDAnimationChannel* channel = animation->AddChannel();

	Point4 v;
	controller->GetValue(OPTS->AnimStart(), &v, FOREVER, CTRL_ABSOLUTE);

	initialValue[0] = v.x;
	initialValue[1] = v.y;
	initialValue[2] = v.z;
	initialValue[3] = v.w;

	// Direct controller not keyed. Try to extract animations from the component controllers
	bool animationValid = false;
	Control* subControllers[NUM_CURVES] = { controller->GetXController(), controller->GetYController(), controller->GetZController(), controller->GetWController() };
	if (subControllers[0] != NULL && subControllers[1] != NULL && subControllers[2] != NULL && subControllers[3] != NULL)
	{	
		for (int i = 0; i < NUM_CURVES; ++i)
		{
			FCDAnimationChannel *channel = bOneChannelPerCurve ? channels[i] : channels[0];
			FCDAnimationCurve* curve = CreateCurve(channel, subControllers[i]);
			ExportCurve(subControllers[i], curve, animated, conversion, forceSampling);
			if (curve != NULL)
			{
				animated->AddCurve(i, curve);
				if ((*it)->colladaClip != NULL) (*it)->colladaClip->AddClipCurve(curve);
				animationValid = true;
				addedCurves.push_back( curve );
			}
		}
	}
	else
	{
		animationValid = ExportAnimation(*it, ColladaMaxAnimatable::FLOAT4, animated, channels, addedCurves, conversion, forceSampling);
	}

	if (!animationValid)
	{
		SAFE_RELEASE(animation);
	}

	AddCurvesToClips( addedCurves, it + 1, clips.end() );

	SAFE_DELETE_ARRAY( channels );

#undef NUM_CURVES
}

FCDAnimated* AnimationExporter::ExportAnimatedAngle(const fm::string& id, Control* controller, ColladaMaxAnimatable::Type type, FCDParameterAnimatableAngleAxis& colladaValue, FCDConversionFunctor* conversion, bool forceSampling)
{
	if (!OPTS->ExportAnimations() || controller == NULL) return NULL;

	// maybe this rotation controller doesn't have XYZ components
	Control* xyzController = NULL;

	if (type == ColladaMaxAnimatable::ROTATION_X) xyzController = controller->GetXController();
	else if (type == ColladaMaxAnimatable::ROTATION_Y) xyzController = controller->GetYController();
	else if (type == ColladaMaxAnimatable::ROTATION_Z) xyzController = controller->GetZController();
	if (xyzController != NULL) controller = xyzController;

	FCDAnimated* animated = colladaValue.GetAnimated();
	ClipList clips = GetAnimationClips(controller);
	VALIDATE_ANIM_CLIPS( clips, return NULL );
	Clip** it = clips.begin();
	FCDAnimationCurveTrackList addedCurves;
	
	controller = (*it)->controller;
	FCDAnimation* animation = CreateAnimation(*it, id.c_str());
	FCDAnimationChannel* channel = animation->AddChannel();
	FCDAnimationCurve* curve = CreateCurve(channel, controller);

	// set the type parameter
	controlType = type;
	ExportCurve(controller, curve, animated, conversion, forceSampling);
	if (curve != NULL)
	{
		animated->AddCurve(3, curve);
		if ((*it)->colladaClip != NULL) (*it)->colladaClip->AddClipCurve(curve);
		addedCurves.push_back( curve );
	}
	else
	{
		SAFE_RELEASE(animation);
	}

	AddCurvesToClips( addedCurves, it + 1, clips.end() );

	if (!animated->HasCurve()) SAFE_RELEASE(animated);
	return animated;
}

void AnimationExporter::ExportAnimatedFloat(const fm::string& id, Control* controller, FCDAnimated* animated, FCDConversionFunctor* conversion, bool forceSampling, int addParamA, int addParamB )
{
	if (!OPTS->ExportAnimations() || controller == NULL || animated == NULL || animated->GetValueCount() < 1)
		return;

	// Relative animation support
	if (isRelativeAnimation)
	{
		SClass_ID ctrlSuperId = controller->SuperClassID();
		if (ctrlSuperId == CTRL_POINT3_CLASS_ID || ctrlSuperId == CTRL_POSITION_CLASS_ID) 
		{
			// Get our initial value
			Point3 v;
			controller->GetValue(OPTS->AnimStart(), &v, FOREVER, CTRL_ABSOLUTE);
			initialValue[0] = v[controlType - ColladaMaxAnimatable::ROTATION_X];
		}
		else if (ctrlSuperId == CTRL_SCALE_CLASS_ID)
		{
			// For COLLADA, a scale is just another P3
			ScaleValue sv;	// A scale value holds a scale and orientation
			controller->GetValue(OPTS->AnimStart(), &sv, FOREVER, CTRL_ABSOLUTE);
			initialValue[0] = sv.s[controlType - ColladaMaxAnimatable::ROTATION_X];
		}
		else if (ctrlSuperId == CTRL_FLOAT_CLASS_ID)
		{
			controller->GetValue(OPTS->AnimStart(), initialValue, FOREVER, CTRL_ABSOLUTE);
		}
		else
		{
			FUFail(return);
		}
	}

	// Now, write out the curves.
	ClipList clips = GetAnimationClips(controller);
	VALIDATE_ANIM_CLIPS( clips, return );
	Clip** it = clips.begin();
	FCDAnimationCurveTrackList addedCurves;
	
	controller = (*it)->controller;
	FCDAnimation* animation = CreateAnimation(*it, id.c_str());
	FCDAnimationChannel* channel = animation->AddChannel();
	FCDAnimationCurve* curve = CreateCurve(channel, controller);

	ExportCurve(controller, curve, animated, conversion, forceSampling, addParamA, addParamB );
	if (curve != NULL)
	{
		animated->AddCurve(0, curve);
		if ((*it)->colladaClip != NULL) (*it)->colladaClip->AddClipCurve(curve);
		addedCurves.push_back( curve );
	}
	else
	{
		SAFE_RELEASE(animation);
	}

	AddCurvesToClips( addedCurves, it + 1, clips.end() );
}

FCDAnimated* AnimationExporter::ExportAnimatedFloat(const fm::string& id, Control* controller, FCDParameterAnimatableFloat& colladaValue, int UNUSED(arrayIndex), FCDConversionFunctor* conversion, bool forceSampling, int addParamA, int addParamB )
{
	if (!OPTS->ExportAnimations()) return NULL;
	FCDAnimated* animated = colladaValue.GetAnimated();
	ExportAnimatedFloat(id, controller, animated, conversion, forceSampling, addParamA, addParamB );
	if (!animated->HasCurve()) SAFE_RELEASE(animated);
	return animated;
}

void AnimationExporter::ExportAnimatedEulerAngles(const fm::string& id, Control* controller, fm::pvector<FCDTRotation> rotations, FCDConversionFunctor* conversion, bool forceSampling)
{
	if (!OPTS->ExportAnimations() || controller == NULL) return;
	
	ClipList clips = GetAnimationClips(controller);
	VALIDATE_ANIM_CLIPS( clips, return );
	Clip** it = clips.begin();
	FCDAnimationCurveTrackList addedCurves;

	controller = (*it)->controller;
	FCDAnimation* animation = CreateAnimation(*it, id.c_str());

#define NUM_CURVES 3

	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	bOneChannelPerCurve = true;
	int numChannels = bOneChannelPerCurve ? NUM_CURVES : 1 ;

	FCDAnimationChannel **channels = new FCDAnimationChannel*[numChannels];
	for ( int i = 0; i < numChannels; ++i )
	{
		channels[i] =  animation->AddChannel();
	}

	FCDAnimated *animateds[NUM_CURVES];
	FUAssert( rotations.size() == NUM_CURVES, return );
	for (size_t i = 0; i < NUM_CURVES; ++i)
	{
		animateds[i] = rotations[i]->GetAngleAxis().GetAnimated();
	}

	Control* subControllers[3] = { controller->GetZController(), controller->GetYController(), controller->GetXController() };

	// First, Try to extract animations from the component controllers
	bool animationValid = false;
	FUAssert( subControllers[0] != NULL && subControllers[1] != NULL && subControllers[2] != NULL, return );
	if (subControllers[0] != NULL && subControllers[1] != NULL && subControllers[2] != NULL)
	{
		for (size_t i = 0; i < NUM_CURVES; ++i)
		{
			FCDAnimationChannel *channel = bOneChannelPerCurve ? channels[i] : channels[0];
			FCDAnimationCurve* curve = CreateCurve(channel, subControllers[i]);
			ExportCurve( subControllers[i], curve, animateds[i], conversion, forceSampling, APA_ANIM_TARGET_ROTATION );
			if (curve != NULL)
			{
				animateds[i]->AddCurve(EULER_ROTATION_ANGLE_ID, curve);
				if ((*it)->colladaClip != NULL) (*it)->colladaClip->AddClipCurve(curve);
				animationValid = true;
				addedCurves.push_back( curve );

				rotations[i]->SetSubId( k_eulerRotationSubIds[i] );
			}
		}
	}

	AddCurvesToClips( addedCurves, it + 1, clips.end() );

	SAFE_DELETE_ARRAY( channels );

	if (!animationValid) SAFE_RELEASE(animation);

	for (size_t i = 0; i < NUM_CURVES; ++i)
	{
		if (!animateds[i]->HasCurve())
		{
			SAFE_RELEASE(animateds[i]);
		}
	}

#undef NUM_CURVES
}

FCDAnimated* AnimationExporter::ExportAnimatedAngAxis(const fm::string& id, Control* controller, ColladaMaxAnimatable::Type type, FCDParameterAnimatableAngleAxis& colladaValue, FCDConversionFunctor* conversion, bool forceSampling)
{
	if (!OPTS->ExportAnimations() || controller == NULL) return NULL;
	
	FCDAnimated* animated = colladaValue.GetAnimated();
	ClipList clips = GetAnimationClips(controller);
	VALIDATE_ANIM_CLIPS( clips, return NULL );
	Clip** it = clips.begin();
	FCDAnimationCurveTrackList addedCurves;

	controller = (*it)->controller;
	FCDAnimation* animation = CreateAnimation(*it, id.c_str());

#define NUM_CURVES 4

	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	int numChannels = bOneChannelPerCurve ? NUM_CURVES : 1 ;

	FCDAnimationChannel **channels = new FCDAnimationChannel*[numChannels];
	for ( int i = 0; i < numChannels; ++i )
	{
		channels[i] =  animation->AddChannel();
	}

	if (!ExportAnimation(*it, type, animated, channels, addedCurves, conversion, forceSampling))
	{
		SAFE_RELEASE(animation);
	}

	AddCurvesToClips( addedCurves, it + 1, clips.end() );

	SAFE_DELETE_ARRAY( channels );

	if (!animated->HasCurve())
	{
		SAFE_RELEASE(animated);
	}

	return animated;

#undef NUM_CURVES
}

bool AnimationExporter::ValidateAnimKeyTime( FCDAnimationCurve *curve, TimeValue t )
{
	if ( !OPTS->CleanAnimKeys() )
	{
		return true;
	}

	bool bValidAnimTime = false;
	int clipCount = curve->GetClipCount();
	FCDAnimationClip **begin = curve->GetClips();
	FCDAnimationClip **end = curve->GetClipsEnd();
	float timeInSecs = FSConvert::MaxTickToSecs( t );

	for ( FCDAnimationClipList::iterator clipIt = begin; clipIt != end; clipIt++ )
	{
		FCDAnimationClip *pClip = *clipIt;
		float clipStart = pClip->GetStart();
		float clipEnd = pClip->GetEnd();

		if ( timeInSecs >= clipStart && timeInSecs <= clipEnd )
		{
			bValidAnimTime = true;
			break;
		}
	}

	return bValidAnimTime;
}

void AnimationExporter::GetAnimatableTimeMappingList( Animatable *pAnimatable, TimeMappingList &keyTimeMappingList, int flags )
{
	if ( !pAnimatable )
	{
		return;
	}

	TicksToSecsMap controllerKeyTimeMap;
	AnimationClipManager::GetInst()->GetAnimatableKeyTimes( pAnimatable, controllerKeyTimeMap );

	// Get proper global time sampling map
	TicksToSecsMap animatableSamplingTimeMap;
	animatableSamplingTimeMap.merge( TEST_FLAG( flags, GATMLF_USE_SECONDARY_SAMPLE_RATE ) ? m_secondaryLinearSamplingTimeMap : m_primaryLinearSamplingTimeMap );

	// Merge animatable key times in map if required
	if ( OPTS->ForceAnimationKeySampling() )
	{
		animatableSamplingTimeMap.merge( controllerKeyTimeMap );
	}
	
	// fill time mapping list
	for ( TicksToSecsMap::iterator keyTimeMapIter = animatableSamplingTimeMap.begin(); keyTimeMapIter != animatableSamplingTimeMap.end(); ++keyTimeMapIter )
	{
		keyTimeMappingList.push_back( TimeMapping( keyTimeMapIter->first, keyTimeMapIter->second ) );
	}

	// sort
	keyTimeMappingList.sort();
}

FCDAnimated* AnimationExporter::ExportAnimatedTransform(const fm::string& id, INode* node, Control *c, FCDParameterAnimatableMatrix44& colladaValue, bool bIsWSMPivot )
{
	if (!OPTS->ExportAnimations() || node == NULL || c == NULL ) return NULL;

	FCDAnimated* animated = colladaValue.GetAnimated();
	ClipList clips = GetAnimationClips(NULL);
	VALIDATE_ANIM_CLIPS( clips, return NULL );
	Clip** it = clips.begin();
	FCDAnimationCurveTrackList addedCurves;
	FCDAnimation* animation = CreateAnimation((*it), id.c_str());
	animation->SetDefaultAnimated( animated );

#define NUM_COLS 3
#define NUM_ROWS 4
#define NUM_CURVES ( NUM_COLS * NUM_ROWS )
	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	int numChannels = bOneChannelPerCurve ? NUM_CURVES : 1 ;
	FCDAnimationChannel **channels = new FCDAnimationChannel*[numChannels];
	for ( int i = 0; i < numChannels; ++i )
	{
		channels[i] =  animation->AddChannel();
	}

	// Create the sixteen needed curves
	FCDAnimationCurveList curves;
    FUAssert(animated->GetValueCount() >= NUM_CURVES, return NULL); // checks against programmer error?
	static const size_t dimensions = NUM_CURVES;
	curves.reserve(dimensions);
	for (size_t i = 0; i < dimensions; ++i)
	{
		FCDAnimationChannel *channel = bOneChannelPerCurve ? channels[i] : channels[0];
		FCDAnimationCurve* curve = CreateCurve(channel);
		animated->AddCurve(i, curve);
		curves.push_back(curve);
		if ((*it)->colladaClip != NULL) (*it)->colladaClip->AddClipCurve(curve);
		addedCurves.push_back( curve );
	}

	AddCurvesToClips( addedCurves, it + 1, clips.end() );

	// Actual sampling
	Matrix3 nodeTM, tmpTM;
	INode* parentNode = NULL;

	if ( !bIsWSMPivot )
	{
		parentNode = node->GetParentNode();
		if (parentNode != NULL && parentNode->IsRootNode()) parentNode = NULL;
	}

	FUDaeInterpolation::Interpolation interpolation = GET_TM_CONTROLLER_SAMPLING_INTERPOLATION( c );

	// key buffers
	FCDAnimationKeyList keyBuffers[NUM_CURVES];
	for ( int curveId = 0; curveId < NUM_CURVES; ++curveId )
	{
		keyBuffers[curveId].reserve( 1024 );
	}

	TimeMappingList keyTimeMappingList;
	GetAnimatableTimeMappingList( c, keyTimeMappingList, CTRL->FindConstraint( c ) ? GATMLF_USE_SECONDARY_SAMPLE_RATE : GATMLF_NONE );
	for ( TimeMappingList::iterator ticksToSecsMapIter = keyTimeMappingList.begin(); ticksToSecsMapIter != keyTimeMappingList.end(); ++ticksToSecsMapIter )
	{
		const TimeValue &t = ticksToSecsMapIter->m_ticks;
		const float &tSecs = ticksToSecsMapIter->m_secs;

		// Export the base NODE TM
		nodeTM = node->GetNodeTM(t);

		if ( bIsWSMPivot )
		{
			tmpTM = node->GetObjTMAfterWSM(t);
			nodeTM.Invert();
			nodeTM = tmpTM * nodeTM;
		}
		else if ( parentNode != NULL)
		{
			// export a relative TM
			// We have to use whatever value we exported for this parent
			// in order to remain consistent in collada
			tmpTM = parentNode->GetNodeTM(t);
			tmpTM.Invert();
			nodeTM = nodeTM * tmpTM;
		}

		// Push this matrix as separate animation curves
		const Matrix3& _tm = nodeTM;
		for (int row = 0; row < NUM_ROWS; ++row)
		{
			for (int col = 0; col < NUM_COLS; ++col)
			{
				ADD_ANIMATION_KEY( keyBuffers[col * NUM_ROWS + row], FUDaeInterpolation::LINEAR, tSecs, _tm[row][col] );
			}
		}
	}

	bool bForceAnimExport = IS_FORCE_ANIM_EXPORT( c );

	// adding keys to curves
	for ( int row = 0; row < NUM_ROWS; ++row )
	{
		for ( int col = 0; col < NUM_COLS; col++ )
		{
			int curveId = col * NUM_ROWS + row;
			AddAnimationKeysToCurve( keyBuffers[curveId], curves[curveId], bOneChannelPerCurve, true );
			bool bValidCurve = ValidateAnimationCurve( curves[curveId], true );
			if ( !bValidCurve && !bForceAnimExport && !OPTS->ExportInvalidTransformAnimations() )
			{
				SAFE_RELEASE( curves[curveId] );
			}
		}
	}

	SAFE_DELETE_ARRAY( channels );

	// Delete empty animations
	if (!animated->HasCurve())
	{
		SAFE_RELEASE(animated);
		SAFE_RELEASE(animation);
	}
	return animated;

#undef NUM_COLS
#undef NUM_ROWS
#undef NUM_CURVES
}

void AnimationExporter::AddAnimationKeysToCurve( FCDAnimationKeyList &keys, FCDAnimationCurve *pCurve, bool bOneChannelPerCurve, bool bIsSampled )
{
#define ADD_CURVE_KEY( key, keyId ) \
{ \
	pCurve->AddKey( key ); \
	keys[keyId] = NULL; \
}

#define CALCULATE_SLOPE ( ( curOutput - lastValidOutput ) / ( curInput - lastValidInput ) )

#define SET_LAST_VALID_SLOPE \
{ \
	lastValidOutput = prevOutput; \
	lastValidInput = prevInput; \
	lastValidSlope = CALCULATE_SLOPE; \
}

	int keyCount = keys.size();
	if ( keyCount < 2 )
	{
		return;
	}

	// first key values
	uint32 firstKeyInterpolation = keys[0]->interpolation;

	// linear key compression inits
	float curInput;
	float curOutput;
	
	float prevInput;
	float prevOutput;

	float lastValidInput;
	float lastValidOutput;
	float lastValidSlope;

	float currentSlope;
	
	FCDAnimationKey *pCurKey = NULL;
	FCDAnimationKey *pPrevKey = NULL;
	const float compressionTolerance = OPTS->KeyCompressionTolerance();

	bool bCompressKeys = OPTS->CompressAnimKeys();
	
	// assuming all pCurve keys are interpolated the same way
	bCompressKeys = VALIDATE_ANIM_KEY_OPTIMIZATION( bCompressKeys, bOneChannelPerCurve, firstKeyInterpolation, bIsSampled );

	for ( size_t keyId = 0; keyId < keyCount; ++keyId )
	{
		pCurKey = keys[keyId];

		// linear key compression
		if ( bCompressKeys )
		{
			// first key
			if ( keyId == 0 ) 
			{ 
				ADD_CURVE_KEY( pCurKey, keyId );
			} 
			else 
			{
				curInput = pCurKey->input;
				curOutput = pCurKey->output;
				prevInput = pPrevKey->input;
				prevOutput = pPrevKey->output;

				// second key
				if ( keyId == 1 ) 
				{ 
					SET_LAST_VALID_SLOPE;
				} 
				else
				{ 
					currentSlope = CALCULATE_SLOPE;

					// if test fails, non linear progression, must add previous key
					if ( !IsEquivalent( currentSlope, lastValidSlope, compressionTolerance ) )
					{
						ADD_CURVE_KEY( pPrevKey, keyId - 1 );
						SET_LAST_VALID_SLOPE;
					}
				}
				// else linear progression, keep the value of the current key in a temporary buffer	
			} 
		}
		// no key compression
		else
		{
			ADD_CURVE_KEY( pCurKey, keyId );
		}

		pPrevKey = pCurKey;
	}

	// add last anim key
	if ( bCompressKeys )
	{
		ADD_CURVE_KEY( pPrevKey, keyCount - 1 );
	}

	CLEAR_POINTER_VECTOR( keys );

#undef ADD_CURVE_KEY
#undef CALCULATE_SLOPE
#undef SET_LAST_VALID_SLOPE
}

bool AnimationExporter::ValidateAnimationCurve( FCDAnimationCurve *&pCurve, bool bIsSampled )
{
	FUAssert( pCurve, return false );

	int keyCount = pCurve->GetKeyCount();
	if ( keyCount < 2 )
	{
		return false;
	}

	bool bCompressKeys = OPTS->CompressAnimKeys();
	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	
	uint32 firstKeyInterpolation = pCurve->GetKey( 0 )->interpolation;
	float firstKeyOutput = pCurve->GetKey( 0 )->output;
	
	bCompressKeys = VALIDATE_ANIM_KEY_OPTIMIZATION( bCompressKeys, bOneChannelPerCurve, firstKeyInterpolation, bIsSampled );

	// Verify that the samples create an animation
	if ( bCompressKeys )
	{
		const float compressionTolerance = OPTS->KeyCompressionTolerance();

		// if keys are optimized ( compressed ), the only way we can have a constant animation
		// is if there are only two identical keys
		if ( keyCount == 2 
			&& IsEquivalent( pCurve->GetKey( 0 )->output, pCurve->GetKey( 1 )->output, compressionTolerance ))
		{
			return false;
		}
	}
	else
	{
		bool bKeepCurve = false;
		for ( size_t keyId = 1; keyId < keyCount && !bKeepCurve; ++keyId )
		{
			bKeepCurve |= !IsEquivalent(firstKeyOutput, pCurve->GetKey( keyId )->output );
		}

		if (!bKeepCurve)
		{
			return false;
		}
	}

	return true;
}

#undef RELEASE_CURVE

void AnimationExporter::AddControllerKeyTimesToMap( Control *pControl, fm::map<TimeValue,bool> &map, FUDaeInterpolation::Interpolation interpolation )
{
	IKeyControl *pKeyInterface = NULL;
	int numKeys = ControllerExporter::GetNumControllerKeys( pControl, pKeyInterface );
	if ( numKeys > 0 && pKeyInterface )
	{
		int keySize = pKeyInterface->GetKeySize();
		void *p = malloc( keySize );
		IKey *pKey = ( IKey * )p;

		for ( int i = 0; i < numKeys; i++ )
		{
			pKeyInterface->GetKey( i, pKey );
			if ( pKey == NULL )
			{
				continue;
			}

			TimeValue keyTime = pKey->time;
			fm::map<TimeValue,bool>::iterator iter = map.find( keyTime );
			if ( iter == map.end() )
			{
				map.insert( keyTime, false );
			}
		}

		free( p );	
	}
}

// Establishing correspondance between curves and their respective controllers
// Used to decide whether or not we should keep the curve when option
// to force the export of degenerate curves is enabled
void AnimationExporter::GetDecomposedTransformControllerArray( Control *pControl, int numCurves[], fm::vector<fm::pvector<Control>> &controllers )
{
	controllers.resize( NUM_ANIMATIONS );
	for ( size_t animId = 0; animId < NUM_ANIMATIONS; ++animId )
	{
		controllers[animId].resize( numCurves[animId] ); 
	}

	int rotationFormat = OPTS->RotationFormat();
	Class_ID animControllerClassId = pControl->ClassID();

	if ( !pControl || ControllerExporter::IsBipedController( pControl ) )
	{
		for ( size_t animId = 0; animId < NUM_ANIMATIONS; ++animId )
		{
			for ( size_t curveId = 0; curveId < numCurves[animId]; ++curveId )
			{
				controllers[animId][curveId] = pControl;
			}
		}		
	}
	else if ( animControllerClassId == Class_ID(PRS_CONTROL_CLASS_ID, 0) )
	{
#define SET_XYZ_CONTROLLERS( animId, controller ) \
{ \
	controllers[animId][0] = controller ? controller->GetXController() : NULL; \
	controllers[animId][1] = controller ? controller->GetYController() : NULL; \
	controllers[animId][2] = controller ? controller->GetZController() : NULL; \
}
		Control *positionController = pControl->GetPositionController();
		SET_XYZ_CONTROLLERS( TRANSLATION_ANIM, positionController );

		Control *rotationController = pControl->GetRotationController();
		if ( rotationFormat == ColladaOptions::EULER_XYZ )
		{
			SET_XYZ_CONTROLLERS( ROTATION_ANIM, rotationController );
		}
		else
		{
			for ( size_t curveId = 0; curveId < numCurves[ROTATION_ANIM]; ++curveId )
			{
				controllers[ROTATION_ANIM][curveId] = rotationController;
			}
		}

		Control *scaleController = pControl->GetScaleController();
		SET_XYZ_CONTROLLERS( SCALE_ANIM, scaleController );

#undef SET_XYZ_CONTROLLERS
	}
}

void AnimationExporter::DecomposedAnimatedTransformPostExport( INode *pNode, Control *pControl, FCDAnimationCurveList curves[], int numCurves[], FUDaeInterpolation::Interpolation interpolation, fm::pvector<FCDTRotation> rotations, FCDAnimationKeyList *keyBuffers[], fm::vector<fm::pvector<Control>> &controllers )
{
	Class_ID animControllerClassId = pControl->ClassID();
	int rotationFormat = OPTS->RotationFormat();
	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	//bool bCompressKeys = OPTS->CompressAnimKeys();
	//bool bForceAnimExport = IS_FORCE_ANIM_EXPORT( pControl );

	// validity buffers
	fm::vector<fm::vector<bool>> curveValidityBuffer;
	fm::vector<bool> animValidityBuffer;

	curveValidityBuffer.resize( NUM_ANIMATIONS );
	animValidityBuffer.resize( NUM_ANIMATIONS );

	for ( int animId = 0; animId < NUM_ANIMATIONS; ++animId )
	{
		curveValidityBuffer[animId].resize( numCurves[animId] );
		animValidityBuffer[animId] = false;
	}

	// adding keys to curves and validating curve
	for ( int animId = 0; animId < NUM_ANIMATIONS; ++animId )
	{
		for ( int curveId = 0; curveId < numCurves[animId]; curveId++ )
		{
			FCDAnimationCurve *pCurve = curves[animId][curveId];
			AddAnimationKeysToCurve( keyBuffers[animId][curveId], pCurve, IS_TRANSFORM_ANIM_ONE_CHANNEL_PER_CURVE( bOneChannelPerCurve, rotationFormat, animId == ROTATION_ANIM ), true );
			curveValidityBuffer[animId][curveId] = ValidateAnimationCurve( pCurve, true );
			animValidityBuffer[animId] |= curveValidityBuffer[animId][curveId];
		}
	}

	if ( !OPTS->ExportInvalidTransformAnimations() )
	{
		// removing invalid curves
		for ( int animId = 0; animId < NUM_ANIMATIONS; ++animId )
		{
			for ( int curveId = 0; curveId < numCurves[animId]; curveId++ )
			{
				FCDAnimationCurve *pCurve = curves[animId][curveId];
				if ( !curveValidityBuffer[animId][curveId] )
				{
					int keyCount = pCurve->GetKeyCount();
					if ( !IS_FORCE_ANIM_EXPORT( controllers[animId][curveId] ) || keyCount < 1 )
					{
						SAFE_RELEASE( curves[animId][curveId] );
					}
				}
			}
		}

		// always removing useless biped animations
		// useless biped animation : scale and translate anims for biped nodes that are not Bip01, Prop1, Prop2 and Prop3
		for ( int animId = 0; animId < NUM_ANIMATIONS; ++animId )
		{
			bool bRemoveAnim = ControllerExporter::IsBipedController( pControl )
				&& ( animId == TRANSLATION_ANIM || animId == SCALE_ANIM );

			// Bip01
			bRemoveAnim = bRemoveAnim && animControllerClassId != BIPBODY_CONTROL_CLASS_ID; // not using &= to avoir compilation warning

			// prop1, prop2, prop3
			IBipMaster *pBipMaster = GetBipMasterInterface( pControl );
			if ( pBipMaster )
			{
				int id = KEY_NOTHING;
				int link = -1;
				BOOL bRetVal = pBipMaster->GetIdLink( pNode, id, link );
				FUDbgAssert( bRetVal, );
				if ( bRetVal )
				{
					bRemoveAnim &= id != KEY_PROP1 
							&& id != KEY_PROP2
							&& id != KEY_PROP3;
				}
			}
			
			if ( !OPTS->TrimBipedAnims() )
			{
				// removing useless biped animations only if curve is invalid and forcing export of degenerate curves
				bRemoveAnim &= !animValidityBuffer[animId] && IS_FORCE_ANIM_EXPORT( pControl );
			}

			if ( bRemoveAnim )
			{
				for ( int curveId = 0; curveId < numCurves[animId]; curveId++ )
				{
					SAFE_RELEASE( curves[animId][curveId] );
				}
			}
		}
	}

	// reducing number of keys of curves belonging to invalid anims to 1
	for ( int animId = 0; animId < NUM_ANIMATIONS; ++animId )
	{
		if ( !animValidityBuffer[animId] )
		{
			for ( int curveId = 0; curveId < numCurves[animId]; curveId++ )
			{
				if ( curves[animId][curveId] )
				{
					curves[animId][curveId]->SetKeyCount( 1, interpolation );
				}
			}
		}
	}

	// set euler rotation sub id
	for ( int animId = 0; animId < NUM_ANIMATIONS; ++animId )
	{
		for ( int curveId = 0; curveId < numCurves[animId]; curveId++ )
		{		
			FCDAnimationCurve *pCurve = curves[animId][curveId];
			if ( IS_EULER_ROT_ANIM( rotationFormat, animId )
				&& pCurve )
			{
				rotations[curveId]->SetSubId( k_eulerRotationSubIds[curveId] );
			}
		}
	}
}


void AnimationExporter::FitCubicCurve(const fm::string& id, INode* node, Control *c, FCDParameterAnimatableVector3& tColladaValue, FCDParameterAnimatableVector3 &kColladaValue, fm::pvector<FCDTRotation> rotations, bool bIsWSMPivot )
{
#define PREV_KEY_EVAL 0
#define CUR_KEY_EVAL 1
#define SUP_KEY_A_EVAL 2
#define SUP_KEY_B_EVAL 3
#define NUM_EVAL_POINTS 4

#define ADD_CONTROLLER_KEYTIMES_TO_MAP( pControl ) \
{ \
	AddControllerKeyTimesToMap( pControl, keyTimes, interpolation ); \
}

#define EVAL_KEY( __evalData, __time, __t ) \
{ \
	(__evalData).m_time = (__time); \
	(__evalData).m_t = (__t); \
	(__evalData).m_secs = (float)FSConvert::MaxTickToSecs(__time); \
	 \
	GET_NODE_TM( nodeTM, tmpTM, parentNode, (__evalData).m_time ); \
	NodeExporter::AffineDecomp( nodeTM, (__evalData).m_ap ); \
	 \
	(__evalData).m_translation = (__evalData).m_ap.t; \
	(__evalData).m_scale = (__evalData).m_ap.k; \
	GET_ROTATION_FROM_AFFINE_PARTS( (__evalData).m_rotation, (__evalData).m_ap, rotationFormat ); \
}

#define GET_LINEAR_SLOPE_AND_INTERCEPT( intercept, slope, __t, __b_t, p0, p1 ) \
{ \
	float __t_squared = __t * __t; \
	float __t_cubed = __t_squared * __t; \
	 \
	float __iT = 1.f - __t; \
	float __iT_squared = __iT * __iT; \
	float __iT_cubed = __iT_squared * __iT; \
	 \
	intercept = ( __b_t - ( __iT_cubed * p0 ) - ( __t_cubed * p1 ) ) / ( 3 * __iT_squared * __iT ); \
	slope = - __t / __iT; \
}

#define CREATE_FIRST_KEYS( __animId, __interpolation, __secs, __transform ) \
{ \
	for ( int curveId = 0; curveId < numCurves[(__animId)]; ++curveId ) \
	{ \
		FCDAnimationKey *pKey; \
		ADD_ANIMATION_KEY_PTR( pKey, keyBuffers[(__animId)][curveId], __interpolation, __secs, __transform[curveId] ); \
		prevKeys[(__animId)][curveId] = pKey; \
	} \
}

#define GET_BEZIER_CONTROL_POINTS( __animId, __transform ) \
{ \
	for ( int curveId = 0; curveId < numCurves[(__animId)]; curveId++ ) \
	{ \
		FMVector2 p0( evalData[PREV_KEY_EVAL].m_secs, evalData[PREV_KEY_EVAL].__transform[curveId] ); \
		FMVector2 p1( evalData[CUR_KEY_EVAL].m_secs, evalData[CUR_KEY_EVAL].__transform[curveId] ); \
		 \
		FMVector2 c0; \
		FMVector2 c1; \
		 \
		float a, b, c, d; \
		 \
		GET_LINEAR_SLOPE_AND_INTERCEPT( a, b, evalData[SUP_KEY_A_EVAL].m_t, evalData[SUP_KEY_A_EVAL].m_secs, p0.x, p1.x ); \
		GET_LINEAR_SLOPE_AND_INTERCEPT( c, d, evalData[SUP_KEY_B_EVAL].m_t, evalData[SUP_KEY_B_EVAL].m_secs, p0.x, p1.x ); \
		 \
		c1.x = ( c - a ) / ( b - d ); \
		c0.x = a + b * c1.x; \
		 \
		GET_LINEAR_SLOPE_AND_INTERCEPT( a, b, evalData[SUP_KEY_A_EVAL].m_t, evalData[SUP_KEY_A_EVAL].__transform[curveId], p0.y, p1.y ); \
		GET_LINEAR_SLOPE_AND_INTERCEPT( c, d, evalData[SUP_KEY_B_EVAL].m_t, evalData[SUP_KEY_B_EVAL].__transform[curveId], p0.y, p1.y ); \
		 \
		c1.y = ( c - a ) / ( b - d ); \
		c0.y = a + b * c1.y; \
		 \
		if ( interpolation == FUDaeInterpolation::HERMITE ) \
		{ \
			c0 = BEZIER_TO_HERMITE_OUTTAN( c0, p0 ); \
			c1 = BEZIER_TO_HERMITE_INTAN( c1, p1 ); \
		} \
		 \
		/* Set Prev Key outtangent */ \
		FCDAnimationKeyBezier *__pBezierKey; \
		__pBezierKey = ( FCDAnimationKeyBezier * )prevKeys[(__animId)][curveId]; \
		__pBezierKey->outTangent.x = c0.x; \
		__pBezierKey->outTangent.y = c0.y; \
		\
		/* Create Current Key */ \
		FCDAnimationKey *pCurKey; \
		ADD_ANIMATION_KEY_PTR( pCurKey, keyBuffers[(__animId)][curveId], interpolation, p1.x, p1.y ); \
		 \
		 \
		/* Set Cur Key intangent */ \
		__pBezierKey = ( FCDAnimationKeyBezier * )pCurKey; \
		__pBezierKey->inTangent.x = c1.x; \
		__pBezierKey->inTangent.y = c1.y; \
		 \
		prevKeys[(__animId)][curveId] = pCurKey; \
	} \
}

	// Export Animations
	if (!OPTS->ExportAnimations() || node == NULL) return;

	if ( !ControllerExporter::IsBipedController( c ) )
	{
		return;
	}

	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	FUDaeInterpolation::Interpolation interpolation = OPTS->CubicInterpolation();
	Class_ID animControllerClassId = c->ClassID();

	FUAssert( interpolation == FUDaeInterpolation::BEZIER || interpolation == FUDaeInterpolation::HERMITE, return );

	// get key times
	fm::map<TimeValue,bool> keyTimes;

	ADD_CONTROLLER_KEYTIMES_TO_MAP( c );
	if ( animControllerClassId == BIPBODY_CONTROL_CLASS_ID )
	{
		IBipMaster *bipMaster = GetBipMasterInterface(c);
		if ( bipMaster )
		{
			ADD_CONTROLLER_KEYTIMES_TO_MAP( bipMaster->GetVerticalControl() );
			ADD_CONTROLLER_KEYTIMES_TO_MAP( bipMaster->GetHorizontalControl() );
			ADD_CONTROLLER_KEYTIMES_TO_MAP( bipMaster->GetTurnControl() );
		}
	}

	// validate
	size_t numKeys = keyTimes.size();
	if ( numKeys <= 0 )
	{
		return;
	}

	DECOMPOSED_ANIMATED_TRANSFORM_PRE_EXPORT;

	CurveEvalData evalData[NUM_EVAL_POINTS];
	FCDAnimationKeyList prevKeys[NUM_ANIMATIONS];
	for ( size_t animId = 0; animId < NUM_ANIMATIONS; ++animId )
	{
		prevKeys[animId].resize( numCurves[animId] );
	}

	// get first key decomposed transform
	EVAL_KEY( evalData[PREV_KEY_EVAL], keyTimes.begin()->first, 0.f );

	CREATE_FIRST_KEYS( TRANSLATION_ANIM, interpolation, evalData[PREV_KEY_EVAL].m_secs, evalData[PREV_KEY_EVAL].m_translation );
	CREATE_FIRST_KEYS( ROTATION_ANIM, interpolation, evalData[PREV_KEY_EVAL].m_secs, evalData[PREV_KEY_EVAL].m_rotation );
	CREATE_FIRST_KEYS( SCALE_ANIM, interpolation, evalData[PREV_KEY_EVAL].m_secs, evalData[PREV_KEY_EVAL].m_scale );

	for ( fm::map<TimeValue,bool>::iterator keyTimeIter = ++keyTimes.begin(); keyTimeIter != keyTimes.end(); ++keyTimeIter )
	{
		EVAL_KEY( evalData[CUR_KEY_EVAL], keyTimeIter->first, 1.f );

		float oneThird = 1.f / 3.f;
		TimeValue keyRangeThird = (TimeValue)(( evalData[CUR_KEY_EVAL].m_time - evalData[PREV_KEY_EVAL].m_time ) * oneThird );

		TimeValue evalATime = evalData[PREV_KEY_EVAL].m_time + keyRangeThird;
		EVAL_KEY( evalData[SUP_KEY_A_EVAL], evalATime, oneThird );

		TimeValue evalBTime = evalData[CUR_KEY_EVAL].m_time - keyRangeThird;
		EVAL_KEY( evalData[SUP_KEY_B_EVAL], evalBTime, oneThird * 2 );

#ifdef _DEBUG
		FUAssert( evalData[SUP_KEY_A_EVAL].m_time != evalData[PREV_KEY_EVAL].m_time, continue );
		FUAssert( evalData[SUP_KEY_A_EVAL].m_time != evalData[SUP_KEY_B_EVAL].m_time, continue );
		FUAssert( evalData[SUP_KEY_B_EVAL].m_time != evalData[CUR_KEY_EVAL].m_time, continue );
#endif // _DEBUG

		GET_BEZIER_CONTROL_POINTS( TRANSLATION_ANIM, m_translation );
		GET_BEZIER_CONTROL_POINTS( ROTATION_ANIM, m_rotation );
		GET_BEZIER_CONTROL_POINTS( SCALE_ANIM, m_scale );
	}

	fm::vector<fm::pvector<Control>> controllers;
	GetDecomposedTransformControllerArray( c, numCurves, controllers );
	DecomposedAnimatedTransformPostExport( node, c, curves, numCurves, interpolation, rotations, keyBuffers, controllers );
	DECOMPOSED_ANIMATED_TRANSFORM_CLEANUP;

#undef ADD_CONTROLLER_KEYTIMES_TO_MAP
#undef PREV_KEY_EVAL
#undef CUR_KEY_EVAL
#undef SUP_KEY_A_EVAL
#undef SUP_KEY_B_EVAL
#undef NUM_EVAL_POINTS
#undef EVAL_KEY
#undef GET_LINEAR_SLOPE_AND_INTERCEPT
#undef CREATE_FIRST_KEYS
#undef GET_BEZIER_CONTROL_POINTS
}

void AnimationExporter::ExportDecomposedAnimatedTransform(const fm::string& id, INode* node, Control *c, FCDParameterAnimatableVector3& tColladaValue, FCDParameterAnimatableVector3 &kColladaValue, fm::pvector<FCDTRotation> rotations, bool bIsWSMPivot )
{
	// Export Animations
	if (!OPTS->ExportAnimations() || node == NULL || c == NULL ) return;

	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	FUDaeInterpolation::Interpolation interpolation = GET_TM_CONTROLLER_SAMPLING_INTERPOLATION( c );
	Class_ID animControllerClassId = c->ClassID();

	// Setup the curve keys and interpolations.
	DECOMPOSED_ANIMATED_TRANSFORM_PRE_EXPORT;

	// biped debug
	//if ( c->ClassID() == BIPBODY_CONTROL_CLASS_ID )
	//{
	//	IBipMaster *pBipMaster = GetBipMasterInterface(c);
	//	DWORD activeModes = pBipMaster->GetActiveModes();
	//	if ( TEST_FLAG( activeModes, BMODE_MIXER ) )
	//	{
	//		int tmp = 0;
	//		IMixer *pMixer = pBipMaster->GetMixer();
	//		if ( pMixer )
	//		{
	//			int trackGroupCount = pMixer->NumTrackgroups();
	//			if ( trackGroupCount > 0 )
	//			{
	//				IMXtrackgroup *pTrackGroup = pMixer->GetTrackgroup( 0 );
	//				//const TCHAR *pTrackGroupName = pTrackGroup->GetName();
	//				int trackCount = pTrackGroup->NumTracks();
	//				if ( trackCount > 0 )
	//				{
	//					IMXtrack *pTrack = pTrackGroup->GetTrack( 0 );
	//					int trackType = pTrack->GetTrackType();
	//					if ( trackType == LAYERTRACK )
	//					{
	//						int clipCount = pTrack->NumClips( BOT_ROW );
	//						for ( int clipId = 0; clipId < clipCount; ++clipId )
	//						{
	//							IMXclip *pClip = pTrack->GetClip( clipId, BOT_ROW );
	//							TCHAR *pClipFile = pClip->GetFilename();
	//							int weightCount = pClip->NumWeights();
	//							for ( int weightId = 0; weightId < weightCount; ++weightId )
	//							{
	//								float weight = pClip->GetWeight( weightId );
	//								int weightTime = pClip->GetWeightTime( weightId );
	//								int tmp = 9;
	//							}
	//							int timeWarpCount = pClip->NumTimeWarps();
	//							int tmp = 9;
	//						}
	//					}
	//					else
	//					{
	//						// TODO
	//					}

	//					int tmp = 0;
	//				}
	//			}
	//		}
	//	}
	//}

	// TODO : Generate seperate key time mapping list for each channel if it's a PRS controller
	TimeMappingList keyTimeMappingList;
	GetAnimatableTimeMappingList( c, keyTimeMappingList, CTRL->FindConstraint( c ) ? GATMLF_USE_SECONDARY_SAMPLE_RATE : GATMLF_NONE );
	for ( TimeMappingList::iterator ticksToSecsMapIter = keyTimeMappingList.begin(); ticksToSecsMapIter != keyTimeMappingList.end(); ++ticksToSecsMapIter )
	{
		const TimeValue &t = ticksToSecsMapIter->m_ticks;
		const float &tSecs = ticksToSecsMapIter->m_secs;

		GET_NODE_TM( nodeTM, tmpTM, parentNode, t );

		// Push this matrix as separate animation curves
		const Matrix3& _tm = nodeTM;
		AffineParts ap;
		NodeExporter::AffineDecomp( _tm, ap );

		//// debug
		//{
		//	static IBipMaster *bipMaster = NULL;
		//	if ( !bipMaster )
		//	{
		//		if ( c->ClassID() == BIPBODY_CONTROL_CLASS_ID )
		//		{
		//			bipMaster = GetBipMasterInterface(c);
		//		}
		//	}

		//	if ( bipMaster &&
		//		 CTRL->IsBipedController( c ) )
		//	{
		//		int id, link;
		//		BOOL retVal;
		//		retVal = bipMaster->GetIdLink( node, id, link );

		//		INode *pRotParentNode = bipMaster->GetRotParentNode( id, link ); // in world coords
		//		INode *pPosParentNode = bipMaster->GetPosParentNode( id, link ); // in world coords
		//		Point3 parentPos = bipMaster->GetParentNodePos( t, id, link ); 
		//		Quat parentRot = bipMaster->GetParentNodeRot( t, id, link ); 

		//		Matrix3 parentWorldTM;
		//		Matrix3 parentLocalTM;
		//		INode *grandParentNode = NULL;
		//		if ( parentNode )
		//		{
		//			// world parent
		//			parentWorldTM = parentNode->GetNodeTM( t );

		//			grandParentNode = parentNode->GetParentNode();
		//			if ( grandParentNode != NULL && grandParentNode->IsRootNode() ) grandParentNode = NULL;
		//			if ( grandParentNode != NULL )
		//			{
		//				Matrix3 grandParentTM = grandParentNode->GetNodeTM( t );
		//				grandParentTM.Invert();
		//				parentLocalTM = parentWorldTM * grandParentTM;
		//			}
		//		}

		//		AffineParts parentWorldAP;
		//		AffineParts parentLocalAP;
		//		NodeExporter::AffineDecomp( parentWorldTM, parentWorldAP );
		//		NodeExporter::AffineDecomp( parentLocalTM, parentLocalAP );

		//		// These methods return biped pos and rot in world coords
		//		IOurBipExport *ourBipInterface = (IOurBipExport *) c->GetInterface(I_OURINTERFACE);
		//		Point3 bipedPosition;
		//		Quat bipedRotation;
		//		//Point3 parentBipedPosition;
		//		//Quat parentBipedRotation;
		//		if ( ourBipInterface )
		//		{
		//			bipedPosition = ourBipInterface->GetBipedPosition( t, node );
		//			bipedRotation = ourBipInterface->GetBipedRotation( t, node );
		//			//parentBipedPosition = ourBipInterface->GetBipedPosition( t, pPosParentNode );
		//			//parentBipedRotation = ourBipInterface->GetBipedRotation( t, pRotParentNode );
		//		}

		//		c->ReleaseInterface(I_OURINTERFACE,ourBipInterface);

		//		Point3 localPos = bipedPosition - parentPos;

		//		Matrix3 nodeRotMat, parentRotMat;
		//		bipedRotation.MakeMatrix( nodeRotMat );
		//		parentRot.MakeMatrix( parentRotMat );
		//		parentRotMat.Invert();
		//		nodeRotMat = nodeRotMat * parentRotMat;
		//		AffineParts rotAP;
		//		NodeExporter::AffineDecomp( nodeRotMat, rotAP );
		//		Quat localRot = rotAP.q;

		//		//Quat localRot = bipedRotation * parentRot.Inverse();

		//		if ( /*!( pRotParentNode == pPosParentNode && pPosParentNode == parentNode ) 
		//			&&*/ /*t == OPTS->AnimStart()*/ true
		//			)
		//		{
		//			g_warningLog.append( node->GetName() );
		//			g_warningLog.append( " ==> " );
		//			g_warningLog.append( " ( " );
		//			g_warningLog.append( FMVector3( ( float * )&ap.t ) );
		//			g_warningLog.append( " , " );
		//			g_warningLog.append( FMVector3( ( float * )&localPos ) );
		//			g_warningLog.append( " , " );
		//			g_warningLog.append( FMVector4( ( float * )&ap.q ) );
		//			g_warningLog.append( " , " );
		//			g_warningLog.append( FMVector4( ( float * )&localRot ) );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( FMVector3( ( float * )&ap.t ) );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( FMVector3( ( float * )&bipedPosition ) );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( FMVector4( ( float * )&ap.q ) );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( FMVector4( ( float * )&bipedRotation ) );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( id );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( link );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( pRotParentNode ? pRotParentNode->GetName() : "NULL" );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( pPosParentNode ? pPosParentNode->GetName() : "NULL" );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( FMVector3( ( float * )&parentPos ) );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( FMVector4( ( float * )&parentRot ) );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( parentNode ? parentNode->GetName() : "NULL" );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( FMVector3( ( float * )parentWorldAP.t ) );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( FMVector4( ( float * )parentWorldAP.q ) );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( grandParentNode ? grandParentNode->GetName() : "NULL" );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( FMVector3( ( float * )parentLocalAP.t ) );
		//			//g_warningLog.append( " , " );
		//			//g_warningLog.append( FMVector4( ( float * )parentLocalAP.q ) );
		//			g_warningLog.append( " ) " );
		//			g_warningLog.append( "\n" );	
		//		}

		//		//ap.t = localPos;
		//		//ap.q = localRot;
		//	}
		//}

#define SET_CHANNEL_OUTPUT( animId, curves, numCurves, transform ) \
		for ( int curveId = 0; curveId < numCurves[animId]; curveId++ ) \
		{ \
			ADD_ANIMATION_KEY( keyBuffers[animId][curveId], interpolation, tSecs, transform[curveId] ); \
		}

		Point4 rotation;
		GET_ROTATION_FROM_AFFINE_PARTS( rotation, ap, rotationFormat );

		SET_CHANNEL_OUTPUT( TRANSLATION_ANIM, curves, numCurves, ap.t );
		SET_CHANNEL_OUTPUT( ROTATION_ANIM, curves, numCurves, rotation );
		SET_CHANNEL_OUTPUT( SCALE_ANIM, curves, numCurves, ap.k );

#undef SET_CHANNEL_OUTPUT
	}

	fm::vector<fm::pvector<Control>> controllers;
	GetDecomposedTransformControllerArray( c, numCurves, controllers );
	DecomposedAnimatedTransformPostExport( node, c, curves, numCurves, interpolation, rotations, keyBuffers, controllers );
	DECOMPOSED_ANIMATED_TRANSFORM_CLEANUP;
}

#define INTAN(key,fmVector2) ((FCDAnimationKeyBezier*) key)->inTangent = fmVector2
#define OUTTAN(key,fmVector2) ((FCDAnimationKeyBezier*) key)->outTangent = fmVector2
#define TCBPARAM(key, t, c, b, ei, eo) \
				((FCDAnimationKeyTCB*) key)->tension = t; \
				((FCDAnimationKeyTCB*) key)->continuity = c; \
				((FCDAnimationKeyTCB*) key)->bias = b; \
				((FCDAnimationKeyTCB*) key)->easeIn = ei; \
				((FCDAnimationKeyTCB*) key)->easeOut = eo

#define F( a, b, c )( ((a) + (b)) * (c) ) 
#define IN_INT( keyTime, prevTime, inLength )( F( keyTime, -prevTime, inLength ) )
#define OUT_INT( keyTime, nextTime, outLength )( F( nextTime, -keyTime, outLength ) )
#define INTAN_X( keyTime, inInterval, secsPerTick )( F( keyTime, -inInterval, secsPerTick ) )
#define OUTTAN_X( keyTime, outInterval, secsPerTick )( F( keyTime, outInterval, secsPerTick ) )
#define TAN_Y( value, interval, tan )( value + tan * interval )
#define BEZIER( key, inTan, outTan ){ INTAN( key, inTan ); OUTTAN( key, outTan ); }
#define HERMITE( key, inTan, outTan, p ){ INTAN( key, BEZIER_TO_HERMITE_INTAN( p, inTan ) ); OUTTAN( key, BEZIER_TO_HERMITE_OUTTAN( p, outTan ) ); }
#define CUBIC( key, value, inLength, outLength, inTan, outTan ) \
{ \
	if ( interp == FUDaeInterpolation::BEZIER || \
		 interp == FUDaeInterpolation::HERMITE ) \
	{ \
		/* values needed for BEZIER and HERMITE curves */ \
		float inInterval = IN_INT( k.time, previousTime, inLength ); \
		float outInterval = OUT_INT( k.time, nextTime, outLength ); \
		\
		FMVector2 inTanVec( INTAN_X( k.time, inInterval, secsPerTick ), TAN_Y( value, inInterval, inTan )); \
		FMVector2 outTanVec( OUTTAN_X( k.time, outInterval, secsPerTick ),TAN_Y( value, outInterval, outTan )); \
		\
		/* Dont forget! Max Bezier handles multiple curve types! */ \
		if (interp == FUDaeInterpolation::BEZIER) \
		{ \
			BEZIER( key, inTanVec, outTanVec ); \
		} \
		else if (interp == FUDaeInterpolation::HERMITE) \
		{ \
			FMVector2 p( key->input, key->output ); \
			HERMITE( key, inTanVec, outTanVec, p ); \
		} \
	} \
}

void AnimationExporter::GetSamplingTimeMap( float sampleRate, TicksToSecsMap &samplingMap )
{
	// Get times at specified sampling rate
	{
		int tpf = ( int )( TIME_TICKSPERSEC / sampleRate );
		TimeValue startTime = OPTS->AnimStart();
		TimeValue endTime = OPTS->AnimEnd() + 1;
		
		for ( TimeValue t = startTime; t < endTime; t += tpf )
		{
			samplingMap[t] = FSConvert::MaxTickToSecs( t );
		}
	}

	// forcing clip start and end times to be sampled
	{
		int numAnimClipsToExport = AnimationClipManager::GetInst()->GetNumAnimClipsToExport();
		if ( numAnimClipsToExport >= 1 )
		{
			ClipList clips = GetAnimationClips( NULL );
			for ( size_t clipId = 0; clipId < clips.size(); ++clipId )
			{
				const int clipIndex = AnimationClipManager::GetInst()->GetAnimClipsToExport()[ clipId ];
				samplingMap[AnimationClipManager::GetInst()->GetAnimClipStart( clipIndex )] = clips[clipId]->colladaClip->GetStart();
				samplingMap[AnimationClipManager::GetInst()->GetAnimClipEnd( clipIndex )] = clips[clipId]->colladaClip->GetEnd();
			}
		}
	}
}

void AnimationExporter::GenerateLinearSamplingTimesMaps()
{
	if ( !OPTS->ExportAnimations() )
	{
		return;
	}

	// Get times at specified sampling rate
	GetSamplingTimeMap( OPTS->SampleRate(), m_primaryLinearSamplingTimeMap );
	GetSamplingTimeMap( OPTS->SecondarySampleRate(), m_secondaryLinearSamplingTimeMap );
}

void AnimationExporter::ExportCurve(Control* controller, FCDAnimationCurve*& curve, FCDAnimated *pAnimated, FCDConversionFunctor* conversion, bool forceSampling, int addParamA, int addParamB )
{
	if (!OPTS->ExportAnimations() || controller == NULL)
	{
		SAFE_RELEASE(curve);
		return;
	}

#define ADD_KEY(value) \
	FCDAnimationKey *key = NULL; \
	ADD_ANIMATION_KEY_PTR( key, keyBuffer, interp, tSecs, value )

	// TODO : Cover case when ExportCurve is called in ExportAnimatedEulerAngles
	curve->GetParent()->GetParent()->SetDefaultAnimated( pAnimated );

	// key buffer
	FCDAnimationKeyList keyBuffer;
	keyBuffer.reserve( 1024 );

	bool isSampling = OPTS->SampleAnim() || forceSampling;
	if (!isSampling)
	{
		IKeyControl* keyInterface = GetKeyControlInterface(controller);
		Class_ID clid = controller->ClassID();
		if (clid.PartB() != 0)
		{
			// This is not a Max controller, sample it.
			// The only max controllers that have non-zero
			// values are not keyable (attach, link, etc).
			isSampling = true;
		}
		else if (keyInterface != NULL && keyInterface->GetNumKeys() > 0)
		{
			float secsPerTick = 1.0f / AnimationImporter::GetTicksPerSec();
			int keyCount = keyInterface->GetNumKeys();
			FUDaeInterpolation::Interpolation cubicInterpolation = OPTS->CubicInterpolation();

#define PROCESS_KEYS(keyClassName, interpolation) { \
				keyClassName previousValue, nextValue; \
				for (int i = 0; i < keyCount; ++i) { \
					TimeValue curTime = controller->GetKeyTime(i); \
					TimeValue previousTime = (i > 0) ? controller->GetKeyTime(i - 1) : curTime - 1.0f / secsPerTick; /* Huh? */ \
					TimeValue nextTime = (i < keyCount - 1) ? controller->GetKeyTime(i + 1) : curTime + 1.0f / secsPerTick; /* Huh? */ \
					keyClassName k; \
					keyInterface->GetKey(i, &k); \
					FUDaeInterpolation::Interpolation interp = GetControlInterpolation( controller, interpolation ); \
					float tSecs = k.time * secsPerTick;
#define PROCESS_KEYS_END } break; }

			int type = clid.PartA();
			switch (type)
			{
			case LININTERP_FLOAT_CLASS_ID:
				PROCESS_KEYS(ILinFloatKey, FUDaeInterpolation::LINEAR)
					ADD_KEY(k.val);
				PROCESS_KEYS_END

			case LININTERP_POSITION_CLASS_ID:
				controlType = FMath::Clamp(controlType, ColladaMaxAnimatable::ROTATION_X, ColladaMaxAnimatable::ROTATION_Z);
				PROCESS_KEYS(ILinPoint3Key, FUDaeInterpolation::LINEAR)
					ADD_KEY(k.val[controlType - ColladaMaxAnimatable::ROTATION_X]);
				PROCESS_KEYS_END
			
			case LININTERP_ROTATION_CLASS_ID: {
				controlType = FMath::Clamp(controlType, ColladaMaxAnimatable::ROTATION_X, ColladaMaxAnimatable::ROTATION_Z);

				float pval[3];
				PROCESS_KEYS(ILinRotKey, FUDaeInterpolation::LINEAR)
					float v[3];
					k.val.GetEuler(&v[0],&v[1],&v[2]);
					
					if (i > 0) FSConvert::PatchEuler(pval, v);
					pval[0] = v[0]; pval[1] = v[1]; pval[2] = v[2];

					ADD_KEY(v[controlType - ColladaMaxAnimatable::ROTATION_X]);
				PROCESS_KEYS_END }

			case HYBRIDINTERP_FLOAT_CLASS_ID:
				PROCESS_KEYS(IBezFloatKey, cubicInterpolation)
					interp = PatchBezierKey(k, 0.0f, 0.333f, previousTime, nextTime, interp);
					ADD_KEY(k.val);
					CUBIC( key, k.val, k.inLength, k.outLength, k.intan, k.outtan );
				PROCESS_KEYS_END

			case HYBRIDINTERP_POINT3_CLASS_ID:
			case HYBRIDINTERP_POSITION_CLASS_ID: {
				controlType = FMath::Clamp(controlType, ColladaMaxAnimatable::ROTATION_X, ColladaMaxAnimatable::ROTATION_Z);
				int ii = controlType - ColladaMaxAnimatable::ROTATION_X;
				PROCESS_KEYS(IBezPoint3Key, cubicInterpolation)
					interp = PatchBezierKey(k, Point3::Origin, Point3(0.333f, 0.333f, 0.33f), previousTime, nextTime, interp);
					ADD_KEY(k.val[ii]);
					CUBIC( key, k.val[ii], k.inLength[ii], k.outLength[ii], k.intan[ii], k.outtan[ii] );
				PROCESS_KEYS_END }

			case HYBRIDINTERP_ROTATION_CLASS_ID: {
				controlType = FMath::Clamp(controlType, ColladaMaxAnimatable::ROTATION_X, ColladaMaxAnimatable::ROTATION_Z);
				
				float pval[3];
				PROCESS_KEYS(IBezQuatKey, cubicInterpolation)
					float v[3];
					k.val.GetEuler(&v[0],&v[1],&v[2]);

					if (i > 0) FSConvert::PatchEuler(pval, v);
					pval[0] = v[0]; pval[1] = v[1]; pval[2] = v[2];

					float value = v[controlType - ColladaMaxAnimatable::ROTATION_X];
					ADD_KEY(value);

					CUBIC( key, value, 0.333f, 0.333f, 0.f, 0.f );
				PROCESS_KEYS_END }

			case TCBINTERP_FLOAT_CLASS_ID:
				PROCESS_KEYS(ITCBFloatKey, FUDaeInterpolation::TCB)
					ADD_KEY(k.val);
					TCBPARAM(key, k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
				PROCESS_KEYS_END

			case TCBINTERP_POINT3_CLASS_ID:
			case TCBINTERP_POSITION_CLASS_ID: {
				controlType = FMath::Clamp(controlType, ColladaMaxAnimatable::ROTATION_X, ColladaMaxAnimatable::ROTATION_Z);
				int ii = controlType - ColladaMaxAnimatable::ROTATION_X;
				PROCESS_KEYS(ITCBPoint3Key, FUDaeInterpolation::TCB)
				//PROCESS_KEYS(ITCBPoint3Key, BEZIER)
					ADD_KEY(k.val[ii]);
					TCBPARAM(key,0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
				PROCESS_KEYS_END }

			case TCBINTERP_ROTATION_CLASS_ID:
				controlType = FMath::Clamp(controlType, ColladaMaxAnimatable::ROTATION_X, ColladaMaxAnimatable::ROTATION_Z);
				float pval[3];
				PROCESS_KEYS(ITCBRotKey, FUDaeInterpolation::TCB)
					float v[3];
					Quat quat(k.val);
					quat.GetEuler(&v[0],&v[1],&v[2]);
					if (i > 0) FSConvert::PatchEuler(pval, v);
					pval[0] = v[0]; pval[1] = v[1]; pval[2] = v[2];
					ADD_KEY(v[controlType - ColladaMaxAnimatable::ROTATION_X]);
					TCBPARAM(key,0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
				PROCESS_KEYS_END
		
			default:
				isSampling = true;
				break;
			}
		}
		else if (clid.PartA() == LININTERP_FLOAT_CLASS_ID ||
				clid.PartA() == HYBRIDINTERP_FLOAT_CLASS_ID)
		{
			SAFE_RELEASE(curve);
			return;
		}
		else
		{
			isSampling = true;
		}

#undef PROCESS_KEYS
#undef PROCESS_KEYS_END

	}

	if (isSampling)
	{
		TimeMappingList keyTimeMappingList;
		bool bUseSecondarySamplingRate = CTRL->FindConstraint( controller ) || addParamA == APA_FORCE_SECONDARY_SAMPLE_RATE;
		int flags = bUseSecondarySamplingRate ? GATMLF_USE_SECONDARY_SAMPLE_RATE : GATMLF_NONE;
		GetAnimatableTimeMappingList( controller, keyTimeMappingList, flags );

#define PROCESS_KEYS(valueClassName, defaultValue) { \
			valueClassName value(defaultValue); \
			size_t i = 0; \
			for ( TimeMappingList::iterator ticksToSecsMapIter = keyTimeMappingList.begin(); ticksToSecsMapIter != keyTimeMappingList.end(); ++ticksToSecsMapIter ) \
			{ \
				const TimeValue &t = ticksToSecsMapIter->m_ticks; \
				const float &tSecs = ticksToSecsMapIter->m_secs; \
				controller->GetValue(t, &value, FOREVER, CTRL_ABSOLUTE);

#define PROCESS_KEYS_END ++i; } break; } 

		FUDaeInterpolation::Interpolation interp = GetControlInterpolation( controller, FUDaeInterpolation::LINEAR );
		
		SClass_ID type = controller->SuperClassID();
		switch (type)
		{
		case CTRL_FLOAT_CLASS_ID:
			PROCESS_KEYS(float, 1.0f)
				ADD_KEY(value);
			PROCESS_KEYS_END

		case CTRL_ROTATION_CLASS_ID:
			controlType = FMath::Clamp(controlType, ColladaMaxAnimatable::ROTATION_X, ColladaMaxAnimatable::ROTATION_Z);

			float pval[3], v[3];
			PROCESS_KEYS(Quat, 1)
				//value.GetEuler(&v[0],&v[1],&v[2]);
				QuatToEuler(value, v, EULERTYPE_XYZ);
				if (i > 0) FSConvert::PatchEuler(pval, v);
				pval[0] = v[0]; pval[1] = v[1]; pval[2] = v[2];

				ADD_KEY(v[controlType - ColladaMaxAnimatable::ROTATION_X]);
				++i;
			PROCESS_KEYS_END

		case CTRL_POINT3_CLASS_ID:
			controlType = FMath::Clamp(controlType, ColladaMaxAnimatable::ROTATION_X, ColladaMaxAnimatable::ROTATION_Z);
			PROCESS_KEYS(Point3, Point3::Origin)
				ADD_KEY(value[controlType - ColladaMaxAnimatable::ROTATION_X]);
			PROCESS_KEYS_END

		default:
			FUFail(;);
		}

#undef PROCESS_KEYS
#undef PROCESS_KEYS_END

	}

	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();
	int rotationFormat = OPTS->RotationFormat();

	AddAnimationKeysToCurve( keyBuffer, curve, IS_TRANSFORM_ANIM_ONE_CHANNEL_PER_CURVE( bOneChannelPerCurve, rotationFormat, addParamA == APA_ANIM_TARGET_ROTATION ), isSampling );

	bool bForceAnimExport = IS_FORCE_ANIM_EXPORT( controller );
	if ( !bForceAnimExport )
	{
		bool bValidCurve = ValidateAnimationCurve( curve, isSampling );
		bool bForceExportMorph = OPTS->ExportInvalidMorphTargetWeightAnimations() && addParamA == APA_ANIM_TARGET_MORPH;
		if ( !bForceExportMorph && !bValidCurve )
		{
			SAFE_RELEASE( curve );
		}
	}

#undef ADD_KEY

	if (curve != NULL)
	{
		if (conversion != NULL)
		{
			curve->ConvertValues(conversion, conversion);
		}
		if (isRelativeAnimation && curve->GetKeyCount() > 0)
		{
			FCDConversionOffsetFunctor functor(-initialValue[0]);
			curve->ConvertValues(&functor, &functor);
		}
	}
}

bool AnimationExporter::ExportAnimation(Clip* clip, ColladaMaxAnimatable::Type type, FCDAnimated* animated, FCDAnimationChannel** channels, FCDAnimationCurveTrackList &addedCurves, FCDConversionFunctor* conversion, bool forceSampling)
{
	Control* controller = clip->controller;
	if (!OPTS->ExportAnimations() || controller == NULL || !controller->IsAnimated()) return false;

	bool bOneChannelPerCurve = OPTS->OneChannelPerCurve();

	// Check that the information necessary for a keyframe animation is available.
	bool isSampling = OPTS->SampleAnim() || forceSampling;
	IKeyControl* keyInterface = NULL;
	if (!isSampling)
	{
		keyInterface = GetKeyControlInterface(controller);
		if (keyInterface == NULL) isSampling = true;
		else if (keyInterface->GetNumKeys() <= 0) return false;
	}

	channels[0]->GetParent()->SetDefaultAnimated( animated );

	// Create the necessary curves
	FCDAnimationCurveList curves;
	size_t dimensions = animated->GetValueCount();
	curves.reserve(dimensions);
	for (size_t i = 0; i < dimensions; ++i)
	{
		FCDAnimationChannel *channel = bOneChannelPerCurve ? channels[i] : channels[0];
		FCDAnimationCurve* curve = CreateCurve(channel, controller);
		animated->AddCurve(i, curve);
		curves.push_back(curve);
		if (clip->colladaClip != NULL) clip->colladaClip->AddClipCurve(curve);
		addedCurves.push_back( curve );
	}

	// key buffers
	int curveCount = dimensions;
	FCDAnimationKeyList *keyBuffers;
	keyBuffers = new FCDAnimationKeyList[curveCount];
	for ( int curveId = 0; curveId < curveCount; ++curveId )
	{
		keyBuffers[curveId].reserve( 1024 );
	}

	FCDAnimationKeyList curKeys( dimensions );

#define ADD_KEY(dim, value) ADD_ANIMATION_KEY_PTR( curKeys[dim], keyBuffers[dim], interp, tSecs, value )

	float secsPerTick = 1.0f / AnimationImporter::GetTicksPerSec();
	if (!isSampling)
	{
		int keyCount = keyInterface->GetNumKeys();
		FUDaeInterpolation::Interpolation cubicInterpolation = OPTS->CubicInterpolation();

#define PROCESS_KEYS(expectedDimensions, keyClassName, interpolation) { \
			if (expectedDimensions > dimensions) break; \
			for (int i = 0; i < keyCount; ++i) {  \
				TimeValue curTime = controller->GetKeyTime(i); \
				TimeValue previousTime = (i > 0) ? controller->GetKeyTime(i - 1) : curTime - 1.0f / secsPerTick; /* Huh? */ \
				TimeValue nextTime = (i < keyCount - 1) ? controller->GetKeyTime(i + 1) : curTime + 1.0f / secsPerTick; /* Huh? */ \
				keyClassName k; \
				keyInterface->GetKey(i, &k); \
				FUDaeInterpolation::Interpolation interp = GetControlInterpolation( controller, interpolation ); \
				float tSecs = k.time * secsPerTick;

#define PROCESS_KEYS_END } } break;

#define PROCESS_SCALE_ROT_AXIS( sign ) \
{ \
	interp = PatchBezierKey(k, Point3::Origin, Point3(0.333f, 0.333f, 0.333f), previousTime, nextTime, cubicInterpolation); \
	\
	AngAxis aa(k.val.q); \
	if (aa.axis.Equals(Point3::Origin, TOLERANCE)) aa.axis = Point3::XAxis; \
	float degAngle = sign##FMath::RadToDeg(aa.angle); \
	 \
	ADD_KEY(0, aa.axis.x); ADD_KEY(1, aa.axis.y); ADD_KEY(2, aa.axis.z); ADD_KEY(3, degAngle); \
	 \
	CUBIC( curKeys[0], aa.axis.x, 0.333f, 0.333f, 0.f, 0.f ); \
	CUBIC( curKeys[1], aa.axis.y, 0.333f, 0.333f, 0.f, 0.f ); \
	CUBIC( curKeys[2], aa.axis.z, 0.333f, 0.333f, 0.f, 0.f ); \
	CUBIC( curKeys[3], degAngle, 0.333f, 0.333f, 0.f, 0.f ); \
}
			
		int controllerType = controller->ClassID().PartA();
		switch (controllerType)
		{
		case LININTERP_FLOAT_CLASS_ID:
			PROCESS_KEYS(1, ILinFloatKey, FUDaeInterpolation::LINEAR)
				ADD_KEY(0, k.val);
			PROCESS_KEYS_END

		case HYBRIDINTERP_FLOAT_CLASS_ID:
			PROCESS_KEYS(1, IBezFloatKey, cubicInterpolation)
				interp = PatchBezierKey(k, 0.0f, 0.333f, previousTime, nextTime, cubicInterpolation);
				ADD_KEY(0, k.val);
				CUBIC( curKeys[0], k.val, k.inLength, k.outLength, k.intan, k.outtan );
			PROCESS_KEYS_END

		case LININTERP_POSITION_CLASS_ID:
			PROCESS_KEYS(3, ILinPoint3Key, FUDaeInterpolation::LINEAR)
				ADD_KEY(0, k.val.x); ADD_KEY(1, k.val.y); ADD_KEY(2, k.val.z);
			PROCESS_KEYS_END

		case LININTERP_ROTATION_CLASS_ID: {
			float pval[3];
			PROCESS_KEYS(3, ILinRotKey, FUDaeInterpolation::LINEAR)

				float v[3];
				k.val.GetEuler(&v[0], &v[1], &v[2]);

				if (i > 0) FSConvert::PatchEuler(pval, v);
				pval[0] = v[0]; pval[1] = v[1]; pval[2] = v[2];

				ADD_KEY(0, v[0]); ADD_KEY(1, v[1]); ADD_KEY(2, v[2]);

			PROCESS_KEYS_END }

		case LININTERP_SCALE_CLASS_ID:
			if (type == ColladaMaxAnimatable::SCALE)
			{
				PROCESS_KEYS(3, ILinScaleKey, FUDaeInterpolation::LINEAR)
					ADD_KEY(0, k.val.s.x); ADD_KEY(1, k.val.s.y); ADD_KEY(2, k.val.s.z);
				PROCESS_KEYS_END
			}
			else if (type == ColladaMaxAnimatable::SCALE_ROT_AXIS)
			{
				PROCESS_KEYS(4, ILinScaleKey, FUDaeInterpolation::LINEAR)
					AngAxis aa(k.val.q);
					if (aa.axis.Equals(Point3::Origin, TOLERANCE)) aa.axis = Point3::XAxis;
					ADD_KEY(0, aa.axis.x); ADD_KEY(1, aa.axis.y); ADD_KEY(2, aa.axis.z); ADD_KEY(3, FMath::RadToDeg(aa.angle));
				PROCESS_KEYS_END
			}
			else if (type == ColladaMaxAnimatable::SCALE_ROT_AXIS_R)
			{
				PROCESS_KEYS(4, ILinScaleKey, FUDaeInterpolation::LINEAR)
					AngAxis aa(k.val.q);
					if (aa.axis.Equals(Point3::Origin, TOLERANCE)) aa.axis = Point3::XAxis;
					ADD_KEY(0, aa.axis.x); ADD_KEY(1, aa.axis.y); ADD_KEY(2, aa.axis.z); ADD_KEY(3, -FMath::RadToDeg(aa.angle));
				PROCESS_KEYS_END
			}
			break;

		case TCBINTERP_FLOAT_CLASS_ID:
			PROCESS_KEYS(1, ITCBFloatKey, FUDaeInterpolation::TCB)
				ADD_KEY(0, k.val);
				TCBPARAM(curKeys[0], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
			PROCESS_KEYS_END

		case TCBINTERP_POSITION_CLASS_ID:
		case TCBINTERP_POINT3_CLASS_ID:
			PROCESS_KEYS(3, ITCBPoint3Key, FUDaeInterpolation::TCB)
				ADD_KEY(0, k.val.x); 
				ADD_KEY(1, k.val.y); 
				ADD_KEY(2, k.val.z);
				TCBPARAM(curKeys[0], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
				TCBPARAM(curKeys[1], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
				TCBPARAM(curKeys[2], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
			PROCESS_KEYS_END

		case TCBINTERP_POINT4_CLASS_ID:
			PROCESS_KEYS(4, ITCBPoint4Key, FUDaeInterpolation::TCB)
				ADD_KEY(0, k.val.x); 
				ADD_KEY(1, k.val.y); 
				ADD_KEY(2, k.val.z);
				ADD_KEY(2, k.val.w);
				TCBPARAM(curKeys[0], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
				TCBPARAM(curKeys[1], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
				TCBPARAM(curKeys[2], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
				TCBPARAM(curKeys[3], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
			PROCESS_KEYS_END

		case TCBINTERP_ROTATION_CLASS_ID:
			float pval[3];
			PROCESS_KEYS(3, ITCBRotKey, FUDaeInterpolation::TCB)
				float v[3];
				Quat quat(k.val);
				quat.GetEuler(&v[0], &v[1], &v[2]);
				if (i > 0) FSConvert::PatchEuler(pval, v);
				pval[0] = v[0]; pval[1] = v[1]; pval[2] = v[2];
			
				ADD_KEY(0, v[0]); ADD_KEY(1, v[1]); ADD_KEY(2, v[2]);
				TCBPARAM(curKeys[1], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
				TCBPARAM(curKeys[2], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
				TCBPARAM(curKeys[3], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
			PROCESS_KEYS_END
		
		case TCBINTERP_SCALE_CLASS_ID:
			if (type == ColladaMaxAnimatable::SCALE)
			{
				PROCESS_KEYS(3, ITCBScaleKey, FUDaeInterpolation::TCB)
					ADD_KEY(0, k.val.s.x); ADD_KEY(1, k.val.s.y); ADD_KEY(2, k.val.s.z);
					TCBPARAM(curKeys[0], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
					TCBPARAM(curKeys[1], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
					TCBPARAM(curKeys[2], k.tens, k.cont, k.bias, k.easeIn, k.easeOut);
				PROCESS_KEYS_END
			}
			else if (type == ColladaMaxAnimatable::SCALE_ROT_AXIS)
			{
				PROCESS_KEYS(4, ITCBScaleKey, FUDaeInterpolation::TCB)
					AngAxis aa(k.val.q);
					if (aa.axis.Equals(Point3::Origin, TOLERANCE)) aa.axis = Point3::XAxis;
					ADD_KEY(0, aa.axis.x); ADD_KEY(1, aa.axis.y); ADD_KEY(2, aa.axis.z); ADD_KEY(3, FMath::RadToDeg(aa.angle));
					TCBPARAM(curKeys[0], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
					TCBPARAM(curKeys[1], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
					TCBPARAM(curKeys[2], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
					TCBPARAM(curKeys[3], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
				PROCESS_KEYS_END
			}
			else if (type == ColladaMaxAnimatable::SCALE_ROT_AXIS_R)
			{
				PROCESS_KEYS(4, ITCBScaleKey, FUDaeInterpolation::TCB)
					AngAxis aa(k.val.q);
					if (aa.axis.Equals(Point3::Origin, TOLERANCE)) aa.axis = Point3::XAxis;
					ADD_KEY(0, aa.axis.x); ADD_KEY(1, aa.axis.y); ADD_KEY(2, aa.axis.z); ADD_KEY(3, -FMath::RadToDeg(aa.angle));
					TCBPARAM(curKeys[0], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
					TCBPARAM(curKeys[1], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
					TCBPARAM(curKeys[2], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
					TCBPARAM(curKeys[3], 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
				PROCESS_KEYS_END
			}
			break;

		case HYBRIDINTERP_POSITION_CLASS_ID:
		case HYBRIDINTERP_POINT3_CLASS_ID:
		case HYBRIDINTERP_COLOR_CLASS_ID:
			PROCESS_KEYS(3, IBezPoint3Key, cubicInterpolation)
				interp = PatchBezierKey(k, Point3::Origin, Point3(0.333, 0.333, 0.333), previousTime, nextTime, cubicInterpolation );
				ADD_KEY(0, k.val.x); ADD_KEY(1, k.val.y); ADD_KEY(2, k.val.z);
				CUBIC( curKeys[0], k.val.x, k.inLength.x, k.outLength.x, k.intan.x, k.outtan.x );
				CUBIC( curKeys[1], k.val.y, k.inLength.y, k.outLength.y, k.intan.y, k.outtan.y );
				CUBIC( curKeys[2], k.val.z, k.inLength.z, k.outLength.z, k.intan.z, k.outtan.z );
			PROCESS_KEYS_END

		case HYBRIDINTERP_POINT4_CLASS_ID:
		case HYBRIDINTERP_FRGBA_CLASS_ID:
			PROCESS_KEYS(4, IBezPoint4Key, cubicInterpolation)
				interp = PatchBezierKey(k, Point4::Origin, Point4(0.333f, 0.333f, 0.333f, 0.333f), previousTime, nextTime, cubicInterpolation);
				ADD_KEY(0, k.val.x); ADD_KEY(1, k.val.y); ADD_KEY(2, k.val.z); ADD_KEY(3, k.val.w);
				CUBIC( curKeys[0], k.val.x, k.inLength.x, k.outLength.x, k.intan.x, k.outtan.x );
				CUBIC( curKeys[1], k.val.y, k.inLength.y, k.outLength.y, k.intan.y, k.outtan.y );
				CUBIC( curKeys[2], k.val.z, k.inLength.z, k.outLength.z, k.intan.z, k.outtan.z );
				CUBIC( curKeys[3], k.val.w, k.inLength.w, k.outLength.w, k.intan.w, k.outtan.w );
			PROCESS_KEYS_END

		case HYBRIDINTERP_ROTATION_CLASS_ID: {
			float pval[3];
			PROCESS_KEYS(3, IBezQuatKey, cubicInterpolation)
				float v[3];
				k.val.GetEuler(&v[0], &v[1], &v[2]);

				if (i > 0) FSConvert::PatchEuler(pval, v);
				pval[0] = v[0]; pval[1] = v[1]; pval[2] = v[2];
			
				ADD_KEY(0, v[0]); ADD_KEY(1, v[1]); ADD_KEY(2, v[2]);

				CUBIC( curKeys[0], v[0], 0.333f, 0.333f, 0.f, 0.f );
				CUBIC( curKeys[1], v[1], 0.333f, 0.333f, 0.f, 0.f );
				CUBIC( curKeys[2], v[2], 0.333f, 0.333f, 0.f, 0.f );

			PROCESS_KEYS_END }

		case HYBRIDINTERP_SCALE_CLASS_ID:
			if (type == ColladaMaxAnimatable::SCALE)
			{
				PROCESS_KEYS(3, IBezScaleKey, cubicInterpolation)
					interp = PatchBezierKey(k, Point3::Origin, Point3(0.333f, 0.333f, 0.333f), previousTime, nextTime, cubicInterpolation);
					ADD_KEY(0, k.val.s.x); ADD_KEY(1, k.val.s.y); ADD_KEY(2, k.val.s.z);
					CUBIC( curKeys[0], k.val.s.x, k.inLength.x, k.outLength.x, k.intan.x, k.outtan.x );	
					CUBIC( curKeys[1], k.val.s.y, k.inLength.y, k.outLength.y, k.intan.y, k.outtan.y );	
					CUBIC( curKeys[2], k.val.s.z, k.inLength.z, k.outLength.z, k.intan.z, k.outtan.z );	
				PROCESS_KEYS_END
			}
			else if (type == ColladaMaxAnimatable::SCALE_ROT_AXIS)
			{
				PROCESS_KEYS(4, IBezScaleKey, cubicInterpolation)
					PROCESS_SCALE_ROT_AXIS( + );
				PROCESS_KEYS_END
			}
			else if (type == ColladaMaxAnimatable::SCALE_ROT_AXIS_R)
			{
				PROCESS_KEYS(4, IBezScaleKey, cubicInterpolation)
					PROCESS_SCALE_ROT_AXIS( - );
				PROCESS_KEYS_END
			}
			break;

		case FLOATWAVE_CONTROL_CLASS_ID:
		default:
			isSampling = true;
			break;
		}
	}

#undef PROCESS_KEYS
#undef VAL
#undef PROCESS_KEYS_END
#undef PROCESS_SCALE_ROT_AXIS

	if (isSampling)
	{
		// Reserve the necessary memory and generate the key times
		FUDaeInterpolation::Interpolation interp = GetControlInterpolation( controller, FUDaeInterpolation::LINEAR );

		TimeMappingList keyTimeMappingList;
		GetAnimatableTimeMappingList( controller, keyTimeMappingList, CTRL->FindConstraint( controller ) ? GATMLF_USE_SECONDARY_SAMPLE_RATE : GATMLF_NONE );

#define PROCESS_KEYS(expectedDimensions, valueClassName) { \
			if (expectedDimensions > dimensions) break; \
			valueClassName value; \
			size_t i = 0; \
			for ( TimeMappingList::iterator ticksToSecsMapIter = keyTimeMappingList.begin(); ticksToSecsMapIter != keyTimeMappingList.end(); ++ticksToSecsMapIter ) \
			{ \
				const TimeValue &t = ticksToSecsMapIter->m_ticks; \
				const float &tSecs = ticksToSecsMapIter->m_secs; \
				controller->GetValue(t, &value, FOREVER, CTRL_ABSOLUTE);
#define PROCESS_KEYS_END ++i; } } break;

		int sclassID = controller->SuperClassID();
		switch (sclassID)
		{
		case CTRL_FLOAT_CLASS_ID:
			PROCESS_KEYS(1, float)
				ADD_KEY(0, value);
			PROCESS_KEYS_END

		case CTRL_POINT3_CLASS_ID:
		case CTRL_POSITION_CLASS_ID:
			PROCESS_KEYS(3, Point3)
				ADD_KEY(0, value.x); ADD_KEY(1, value.y); ADD_KEY(2, value.z);
			PROCESS_KEYS_END;

		case CTRL_POINT4_CLASS_ID:
		case CTRL_FRGBA_CLASS_ID:
			PROCESS_KEYS(4, Point4)
				ADD_KEY(0, value.x); ADD_KEY(1, value.y); ADD_KEY(2, value.z); ADD_KEY(3, value.w);
			PROCESS_KEYS_END;

		case CTRL_ROTATION_CLASS_ID :{
			float pval[3];
			PROCESS_KEYS(3, AngAxis)
				Quat q(value);
				float v[3];
				q.GetEuler(&v[0], &v[1], &v[2]);

				if (i > 0) FSConvert::PatchEuler(pval, v);
				pval[0] = v[0]; pval[1] = v[1]; pval[2] = v[2];

				ADD_KEY(0, v[0]); ADD_KEY(1, v[1]); ADD_KEY(2, v[2]);
			PROCESS_KEYS_END }

		case CTRL_SCALE_CLASS_ID:
			if (type == ColladaMaxAnimatable::SCALE)
			{
				PROCESS_KEYS(3, ScaleValue)
					ADD_KEY(0, value.s.x); ADD_KEY(1, value.s.y); ADD_KEY(2, value.s.z);
				PROCESS_KEYS_END
			}
			else if (type == ColladaMaxAnimatable::SCALE_ROT_AXIS)
			{
				PROCESS_KEYS(4, ScaleValue)
					AngAxis aa(value.q);
					if (aa.axis.Equals(Point3::Origin, TOLERANCE)) aa.axis = Point3::XAxis;
					ADD_KEY(0, aa.axis.x); ADD_KEY(1, aa.axis.y); ADD_KEY(2, aa.axis.z); ADD_KEY(3, FMath::RadToDeg(aa.angle));
				PROCESS_KEYS_END
			}
			else if (type == ColladaMaxAnimatable::SCALE_ROT_AXIS_R)
			{
				PROCESS_KEYS(4, ScaleValue)
					AngAxis aa(value.q);
					if (aa.axis.Equals(Point3::Origin, TOLERANCE)) aa.axis = Point3::XAxis;
					ADD_KEY(0, aa.axis.x); ADD_KEY(1, aa.axis.y); ADD_KEY(2, aa.axis.z); ADD_KEY(3, -FMath::RadToDeg(aa.angle));
				PROCESS_KEYS_END
			}
			break;
		default: break;
		}

#undef PROCESS_KEYS
#undef PROCESS_KEYS_END

	}

#undef ADD_KEY

	bool bForceAnimExport = IS_FORCE_ANIM_EXPORT( controller );

	// Add keys to curve and validate
	for ( int curveId = 0; curveId < dimensions; curveId++ )
	{
		AddAnimationKeysToCurve( keyBuffers[curveId], curves[curveId], bOneChannelPerCurve, isSampling );

		int curveKeyCount = curves[curveId]->GetKeyCount();
		if ( isSampling 
			|| type == ColladaMaxAnimatable::SCALE_ROT_AXIS_R 
			|| type == ColladaMaxAnimatable::SCALE_ROT_AXIS 
			|| curveKeyCount <= 0 )
		{
			bool bValidCurve = ValidateAnimationCurve( curves[curveId], isSampling );
			if ( !bValidCurve && !bForceAnimExport )
			{
				SAFE_RELEASE( curves[curveId] );
			}
		}
	}

	// Delete key buffers
	SAFE_DELETE_ARRAY( keyBuffers );

	// apply conversion functor to valid curves
	bool anyCurvesLeft = false;
	for (size_t i = 0; i < dimensions; ++i)
	{
		if ( curves[i] )
		{
			FCDAnimationCurve* curve = curves[i];
			if (conversion != NULL)
			{
				curve->ConvertValues(conversion, conversion);
			}
			if (isRelativeAnimation && i < maxInitialValues && !IsEquivalent(initialValue[i], 0.0f))
			{
				FCDConversionOffsetFunctor functor(-initialValue[i]);
				curve->ConvertValues(&functor, &functor);
			}
			anyCurvesLeft = true;
		}
	}

	return anyCurvesLeft;
}

#undef INTAN
#undef OUTTAN
#undef TCBPARAM

#undef F
#undef IN_INT
#undef OUT_INT
#undef INTAN_X
#undef OUTTAN_X
#undef TAN_Y
#undef BEZIER
#undef HERMITE
#undef CUBIC

/** IParamBlock ExportProperty methods */

float AnimationExporter::GetParameterAsFloat( IParamBlock* parameters, int parameterId, FCDConversionFunctor* conversion )
{
	float colladaValue = 0.f;

	if (parameters == NULL)
	{	
		return colladaValue;
	}

	ParamType type = parameters->GetParameterType(parameterId); // support both int and floats here.
	if (type == TYPE_INT || 
		type == TYPE_TIMEVALUE || 
		type == TYPE_BOOL ||
		type == TYPE_INDEX) 
	{
		colladaValue = (float) parameters->GetInt(parameterId, TIME_EXPORT_START);
	}
	else if (type == TYPE_FLOAT || 
			 type == TYPE_PCNT_FRAC || 
			 type == TYPE_WORLD || 
			 type == TYPE_ANGLE ||
			 type == TYPE_COLOR_CHANNEL) 
	{
		colladaValue = parameters->GetFloat(parameterId, TIME_EXPORT_START);
	}
	else
	{
		FUFail( return colladaValue );
	}

	if (conversion != NULL)
	{
		colladaValue = (*conversion)(colladaValue);
	}

	return colladaValue;
}

void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock* parameters, int parameterId, FCDParameterAnimatableFloat& colladaValue, FCDConversionFunctor* conversion)
{
	if (parameters == NULL)
	{	
		return;
	}

	colladaValue = GetParameterAsFloat( parameters, parameterId, conversion );
	ExportProperty(id, parameters, parameterId, colladaValue.GetAnimated(), conversion);
}

void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock* parameters, int parameterIdA, int parameterIdB, int parameterIdC, FCDParameterAnimatableVector3& colladaValue, FCDConversionFunctor* conversion)
{
	if (parameters == NULL)
	{	
		return;
	}

	FMVector3 v;
	v[0] = GetParameterAsFloat( parameters, parameterIdA, conversion );
	v[1] = GetParameterAsFloat( parameters, parameterIdB, conversion );
	v[2] = GetParameterAsFloat( parameters, parameterIdC, conversion );

	colladaValue = v;
	if ( OPTS->ExportAnimations() )
	{
		ExportAnimatedPoint3( 
			id
			, parameters->GetController((ParamID)parameterIdA)
			, parameters->GetController((ParamID)parameterIdB)
			, parameters->GetController((ParamID)parameterIdC)
			, colladaValue.GetAnimated()
			, conversion 
			);
	}
}

void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock* parameters, int parameterId, FCDAnimated* colladaAnimated, FCDConversionFunctor* conversion)
{
	if (OPTS->ExportAnimations())
	{
		Control* controller = parameters->GetController((ParamID)parameterId);
		ExportAnimatedFloat(id, controller, colladaAnimated, conversion);
	}
}

void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock* parameters, int parameterId, FCDParameterAnimatableVector3& colladaValue, FCDConversionFunctor* conversion)
{
	if (parameters == NULL) return;
	ParamType type = parameters->GetParameterType(parameterId);
	switch (type)
	{
	case TYPE_RGBA: {
		Color c = parameters->GetColor(parameterId, TIME_EXPORT_START);
		if (conversion != NULL)
		{
			c.r = (*conversion)(c.r);
			c.g = (*conversion)(c.g);
			c.b = (*conversion)(c.b);
		}
		colladaValue = ToFMVector3(c);
		break; }

	case TYPE_POINT3: {
		Point3 p = parameters->GetPoint3(parameterId, TIME_EXPORT_START);
		if (conversion != NULL)
		{
			p.x = (*conversion)(p.x);
			p.y = (*conversion)(p.y);
			p.z = (*conversion)(p.z);
		}
		colladaValue = ToFMVector3(p);
		break; }

	default:
		// should never happen
		FUFail(return);
	}

	if (OPTS->ExportAnimations())
	{
		Control* controller = parameters->GetController((ParamID)parameterId);
		ExportAnimatedPoint3(id, controller, colladaValue, -1, conversion);
	}
}

void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock* parameters, int parameterId, FCDParameterAnimatableVector4& colladaValue, FCDConversionFunctor* conversion)
{
	if (parameters == NULL) return;
	colladaValue->w = 1.0f;
	ExportProperty(id, parameters, parameterId, (FCDParameterAnimatableVector3&) colladaValue, conversion);
}

/** Export named parameter, for cases where we are unsure of the structure of the parameter block */

void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock2* parameters, TSTR name, int tabIndex, FCDParameterAnimatableFloat& colladaValue, FCDConversionFunctor* conversion)
{
	for (int i = 0; i < parameters->NumParams(); ++i)
	{
		ParamID parameterId = parameters->IndextoID(i);
		TSTR localName = parameters->GetLocalName(parameterId);
		if (localName == name)
		{
			ExportProperty(id, parameters, parameterId, tabIndex, colladaValue, conversion);
			return;
		}
	}
}


void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock2* parameters, TSTR name, int tabIndex, FCDParameterAnimatableVector3& colladaValue, FCDConversionFunctor* conversion)
{
	for (int i = 0; i < parameters->NumParams(); ++i)
	{
		ParamID parameterId = parameters->IndextoID(i);
		if (parameters->GetLocalName(parameterId) == name)
		{
			ExportProperty(id, parameters, parameterId, tabIndex, colladaValue, conversion);
		}
	}
}


void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock2* parameters, TSTR name, int tabIndex, FCDParameterAnimatableVector4& colladaValue, FCDConversionFunctor* conversion)
{
	for (int i = 0; i < parameters->NumParams(); ++i)
	{
		ParamID parameterId = parameters->IndextoID(i);
		TSTR localName = parameters->GetLocalName(parameterId);
		if (localName == name)
		{
			ExportProperty(id, parameters, parameterId, tabIndex, colladaValue, conversion);
			return;
		}
	}
}

/** IParamBlock2 ID-based methods */

void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock2* parameters, int parameterId, int tabIndex, FCDParameterAnimatableFloat& colladaValue, FCDConversionFunctor* conversion)
{
	if (parameters == NULL) return;
	FUAssert(parameters->IDtoIndex(parameterId) >= 0, return);

	Interval dummy;
	ParamType2 type = parameters->GetParameterType(parameterId); // support both int and floats here.
	FUAssert(tabIndex == 0 || is_tab(type), tabIndex = 0); // Only allow access to proper whatsits here
	type = base_type(type);
	if (type == TYPE_INT || 
		type == TYPE_TIMEVALUE || 
		type == TYPE_BOOL ||
		type == TYPE_INDEX ||
		type == TYPE_RADIOBTN_INDEX ) 
	{
		colladaValue = (float) parameters->GetInt(parameterId, TIME_EXPORT_START, tabIndex);
	}
	else if (type == TYPE_FLOAT || 
			 type == TYPE_PCNT_FRAC || 
			 type == TYPE_WORLD || 
			 type == TYPE_ANGLE ||
			 type == TYPE_COLOR_CHANNEL) 
	{
		colladaValue = parameters->GetFloat(parameterId, TIME_EXPORT_START, tabIndex);
	}
	else
	{
#ifdef _DEBUG
		ParamDef& pd = parameters->GetParamDef(parameterId);
		pd;
#endif
		colladaValue = 0.0f;
		FUFail(return);
	}
	if (conversion != NULL) colladaValue = (*conversion)(colladaValue);
	if (OPTS->ExportAnimations())
	{
		Control* controller = parameters->GetController((ParamID)parameterId);
		ExportAnimatedFloat(id, controller, colladaValue, -1, conversion);
	}
}


void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock2* parameters, int parameterId, int tabIndex, FCDParameterAnimatableVector3& colladaValue, FCDConversionFunctor* conversion)
{
	if (parameters == NULL) return;
	FUAssert(parameters->IDtoIndex(parameterId) >= 0, return);

	int type = parameters->GetParameterType(parameterId); // (ParamType2)
	int baseType = base_type(type);
	// If we have a tab index, make sure that our type was a TAB type.
	if (tabIndex != 0) FUAssert(baseType == type, tabIndex = 0);
	switch (baseType)
	{
	case TYPE_RGBA:
	case TYPE_FRGBA:
	case TYPE_POINT4: {
		Point4 c = parameters->GetPoint4(parameterId, TIME_EXPORT_START, tabIndex);
		if (conversion != NULL)
		{
			c.x = (*conversion)(c.x);
			c.y = (*conversion)(c.y);
			c.z = (*conversion)(c.z);
			c.w = (*conversion)(c.w);
		}
		colladaValue = ToFMVector3(c);
		break; }

	case TYPE_POINT3: {
		Point3 p = parameters->GetPoint3(parameterId, TIME_EXPORT_START, tabIndex);
		if (conversion != NULL)
		{
			p.x = (*conversion)(p.x);
			p.y = (*conversion)(p.y);
			p.z = (*conversion)(p.z);
		}
		colladaValue = ToFMVector3(p);
		break; }
	default:
		FUFail(return);
	}

	if (OPTS->ExportAnimations())
	{
		Control* controller = parameters->GetController((ParamID)parameterId);
		ExportAnimatedPoint3(id, controller, colladaValue, -1, conversion);
	}
}

void AnimationExporter::ExportProperty(const fm::string& id, IParamBlock2* parameters, int parameterId, int tabIndex, FCDParameterAnimatableVector4& colladaValue, FCDConversionFunctor* conversion)
{
	if (parameters == NULL) return;
	FUAssert(parameters->IDtoIndex(parameterId) >= 0, return);

	int type = parameters->GetParameterType(parameterId); // (ParamType2)
	int baseType = base_type(type);
	// If we have a tab index, make sure that our type was a TAB type.
	if (tabIndex != 0) FUAssert(baseType == type, tabIndex = 0);
	switch (baseType)
	{
	case TYPE_RGBA: {
		Color c = parameters->GetColor(parameterId, TIME_EXPORT_START, tabIndex);
		if (conversion != NULL)
		{
			c.r = (*conversion)(c.r);
			c.g = (*conversion)(c.g);
			c.b = (*conversion)(c.b);
		}
		colladaValue = ToFMVector4(c);
		break; }

	case TYPE_FRGBA: 
	case TYPE_POINT4: {
		Point4 c = parameters->GetPoint4(parameterId, TIME_EXPORT_START, tabIndex);
		if (conversion != NULL)
		{
			c.x = (*conversion)(c.x);
			c.y = (*conversion)(c.y);
			c.z = (*conversion)(c.z);
			c.w = (*conversion)(c.w);
		}
		colladaValue = ToFMVector4(c);
		break; }

	case TYPE_POINT3: {
		Point3 p = parameters->GetPoint3(parameterId, TIME_EXPORT_START, tabIndex);
		if (conversion != NULL)
		{
			p.x = (*conversion)(p.x);
			p.y = (*conversion)(p.y);
			p.z = (*conversion)(p.z);
		}
		colladaValue = ToFMVector4(p);
		break; }
	default:
#ifdef _DEBUG
		ParamDef& pd = parameters->GetParamDef(parameterId);
		pd = pd; // get rid of warning message
#endif
		FUFail(return);
	}

	if (OPTS->ExportAnimations())
	{
		
		Control* controller = parameters->GetController((ParamID)parameterId);
		if (baseType == TYPE_POINT3 || baseType == TYPE_RGBA)
			ExportAnimatedPoint3(id, controller, (FCDParameterAnimatableVector3&) colladaValue, -1, conversion, false, baseType );
		else
			ExportAnimatedPoint4(id, controller, colladaValue, -1, conversion);
	}
}

void AnimationExporter::ExportEvents()
{
	if ( !OPTS->ExportAnimations() )
	{
		return;
	}

	int numEventGroups = g_animationSelector.GetNumEventGroups();
	bool bValidEventGroups = false;

	// export only if at least one valid event
	for ( size_t eventGroupId = 0; eventGroupId < numEventGroups; ++eventGroupId )
	{
		const FCDEventGroup<int> &eventGroup = g_animationSelector.GetEventGroup( eventGroupId );
		size_t numEvents = eventGroup.GetNumEvents();
		for ( size_t eventId = 0; eventId < numEvents; ++eventId )
		{
			if ( eventGroup.GetEvent( eventId ).length() > 0 )
			{
				bValidEventGroups = true;
				break;
			}
		}

		if ( bValidEventGroups )
		{
			break;
		}
	}

	if ( !bValidEventGroups )
	{
		return;
	}

	ClipList animationClips = GetAnimationClips( NULL );
	VALIDATE_ANIM_CLIPS( animationClips, return );
	Clip** it = animationClips.begin();
	FCDAnimationCurveTrackList addedCurves;

	FCDAnimation* animation = CreateAnimation( *it, fm::string( DAE_EVENTS_ANIMATION ).c_str() );
	FCDAnimationChannel* channel = animation->AddChannel();
	FCDAnimationCurve* curve = CreateCurve( channel, NULL );
	addedCurves.push_back( curve );

	// add event animations
	FUStringBuilder eventGroupStringBuilder;

	for ( size_t eventGroupId = 0; eventGroupId < numEventGroups; ++eventGroupId )
	{
		const FCDEventGroup<int> &eventGroup = g_animationSelector.GetEventGroup( eventGroupId );

		int tpf = GetTicksPerFrame();
		int timeFrames = eventGroup.GetTime();
		float timeSecs = ( timeFrames * tpf ) / ( float )TIME_TICKSPERSEC;

		eventGroupStringBuilder.clear();
		
		size_t numEvents = eventGroup.GetNumEvents();
		for ( size_t eventId = 0; eventId < numEvents; ++eventId )
		{
			eventGroupStringBuilder.append( eventGroup.GetEvent( eventId ) );
			
			if ( eventId < numEvents - 1 )
			{
				eventGroupStringBuilder.append( ';' );
			}
		}

		FCDAnimationKeyFString *pKey = new FCDAnimationKeyFString();
		pKey->input = timeSecs;
		pKey->outputString = eventGroupStringBuilder.ToCharPtr();
		pKey->interpolation = FUDaeInterpolation::STEP;
		curve->AddKey( pKey );
	}

	AddCurvesToClips( addedCurves, it, animationClips.end() );
	curve->SetFStringAnimationCurve( true );
	curve->SetFStringAnimationOutputParameterName( fstring( DAE_EVENTS_ANIMATION ).ToUpper() );
}

void AnimationExporter::RemoveCurveFromClips( FCDAnimationCurve *curve, Clip **dstStartIter, Clip **dstEndIter )
{
	for ( Clip **clipIt = dstStartIter; clipIt != dstEndIter; ++clipIt )
	{
		if ((*clipIt)->colladaClip != NULL)
		{
			(*clipIt)->colladaClip->RemoveClipCurve( curve, false );
		}
	}
}
void AnimationExporter::AddCurvesToClips( FCDAnimationCurveTrackList &curves, Clip **dstStartIter, Clip **dstEndIter )
{
	for ( Clip **clipIt = dstStartIter; clipIt != dstEndIter; ++clipIt )
	{
		if ((*clipIt)->colladaClip != NULL)
		{
			for ( FCDAnimationCurveTrackList::iterator curveIt = curves.begin(); curveIt != curves.end(); curveIt++ )
			{
				(*clipIt)->colladaClip->AddClipCurve( *curveIt );
			}
		}
	}
}

//void AnimationExporter::PostExportAnimatedEnum()
//{
//	// convert all keys to string keys
//	FCDAnimated *pAnimated = colladaValue.GetAnimated();
//	size_t valueCount = pAnimated->GetValueCount();
//	for ( size_t valueId = 0; valueId < valueCount; ++valueId )
//	{
//		size_t curveCount = pAnimated->GetCurveCount( valueId );
//		for ( size_t curveId = 0; curveId < curveCount; ++curveId )
//		{
//			FCDAnimationCurve *pCurve = pAnimated->GetCurve( valueId, curveId );
//			pCurve->ConvertValuesToStrings( pCurve, pExportBehavior );
//		}
//	}
//}

//void AnimationExporter::ExportAnimatedEnum(const fm::string& id, IParamBlock2* parameters, int parameterId, int tabIndex, FCDParameterAnimatableFloat& colladaValue, FCDExportBehavior__T_CONVERT_ENUM_TO_STRING *pExportBehavior, FCDConversionFunctor* conversion )
//{
//	ExportProperty( id, parameters, parameterId, tabIndex, colladaValue, conversion );
//
//	if ( !colladaValue.IsAnimated() )
//	{
//		return;
//	}
//
//
//	PostExportAnimatedEnum();
//}
//
//void AnimationExporter::ExportAnimatedEnum(const fm::string& id, IParamBlock2* parameters, int parameterId, int tabIndex, FCDParameterAnimatableColor4& colladaValue, FCDExportBehavior__T_CONVERT_ENUM_TO_STRING *pExportBehavior, FCDConversionFunctor* conversion )
//{
//	ExportProperty( id, parameters, parameterId, tabIndex, colladaValue, conversion );
//	PostExportAnimatedEnum();
//
//	if ( !colladaValue.IsAnimated() )
//	{
//		return;
//	}
//	
//	// convert all keys to string keys
//	FCDAnimated *pAnimated = colladaValue.GetAnimated();
//	size_t valueCount = pAnimated->GetValueCount();
//	for ( size_t valueId = 0; valueId < valueCount; ++valueId )
//	{
//		size_t curveCount = pAnimated->GetCurveCount( valueId );
//		for ( size_t curveId = 0; curveId < curveCount; ++curveId )
//		{
//			FCDAnimationCurve *pCurve = pAnimated->GetCurve( valueId, curveId );
//			pCurve->ConvertValuesToStrings( pCurve, pExportBehavior );
//		}
//	}
//}


#undef GET_TM_CONTROLLER_SAMPLING_INTERPOLATION
#undef ADD_ANIMATION_KEY
#undef ADD_ANIMATION_KEY_PTR
#undef VALIDATE_ANIM_KEY_OPTIMIZATION
#undef IS_FORCE_ANIM_EXPORT
#undef TRANSLATION_ANIM
#undef ROTATION_ANIM
#undef SCALE_ANIM
#undef NUM_ANIMATIONS
#undef EULER_ROTATION_ANGLE_ID
#undef TRANSLATION_NUM_CURVES
#undef SCALE_NUM_CURVES
#undef EULER_ROT_NUM_CURVES
#undef ANGLE_AXIS_ROT_NUM_CURVES
#undef IS_EULER_ROT_ANIM
#undef IS_TRANSFORM_ANIM_ONE_CHANNEL_PER_CURVE
#undef GET_NODE_TM
#undef GET_ROTATION_FROM_AFFINE_PARTS
#undef DECOMPOSED_ANIMATED_TRANSFORM_PRE_EXPORT