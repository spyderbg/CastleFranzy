/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDEmitter.h"
#include "FCDocument/FCDForceField.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDEmitterParticle.h"
#include "FCDocument/FCDEntityReference.h"

//
// FCDEmitter
//
ImplementObjectType(FCDEmitter);

FCDEmitter::FCDEmitter(FCDocument* document)
:	FCDEntity(document, "Emitter")
,	InitializeParameter(m_emitterType, FSEmitter::PCLOUD)
,	InitializeParameterAnimatable(m_shape, 0.0f)
,	InitializeParameterAnimatable(m_shapeLength, 0.0f)
,	InitializeParameterAnimatable(m_shapeWidth, 0.0f)
,	InitializeParameterAnimatable(m_shapeHeight, 0.0f)
,	InitializeParameterAnimatable(m_shapeRadius, 0.0f)
,	InitializeParameterAnimatable(m_particleBirthRate, 0.0f)
,	InitializeParameterAnimatable(m_particleSpeed, 0.0f)
,	InitializeParameterAnimatable(m_particleSpeedVar, 0.0f)
,	InitializeParameterAnimatable(m_particleDirectionMethod, 0.0f)
,	InitializeParameterAnimatable(m_particleDirection, FMVector3::Zero )
,	InitializeParameterAnimatable(m_particleDirectionVar, 0.0f)
,	InitializeParameter(m_pParticleMotionTargetNode, NULL)
,	InitializeParameterAnimatable(m_maxParticles, 0.0f)
,	InitializeParameterAnimatable(m_particleLife, 0.0f)
,	InitializeParameterAnimatable(m_particleLifeVar, 0.0f)
,	InitializeParameterAnimatable(m_particleSize, 0.0f)
,	InitializeParameterAnimatable(m_particleSizeVar, 0.0f)
,	InitializeParameterAnimatable(m_particleSizeGrowFor, 0.0f)
,	InitializeParameterAnimatable(m_particleSizeFadeFor, 0.0f)
,	InitializeParameterAnimatable(m_seed, 0.0f)
,	InitializeParameterAnimatable(m_particleType, 0.0f)
,	InitializeParameterAnimatable(m_stdParticleType, 0.0f)
,	InitializeParameterAnimatable(m_instancedGeometryUseSubtree, 0.0f)
,	InitializeParameter(m_pInstancedGeometryTargetNode, NULL)
,	InitializeParameterAnimatable(m_animationOffsetMethod, 0.0f)
,	InitializeParameterAnimatable(m_animationOffsetAmount, 0.0f)
,	InitializeParameterAnimatable(m_animationOffsetVariance, 0.0f)
,	InitializeParameterAnimatable(m_animationScalingMethod, 0.0f)
,	InitializeParameterAnimatable(m_animationScalingSrcLength, 0.0f)
,	InitializeParameterAnimatable(m_animationScalingSrcVariance, 0.0f)
,	InitializeParameterAnimatable(m_animationScalingDstMultiplier, 0.0f)
,	InitializeParameterAnimatable(m_animationScalingDstVariance, 0.0f)
,	InitializeParameterAnimatable(m_particleSpinTime, 0.0f)
,	InitializeParameterAnimatable(m_particleSpinTimeVar, 0.0f)
,	InitializeParameterAnimatable(m_particleSpinPhase, 0.0f)
,	InitializeParameterAnimatable(m_particleSpinPhaseVar, 0.0f)
,	InitializeParameterAnimatable(m_particleSpinAxisType, 0.0f)
,	InitializeParameterAnimatable(m_particleSpinAxis, FMVector3::Zero )
,	InitializeParameterAnimatable(m_particleSpinAxisVar, 0.0f)
,	InitializeParameterAnimatable(m_particleStretch, 0.0f)
{
	// Always create this.
	m_pShapeTarget = new FCDEntityReference(document, NULL);
	m_pParticleMotionTarget = new FCDEntityReference(document, NULL);
	m_pInstancedGeometryTarget = new FCDEntityReference(document, NULL);
}

