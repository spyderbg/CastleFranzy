#ifndef __IRR_PCOLOR_MODEL_H__
#define __IRR_PCOLOR_MODEL_H__

#include "ParticleContext.h"
#include "ParameterNames.h"
#include "PSUtils.h"

#include "CColladaAnimationTrackOffsetUV.h"
#include "CColladaAnimationTrackLightColor.h"
//Needs ColladaConverter and ResFile include path
#include "SDatabaseCollada.h"

namespace irr
{
namespace ps
{

template<class PType>
class PColorModel
	: public virtual IParticleContext<PType>
{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType)
public:
	enum PAnimKeyMapping
	{
		KS_NONE = 0,
		KS_LIFE
	};
private:
	collada::SAnimation*	m_textureTranformTrack;
	collada::SAnimation*	m_diffuseColorTrack;
	PAnimKeyMapping			m_animKeyMappingType;
	float					m_animOffset;
	float					m_animOffsetVariation;
	float					m_animLength;
	float					m_animLengthVariation;
	float					m_scaleMultiplier;
	float					m_scaleMultiplierVariation;
public:
	PColorModel()
		: m_textureTranformTrack(0)
		, m_diffuseColorTrack(0)
		, m_animKeyMappingType(KS_NONE)
		, m_animOffset(0.0f)
		, m_animOffsetVariation(0.0f)
		, m_animLength(0.0f)
		, m_animLengthVariation(0.0f)
	{
		this->registerPParameter(m_textureTranformTrack, TEXTURE_TRANSFORM_TRACK_N);
		this->registerPParameter(m_diffuseColorTrack, DIFFUSE_COLOR_TRACK_N);
		//NOTE-FH: Watch for code bloat for enum type which can use the <int> version
		this->registerPParameter(m_animKeyMappingType, ANIMKEY_MAPPING_TYPE_N);
		this->registerPParameter(m_animOffset, ANIM_OFFSET_N);
		this->registerPParameter(m_animOffsetVariation, ANIM_OFFSET_VARIATION_N);
		this->registerPParameter(m_animLength, ANIM_LENGTH_N);
		this->registerPParameter(m_animLengthVariation, ANIM_LENGTH_VARIATION_N);
		this->registerPParameter(m_scaleMultiplier, ANIM_SCALE_MULTIPLIER_N);
		this->registerPParameter(m_scaleMultiplierVariation, ANIM_SCALE_MULTIPLIER_VARIATION_N);
	}
	~PColorModel()
	{
	}
protected:
	void initPColorModel() 
	{
		//No initialization needed
	}
	void initPColor(typename IParticleContext<PType>::ParticleItt p) {}
	void applyPColor(typename IParticleContext<PType>::ParticleItt p) {}
	void initPColor(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend) 
	{
		if (m_textureTranformTrack || m_diffuseColorTrack) {
			PSRandom& r = this->getPRand();
			float oVarRange = m_animOffset * m_animOffsetVariation;
			float lVarRange = m_animLength * m_animLengthVariation;
			float oVariation = 0.0f;
			float lVariation = 0.0f;
			for (typename IParticleContext<PType>::ParticleItt p = ibegin; p != iend; ++p)
			{
				oVariation = oVarRange == 0.0f ? 0.0f : r.Randf() * oVarRange - oVarRange / 2.0f;
				lVariation = lVarRange == 0.0f ? 0.0f : r.Randf() * lVarRange - lVarRange / 2.0f;
				p->animOffset = m_animOffset + oVariation;
				p->animLength = m_animLength + lVariation;
			}
		}
	}
	void applyPColor(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend) 
	{
		if (m_diffuseColorTrack || m_textureTranformTrack)
		{
			float mappedTimeMs = 0.0f;
			int	key = 0;
			for (typename IParticleContext<PType>::ParticleItt p = ibegin; p != iend; ++p)
			{
				//Map the time depending on mapping type
				mappedTimeMs = m_animKeyMappingType == KS_LIFE ? (p->age * p->animLength / p->life * m_scaleMultiplier + p->animOffset) * 1000.0f : (p->animOffset + p->age) * 1000.0f;
				if (m_textureTranformTrack) {
					collada::animation_track::CTextureTransformEx::SData tranform;
					collada::animation_track::CTextureTransformEx::getValueEx(*m_textureTranformTrack, 
						(int)mappedTimeMs, &tranform, key, false);
					
					const f32 c = cosf(tranform.Rotate);
					const f32 s = sinf(tranform.Rotate);
					p->uTransform.X = c * tranform.ScaleU;
					p->uTransform.Y = -s * tranform.ScaleU;
					p->uTransform.Z = tranform.OffsetU;
					p->vTransform.X = s * tranform.ScaleV;
					p->vTransform.Y = c * tranform.ScaleV;
					p->vTransform.Z = tranform.OffsetV;
				}
				
				if (m_diffuseColorTrack) {
					int hint = s32(mappedTimeMs / 33);
					((collada::CAnimationTrackEx *)m_diffuseColorTrack->pAnimator)->getValue(*m_diffuseColorTrack, s32(mappedTimeMs), &p->color, hint, true);
					//((collada::CAnimationTrackEx *)m_diffuseColorTrack->pAnimator)->getKeyBasedValue(*m_diffuseColorTrack, key, &p->color);
				}
			}
		}
	}
};

}; // end namespace ps
}; // end namespace irr

#endif //__IRR_PCOLOR_MODEL_H__
