/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "AnimationExporter.h"
#include "EmitterExporter.h"
#include "EntityExporter.h"
#include "GeometryExporter.h"
#include "NodeExporter.h"
#include "MaterialExporter.h"
#include "DocumentExporter.h"
#include "ColladaOptions.h"
#include "ColladaIDCreator.h"
#include "Common/MaxParticleDefinitions.h"
#include "XRefFunctions.h"
#include "Common/ConversionFunctions.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDEmitter.h"
#include "FCDocument/FCDEmitterInstance.h"
#include "FCDocument/FCDEmitterParticle.h"
#include "FCDocument/FCDEmitterObject.h"
#include "FCDocument/FCDMaterial.h"
#include "FCDocument/FCDMaterialInstance.h"
#include "IParticle/IParticle.h"
#include "ICPCloud.h"
#include "FUtils/FUDaeEnumSyntax.h"
#include "ColladaModifiers.h"

EmitterExporter::EmitterExporter(DocumentExporter* _document)
:	BaseExporter(_document)
{
}

EmitterExporter::~EmitterExporter()
{
}

void EmitterExporter::AddToMaxForceNodes( INode *pNode, Modifier *pModifier, NodeList& maxForceNodes )
{
	bool bValid = true;
	
	// this effectively prevents emitters from referencing the 
	// same deflector twice
	if ( pModifier->ClassID() == FSDeflector::IC_MODIFIER_CLASSID )
	{
		bValid = maxForceNodes.find( pNode ) == maxForceNodes.end();
	}
	
	if ( bValid )
	{
		maxForceNodes.push_back( pNode );
	}
}

void EmitterExporter::GetEmitterForceMaxNodes( INode* node, NodeList& maxForceNodes )
{
	IDerivedObject *pDerivedObject = node->GetWSMDerivedObject();
	ColladaModifierManager::ResolveModifiers( node, pDerivedObject );
	for ( ColladaModifierStack::iterator itM = modifiers.begin(); itM != modifiers.end(); ++itM )
	{
		ColladaModifier* mod = ( *itM );
		if ( mod->GetClassID() == FSForceGravity::IC_MODIFIER_CLASSID )
		{
			ICGravityMod *pModifier = ((ICGravityModifier*)mod)->GetInterface();
			FUAssert( pModifier, continue );
			INode *pNode = pModifier->force.node;
			FUAssert( pNode, continue );
			AddToMaxForceNodes( pNode, pModifier, maxForceNodes );
		}
		else 
		if ( mod->GetClassID() == FSForceWind::IC_MODIFIER_CLASSID )
		{
			ICWindMod *pModifier = ((ICWindModifier*)mod)->GetInterface();
			FUAssert( pModifier, continue );
			INode *pNode = pModifier->force.node;
			FUAssert( pNode, continue );
			AddToMaxForceNodes( pNode, pModifier, maxForceNodes );
		}
		else 
		if ( mod->GetClassID() == FSDeflector::IC_MODIFIER_CLASSID )
		{
			ICDeflectMod *pModifier = ((ICDeflectorModifier*)mod)->GetInterface();
			FUAssert( pModifier, continue );
			INode *pNode = pModifier->deflect.node;
			FUAssert( pNode, continue );
			AddToMaxForceNodes( pNode, pModifier, maxForceNodes );
		}
	}	
}

