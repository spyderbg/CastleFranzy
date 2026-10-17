/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDModularSkinCategoryInstance.h
	This file contains the FCDModularSkinCategoryInstance class.

	@ingroup FCDModularSkinCategorys
*/

#ifndef _FCD_MODULAR_SKIN_CATEGORY_INSTANCE_H_
#define _FCD_MODULAR_SKIN_CATEGORY_INSTANCE_H_

#ifndef _FCD_ENTITY_INSTANCE_H_
#include "FCDocument/FCDEntityInstance.h"
#endif // _FCD_ENTITY_INSTANCE_H_

/**
	A COLLADA modularSkinCategory instance.
*/
class FCOLLADA_EXPORT FCDModularSkinCategoryInstance : public FCDEntityInstance
{
private:
    DeclareObjectType(FCDEntityInstance);
	friend class FCDEntityInstanceFactory;

protected:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddInstance function.
		@param document The COLLADA document that owns the modularSkinCategory instance.
		@param parent The parent visual scene node.
		@param entityType The type of the entity to instantiate. Unless this class
			is overwritten, FCDEntity::FORCE should be given. */
	FCDModularSkinCategoryInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType = FCDEntity::MODULAR_SKIN_CATEGORY);

public:
	/** Destructor. */
	virtual ~FCDModularSkinCategoryInstance();

	///** Clones the modularSkinCategory instance.
	//	@param clone The modularSkinCategory instance to become the clone. If this pointer
	//		is NULL, a new modularSkinCategory instance will be created.
	//	@return The cloned modularSkinCategory instance. */
	//virtual FCDEntityInstance* Clone(FCDEntityInstance* clone = NULL) const;

	///** Retrieves the entity instance class type.
	//	This is used to determine the up-class for the entity instance object.
	//	@deprecated Instead use: FCDEntityInstance::HasType(FCDGeometryInstance::GetClassType())
	//	@return The class type: GEOMETRY. */
	//virtual Type GetType() const { return SIMPLE; }
};

#endif // _FCD_MODULAR_SKIN_CATEGORY_INSTANCE_H_
