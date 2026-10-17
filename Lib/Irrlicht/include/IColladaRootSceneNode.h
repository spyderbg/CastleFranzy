#ifndef __ICOLLADAROOTSCENENODE_H__
#define __ICOLLADAROOTSCENENODE_H__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include <ISceneNode.h>
#include <irrList.h>
#include "CColladaMorphingMesh.h"
#include "CColladaMaterial.h"
#include "CColladaSkinnedMesh.h"
#include "CColladaDatabase.h"
#include "CColladaImage.h"
#include "IColladaObject.h"
#include "CColladaLightSceneNode.h"
#include "CColladaParticleSystemSceneNode.h"

#include "IrrPlatform.h"	//for irr::stricmp

namespace irr
{
namespace collada
{

class IRootSceneNode
	: public IObject
{
public:
	IRootSceneNode(const CColladaDatabase &database)
		: IObject(database)
	{
	}

	~IRootSceneNode()
	{
		// Clear up materials
		for (core::list<CMaterial *>::Iterator it = m_Materials.begin();
			 it != m_Materials.end();
			 ++it)
		{
			(*it)->drop();
		}
		// Clear up images
		for (core::list<CImage *>::Iterator it = m_Images.begin();
			 it != m_Images.end();
			 ++it)
		{
			(*it)->drop();
		}
		// for the rest, no grab was done
	}

	void addCamera(CCameraSceneNode *pCamera)
	{
		m_Cameras.push_back(pCamera);
	}

	virtual void addSkinnedMesh(scene::CColladaSkinnedMesh *pSkinnedMesh)
	{
		m_SkinnedMeshs.push_back(pSkinnedMesh);
	}

	void addMorphingMesh(CColladaMorphingMesh *pMorphingMesh)
	{
		m_MorphingMeshs.push_back(pMorphingMesh);
	}

	void addMaterial(CMaterial *pMaterial)
	{
		pMaterial->grab();
		m_Materials.push_back(pMaterial);
	}

	void addLight(CLightSceneNode *pLight)
	{
		m_Lights.push_back(pLight);
	}

	void addParticleSystem(CParticleSystemSceneNode *pParticleSystem)
	{
		m_ParticleSystems.push_back(pParticleSystem);
	}

	CColladaMorphingMesh *getMorphingMesh(const char *nickname)
	{	
		core::list<CColladaMorphingMesh *>::Iterator materialIt = m_MorphingMeshs.begin();
		for(; materialIt != m_MorphingMeshs.end(); materialIt++)
		{
			if((*materialIt)->getUID() == nickname)
			{
				return *materialIt;
			}
		}

		return 0;
	}

	CLightSceneNode *getLibraryLight(const char *light)
	{
		core::list<CLightSceneNode *>::Iterator lightIt = m_Lights.begin();
		for(; lightIt != m_Lights.end(); lightIt++)
		{
			if((*lightIt)->getLightID() == light)
			{
				return *lightIt;
			}
		}
		return 0;
	}

	CImage *getLibraryImage(const char *image)
	{
		core::list<CImage *>::Iterator imageIt = m_Images.begin();
		for(; imageIt != m_Images.end(); imageIt++)
		{
			if((*imageIt)->getUID() == image)
			{
				return *imageIt;
			}
		}

		// create image if not found
		CImage *newImage = getCollada().constructImage(image);
		if(newImage)
		{
			m_Images.push_back(newImage);
		}

		return newImage;
	}

	CMaterial *hasLibraryMaterial(const char *nickname)
	{
		core::list<CMaterial *>::Iterator materialIt = m_Materials.begin();
		for(; materialIt != m_Materials.end(); materialIt++)
		{
			if((*materialIt)->getUID() == nickname)
			{
				return *materialIt;
			}
		}

		return 0;
	}

	CMaterial *addLibraryMaterial(const char *nickname, const CColladaDatabase &database)
	{
		CMaterial *newMaterial = database.constructMaterial(nickname, this);
		if(newMaterial)
		{
			m_Materials.push_back(newMaterial);
		}

		return newMaterial;
	}
	
	/*CMaterial *getLibraryMaterial(const char *nickname)
	{
		core::list<CMaterial *>::Iterator materialIt = m_Materials.begin();
		for(; materialIt != m_Materials.end(); materialIt++)
		{
			if((*materialIt)->getUID() == nickname)
			{
				return *materialIt;
			}
		}

		// create material if not found
		CMaterial *newMaterial = m_Collada.constructMaterial(nickname, this);
		if(newMaterial)
		{
			m_Materials.push_back(newMaterial);
		}

		return newMaterial;
	}*/

	CMaterial *getLibraryEffect(const char *nickname)
	{
		core::list<CMaterial *>::Iterator materialIt = m_Materials.begin();
		for(; materialIt != m_Materials.end(); materialIt++)
		{
			if((*materialIt)->getEffectUID() == nickname)
			{
				return *materialIt;
			}
		}

		return 0;
	}

	CParticleSystemSceneNode *getParticleSystem(const char *nickname)
	{
		core::list<CParticleSystemSceneNode *>::Iterator particleSystemIt = m_ParticleSystems.begin();
		for(; particleSystemIt != m_ParticleSystems.end(); particleSystemIt++)
		{
			if(irr::stricmp((*particleSystemIt)->getUID(), nickname) == 0)
			{
				return *particleSystemIt;
			}
		}

		return 0;
	}

	void removeMorphingMesh(CColladaMorphingMesh *pMorphingMesh)
	{
		_IRR_DEBUG_BREAK_IF("Not implemented");
	}

	void removeMaterial(CMaterial *pMaterial)
	{
		_IRR_DEBUG_BREAK_IF("Not implemented");
	}

	/*void OnAnimateMaterial(u32 time)
	{
		core::list<CMaterial *>::Iterator materialIt = m_Materials.begin();
		for(; materialIt != m_Materials.end(); materialIt++)
		{
			(*materialIt)->OnAnimate(time);
		}
	}*/

	int getParticleSystemCount() const { return m_ParticleSystems.getSize(); }
	int getSkinnedMeshCount() const { return m_SkinnedMeshs.getSize(); }
	int getMorphingMeshCount() const { return m_MorphingMeshs.getSize(); }
	int getCameraCount() const { return m_Cameras.getSize(); }
	int getMaterialCount() const { return m_Materials.getSize(); }
	int getImageCount() const { return m_Images.getSize(); }
	int getLightCount() const { return m_Lights.getSize(); }

	const core::list<CParticleSystemSceneNode *>&		getParticleSystemsList() const { return m_ParticleSystems; };
	const core::list<scene::CColladaSkinnedMesh *>&		getSkinnedMeshsList() const { return m_SkinnedMeshs; };
	const core::list<CColladaMorphingMesh *>&			getMorphingMeshsList() const { return m_MorphingMeshs; }
	const core::list<CCameraSceneNode *>&				getCamerasList() const { return m_Cameras; }
	const core::list<CMaterial *>&						getMaterialsList() const { return m_Materials;	}
	const core::list<CImage *>&							getImagesList() const { return m_Images; }
	const core::list<CLightSceneNode *>&				getLightsList() const { return m_Lights; }

protected:
	// allow Attachement with the current scene node
	core::list<CParticleSystemSceneNode *>		m_ParticleSystems;

	// allow Weight animation
	core::list<scene::CColladaSkinnedMesh *>	m_SkinnedMeshs;	

	// allow Weight animation
	core::list<CColladaMorphingMesh *>			m_MorphingMeshs;

	// allow Weight animation
	core::list<CCameraSceneNode *>				m_Cameras;

	// allow UV animation, shader animation and material sharing
	core::list<CMaterial *>						m_Materials;	

	// allow Texture animation
	core::list<CImage *>						m_Images;

	// allow Lights animation
	core::list<CLightSceneNode *>				m_Lights;
};

}; // namespace collada
}; // namespace irr

#endif //  _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#endif // __ICOLLADAROOTSCENENODE_H__
