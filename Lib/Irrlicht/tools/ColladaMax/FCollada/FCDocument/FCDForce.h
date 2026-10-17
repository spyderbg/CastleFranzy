/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _FCD_FORCE_H_
#define _FCD_FORCE_H_

class FCDocument;
class FCDExtra;

#ifndef _FCD_ENTITY_H_
#include "FCDocument/FCDEntity.h"
#endif // _FCD_ENTITY_H_

#include "FUtils/FUDaeEnum.h"

/**
	A force.
	@ingroup FCDocument
*/

class FCOLLADA_EXPORT FCDForce : public FCDEntity
{
private:
	DeclareObjectType(FCDEntity);

	DeclareParameter( FUDaeForce::ForceType, FUParameterQualifiers::SIMPLE, m_forceType, FC("Force Type"));

public:
	/** Constructor.
		@param document The COLLADA document that owns the force. */
	FCDForce(FCDocument* document, FUDaeForce::ForceType forceType = FUDaeForce::FORCE_INVALID);

	/** Destructor. */
	virtual ~FCDForce();

	/** Retrieves the entity class type.
		@return The entity class type: FORCE */
	virtual Type GetType() const { return FORCE; }

	/** Clones the force field information.
		@param clone The cloned force field.
			If this pointer is NULL, a new force is created and
			you will need to release it manually.
		@param cloneChildren Whether to recursively clone this entity's children.
		@return The clone. */
	virtual FCDEntity* Clone(FCDEntity* clone = NULL, bool cloneChildren = false) const;

	DECL_IMPL_ACCESSORS( FUDaeForce::ForceType, FUDaeForce::ForceType, ForceType, m_forceType );
};

#endif // _FCD_FORCE_H_