void EmitterExporter::CalculateForcedNodes(INode* node, Object* object, NodeList& forcedNodes)
{
	if (node == NULL || object == NULL) return;

	ParticleObject *pMaxParticleObject = GetParticleInterface( object );
	if ( !pMaxParticleObject )
	{
		return;
	}

	SimpleParticle *pSimpleParticle = ( SimpleParticle * )pMaxParticleObject;
	ICPCloudParticle *pCloudParticle = ( ICPCloudParticle * )pSimpleParticle;
	INode* targetNode = NULL;

	// emitter shape target
	targetNode = pCloudParticle->distnode;
	IParamBlock *pParamBlock = pCloudParticle->pblock;
	FUDaeEmitter::Shape shape = ( FUDaeEmitter::Shape ) pParamBlock->GetInt(FSParticlePCloud::PB_CREATEIN, TIME_EXPORT_START);
	if (targetNode != NULL && shape == FUDaeEmitter::SHAPE_OBJECT_ )
	{
		forcedNodes.push_back( targetNode );
	}

	// direction target
	targetNode = pCloudParticle->mbase;
	FUDaeEmitter::ParticleDirectionMethod directionMethod = ( FUDaeEmitter::ParticleDirectionMethod ) pParamBlock->GetInt( FSParticlePCloud::PB_SPEEDDIR, TIME_EXPORT_START );
	if ( targetNode != NULL && shape == FUDaeEmitter::PDM_OBJECT )
	{
		forcedNodes.push_back( targetNode );
	}

	// applied forces
	//GetEmitterForceMaxNodes( node, forcedNodes );
}

void EmitterExporter::AddNodeTarget( INode *pNode, FCDEmitter* pEmitter, void ( FCDEmitter::*pNodeTargetAdderFunc )( FCDSceneNode * ) )
{
	if ( pNodeTargetAdderFunc == NULL )
	{
		return;
	}

	FCDSceneNode* colladaTarget = document->GetNodeExporter()->FindExportedNode( pNode );
	if ( colladaTarget == NULL )
	{
		return ;
	}

	( (*pEmitter).*pNodeTargetAdderFunc )( colladaTarget );
}

void EmitterExporter::LinkEmitters()
{
	size_t numEmitters = CDOC->GetEmitterLibrary()->GetEntityCount();
	for (size_t index = 0; index < numEmitters; ++index)
	{
		FCDEmitter* colladaEmitter = CDOC->GetEmitterLibrary()->GetEntity(index);

		INode *pNode = NULL;

		pNode = ( INode * )colladaEmitter->GetUserHandle();
		AddNodeTarget( pNode, colladaEmitter, &FCDEmitter::SetShapeTarget );

		pNode = ( INode * )colladaEmitter->GetParticleMotionTargetNode();
		AddNodeTarget( pNode, colladaEmitter, &FCDEmitter::SetParticleMotionTarget );

		pNode = ( INode * )colladaEmitter->GetInstancedGeometryTargetNode();
		AddNodeTarget( pNode, colladaEmitter, &FCDEmitter::SetInstancedGeometryTarget );

		// forces
		size_t numForceMaxNodes = colladaEmitter->GetNumForceMaxNodes();
		for ( size_t forceId = 0; forceId < numForceMaxNodes; ++forceId )
		{
			INode *pForceMaxNode = colladaEmitter->GetForceMaxNode( forceId );
			AddNodeTarget( pForceMaxNode, colladaEmitter, &FCDEmitter::AddForceFCDSceneNode );
		}
	}
}

