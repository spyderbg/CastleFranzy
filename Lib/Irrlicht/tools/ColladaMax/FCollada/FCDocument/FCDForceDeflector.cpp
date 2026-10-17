/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDForceDeflector.h"

//
// FCDForceDeflector
//

FCDForceDeflector::FCDForceDeflector(FCDocument* document, FUDaeForce::ForceType forceType )
:	FCDForce( document, forceType )
,	InitializeParameterAnimatable(m_bounce, 0.0f)
,	InitializeParameterAnimatable(m_variation, 0.0f)
,	InitializeParameterAnimatable(m_chaos, 0.0f)
,	InitializeParameterAnimatable(m_friction, 0.0f)
,	InitializeParameterAnimatable(m_inheritVelocity, 0.0f)
,	InitializeParameterAnimatable(m_width, 0.0f)
,	InitializeParameterAnimatable(m_height, 0.0f)
{
}

FCDForceDeflector::~FCDForceDeflector()
{
}

FCDEntity* FCDForceDeflector::Clone(FCDEntity* _clone, bool cloneChildren) const
{
	FCDForceDeflector* clone = NULL;
	if (_clone == NULL) _clone = clone = new FCDForceDeflector(const_cast<FCDocument*>(GetDocument()));
	else if (_clone->HasType(FCDForceDeflector::GetClassType())) clone = (FCDForceDeflector*) _clone;

	FCDForce::Clone(_clone, cloneChildren);

	if (clone != NULL)
	{
		CLONE_PARAMETER( clone, m_bounce );
		CLONE_PARAMETER( clone, m_variation );
		CLONE_PARAMETER( clone, m_chaos );
		CLONE_PARAMETER( clone, m_friction );
		CLONE_PARAMETER( clone, m_inheritVelocity );
		CLONE_PARAMETER( clone, m_width );
		CLONE_PARAMETER( clone, m_height );
	}

	return clone;
}