FCDEmitter::~FCDEmitter()
{
	SAFE_RELEASE( m_pShapeTarget );
	SAFE_RELEASE( m_pParticleMotionTarget );
	SAFE_RELEASE( m_pInstancedGeometryTarget );
	CLEAR_POINTER_VECTOR( m_forceFCDSceneNodeRefs );
}

FCDEntity* FCDEmitter::Clone(FCDEntity* _clone, bool cloneChildren) const
{
	FCDEmitter* clone = NULL;
	if (_clone == NULL) _clone = clone = new FCDEmitter(const_cast<FCDocument*>(GetDocument()));
	else if (_clone->HasType(FCDEmitter::GetClassType())) clone = (FCDEmitter*) _clone;

	Parent::Clone(_clone, cloneChildren);

	if (clone != NULL)
	{
#define CLONE_OBJECT_REF( objectRef ){ clone->objectRef->SetEntity( const_cast<FCDEntity*>( objectRef->GetEntity() ) ); }

		CLONE_PARAMETER( clone, m_emitterMaterials );
		CLONE_PARAMETER( clone, m_emitterType );
		CLONE_PARAMETER( clone, m_shape );
		CLONE_PARAMETER( clone, m_shapeLength );
		CLONE_PARAMETER( clone, m_shapeWidth );
		CLONE_PARAMETER( clone, m_shapeHeight );
		CLONE_PARAMETER( clone, m_shapeRadius );
		CLONE_OBJECT_REF( m_pShapeTarget );
		CLONE_PARAMETER( clone, m_particleBirthRate );
		CLONE_PARAMETER( clone, m_particleSpeed );
		CLONE_PARAMETER( clone, m_particleSpeedVar );
		CLONE_PARAMETER( clone, m_particleDirectionMethod );
		CLONE_PARAMETER( clone, m_particleDirection );
		CLONE_PARAMETER( clone, m_particleDirectionVar );
		CLONE_OBJECT_REF( m_pParticleMotionTarget );
		CLONE_PARAMETER( clone, m_pParticleMotionTargetNode );
		CLONE_PARAMETER( clone, m_maxParticles );
		CLONE_PARAMETER( clone, m_particleLife );
		CLONE_PARAMETER( clone, m_particleLifeVar );
		CLONE_PARAMETER( clone, m_particleSize );
		CLONE_PARAMETER( clone, m_particleSizeVar );
		CLONE_PARAMETER( clone, m_particleSizeGrowFor );
		CLONE_PARAMETER( clone, m_particleSizeFadeFor );
		CLONE_PARAMETER( clone, m_seed );
		CLONE_PARAMETER( clone, m_particleType );
		CLONE_PARAMETER( clone, m_stdParticleType );
		CLONE_PARAMETER( clone, m_instancedGeometryUseSubtree );
		CLONE_OBJECT_REF( m_pInstancedGeometryTarget );
		CLONE_PARAMETER( clone, m_pInstancedGeometryTargetNode );
		CLONE_PARAMETER( clone, m_animationOffsetMethod );
		CLONE_PARAMETER( clone, m_animationOffsetAmount );
		CLONE_PARAMETER( clone, m_animationOffsetVariance );
		CLONE_PARAMETER( clone, m_animationScalingMethod );
		CLONE_PARAMETER( clone, m_animationScalingSrcLength );
		CLONE_PARAMETER( clone, m_animationScalingSrcVariance );
		CLONE_PARAMETER( clone, m_animationScalingDstMultiplier );
		CLONE_PARAMETER( clone, m_animationScalingDstVariance );
		CLONE_PARAMETER( clone, m_particleSpinTime );
		CLONE_PARAMETER( clone, m_particleSpinTimeVar );
		CLONE_PARAMETER( clone, m_particleSpinPhase );
		CLONE_PARAMETER( clone, m_particleSpinPhaseVar );
		CLONE_PARAMETER( clone, m_particleSpinAxisType );
		CLONE_PARAMETER( clone, m_particleSpinAxis );
		CLONE_PARAMETER( clone, m_particleSpinAxisVar );
		CLONE_PARAMETER( clone, m_particleStretch );
		CLONE_PARAMETER( clone, m_forceMaxNodes );

		size_t numSceneNodeRefs = m_forceFCDSceneNodeRefs.size();
		for ( size_t sceneNodeId = 0; sceneNodeId < numSceneNodeRefs; ++sceneNodeId )
		{
			FCDSceneNode *pSceneNode = ( FCDSceneNode * )const_cast<FCDEntity *>( m_forceFCDSceneNodeRefs[sceneNodeId]->GetEntity() );
			clone->AddForceFCDSceneNode( pSceneNode );
		}

#undef CLONE_OBJECT_REF
	}
	return _clone;
}

