// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_FPS_PARTICLE_SYSTEM_SCENE_NODE_H_INCLUDED__
#define __C_FPS_PARTICLE_SYSTEM_SCENE_NODE_H_INCLUDED__

#include "IFpsParticleSystemSceneNode.h"
#include "irrArray.h"
#include "irrList.h"
#include "SMeshBuffer.h"

namespace irr
{

using namespace scene;

namespace scene
{
	class ICameraSceneNode;
};

//! A particle system scene node.
/** A scene node controlling a particle system. The behavior of the particles
can be controlled by setting the right particle emitters and affectors.
*/
class CFpsParticleSystemSceneNode : public IFpsParticleSystemSceneNode
{
public:

	//! constructor
	CFpsParticleSystemSceneNode(bool createDefaultEmitter, s32 id, 
		const core::vector3df& position,
		const core::vector3df& rotation,
		const core::vector3df& scale);

	//! destructor
	virtual ~CFpsParticleSystemSceneNode();

	//! Gets the particle emitter, which creates the particles.
	virtual IFpsParticleEmitter* getEmitter();

	//! Sets the particle emitter, which creates the particles.
	virtual void setEmitter(IFpsParticleEmitter* emitter);

	//! Adds new particle affector to the particle system.
	virtual void addAffector(IFpsParticleAffector* affector);

	//! Removes particle affector from particle system.
	virtual void removeAffector(IFpsParticleAffector* affector);

	//! Gets the particle affectors
	core::list<IFpsParticleAffector*> getAffectors()	{return AffectorList;}

	//! Removes all particle affectors in the particle system.
	virtual void removeAllAffectors();

	//! Returns the material based on the zero based index i.
	virtual video::SMaterial& getMaterial(u32 i);

	//! Returns amount of materials used by this scene node.
	virtual u32 getMaterialCount() const;

	//! pre render event
	virtual void OnRegisterSceneNode();
	virtual void OnAnimate(u32 timeMs);

	//! render
	virtual void render(void*);

	//! returns the axis aligned bounding box of this node
	virtual const core::aabbox3d<f32>& getBoundingBox() const;

	//! Creates a box particle emitter.
	virtual IFpsParticleBoxEmitter* createBoxEmitter(
		const core::aabbox3df& box = core::aabbox3d<f32>(-10,0,-10,5,30,10),
		const core::vector3df& direction = core::vector3df(0.0f,0.03f,0.0f), 
		u32 speedVariation = 0,
		u32 minParticlesPerSecond = 5,
		u32 maxParticlesPerSecond = 10,
		f32 particleWidth = 5.f, f32 particleHeight = 5.f, s32 sizeVariation = 0,
		const video::SColor& minStartColor = video::SColor(255,0,0,0),
		const video::SColor& maxStartColor = video::SColor(255,255,255,255),
		u32 lifeTimeMin=2000, u32 lifeTimeMax=4000,
		s32 maxAngleDegreesXY=0, s32 maxAngleDegreesYZ=0, s32 maxAngleDegreesXZ=0,
		s32 initialRotMin = 0, s32 initialRotMax = 0);

	//! Creates a point attraction affector. This affector modifies the positions of the
	//! particles and attracts them to a specified point at a specified speed per second.
	virtual IFpsParticleAttractionAffector* createAttractionAffector(
		const core::vector3df& point = core::vector3df(0.0f,0.0f,0.0f), f32 speed = 1.0f, bool attract = true,
		bool affectX = true, bool affectY = true, bool affectZ = true);

	//! Creates a fade out particle affector.
	virtual IFpsParticleFadeOutAffector* createFadeOutParticleAffector(
		const video::SColor& targetColor = video::SColor(0,0,0,0),
		f32 startTimePercent = 0.f,
		f32 endTimePercent = 1.f);

	//! Creates a gravity affector.
	virtual IFpsParticleGravityAffector* createGravityAffector(
		const core::vector3df& gravity = core::vector3df(0.0f,0.0f,-0.03f),
		f32 startTimePercent = 0.f, f32 endTimePercent = 1.f);

	//! Creates a rotation affector. This affector rotates the particles
	//! around a specified pivot point. The speed is in Degrees per second.
	virtual IFpsParticleRotationAffector* createRotationAffector(
		const core::vector3df& speed = core::vector3df(5.0f,5.0f,5.0f),
		const core::vector3df& pivotPoint = core::vector3df(0.0f,0.0f,0.0f) );

