#ifndef __IRR_PEMITTER_MODEL_H__
#define __IRR_PEMITTER_MODEL_H__

#include <algorithm>
#include "ParticleContext.h"
#include "ParameterNames.h"
#include "PSUtils.h"
#include "PDomain.h"
#include "quaternion.h"

namespace irr
{
namespace ps
{

template<class PType>
class PEmitterModel
	: public virtual IParticleContext<PType>
{
//DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType)
public:
	enum PSEmitterType
	{
		ET_BOX = 0,
		ET_SPHERE,
		ET_CYLINDER
	};
private:
	PDomain*		m_emitterShape;
	PSEmitterType	m_emitterType;
	float			m_radiusLength;
	float			m_width;
	float			m_height;
public:
	PEmitterModel()
		: m_emitterShape(irrnew PDBox(core::vector3df(-1.0f, -1.0f, -1.0f), core::vector3df(1.0f, 1.0f, 1.0f)))
		, m_emitterType(ET_BOX)
		, m_radiusLength(2.0f)
		, m_width(2.0f)
		, m_height(2.0f)
	{
		this->registerPParameter(m_emitterType, EMITTER_TYPE_N);
		this->registerPParameter(m_radiusLength, RADIUS_LENGTH_N);
		this->registerPParameter(m_width, WIDTH_N);
		this->registerPParameter(m_height, HEIGHT_N);
	}
	virtual ~PEmitterModel()
	{
		if (m_emitterShape) {
			delete m_emitterShape;
		}
	}
protected:
	void initPEmitterModel()
	{
		//initialize settings
		if (m_emitterShape) {
			delete m_emitterShape;
			m_emitterShape = 0;
		}
		switch (m_emitterType)
		{
			case ET_BOX:
				{
					m_emitterShape = irrnew PDBox(m_width, m_height, m_radiusLength);
				}
				break;
			case ET_SPHERE:
				{
					m_emitterShape = irrnew PDSphere(core::vector3df(), m_radiusLength);
				}
				break;
			case ET_CYLINDER:
				{
					m_emitterShape = irrnew PDCylinder(m_height, m_radiusLength);
				}
				break;
		}
	}

	void initPPosition(typename IParticleContext<PType>::ParticleItt p)
	{
		PSRandom& r = this->getPRand();
		//Transform the shape
		if (this->getWorldMatrixPtr()) {
			m_emitterShape->transform(*this->getWorldMatrixPtr());
		}
		//generate the position
		p->position = m_emitterShape->generate(r);
	}
	void initPPosition(typename IParticleContext<PType>::ParticleItt ibegin, typename IParticleContext<PType>::ParticleItt iend)
	{
		PSRandom& r = this->getPRand();
		//Transform the shape first
		if (this->getWorldMatrixPtr()) {
			m_emitterShape->transform(*(this->getWorldMatrixPtr()));
		}
		for (typename IParticleContext<PType>::ParticleItt p = ibegin; p != iend; ++p)
		{
			//generate the position in local coordinates
			p->position = m_emitterShape->generate(r);
		}
	}
};

}; // end namespace ps
}; // end namespace irr

#endif //__IRR_PEMITTER_MODEL_H__