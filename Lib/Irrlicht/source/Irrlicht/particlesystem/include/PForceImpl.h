#ifndef __IRR_PFORCE_IMPL_H__
#define __IRR_PFORCE_IMPL_H__

#include <math.h>
#include "matrix4.h"
#include "vector3d.h"
#include "PSUtils.h"

#define PARTICLE_ITT typename IParticleContext<ParticleType>::ParticleItt

namespace irr
{
namespace ps
{

template<class ParticleType>
class IParticleContext;

enum PForcePriority
{
	FP_FORCE = 0,
	FP_DEFLECTOR
};


class PGravity
{
public:
	enum PGravityType
	{
		PG_PLANE = 0,
		PG_SPHERE
	};
	struct Parameters
	{
		core::matrix4	*transformation;
		float			strength;
		float			decay;
		PGravityType	type;
	};

	typedef Parameters parameters_type;
	static const int s_priority = FP_FORCE;
private:
	parameters_type		&m_parameters;
public:
	PGravity(parameters_type& p)
		: m_parameters(p)
	{}

	template<class ParticleType>
	void apply(PARTICLE_ITT ibegin,						//See define up top
			   PARTICLE_ITT iend,
			   IParticleContext<ParticleType>* ps)
	{
		core::vector3df force;
		core::matrix4& mat	= *m_parameters.transformation;
		float strength		= m_parameters.strength;
		float decay			= m_parameters.decay;
		PGravityType type	= m_parameters.type;
		
		float dt = ps->getPCurDt();
		float tmpdist = 0.0f;
		float tmpstr = 0.0f;

		PARTICLE_ITT p = ibegin;
		for(; p != iend; ++p)
		{
			tmpstr = strength;
			if (type == PG_PLANE) {
				//Take the front vector -- this should already be normalized
				force.X = mat[8];
				force.Y = mat[9];
				force.Z = mat[10];
				force.normalize();
				if (decay > 0.0f) {
					//Calculate the projected distance from the particule into the force orientation
					tmpdist = fabs(force.dotProduct(p->position - mat.getTranslation()));
					//Adjust the strength with the decay
					tmpstr *= exp(-decay * tmpdist);
				}
			}
			else { //PG_SPHERE
				//Force orientation
				force = mat.getTranslation() - p->position;
				tmpdist = force.getLength();
				if (tmpdist != 0.0f)
					force /= tmpdist;
				if (decay > 0.0f) {
					//Adjust the strength with the decay
					tmpstr *= exp(-decay * tmpdist);
				}
			}
			//Multiply by the time delta
			force *= tmpstr * dt;

			//Apply the force on the particle velocity
			p->velocity += force;
		}
	}
};

class PWind
{
public:
	enum PWindType
	{
		PW_PLANE = 0,
		PW_SPHERE
	};
	struct Parameters
	{
		core::matrix4	*transformation;
		float			strength;
		float			decay;
		float			turbulence;
		float			frequency;
		float			scale;
		PWindType		type;
	};

	typedef Parameters parameters_type;
	static const int s_priority = FP_FORCE;
private:
	parameters_type &m_parameters;
public:
	PWind(parameters_type& p)
		: m_parameters(p)
	{}