	//! Creates a size affector. This affector changes the size of the particles
	//! from 0 to the target size during the amount of grow for time
	//! and from target to 0 during the amount of fade for time
	virtual IFpsParticleSizeAffector* createSizeAffector(
		f32 targetWidth = 1.0f,
		f32 targetHeight = 1.0f,
		u32 variation = 0,
		f32 startTimePercent = 0.f,
		f32 endTimePercent = 1.f);

	//! Creates a spin affector. This affector spins a particule performing a full
	//! spin during the amount of time
	virtual IFpsParticleSpinAffector* createSpinAffector(
		s32 minSpin = 0, s32 maxSpin = 360, f32 startTimePercent = 0.f, f32 endTimePercent = 1.f);

	//! Sets if the particles should be global. If they are, the particles are affected by
	//! the movement of the particle system scene node too, otherwise they completely
	//! ignore it. Default is true.
	virtual void setParticlesAreGlobal(bool global=true);

	//! Sets if the particles should change rotation in function of direction.
	virtual void setDirectionalRotation(bool bDirectionalRotation=true) {DirectionalRotation = bDirectionalRotation;}

	//! Sets if the particles direction is projected to camera view when we calculate directional rotation.
	virtual void setProjectDirection(bool bProjectDirection=true) {ProjectDirection = bProjectDirection;}

	//! Sets particle system min life time
	virtual void setSysMinLifeTime(s32 minLifeTime=-1) {SysMinLifeTime = minLifeTime;}

	//! Sets particle system max life time
	virtual void setSysMaxLifeTime(s32 maxLifeTime=-1) {SysMaxLifeTime = maxLifeTime;}

	//! Sets particle system min restart time
	virtual void setRestartTimeMin(s32 minRestartTime=-1) {RestartTimeMin = minRestartTime;}

	//! Sets particle system max restart time
	virtual void setRestartTimeMax(s32 maxRestartTime=-1) {RestartTimeMax = maxRestartTime;}

	//! Sets particle system start delay
	virtual void setStartDelay(s32 delay=0) {StartDelay = delay;}

	//! Writes attributes of the scene node.
	virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

	//! Reads attributes of the scene node.
	virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	//! Returns type of the scene node
	virtual ESCENE_NODE_TYPE getType() const { return ESNT_PARTICLE_SYSTEM; }

	//! Creates a clone of this scene node and its children.
	virtual ISceneNode* clone();

	//! Returns the number of particles in use
	virtual u32 getNumberOfParticles() const { return Particles.size(); }

	//! Sets the type of billboard in use
	virtual void setBillboardType(E_FPS_PARTICLE_BILLBOARD_TYPE billboardType);

	//! Sets global transparency of the node
	virtual void setTransparency(f32 transparency);

	//! Returns true if particle system does not emit particles anymore
	virtual bool IsEnded();

	//! Restarts the particle system after is ended
	virtual void Restart();

	//! Pause the particle system
	virtual void Pause();

	//! Unpause the particle system
	virtual void UnPause();

	virtual void SetSystemScale (float scale);
	virtual void SetSystemRotation (const core::quaternion& rotation);

private:
	float SystemScale;
	core::quaternion SystemRotation;

	void doParticleSystem(u32 time);
	void reallocateBuffers();

	void SetRandomLifeTime();

	bool project3DTo2D (core::vector3df pt, core::vector2df* result, ICameraSceneNode* camera = 0);

	core::list<IFpsParticleAffector*> AffectorList;
	IFpsParticleEmitter* Emitter;
	core::array<SFpsParticle> Particles;
	u32 LastEmitTime;
	s32 MaxParticles;

	bool DirectionalRotation;
	bool ProjectDirection;
	s32 SysMinLifeTime;
	s32 SysMaxLifeTime;
	s32 RestartTimeMin;
	s32 RestartTimeMax;
	s32 StartDelay;

	u32 TotalEmitTime;
	u32 RandomSysLifeTime;
	u32 RandomRestartTime;
	u32 TotalRestartTime;
	s32 TotalDelay;

	bool m_bPaused;

	SMeshBuffer* Buffer;

	enum E_PARTICLES_PRIMITIVE
	{
		EPP_POINT=0,
		EPP_BILLBOARD,
		EPP_POINTSPRITE
	};
	E_PARTICLES_PRIMITIVE ParticlePrimitive;
	E_FPS_PARTICLE_BILLBOARD_TYPE BillboardType;

	bool ParticlesAreGlobal;
	f32 Transparency;
};

}
#endif