void EmitterExporter::ExportEmitterShape( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;
	fstring entityName = pEmitter->GetName();

	ANIM->ExportProperty( "", pParamBlock, FSParticlePCloud::PB_CREATEIN, pEmitter->GetShape(), NULL );
	fstring idPrefix = entityName + "-emitter" + "_" + DAE_EMIT_SHAPE;

	FUDaeEmitter::Shape shape = ( FUDaeEmitter::Shape )( int )pEmitter->GetShape();
	if ( shape == FUDaeEmitter::SHAPE_BOX )
	{
		idPrefix = idPrefix + "_" + DAE_EMIT_SHAPE_BOX;
		ANIM->ExportProperty( idPrefix + "_" + DAE_EMIT_SHAPE_LENGTH, pParamBlock, FSParticlePCloud::PB_EMITRWID, pEmitter->GetShapeLength(), NULL );
		ANIM->ExportProperty( idPrefix + "_" + DAE_EMIT_SHAPE_WIDTH, pParamBlock, FSParticlePCloud::PB_EMITRHEIGHT, pEmitter->GetShapeWidth(), NULL );
		ANIM->ExportProperty( idPrefix + "_" + DAE_EMIT_SHAPE_HEIGHT, pParamBlock, FSParticlePCloud::PB_EMITRDEPTH, pEmitter->GetShapeHeight(), NULL );
	}
	else if ( shape == FUDaeEmitter::SHAPE_SPHERE )
	{
		idPrefix = idPrefix + "_" + DAE_EMIT_SHAPE_SPHERE;
		ANIM->ExportProperty( idPrefix + "_" + DAE_EMIT_SHAPE_RADIUS, pParamBlock, FSParticlePCloud::PB_EMITRWID, pEmitter->GetShapeRadius(), NULL );
	}
	else if ( shape == FUDaeEmitter::SHAPE_CYLINDER )
	{
		idPrefix = idPrefix + "_" + DAE_EMIT_SHAPE_CYLINDER;
		ANIM->ExportProperty( idPrefix + "_" + DAE_EMIT_SHAPE_RADIUS, pParamBlock, FSParticlePCloud::PB_EMITRWID, pEmitter->GetShapeRadius(), NULL );
		ANIM->ExportProperty( idPrefix + "_" + DAE_EMIT_SHAPE_HEIGHT, pParamBlock, FSParticlePCloud::PB_EMITRDEPTH, pEmitter->GetShapeHeight(), NULL );
	}
	else
	{
		ICPCloudParticle *pCloudParticle = ( ICPCloudParticle * ) pMaxSimpleParticle;
		pEmitter->SetUserHandle( pCloudParticle->distnode );
	}
}

void EmitterExporter::ExportEmitterEmission( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;
	fstring entityName = pEmitter->GetName();
	fstring idPrefix = entityName + "-emitter" + "_" + DAE_EMIT_EMISSION;

	ANIM->ExportProperty( idPrefix + "_" + DAE_EMIT_MAX_PARTICLES, pParamBlock, FSParticlePCloud::PB_PTOTALNUMBER, pEmitter->GetMaxParticles(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_EMIT_BIRTH_RATE, pParamBlock, FSParticlePCloud::PB_PBIRTHRATE, pEmitter->GetParticleBirthRate(), &FSConvert::PerFrameToPerSec );
}

void EmitterExporter::ExportEmitterRandomization( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;
	fstring entityName = pEmitter->GetName();
	fstring idPrefix = entityName + "-emitter" + "_" + DAE_EMIT_RANDOMIZATION;

	ANIM->ExportProperty( idPrefix + "_" + DAE_EMIT_SEED, pParamBlock, FSParticlePCloud::PB_RNDSEED, pEmitter->GetSeed(), NULL );
}

void EmitterExporter::ExportParticleLife( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;
	fstring entityName = pEmitter->GetName();
	fstring idPrefix = entityName + "-emitter" + "_" + DAE_PART_PARTICLE_LIFE;

	// particle timing
	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_LIFE, pParamBlock, FSParticlePCloud::PB_LIFE, pEmitter->GetParticleLife(), &FSConvert::MaxTickToSecs );
	ANIM->ExportProperty( idPrefix + "_" + FUDaeEmitter::GetParamVarianceName( DAE_PART_LIFE ), pParamBlock, FSParticlePCloud::PB_LIFEVAR, pEmitter->GetParticleLifeVar(), &FSConvert::MaxTickToSecs );
}

void EmitterExporter::ExportParticleSize( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;
	fstring entityName = pEmitter->GetName();
	fstring idPrefix = entityName + "-emitter" + "_" + DAE_PART_PARTICLE_SIZE;

	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_SIZE, pParamBlock, FSParticlePCloud::PB_SIZE, pEmitter->GetParticleSize(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + FUDaeEmitter::GetParamVarianceName( DAE_PART_SIZE ), pParamBlock, FSParticlePCloud::PB_SIZEVAR, pEmitter->GetParticleSizeVar(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_GROW_FOR, pParamBlock, FSParticlePCloud::PB_GROWTIME, pEmitter->GetParticleSizeGrowFor(), &FSConvert::MaxTickToSecs );
	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_FADE_FOR, pParamBlock, FSParticlePCloud::PB_FADETIME, pEmitter->GetParticleSizeFadeFor(), &FSConvert::MaxTickToSecs );
}

