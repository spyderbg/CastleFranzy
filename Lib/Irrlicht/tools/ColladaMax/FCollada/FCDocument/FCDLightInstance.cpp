/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDLightInstance.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDEntity.h"

//
// FCDLightInstance
//

ImplementObjectType(FCDLightInstance);
ImplementParameterObjectNoCtr(FCDLightInstance, FCDEntityInstance, forceInstances);
ImplementParameterObjectNoCtr(FCDLightInstance, FCDEntityInstance, emittedInstances);

FCDLightInstance::FCDLightInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType)
:	FCDEntityInstance(document, parent, entityType)
{
}

FCDLightInstance::~FCDLightInstance()
{
}

