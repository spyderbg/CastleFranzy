#ifndef __IRR_PLIFE_MODEL_H__
#define __IRR_PLIFE_MODEL_H__

#include <algorithm>
#include "ParticleContext.h"
#include "ParameterNames.h"
#include "PSUtils.h"

namespace irr
{
namespace ps
{

template<class PType>
class PLifeModel
	: public virtual IParticleContext<PType>
{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType)
private:
	float		m_life;
	float		m_lifeVar;
public:
	PLifeModel()
	{
		this->registerPParameter(m_life, LIFE_N);
		this->registerPParameter(m_lifeVar, LIFE_VARIATION_N);
	}
	virtual ~PLifeModel() {}
protected:
	void initPLifeModel()
	{
		//Nothing to initialize
	}

	void	initPLife(typename IParticleContext<PType>::ParticleItt p)
	{
		PSRandom& r = this->getPRand();
		//NOTE-FH:  m_lifeVar is a variation in sec not in percentage
		//float varRange = m_life * m_lifeVar;
		float variation = r.Randf() * m_lifeVar - m_lifeVar / 2.0f;
		p->age = 0.0f;
		p->life = m_life + variation;
	}

	void	applyPLife(typename IParticleContext<PType>::ParticleItt p)
	{
		p->age += this->m_curDt;
		//TODO-FH: make old particle die!
		//NOTE-FH: removing the particles here will not be possible without invalidating the iterator
	}

	void	initPLife(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend)
	{
		PSRandom& r = this->getPRand();
		//NOTE-FH:  m_lifeVar is a variation in sec not in percentage
		//float varRange = m_life * m_lifeVar;
		for (typename IParticleContext<PType>::ParticleItt p = ibegin; p != iend; ++p)
		{
			//float variation = varRange ? ((os::Randomizer::rand() % static_cast<int>(varRange)) - (static_cast<int>(varRange) / 2.0f)) / 100.0f : 0.0f;
			float variation = r.Randf() * m_lifeVar - m_lifeVar / 2.0f;
			p->age = 0.0f;
			p->life = m_life + variation;
		}
	}

	void	applyPLife(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend)
	{
		//Age and remove the particles
		this->m_particles.erase(std::remove_if(ibegin, iend, AgeNKillCheck<PType>(this->m_curDt)), iend);
	}
};

}; // end namespace ps
}; // end namespace irr

#endif //__IRR_PLIFE_MODEL_H__