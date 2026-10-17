/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDForceGravity.h"

//
// FCDForce
//

FCDForceGravity::FCDForceGravity(FCDocument* document, FUDaeForce::ForceType forceType )
:	FCDForce( document, forceType )
,	InitializeParameterAnimatable(m_forceParamsStrength, 0.0f)
,	InitializeParameterAnimatable(m_forceParamsDecay, 0.0f)
,	InitializeParameterAnimatable(m_forceParamsType, 0.0f)
{
}

FCDForceGravity::~FCDForceGravity()
{
}

FCDEntity* FCDForceGravity::Clone(FCDEntity* _clone, bool cloneChildren) const
{
	FCDForceGravity* clone = NULL;
	if (_clone == NULL) _clone = clone = new FCDForceGravity(const_cast<FCDocument*>(GetDocument()));
	else if (_clone->HasType(FCDForceGravity::GetClassType())) clone = (FCDForceGravity*) _clone;

	FCDForce::Clone(_clone, cloneChildren);

	if (clone != NULL)
	{
		CLONE_PARAMETER( clone, m_forceParamsStrength );
		CLONE_PARAMETER( clone, m_forceParamsDecay );
		CLONE_PARAMETER( clone, m_forceParamsType );
	}

	return clone;
}
