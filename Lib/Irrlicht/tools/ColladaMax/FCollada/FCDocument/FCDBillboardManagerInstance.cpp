/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDBillboardManagerInstance.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDEntity.h"
#include "FUtils/FUDaeSyntax.h"

//
// FCDBillboardManagerInstance
//

ImplementObjectType(FCDBillboardManagerInstance);

FCDBillboardManagerInstance::FCDBillboardManagerInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType)
:	FCDEntityInstance(document, parent, entityType)
{
	SetWantedSubId( DAE_INSTANCE_BILLBOARD_ELEMENT );
}

FCDBillboardManagerInstance::~FCDBillboardManagerInstance()
{
}

//FCDEntityInstance* FCDBillboardManagerInstance::Clone(FCDEntityInstance* _clone) const
//{
//	FCDBillboardManagerInstance* clone = NULL;
//	if (_clone == NULL) clone = new FCDBillboardManagerInstance(const_cast<FCDocument*>(GetDocument()), const_cast<FCDSceneNode*>(GetParent()), GetEntityType());
//	else if (!_clone->HasType(FCDBillboardManagerInstance::GetClassType())) return Parent::Clone(_clone);
//	else clone = (FCDBillboardManagerInstance*) _clone;
//
//	Parent::Clone(clone);
//	return clone;
//}
