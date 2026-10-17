// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_FPS_PARTICLE_SYSTEM_SCENE_NODE_H_INCLUDED__
#define __I_FPS_PARTICLE_SYSTEM_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "IFpsParticleBoxEmitter.h"
#include "IFpsParticleAttractionAffector.h"
#include "IFpsParticleFadeOutAffector.h"
#include "IFpsParticleGravityAffector.h"
#include "IFpsParticleRotationAffector.h"
#include "IFpsParticleSizeAffector.h"
#include "IFpsParticleSpinAffector.h"
#include "dimension2d.h"

namespace irr
{

//! A particle system scene node for creating snow, fire, exlosions, smoke...
/** A scene node controlling a particle System. The behavior of the particles
can be controlled by setting the right particle emitters and affectors.
You can for example easily create a campfire by doing this:

\code
	scene::IParticleSystemSceneNode* p = scenemgr->addParticleSystemSceneNode();
	p->setParticleSize(core::dimension2d<f32>(20.0f, 10.0f));
	scene::IParticleEmitter* em = p->createBoxEmitter(
		core::aabbox3d<f32>(-5,0,-5,5,1,5),
		core::vector3df(0.0f,0.03f,0.0f),
		40,80, video::SColor(0,255,255,255),video::SColor(0,255,255,255), 1100,2000);
	p->setEmitter(em);
	em->drop();
	scene::IParticleAffector* paf = p->createFadeOutParticleAffector();
	p->addAffector(paf);
	paf->drop();
\endcode

*/
class IFpsParticleSystemSceneNode : public irr::scene::ISceneNode
{
public:

	//! Constructor
	IFpsParticleSystemSceneNode(s32 id,
		const core::vector3df& position = core::vector3df(0,0,0),
		const core::vector3df& rotation = core::vector3df(0,0,0),
		const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f))
		: irr::scene::ISceneNode(id, position, rotation, scale) {}

	//! Sets if the particles should be global.
	/** If they are, the particles are affected by the movement of the
	particle system scene node too, otherwise they completely ignore it.
	Default is true. */
	virtual void setParticlesAreGlobal(bool global=true) = 0;

	//! Sets if the particles should change rotation in function of direction.
	virtual void setDirectionalRotation(bool bDirectionalRotation=true) = 0;

	//! Sets if the particles direction is projected to camera view when we calculate directional rotation.
	virtual void setProjectDirection(bool bProjectDirection=true) = 0;

	//! Gets the particle emitter, which creates the particles.
	/** \return The particle emitter. Can be 0 if none is set. */
	virtual IFpsParticleEmitter* getEmitter() =0;

	//! Sets the particle emitter, which creates the particles.
	/** A particle emitter can be created using one of the createEmitter
	methods. For example to create and use a simple PointEmitter, call
	IParticleEmitter* p = createPointEmitter(); setEmitter(p); p->drop();
	\param emitter: Sets the particle emitter. You can set this to 0 for
	removing the current emitter and stopping the particle system emitting
	new particles. */
	virtual void setEmitter(IFpsParticleEmitter* emitter) = 0;

	//! Adds new particle effector to the particle system.
	/** A particle affector modifies the particles. For example, the FadeOut
	affector lets all particles fade out after some time. It is created and
	used in this way:
	\code
	IParticleAffector* p = createFadeOutParticleAffector();
	addAffector(p);
	p->drop();
	\endcode
	Please note that an affector is not necessary for the particle system to
	work.
	\param affector: New affector. */
	virtual void addAffector(IFpsParticleAffector* affector) = 0;

	//! Removes particle affector from particle system.
	virtual void removeAffector(IFpsParticleAffector* affector) = 0;

	//! Gets all particle affectors in the particle system.
	virtual core::list<IFpsParticleAffector*> getAffectors() = 0;

	//! Removes all particle affectors in the particle system.
	virtual void removeAllAffectors() = 0;

