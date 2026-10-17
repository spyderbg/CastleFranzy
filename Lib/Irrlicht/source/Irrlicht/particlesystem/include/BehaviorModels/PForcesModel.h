#ifndef __IRR_PFORCES_MODEL_H__
#define __IRR_PFORCES_MODEL_H__

#include <algorithm>
#include "ParticleContext.h"
#include "ParameterNames.h"
#include "PSUtils.h"
#include "PForce.h"

namespace irr
{
namespace ps
{

template<class PType>
class PForcesModel
	: public virtual IParticleContext<PType>
{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType)
public:
	typedef std::vector< PForce<PType>* >		PForceList;
	typedef typename PForceList::iterator		PForceItt;
private:
	PForceList	m_forces;
	bool		m_forceDirty;

	void sortForces() {
		if (m_forceDirty) {
			std::sort(m_forces.begin(), m_forces.end(), SortPriorityForce<PType>());
			m_forceDirty = false;
		}
	}
public:
	PForcesModel()
		: m_forceDirty(false)
	{}
	virtual ~PForcesModel()
	{
		std::for_each(m_forces.begin(), m_forces.end(), DeleteFunctor< PForce<PType> >());
	}
protected:
	int	 addPForce(PForce<PType> *force) 
	{
		m_forces.push_back(force);
		m_forceDirty = true;
		return m_forces.size();
	}
	void removePForce(PForce<PType> *force) 
	{
		PForceItt it = std::find_if(m_forces.begin(), m_forces.end(), CompareForce<PType>(force));
		if (it != m_forces.end()) {
			delete (*it);
			m_forces.erase(it);
			m_forceDirty = true;
		}
	}

	void removePForce(int index) 
	{
		if (index > 0 && index < (int)m_forces.size()) {
			PForceItt it = m_forces.begin();
			std::advance(it, index);
			delete (*it);
			m_forces.erase(it);
		}
	}

	void initPForcesModel() 
	{
		//Sort Forces
		sortForces();
	}

	void initPForces(typename IParticleContext<PType>::ParticleItt p) {}
	void applyPForces(typename IParticleContext<PType>::ParticleItt p) {}

	void initPForces(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend)
	{
		//No initialization needed
	}
	void applyPForces(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend) 
	{
		sortForces(); //Check if we need to sort forces
		PForceItt forceItt = m_forces.begin();
		PForceItt forceEnd = m_forces.end();
		for(; forceItt != forceEnd; ++forceItt)
		{
			(*forceItt)->apply(ibegin, iend, this);
		}
	}
};

}; // end namespace ps
}; // end namespace irr

#endif //__IRR_PFORCES_MODEL_H__
