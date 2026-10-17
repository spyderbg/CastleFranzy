/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDForceGravity.h
	This file contains the FCDForceGravity class.
*/

#ifndef _FCD_FORCE_GRAVITY_H_
#define _FCD_FORCE_GRAVITY_H_

#include "FCDocument/FCDForce.h"
#include "FCDocument/FCDParameterAnimatable.h"

/**
	Gravity force.
	@ingroup FCDocument
*/

class FCOLLADA_EXPORT FCDForceGravity: public FCDForce
{
private:
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_forceParamsStrength, FC("Force Params Strength"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_forceParamsDecay, FC("Force Params Decay"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_forceParamsType, FC("Force Params Type"));

public:
	/** Constructor.
		@param document The COLLADA document that owns the force. */
	FCDForceGravity(FCDocument* document, FUDaeForce::ForceType forceType = FUDaeForce::FORCE_GRAVITY );

	/** Destructor. */
	virtual ~FCDForceGravity();

	/** Clones the force field information.
		@param clone The cloned force field.
			If this pointer is NULL, a new force is created and
			you will need to release it manually.
		@param cloneChildren Whether to recursively clone this entity's children.
		@return The clone. */
	virtual FCDEntity* Clone(FCDEntity* clone = NULL, bool cloneChildren = false) const;

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ForceParamsStrength, m_forceParamsStrength )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ForceParamsDecay, m_forceParamsDecay )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ForceParamsType, m_forceParamsType )
};


#endif // _FCD_FORCE_GRAVITY_H_
