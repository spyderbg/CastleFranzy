#ifndef __IRR_PGENERATION_MODEL_H__
#define __IRR_PGENERATION_MODEL_H__

#include <algorithm>
#include "ParticleContext.h"
#include "ParameterNames.h"
#include "PSUtils.h"

namespace irr
{
namespace ps
{

//NOTE-FH: The spawn model should probably be part of the generation model
template<typename PType>
class PGenerationModel
	: public virtual IParticleContext<PType>
{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType)
private:
	float	m_birthRate;
	int		m_maxParticles;
	float	m_emitDt;
	float	m_particleLeftOver;
public:

	PGenerationModel()
		: m_birthRate(1)
		, m_maxParticles(1)
		, m_emitDt(.0f)
		, m_particleLeftOver(.0f)
	{
		this->registerPParameter(m_birthRate, BIRTH_RATE_N);
		this->registerPParameter(m_maxParticles, MAX_PARTICLES_N);
	}
	virtual ~PGenerationModel() {}
protected:
	void initGenerationModel()
	{
		m_particleLeftOver = 0.0f;
		this->m_particles.clear();
		this->m_particles.reserve(m_maxParticles);
	}

	typename IParticleContext<PType>::ParticleItt	generateParticles()
	{
		int oldSize = this->m_particles.size();
		float curTime = this->m_curTime;
		float lastTime = this->m_lastTime;
		m_emitDt = curTime - lastTime;

		//TODO-FH: Add Variance ?
		int iPCount = int(m_emitDt * m_birthRate + m_particleLeftOver);
		float fPCount = m_emitDt * m_birthRate + m_particleLeftOver;
		m_particleLeftOver = fPCount - iPCount;
		
		//if there's any particle, generate them
		if (iPCount > 0) {

			//Check we don't get over the particles limit
			int newSize =  oldSize + iPCount;
			if (newSize > m_maxParticles && m_maxParticles != 0)
				newSize = m_maxParticles;

			//generate default particles in the container
			this->m_particles.resize(newSize);

			//Return an iterator to the beginning of the newly generated particles
			typename IParticleContext<PType>::ParticleItt itt = this->pBegin();
			std::advance(itt, oldSize);
			return itt;
		}
		return this->pEnd();
	}
};

}; // end namespace ps
}; // end namespace irr

#endif //__IRR_PGENERATION_MODEL_H__
