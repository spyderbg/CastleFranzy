/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDForceInstance.h
	This file contains the FCDForceInstance class.

	@ingroup FCDForces
*/

#ifndef _FCD_FORCE_INSTANCE_H_
#define _FCD_FORCE_INSTANCE_H_

#ifndef _FCD_ENTITY_INSTANCE_H_
#include "FCDocument/FCDEntityInstance.h"
#endif // _FCD_ENTITY_INSTANCE_H_

/**
	A COLLADA force instance.
*/
class FCOLLADA_EXPORT FCDForceInstance : public FCDEntityInstance
{
private:
    DeclareObjectType(FCDEntityInstance);
	friend class FCDEntityInstanceFactory;

protected:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddInstance function.
		@param document The COLLADA document that owns the force instance.
		@param parent The parent visual scene node.
		@param entityType The type of the entity to instantiate. Unless this class
			is overwritten, FCDEntity::FORCE should be given. */
	FCDForceInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType = FCDEntity::FORCE);

public:
	/** Destructor. */
	virtual ~FCDForceInstance();

	///** Clones the force instance.
	//	@param clone The force instance to become the clone. If this pointer
	//		is NULL, a new force instance will be created.
	//	@return The cloned force instance. */
	//virtual FCDEntityInstance* Clone(FCDEntityInstance* clone = NULL) const;

	///** Retrieves the entity instance class type.
	//	This is used to determine the up-class for the entity instance object.
	//	@deprecated Instead use: FCDEntityInstance::HasType(FCDGeometryInstance::GetClassType())
	//	@return The class type: GEOMETRY. */
	//virtual Type GetType() const { return SIMPLE; }
};

#endif // _FCD_FORCE_INSTANCE_H_
