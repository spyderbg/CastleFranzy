#ifndef __IRR_PFORCE_H__
#define __IRR_PFORCE_H__

namespace irr
{
namespace ps
{

template<class ParticleType>
class IParticleContext;


template<class ParticleType>
class PForce
{
public:
	typedef typename IParticleContext<ParticleType>::ParticleItt ParticleItt;
private:
	void*	m_cmpPtr;	//For equality comparison
	int		m_priority;	//For equivalence comparison
public:
	PForce(void* ptr, int priority)
		: m_cmpPtr(ptr)
		, m_priority(priority)
	{}

	virtual void apply(ParticleItt ibegin, ParticleItt iend, IParticleContext<ParticleType>* ps) = 0;

	bool operator==(const PForce<ParticleType>& other) const
	{
		return m_cmpPtr == other.m_cmpPtr;
	}
	bool operator!=(const PForce<ParticleType>& other) const
	{
		return m_cmpPtr != other.m_cmpPtr;
	}
	bool operator<(const PForce<ParticleType>& other) const
	{
		return m_priority < other.m_priority;
	}
	bool operator<=(const PForce<ParticleType>& other) const
	{
		return m_priority <= other.m_priority;
	}
	bool operator>(const PForce<ParticleType>& other) const
	{
		return m_priority > other.m_priority;
	}
	bool operator>=(const PForce<ParticleType>& other) const
	{
		return m_priority >= other.m_priority;
	}
};

template<class ParticleType, class Force>
class PForceProxy : public PForce<ParticleType>, public Force
{
public:
	typedef typename Force::parameters_type				parameters_type;
	typedef typename PForce<ParticleType>::ParticleItt	ParticleItt;
public:
	PForceProxy(parameters_type& p)
		: PForce<ParticleType>(&p, Force::s_priority)
		, Force(p)
	{
	}

	void apply(ParticleItt ibegin, ParticleItt iend, IParticleContext<ParticleType>* ps)
	{
		Force::apply(ibegin, iend, ps);
	}
};

template<class ParticleType>
class CompareForce
{
private:
	PForce<ParticleType>* m_force;
public:
	CompareForce(PForce<ParticleType>* force) : m_force(force) {}

	bool operator()(PForce<ParticleType>* force) const
	{
		return (*m_force) == (*force);
	}
};

template<class ParticleType>
class SortPriorityForce : public std::binary_function<const PForce<ParticleType>*, const PForce<ParticleType>*, bool>
{
public:
	SortPriorityForce() {}

	bool operator()(const PForce<ParticleType>* force1, const PForce<ParticleType>* force2) const
	{
		return (*force1) < (*force2);
	}
};



}; //end namespace ps
}; //end namespace irr

#endif