	template<class ParticleType>
	void apply(PARTICLE_ITT ibegin, 
			   PARTICLE_ITT iend,
			   IParticleContext<ParticleType>* ps)
	{
		core::vector3df force;
		core::matrix4& mat	= *m_parameters.transformation;
		float strength		= m_parameters.strength;
		float decay			= m_parameters.decay;
		float turbulence	= m_parameters.turbulence;
		float frequency		= m_parameters.frequency;
		PWindType type		= m_parameters.type;
		
		float dt = ps->getPCurDt();
		float tmpdist = 0.0f;
		float tmpstr = 0.0f;
		core::vector3df tmptur;
		core::vector3df tmpvdist;
		core::vector3df tmp;

		PARTICLE_ITT p = ibegin;
		for(; p != iend; ++p)
		{
			//This is exactly like gravity...
			tmpstr = strength;
			if (type == PW_PLANE) {
				//Take the front vector -- this should already be normalized
				force.X = mat[8];
				force.Y = mat[9];
				force.Z = mat[10];
				force.normalize();
				if (decay > 0.0f) {
					//Calculate the projected distance from the particule into the force orientation
					tmpdist = fabs(force.dotProduct(p->position - mat.getTranslation()));
					//Adjust the strength with the decay
					tmpstr *= exp(-decay * tmpdist);
				}
			}
			else { //PG_SPHERE
				//Force orientation
				force = mat.getTranslation() - p->position;
				tmpdist = force.getLength();
				if (tmpdist != 0.0f)
					force /= tmpdist;
				if (decay > 0.0f) {
					//Adjust the strength with the decay
					tmpstr *= exp(-decay * tmpdist);
				}
			}

			force *= tmpstr * dt;

			//Now for the peculiarity of wind
			if (turbulence > 0.0f) {
				tmpvdist = p->position - mat.getTranslation();

				//TODO-FH: Replace by Perlin Noise or something... 
				// since frequency and scale arent really taken into account

				//Generate turbulence through noise function
				tmp = tmpvdist;
				tmp.X = frequency * dt;
				tmptur.X = pRandf() * tmp.X;

				tmp = tmpvdist;
				tmp.Y = frequency * dt;
				tmptur.Y = pRandf() * tmp.Y;

				tmp = tmpvdist;
				tmp.Z = frequency * dt;
				tmptur.Z = pRandf() * tmp.Z;

				//Add turbulence to the force
				force += tmptur * turbulence;
			}

			//NOTE-FH: this should make the force constant?
			//Apply the force on the particle position directly so multiply again by delta time since its acceleration
			p->position += force * dt;
		}
	}
};

class PDeflector
{
public:
	struct Parameters
	{
		core::matrix4	*transformation;
		float			bounce;
		float			variation;
		float			chaos;
		float			friction;
		float			inheritVelocity;
		float			width;
		float			length;
	};

	typedef Parameters parameters_type;
	static const int s_priority = FP_DEFLECTOR;
private:
	parameters_type		&m_parameters;
	core::matrix4		m_lastTransformation;
public:
	PDeflector(parameters_type& p)
		: m_parameters(p)
	{
		m_lastTransformation = *m_parameters.transformation;
	}

