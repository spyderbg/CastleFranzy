/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDEntity.h"
#include "FCDocument/FCDEmitterInstance.h"

//
// FCDEmitterInstance
//

ImplementObjectType(FCDEmitterInstance);

FCDEmitterInstance::FCDEmitterInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType)
:	FCDGeometryInstance(document, parent, entityType)
{
}

FCDEmitterInstance::~FCDEmitterInstance()
{
}

FCDEntityInstance* FCDEmitterInstance::Clone(FCDEntityInstance* _clone) const
{
	FCDEmitterInstance* clone = NULL;
	if (_clone == NULL) clone = new FCDEmitterInstance(const_cast<FCDocument*>(GetDocument()), const_cast<FCDSceneNode*>(GetParent()), GetEntityType());
	else if (!_clone->HasType(FCDEmitterInstance::GetClassType())) return Parent::Clone(_clone);
	else clone = (FCDEmitterInstance*) _clone;

	Parent::Clone(clone);
	return clone;
}
