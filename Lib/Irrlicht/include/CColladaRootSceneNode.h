#ifndef __CCOLLADAROOTSCENENODE_H__
#define __CCOLLADAROOTSCENENODE_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaRootSceneNode.h"
#include "CEmptySceneNode.h"
#include "CColladaCameraSceneNode.h"
#include "ISceneManager.h"
#include "CColladaSceneNode.h"

namespace irr
{
namespace collada
{

class CRootSceneNode
	: public IRootSceneNode
	, public CSceneNode
{
public:
	enum E_AnimateContition
	{
		E_ONLY_IF_VISIBLE,
		E_ALWAYS,
	};

protected:
	bool					m_bIsPostLoadDone;
	int						m_iAnimateContition;
	u32						m_iLastOnAnimateTime;
	irr::core::aabbox3df	m_OriginalBoundingBox;

public:

	CRootSceneNode(const CColladaDatabase &database)
		: IRootSceneNode(database)
		, CSceneNode(database)
		, m_iAnimateContition(E_ALWAYS)
		, m_bIsPostLoadDone(false)
	{
		#ifdef _DEBUG
		setDebugName("CColladaRootSceneNode");
		#endif
		setAutomaticCulling(irr::scene::EAC_FRUSTUM_BOX);
	}

	virtual const char * getUID() const
	{
		return IRootSceneNode::getCollada().getAbsoluteFilename();
	}

	void setAnimateCondition(E_AnimateContition cond)
	{
		m_iAnimateContition = cond;
	}

	E_AnimateContition getAnimateCondition()
	{
		return (E_AnimateContition)m_iAnimateContition;
	}

	void onPostLoad()
	{
		updateAbsolutePosition(true);
		computeBoundingBox(m_OriginalBoundingBox);
		attachSkin();
		attachCameras();
		attachParticleSystems();
		m_bIsPostLoadDone = true;
	}

	virtual void addSkinnedMesh(scene::CColladaSkinnedMesh *pSkinnedMesh)
	{
		IRootSceneNode::addSkinnedMesh(pSkinnedMesh);
		if(m_bIsPostLoadDone)
		{
			pSkinnedMesh->attach(this);
		}
	}

	virtual void OnAnimate(u32 timeMs)
	{
		if(m_iAnimateContition == E_ALWAYS)
		{
			ISceneNode::OnAnimate(timeMs);
		}
		else
		{
			// update absolute position only
			updateAbsolutePosition();
		}
		m_iLastOnAnimateTime = timeMs;
	}

	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
	{
		CSceneNode::serializeAttributes(out, options);
		out->addString("Source", IRootSceneNode::getCollada().getAbsoluteFilename());
		//IRootSceneNode::serializeAttributes(out, options);
	}

	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
	{
		CSceneNode::deserializeAttributes(in, options);
		//IRootSceneNode::serializeAttributes(in, options);
	}

	virtual void OnRegisterSceneNode()
	{
		if(m_iAnimateContition == E_ONLY_IF_VISIBLE)
		{
			irr::core::aabbox3df transformedBoundingBox = m_OriginalBoundingBox;
			getAbsoluteTransformation().transformBoxEx(transformedBoundingBox);
			if(!SceneManager->isCulled(transformedBoundingBox, scene::EAC_FRUSTUM_BOX))
			{
				ISceneNode::OnAnimate(m_iLastOnAnimateTime);
				ISceneNode::OnRegisterSceneNode();
			}
			else
			{
				
			}
		}
		else
		{
			ISceneNode::OnRegisterSceneNode();
		}
	}



	//! Returns type of the scene node
	virtual scene::ESCENE_NODE_TYPE getType() const { return scene::ESNT_COLLADA_ROOT; }


protected:
	void attachSkin()
	{
		core::list<scene::CColladaSkinnedMesh *>::Iterator skinIt = m_SkinnedMeshs.begin();
		for(; skinIt != m_SkinnedMeshs.end(); skinIt++)
		{
			(*skinIt)->attach(this);
		}
	}

	void attachCameras()
	{
		core::list<CCameraSceneNode *>::Iterator cameraIt = m_Cameras.begin();
		for(; cameraIt != m_Cameras.end(); cameraIt++)
		{
			(*cameraIt)->attach(this);
		}
	}

	void attachParticleSystems()
	{
		core::list<CParticleSystemSceneNode *>::Iterator psIt = m_ParticleSystems.begin();
		for(; psIt != m_ParticleSystems.end(); ++psIt)
		{
			(*psIt)->attach(this);
		}
	}
	
};

}; // namespace collada
}; // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif // __CCOLLADAROOTSCENENODE_H__
