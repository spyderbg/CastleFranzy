#ifndef __IRR_PSIZE_MODEL_H__
#define __IRR_PSIZE_MODEL_H__

#include <algorithm>
#include "ParticleContext.h"
#include "ParameterNames.h"
#include "PSUtils.h"

namespace irr
{
namespace ps
{

template<typename PType>
class PSizeModel
	: public virtual IParticleContext<PType>
{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType)
private:
	float	m_targetSize;
	float	m_sizeVar;
	float	m_sizeGrowthTime;
	float	m_sizeFadeTime;
public:

	PSizeModel()
		: m_targetSize(1.0f)
		, m_sizeVar(0.0f)
		, m_sizeGrowthTime(0.0f)
		, m_sizeFadeTime(0.0f)
	{
		this->registerPParameter(m_targetSize, TARGET_SIZE_N);
		this->registerPParameter(m_sizeVar, SIZE_VARIATION_N);
		this->registerPParameter(m_sizeGrowthTime, SIZE_GROWTH_TIME_N);
		this->registerPParameter(m_sizeFadeTime, SIZE_FADE_TIME_N);
	}
	virtual ~PSizeModel() {}
protected:
	void initPSizeModel()
	{
		//Nothing to initialize

		//Reinit current living particles however
		//initPSize(pBegin(), pEnd());
	}

	void initPSize(typename IParticleContext<PType>::ParticleItt p)
	{
		PSRandom& r = this->getPRand();
		float varRange = m_targetSize * m_sizeVar;
		float variation = r.Randf() * varRange - varRange / 2.0f;

		if (m_sizeGrowthTime > 0) {
			p->size = 0.0f;
			p->targetSize = m_targetSize + variation;
		}
		else {
			p->targetSize = m_targetSize + variation;
			p->size = p->targetSize;
		}
	}
	void applyPSize(typename IParticleContext<PType>::ParticleItt p)
	{
		p->size = p->targetSize;
		if (m_sizeGrowthTime > 0.0f && p->age < m_sizeGrowthTime)
		{
			float d = p->age / m_sizeGrowthTime;
			p->size = p->targetSize * d;
		}
		if (m_sizeFadeTime > 0.0f && p->age < m_sizeFadeTime)
		{
			float d = (p->life - p->age) / m_sizeFadeTime;
			p->size = p->targetSize * d;
		}
	}
	void initPSize(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend)
	{
		PSRandom& r = this->getPRand();
		float varRange = m_targetSize * m_sizeVar;
		for (typename IParticleContext<PType>::ParticleItt p = ibegin; p != iend; ++p)
		{
			float variation = r.Randf() * varRange - varRange / 2.0f;

			if (m_sizeGrowthTime > 0) {
				p->size = 0.0f;
				p->targetSize = m_targetSize + variation;
			}
			else {
				p->targetSize = m_targetSize + variation;
				p->size = p->targetSize;
			}
		}
	}
	void applyPSize(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend)
	{
		for (typename IParticleContext<PType>::ParticleItt p = ibegin; p != iend; ++p)
		{
			p->size = p->targetSize;
			if (m_sizeGrowthTime > 0.0f && p->age < m_sizeGrowthTime)
			{
				float d = p->age / m_sizeGrowthTime;
				p->size = p->targetSize * d;
			}
			if (m_sizeFadeTime > 0.0f && p->life - p->age < m_sizeFadeTime)
			{
				float d = (p->life - p->age) / m_sizeFadeTime;
				p->size = p->targetSize * d;
			}
		}
	}
};

}; // end namespace ps
}; // end namespace irr

#endif //__IRR_PSIZE_MODEL_H__