/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDForce.h"

//
// FCDForce
//

ImplementObjectType(FCDForce);

FCDForce::FCDForce(FCDocument* document, FUDaeForce::ForceType forceType )
:	FCDEntity(document, "Force")
,	InitializeParameter(m_forceType, forceType)
{
}

FCDForce::~FCDForce()
{
}

FCDEntity* FCDForce::Clone(FCDEntity* _clone, bool cloneChildren) const
{
	FCDForce* clone = NULL;
	if (_clone == NULL) _clone = clone = new FCDForce(const_cast<FCDocument*>(GetDocument()));
	else if (_clone->HasType(FCDForce::GetClassType())) clone = (FCDForce*) _clone;

	Parent::Clone(_clone, cloneChildren);

	if (clone != NULL)
	{
		CLONE_PARAMETER( clone, m_forceType );
	}

	return clone;
}
