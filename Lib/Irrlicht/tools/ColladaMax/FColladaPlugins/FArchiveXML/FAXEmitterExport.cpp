/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FArchiveXML.h"
#include "FCDocument/FCDGeometry.h"
#include "FCDocument/FCDEmitter.h"
#include "FCDocument/FCDEmitterObject.h"
#include "FCDocument/FCDEmitterParticle.h"
#include "FCDocument/FCDParticleModifier.h"
#include "FUtils/FUDaeEnumSyntax.h"
#include "FAXICHelpers.h"


void WriteEmitterTarget( xmlNode *pParent, const fstring &uriString )
{
	xmlNode *pTargetNode = AddChild( pParent, DAE_PART_TARGET );
	FAXICHelpers::AddUriContent( pTargetNode, uriString );
}

void WriteEmitterShape( FCDEmitter *pEmitter, xmlNode *pParent )
{
	xmlNode *pShapeNode = AddChildWithSid( pParent, DAE_EMIT_SHAPE, DAE_EMIT_SHAPE );

	FUDaeEmitter::Shape shape = (FUDaeEmitter::Shape)(int)pEmitter->GetShape();
	if ( shape == FUDaeEmitter::SHAPE_BOX )
	{
		xmlNode *pSpecificShapeNode = AddChildWithSid( pShapeNode, DAE_EMIT_SHAPE_BOX, DAE_EMIT_SHAPE_BOX );
		FAXICHelpers::WriteAnimatedParameterContent( pSpecificShapeNode, DAE_EMIT_SHAPE_LENGTH, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetShapeLength() );
		FAXICHelpers::WriteAnimatedParameterContent( pSpecificShapeNode, DAE_EMIT_SHAPE_WIDTH, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetShapeWidth() );
		FAXICHelpers::WriteAnimatedParameterContent( pSpecificShapeNode, DAE_EMIT_SHAPE_HEIGHT, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetShapeHeight() );
	}
	else if ( shape == FUDaeEmitter::SHAPE_SPHERE )
	{
		xmlNode *pSpecificShapeNode = AddChildWithSid( pShapeNode, DAE_EMIT_SHAPE_SPHERE, DAE_EMIT_SHAPE_SPHERE );
		FAXICHelpers::WriteAnimatedParameterContent( pSpecificShapeNode, DAE_EMIT_SHAPE_RADIUS, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetShapeRadius() );
	}
	else if ( shape == FUDaeEmitter::SHAPE_CYLINDER )
	{
		xmlNode *pSpecificShapeNode = AddChildWithSid( pShapeNode, DAE_EMIT_SHAPE_CYLINDER, DAE_EMIT_SHAPE_CYLINDER );
		FAXICHelpers::WriteAnimatedParameterContent( pSpecificShapeNode, DAE_EMIT_SHAPE_RADIUS, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetShapeRadius() );
		FAXICHelpers::WriteAnimatedParameterContent( pSpecificShapeNode, DAE_EMIT_SHAPE_HEIGHT, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetShapeHeight() );
	}
	else
	{
		fstring uriString = pEmitter->GetDocument()->GetFileManager()->CleanUri( pEmitter->GetShapeTargetUri() );
		WriteEmitterTarget( pShapeNode, uriString );
	}
}

void WriteEmitterEmission( FCDEmitter *pEmitter, xmlNode *pParent )
{
	xmlNode *pEmissionNode = AddChildWithSid( pParent, DAE_EMIT_EMISSION, DAE_EMIT_EMISSION );
	FAXICHelpers::WriteUnanimatedParameter( pEmissionNode, DAE_EMIT_MAX_PARTICLES, DAEFC_FLOAT_ATTRIBUTE_TYPE, ( int )pEmitter->GetMaxParticles() );
	FAXICHelpers::WriteAnimatedParameterContent( pEmissionNode, DAE_EMIT_BIRTH_RATE, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleBirthRate() );
}

