#ifndef __IRR_PARTICLE_CONTEXT_H__
#define __IRR_PARTICLE_CONTEXT_H__

#include <vector>
#include <map>
#include <string>
#include <iterator>
#include "vector3d.h"
#include "matrix4.h"

//#ifdef _IRR_COLLADA_MAX_
#pragma warning ( disable : 4250 )
//#endif // _IRR_COLLADA_MAX_

namespace irr
{

namespace ps
{

struct PInit {};

template<class ParticleType>
class PForce;
class PSRandom;
class PRenderData;

//! The Particle system mixin primitive service interface
template<class ParticleType>
class IParticleContext
{
public:
	typedef typename std::vector<ParticleType>		ParticleList;
	typedef typename ParticleList::iterator			ParticleItt;
	typedef typename ParticleList::const_iterator	ParticleConstItt;

	typedef std::map<std::string, void*>			ParamMap;
	typedef typename ParamMap::iterator				ParamItt;

	typedef float									time_t;
	typedef PRenderData								PRenderData_t;
protected:
	ParticleList	m_particles;
	ParamMap		m_parameters;

	float			m_curTime;
	float			m_lastTime;
	float			m_curDt;
public:
	IParticleContext()
		: m_curTime(0), m_lastTime(0), m_curDt(0)
	{}
	IParticleContext(const IParticleContext& ps)
		: m_curTime(ps.m_curTime)
	{
		m_particles.reserve(ps.m_particles.size());
		std::copy(ps.m_particles.begin(), ps.m_particles.end(), std::back_inserter(m_particles));
	}
	virtual ~IParticleContext() {}

	IParticleContext& operator=(const IParticleContext &ps) {
		//NOTE-FH: check size first ?
		m_particles.resize(ps.m_particles.size());
		std::copy(ps.m_particles.begin(), ps.m_particles.end(), m_particles.begin());
	}

	inline float getPCurTime() const {
		return m_curTime;
	}
	inline float getPLastTime() const {
		return m_lastTime;
	}
	inline float getPCurDt() const {
		return m_curDt;
	}
	//TODO-FH: Is it useful to explicitly declare inline here?
	inline ParticleList& getParticleList() {
		return m_particles;
	}
	inline int getParticleCount() const {
		return m_particles.size();
	}
	inline ParticleItt pBegin() {
		return m_particles.begin();
	}
	inline ParticleItt pEnd() {
		return m_particles.end();
	}
	inline ParticleConstItt pBegin() const {
		return m_particles.begin();
	}
	inline ParticleConstItt pEnd() const {
		return m_particles.end();
	}

	inline void trim()
	{
		//Swap trick
		ParticleList(m_particles.begin(), m_particles.end()).swap(m_particles);
	}

	//Parameter accessor functions
	//NOTE-FH: Getting and setting parameters should be done with parcimony since each operation is log(n)
	// Get a void pointer to a registered parameters through behavior model
	void* getParameter(const char* pName)
	{
		//NOTE-FH: Use find instead?
		return m_parameters[pName];
	}

	// Set a parameter registered through behavior models
	template<class T>
	void setParameter(const char*pName, T value)
	{
		//NOTE-FH: Use find instead?
		void* paramPtr = m_parameters[pName];
		if (paramPtr) {
			*(reinterpret_cast<T*>(paramPtr)) = value;
		}
	}

	// Particle render data model public functions
	virtual	PRenderData_t&	getRenderData(int index = 0) = 0;
	virtual size_t			getRenderBufferSizeNeeded() const = 0;
	virtual void			setRenderBuffer(void* buffer, size_t size) = 0;

	//Random number generator
	virtual PSRandom& getPRand() = 0;

protected:
	// Parameters management functions
	template<class T>
	void registerPParameter(T& param, const char* pName)
	{
		m_parameters.insert(ParamMap::value_type(pName, reinterpret_cast<void*>(&param)));
	}

	// Scene node functions 
	virtual const core::matrix4* getWorldMatrixPtr() const = 0;

	// Particle generation model functions
	virtual void		initGenerationModel() = 0;
	virtual ParticleItt	generateParticles() = 0;

	// Particle size model functions
	virtual void initPSizeModel() = 0;
	virtual void initPSize(ParticleItt p) = 0;
	virtual void applyPSize(ParticleItt p) = 0;
	virtual void initPSize(ParticleItt ibegin, ParticleItt iend) = 0;
	virtual void applyPSize(ParticleItt ibegin, ParticleItt iend) = 0;

	// Particle color model functions
	virtual void initPColorModel() = 0;
	virtual void initPColor(ParticleItt p) = 0;
	virtual void applyPColor(ParticleItt p) = 0;
	virtual void initPColor(ParticleItt ibegin, ParticleItt iend) = 0;
	virtual void applyPColor(ParticleItt ibegin, ParticleItt iend) = 0;

	// Particler emitter model functions
	virtual void initPEmitterModel() = 0;
	virtual void initPPosition(ParticleItt p) = 0;
	virtual void initPPosition(ParticleItt ibegin, ParticleItt iend) = 0;

	// Particle motion model functions
	virtual void initPMotionModel() = 0;
	virtual void initPMotion(ParticleItt p) = 0;
	virtual void initPMotion(ParticleItt ibegin, ParticleItt iend) = 0;
	virtual void applyPMotion(ParticleItt p) = 0;
	virtual void applyPMotion(ParticleItt ibegin, ParticleItt iend) = 0;

	// Particle forces model functions
	virtual int	 addPForce(PForce<ParticleType> *force) = 0;
	virtual void removePForce(PForce<ParticleType> *force) = 0;
	virtual void removePForce(int index) = 0;
	virtual void initPForcesModel() = 0;
	virtual void initPForces(ParticleItt p) = 0;
	virtual void applyPForces(ParticleItt p) = 0;
	virtual void initPForces(ParticleItt ibegin, ParticleItt iend) = 0;
	virtual void applyPForces(ParticleItt ibegin, ParticleItt iend) = 0;

	// Particle spin model functions
	virtual void initPSpinModel() = 0;
	virtual void initPSpin(ParticleItt p) = 0;
	virtual void applyPSpin(ParticleItt p) = 0;
	virtual void initPSpin(ParticleItt ibegin, ParticleItt iend) = 0;
	virtual void applyPSpin(ParticleItt ibegin, ParticleItt iend) = 0;

	// Particle life model functions
	virtual void initPLifeModel() = 0;
	virtual void initPLife(ParticleItt p) = 0;
	virtual void applyPLife(ParticleItt p) = 0;
	virtual void initPLife(ParticleItt ibegin, ParticleItt iend) = 0;
	virtual void applyPLife(ParticleItt ibegin, ParticleItt iend) = 0;

	// Particle render data model functions
	virtual void initPRenderDataModel() = 0;
	virtual void initPRenderData(ParticleItt p) = 0;
	virtual void applyPRenderData(ParticleItt p) = 0;
	virtual void initPRenderData(ParticleItt ibegin, ParticleItt iend) = 0;
	virtual void applyPRenderData(ParticleItt ibegin, ParticleItt iend) = 0;
};

} // end namespace ps
} // end namespace irr

#endif //__IRR_PARTICLE_CONTEXT_H__
