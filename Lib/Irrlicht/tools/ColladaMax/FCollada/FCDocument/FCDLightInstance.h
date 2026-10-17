/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDLightInstance.h
	This file contains the FCDLightInstance class.

	@ingroup FCDLights
*/

#ifndef _FCD_LIGHT_INSTANCE_H_
#define _FCD_LIGHT_INSTANCE_H_

#ifndef _FCD_ENTITY_INSTANCE_H_
#include "FCDocument/FCDEntityInstance.h"
#endif // _FCD_ENTITY_INSTANCE_H_

/**
	A COLLADA light instance.
*/
class FCOLLADA_EXPORT FCDLightInstance : public FCDEntityInstance
{
private:
    DeclareObjectType(FCDEntityInstance);
	friend class FCDEntityInstanceFactory;

protected:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddInstance function.
		@param document The COLLADA document that owns the light instance.
		@param parent The parent visual scene node.
		@param entityType The type of the entity to instantiate. Unless this class
			is overwritten, FCDEntity::LIGHT should be given. */
	FCDLightInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType = FCDEntity::LIGHT);

public:
	/** Destructor. */
	virtual ~FCDLightInstance();

};

#endif // _FCD_LIGHT_INSTANCE_H_
