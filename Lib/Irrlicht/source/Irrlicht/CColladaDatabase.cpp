#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "CColladaDatabase.h"
#include <SDatabaseCollada.h>
#include "CColladaMesh.h"
#include "CEmptySceneNode.h"
#include "CColladaCameraSceneNode.h"
#include "CColladaMeshSceneNode.h"
#include "CColladaSkinnedMesh.h"
#include "CColladaModularSkinnedMesh.h"
#include "CColladaAnimationTrack.h"
#include "CColladaAnimationTrackSRT.h"
#include "CColladaAnimationTrackQuaternion.h"
#include "CColladaAnimationTrackWeight.h"
#include "CColladaAnimationTrackFloat.h"
#include "CColladaAnimationTrackPosition.h"
#include "CColladaAnimationTrackVector3d.h"
#include "CColladaAnimationTrackOffsetUV.h"
#include "CColladaAnimationTrackImageFileList.h"
#include "CColladaAnimationTrackVector3d.h"
#include "CColladaAnimationTrackLightColor.h"
#include "CColladaAnimationTrackVisibility.h"
#include "CColladaAnimationTrackTransparency.h"
#include "CColladaSceneNodeAnimator.h"
#include "CColladaSkinnedMeshSceneNode.h"
#include "CColladaModularSkinnedMeshSceneNode.h"

#include "CColladaMorphingMesh.h"
#include "IColladaRootSceneNode.h"
#include "CColladaRootSceneNode.h"
#include "CColladaLightSceneNode.h"
#include "CColladaParticleSystemSceneNode.h"
#include "CColladaParticleSystemForceSceneNode.h"
#include "CCameraTargetTrackerSceneNode.h"
#include <SDatabaseCollada.h>
#include "CLightSceneNode.h"
#include "CColladaSceneNode.h"
#include "CColladaFactory.h"

