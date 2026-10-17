/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDForceDeflector.h
	This file contains the FCDForceDeflector class.
*/

#ifndef _FCD_FORCE_DEFLECTOR_H_
#define _FCD_FORCE_DEFLECTOR_H_

#include "FCDocument/FCDForce.h"
#include "FCDocument/FCDParameterAnimatable.h"

#include "FCDocument/FCDForce.h"
#include "FCDocument/FCDParameterAnimatable.h"

/**
	Deflector force.
	@ingroup FCDocument
*/

class FCOLLADA_EXPORT FCDForceDeflector: public FCDForce
{
private:
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_bounce, FC("Deflector Bounce"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_variation, FC("Deflector Variation"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_chaos, FC("Deflector Chaos"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_friction, FC("Deflector Friction"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_inheritVelocity, FC("Deflector Inherit Velocity"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_width, FC("Deflector Width"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_height, FC("Deflector Height"));

public:
	/** Constructor.
		@param document The COLLADA document that owns the force. */
	FCDForceDeflector(FCDocument* document, FUDaeForce::ForceType forceType = FUDaeForce::FORCE_DEFLECTOR );

	/** Destructor. */
	virtual ~FCDForceDeflector();

	/** Clones the force field information.
		@param clone The cloned force field.
			If this pointer is NULL, a new force is created and
			you will need to release it manually.
		@param cloneChildren Whether to recursively clone this entity's children.
		@return The clone. */
	virtual FCDEntity* Clone(FCDEntity* clone = NULL, bool cloneChildren = false) const;

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Bounce, m_bounce )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Variation, m_variation )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Chaos, m_chaos )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Friction, m_friction )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, InheritVelocity, m_inheritVelocity )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Width, m_width )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Height, m_height )
};

#endif // _FCD_FORCE_DEFLECTOR_H_
