/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "AnimationExporter.h"
#include "DocumentExporter.h"
#include "EntityExporter.h"
#include "ForceExporter.h"
#include "ICWind.h"
#include "ICGravity.h"
#include "ICDeflector.h"
#include "FCDocument/FCDForceGravity.h"
#include "FCDocument/FCDForceWind.h"
#include "FCDocument/FCDForceDeflector.h"
//#include "Common/ConversionFunctions.h"
#include "FCDocument/FCDocument.h"
#include "ColladaOptions.h"
#include "XRefFunctions.h"
#include "FCDocument/FCDLibrary.h"
#include "FUtils/FUDaeEnum.h"
#include "FUtils/FUDaeEnumSyntax.h"

// taken from <simpmod.h>
//#define SIMPWSMMOD_OBREF		0
//#define SIMPWSMMOD_NODEREF		1
//#define SIMPWSMMOD_PBLOCKREF	2

ForceExporter::ForceExporter(DocumentExporter* _document)
:	BaseExporter(_document)
{
}

ForceExporter::~ForceExporter()
{
}

void ForceExporter::ExportForceParams( FCDForceGravity *pForceGravity, IParamBlock2 *pParamBlock, fstring idPrefix )
{
	FUAssert( pParamBlock, return );
	idPrefix = idPrefix + "_" + DAE_FORCE_PARAMS;
	ANIM->ExportProperty( idPrefix + "_" + DAE_FORCE_STRENGTH, pParamBlock, FSForceGravity::PB_STRENGTH, 0, pForceGravity->GetForceParamsStrength(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_FORCE_DECAY, pParamBlock, FSForceGravity::PB_DECAY, 0, pForceGravity->GetForceParamsDecay(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_FORCE_TYPE, pParamBlock, FSForceGravity::PB_TYPE, 0, pForceGravity->GetForceParamsType(), NULL );
}

void ForceExporter::ExportWindParams( FCDForceWind *pForceWind, IParamBlock2 *pParamBlock, fstring idPrefix )
{
	FUAssert( pParamBlock, return );
	idPrefix = idPrefix + "_" + DAE_WIND_PARAMS;
	ANIM->ExportProperty( idPrefix + "_" + DAE_WIND_TURBULENCE, pParamBlock, FSForceWind::PB_TURBULENCE, 0, pForceWind->GetWindParamsTurbulence(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_WIND_FREQUENCY, pParamBlock, FSForceWind::PB_FREQUENCY, 0, pForceWind->GetWindParamsFrequency(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_WIND_SCALE, pParamBlock, FSForceWind::PB_SCALE, 0, pForceWind->GetWindParamsScale(), NULL );
}

void ForceExporter::ExportGravity( FCDForceGravity *pForceGravity, ICGravityObject *pMaxGravity )
{
	IParamBlock2 *pParamBlock = pMaxGravity->GetParamBlockByID( FSForceGravity::PBLOCK_REF_NO );

	fstring entityName = pForceGravity->GetName();
	fstring idPrefix = entityName + "-force" + "_" + DAE_FORCE_GRAVITY;

	// force params
	ExportForceParams( pForceGravity, pParamBlock, idPrefix );
}

void ForceExporter::ExportWind( FCDForceWind *pForceWind, ICWindObject *pMaxWind )
{
	IParamBlock2 *pParamBlock = pMaxWind->GetParamBlockByID( FSForceWind::PBLOCK_REF_NO );

	fstring entityName = pForceWind->GetName();
	fstring idPrefix = entityName + "-force" + "_" + DAE_FORCE_WIND;

	// force params
	ExportForceParams( pForceWind, pParamBlock, idPrefix );
	ExportWindParams( pForceWind, pParamBlock, idPrefix );
}

void ForceExporter::ExportDeflector( FCDForceDeflector *pForceDeflector, ICDeflectObject *pMaxDeflector )
{
	IParamBlock2 *pParamBlock = pMaxDeflector->GetParamBlockByID( FSDeflector::PBLOCK_REF_NO );

	fstring entityName = pForceDeflector->GetName();
	fstring idPrefix = entityName + "-force" + "_" + DAE_FORCE_DEFLECTOR;

	// force params
	ANIM->ExportProperty( idPrefix + "_" + DAE_DEFLECTOR_BOUNCE, pParamBlock, FSDeflector::deflect_bounce, 0, pForceDeflector->GetBounce(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_DEFLECTOR_VARIATION, pParamBlock, FSDeflector::deflect_variation, 0, pForceDeflector->GetVariation(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_DEFLECTOR_CHAOS, pParamBlock, FSDeflector::deflect_chaos, 0, pForceDeflector->GetChaos(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_DEFLECTOR_FRICTION, pParamBlock, FSDeflector::deflect_friction, 0, pForceDeflector->GetFriction(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_DEFLECTOR_INHERIT_VELOCITY, pParamBlock, FSDeflector::deflect_inherit_vel, 0, pForceDeflector->GetInheritVelocity(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_DEFLECTOR_WIDTH, pParamBlock, FSDeflector::deflect_width, 0, pForceDeflector->GetWidth(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_DEFLECTOR_HEIGHT, pParamBlock, FSDeflector::deflect_height, 0, pForceDeflector->GetHeight(), NULL );
}

FCDEntity* ForceExporter::ExportForce(INode* node, Object* object, int flags)
{
	if (node == NULL || object == NULL) return NULL;

	bool isXRef = false;
	if (XRefFunctions::IsXRefObject(object))
	{
		// Resolve the source
		object = XRefFunctions::GetXRefObjectSource(object);
		if (OPTS->ExportXRefs()) isXRef = true;
	}
	
	// temporarily deactivating animation export if xref
	bool bExportAnimationsBackup = OPTS->ExportAnimations();
	OPTS->animations = !isXRef;

	SimpleWSMObject2 *pWSMObject = ( SimpleWSMObject2 * )object;
	FCDForce* pForce = NULL;

	Class_ID classId = object->ClassID();
	if ( classId == FSForceGravity::IC_OBJECT_CLASSID )
	{
		ICGravityObject *pGravityObject = ( ICGravityObject * )object;
		FCDForceGravity *pForceGravity = new FCDForceGravity(CDOC);
		pForce = pForceGravity;
		ExportGravity( pForceGravity, pGravityObject );
	}
	else if ( classId == FSForceWind::IC_OBJECT_CLASSID )
	{
		ICWindObject *pWindObject = ( ICWindObject * )object;
		FCDForceWind *pForceWind = new FCDForceWind(CDOC);
		pForce = pForceWind;
		ExportWind( pForceWind, pWindObject );
	}
	else if ( classId == FSDeflector::IC_OBJECT_CLASSID )
	{
		ICDeflectObject *pDeflectorObject = ( ICDeflectObject * )object;
		FCDForceDeflector *pForceDeflector = new FCDForceDeflector(CDOC);
		pForce = pForceDeflector;
		ExportDeflector( pForceDeflector, pDeflectorObject );
	}

	FUAssert( pForce, OPTS->animations = bExportAnimationsBackup; return NULL );
	if ( !isXRef )
	{
		CDOC->GetForceLibrary()->AddEntity( pForce );
	}

	fstring entityName = fstring( node->GetName() ) + "-force";
	ENTE->ExportEntity(object, pForce, entityName.c_str());
	pForce->SetName(fm::string(node->GetName()));

	OPTS->animations = bExportAnimationsBackup;

	return pForce;
}

