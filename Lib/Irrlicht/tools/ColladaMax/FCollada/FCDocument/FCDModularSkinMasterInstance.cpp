/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDModularSkinMasterInstance.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDEntity.h"
#include "FUtils/FUDaeSyntax.h"

//
// FCDModularSkinMasterInstance
//

ImplementObjectType(FCDModularSkinMasterInstance);

FCDModularSkinMasterInstance::FCDModularSkinMasterInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType)
:	FCDModularSkinCategoryInstance(document, parent, entityType)
{
	SetWantedSubId( DAE_INSTANCE_MODULAR_SKIN_MASTER_ELEMENT );
}

FCDModularSkinMasterInstance::~FCDModularSkinMasterInstance()
{
}

//FCDEntityInstance* FCDModularSkinMasterInstance::Clone(FCDEntityInstance* _clone) const
//{
//	FCDModularSkinMasterInstance* clone = NULL;
//	if (_clone == NULL) clone = new FCDModularSkinMasterInstance(const_cast<FCDocument*>(GetDocument()), const_cast<FCDSceneNode*>(GetParent()), GetEntityType());
//	else if (!_clone->HasType(FCDModularSkinMasterInstance::GetClassType())) return Parent::Clone(_clone);
//	else clone = (FCDModularSkinMasterInstance*) _clone;
//
//	Parent::Clone(clone);
//	return clone;
//}
