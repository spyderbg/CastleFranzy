/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FArchiveXML.h"
#include "FCDocument/FCDForce.h"
#include "FCDocument/FCDForceGravity.h"
#include "FCDocument/FCDForceWind.h"
#include "FCDocument/FCDForceDeflector.h"
#include "FUtils/FUDaeEnumSyntax.h"
#include "FAXICHelpers.h"

void WriteWindParams( FCDForceWind *pForceWind, xmlNode *pParent )
{
	xmlNode *pForceParamsNode = AddChildWithSid( pParent, DAE_WIND_PARAMS, DAE_WIND_PARAMS );
	FAXICHelpers::WriteAnimatedParameterContent( pForceParamsNode, DAE_WIND_TURBULENCE, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceWind->GetWindParamsTurbulence() );
	FAXICHelpers::WriteAnimatedParameterContent( pForceParamsNode, DAE_WIND_FREQUENCY, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceWind->GetWindParamsFrequency() );
	FAXICHelpers::WriteAnimatedParameterContent( pForceParamsNode, DAE_WIND_SCALE, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceWind->GetWindParamsScale() );
}

void WriteForceParams( FCDForceGravity *pForceGravity, xmlNode *pParent )
{
	xmlNode *pForceParamsNode = AddChildWithSid( pParent, DAE_FORCE_PARAMS, DAE_FORCE_PARAMS );
	FAXICHelpers::WriteAnimatedParameterContent( pForceParamsNode, DAE_FORCE_STRENGTH, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceGravity->GetForceParamsStrength() );
	FAXICHelpers::WriteAnimatedParameterContent( pForceParamsNode, DAE_FORCE_DECAY, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceGravity->GetForceParamsDecay() );
	FAXICHelpers::WriteUnanimatedParameter( pForceParamsNode, DAE_FORCE_TYPE, DAEFC_FLOAT_ATTRIBUTE_TYPE, ( int )pForceGravity->GetForceParamsType() );
}

void WriteWind( FCDForceWind *pForceWind, xmlNode *pParent )
{
	WriteForceParams( pForceWind, pParent );
	WriteWindParams( pForceWind, pParent );
}

void WriteGravity( FCDForceGravity *pForceGravity, xmlNode *pParent )
{
	WriteForceParams( pForceGravity, pParent );
}

void WriteDeflector( FCDForceDeflector *pForceDeflector, xmlNode *pParent )
{
	FAXICHelpers::WriteAnimatedParameterContent( pParent, DAE_DEFLECTOR_BOUNCE, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceDeflector->GetBounce() );
	FAXICHelpers::WriteAnimatedParameterContent( pParent, DAE_DEFLECTOR_VARIATION, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceDeflector->GetVariation() );
	FAXICHelpers::WriteAnimatedParameterContent( pParent, DAE_DEFLECTOR_CHAOS, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceDeflector->GetChaos() );
	FAXICHelpers::WriteAnimatedParameterContent( pParent, DAE_DEFLECTOR_FRICTION, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceDeflector->GetFriction() );
	FAXICHelpers::WriteAnimatedParameterContent( pParent, DAE_DEFLECTOR_INHERIT_VELOCITY, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceDeflector->GetInheritVelocity() );
	FAXICHelpers::WriteAnimatedParameterContent( pParent, DAE_DEFLECTOR_WIDTH, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceDeflector->GetWidth() );
	FAXICHelpers::WriteAnimatedParameterContent( pParent, DAE_DEFLECTOR_HEIGHT, DAEFC_FLOAT_ATTRIBUTE_TYPE, pForceDeflector->GetHeight() );
}

//
// Force export
//
xmlNode* FArchiveXML::WriteForce(FCDObject* object, xmlNode* parentNode)
{
	FCDForce* pForce = (FCDForce*) object;
	xmlNode* forceNode = FArchiveXML::WriteToEntityXMLFCDEntity(pForce, parentNode, DAE_FORCE_ELEMENT);

	xmlNode *pSpecificForceNode = NULL;
	WRITE_IC_ENUM_PARAM_AS_CHILD_WITH_SID( FUDaeForce, forceNode, ForceType, pForce, pSpecificForceNode );

	if ( pForce->GetForceType() == FUDaeForce::FORCE_GRAVITY )
	{
		WriteGravity( ( FCDForceGravity * )pForce, pSpecificForceNode );	
	}
	else if ( pForce->GetForceType() == FUDaeForce::FORCE_WIND )
	{
		WriteWind( ( FCDForceWind * )pForce, pSpecificForceNode );
	}
	else if ( pForce->GetForceType() == FUDaeForce::FORCE_DEFLECTOR )
	{
		WriteDeflector( ( FCDForceDeflector * )pForce, pSpecificForceNode );
	}

	FArchiveXML::WriteEntityExtra(pForce, forceNode);
	return forceNode;
}