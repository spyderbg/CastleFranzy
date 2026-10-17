/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _FCD_EMITTER_H_
#define _FCD_EMITTER_H_

/**
	@file FCDEmitter.h
	This file defines the entity class for emitters in FCollada.
*/

/**	
	@defgroup FCDEmitters COLLADA Particle Emitter Classes
*/

class FCDocument;
class FCDExtra;
class FCDEmitterParticle;
class FCDSceneNode;
class FUUri;
class INode;

#ifndef _FCD_ENTITY_H_
#include "FCDocument/FCDEntity.h"
#endif // _FCD_ENTITY_H_
#ifndef _FCD_PARAMETER_ANIMATABLE_H_
#include "FCDocument/FCDParameterAnimatable.h"
#endif // _FCD_PARAMETER_ANIMATABLE_H_

/**
	A COLLADA generic emitter.

	This class does not belong to the COLLADA COMMON profile.
	It is used to define both particle and sound emitters.

	@ingroup FCDEmitters
*/

struct ExportedMaterial;

typedef fm::pvector<INode> NodeList;
typedef fm::pvector<FCDEntityReference> FCDSceneNodeRefList;

namespace FSEmitter
{
	enum Type
	{
		PCLOUD
	};
}

struct EmitterMaterial
{
public:
	ExportedMaterial *m_exportedMaterial;
	fstring m_semantic;

public:
	EmitterMaterial()
		: m_exportedMaterial( NULL )
	{}

	EmitterMaterial( ExportedMaterial *exportedMaterial, fstring semantic )
		: m_exportedMaterial( exportedMaterial )
		, m_semantic( semantic )
	{}
};

class FCOLLADA_EXPORT FCDEmitter : public FCDEntity
{
private:
	DeclareObjectType(FCDEntity);

	fm::vector<EmitterMaterial> m_emitterMaterials;
	NodeList m_forceMaxNodes;
	FCDSceneNodeRefList m_forceFCDSceneNodeRefs;

	// emitter
	DeclareParameter(FSEmitter::Type, FUParameterQualifiers::SIMPLE, m_emitterType, FC("Emitter Type"));