void EmitterExporter::ExportVariableVector( SimpleParticle *pMaxSimpleParticle, fstring &prefix, const FMVector3 &vectorPBlockIds, FCDParameterAnimatableVector3 &vector, int variancePBlockId, FCDParameterAnimatableFloat &variance )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;

	prefix = prefix + "_" + DAE_PART_VARIABLE_VECTOR;
	ANIM->ExportProperty( prefix + "_" + DAE_PART_VECTOR, pParamBlock, vectorPBlockIds[0], vectorPBlockIds[1], vectorPBlockIds[2], vector, NULL );
	ANIM->ExportProperty( prefix + "_" + DAE_EMIT_VARIANCE, pParamBlock, variancePBlockId, variance, NULL );
}

void EmitterExporter::ExportParticleMotion( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;
	fstring entityName = pEmitter->GetName();
	fstring idPrefix = entityName + "-emitter" + "_" + DAE_PART_PARTICLE_MOTION;

	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_SPEED, pParamBlock, FSParticlePCloud::PB_SPEED, pEmitter->GetParticleSpeed(), &FSConvert::PerFrameToPerSec );
	ANIM->ExportProperty( idPrefix + "_" + FUDaeEmitter::GetParamVarianceName( DAE_PART_SPEED ), pParamBlock, FSParticlePCloud::PB_SPEEDVAR, pEmitter->GetParticleSpeedVar(), NULL );

	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_DIRECTION, pParamBlock, FSParticlePCloud::PB_SPEEDDIR, pEmitter->GetParticleDirectionMethod(), NULL );
	
	FUDaeEmitter::ParticleDirectionMethod directionMethod = ( FUDaeEmitter::ParticleDirectionMethod )( int ) pEmitter->GetParticleDirectionMethod();
	fstring dirMethodPrefix = idPrefix + "_" + DAE_PART_DIRECTION + "_" + FUDaeEmitter::ToStringParticleDirectionMethod( directionMethod );
	if ( directionMethod == FUDaeEmitter::PDM_VECTOR )
	{
		ExportVariableVector( pMaxSimpleParticle, dirMethodPrefix, 
			FMVector3( FSParticlePCloud::PB_DIRX, FSParticlePCloud::PB_DIRY, FSParticlePCloud::PB_DIRZ ), pEmitter->GetParticleDirection(), 
			FSParticlePCloud::PB_DIRVAR, pEmitter->GetParticleDirectionVar() 
			);
	}
	else if ( directionMethod == FUDaeEmitter::PDM_OBJECT )
	{
		ICPCloudParticle *pCloudParticle = ( ICPCloudParticle * ) pMaxSimpleParticle;
		pEmitter->SetParticleMotionTargetNode( pCloudParticle->mbase );
		ANIM->ExportProperty( dirMethodPrefix + "_" + DAE_EMIT_VARIANCE, pParamBlock, FSParticlePCloud::PB_DIRVAR, pEmitter->GetParticleDirectionVar(), NULL );
	}
}

