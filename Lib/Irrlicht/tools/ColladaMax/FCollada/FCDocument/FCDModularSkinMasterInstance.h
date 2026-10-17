/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDModularSkinMasterInstance.h
	This file contains the FCDModularSkinMasterInstance class.

	@ingroup FCDModularSkinMasters
*/

#ifndef _FCD_MODULAR_SKIN_MASTER_INSTANCE_H_
#define _FCD_MODULAR_SKIN_MASTER_INSTANCE_H_

#ifndef _FCD_ENTITY_INSTANCE_H_
#include "FCDocument/FCDEntityInstance.h"
#endif // _FCD_ENTITY_INSTANCE_H_

#include "FCDocument/FCDModularSkinCategoryInstance.h"

/**
	A COLLADA modularSkinMaster instance.
*/
class FCOLLADA_EXPORT FCDModularSkinMasterInstance : public FCDModularSkinCategoryInstance
{
private:
    DeclareObjectType(FCDModularSkinCategoryInstance);
	friend class FCDEntityInstanceFactory;

protected:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddInstance function.
		@param document The COLLADA document that owns the modularSkinMaster instance.
		@param parent The parent visual scene node.
		@param entityType The type of the entity to instantiate. Unless this class
			is overwritten, FCDEntity::FORCE should be given. */
	FCDModularSkinMasterInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType = FCDEntity::MODULAR_SKIN_MASTER);

public:
	/** Destructor. */
	virtual ~FCDModularSkinMasterInstance();

	///** Clones the modularSkinMaster instance.
	//	@param clone The modularSkinMaster instance to become the clone. If this pointer
	//		is NULL, a new modularSkinMaster instance will be created.
	//	@return The cloned modularSkinMaster instance. */
	//virtual FCDEntityInstance* Clone(FCDEntityInstance* clone = NULL) const;

	///** Retrieves the entity instance class type.
	//	This is used to determine the up-class for the entity instance object.
	//	@deprecated Instead use: FCDEntityInstance::HasType(FCDGeometryInstance::GetClassType())
	//	@return The class type: GEOMETRY. */
	//virtual Type GetType() const { return SIMPLE; }
};

#endif // _FCD_MODULAR_SKIN_MASTER_INSTANCE_H_
