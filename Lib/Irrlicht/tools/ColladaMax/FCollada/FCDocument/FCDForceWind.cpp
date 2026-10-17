/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDForceWind.h"

//
// FCDForce
//

FCDForceWind::FCDForceWind(FCDocument* document, FUDaeForce::ForceType forceType )
:	FCDForceGravity( document, forceType )
,	InitializeParameterAnimatable(m_windParamsTurbulence, 0.0f)
,	InitializeParameterAnimatable(m_windParamsFrequency, 0.0f)
,	InitializeParameterAnimatable(m_windParamsScale, 0.0f)
{
}

FCDForceWind::~FCDForceWind()
{
}

FCDEntity* FCDForceWind::Clone(FCDEntity* _clone, bool cloneChildren) const
{
	FCDForceWind* clone = NULL;
	if (_clone == NULL) _clone = clone = new FCDForceWind(const_cast<FCDocument*>(GetDocument()));
	else if (_clone->HasType(FCDForceWind::GetClassType())) clone = (FCDForceWind*) _clone;

	FCDForceGravity::Clone(_clone, cloneChildren);

	if (clone != NULL)
	{
		CLONE_PARAMETER( clone, m_windParamsTurbulence );
		CLONE_PARAMETER( clone, m_windParamsFrequency );
		CLONE_PARAMETER( clone, m_windParamsScale );
	}

	return clone;
}


