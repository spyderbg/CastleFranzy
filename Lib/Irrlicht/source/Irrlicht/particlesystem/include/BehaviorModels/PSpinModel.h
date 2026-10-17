#ifndef __IRR_PSPIN_MODEL_H__
#define __IRR_PSPIN_MODEL_H__

#include <algorithm>
#include "ParticleContext.h"
#include "ParameterNames.h"
#include "PSUtils.h"

namespace irr
{
namespace ps
{

template<class PType>
class PSpinModel
	: public virtual IParticleContext<PType>
{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType)
private:
	float			m_spinTime;
	float			m_spinTimeVar;
	float			m_spinPhase;
	float			m_spinPhaseVar;
	core::vector3df m_spinAxis;
public:
	struct MInit : public PInit
	{
	};

	PSpinModel()
	{
		this->registerPParameter(m_spinTime, SPIN_TIME_N);
		this->registerPParameter(m_spinTimeVar, SPIN_VARIATION_N);
		this->registerPParameter(m_spinPhase, SPIN_PHASE_N);
		this->registerPParameter(m_spinPhaseVar, SPIN_PHASE_VARIATION_N);
		this->registerPParameter(m_spinAxis, SPIN_AXIS_N);
	}
	virtual ~PSpinModel() {}
protected:
	void initPSpinModel() {}
	void initPSpin(typename IParticleContext<PType>::ParticleItt p) {}
	void applyPSpin(typename IParticleContext<PType>::ParticleItt p) {}
	void initPSpin(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend) {}
	void applyPSpin(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend) {}
};

}; // end namespace ps
}; // end namespace irr

#endif //__IRR_PSPIN_MODEL_H__