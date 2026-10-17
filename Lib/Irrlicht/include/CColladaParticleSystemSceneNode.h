#ifndef __C_COLLADA_PARTICLE_SYSTEM_SCENE_NODE_H_INCLUDED__
#define __C_COLLADA_PARTICLE_SYSTEM_SCENE_NODE_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
#include "PSManager.h"
#endif
#include "ISceneNode.h"
#include "CColladaDatabase.h"
#include "IColladaObject.h"

namespace irr
{

namespace ps
{
	class CPSQuadMeshBuffer;
};

namespace collada
{

class CParticleSystemSceneNode 
	: public scene::ISceneNode
	, public IObject
{
public:
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	typedef ps::IParticleSystem<ps::SParticle> ps_type;
#endif

public:
	CParticleSystemSceneNode(const CColladaDatabase &database, SEmitter &emitter, res::vector<res::String> *forceNodes, collada::IRootSceneNode *pRoot);
	virtual ~CParticleSystemSceneNode();

#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	const ps_type* getParticleSystem() const {
		return m_particleSystem;
	}

	ps_type* getParticleSystem() {
		return m_particleSystem;
	}
#endif

	virtual const char *getUID() const
	{
		return m_pEmitter->id;
	}

	virtual void initParticleSystem();

	virtual int getParticleCount();

	//! Returns type of the scene node
	virtual scene::ESCENE_NODE_TYPE getType() const { 
		return scene::ESNT_COLLADA_PARTICLE_SYSTEM;
	}

	//! returns the axis aligned bounding box of this node
	virtual const core::aabbox3d<f32>& getBoundingBox() const 
	{ 
		//TODO-FH !!!
		static core::aabbox3d<f32> box;
		return box;
	};

	virtual void OnRegisterSceneNode();
	virtual void OnAnimate(u32 timeMs);

	virtual void render(void* renderData = 0);

	//! Kinda deprecated part of the interface, but adding it in case...
	virtual ISceneNode* clone() { return 0; };
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const 
	{
		scene::ISceneNode::serializeAttributes(out, options);
	};
	
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0) 
	{
		scene::ISceneNode::deserializeAttributes(in, options);
	};

	virtual void prepareMaterial();

	void addMaterial(const collada::SMaterial &material)
	{
		m_OriginalMaterials.push_back(&material);
	}

	//! Returns the material based on the zero based index i.
		/** To get the amount of materials used by this scene node, use
		getMaterialCount(). This function is needed for inserting the
		node into the scene hierarchy at an optimal position for
		minimizing renderstate changes, but can also be used to
		directly modify the material of a scene node.
		\param num Zero based index. The maximal value is getMaterialCount() - 1.
		\return The material at that index. */
	virtual video::SMaterial& getMaterial(u32 num);

	//! Get amount of materials used by this scene node.
		/** \return Current amount of materials of this scene node. */
	virtual u32 getMaterialCount() const { return m_Materials.size(); }

	//! Attach the particle system to the forces include under a provided node
	virtual void attach(ISceneNode* node);

protected:
#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
	ps_type*											m_particleSystem;
#endif
	ps::CPSQuadMeshBuffer								*m_meshBuffer;
	u32													m_psTimeMs;
	core::matrix4										*m_psViewMat;
	SEmitter											*m_pEmitter;
	res::vector<res::String>							*m_pForceNodes;

	collada::IRootSceneNode								*m_pRoot;
	core::array<const collada::SMaterial *>				m_OriginalMaterials;
	core::array<collada::CMaterial *>					m_Materials;
	core::array<particle_system::CForceSceneNode *>		m_Forces;
	bool												m_bHasUVAnimation;
};

}; //end namespace scene
}; //end namespace irr


#endif //_IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif
