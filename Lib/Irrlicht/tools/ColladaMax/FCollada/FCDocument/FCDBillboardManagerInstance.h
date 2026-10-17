/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDBillboardManagerInstance.h
	This file contains the FCDBillboardManagerInstance class.

	@ingroup FCDBillboardManagers
*/

#ifndef _FCD_BILLBOARD_MANAGER_INSTANCE_H_
#define _FCD_BILLBOARD_MANAGER_INSTANCE_H_

#ifndef _FCD_ENTITY_INSTANCE_H_
#include "FCDocument/FCDEntityInstance.h"
#endif // _FCD_ENTITY_INSTANCE_H_

/**
	A COLLADA billboardManagerInstance instance.
*/
class FCOLLADA_EXPORT FCDBillboardManagerInstance : public FCDEntityInstance
{
private:
    DeclareObjectType(FCDEntityInstance);
	friend class FCDEntityInstanceFactory;

protected:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddInstance function.
		@param document The COLLADA document that owns the modularSkinMaster instance.
		@param parent The parent visual scene node.
		@param entityType The type of the entity to instantiate. Unless this class
			is overwritten, FCDEntity::FORCE should be given. */
	FCDBillboardManagerInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType = FCDEntity::MODULAR_SKIN_MASTER);

public:
	/** Destructor. */
	virtual ~FCDBillboardManagerInstance();
};

#endif // _FCD_BILLBOARD_MANAGER_INSTANCE_H_
