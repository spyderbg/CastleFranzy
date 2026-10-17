/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDModularSkinCategory.h"

//
// FCDModularSkinCategory
//

ImplementObjectType(FCDModularSkinCategory);

FCDModularSkinCategory::FCDModularSkinCategory(FCDocument* document, const char* baseId )
:	FCDEntity(document, baseId)
{
}

FCDModularSkinCategory::~FCDModularSkinCategory()
{
}

FCDEntity* FCDModularSkinCategory::Clone(FCDEntity* _clone, bool cloneChildren) const
{
	FCDModularSkinCategory* clone = NULL;
	if (_clone == NULL) _clone = clone = new FCDModularSkinCategory(const_cast<FCDocument*>(GetDocument()));
	else if (_clone->HasType(FCDModularSkinCategory::GetClassType())) clone = (FCDModularSkinCategory*) _clone;

	Parent::Clone(_clone, cloneChildren);

	if (clone != NULL)
	{
		CLONE_PARAMETER( clone, m_controllerInstances );
		CLONE_PARAMETER( clone, m_skinnedMeshNodes );
	}

	return clone;
}