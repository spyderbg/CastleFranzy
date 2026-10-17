/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDForceWind.h
	This file contains the FCDForceWind class.
*/

#ifndef _FCD_FORCE_WIND_H_
#define _FCD_FORCE_WIND_H_

#include "FCDocument/FCDForceGravity.h"

/**
	Gravity force.
	@ingroup FCDocument
*/

class FCOLLADA_EXPORT FCDForceWind: public FCDForceGravity
{
private:
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_windParamsTurbulence, FC("Wind Params Turbulence"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_windParamsFrequency, FC("Wind Params Frequency"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_windParamsScale, FC("Wind Params Scale"));

public:
	/** Constructor.
		@param document The COLLADA document that owns the force. */
	FCDForceWind(FCDocument* document, FUDaeForce::ForceType forceType = FUDaeForce::FORCE_WIND );

	/** Destructor. */
	virtual ~FCDForceWind();

	/** Clones the force field information.
		@param clone The cloned force field.
			If this pointer is NULL, a new force is created and
			you will need to release it manually.
		@param cloneChildren Whether to recursively clone this entity's children.
		@return The clone. */
	virtual FCDEntity* Clone(FCDEntity* clone = NULL, bool cloneChildren = false) const;

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, WindParamsTurbulence, m_windParamsTurbulence )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, WindParamsFrequency, m_windParamsFrequency )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, WindParamsScale, m_windParamsScale )
};


#endif // _FCD_FORCE_WIND_H_