void EmitterExporter::ExportParticleType( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;
	fstring entityName = pEmitter->GetName();
	fstring idPrefix = entityName + "-emitter" + "_" + DAE_PART_PARTICLE_TYPE;

	ANIM->ExportProperty( idPrefix, pParamBlock, FSParticlePCloud::PB_PARTICLECLASS, pEmitter->GetParticleType(), NULL );
	FUDaeEmitter::ParticleType particleType = (FUDaeEmitter::ParticleType)(int)pEmitter->GetParticleType();
	idPrefix = idPrefix + "_" + FUDaeEmitter::ToStringParticleType( particleType );
	if ( particleType == FUDaeEmitter::PT_STANDARD )
	{
		ANIM->ExportProperty( idPrefix + "_" + DAE_PART_STD_TYPE, pParamBlock, FSParticlePCloud::PB_PARTICLETYPE, pEmitter->GetStdParticleType(), NULL );
	}
	else if ( particleType == FUDaeEmitter::PT_INSTANCED_GEOMETRY )
	{
		ICPCloudParticle *pCloudParticle = ( ICPCloudParticle * ) pMaxSimpleParticle;
		pEmitter->SetInstancedGeometryTargetNode( pCloudParticle->custnode );
		ANIM->ExportProperty( idPrefix + "_" + DAE_PART_USE_SUBTREE, pParamBlock, FSParticlePCloud::PB_USESUBTREE, pEmitter->GetInstancedGeometryUseSubtree(), NULL );
	}
}

void EmitterExporter::ExportParticleAnimationOffset( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;
	fstring entityName = pEmitter->GetName();
	fstring idPrefix = entityName + "-emitter" + "_" + DAE_PART_ANIMATION_OFFSET;

	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_ANIMATION_OFFSET_METHOD, pParamBlock, FSParticlePCloud::PB_ANIMATION_OFFSET_METHOD, pEmitter->GetAnimationOffsetMethod(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_ANIMATION_OFFSET_AMOUNT, pParamBlock, FSParticlePCloud::PB_ANIMATION_OFFSET, pEmitter->GetAnimationOffsetAmount(), &FSConvert::MaxTickToSecs );
	ANIM->ExportProperty( idPrefix + "_" + FUDaeEmitter::GetParamVarianceName( DAE_PART_ANIMATION_OFFSET_AMOUNT ), pParamBlock, FSParticlePCloud::PB_ANIMATION_OFFSET_VARIATION, pEmitter->GetAnimationOffsetVariance(), NULL );
}

