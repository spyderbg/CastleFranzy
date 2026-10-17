#ifndef __CCAMERATARGETTRACKERSCENENODE_H__
#define __CCAMERATARGETTRACKERSCENENODE_H__

#include "CCameraSceneNode.h"

namespace irr
{
namespace scene
{

class CCameraTargetTrackerSceneNode
	: public CCameraSceneNode
{
public:
	CCameraTargetTrackerSceneNode(const ISceneNode * lookat = 0,
		const core::vector3df& position = core::vector3df(0, 0, 0),
		s32 id = -1)
	: CCameraSceneNode(id, position)
	, m_pTarget(0)
	{
		setTarget(lookat);
	}

	~CCameraTargetTrackerSceneNode() 
	{
		setTarget(0);
	}

	//! Returns type of the scene node
	virtual ESCENE_NODE_TYPE getType() const { return ESNT_CAMERA_TARGET; }

	virtual const ISceneNode* getTargetNode() const { return m_pTarget; }
	virtual ISceneNode* getTargetNode()  { return const_cast<ISceneNode*>(m_pTarget); }
	
	void setTarget(const ISceneNode *pTarget)
	{
		if(m_pTarget)
		{
			m_pTarget->drop();
		}

		m_pTarget = pTarget;

		if(m_pTarget)
		{
			m_pTarget->grab();
		}
	}

	virtual void render(void* renderData);

protected:
	const ISceneNode *m_pTarget;

}; // class CCameraTagetTrackerSceneNode
}; // namespace scene
}; // namespace irr

#endif