	//! Creates a box particle emitter.
	/** \param box: The box for the emitter.
	\param direction: Direction and speed of particle emission.
	\param minParticlesPerSecond: Minimal amount of particles emitted per
	second.
	\param maxParticlesPerSecond: Maximal amount of particles emitted per
	second.
	\param minStartColor: Minimal initial start color of a particle. The
	real color of every particle is calculated as random interpolation
	between minStartColor and maxStartColor.
	\param maxStartColor: Maximal initial start color of a particle. The
	real color of every particle is calculated as random interpolation
	between minStartColor and maxStartColor.
	\param lifeTimeMin: Minimal lifetime of a particle, in milliseconds.
	\param lifeTimeMax: Maximal lifetime of a particle, in milliseconds.
	\param maxAngleDegrees: Maximal angle in degrees, the emitting
	direction of the particle will differ from the original direction.
	\return Pointer to the created particle emitter. To set this emitter
	as new emitter of this particle system, just call setEmitter(). Note
	that you'll have to drop() the returned pointer, after you don't need
	it any more, see IReferenceCounted::drop() for more informations. */
	virtual IFpsParticleBoxEmitter* createBoxEmitter(
		const core::aabbox3df& box = core::aabbox3df(-10,28,-10,10,30,10),
		const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f),
		u32 speedVariation = 0,
		u32 minParticlesPerSecond = 5,
		u32 maxParticlesPerSecond = 10,
		f32 particleWidth = 5.f, f32 particleHeight = 5.f, s32 sizeVariation = 0,
		const video::SColor& minStartColor = video::SColor(255,0,0,0),
		const video::SColor& maxStartColor = video::SColor(255,255,255,255),
		u32 lifeTimeMin=2000, u32 lifeTimeMax=4000,
		s32 maxAngleDegreesXY=0, s32 maxAngleDegreesYZ=0, s32 maxAngleDegreesXZ=0,
		s32 initialRotMin = 0, s32 initialRotMax = 0) = 0;

	//! Creates a point attraction affector.
	/** This affector modifies the positions of the particles and attracts
	them to a specified point at a specified speed per second.
	\param point: Point to attract particles to.
	\param speed: Speed in units per second, to attract to the specified
	point.
	\param attract: Whether the particles attract or detract from this
	point.
	\param affectX: Whether or not this will affect the X position of the
	particle.
	\param affectY: Whether or not this will affect the Y position of the
	particle.
	\param affectZ: Whether or not this will affect the Z position of the
	particle.
	\return Pointer to the created particle affector. To add this affector
	as new affector of this particle system, just call addAffector(). Note
	that you'll have to drop() the returned pointer, after you don't need
	it any more, see IReferenceCounted::drop() for more informations. */
	virtual IFpsParticleAttractionAffector* createAttractionAffector(
		const core::vector3df& point, f32 speed = 1.0f, bool attract = true,
		bool affectX = true, bool affectY = true, bool affectZ = true) = 0;

	//! Creates a fade out particle affector.
	/** This affector modifies the color of every particle and and reaches
	the final color when the particle dies. This affector looks really
	good, if the EMT_TRANSPARENT_VERTEX_ALPHA material is used and the
	targetColor is video::SColor(0,0,0,0): Particles are fading out into
	void with this setting.
	\param targetColor: Color whereto the color of the particle is changed.
	\param timeNeededToFadeOut: How much time in milli seconds should the
	affector need to change the color to the targetColor.
	\return Pointer to the created particle affector. To add this affector
	as new affector of this particle system, just call addAffector(). Note
	that you'll have to drop() the returned pointer, after you don't need
	it any more, see IReferenceCounted::drop() for more informations. */
	virtual IFpsParticleFadeOutAffector* createFadeOutParticleAffector(
		const video::SColor& targetColor = video::SColor(0,0,0,0),
		f32 startTimePercent = 0.f,
		f32 endTimePercent = 1.f) = 0;

	//! Creates a gravity affector.
	/** This affector modifies the direction of the particle. It assumes
	that the particle is fired out of the emitter with huge force, but is
	loosing this after some time and is catched by the gravity then. This
	affector is ideal for creating things like fountains.
	\param gravity: Direction and force of gravity.
	\param timeForceLost: Time in milli seconds when the force of the
	emitter is totally lost and the particle does not move any more. This
	is the time where gravity fully affects the particle.
	\return Pointer to the created particle affector. To add this affector
	as new affector of this particle system, just call addAffector(). Note
	that you'll have to drop() the returned pointer, after you don't need
	it any more, see IReferenceCounted::drop() for more informations. */
	virtual IFpsParticleGravityAffector* createGravityAffector(
		const core::vector3df& gravity = core::vector3df(0.0f,0.0f,-0.03f),
		f32 startTimePercent = 0.f, f32 endTimePercent = 1.f) = 0;

	//! Creates a rotation affector.
	/** This affector modifies the positions of the particles and attracts
	them to a specified point at a specified speed per second.
	\param speed: Rotation in degrees per second
	\param pivotPoint: Point to rotate the particles around
	\return Pointer to the created particle affector. To add this affector
	as new affector of this particle system, just call addAffector(). Note
	that you'll have to drop() the returned pointer, after you don't need
	it any more, see IReferenceCounted::drop() for more informations. */
	virtual IFpsParticleRotationAffector* createRotationAffector(
		const core::vector3df& speed = core::vector3df(5.0f,5.0f,5.0f),
		const core::vector3df& pivotPoint = core::vector3df(0.0f,0.0f,0.0f) ) = 0;

	//! Creates a size affector. 
	/** This affector changes the size of the particles
	from 0 to the target size during the amount of time to grow
	and from the target size to 0 during the amount of time to fade
	\param targetSize: Target size
	\param variation: Variation of the size in percentage
	\param growForTime: Time needed to grow
	\param fadeForTime: Time needed to fade
	\return Pointer to the created particle affector. To add this affector
	as new affector of this particle system, just call addAffector(). Note
	that you'll have to drop() the returned pointer, after you don't need
	it any more, see IReferenceCounted::drop() for more informations. */
	virtual IFpsParticleSizeAffector* createSizeAffector(
		f32 targetWidth = 1.0f,
		f32 targetHeight = 1.0f,
		u32 variation = 0,
		f32 startTimePercent = 0.f,
		f32 endTimePercent = 1.f) = 0;

	//! Creates a spin affector. 
	/** This affector spins the particles doing a full spin in a determined
	amount of time
	\param spinTime: Time needed to a full spin
	\param variation: Variation of the size in percentage
	\return Pointer to the created particle affector. To add this affector
	as new affector of this particle system, just call addAffector(). Note
	that you'll have to drop() the returned pointer, after you don't need
	it any more, see IReferenceCounted::drop() for more informations. */
	virtual IFpsParticleSpinAffector* createSpinAffector(
		s32 minSpin = 0, s32 maxSpin = 360, f32 startTimePercent = 0.f, f32 endTimePercent = 1.f) = 0;

	//! Returns the number of particles in use
	virtual u32 getNumberOfParticles() const = 0;

	//! Sets the type of billboard in use
	virtual void setBillboardType(E_FPS_PARTICLE_BILLBOARD_TYPE billboardType) = 0;

	//! Sets global transparency of the node
	virtual void setTransparency(f32 transparency) = 0;

	//! Returns true if particle system does not emit particles anymore
	virtual bool IsEnded() = 0;

	//! Restarts the particle system after is ended
	virtual void Restart() = 0;

	//! Pause the particle system
	virtual void Pause() = 0;

	//! Unpause the particle system
	virtual void UnPause () = 0;

	virtual void SetSystemScale (float scale) = 0;
	virtual void SetSystemRotation (const core::quaternion& rotation) = 0;
};

}
#endif