void EmitterExporter::ExportParticleAnimationScaling( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;
	fstring entityName = pEmitter->GetName();
	fstring idPrefix = entityName + "-emitter" + "_" + DAE_PART_ANIMATION_SCALING;

	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_ANIMATION_SCALING_METHOD, pParamBlock, FSParticlePCloud::PB_ANIMATION_SCALING_METHOD, pEmitter->GetAnimationScalingMethod(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_ANIMATION_SCALING_SRC_LENGTH, pParamBlock, FSParticlePCloud::PB_ANIMATION_SCALING_SRC_LENGTH, pEmitter->GetAnimationScalingSrcLength(), &FSConvert::MaxTickToSecs );
	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_ANIMATION_SCALING_SRC_VARIANCE, pParamBlock, FSParticlePCloud::PB_ANIMATION_SCALING_SRC_VARIANCE, pEmitter->GetAnimationScalingSrcVariance(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_ANIMATION_SCALING_DST_MULTIPLIER, pParamBlock, FSParticlePCloud::PB_ANIMATION_SCALING_DST_MULTIPLIER, pEmitter->GetAnimationScalingDstMultiplier(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_ANIMATION_SCALING_DST_VARIANCE, pParamBlock, FSParticlePCloud::PB_ANIMATION_SCALING_DST_VARIANCE, pEmitter->GetAnimationScalingDstVariance(), NULL );
}

void EmitterExporter::ExportParticleSpin( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle )
{
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;
	fstring entityName = pEmitter->GetName();
	fstring idPrefix = entityName + "-emitter" + "_" + DAE_PART_PARTICLE_SPIN;

	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_SPIN_TIME, pParamBlock, FSParticlePCloud::PB_SPINTIME, pEmitter->GetParticleSpinTime(), &FSConvert::MaxTickToSecs );
	ANIM->ExportProperty( idPrefix + "_" + FUDaeEmitter::GetParamVarianceName( DAE_PART_SPIN_TIME ), pParamBlock, FSParticlePCloud::PB_SPINTIMEVAR, pEmitter->GetParticleSpinTimeVar(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_SPIN_PHASE, pParamBlock, FSParticlePCloud::PB_SPINPHASE, pEmitter->GetParticleSpinPhase(), &FSConvert::RadToDeg );
	ANIM->ExportProperty( idPrefix + "_" + FUDaeEmitter::GetParamVarianceName( DAE_PART_SPIN_PHASE ), pParamBlock, FSParticlePCloud::PB_SPINPHASEVAR, pEmitter->GetParticleSpinPhaseVar(), NULL );
	ANIM->ExportProperty( idPrefix + "_" + DAE_PART_SPIN_AXIS_TYPE, pParamBlock, FSParticlePCloud::PB_SPINAXISTYPE, pEmitter->GetParticleSpinAxisType(), NULL );

	FUDaeEmitter::ParticleSpinAxisType spinAxisType = ( FUDaeEmitter::ParticleSpinAxisType )( int ) pEmitter->GetParticleSpinAxisType();
	fstring spinAxisIdPrefix = idPrefix + "_" + DAE_PART_SPIN_AXIS + "_" + FUDaeEmitter::ToStringParticleSpinAxisType( spinAxisType );
	if ( spinAxisType == FUDaeEmitter::PSAT_VECTOR )
	{
		ExportVariableVector( pMaxSimpleParticle, spinAxisIdPrefix, 
			FMVector3( FSParticlePCloud::PB_SPINAXISX, FSParticlePCloud::PB_SPINAXISY, FSParticlePCloud::PB_SPINAXISZ ), pEmitter->GetParticleSpinAxis(), 
			FSParticlePCloud::PB_SPINAXISVAR, pEmitter->GetParticleSpinAxisVar() 
			);
	}
	else if ( spinAxisType == FUDaeEmitter::PSAT_TRAVEL_OR_MBLUR_DIR )
	{
		ANIM->ExportProperty( spinAxisIdPrefix + "_" + DAE_PART_STRETCH, pParamBlock, FSParticlePCloud::PB_STRETCH, pEmitter->GetParticleStretch(), NULL );
	}
}

FCDEntity* EmitterExporter::ExportEmitter(INode* node, Object* object, int flags)
{
	if (node == NULL || object == NULL) return NULL;

	ParticleObject *pMaxParticleObject = GetParticleInterface( object );
	if ( !pMaxParticleObject )
	{
		return NULL;
	}

	bool isXRef = false;
	if (XRefFunctions::IsXRefObject(object))
	{
		// Resolve the source
		object = XRefFunctions::GetXRefObjectSource(object);
		if (OPTS->ExportXRefs()) isXRef = true;
	}

	// Create the COLLADA emitter entity and its mesh
	FCDEmitter* pEmitter = isXRef ? new FCDEmitter(CDOC) : CDOC->GetEmitterLibrary()->AddEntity();

	// temporarily deactivating animation export if xref
	bool bExportAnimationsBackup = OPTS->ExportAnimations();
	OPTS->animations = !isXRef;

	fstring entityName = fstring( node->GetName() ) + "-emitter";
	ENTE->ExportEntity(object, pEmitter, entityName.c_str());
	pEmitter->SetName(fm::string(node->GetName()));

	pEmitter->SetEmitterType( FSEmitter::PCLOUD );

	SimpleParticle *pMaxSimpleParticle = ( SimpleParticle * )pMaxParticleObject;
	IParamBlock *pParamBlock = pMaxSimpleParticle->pblock;

	ExportEmitterShape( pEmitter, pMaxSimpleParticle );
	ExportEmitterEmission( pEmitter, pMaxSimpleParticle );
	ExportEmitterRandomization( pEmitter, pMaxSimpleParticle );

	ExportParticleLife( pEmitter, pMaxSimpleParticle );
	ExportParticleSize( pEmitter, pMaxSimpleParticle );
	ExportParticleMotion( pEmitter, pMaxSimpleParticle );
	ExportParticleType( pEmitter, pMaxSimpleParticle );
	ExportParticleAnimationOffset( pEmitter, pMaxSimpleParticle );
	ExportParticleAnimationScaling( pEmitter, pMaxSimpleParticle );
	ExportParticleSpin( pEmitter, pMaxSimpleParticle );

	OPTS->animations = bExportAnimationsBackup;

	// material
	// Retrieve the list of materials assigned to the different polygons of this mesh
	Mtl* mat = node->GetMtl();
	MaterialList materials;
	FLATTEN_MATERIALS( mat, materials );

	fm::vector<size_t> matIDs;
	size_t numMaterials = materials.size();

	if (numMaterials > 1)
	{
		MATE->GetSubMaterialIds( mat, matIDs );
	} 
	else
	{
		matIDs.push_back(0);
	}

	for (size_t* it = matIDs.begin(); it != matIDs.end(); ++it)
	{
		size_t matID = *it;
		fstring semantic;
		EmitterMaterial emitterMaterial;

		ExportedMaterial* exportedMat = GEOM->GetExportedMaterial( mat, matID, materials, semantic );

		emitterMaterial.m_exportedMaterial = exportedMat;
		emitterMaterial.m_semantic = semantic;

		pEmitter->AddEmitterMaterial( emitterMaterial );
	}

	// get forces
	NodeList maxForceNodes;
	GetEmitterForceMaxNodes( node, maxForceNodes );
	for ( size_t forceNodeId = 0; forceNodeId < maxForceNodes.size(); ++forceNodeId )
	{
		pEmitter->AddForceMaxNode( maxForceNodes[forceNodeId] );
	}

	return pEmitter;
}

void EmitterExporter::ResolveModifiers(INode *pNode, Object* obj, ColladaModifierStack &modifiers )
{
	if (obj == NULL)
	{
		return;
	}

	SClass_ID		sc;
	IDerivedObject* dobj;
	Object *currentObject = obj;

	int modIdx = 0;
	sc = obj->SuperClassID();
	if (sc == GEN_DERIVOB_CLASS_ID || sc == DERIVOB_CLASS_ID || sc == WSM_DERIVOB_CLASS_ID)
	{
		dobj = (IDerivedObject*)obj;
		while (sc == GEN_DERIVOB_CLASS_ID || sc == DERIVOB_CLASS_ID || sc == WSM_DERIVOB_CLASS_ID)
		{
			for (int j = 0; j < dobj->NumModifiers(); j++, modIdx++)
			{
				ICGravityModifier gravityMod;
				ICWindModifier windMod;
				ICDeflectorModifier deflectorMod;

				Modifier* mod = dobj->GetModifier(j);

				ColladaModifier *pColladaModifier = NULL;
				if ( gravityMod.Matches( mod ) )
				{
					pColladaModifier = new ICGravityModifier();
				}
				else if ( windMod.Matches( mod ) )
				{
					pColladaModifier = new ICWindModifier();
				}
				else if ( deflectorMod.Matches( mod ) )
				{
					pColladaModifier = new ICDeflectorModifier();
				}

				if ( pColladaModifier )
				{
					pColladaModifier->SetDerivedObject( dobj );
					pColladaModifier->SetModifierIndex( j );
					pColladaModifier->SetModifierStackIndex( modIdx );
					pColladaModifier->SetResolved( true );

					modifiers.push_back( pColladaModifier );
				}
			}
			dobj = (IDerivedObject*) dobj->GetObjRef();
			currentObject = (Object*) dobj;
			sc = dobj->SuperClassID();
		}
	}

	int bct = currentObject->NumPipeBranches(FALSE);
	if (bct > 0)
	{
		for (int bi = 0; bi < bct; bi++)
		{
			Object* bobj = currentObject->GetPipeBranch(bi,FALSE);
			ResolveModifiers(pNode, bobj,modifiers);
		}
	}
}