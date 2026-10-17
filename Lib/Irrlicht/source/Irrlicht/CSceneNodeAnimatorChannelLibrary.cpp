#include "stdafx.h"
#include "CSceneNodeAnimatorChannelLibrary.h"
#include "irros.h"
#include "CColladaFileLoader.h"
#include "IXMLReader.h"
#include "IDummyTransformationSceneNode.h"
#include "SAnimatedMesh.h"
#include "fast_atof.h"
#include "quaternion.h"
#include "ILightSceneNode.h"
#include "ICameraSceneNode.h"
#include "IMeshManipulator.h"
#include "IReadFile.h"
#include "IAttributes.h"
#include "IMeshCache.h"
#include "IMeshSceneNode.h"
#include "SMeshBufferLightMap.h"
#include "irrMap.h"
#include "CMemoryReadFile.h"
#include "CMorphingMesh.h"
#include "CSkinnedMesh2.h"
#include "CAnimationTrack.h"
#include "CCameraTargetTrackerSceneNode.h"
#include "irrMath.h"
#include "CSkinnedMeshSceneNode.h"
#include "CTimelineController.h"

using namespace irr;
using namespace scene;

void 
CSceneNodeAnimatorChannelLibrary::BindChannels()
{
	for(int i = 0, sz = m_ChannelAnimators.size(); i < sz; i++)
	{
		BindChannel(m_ChannelAnimators[i]);
	}
}


//! BingChannel connect a channel to a Node based on the URI it look for.
/** This process may be long enough to see a drop in the framerate , 
	you should prebind at load time. 	
*/
void 
CSceneNodeAnimatorChannelLibrary::BindChannel(CChannelAnimator* pChannel)
{
	core::stringc uri = pChannel->getBindURI();
	ISceneNode* depthNode = 0;
	core::stringc depthUri;

	s32 firstSlash = uri.findNext('/', 0);
	if(firstSlash != -1)
	{
		depthUri = uri.subString(0, firstSlash);
		uri = uri.subString(firstSlash + 1, uri.size() - firstSlash - 1);
		depthNode = m_BindTo->getSceneNodeFromUID(depthUri.c_str());
	}

	// Incomplete implementation
	
	if(uri.equalsn("transform", sizeof("transform") - 1) && depthNode) 
	{
		if(uri == "transform")
		{
			pChannel->setType(CChannelAnimator::Transform4x4);
			pChannel->bind(depthNode);
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
			depthNode->setIsAnimated(ESNAF_SCALE, true);
			depthNode->setIsAnimated(ESNAF_ROTATION, true);
			depthNode->setIsAnimated(ESNAF_TRANSLATION, true);
#endif
		}
		else
		{
			pChannel->setType(CChannelAnimator::SingleFieldF32);
			int iBegin = uri.findNext('(', 0);
			int iEnd = uri.findNext(')', iBegin);
			core::stringc sLine = uri.subString(iBegin+1, iEnd - iBegin);
			int iColumn = atoi(sLine.c_str());

			iBegin = uri.findNext('(', iEnd);
			iEnd = uri.findNext(')', iBegin);
			sLine = uri.subString(iBegin+1, iEnd - iBegin);
			int iLine = atoi(sLine.c_str());

			if(iColumn == 3)
			{
				switch(iLine)
				{
				case 0:
					pChannel->bind((void *)&depthNode->getPosition().X);
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
					depthNode->setIsAnimated(ESNAF_TRANSLATION, true);
#endif
					break;
				case 1:
					pChannel->bind((void *)&depthNode->getPosition().Y);
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
					depthNode->setIsAnimated(ESNAF_TRANSLATION, true);
#endif
					break;
				case 2:
					pChannel->bind((void *)&depthNode->getPosition().Z);
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
					depthNode->setIsAnimated(ESNAF_TRANSLATION, true);
#endif
					break;
				}
			}
		}
	}
	
	if(depthNode == 0)
	{
		pChannel->bind(0);
	}
}

ISceneNodeAnimator* CSceneNodeAnimatorChannelLibrary::createClone()
{
	CSceneNodeAnimatorChannelLibrary * newAnimator = 
		irrnew CSceneNodeAnimatorChannelLibrary();
	
	if (TimelineCtrl)
	{
	/*	CTimelineController* controller = irrnew CTimelineController();
		controller->SetAnimationClipsLibrary(((CTimelineController* )pTimelineCtrl)->GetAnimationClipsLibrary());
		newAnimator->SetTimelineCtrl(controller);
		controller->drop();*/
	}

	for(u32 i = 0; i < m_ChannelAnimators.size(); i++)
	{		
		CChannelAnimator *pAnimator = m_ChannelAnimators[i]->createClone();
		newAnimator->addAnimator(pAnimator);
		pAnimator->drop();
	}
		
	return newAnimator;
}
