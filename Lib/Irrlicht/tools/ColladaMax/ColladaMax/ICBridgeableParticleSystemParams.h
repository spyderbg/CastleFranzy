/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_PARTICLE_SYSTEM_PARAMS_H
#define _IC_PARTICLE_SYSTEM_PARAMS_H

#include <vector>

#undef snprintf
#include "PSManager.h"

class FCDConversionFunctor;
class IParamBlock;

// IICBridgeableParticleSystemUIParam = Interface for IrrCollada UI parameters
// If was a fanatical it would be named IICBPSUIParam = Interface for IrrCollada Bridgeable Particle System UI Parameters
class IICBridgeableParticleSystemUIParam
{
protected:
	// OPTIM: ParamBlock for particle system ui params is the same for all params.
	// We could remove it from this struct and pass it as parameter to the funcs
	// that need to access the ParamBlock
	IParamBlock *m_pBlock; 
	int m_pbid;
	FCDConversionFunctor *m_pConversionFunctor;
	int m_nameIds;
	ParamDimension *m_pDimension;

public:
	IICBridgeableParticleSystemUIParam( IParamBlock *pBlock
		, int pbid
		, FCDConversionFunctor *pConversionFunctor 
		, int nameIds
		, ParamDimension *pDimension
		)
		: m_pBlock( pBlock )
		, m_pbid( pbid )
		, m_pConversionFunctor( pConversionFunctor )
		, m_nameIds( nameIds )
		, m_pDimension( pDimension )
	{
		FUAssert( m_pBlock, );
		FUAssert( m_pbid >= 0, );
	}

	// Type unsafe - only derived type knows pVal type
	virtual int GetPBlockValue( void *pVal, TimeValue t, Interval &interval ) = 0;
	virtual void *SetSystemValuePtr( void *p ) = 0;
	virtual void *GetSystemValuePtr() const = 0;

	virtual int UpdateSystemValueUsingPBlockValue( TimeValue t, Interval &interval ) = 0;

	inline IParamBlock *GetPBlockPtr()
	{ 
		return m_pBlock; 
	}
	
	inline void SetPBlockPtr( IParamBlock *pBlock )
	{ 
		m_pBlock = pBlock; 
	}

	inline int GetNameIds() const
	{
		return m_nameIds;
	}

	inline ParamDimension *GetDimensionPtr()
	{
		return m_pDimension;
	}

	inline int GetParamBlockId() const
	{
		return m_pbid;
	}

protected:

	virtual int SetPBlockValue( void *pVal, TimeValue t ) = 0;
};

typedef std::vector<IICBridgeableParticleSystemUIParam *> InParamList;

template <class UIType, class SystemType>
class ICBridgeableParticleSystemUIParam : public IICBridgeableParticleSystemUIParam
{
private:
	UIType m_uiDefaultValue;
	SystemType *m_pSystemValue;

public:
	ICBridgeableParticleSystemUIParam( IParamBlock *pBlock
		, int pbid
		, UIType uiDefaultValue
		, SystemType *pSystemValue
		, FCDConversionFunctor *pConversionFunctor
		, int nameIds = -1
		, ParamDimension *pDimension = defaultDim
		) 
		: IICBridgeableParticleSystemUIParam( pBlock, pbid, pConversionFunctor, nameIds, pDimension )
		, m_uiDefaultValue( uiDefaultValue )
		, m_pSystemValue( pSystemValue )
	{
		SetPBlockValue( &m_uiDefaultValue, 0 );
	}

	int UpdateSystemValueUsingPBlockValue( TimeValue t, Interval &interval )
	{
		if ( !m_pSystemValue )
		{
			return IC_ERROR;
		}

		UIType paramBlockVal = ( UIType )0;
		int errCode = IC_ERROR;
		errCode = GetPBlockValue( &paramBlockVal, t, interval );

		if ( errCode == IC_OK )
		{
			*m_pSystemValue = ( SystemType )( m_pConversionFunctor
				? (*m_pConversionFunctor)( paramBlockVal )
				: paramBlockVal );
		}

		return errCode;
	}

	void *SetSystemValuePtr( void *p )
	{
		void *pCurrentValue = m_pSystemValue;
		m_pSystemValue = ( SystemType * )p;
		return pCurrentValue;
	}

	void *GetSystemValuePtr() const
	{
		return m_pSystemValue;
	}

private:
	int SetPBlockValue( void *pVal, TimeValue t )
	{
		FUAssert( pVal, return IC_ERROR );
		BOOL bSuccess = false;
		UIType &pBlockValRef = *( UIType * )pVal;
		bSuccess = m_pBlock->SetValue( m_pbid, t, pBlockValRef );
		return bSuccess ? IC_OK : IC_ERROR;
	}

	int GetPBlockValue( void *pVal, TimeValue t, Interval &interval )
	{
		FUAssert( pVal, return IC_ERROR );
		BOOL bSuccess = false;
		UIType &pBlockValRef = *( UIType * )pVal;
		bSuccess = m_pBlock->GetValue( m_pbid, t, pBlockValRef, interval );
		return bSuccess ? IC_OK : IC_ERROR;
	}
};

class ICBridgeableParticleSystemInParams
{
public:
	InParamList m_uiParams;
	irr::core::matrix4 m_emitterWorldTM;

public:
	IICBridgeableParticleSystemUIParam *GetParamByParamBlockId( int paramBlockId );
};

class ICBridgeableParticleParamsOut
{
public:
	irr::core::vector3df m_position;
	irr::core::vector3df m_velocity;
	float m_age; // secs
	float m_radius;
	float m_tension;
};

class ICBridgeableParticleSystemOutParams
{
public:
	//ParticleVector m_particles;
	//float m_particleSize; // World space radius of a particle

	int m_particleStride;
	int m_particleCount;
	float *m_pParticlePositions;
	float *m_pParticleVelocities;
	float *m_pParticleColors;
	float *m_pParticleAges;
	float *m_pParticleSizes;
};

typedef std::vector<ICBridgeableParticleParamsOut> ParticleVector;

#endif // _IC_PARTICLE_SYSTEM_PARAMS_H

