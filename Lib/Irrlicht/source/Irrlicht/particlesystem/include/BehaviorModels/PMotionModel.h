#ifndef __IRR_PMOTION_MODEL_H__
#define __IRR_PMOTION_MODEL_H__

#include <algorithm>
#include "ParticleContext.h"
#include "ParameterNames.h"
#include "PSUtils.h"

namespace irr
{
namespace ps
{

	template<class PType>
class PMotionModel
	: public virtual IParticleContext<PType>
{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType)
private:
	core::vector3df m_dir;
	float			m_dirVar;
	float			m_speed;
	float			m_speedVar;
public:
	struct MInit : public PInit
	{
	};

	PMotionModel()
	{
		this->registerPParameter(m_dir, DIRECTION_N);
		this->registerPParameter(m_dirVar, DIR_VARIATION_N);
		this->registerPParameter(m_speed, SPEED_N);
		this->registerPParameter(m_speedVar, SPEED_VARIATION_N);
	}
	virtual ~PMotionModel() {}
protected:
	void initPMotionModel()
	{
		//Nothing to initialize

		//Reinit current living particles however if any
		//initPMotion(pBegin(), pEnd());
	}

	void initPMotion(typename IParticleContext<PType>::ParticleItt p)
	{
		PSRandom& r = this->getPRand();
		core::matrix4 mat = this->getWorldMatrixPtr() ? *this->getWorldMatrixPtr() : core::matrix4();
		float sVarRange = m_speed * m_speedVar;		
		float sVariation = r.Randf() * sVarRange - sVarRange / 2.0f;

		//if m_dir is null the direction is random
		if (m_dir == core::vector3df()) //TODO-FH: check a bool for random instead of computing length
		{
			p->velocity = (r.RandVec() - vHalf).normalize();
			p->velocity *= m_speed + sVariation;
		}
		else
		{
			core::vector3df dir = core::vector3df(.0f, .0f, 1.0f);
			if (m_dirVar > 0) {
				//float theta = r.Randf() * 2.0f * float(M_PI);						// Angle around m_dir
				//float v = (cosf(float(M_PI) * (1.0f - m_dirVar)) + 1.0f) / 2.0f;
				//float phi = acosf((2.0f * r.Randf() * v) - 1.0f);					// Angle around othogonal vector to m_dir
				//
				//core::quaternion quat;
				//quat.rotationFromTo(dir, m_dir);
				//dir.rotateXZBy(phi * core::RADTODEG64);
				//dir.rotateXYBy(theta * core::RADTODEG64);
				//dir = quat * dir;

				core::vector3df tgt = m_dir;
				float dirVarRange = 180 * m_dirVar;
				mat.transformVect(tgt);
				tgt.rotateXYBy((r.Randf() * dirVarRange) - (dirVarRange / 2.0f));
				tgt.rotateYZBy((r.Randf() * dirVarRange) - (dirVarRange / 2.0f));
				tgt.rotateXZBy((r.Randf() * dirVarRange) - (dirVarRange / 2.0f));
				dir = tgt;
			}
			else {
				dir = m_dir;
			}

			p->velocity = dir.normalize() * (m_speed + sVariation);
		}
	}
	void initPMotion(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend)
	{
		PSRandom& r = this->getPRand();
		core::matrix4 mat = this->getWorldMatrixPtr() ? *this->getWorldMatrixPtr() : core::matrix4();
		float sVarRange = m_speed * m_speedVar;
		core::vector3df dir = core::vector3df(.0f, .0f, 1.0f);
		float v = 0.0f;
		//core::quaternion quat;
		//if (m_dirVar > 0.0f) {
		//	v = (cosf(float(M_PI) * (1.0f - m_dirVar)) + 1.0f) / 2.0f;
		//	quat.rotationFromTo(dir, m_dir);
		//}
		//else {
		//	dir = m_dir;
		//}

		for (typename IParticleContext<PType>::ParticleItt p = ibegin; p != iend; ++p)
		{
			float sVariation = (r.Randf() * sVarRange) - (sVarRange / 2.0f);

			//if m_dir is null the direction is random
			if (m_dir == core::vector3df())
			{
				p->velocity = (r.RandVec() - vHalf).normalize();
				p->velocity *= m_speed + sVariation;
			}
			else
			{
				if (m_dirVar > 0.0f) {
					//float theta = r.Randf() * 2.0f * float(M_PI);						// Angle around m_dir
					//float phi = acosf((2.0f * r.Randf() * v) - 1.0f);					// Angle around othogonal vector to m_dir
					//
					//dir.rotateXZBy(phi * core::RADTODEG64);
					//dir.rotateXYBy(theta * core::RADTODEG64);
					//dir = quat * dir;

					core::vector3df tgt = m_dir;
					float dirVarRange = 180 * m_dirVar;
					//mat.transformVect(tgt);
					tgt.rotateXYBy((r.Randf() * dirVarRange) - (dirVarRange / 2.0f));
					tgt.rotateYZBy((r.Randf() * dirVarRange) - (dirVarRange / 2.0f));
					tgt.rotateXZBy((r.Randf() * dirVarRange) - (dirVarRange / 2.0f));
					dir = tgt;
				}
				else {
					dir = m_dir;
				}

				p->velocity = dir.normalize() * (m_speed + sVariation);
			}
		}
	}

	void applyPMotion(typename IParticleContext<PType>::ParticleItt p)
	{
		p->position += p->velocity * this->m_curDt;
	}
	
	void applyPMotion(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend)
	{
		for (typename IParticleContext<PType>::ParticleItt p = ibegin; p != iend; ++p)
		{
			p->position += p->velocity * this->m_curDt;
		}
	}
};

}; // end namespace ps
}; // end namespace irr

#endif //__IRR_PMOTION_MODEL_H__