void WriteEmitterRandomization( FCDEmitter *pEmitter, xmlNode *pParent )
{
	xmlNode *pRandomizationNode = AddChildWithSid( pParent, DAE_EMIT_RANDOMIZATION, DAE_EMIT_RANDOMIZATION );
	FAXICHelpers::WriteUnanimatedParameter( pRandomizationNode, DAE_EMIT_SEED, DAEFC_FLOAT_ATTRIBUTE_TYPE, ( int )pEmitter->GetSeed() );
}

void WriteParticleLife( FCDEmitter *pEmitter, xmlNode *pParent )
{
	xmlNode *pLifeNode = AddChildWithSid( pParent, DAE_PART_PARTICLE_LIFE, DAE_PART_PARTICLE_LIFE );

	FAXICHelpers::WriteAnimatedParameterContent( pLifeNode, DAE_PART_LIFE, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleLife() );
	FAXICHelpers::WriteAnimatedParameterContent( pLifeNode, FUDaeEmitter::GetParamVarianceName( DAE_PART_LIFE ), DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleLifeVar() );
}

void WriteParticleSize( FCDEmitter *pEmitter, xmlNode *pParent )
{
	xmlNode *pSizeNode = AddChildWithSid( pParent, DAE_PART_PARTICLE_SIZE, DAE_PART_PARTICLE_SIZE );

	FAXICHelpers::WriteAnimatedParameterContent( pSizeNode, DAE_PART_SIZE, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleSize() );
	FAXICHelpers::WriteAnimatedParameterContent( pSizeNode, FUDaeEmitter::GetParamVarianceName( DAE_PART_SIZE ), DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleSizeVar() );
	FAXICHelpers::WriteAnimatedParameterContent( pSizeNode, DAE_PART_GROW_FOR, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleSizeGrowFor() );
	FAXICHelpers::WriteAnimatedParameterContent( pSizeNode, DAE_PART_FADE_FOR, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleSizeFadeFor() );
}

void WriteEmitterVariableVector( xmlNode *pParent, FCDParameterAnimatableVector3 &vector, FCDParameterAnimatableFloat &variance )
{
	xmlNode *pVariableVectorNode = AddChildWithSid( pParent, DAE_PART_VARIABLE_VECTOR, DAE_PART_VARIABLE_VECTOR );
	FAXICHelpers::WriteAnimatedParameterContent( pVariableVectorNode, DAE_PART_VECTOR, DAEFC_FLOAT3_ATTRIBUTE_TYPE, vector );
	FAXICHelpers::WriteAnimatedParameterContent( pVariableVectorNode, DAE_EMIT_VARIANCE, DAEFC_FLOAT_ATTRIBUTE_TYPE, variance );
}

void WriteEmitterDirectionTarget( xmlNode *pParent, fstring &uriString, FCDParameterAnimatableFloat *variance )
{
	xmlNode *pDirectionTargetNode = AddChildWithSid( pParent, DAE_PART_DIRECTION_TARGET, DAE_PART_DIRECTION_TARGET );
	WriteEmitterTarget( pDirectionTargetNode, uriString );
	FAXICHelpers::WriteAnimatedParameterContent( pDirectionTargetNode, DAE_EMIT_VARIANCE, DAEFC_FLOAT_ATTRIBUTE_TYPE, *variance );
}

void WriteParticleMotion( FCDEmitter *pEmitter, xmlNode *pParent )
{
	xmlNode *pMotionNode = AddChildWithSid( pParent, DAE_PART_PARTICLE_MOTION, DAE_PART_PARTICLE_MOTION );

	FAXICHelpers::WriteAnimatedParameterContent( pMotionNode, DAE_PART_SPEED, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleSpeed() );
	FAXICHelpers::WriteAnimatedParameterContent( pMotionNode, FUDaeEmitter::GetParamVarianceName( DAE_PART_SPEED ), DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleSpeedVar() );
	
	xmlNode *pDirectionNode = AddChildWithSid( pMotionNode, DAE_PART_DIRECTION, DAE_PART_DIRECTION );

	FUDaeEmitter::ParticleDirectionMethod directionMethod = ( FUDaeEmitter::ParticleDirectionMethod )( int )pEmitter->GetParticleDirectionMethod();
	if ( directionMethod == FUDaeEmitter::PDM_RANDOM )
	{
		AddChild( pDirectionNode, DAE_PART_RANDOM_VECTOR );
	}
	else if ( directionMethod == FUDaeEmitter::PDM_VECTOR )
	{
		WriteEmitterVariableVector( pDirectionNode, pEmitter->GetParticleDirection(), pEmitter->GetParticleDirectionVar() );
	}
	else if ( directionMethod == FUDaeEmitter::PDM_OBJECT )
	{
		fstring uriString = pEmitter->GetDocument()->GetFileManager()->CleanUri( pEmitter->GetParticleMotionTargetUri() );
		WriteEmitterDirectionTarget( pDirectionNode, uriString, &pEmitter->GetParticleDirectionVar() );
	}
}

