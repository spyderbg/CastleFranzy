/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDModularSkinCategoryInstance.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDEntity.h"
#include "FUtils/FUDaeSyntax.h"

//
// FCDModularSkinCategoryInstance
//

ImplementObjectType(FCDModularSkinCategoryInstance);

FCDModularSkinCategoryInstance::FCDModularSkinCategoryInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType)
:	FCDEntityInstance(document, parent, entityType)
{
	SetWantedSubId( DAE_INSTANCE_MODULAR_SKIN_CATEGORY_ELEMENT );
}

FCDModularSkinCategoryInstance::~FCDModularSkinCategoryInstance()
{
}

//FCDEntityInstance* FCDModularSkinCategoryInstance::Clone(FCDEntityInstance* _clone) const
//{
//	FCDModularSkinCategoryInstance* clone = NULL;
//	if (_clone == NULL) clone = new FCDModularSkinCategoryInstance(const_cast<FCDocument*>(GetDocument()), const_cast<FCDSceneNode*>(GetParent()), GetEntityType());
//	else if (!_clone->HasType(FCDModularSkinCategoryInstance::GetClassType())) return Parent::Clone(_clone);
//	else clone = (FCDModularSkinCategoryInstance*) _clone;
//
//	Parent::Clone(clone);
//	return clone;
//}