namespace irr
{
namespace collada
{

namespace util
{

template<typename T>
T* find(res::vector<T>& v, const char* url)
{
	// # should be removed earlier
	_IRR_DEBUG_BREAK_IF(*url == '#');
	for(int i = 0; i < v.size(); i++)
	{
		if(v[i].id == url)
		{
			return &v[i];
		}
	}
	return 0;
}

}

bool CColladaDatabase::OptimizeSceneNodes = false;
CColladaFactory CColladaDatabase::DefaultFactory;
CColladaDatabase::E_VERSION_CHECK_BEHAVIOR CColladaDatabase::VersionCheckBehavior = CColladaDatabase::EVCB_LOAD_ANYWAY;

CColladaDatabase::CColladaDatabase(const char* url, CColladaFactory* factory)
	: ResFile(collada::CResFileManager::getInst()->load(url))
	, Factory(factory)
{
	if (ResFile)
	{
		ResFile->grab();
	}
}

CColladaDatabase::CColladaDatabase(io::IReadFile* file, CColladaFactory* factory)
	: ResFile(collada::CResFileManager::getInst()->load(file))
	, Factory(factory)
{
	if (ResFile)
	{
		ResFile->grab();
	}
}

CColladaDatabase::CColladaDatabase(io::IReadFile* file, bool refData, CColladaFactory* factory)
	: ResFile(collada::CResFileManager::getInst()->load(file, false, 0, refData))
	, Factory(factory)
{
	if (ResFile)
	{
		ResFile->grab();
	}
}

CColladaDatabase::CColladaDatabase(const CColladaDatabase* caller, const char* url, CColladaFactory* factory)
	: ResFile(collada::CResFileManager::getInst()->get(caller->ResFile, url))
	, Factory(factory)
{
	if (ResFile)
	{
		ResFile->grab();
	}
}

CColladaDatabase::~CColladaDatabase()
{
	// if ref count is zero, it means that resFile is actually being destroyed
	// (or there is a missing grab somewhere)
	if (ResFile && ResFile->getReferenceCount() > 0)
	{
		ResFile->drop();
		if (collada::CResFileManager::getInst()->isAutoUnload()
			&& ResFile->getReferenceCount() == 1)
		{
			collada::CResFileManager::getInst()->unload(ResFile->absoluteFilename.c_str());
		}
	}
}

void*
CColladaDatabase::find(const char* url /*in*/, unsigned int& typeMask /*in_out*/) const
{
	void* obj = 0;
	if(typeMask & EDT_ANIMATION)
	{
		obj = getAnimation(url);
		if(obj)
		{
			typeMask = EDT_ANIMATION;
			return obj;
		}
	}

	if(typeMask & EDT_ANIMATION_CLIP)
	{
		obj = getAnimationClip(url);
		if(obj)
		{
			typeMask = EDT_ANIMATION_CLIP;
			return obj;
		}
	}

	if(typeMask & EDT_IMAGE)
	{
		obj = getImage(url);
		if(obj)
		{
			typeMask = EDT_IMAGE;
			return obj;
		}
	}

	if(typeMask & EDT_EFFECT)
	{
		obj = getEffect(url);
		if(obj)
		{
			typeMask = EDT_EFFECT;
			return obj;
		}
	}

	if(typeMask & EDT_MATERIAL)
	{
		obj = getMaterial(url);
		if(obj)
		{
			typeMask = EDT_MATERIAL;
			return obj;
		}
	}

	if(typeMask & EDT_GEOMETRY)
	{
		obj = getGeometry(url);
		if(obj)
		{
			typeMask = EDT_GEOMETRY;
			return obj;
		}
	}

	if(typeMask & EDT_CONTROLLER)
	{
		obj = getController(url);
		if(obj)
		{
			typeMask = EDT_CONTROLLER;
			return obj;
		}
	}

	if(typeMask & EDT_VISUAL_SCENE)
	{
		obj = getVisualScene(url);
		if(obj)
		{
			typeMask = EDT_VISUAL_SCENE;
			return obj;
		}
	}
	
	return 0;
}

SAnimation*
CColladaDatabase::getBlendableAnimation(const SAnimation* animation) const
{
	if(animation == 0)
	{
		return 0;
	}

	return getBlendableAnimation(&animation->channels[0]);
}



SAnimation*
CColladaDatabase::getBlendableAnimation(const SChannel *channel) const
{
	if(channel == 0)
	{
		return 0;
	}

	return getAnimation(channel->target, channel->type, channel->reserved0);
}


SAnimation*
CColladaDatabase::getAnimation(const char* url, SChannel::Type type, unsigned char reserved0) const
{
	for(int i = 0, cnt = getAnimationCount(); i < cnt; i++)
	{
		SAnimation* animIt = getAnimation(i);
		SChannel* channelIt = &animIt->channels[0];

		switch(type)
		{
		case SChannel::CT_POSITION:
		case SChannel::CT_POSITION_X:
		case SChannel::CT_POSITION_Y:
		case SChannel::CT_POSITION_Z:
			switch (channelIt->type)
			{
			case SChannel::CT_POSITION:
			case SChannel::CT_POSITION_X:
			case SChannel::CT_POSITION_Y:
			case SChannel::CT_POSITION_Z:
				if (channelIt->target == url)
				{
					return animIt;
				}
			}
			break;
		case SChannel::CT_ROTATION:
		case SChannel::CT_ROTATION_W:
			switch (channelIt->type)
			{
			case SChannel::CT_ROTATION:
			case SChannel::CT_ROTATION_W:
				if (channelIt->target == url)
				{
					return animIt;
				}
			}
			break;
		case SChannel::CT_EFFECT_TRANSFORM:
			if ((channelIt->type & SChannel::CT_EFFECT_TRANSFORM)
				&& channelIt->target == url)
			{
				return animIt;
			}
			break;
		default:
			if ((channelIt->type == type)
				&& (channelIt->reserved0 == reserved0)
				&& (channelIt->target == url))
			{
				return animIt;
			}
		};
		
	}
	return 0;
}

bool
CColladaDatabase::getDefaultValue(const SAnimation* animation, void** value) const
{
	return getDefaultValue(&animation->channels[0], value);
}

bool
CColladaDatabase::getDefaultValue(const SChannel* channel, void** value) const
{
	return getDefaultValue(channel->target, channel->type, value, channel->reserved0);
}

bool				
CColladaDatabase::getDefaultValue(const char* url, SChannel::Type type, void** hValue, int res0) const
{
	//void *pTarget = find(url, 

	switch(type)
	{
	case SChannel::CT_TRANSFORM:
		{
			SNode* node = getNode(url);
			if(!node)
				break;
			//_IRR_DEBUG_BREAK_IF("Not implemented");
		}
		break;
	case SChannel::CT_POSITION:
	case SChannel::CT_POSITION_X:
	case SChannel::CT_POSITION_Y:
	case SChannel::CT_POSITION_Z:
		{
			SNode* node = getNode(url);
			if(!node)
				break;
			*hValue = &node->position;
			return true;
		}
		break;
	case SChannel::CT_ROTATION:
		{
			SNode* node = getNode(url);
			if(!node)
				break;
			*hValue = &node->rotation;
			return true;
		}
		break;
	case SChannel::CT_ROTATION_X:
		/*{
			SNode* node = getNode(url);
			if(!node)
				return false;
			*hValue = &node->rotation.X;
			return true;
		}*/
		break;
	case SChannel::CT_ROTATION_Y:
		/*{
			SNode* node = getNode(url);
			if(!node)
				return false;
			*hValue = &node->rotation.Y;
			return true;
		}*/
		break;
	case SChannel::CT_ROTATION_Z:
		/*{
			SNode* node = getNode(url);
			if(!node)
				return false;
			*hValue = &node->rotation.Z;
			return true;
		}*/
		break;
	case SChannel::CT_ROTATION_W:
		{
			SNode* node = getNode(url);
			if(!node)
				break;
			*hValue = &node->rotation;
			return true;
		}
		break;
	case SChannel::CT_SCALE:
		{
			SNode* node = getNode(url);
			if(!node)
				break;
			*hValue = &node->scale;
			return true;
		}
	case SChannel::CT_SCALE_X:
		{
			SNode* node = getNode(url);
			if(!node)
				break;
			*hValue = &node->scale.X;
			return true;
		}
	case SChannel::CT_SCALE_Y:
		{
			SNode* node = getNode(url);
			if(!node)
				return false;
			*hValue = &node->scale.Y;
			return true;
		}
	case SChannel::CT_SCALE_Z:
		{
			SNode* node = getNode(url);
			if(!node)
				break;
			*hValue = &node->scale.Z;
			return true;
		}
		break;
	case SChannel::CT_EFFECT_TRANSFORM_OFFSET_U:
	case SChannel::CT_EFFECT_TRANSFORM_OFFSET_V:
	case SChannel::CT_EFFECT_TRANSFORM_ROTATE:
	case SChannel::CT_EFFECT_TRANSFORM_SCALE_U:
	case SChannel::CT_EFFECT_TRANSFORM_SCALE_V:
		{
			SEffect *pEffect = getEffect(url);
			if(!pEffect)
			{
				static float defaultVOffset = 0;
				*hValue = &defaultVOffset;
			}
			else
			{
				*hValue = &(*pEffect->diffuse.pTexture)[0].offsetU;
			}
			return true;
		}
		break;
	case SChannel::CT_EFFECT_TRANSPARENCY:
		{
			SEffect *pEffect = getEffect(url);
			if(!pEffect)
				break;
			*hValue = &pEffect->transparency;
			return true;
		}
		break;
	case SChannel::CT_EFFECT_AMBIENT:
	case SChannel::CT_EFFECT_AMBIENT_R:
	case SChannel::CT_EFFECT_AMBIENT_G:
	case SChannel::CT_EFFECT_AMBIENT_B:
	case SChannel::CT_EFFECT_AMBIENT_A:
		{
			SEffect *pEffect = getEffect(url);
			if(!pEffect || pEffect->ambient.eType != SCommonColorOrTexture::ECOLOR)
				break;
			*hValue = pEffect->ambient.pColor;
			return true;
		}
		break;
	case SChannel::CT_EFFECT_DIFFUSE:
	case SChannel::CT_EFFECT_DIFFUSE_R:
	case SChannel::CT_EFFECT_DIFFUSE_G:
	case SChannel::CT_EFFECT_DIFFUSE_B:
	case SChannel::CT_EFFECT_DIFFUSE_A:
		{
			SEffect *pEffect = getEffect(url);
			if(!pEffect || pEffect->diffuse.eType != SCommonColorOrTexture::ECOLOR)
				break;
			*hValue = pEffect->diffuse.pColor;
			return true;
		}
		break;
	case SChannel::CT_EFFECT_SPECULAR:
	case SChannel::CT_EFFECT_SPECULAR_R:
	case SChannel::CT_EFFECT_SPECULAR_G:
	case SChannel::CT_EFFECT_SPECULAR_B:
	case SChannel::CT_EFFECT_SPECULAR_A:
		{
			SEffect *pEffect = getEffect(url);
			if(!pEffect || pEffect->specular.eType != SCommonColorOrTexture::ECOLOR)
				break;
			*hValue = pEffect->specular.pColor;
			return true;
		}
		break;
	case SChannel::CT_EFFECT_EMISSION:
	case SChannel::CT_EFFECT_EMISSION_R:
	case SChannel::CT_EFFECT_EMISSION_G:
	case SChannel::CT_EFFECT_EMISSION_B:
	case SChannel::CT_EFFECT_EMISSION_A:
		{
			break;
			/*SEffect *pEffect = getEffect(url);
			if(!pEffect || pEffect->emissive.eType != SCommonColorOrTexture::ECOLOR)
				break;
			*hValue = pEffect->emission.pColor;
			return true;*/
		}
		break;
	case SChannel::CT_MORPHING_WEIGHT:
		{
			SController* controller = getController(url);
			if(!controller)
				break;
			*hValue = &controller->pMorph->weights[res0];
			return true;
			//_IRR_DEBUG_BREAK_IF("Not properly implemented");
		}
		break;
	case SChannel::CT_IFL:
		//_IRR_DEBUG_BREAK_IF("Not implemented");
		break;
	case SChannel::CT_LIGHT_COLOR:
		{
			SLight* pLight = getLight(url);
			if(!pLight)
				break;
			*hValue = &pLight->color;
			return true;
		}
	case SChannel::CT_LIGHT_CONSTANT_ATTENUATION:
	case SChannel::CT_LIGHT_LINEAR_ATTENUATION:
	case SChannel::CT_LIGHT_QUADRATIC_ATTENUATION:
		_IRR_DEBUG_BREAK_IF("Not implemented");
		break;
	case SChannel::CT_VISIBILITY:
		{
			SNode* node = getNode(url);
			if(!node)
				break;
			*hValue = &node->visibility;
			return true;
		}
		//_IRR_DEBUG_BREAK_IF("Not implemented");
		break;
	default:
		*hValue = 0;
		//_IRR_DEBUG_BREAK_IF("Not implemented");
		break;
	};

	*hValue = 0;
	return false;
}

const char*
CColladaDatabase::getVersion() const
{
	return getCollada().version;
}

collada::SAnimation*
CColladaDatabase::getAnimation(const char* url) const
{
	return util::find(getCollada().libraryAnimations.animations, url);
}

collada::SAnimationClip*	
CColladaDatabase::getAnimationClip(const char* url) const
{
	return util::find(getCollada().libraryAnimationClips.animationClips, url);
}

SLibraryAnimationClips*
CColladaDatabase::getAnimationClipLibrary() const
{
	return &getCollada().libraryAnimationClips;
}

SCamera*
CColladaDatabase::getCamera(const char* url) const
{
	return util::find(getCollada().libraryCameras.cameras, url);
}

SLight*
CColladaDatabase::getLight(const char* url) const
{
	return util::find(getCollada().libraryLights.lights, url);
}

collada::SImage*
CColladaDatabase::getImage(const char* url) const
{
	return util::find(getCollada().libraryImages.images, url);
}

collada::SEffect*
CColladaDatabase::getEffect(const char* url) const
{
	return util::find(getCollada().libraryEffects.effects, url);
}

collada::SMaterial*
CColladaDatabase::getMaterial(const char* url) const
{
	return util::find(getCollada().libraryMaterials.materials, url);
}

SNode*
CColladaDatabase::getNode(const char* url) const
{
	collada::SVisualScene* pVisualScene = getVisualScene(0);
	if(pVisualScene == 0)
		return 0;
	res::vector<SNode>& nodes = pVisualScene->nodes;
	for(int i = 0, sz = nodes.size(); i < sz; ++i)
	{
		SNode* ret = getNode(url, nodes[i]);
		if(ret)
			return ret;
	}
	return 0;
}

SNode*				
CColladaDatabase::getNode(const char* url, SNode& node) const
{
	SNode* ret = 0;
	if(node.id == url)
		return &node;

	for(int i = 0, sz = node.children.size(); i < sz; ++i)
	{
		SNode* ret = getNode(url, node.children[i]);
		if(ret)
			return ret;
	}
	return 0;
}

collada::SMaterial*
CColladaDatabase::getMaterial(const char* filename, const char* url) const
{
	CColladaDatabase database(this, filename);
	SMaterial* material = database.getMaterial(url);

	return material;
}

collada::SGeometry*
CColladaDatabase::getGeometry(const char* url) const
{
	return util::find(getCollada().libraryGeometries.geometries, url);
}

collada::SController*
CColladaDatabase::getController(const char* url) const
{
	return util::find(getCollada().libraryControllers.controllers, url);
}

collada::SForce*
CColladaDatabase::getForce(const char* url) const
{
	return util::find(getCollada().libraryForces.forces, url);
}

collada::SEmitter*
CColladaDatabase::getEmitter(const char* url) const
{
	return util::find(getCollada().libraryEmitters.emitters, url);
}

collada::SVisualScene*
CColladaDatabase::getVisualScene(const char* url) const
{
	return util::find(getCollada().libraryVisualScene.visualScenes, url);
}

collada::SScene*
CColladaDatabase::getScene() const
{
	return &getCollada().scene;
}

collada::SAnimation*
CColladaDatabase::getAnimation(int index) const
{
	return &getCollada().libraryAnimations.animations[index];
}

collada::SAnimationClip*
CColladaDatabase::getAnimationClip(int index) const
{
	return &getCollada().libraryAnimationClips.animationClips[index];
}

SCamera*
CColladaDatabase::getCamera(int index) const
{
	return &getCollada().libraryCameras.cameras[index];
}

SLight*
CColladaDatabase::getLight(int index) const
{
	return &getCollada().libraryLights.lights[index];
}

collada::SImage*
CColladaDatabase::getImage(int index) const
{
	return &getCollada().libraryImages.images[index];
}

collada::SEffect*
CColladaDatabase::getEffect(int index) const
{
	return &getCollada().libraryEffects.effects[index];
}

collada::SMaterial*
CColladaDatabase::getMaterial(int index) const
{
	return &getCollada().libraryMaterials.materials[index];
}

collada::SGeometry*
CColladaDatabase::getGeometry(int index) const
{
	return &getCollada().libraryGeometries.geometries[index];
}

collada::SController*
CColladaDatabase::getController(int index) const
{
	return &getCollada().libraryControllers.controllers[index];
}

collada::SForce*
CColladaDatabase::getForce(int index) const
{
	return &getCollada().libraryForces.forces[index];
}

collada::SEmitter*
CColladaDatabase::getEmitter(int index) const
{
	return &getCollada().libraryEmitters.emitters[index];
}

collada::SVisualScene*
CColladaDatabase::getVisualScene(int index) const
{
	if(getCollada().libraryVisualScene.visualScenes.size() > 0)
	{
		return &getCollada().libraryVisualScene.visualScenes[index];
	}
	else
	{
		return 0;
	}
}

CMaterial*
CColladaDatabase::constructMaterial(int index, IRootSceneNode* root) const
{
	return constructMaterial(getMaterial(index), root);
}

CMaterial*
CColladaDatabase::constructMaterial(const char* url, IRootSceneNode* root) const
{
	return constructMaterial(getMaterial(url), root);
}

CMaterial*
CColladaDatabase::constructMaterial(SMaterial* material, IRootSceneNode* root) const
{
	if(!material)
	{
		return 0;
	}

	return Factory->createMaterial(*this, material, root);
}

CImage*				
CColladaDatabase::constructImage(int index, IRootSceneNode* root) const
{
	return constructImage(getImage(index), root);
}

CImage*
CColladaDatabase::constructImage(const char* url, IRootSceneNode* root) const
{
	return constructImage(getImage(url), root);
}

CImage*
CColladaDatabase::constructImage(SImage* image, IRootSceneNode* root) const
{
	if(!image)
	{
		return 0;
	}

	return irrnew CImage(*this, *image);
}

scene::CLightSceneNode*
CColladaDatabase::constructLight(int index, IRootSceneNode* root) const
{
	return constructLight(getLight(index), root);
}

scene::CLightSceneNode*
CColladaDatabase::constructLight(const char* url, IRootSceneNode* root) const
{
	return constructLight(getLight(url), root);
}

scene::CLightSceneNode*
CColladaDatabase::constructLight(SLight* light, IRootSceneNode* root) const
{
	if(!light)
	{
		return 0;
	}

	CLightSceneNode* node = Factory->createLight(*this, light);
	root->addLight(node);

	return node;
}

CCameraSceneNode*
CColladaDatabase::constructCamera(int index, IRootSceneNode* root) const
{
	return constructCamera(getCamera(index), root);
}

CCameraSceneNode*
CColladaDatabase::constructCamera(const char* url, IRootSceneNode* root) const
{
	return constructCamera(getCamera(url), root);
}

CCameraSceneNode*
CColladaDatabase::constructCamera(SCamera* camera, IRootSceneNode* root) const
{
	if(!camera)
	{
		return 0;
	}

	CCameraSceneNode* node = Factory->createCameraNode(*this, camera);
	root->addCamera(node);

	return node;
}

CCameraSceneNode*
CColladaDatabase::constructCamera(SInstanceCamera* instance, IRootSceneNode* root) const
{
	return 0;
}

scene::CColladaMesh*
CColladaDatabase::constructGeometry(const char* file, const char* url, IRootSceneNode* root) const
{
	collada::CResFile* data = collada::CResFileManager::getInst()->get(ResFile, file);
	if(!data)
	{
		os::Printer::log("File not found", ELL_ERROR);
		os::Printer::log(file, ELL_ERROR);
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data);
	scene::CColladaMesh* geometry = database.constructGeometry(url, root);
	if(!geometry)
	{
		os::Printer::log("Geometry not found", ELL_ERROR);
		os::Printer::log(file, ELL_ERROR);
		os::Printer::log(url, ELL_ERROR);
	}
	return geometry;
}

scene::CColladaMesh*
CColladaDatabase::constructGeometry(const char* url, IRootSceneNode* root) const
{
	return constructGeometry(getGeometry(url), root);
}

scene::CColladaMesh*
CColladaDatabase::constructGeometry(int index, IRootSceneNode* root) const
{
	return constructGeometry(getGeometry(index), root);
}

scene::CColladaMesh*
CColladaDatabase::constructGeometry(SGeometry* geometry, IRootSceneNode* root) const
{
	if(geometry == 0 || geometry->type != SGeometry::EGT_MESH)
		return 0;
	return Factory->createGeometry(*this, geometry);
}

CAnimationTrack*
CColladaDatabase::constructAnimation(const char* url) const
{
	return constructAnimation(getAnimation(url));
}

CAnimationTrack*
CColladaDatabase::constructAnimation(int index) const
{
	return constructAnimation(getAnimation(index));
}

CAnimationTrack*
CColladaDatabase::constructAnimation(SAnimation* animator) const
{
	CAnimationTrack* animationTrack = 0;
	switch(animator->channels[0].type)
	{
	case SChannel::CT_ROTATION:
		animationTrack = irrnew animation_track::CQuaternion(*animator);
		break;
	case SChannel::CT_ROTATION_W:
		animationTrack = irrnew animation_track::CQuaternionAngle(*animator);
		break;
	case SChannel::CT_POSITION_X:
		animationTrack = irrnew animation_track::CPositionX(*animator);
		break;
	case SChannel::CT_POSITION_Y:
		animationTrack = irrnew animation_track::CPositionY(*animator);
		break;
	case SChannel::CT_POSITION_Z:
		animationTrack = irrnew animation_track::CPositionZ(*animator);
		break;
	case SChannel::CT_POSITION:
		animationTrack = irrnew animation_track::CPosition(*animator);
		break;
	case SChannel::CT_SCALE:
		animationTrack = irrnew animation_track::CPosition(*animator);
		break;
	case SChannel::CT_SCALE_X:
		animationTrack = irrnew animation_track::CPositionX(*animator);
		break;
	case SChannel::CT_SCALE_Y:
		animationTrack = irrnew animation_track::CPositionY(*animator);
		break;
	case SChannel::CT_SCALE_Z:
		animationTrack = irrnew animation_track::CPositionZ(*animator);
		break;
	case SChannel::CT_MORPHING_WEIGHT:
		animationTrack = irrnew animation_track::CWeight(*animator);
		break;
	case SChannel::CT_EFFECT_TRANSFORM_OFFSET_U:
	case SChannel::CT_EFFECT_TRANSFORM_OFFSET_V:
	case SChannel::CT_EFFECT_TRANSFORM_ROTATE:
	case SChannel::CT_EFFECT_TRANSFORM_SCALE_U:
	case SChannel::CT_EFFECT_TRANSFORM_SCALE_V:
		animationTrack = irrnew animation_track::CTextureTransform(*animator);
		break;
	case SChannel::CT_EFFECT_TRANSPARENCY:
		animationTrack = irrnew animation_track::CTransparency(*animator);
		break;
	case SChannel::CT_IFL:
		animationTrack = irrnew animation_track::CImageFileList(*animator);
		break;
	case SChannel::CT_LIGHT_COLOR:
		animationTrack = irrnew animation_track::CLightColor(*animator);
		break;

#define CASE_EFFECT_COMPONENT_COLOR(tag, color, mat_component) \
	case SChannel::CT_EFFECT_##tag : \
		animationTrack = irrnew animation_track::CColorMaterial##mat_component##color (*animator); \
		break;

#define CASE_EFFECT_FULL_COLOR(tag, mat_component) \
	case SChannel::CT_EFFECT_##tag : \
		animationTrack = irrnew animation_track::CColorMaterial##mat_component (*animator); \
		break;

#define CASE_EFFECT(tag, mat_component) \
	CASE_EFFECT_FULL_COLOR(tag, mat_component) \
	CASE_EFFECT_COMPONENT_COLOR(tag##_A, Alpha, mat_component) \
	CASE_EFFECT_COMPONENT_COLOR(tag##_R, Red, mat_component) \
	CASE_EFFECT_COMPONENT_COLOR(tag##_G, Green, mat_component) \
	CASE_EFFECT_COMPONENT_COLOR(tag##_B, Blue, mat_component)

	CASE_EFFECT(AMBIENT, Ambient);
	CASE_EFFECT(DIFFUSE, Diffuse);
	CASE_EFFECT(SPECULAR, Specular);
	CASE_EFFECT(EMISSION, Emissive);

#undef CASE_EFFECT
#undef CASE_EFFECT_FULL_COLOR
#undef CASE_EFFECT_COMPONENT_COLOR
	
	case SChannel::CT_VISIBILITY:
		animationTrack = irrnew animation_track::CVisibility(*animator);
		break;
	case SChannel::CT_EMITTER_BIRTH_RATE:
	case SChannel::CT_EMITTER_PARTICLE_SIZE:
	case SChannel::CT_EMITTER_PARTICLE_SIZE_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_SPEED:
	case SChannel::CT_EMITTER_PARTICLE_SPEED_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_DIRECTION_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_TIME:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_TIME_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_PHASE:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_PHASE_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_AXIS_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_OFFSET:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_OFFSET_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_LENGTH:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_MULTIPLIER:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_VARIANCE:
		animationTrack = irrnew animation_track::CFloat(*animator);
		break;
	case SChannel::CT_EMITTER_PARTICLE_DIRECTION:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_AXIS:
		animationTrack = irrnew animation_track::CVector3d(*animator);
		break;
	}

	return animationTrack;
}

const collada::CAnimationTrackEx*
CColladaDatabase::getAnimationTrackEx(const SAnimation* animator)
{
	if(animator == 0)
		return 0;

	return getAnimationTrackEx(&animator->channels[0]);
}

const collada::CAnimationTrackEx*
CColladaDatabase::getAnimationTrackEx(const SChannel* channel)
{
	const CAnimationTrackEx* animationTrack = 0;
	switch(channel->type)
	{
	case SChannel::CT_ROTATION:
		animationTrack = &animation_track::CQuaternionEx::s_Instance;
		break;
	case SChannel::CT_ROTATION_W:
		animationTrack = &animation_track::CQuaternionAngleEx::s_Instance;
		break;
	case SChannel::CT_POSITION_X:
		animationTrack = &animation_track::CPositionXEx::s_Instance;
		break;
	case SChannel::CT_POSITION_Y:
		animationTrack = &animation_track::CPositionYEx::s_Instance;
		break;
	case SChannel::CT_POSITION_Z:
		animationTrack = &animation_track::CPositionZEx::s_Instance;
		break;
	case SChannel::CT_POSITION:
		animationTrack = &animation_track::CPositionEx::s_Instance;
		break;
	case SChannel::CT_SCALE:
		animationTrack = &animation_track::CVector3dEx::s_Instance;
		break;
	case SChannel::CT_MORPHING_WEIGHT:
		animationTrack = &animation_track::CWeightEx::s_Instance;
		break;
	case SChannel::CT_EFFECT_TRANSFORM_OFFSET_U:
	case SChannel::CT_EFFECT_TRANSFORM_OFFSET_V:
	case SChannel::CT_EFFECT_TRANSFORM_ROTATE:
	case SChannel::CT_EFFECT_TRANSFORM_SCALE_U:
	case SChannel::CT_EFFECT_TRANSFORM_SCALE_V:
		animationTrack = &animation_track::CTextureTransformEx::s_Instance;
		break;
	case SChannel::CT_EFFECT_TRANSPARENCY:
		animationTrack = &animation_track::CTransparencyEx::Instance;
		break;
	case SChannel::CT_IFL:
		//animationTrack = new animation_track::CImageFileListEx();
		break;
	case SChannel::CT_LIGHT_COLOR:
		animationTrack = &animation_track::CLightColorEx::s_Instance;
		break;

#define CASE_EFFECT_COMPONENT_COLOR(tag, color, mat_component) \
	case SChannel::CT_EFFECT_##tag : \
		animationTrack = &animation_track::CColorMaterial##mat_component##color##Ex::getInstance(); \
		break;

#define CASE_EFFECT_FULL_COLOR(tag, mat_component) \
	case SChannel::CT_EFFECT_##tag : \
		animationTrack = &animation_track::CColorMaterial##mat_component##Ex::getInstance(); \
		break;

#define CASE_EFFECT(tag, mat_component) \
	CASE_EFFECT_FULL_COLOR(tag, mat_component) \
	CASE_EFFECT_COMPONENT_COLOR(tag##_A, Alpha, mat_component) \
	CASE_EFFECT_COMPONENT_COLOR(tag##_R, Red, mat_component) \
	CASE_EFFECT_COMPONENT_COLOR(tag##_G, Green, mat_component) \
	CASE_EFFECT_COMPONENT_COLOR(tag##_B, Blue, mat_component)

	CASE_EFFECT(AMBIENT, Ambient);
	CASE_EFFECT(DIFFUSE, Diffuse);
	CASE_EFFECT(SPECULAR, Specular);
	CASE_EFFECT(EMISSION, Emissive);

#undef CASE_EFFECT
#undef CASE_EFFECT_FULL_COLOR
#undef CASE_EFFECT_COMPONENT_COLOR

	case SChannel::CT_VISIBILITY:
		animationTrack = &animation_track::CVisibilityEx::s_Instance;
		break;
	case SChannel::CT_EMITTER_BIRTH_RATE:
	case SChannel::CT_EMITTER_PARTICLE_SIZE:
	case SChannel::CT_EMITTER_PARTICLE_SIZE_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_SPEED:
	case SChannel::CT_EMITTER_PARTICLE_SPEED_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_DIRECTION_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_TIME:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_TIME_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_PHASE:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_PHASE_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_AXIS_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_OFFSET:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_OFFSET_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_LENGTH:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_VARIANCE:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_MULTIPLIER:
	case SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_VARIANCE:
		animationTrack = &animation_track::CFloatEx::s_Instance;
		break;
	case SChannel::CT_EMITTER_PARTICLE_DIRECTION:
	case SChannel::CT_EMITTER_PARTICLE_SPIN_AXIS:
		animationTrack = &animation_track::CVector3dEx::s_Instance;
		break;
	
	}

	return animationTrack;
}

CSceneNodeAnimator*
CColladaDatabase::constructAnimator(const char* filename, CColladaFactory* factory)
{
	collada::CResFile* data = collada::CResFileManager::getInst()->load(filename);
	if(!data)
	{
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data, factory);
	return database.constructAnimator();
}

CSceneNodeAnimator*
CColladaDatabase::constructAnimator(io::IReadFile* file, CColladaFactory* factory)
{
	collada::CResFile* data = collada::CResFileManager::getInst()->load(file);
	if(!data)
	{
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data, factory);
	return database.constructAnimator();
}

CSceneNodeAnimator*
CColladaDatabase::constructAnimator(io::IReadFile* file, bool refData, CColladaFactory* factory)
{
	collada::CResFile* data = collada::CResFileManager::getInst()->load(file, false, 0, refData);
	if(!data)
	{
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data, factory);
	return database.constructAnimator();
}


CSceneNodeAnimator*
CColladaDatabase::constructAnimator() const
{
	int iflCount = 0;
	for(int j = 0; j < getImageCount(); ++j)
	{
		SImage* image = getImage(j);
		if(image->type == SImage::CIT_IFL)
			++iflCount;
	}

	if(getAnimationCount() == 0 && iflCount == 0 && getEventsTrack() == 0)
		return 0;

	CSceneNodeAnimator* animatorLibrary = Factory->createAnimator(*this, &getCollada().libraryAnimationClips);
	
	for(int i = 0; i < getAnimationCount(); ++i)
	{
		CAnimationTrack* animator = constructAnimation(i);
		if(animator)
		{
			animatorLibrary->addAnimationTrack(animator);
			animator->drop();
		}
	}

	animatorLibrary->setEventsTrack(getEventsTrack());
	
	for(int j = 0; j < getImageCount(); ++j)
	{
		SImage* image = getImage(j);
		if(image->type == SImage::CIT_IFL)
		{
			CAnimationTrack* animator = constructAnimation(&image->pIFL->animation);
			if(animator)
			{
				animatorLibrary->addAnimationTrack(animator);
				animator->drop();
			}
		}
	}

	return animatorLibrary;
}

scene::CColladaMesh*
CColladaDatabase::constructGeometry(SInstanceGeometry* instance, IRootSceneNode* root) const
{
	scene::CColladaMesh* mesh = 0;
	
	const char* url = instance->url + 1;
	if(instance->filename)
	{
		mesh = constructGeometry(instance->filename, url, root);
	}
	else
	{
		mesh = constructGeometry(url, root);
	}
	

	if(mesh)
	{
		for(int j = 0; j < instance->bindMaterial.instanceMaterials.size(); ++j)
		{
			collada::SMaterial* material = 0;

			if(instance->bindMaterial.instanceMaterials[j].filename)
			{
				material = getMaterial(instance->bindMaterial.instanceMaterials[j].filename, 
					instance->bindMaterial.instanceMaterials[j].url + 1);
			}
			else
			{
				material = getMaterial(instance->bindMaterial.instanceMaterials[j].iMaterial);
			}
			_IRR_DEBUG_BREAK_IF(material == 0);
			mesh->setMaterial(j, *material);
		}
	}
	
	return mesh;
}

scene::IColladaMesh*
CColladaDatabase::constructMorph(SController* controller, IRootSceneNode* root) const
{
	_IRR_DEBUG_BREAK_IF(controller == 0);
	_IRR_DEBUG_BREAK_IF(root == 0);

	CColladaMorphingMesh *mesh = Factory->createMorph(*this, controller);
	root->addMorphingMesh(mesh);

	_IRR_DEBUG_BREAK_IF(mesh == 0);
	return mesh;

}

scene::IColladaMesh*
CColladaDatabase::constructSkin(SController* controller, IRootSceneNode* root) const
{
	_IRR_DEBUG_BREAK_IF(controller == 0);

	scene::CColladaSkinnedMesh* mesh = Factory->createSkin(*this, controller, 0, root);
	_IRR_DEBUG_BREAK_IF(mesh == 0);

	root->addSkinnedMesh(mesh);

	return mesh;
}

scene::IColladaMesh*
CColladaDatabase::constructModularSkin(SInstanceModularSkin* controller, IRootSceneNode* root) const
{
	_IRR_DEBUG_BREAK_IF(controller == 0);

	scene::CColladaModularSkinnedMesh* mesh = Factory->createModularSkin(*this, controller, root);
	_IRR_DEBUG_BREAK_IF(mesh == 0);

	return mesh;
}

scene::IColladaMesh*
CColladaDatabase::constructController(const char* url, IRootSceneNode* root) const
{
	return constructController(getController(url), root);
}

scene::IColladaMesh*
CColladaDatabase::constructController(int index, IRootSceneNode* root) const
{
	return constructController(getController(index));
}

scene::IColladaMesh*
CColladaDatabase::constructController(SController* geometry, IRootSceneNode* root) const
{
	switch(geometry->type)
	{
		case SController::ESKIN:
			return constructSkin(geometry, root);
		case SController::EMORPH:
			return constructMorph(geometry, root);
		/*case SController::EMODULARSKIN:
			return constructModularSkin(geometry, root);*/
	}
	return 0;
}

scene::IColladaMesh*
CColladaDatabase::constructController(SInstanceController* instance, IRootSceneNode* root) const
{
	_IRR_DEBUG_BREAK_IF(instance->url[0] != '#');
	const char* url = instance->url + 1;

	scene::IColladaMesh* mesh = constructController(url, root);

	if(mesh)
	{
		for(int j = 0; j < instance->bindMaterial.instanceMaterials.size(); ++j)
		{
			collada::SMaterial* material = getMaterial(instance->bindMaterial.instanceMaterials[j].iMaterial);
			_IRR_DEBUG_BREAK_IF(material == 0);
			mesh->setMaterial(j, *material);
		}
	}

	return mesh;
}

CParticleSystemSceneNode*
CColladaDatabase::constructEmitter(const char* url, res::vector<res::String> *forceNodes, IRootSceneNode* root) const
{
	return constructEmitter(getEmitter(url), forceNodes, root);
}

CParticleSystemSceneNode*
CColladaDatabase::constructEmitter(int index, res::vector<res::String> *forceNodes, IRootSceneNode* root) const
{
	return constructEmitter(getEmitter(index), forceNodes, root);
}

CParticleSystemSceneNode*
CColladaDatabase::constructEmitter(SEmitter* emitter, res::vector<res::String> *forceNodes, IRootSceneNode* root) const
{
	if(emitter == 0)
		return 0;

	return Factory->createParticleSystem(*this, emitter, forceNodes, root);
}

CParticleSystemSceneNode*
CColladaDatabase::constructEmitter(SInstanceEmitter* instance,  IRootSceneNode* root) const
{
	_IRR_DEBUG_BREAK_IF(instance->url[0] != '#');
	const char* url = instance->url + 1;

	CParticleSystemSceneNode* emitter = constructEmitter(url, &instance->forceNodes, root);

	if(emitter)
	{
		for(int j = 0; j < instance->bindMaterial.instanceMaterials.size(); ++j)
		{
			collada::SMaterial* material = getMaterial(instance->bindMaterial.instanceMaterials[j].iMaterial);
			_IRR_DEBUG_BREAK_IF(material == 0);
			emitter->addMaterial(*material);
		}
	}

	return emitter;
}

particle_system::CForceSceneNode*
CColladaDatabase::constructForce(const char* url, IRootSceneNode* root) const
{
	return constructForce(getForce(url), root);
}

particle_system::CForceSceneNode*
CColladaDatabase::constructForce(int index, IRootSceneNode* root) const
{
	return constructForce(getForce(index), root);
}

particle_system::CForceSceneNode*
CColladaDatabase::constructForce(SInstanceForce* instance, IRootSceneNode* root) const
{
	_IRR_DEBUG_BREAK_IF(instance->url[0] != '#');
	const char* url = instance->url + 1;

	particle_system::CForceSceneNode* force = constructForce(url, root);

	return force;
}

particle_system::CForceSceneNode*
CColladaDatabase::constructForce(SForce* force, IRootSceneNode* root) const
{
	if (force) {
		switch (force->type) {
			case SForce::FT_GRAVITY:
				return Factory->createParticleSystemGravityForce(*this, force);
			case SForce::FT_WIND:
				return Factory->createParticleSystemWindForce(*this, force);
			case SForce::FT_DEFLECTOR:
				return Factory->createParticleSystemDeflectorForce(*this, force);
		}
	}
	return 0;
}

scene::ISceneNode*
CColladaDatabase::constructNode(const char* filename, const char* url, CColladaFactory* factory)
{
	collada::CResFile* data = collada::CResFileManager::getInst()->load(filename);
	if(!data)
	{
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data, factory);
	return database.constructNode(url);
}

scene::ISceneNode*
CColladaDatabase::constructNode(io::IReadFile* file, const char* url, CColladaFactory* factory)
{
	collada::CResFile* data = collada::CResFileManager::getInst()->load(file);
	if(!data)
	{
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data, factory);
	return database.constructNode(url);
}

scene::ISceneNode*
CColladaDatabase::constructNode(io::IReadFile* file, const char* url, bool refData, CColladaFactory* factory)
{
	collada::CResFile* data = collada::CResFileManager::getInst()->load(file, false, 0, refData);
	if(!data)
	{
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data, factory);
	return database.constructNode(url);
}


scene::ISceneNode*
CColladaDatabase::constructNode(const char* url) const
{
	return constructNode(getNode(url));
}

scene::ISceneNode*
CColladaDatabase::constructNode(SNode* node) const
{
	if(node == 0)
		return 0;

	CRootSceneNode*	root = Factory->createScene(*this);
	scene::ISceneNode* child = constructNode(node, root);
	root->addChild(child);
	root->onPostLoad();
	child->drop();
	return root;
}

scene::ISceneNode*
CColladaDatabase::constructNode(SNode* node, IRootSceneNode* root) const
{
	if(node == 0)
		return 0;

	scene::ISceneNode* sceneNode = 0;
	scene::ISceneNode* newChild = 0;
	bool optimizeSceneNodes = false;
	// Optimize scene
	if(!OptimizeSceneNodes || node->instances.size() != 1)
	{
		if(node->pExtraTypeData)
		{ // Billboard data
			sceneNode = Factory->createBillboard(*this, node);
		}
		else
		{
			sceneNode = Factory->createNode(*this, node);
		}
	}
	else
	{
		optimizeSceneNodes = true;
	}
		
	for(int j = 0; j < node->instances.size(); ++j)
	{
		switch(node->instances[j].type)
		{
			case SInstance::ITLight:
			{
				SInstanceLight* instance = node->instances[j].pLight;
				scene::CLightSceneNode* child = constructLight(instance->url + 1, root);
				if(child)
				{
					if(optimizeSceneNodes)
					{
						sceneNode = child;
					}
					else
					{
						sceneNode->addChild(child);
						child->drop();
					}
				}
			}
			break;

			case SInstance::ITCamera:
			{
				SInstanceCamera* instance = node->instances[j].pCamera;
				scene::CCameraSceneNode* child = constructCamera(instance->url + 1, root);
				if(child) 
				{
					if(optimizeSceneNodes)
					{
						sceneNode = child;
					}
					else
					{
						sceneNode->addChild(child);
						child->drop();
					}
				}
				newChild = child;
			}
			break;

			case SInstance::ITGeometry:
			{
				scene::IColladaMesh* mesh = constructGeometry(node->instances[j].pGeometry, root);
				if(mesh == 0)
					break;

				scene::CColladaMeshSceneNode* child = Factory->createMeshNode(*this, mesh, root, node->pUserProperty);
				mesh->drop();
				if(child) 
				{
					child->setRenderingLayer(node->instances[j].pGeometry->drawLayer);
					child->prepareMaterial();
					if(optimizeSceneNodes)
					{
						sceneNode = child;
					}
					else
					{
						sceneNode->addChild(child);
						child->drop();
					}
				}
				newChild = child;
			}
			break;

			case SInstance::ITController:
			{
				scene::IColladaMesh* mesh = constructController(node->instances[j].pController, root);
				scene::CColladaMeshSceneNode* child = 0;
				if(mesh->getType() == scene::IColladaMesh::ET_SKINNED_MESH
				   || mesh->getType() == scene::IColladaMesh::ET_MODULAR_SKINNED_MESH)
				{
					child = Factory->createSkinNode(*this, mesh, root, node->pUserProperty);
				}
				else
				{
					child = Factory->createMeshNode(*this, mesh, root, node->pUserProperty);
				}
				mesh->drop();
				if(child) 
				{
					child->setRenderingLayer(node->instances[j].pController->drawLayer);
					child->prepareMaterial();
					child->setAutomaticCulling(scene::EAC_FRUSTUM_BOX);
					if(optimizeSceneNodes)
					{
						sceneNode = child;
					}
					else
					{
						sceneNode->addChild(child);
						child->drop();
					}
				}
			}
			break;

			case SInstance::ITEmitter:
			{
				//scene::IColladaMesh* mesh = constructEmitter(node->instances[j].pEmitter, root);
				collada::CParticleSystemSceneNode* child = constructEmitter(node->instances[j].pEmitter, root);
				if (child) 
				{
					child->setRenderingLayer(node->instances[j].pEmitter->drawLayer);
					child->prepareMaterial();

					if(optimizeSceneNodes)
					{
						sceneNode = child;
					}
					else
					{
						sceneNode->addChild(child);
						child->drop();
					}
				}
				break;
			}

			case SInstance::ITForce:
			{
				collada::particle_system::CForceSceneNode* child = constructForce(node->instances[j].pForce, root);
				if (child) 
				{
					if(optimizeSceneNodes)
					{
						sceneNode = child;
					}
					else
					{
						sceneNode->addChild(child);
						child->drop();
					}
				}
				break;
			}

			case SInstance::ITModularSkin:
			{
				scene::IColladaMesh* mesh = constructModularSkin(node->instances[j].pModularSkin, root);
				scene::CColladaMeshSceneNode* child = 0;
				
				_IRR_DEBUG_BREAK_IF(mesh->getType() != scene::IColladaMesh::ET_MODULAR_SKINNED_MESH);
				child = Factory->createModularSkinNode(*this, mesh, root, node->pUserProperty);
				mesh->drop();
				if(child) 
				{
					child->setRenderingLayer(node->instances[j].pController->drawLayer);
					child->prepareMaterial();
					child->setAutomaticCulling(scene::EAC_FRUSTUM_BOX);
					if(optimizeSceneNodes)
					{
						sceneNode = child;
					}
					else
					{
						sceneNode->addChild(child);
						child->drop();
					}
				}
				break;
			}

			default:
			{
				//int type = node->instances[j].type;
				_IRR_DEBUG_BREAK_IF("UNSUPPORTED TYPE");
				break;
			}
		}
	}

	sceneNode->setName(node->name);
	sceneNode->setPosition(node->position);
	sceneNode->setRotation(node->rotation);
	sceneNode->setScale(node->scale);

	for(int i = 0; i < node->children.size(); i++)
	{
		scene::ISceneNode* child = constructNode(&node->children[i], root);
		_IRR_DEBUG_BREAK_IF(child == 0);
		sceneNode->addChild(child);
		child->drop();
	}
	return sceneNode;
}

scene::ISceneNode*
CColladaDatabase::constructVisualScene(const char* url, IRootSceneNode* root) const
{
	return constructVisualScene(getVisualScene(url), root);
}

scene::ISceneNode*
CColladaDatabase::constructVisualScene(int index, IRootSceneNode* root) const
{
	return constructVisualScene(getVisualScene(index), root);
}

scene::ISceneNode*
CColladaDatabase::constructVisualScene(SVisualScene* visualScene, IRootSceneNode* root) const
{
	if(visualScene == 0)
		return 0;

	scene::ISceneNode* sceneNode = Factory->createNode(*this);
	sceneNode->setName(visualScene->name);

	for(int i = 0; i < visualScene->nodes.size(); i++)
	{
		scene::ISceneNode* child = constructNode(&visualScene->nodes[i], root);
		_IRR_DEBUG_BREAK_IF(child == 0);
		sceneNode->addChild(child);
		child->drop();

		//attachSkin(child, sceneNode);
	}


	return sceneNode;
}

scene::ISceneNode*
CColladaDatabase::constructScene(io::IReadFile* file, bool createAndAddAnimator, CColladaFactory* factory)
{
	collada::CResFile* data = collada::CResFileManager::getInst()->load(file);
	if(!data)
	{
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data, factory);
	return database.constructScene(createAndAddAnimator);
}

scene::ISceneNode*
CColladaDatabase::constructScene(io::IReadFile* file, bool createAndAddAnimator, bool refData, CColladaFactory* factory)
{
	collada::CResFile* data = collada::CResFileManager::getInst()->load(file, false, 0, refData);
	if(!data)
	{
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data, factory);
	return database.constructScene(createAndAddAnimator);
}

scene::ISceneNode*
CColladaDatabase::constructScene(const char* filename, bool createAndAddAnimator, CColladaFactory* factory)
{
	collada::CResFile * data = collada::CResFileManager::getInst()->load(filename);
	if(!data)
	{
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data, factory);
	return database.constructScene(createAndAddAnimator);
}

scene::ISceneNode*		
CColladaDatabase::constructScene(const char* filename, CColladaFactory* factory)
{
	collada::CResFile* data = collada::CResFileManager::getInst()->load(filename);
	if(!data)
	{
		return 0;
	}

	collada::SAutoUnloadOff autoUnloadOff;
	collada::CColladaDatabase database(data, factory);
	return database.constructScene(true);
}

scene::ISceneNode*		
CColladaDatabase::constructScene(bool createAndAddAnimator) const
{
	scene::ISceneNode* scene = constructScene();
	
	if(scene && createAndAddAnimator)
	{
		CSceneNodeAnimator* animator = constructAnimator();
		if (animator)
		{
			scene->addAnimator(animator);
			animator->drop();
		}
	}

	return scene;
}

scene::ISceneNode*
CColladaDatabase::constructScene() const
{
	if (!isValid())
	{
		return NULL;
	}
	CRootSceneNode* sceneNode = Factory->createScene(*this);
	
	for(int i = 0; i < getCollada().scene.instances.size(); ++i)
	{
		switch(getCollada().scene.instances[i].type)
		{
			case SInstance::ITCamera:
				break;
			case SInstance::ITController:
				break;
			case SInstance::ITGeometry:
				break;
			case SInstance::ITLight:
				break;
			case SInstance::ITNode:
				break;
			case SInstance::ITPhysicsScene:
				break;
			case SInstance::ITVisualScene:
				const char* url =  (const char*)getCollada().scene.instances[i].pVisualScene->url;
				_IRR_DEBUG_BREAK_IF(url[0] != '#');
				scene::ISceneNode* child = constructVisualScene(url + 1, sceneNode);
				sceneNode->addChild(child);
				child->drop();
				//attachSkin(child, sceneNode);
				break;
		}
	}

	sceneNode->onPostLoad();

	return sceneNode;
}

} // namespace scene
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