void WriteParticleType( FCDEmitter *pEmitter, xmlNode *pParent )
{
	xmlNode *pTypeNode = AddChildWithSid( pParent, DAE_PART_PARTICLE_TYPE, DAE_PART_PARTICLE_TYPE );
	xmlNode *pCurrentTypeNode = NULL;
	
	WRITE_IC_ENUM_PARAM_AS_CHILD_WITH_SID( FUDaeEmitter, pTypeNode, ParticleType, pEmitter, pCurrentTypeNode );

	FUDaeEmitter::ParticleType particleType = (FUDaeEmitter::ParticleType)(int)pEmitter->GetParticleType();
	if ( particleType == FUDaeEmitter::PT_STANDARD )
	{
		WRITE_IC_ENUM_PARAM_AS_INT( FUDaeEmitter, pCurrentTypeNode, StdParticleType, pEmitter, DAE_PART_STD_TYPE );
	}
	else if ( particleType == FUDaeEmitter::PT_INSTANCED_GEOMETRY )
	{
		fstring uriString = pEmitter->GetDocument()->GetFileManager()->CleanUri( pEmitter->GetInstancedGeometryTargetUri() );
		WriteEmitterTarget( pCurrentTypeNode, uriString );

		WRITE_IC_ENUM_PARAM_AS_STRING( FUDaeBool, pCurrentTypeNode, Bool, pEmitter, GetInstancedGeometryUseSubtree, DAE_PART_USE_SUBTREE );
	}
}

void WriteParticleAnimationOffset( FCDEmitter *pEmitter, xmlNode *pParent )
{
	xmlNode *pAnimationOffsetNode = AddChildWithSid( pParent, DAE_PART_ANIMATION_OFFSET, DAE_PART_ANIMATION_OFFSET );

	WRITE_IC_ENUM_PARAM_AS_STRING( FUDaeEmitter, pAnimationOffsetNode, AnimationOffsetMethod, pEmitter, GetAnimationOffsetMethod, DAE_PART_ANIMATION_OFFSET_METHOD ); 
	FAXICHelpers::WriteAnimatedParameterContent( pAnimationOffsetNode, DAE_PART_ANIMATION_OFFSET_AMOUNT, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetAnimationOffsetAmount() );
	FAXICHelpers::WriteAnimatedParameterContent( pAnimationOffsetNode, FUDaeEmitter::GetParamVarianceName( DAE_PART_ANIMATION_OFFSET_AMOUNT ), DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetAnimationOffsetVariance() );
}

void WriteParticleAnimationScaling( FCDEmitter *pEmitter, xmlNode *pParent )
{
	xmlNode *pAnimationScalingNode = AddChildWithSid( pParent, DAE_PART_ANIMATION_SCALING, DAE_PART_ANIMATION_SCALING );

	WRITE_IC_ENUM_PARAM_AS_STRING( FUDaeEmitter, pAnimationScalingNode, AnimationScalingMethod, pEmitter, GetAnimationScalingMethod, DAE_PART_ANIMATION_SCALING_METHOD ); 
	FAXICHelpers::WriteAnimatedParameterContent( pAnimationScalingNode, DAE_PART_ANIMATION_SCALING_SRC_LENGTH, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetAnimationScalingSrcLength() );
	FAXICHelpers::WriteAnimatedParameterContent( pAnimationScalingNode, DAE_PART_ANIMATION_SCALING_SRC_VARIANCE, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetAnimationScalingSrcVariance() );
	FAXICHelpers::WriteAnimatedParameterContent( pAnimationScalingNode, DAE_PART_ANIMATION_SCALING_DST_MULTIPLIER, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetAnimationScalingDstMultiplier() );
	FAXICHelpers::WriteAnimatedParameterContent( pAnimationScalingNode, DAE_PART_ANIMATION_SCALING_DST_VARIANCE, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetAnimationScalingDstVariance() );
}

