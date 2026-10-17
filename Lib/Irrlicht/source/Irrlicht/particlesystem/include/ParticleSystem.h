#ifndef __IRR_PARTICLE_SYSTEM_H__
#define __IRR_PARTICLE_SYSTEM_H__

#include "ParticleContext.h"
#include "PSUtils.h"

namespace irr
{
namespace ps
{
	
template<class ParticleType, class Force>
class PForceProxy;

//TODO-FH: Derive from ReferenceCounted or use smart pointers in PSManager?

//! The Particle system client code interface
template<class ParticleType>
class IParticleSystem 
	: public virtual IParticleContext<ParticleType>
{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(ParticleType)
private:
	//Random number generator specific to the particle system
	PSRandom			m_rand;
	core::matrix4*		m_pWorldMatrix;

protected:
	PSRandom&	getPRand() {
		return m_rand;
	}
	const core::matrix4* getWorldMatrixPtr() const {
		return m_pWorldMatrix;
	}
public:
	IParticleSystem() : m_pWorldMatrix(0) {}
	//IParticleSystem(const IParticleSystem& ps);
	virtual ~IParticleSystem() {}

	//Set the world matrix reference for the particle system
	void setWorldMatrixPtr(core::matrix4* world)
	{
		m_pWorldMatrix = world;
	}

	template<class Force>
	int bindForce(typename Force::parameters_type& param)
	{
		return this->addPForce(irrnew PForceProxy<ParticleType, Force>(param));
	}

	template<class Force>
	void unbindForce(typename Force::parameters_type& param)
	{
		PForceProxy<ParticleType, Force> pforce(param);
		this->removePForce(&pforce);
	}

	void unbindForce(int index)
	{
		this->removePForce(index);
	}

	//! initialize the particle system
	//~ This function should be called after initial parameters are set through SetParameter
	void init()
	{
		m_rand.Reset();
		this->m_lastTime = 0.0f;
		this->m_curTime = 0.0f;
		this->initGenerationModel();
		this->initPSizeModel();
		this->initPColorModel();
		this->initPEmitterModel();
		this->initPMotionModel();
		this->initPForcesModel();
		this->initPSpinModel();
		this->initPLifeModel();
		this->initPRenderDataModel();
	}

	void update(float curTime)
	{
		//TODO-FH: Looping and extended time management?
		float lastTime	= this->m_curTime;
		float curDt	= curTime - lastTime;
		if (curDt < 0)
			init();

		//Update time
		this->m_lastTime	= this->m_curTime;
		this->m_curTime		= curTime;
		this->m_curDt		= this->m_curTime - this->m_lastTime;

		//generate new particules using the generation model
		typename IParticleContext<ParticleType>::ParticleItt itt		= this->generateParticles();
		typename IParticleContext<ParticleType>::ParticleItt begin		= this->pBegin();  //Use pBegin()?
		typename IParticleContext<ParticleType>::ParticleItt end		= this->pEnd();

		//initialize the new particules
		this->initPColor(itt, end);
		this->initPSize(itt, end);
		this->initPPosition(itt, end); 
		this->initPForces(itt, end);
		this->initPMotion(itt, end);
		this->initPSpin(itt, end);
		this->initPLife(itt, end);

		this->initPRenderData(itt, end);

		//Update all particles
		this->applyPColor(begin, end);		//Apply color modification, may be relative to age
		this->applyPSize(begin, end);		//Apply size modification, may be relative to age
		this->applyPForces(begin, end);		//Apply any present forces -- may alter velocity or position or paritcles i.e.: gravity
		this->applyPMotion(begin, end);		//Apply velocity and acceleration, if any; might have been modified by forces
		this->applyPSpin(begin, end);		//Apply rotation and spin
		this->applyPLife(begin, end);		//Age the particle and remove any dead particles, this will invalidate iterators

		this->applyPRenderData(begin, end); //Apply any behavior needed before rendering i.e.: sorting for alpha
	}

	//! Used to get a pointer to the internal storage of particles as well as an offset to the common particles parameters
	//	that should at least be present in ParticleType.
	//  are passed as parameters.
	//+ Returns the number of particles in the system
	//~ Note: we need to be especialy wary when using this function as IParticleSystem doesn't know 
	//  exactly how ParticleType is laid out in memory, nor the real type of its parameters, the user needs to.
	int getPGeneralPointers(float *&ptr, size_t &stride, size_t &pos3Ofs, size_t &vel3Ofs, size_t &colorOfs, size_t &age1Ofs, size_t &sizeOfs)
	{
		int pCount = this->getParticleCount();
		if (pCount < 1) {
			ptr = 0;
			stride = 0;
			return 0;
		}

		typename IParticleContext<ParticleType>::ParticleItt itt = this->pBegin();
		ParticleType* p0 = &(*itt);
		float *fPptr0 = reinterpret_cast<float*>(p0);
		ptr = fPptr0;
		if (pCount > 1) {
			ParticleType* p1 = &(*(itt+1));
			float *fPptr1 = reinterpret_cast<float*>(p1);
			stride = fPptr1 - fPptr0;
		}
		else {
			stride = 0;
		}

		pos3Ofs = reinterpret_cast<float*>(&p0->position) - fPptr0;
		vel3Ofs = reinterpret_cast<float*>(&p0->velocity) - fPptr0;
		colorOfs = reinterpret_cast<float*>(&p0->color) - fPptr0;
		age1Ofs = reinterpret_cast<float*>(&p0->age) - fPptr0;
		sizeOfs = reinterpret_cast<float*>(&p0->size) - fPptr0;

		return pCount;
	}

	typename IParticleContext<ParticleType>::PRenderData_t& getParticleRenderData()
	{
		return this->getRenderData();
	}
};

/// Some helper functions
/////////////////////////////////////////////////////////////////////////

template<class PType>
static int PSIncrementalUpdate(IParticleSystem<PType>* ps, float curTime, float timeDt, float frameDt)
{
	int uCount = timeDt / frameDt;
	for (int i = 0; i <= uCount; ++i) {
		ps->Update(curTime + i * frameDt);
	}
	if (uCount * frameDt != timeDt) {
		ps->Update(curTime + timeDt);
		return ++uCount;
	}
	return uCount;
}

} // end namespace ps
} // end namespace irr

#endif //__IRR_PARTICLE_SYSTEM_H__