	template<class ParticleType>
	void apply(PARTICLE_ITT ibegin, 
			   PARTICLE_ITT iend,
			   IParticleContext<ParticleType>* ps)
	{
		core::matrix4& mat		= *m_parameters.transformation;
		float bounce			= m_parameters.bounce;
		float variation			= m_parameters.variation;
		float chaos				= m_parameters.chaos;
		float friction			= m_parameters.friction;
		float inheritVelocity	= m_parameters.inheritVelocity;

		//Plane component
		core::vector3df planePos = mat.getTranslation();
		//NOTE-FH: in 3dsMax the plane normal is its front vector (z axis)
		//core::vector3df planeZ(mat[8], mat[9], mat[10]);
		//core::vector3df planeX(mat[0], mat[1], mat[2]);
		//core::vector3df normal(mat[4], mat[5], mat[6]);
		core::vector3df planeZ(-mat[4], -mat[5], -mat[6]);
		core::vector3df planeX(mat[0], mat[1], mat[2]);
		core::vector3df normal(mat[8], mat[9], mat[10]);
		normal.normalize();

		float planeLength = planeZ.getLength() * m_parameters.length / 2.0f;
		float planeWidth = planeX.getLength() * m_parameters.width / 2.0f;

		//Last frame plane component
		core::vector3df lastPlanePos = m_lastTransformation.getTranslation();
		//NOTE-FH: in 3dsMax the plane normal is its front vector (z axis)
		//core::vector3df lastPlaneZ(m_lastTransformation[8], m_lastTransformation[9], m_lastTransformation[10]);
		//core::vector3df lastPlaneX(m_lastTransformation[0], m_lastTransformation[1], m_lastTransformation[2]);
		core::vector3df lastPlaneZ(-m_lastTransformation[4], -m_lastTransformation[5], -m_lastTransformation[6]);
		core::vector3df lastPlaneX(m_lastTransformation[0], m_lastTransformation[1], m_lastTransformation[2]);

		//Parameters component
		float dt = ps->getPCurDt();
		float bVarRange = bounce * variation;		

		//Work component
		core::vector3df intersect;
		core::vector3df pVel;
		core::vector3df pVelDt;
		core::vector3df nVel;
		core::vector3df fVel;
		float result = -1.0f;
		float bounceVariation = 0.0f;
		float nVelLen = 0.0f;
		float frictCoef = 0.0f;
		float contactDurFactor = 0.0f;
		float intersectZ = 0.0f;
		float intersectX = 0.0f;

		for(PARTICLE_ITT p = ibegin; p != iend; ++p)
		{
			//TODO-FH: Replace test against 0 for test against Epsilon
			pVel = p->velocity;
			pVelDt = pVel * dt;

			//Check first if the direction of the particle is parralel to the plane
			result = normal.dotProduct(pVelDt);
			if (result == 0.0f) {
				continue;
			}
			//If not they intersect on a single point
			result = normal.dotProduct(planePos - p->position) / result;
			contactDurFactor = 1.0f - result;  //This might be used later for friction
			if (result <= 0.0f || result > 1.0f) {
				continue; //If we are not between p->position and p->position + p->velocity, skip
			}
			//Now check if we are in the actual bounds of the deflector
			intersect = (result * pVelDt) + p->position - planePos;			//vector from plane center to collision point
			//Length
			intersectZ = planeZ.dotProduct(intersect) / planeLength;
			if (intersectZ > 1.0f || intersectZ < -1.0f) {
				continue;
			}
			//Width
			intersectX = planeX.dotProduct(intersect) / planeWidth;
			if (intersectX > 1.0f || intersectX < -1.0f) {
				continue;
			}
			
			//////This particle will collision with the plane//////
			PSRandom& r = ps->getPRand();
			bounceVariation = bVarRange == 0.0f ? 0.0f : r.Randf() * bVarRange - bVarRange / 2.0f;

			//Find the new velocity
			result = normal.dotProduct(pVel);
			fVel = -result * normal + pVel;						//Plane normal perpendicular velocity
			//Module with the bounce factor
			nVelLen = -result * (bounce + bounceVariation);	
			nVel = normal * nVelLen;							//Plane normal parallel velocity
			result = fVel.getLength();

			//Find the friction coefficient  -- result is used as perpendicular velocity length
			frictCoef = GetFrictionCoefficient(friction, nVelLen, result, contactDurFactor* dt);

			//The ricochet velocity
			pVel = nVel + fVel * frictCoef;

			//Apply chaos
			if (chaos > 0.0f) {
				float chaosRange = 180 * chaos;
				pVel.rotateXYBy((r.Randf() * chaosRange) - (chaosRange / 2.0f));
				pVel.rotateYZBy((r.Randf() * chaosRange) - (chaosRange / 2.0f));
				pVel.rotateXZBy((r.Randf() * chaosRange) - (chaosRange / 2.0f));

				//Check if we penetrate the surface
				result = normal.dotProduct(pVel);
				if (result < 0.0f) {
					pVel = -2.0f * result * normal + pVel;
				}
			}

			intersect += planePos; //Intersect is now collision point

			//Apply inherit velocity
			if (inheritVelocity > 0.0f) {
				//Find where the collision point on plane was in the last frame
				pVelDt = intersect - (lastPlanePos + (intersectZ * planeLength * lastPlaneZ) + (intersectX * planeWidth * lastPlaneX)); //Use pVelDt instead of creating a new vector on the stack
				pVel += inheritVelocity * pVelDt;
			}

			//We're done, set the new position and velocity
			p->position = intersect + 0.3f * normal; //Put just above the plane so the particles doesn't go through
			p->velocity = pVel;
		}
		m_lastTransformation = mat;
	}

	float GetFrictionCoefficient(float friction, float normLen, float fricLen, float contactDur)
	{
		static const float angleCoefSmall = 0.035f;
		static const float angleCoefBig = 2.0f * angleCoefSmall;

		float fricCoef;
		if (friction == 0.0f) {
			fricCoef = 1.0f;
		}
		else if (friction == 1.0f) {
			fricCoef = 0.0f;
		}
		else if (normLen >= angleCoefBig*fricLen) {			// angle of reflection is more than 4 degrees: not a "slide"
		  fricCoef = 1.0f - friction;
		}
		else if (normLen < angleCoefSmall*fricLen) {		// angle of reflection is less than 2 degrees: it's a "slide"
		  fricCoef = exp(contactDur * log(1.0f - friction));
		}
		else {
		  float mix = (normLen / fricLen) / angleCoefSmall - 1.0f;
		  fricCoef = mix * (1.0f - friction) + (1.0f - mix)* exp(contactDur*log(1 - friction));
		}
		return fricCoef;
	}
};

class PVortex
{
public:
	struct Parameters
	{
	};

	typedef Parameters parameters_type;
private:
	parameters_type &m_parameters;
public:
	PVortex(parameters_type& p)
		: m_parameters(p)
	{}

	template<class ParticleType>
	void apply(PARTICLE_ITT ibegin, 
			   PARTICLE_ITT iend,
			   IParticleContext<ParticleType>* ps)
	{
	}
};


}; //end namespace ps
}; //end namespace irr

#endif