void WriteParticleSpin( FCDEmitter *pEmitter, xmlNode *pParent )
{
	xmlNode *pSpinNode = AddChildWithSid( pParent, DAE_PART_PARTICLE_SPIN, DAE_PART_PARTICLE_SPIN );

	FAXICHelpers::WriteAnimatedParameterContent( pSpinNode, DAE_PART_SPIN_TIME, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleSpinTime() );
	FAXICHelpers::WriteAnimatedParameterContent( pSpinNode, FUDaeEmitter::GetParamVarianceName( DAE_PART_SPIN_TIME ), DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleSpinTimeVar() );
	FAXICHelpers::WriteAnimatedParameterContent( pSpinNode, DAE_PART_SPIN_PHASE, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleSpinPhase() );
	FAXICHelpers::WriteAnimatedParameterContent( pSpinNode, FUDaeEmitter::GetParamVarianceName( DAE_PART_SPIN_PHASE ), DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleSpinPhaseVar() );

	xmlNode *pSpinAxisNode = AddChildWithSid( pSpinNode, DAE_PART_SPIN_AXIS, DAE_PART_SPIN_AXIS );

	FUDaeEmitter::ParticleSpinAxisType particleSpinAxisType = (FUDaeEmitter::ParticleSpinAxisType)(int)pEmitter->GetParticleSpinAxisType();
	if ( particleSpinAxisType == FUDaeEmitter::PSAT_RANDOM )
	{
		AddChild( pSpinAxisNode, DAE_PART_RANDOM_VECTOR );
	}
	else if ( particleSpinAxisType == FUDaeEmitter::PSAT_TRAVEL_OR_MBLUR_DIR )
	{
		WRITE_IC_ENUM_PARAM_AS_CHILD_WITH_SID( FUDaeEmitter, pSpinAxisNode, ParticleSpinAxisType, pEmitter, pSpinAxisNode );
		FAXICHelpers::WriteAnimatedParameterContent( pSpinAxisNode, DAE_PART_STRETCH, DAEFC_FLOAT_ATTRIBUTE_TYPE, pEmitter->GetParticleStretch() );
	}
	else if ( particleSpinAxisType == FUDaeEmitter::PSAT_VECTOR )
	{
		WriteEmitterVariableVector( pSpinAxisNode, pEmitter->GetParticleSpinAxis(), pEmitter->GetParticleSpinAxisVar() );
	}
}

//
// Emitter export
//

xmlNode* FArchiveXML::WriteEmitter(FCDObject* object, xmlNode* parentNode)
{
	FCDEmitter* pEmitter = (FCDEmitter*) object;
	xmlNode* emitterNode = FArchiveXML::WriteToEntityXMLFCDEntity(pEmitter, parentNode, DAE_EMITTER_ELEMENT);

	// emitter
	WriteEmitterShape( pEmitter, emitterNode );
	WriteEmitterEmission( pEmitter, emitterNode );
	WriteEmitterRandomization( pEmitter, emitterNode );

	// particles
	WriteParticleLife( pEmitter, emitterNode );
	WriteParticleSize( pEmitter, emitterNode );
	WriteParticleMotion( pEmitter, emitterNode );
	WriteParticleType( pEmitter, emitterNode );
	WriteParticleAnimationOffset( pEmitter, emitterNode );
	WriteParticleAnimationScaling( pEmitter, emitterNode );
	WriteParticleSpin( pEmitter, emitterNode );

	FArchiveXML::WriteEntityExtra(pEmitter, emitterNode);
	return emitterNode;
}