FCDSceneNode *FCDEmitter::GetShapeTarget()
{
	return ( FCDSceneNode * )const_cast<FCDEntity *>( m_pShapeTarget->GetEntity() );
}

void FCDEmitter::SetShapeTarget( FCDSceneNode *pShapeTarget )
{
	m_pShapeTarget->SetEntity( ( FCDEntity * )pShapeTarget );
}

FUUri FCDEmitter::GetShapeTargetUri()
{
	return m_pShapeTarget->GetUri();
}

FCDSceneNode *FCDEmitter::GetParticleMotionTarget()
{
	return ( FCDSceneNode * )const_cast<FCDEntity *>( m_pParticleMotionTarget->GetEntity() );
}

void FCDEmitter::SetParticleMotionTarget( FCDSceneNode *pParticleMotionTarget )
{
	m_pParticleMotionTarget->SetEntity( ( FCDEntity * )pParticleMotionTarget );
}

FUUri FCDEmitter::GetParticleMotionTargetUri()
{
	return m_pParticleMotionTarget->GetUri();
}

void FCDEmitter::SetParticleMotionTargetNode( void *pMaxNode )
{
	m_pParticleMotionTargetNode = pMaxNode;
}

void *FCDEmitter::GetParticleMotionTargetNode()
{
	return m_pParticleMotionTargetNode;
}

void FCDEmitter::SetInstancedGeometryTargetNode( void *pMaxNode )
{
	m_pInstancedGeometryTargetNode = pMaxNode;
}

void *FCDEmitter::GetInstancedGeometryTargetNode()
{
	return m_pInstancedGeometryTargetNode;
}

FCDSceneNode *FCDEmitter::GetInstancedGeometryTarget()
{
	return ( FCDSceneNode * )const_cast<FCDEntity *>( m_pInstancedGeometryTarget->GetEntity() );
}

void FCDEmitter::SetInstancedGeometryTarget( FCDSceneNode *pInstancedGeometryTarget )
{
	m_pInstancedGeometryTarget->SetEntity( ( FCDEntity * )pInstancedGeometryTarget );
}

FUUri FCDEmitter::GetInstancedGeometryTargetUri()
{
	return m_pInstancedGeometryTarget->GetUri();
}

size_t FCDEmitter::GetNumForceMaxNodes()
{
	return m_forceMaxNodes.size();
}

void FCDEmitter::AddForceMaxNode( INode *pNode )
{
	m_forceMaxNodes.push_back( pNode );
}

INode *FCDEmitter::GetForceMaxNode( size_t index )
{
	FUAssert( index < m_forceMaxNodes.size(), return NULL );
	return m_forceMaxNodes[index];
}

size_t FCDEmitter::GetNumForceFCDSceneNodes()
{
	return m_forceFCDSceneNodeRefs.size();
}

void FCDEmitter::AddForceFCDSceneNode( FCDSceneNode *pForceFCDSceneNode )
{
	FCDEntityReference *newEntityRef = new FCDEntityReference( GetDocument(), NULL );
	newEntityRef->SetEntity( ( FCDEntity * )pForceFCDSceneNode );
	m_forceFCDSceneNodeRefs.push_back( newEntityRef );
}

FCDSceneNode *FCDEmitter::GetForceFCDSceneNode( size_t index )
{
	FUAssert( index < m_forceFCDSceneNodeRefs.size(), return NULL );
	return ( FCDSceneNode * )const_cast<FCDEntity *>( m_forceFCDSceneNodeRefs[index]->GetEntity() );
}

FUUri FCDEmitter::GetForceFCDSceneNodeUri( size_t index )
{
	FUAssert( index < m_forceFCDSceneNodeRefs.size(), return NULL );
	return m_forceFCDSceneNodeRefs[index]->GetUri();
}
