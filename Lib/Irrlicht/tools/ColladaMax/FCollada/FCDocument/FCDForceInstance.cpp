/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDForceInstance.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDEntity.h"
#include "FUtils/FUDaeSyntax.h"

//
// FCDForceInstance
//

ImplementObjectType(FCDForceInstance);

FCDForceInstance::FCDForceInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType)
:	FCDEntityInstance(document, parent, entityType)
{
	SetWantedSubId( DAE_INSTANCE_FORCE_ELEMENT );
}

FCDForceInstance::~FCDForceInstance()
{
}

//FCDEntityInstance* FCDForceInstance::Clone(FCDEntityInstance* _clone) const
//{
//	FCDForceInstance* clone = NULL;
//	if (_clone == NULL) clone = new FCDForceInstance(const_cast<FCDocument*>(GetDocument()), const_cast<FCDSceneNode*>(GetParent()), GetEntityType());
//	else if (!_clone->HasType(FCDForceInstance::GetClassType())) return Parent::Clone(_clone);
//	else clone = (FCDForceInstance*) _clone;
//
//	Parent::Clone(clone);
//	return clone;
//}