	// particle formation
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_shape, FC("Shape"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_shapeLength, FC("Shape Length"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_shapeWidth, FC("Shape Width"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_shapeHeight, FC("Shape Height"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_shapeRadius, FC("Shape Radius"));
	DeclareParameterRef(FCDEntityReference, m_pShapeTarget, FC("Shape Target"));

	// particle quantity
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleBirthRate, FC("Particle Birth Rate"));

	// particle motion
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSpeed, FC("Particle Speed"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSpeedVar, FC("Particle Speed Variance"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleDirectionMethod, FC("Particle Direction Method"));
	DeclareParameterAnimatable(FMVector3, FUParameterQualifiers::VECTOR, m_particleDirection, FC("Particle Direction"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleDirectionVar, FC("Particle Direction Variance"));
	DeclareParameterRef(FCDEntityReference, m_pParticleMotionTarget, FC("Particle Motion Target"));
	void *m_pParticleMotionTargetNode;

	// particle timing
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_maxParticles, FC("Max Particles"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleLife, FC("Particle Life"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleLifeVar, FC("Particle Life Variance"));

	// particle size
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSize, FC("Particle Size"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSizeVar, FC("Particle Size Variance"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSizeGrowFor, FC("Particle Size Grow For"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSizeFadeFor, FC("Particle Size Fade For"));

	// uniqueness
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_seed, FC("Seed"));

	// particle type
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleType, FC("Particle Type"));

	// standard particle type
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_stdParticleType, FC("Standard Particle Type"));

	// instanced geometry particle type
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_instancedGeometryUseSubtree, FC("Use Subtree"));
	DeclareParameterRef(FCDEntityReference, m_pInstancedGeometryTarget, FC("Instanced Geometry Target"));
	void *m_pInstancedGeometryTargetNode;

	// particle animation offset
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_animationOffsetMethod, FC("Animation Offset Method"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_animationOffsetAmount, FC("Animation Offset Amount"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_animationOffsetVariance, FC("Animation Offset Variance"));

	// particle animation offset
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_animationScalingMethod, FC("Animation Scaling Method"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_animationScalingSrcLength, FC("Animation Scaling Src Length"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_animationScalingSrcVariance, FC("Animation Scaling Src Variance"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_animationScalingDstMultiplier, FC("Animation Scaling Dst Multiplier"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_animationScalingDstVariance, FC("Animation Scaling Dst Variance"));

	// particle rotation
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSpinTime, FC("Particle Spin Time"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSpinTimeVar, FC("Particle Spin Time Variance"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSpinPhase, FC("Particle Spin Phase"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSpinPhaseVar, FC("Particle Spin Phase Variation"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSpinAxisType, FC("Particle Spin Axis Type"));
	DeclareParameterAnimatable(FMVector3, FUParameterQualifiers::VECTOR, m_particleSpinAxis, FC("Particle Spin Axis"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleSpinAxisVar, FC("Particle Spin Axis Variance"));
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_particleStretch, FC("Particle Stretch"));

public:
	/** Constructor.  Do not use directly, emitters should be created using
		the AddEntity function on the FCDEmitterLibrary
		@param document The COLLADA document that owns the emitter. */
	FCDEmitter(FCDocument* document);

	/** Destructor. */
	~FCDEmitter();

	/** Retrieves the entity class type.
		@return The entity class type: EMITTER */
	virtual Type GetType() const { return EMITTER; }

	/** Clones the emitter information.
		@param clone The cloned emitter.
			If this pointer is NULL, a new emitter is created and
			you will need to release it manually.
		@param cloneChildren Whether to recursively clone this entity's children.
		@return The clone. */
	virtual FCDEntity* Clone(FCDEntity* clone = NULL, bool cloneChildren = false) const;

	inline virtual FSEmitter::Type GetEmitterType(){ return m_emitterType; }
	inline virtual void SetEmitterType( FSEmitter::Type emitterType ){ m_emitterType = emitterType; }

	virtual FCDSceneNode *GetShapeTarget();
	virtual void SetShapeTarget( FCDSceneNode *pShapeTarget );
	virtual FUUri GetShapeTargetUri();

	// motion target node funcs
	void SetParticleMotionTargetNode( void *pMaxNode );
	void *GetParticleMotionTargetNode();

	virtual FCDSceneNode *GetParticleMotionTarget();
	virtual void SetParticleMotionTarget( FCDSceneNode *pParticleMotionTarget );
	virtual FUUri GetParticleMotionTargetUri();

	// instanced geometry target node funcs
	void SetInstancedGeometryTargetNode( void *pMaxNode );
	void *GetInstancedGeometryTargetNode();

	virtual FCDSceneNode *GetInstancedGeometryTarget();
	virtual void SetInstancedGeometryTarget( FCDSceneNode *pInstancedGeometryTarget );
	virtual FUUri GetInstancedGeometryTargetUri();

	inline size_t GetEmitterMaterialCount() { return m_emitterMaterials.size(); }
	inline EmitterMaterial &GetEmitterMaterial( size_t index ) { return m_emitterMaterials[index]; }
	inline void AddEmitterMaterial( EmitterMaterial emitterMaterial ) { return m_emitterMaterials.push_back( emitterMaterial ); }

	void AddForceMaxNode( INode *pNode );
	size_t GetNumForceMaxNodes();
	INode *GetForceMaxNode( size_t index );

	size_t GetNumForceFCDSceneNodes();
	void AddForceFCDSceneNode( FCDSceneNode *pForceFCDSceneNode );
	FCDSceneNode *GetForceFCDSceneNode( size_t index );
	FUUri GetForceFCDSceneNodeUri( size_t index );

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Shape, m_shape )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ShapeLength, m_shapeLength )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ShapeWidth, m_shapeWidth )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ShapeHeight, m_shapeHeight )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ShapeRadius, m_shapeRadius )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleBirthRate, m_particleBirthRate )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSpeed, m_particleSpeed )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSpeedVar, m_particleSpeedVar )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleDirectionMethod, m_particleDirectionMethod )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableVector3, FMVector3, ParticleDirection, m_particleDirection )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleDirectionVar, m_particleDirectionVar )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, MaxParticles, m_maxParticles )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleLife, m_particleLife )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleLifeVar, m_particleLifeVar )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSize, m_particleSize )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSizeVar, m_particleSizeVar )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSizeGrowFor, m_particleSizeGrowFor )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSizeFadeFor, m_particleSizeFadeFor )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Seed, m_seed )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleType, m_particleType )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, StdParticleType, m_stdParticleType )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, InstancedGeometryUseSubtree, m_instancedGeometryUseSubtree )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, AnimationOffsetMethod, m_animationOffsetMethod )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, AnimationOffsetAmount, m_animationOffsetAmount )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, AnimationOffsetVariance, m_animationOffsetVariance )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, AnimationScalingMethod, m_animationScalingMethod )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, AnimationScalingSrcLength, m_animationScalingSrcLength )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, AnimationScalingSrcVariance, m_animationScalingSrcVariance )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, AnimationScalingDstMultiplier, m_animationScalingDstMultiplier )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, AnimationScalingDstVariance, m_animationScalingDstVariance )

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSpinTime, m_particleSpinTime )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSpinTimeVar, m_particleSpinTimeVar )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSpinPhase, m_particleSpinPhase )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSpinPhaseVar, m_particleSpinPhaseVar )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSpinAxisType, m_particleSpinAxisType )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableVector3, FMVector3, ParticleSpinAxis, m_particleSpinAxis )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleSpinAxisVar, m_particleSpinAxisVar )
	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, ParticleStretch, m_particleStretch )
};

#endif //_FCD_EMITTER_H
