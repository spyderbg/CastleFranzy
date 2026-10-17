#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_

#include "IColladaSceneNodeAnimator.h"

namespace irr
{
namespace collada
{

ISceneNodeAnimator::ISceneNodeAnimator()
	: BindTo(0)
	, Length(0)
	, InterpolationType(EIT_DEFAULT)
	, EventsManager(0)
	, EventsCallback(FxEchoNotHandledEvents)
	, EventsUserData(0)
{
}
 
ISceneNodeAnimator::~ISceneNodeAnimator()
{
	if (EventsManager)
	{
		EventsManager->drop();
	}
}

void  
ISceneNodeAnimator::updateTime(u32 timeMs)
{
	scene::ITimelineController* timeCtrl = getTimelineCtrl();
	s32 before = 0;
	s32 relativeTime = timeMs;

	// Remap the time base on the controller
	if (timeCtrl)
	{
		before = timeCtrl->getCtrlTime();
		timeCtrl->update(relativeTime);
		relativeTime = timeCtrl->getCtrlTime();
	}
	else if (EventsManager)
	{
		relativeTime = timeMs % Length;
	}

	if (EventsManager && timeCtrl)
	{
		EventsManager->onUpdate(before,
								relativeTime,
								timeCtrl->getCurrentClipStart(),
								timeCtrl->getCurrentClipEnd());
	}
}

bool
ISceneNodeAnimator::hasCompatibleTarget(const SChannel& model)
{
	const char* bindURI = model.target;
	if(model.type == SChannel::CT_MORPHING_WEIGHT)
	{
		int weightIndex = model.reserved0;
		int targetCount = getTargetCount();
		for (int i = 0; i < targetCount; ++i)
		{
			if (strcmp(getBindURI(i), bindURI) == 0)
			{
				const SChannel& track = getAnimationTrack(i);
				_IRR_DEBUG_BREAK_IF(track.type != SChannel::CT_MORPHING_WEIGHT);
				if (track.reserved0 == weightIndex)
				{
					return true;
				}
			}
		}
	}
	else
	{
		int targetCount = getTargetCount();
		for (int i = 0; i < targetCount; ++i)
		{
			if (strcmp(getBindURI(i), bindURI) == 0)
			{
				const SChannel& track = getAnimationTrack(i);
				if(track.type == model.type)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool
ISceneNodeAnimator::setCompatibleTarget(const SChannel& model, void* target)
{
	const char* bindURI = model.target;
	if(model.type == SChannel::CT_MORPHING_WEIGHT)
	{
		int weightIndex = model.reserved0;
		int targetCount = getTargetCount();
		for (int i = 0; i < targetCount; ++i)
		{
			if (strcmp(getBindURI(i), bindURI) == 0)
			{
				const SChannel& track = getAnimationTrack(i);
				_IRR_DEBUG_BREAK_IF(track.type != SChannel::CT_MORPHING_WEIGHT);
				if(track.reserved0 == weightIndex)
				{
					setTarget(i, target);
					return true;
				}
			}
		}
	}
	else
	{
		int targetCount = getTargetCount();
		for (int i = 0; i < targetCount; ++i)
		{
			if (strcmp(getBindURI(i), bindURI) == 0)
			{
				const SChannel& track = getAnimationTrack(i);
				if (track.type == model.type)
				{
					setTarget(i, target);
					return true;
				}
			}
		}
	}
	return false;
}

void
ISceneNodeAnimator::setTarget(const char* targetID, void* target)
{
	int targetCount = getTargetCount();
	for (int i = 0; i < targetCount; ++i)
	{
		const char* bindURI = getBindURI(i);
		if (strcmp(bindURI, targetID) == 0)
		{
			setTarget(i, target);
			return;
		}
	}
	_IRR_DEBUG_BREAK_IF("Not found");
}

void
ISceneNodeAnimator::onBind(scene::ISceneNode* node)
{
	if(node == BindTo)
		return;

	BindTo = node;

	if (getTargetCount() != 0)
	{
		forceBind();
	}
}

void
ISceneNodeAnimator::onUnbind(scene::ISceneNode* node)
{
	forceUnbind(node->getUID());
}

void
ISceneNodeAnimator::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
}

void
ISceneNodeAnimator::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
}

scene::ISceneNodeAnimator*
ISceneNodeAnimator::createClone()
{
	_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
	return 0;
}

int
ISceneNodeAnimator::getLength() const
{
	return Length;
}

void
ISceneNodeAnimator::forceBind()
{
	if(BindTo == 0)
	{
		return;
	}
	
	bool isColladaRoot = (BindTo->getType() == scene::ESNT_COLLADA_ROOT);

	_IRR_DEBUG_BREAK_IF(!isColladaRoot);
	{
		CRootSceneNode* bindTo = (CRootSceneNode*)BindTo;

		for (int i = 0, cnt = getTargetCount(); i < cnt; ++i)
		{
			const char* bindUri = getBindURI(i);
			
			switch (getAnimationTrack(i).type)
			{
				case SChannel::CT_ROTATION_W:
				case SChannel::CT_ROTATION:
				{
					scene::ISceneNode* target = bindTo->getSceneNodeFromUID(bindUri);
					if(!target)
					{
						setTarget(i, 0);
						continue;
					}
					target->addBindedAnimator(this);
					setTarget(i, const_cast<core::quaternion*>(&target->getRotation()));
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
					target->setIsAnimated(scene::ESNAF_ROTATION, true);
#endif
				}
				break;

				case SChannel::CT_POSITION:
				case SChannel::CT_POSITION_X:
				case SChannel::CT_POSITION_Y:
				case SChannel::CT_POSITION_Z:
				{
					scene::ISceneNode* target = bindTo->getSceneNodeFromUID(bindUri);
					if (!target)
					{
						setTarget(i, 0);
						continue;
					}
					target->addBindedAnimator(this);
					setTarget(i, const_cast<core::vector3df*>(&target->getPosition()));
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
					target->setIsAnimated(scene::ESNAF_TRANSLATION, true);
#endif
				}
				break;

				case SChannel::CT_VISIBILITY:
				{
					scene::ISceneNode* target = bindTo->getSceneNodeFromUID(bindUri);
					if (!target)
					{
						setTarget(i, 0);
						continue;
					}
					target->addBindedAnimator(this);
					setTarget(i, target);
				}
				break;

				case SChannel::CT_SCALE:
				case SChannel::CT_SCALE_X:
				case SChannel::CT_SCALE_Y:
				case SChannel::CT_SCALE_Z:
				{
					scene::ISceneNode* target = bindTo->getSceneNodeFromUID(bindUri);
					if (!target)
					{
						setTarget(i, 0);
						continue;
					}
					target->addBindedAnimator(this);
					setTarget(i, const_cast<core::vector3df*>(&target->getScale()));
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
					target->setIsAnimated(scene::ESNAF_SCALE, true);
#endif
				}
				break;

				case SChannel::CT_MORPHING_WEIGHT:
				{
					CColladaMorphingMesh* target = bindTo->getMorphingMesh(bindUri);
					if (!target)
					{
						setTarget(i, 0);
						continue;
					}
					const SChannel& anim = getAnimationTrack(i);

					int idx = anim.reserved0 + 1;
					setTarget(i, &target->getWeightRef(idx));
				}
				break;

				case SChannel::CT_IFL:
				{
					CImage* image = bindTo->getLibraryImage(bindUri);
					if (!image)
					{
						setTarget(i, 0);
						continue;
					}

					video::ITexture** hTexture = &image->getTexture();
					setTarget(i, hTexture);
					break;
				}

				case SChannel::CT_LIGHT_COLOR:
				{
					CLightSceneNode* light = bindTo->getLibraryLight(bindUri);
					if (!light)
					{
						setTarget(i, 0);
						continue;
					}
					
					setTarget(i, &light->getLightData().DiffuseColor);
					break;
				}

				case SChannel::CT_EFFECT_TRANSFORM_OFFSET_U:
				case SChannel::CT_EFFECT_TRANSFORM_OFFSET_V:
				case SChannel::CT_EFFECT_TRANSFORM_SCALE_U:
				case SChannel::CT_EFFECT_TRANSFORM_SCALE_V:
				case SChannel::CT_EFFECT_TRANSFORM_ROTATE:
				{
					CMaterial* target = bindTo->getLibraryEffect(bindUri);
					if (!target)
					{
						setTarget(i, 0);
						continue;
					}
					//core::matrix4* texMatrix = const_cast<core::matrix4*>(&target->getTextureMatrix());
					// if the texture does not have a matrix (returned core::IdentityMatrix), initialise it's matrix
					/*if(texMatrix == &core::IdentityMatrix)
					{
						target->setTextureMatrix(core::IdentityMatrix);
						texMatrix = const_cast<core::matrix4*>(&target->getTextureMatrix());
					}*/
					setTarget(i, &target->get());
				}
				break;

			case SChannel::CT_EFFECT_AMBIENT:
			case SChannel::CT_EFFECT_AMBIENT_R:
			case SChannel::CT_EFFECT_AMBIENT_G:
			case SChannel::CT_EFFECT_AMBIENT_B:
			case SChannel::CT_EFFECT_AMBIENT_A:
			case SChannel::CT_EFFECT_DIFFUSE:
			case SChannel::CT_EFFECT_DIFFUSE_R:
			case SChannel::CT_EFFECT_DIFFUSE_G:
			case SChannel::CT_EFFECT_DIFFUSE_B:
			case SChannel::CT_EFFECT_DIFFUSE_A:
			case SChannel::CT_EFFECT_SPECULAR:
			case SChannel::CT_EFFECT_SPECULAR_R:
			case SChannel::CT_EFFECT_SPECULAR_G:
			case SChannel::CT_EFFECT_SPECULAR_B:
			case SChannel::CT_EFFECT_SPECULAR_A:
			case SChannel::CT_EFFECT_EMISSION:
			case SChannel::CT_EFFECT_EMISSION_R:
			case SChannel::CT_EFFECT_EMISSION_G:
			case SChannel::CT_EFFECT_EMISSION_B:
			case SChannel::CT_EFFECT_EMISSION_A:
			{
				CMaterial* target = bindTo->getLibraryEffect(bindUri);
				if (!target)
				{
					setTarget(i, 0);
					continue;
				}
				setTarget(i, &target->get());
			}
			break;
			case SChannel::CT_EFFECT_TRANSPARENCY:
			{
				CMaterial* target = bindTo->getLibraryEffect(bindUri);
				if (!target)
				{
					setTarget(i, 0);
					continue;
				}
				setTarget(i, &target->get());
			}
			break;

#ifdef _IRR_COMPILE_WITH_COLLADA_PARTICLE_SYSTEM_
#define PS_CASE(type_id, param_id)										\
				case type_id:											\
				{														\
						CParticleSystemSceneNode* target = bindTo->getParticleSystem(bindUri); \
						if (!target)									\
						{												\
							setTarget(i, 0);							\
							continue;									\
						}												\
						setTarget(i, target->getParticleSystem()->getParameter(param_id));	\
				}														\
				break
#else
#define PS_CASE(type_id, param_id)	
#endif
				PS_CASE(SChannel::CT_EMITTER_BIRTH_RATE, BIRTH_RATE_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_SIZE, TARGET_SIZE_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_SIZE_VARIANCE, SIZE_VARIATION_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_SPEED, SPEED_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_SPEED_VARIANCE, SPEED_VARIATION_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_DIRECTION_VARIANCE, DIR_VARIATION_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_SPIN_TIME, SPIN_TIME_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_SPIN_TIME_VARIANCE, SPIN_VARIATION_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_SPIN_PHASE, SPIN_PHASE_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_SPIN_PHASE_VARIANCE, SPIN_PHASE_VARIATION_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_SPIN_AXIS_VARIANCE, SPIN_AXIS_VARIATION_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_DIRECTION, DIRECTION_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_SPIN_AXIS, SPIN_AXIS_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_ANIMATION_OFFSET, ANIM_OFFSET_N); 
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_ANIMATION_OFFSET_VARIANCE, ANIM_OFFSET_VARIATION_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_LENGTH, ANIM_LENGTH_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_VARIANCE, ANIM_LENGTH_VARIATION_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_MULTIPLIER, ANIM_SCALE_MULTIPLIER_N);
				PS_CASE(SChannel::CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_VARIANCE, ANIM_SCALE_MULTIPLIER_VARIATION_N);

				default:
				{
					setTarget(i, 0);
				}
				break;
			}
		}
	}
}

void
ISceneNodeAnimator::forceUnbindAll()
{
	bool isColladaRoot = (BindTo->getType() == scene::ESNT_COLLADA_ROOT);
	_IRR_DEBUG_BREAK_IF(!isColladaRoot);
	CRootSceneNode* bindTo = (CRootSceneNode*)BindTo;

	for(int i = 0, targetCount = getTargetCount(); i < targetCount; ++i)
	{
		const char* bindUri = getBindURI(i);
		scene::ISceneNode* target = bindTo->getSceneNodeFromUID(bindUri);
		if (target)
			target->removeBindedAnimator(this);
		setTarget(i, 0);
	}
	BindTo = 0;
}

void
ISceneNodeAnimator::forceUnbind(const char* uid)
{
	for(int i = 0, targetCount = getTargetCount(); i < targetCount; ++i)
	{
		if (getAnimationTrack(i).target == uid)
		{
			setTarget(i, 0);
		}
	}
}

} // namespace collada
} // namespace irr

#endif // _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
