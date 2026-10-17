#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CParticleSystemSceneNode.h"
#include "irros.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"
#include "IVideoDriver.h"

#include "CParticleAnimatedMeshSceneNodeEmitter.h"
#include "CParticleBoxEmitter.h"
#include "CParticleCylinderEmitter.h"
#include "CParticleMeshEmitter.h"
#include "CParticlePointEmitter.h"
#include "CParticleRingEmitter.h"
#include "CParticleSphereEmitter.h"
#include "CParticleAttractionAffector.h"
#include "CParticleFadeOutAffector.h"
#include "CParticleGravityAffector.h"
#include "CParticleRotationAffector.h"
#include "CParticleSizeAffector.h"
#include "CParticleSpinAffector.h"
#include "SViewFrustum.h"

#define PARTICLES_GROWTH_RATE 64

namespace irr
{
namespace scene
{

//! constructor
CParticleSystemSceneNode::CParticleSystemSceneNode(bool createDefaultEmitter,
	s32 id,
	const core::vector3df& position, const core::vector3df& rotation,
	const core::vector3df& scale)
	: IParticleSystemSceneNode(id, position, rotation, scale),
	Emitter(0), LastEmitTime(0), MaxParticles(0xffff), Buffer(0),
	ParticlesAreGlobal(true), BillboardType(EPBT_CAMERA_FACING),
	Transparency(1.0)
{
	#ifdef _DEBUG
	setDebugName("CParticleSystemSceneNode");
	#endif

	Buffer = irrnew SMeshBuffer();
	if (createDefaultEmitter)
	{
		IParticleEmitter* e = createBoxEmitter();
		setEmitter(e);
		e->drop();
	}

	// reduce copies when the buffers grow
	Particles.set_alloc_chunk_count( PARTICLES_GROWTH_RATE );
	Buffer->Vertices.set_alloc_chunk_count( PARTICLES_GROWTH_RATE * 4 );
	Buffer->Indices.set_alloc_chunk_count( PARTICLES_GROWTH_RATE * 4 );	

	setParticleSize();
}


//! destructor
CParticleSystemSceneNode::~CParticleSystemSceneNode()
{
	if (Emitter)
		Emitter->drop();
	if (Buffer)
		Buffer->drop();

	removeAllAffectors();
}


//! Gets the particle emitter, which creates the particles.
IParticleEmitter* CParticleSystemSceneNode::getEmitter()
{
	return Emitter;
}


//! Sets the particle emitter, which creates the particles.
void CParticleSystemSceneNode::setEmitter(IParticleEmitter* emitter)
{
	if (Emitter)
		Emitter->drop();

	Emitter = emitter;

	if (Emitter)
		Emitter->grab();
}


//! Adds new particle effector to the particle system.
void CParticleSystemSceneNode::addAffector(IParticleAffector* affector)
{
	affector->grab();
	AffectorList.push_back(affector);
}


//! Removes all particle affectors in the particle system.
void CParticleSystemSceneNode::removeAllAffectors()
{
	core::list<IParticleAffector*>::Iterator it = AffectorList.begin();
	while (it != AffectorList.end())
	{
		(*it)->drop();
		it = AffectorList.erase(it);
	}
}


//! Returns the material based on the zero based index i.
video::SMaterial& CParticleSystemSceneNode::getMaterial(u32 i)
{
	return Buffer->Material;
}


//! Returns amount of materials used by this scene node.
u32 CParticleSystemSceneNode::getMaterialCount() const
{
	return 1;
}


//! Creates a particle emitter for an animated mesh scene node
IParticleAnimatedMeshSceneNodeEmitter*
CParticleSystemSceneNode::createAnimatedMeshSceneNodeEmitter(
	scene::IAnimatedMeshSceneNode* node, bool useNormalDirection,
	const core::vector3df& direction, f32 normalDirectionModifier,
	s32 mbNumber, bool everyMeshVertex,
	u32 minParticlesPerSecond, u32 maxParticlesPerSecond,
	const video::SColor& minStartColor, const video::SColor& maxStartColor,
	u32 lifeTimeMin, u32 lifeTimeMax, s32 maxAngleDegrees )
{
	return irrnew CParticleAnimatedMeshSceneNodeEmitter( node,
			useNormalDirection, direction, normalDirectionModifier,
			mbNumber, everyMeshVertex,
			minParticlesPerSecond, maxParticlesPerSecond,
			minStartColor, maxStartColor,
			lifeTimeMin, lifeTimeMax, maxAngleDegrees );
}


//! Creates a box particle emitter.
IParticleBoxEmitter* CParticleSystemSceneNode::createBoxEmitter(
	const core::aabbox3df& box, const core::vector3df& direction,
	u32 minParticlesPerSecond, u32 maxParticlesPerSecond,
	const video::SColor& minStartColor, const video::SColor& maxStartColor,
	u32 lifeTimeMin, u32 lifeTimeMax,
	s32 maxAngleDegrees)
{
	return irrnew CParticleBoxEmitter(box, direction, minParticlesPerSecond,
		maxParticlesPerSecond, minStartColor, maxStartColor,
		lifeTimeMin, lifeTimeMax, maxAngleDegrees);
}


//! Creates a particle emitter for emitting from a cylinder
IParticleCylinderEmitter* CParticleSystemSceneNode::createCylinderEmitter(
	const core::vector3df& center, f32 radius,
	const core::vector3df& normal, f32 length,
	bool outlineOnly, const core::vector3df& direction,
	u32 minParticlesPerSecond, u32 maxParticlesPerSecond,
	const video::SColor& minStartColor, const video::SColor& maxStartColor,
	u32 lifeTimeMin, u32 lifeTimeMax, s32 maxAngleDegrees )
{
	return irrnew CParticleCylinderEmitter( center, radius, normal, length,
			outlineOnly, direction,
			minParticlesPerSecond, maxParticlesPerSecond,
			minStartColor, maxStartColor,
			lifeTimeMin, lifeTimeMax, maxAngleDegrees );
}


//! Creates a mesh particle emitter.
IParticleMeshEmitter* CParticleSystemSceneNode::createMeshEmitter(
	scene::IMesh* mesh, bool useNormalDirection,
	const core::vector3df& direction, f32 normalDirectionModifier,
	s32 mbNumber, bool everyMeshVertex,
	u32 minParticlesPerSecond, u32 maxParticlesPerSecond,
	const video::SColor& minStartColor, const video::SColor& maxStartColor,
	u32 lifeTimeMin, u32 lifeTimeMax, s32 maxAngleDegrees )
{
	return irrnew CParticleMeshEmitter( mesh, useNormalDirection, direction,
			normalDirectionModifier, mbNumber, everyMeshVertex,
			minParticlesPerSecond, maxParticlesPerSecond,
			minStartColor, maxStartColor,
			lifeTimeMin, lifeTimeMax, maxAngleDegrees );
}


//! Creates a point particle emitter.
IParticlePointEmitter* CParticleSystemSceneNode::createPointEmitter(
	const core::vector3df& direction, u32 minParticlesPerSecond,
	u32 maxParticlesPerSecond, const video::SColor& minStartColor,
	const video::SColor& maxStartColor, u32 lifeTimeMin, u32 lifeTimeMax,
	s32 maxAngleDegrees)
{
	return irrnew CParticlePointEmitter(direction, minParticlesPerSecond,
		maxParticlesPerSecond, minStartColor, maxStartColor,
		lifeTimeMin, lifeTimeMax, maxAngleDegrees);
}


//! Creates a ring particle emitter.
IParticleRingEmitter* CParticleSystemSceneNode::createRingEmitter(
	const core::vector3df& center, f32 radius, f32 ringThickness,
	const core::vector3df& direction,
	u32 minParticlesPerSecond, u32 maxParticlesPerSecond,
	const video::SColor& minStartColor, const video::SColor& maxStartColor,
	u32 lifeTimeMin, u32 lifeTimeMax, s32 maxAngleDegrees )
{
	return irrnew CParticleRingEmitter( center, radius, ringThickness, direction,
		minParticlesPerSecond, maxParticlesPerSecond, minStartColor,
		maxStartColor, lifeTimeMin, lifeTimeMax, maxAngleDegrees );
}


//! Creates a sphere particle emitter.
IParticleSphereEmitter* CParticleSystemSceneNode::createSphereEmitter(
	const core::vector3df& center, f32 radius, const core::vector3df& direction,
	u32 minParticlesPerSecond, u32 maxParticlesPerSecond,
	const video::SColor& minStartColor, const video::SColor& maxStartColor,
	u32 lifeTimeMin, u32 lifeTimeMax,
	s32 maxAngleDegrees)
{
	return irrnew CParticleSphereEmitter(center, radius, direction,
			minParticlesPerSecond, maxParticlesPerSecond,
			minStartColor, maxStartColor,
			lifeTimeMin, lifeTimeMax, maxAngleDegrees);
}


//! Creates a point attraction affector. This affector modifies the positions of the
//! particles and attracts them to a specified point at a specified speed per second.
IParticleAttractionAffector* CParticleSystemSceneNode::createAttractionAffector(
	const core::vector3df& point, f32 speed, bool attract,
	bool affectX, bool affectY, bool affectZ )
{
	return irrnew CParticleAttractionAffector( point, speed, attract, affectX, affectY, affectZ );
}


//! Creates a fade out particle affector.
IParticleFadeOutAffector* CParticleSystemSceneNode::createFadeOutParticleAffector(
		const video::SColor& targetColor, u32 timeNeededToFadeOut)
{
	return irrnew CParticleFadeOutAffector(targetColor, timeNeededToFadeOut);
}


//! Creates a gravity affector.
IParticleGravityAffector* CParticleSystemSceneNode::createGravityAffector(
		const core::vector3df& gravity, u32 timeForceLost)
{
	return irrnew CParticleGravityAffector(gravity, timeForceLost);
}


//! Creates a rotation affector. This affector rotates the particles around a specified pivot
//! point.  The speed represents Degrees of rotation per second.
IParticleRotationAffector* CParticleSystemSceneNode::createRotationAffector(
	const core::vector3df& speed, const core::vector3df& pivotPoint )
{
	return irrnew CParticleRotationAffector( speed, pivotPoint );
}

//! Creates a size affector. This affector changes the size of the particles
//! from 0 to the target size during the amount of time
IParticleSizeAffector* CParticleSystemSceneNode::createSizeAffector(
		f32 targetSize, f32 variation, u32 growForTime, u32 fadeForTime)
{
	return irrnew CParticleSizeAffector( targetSize, variation, growForTime, fadeForTime );
}

//! Creates a spin affector. This affector spins a particule performing a full
//! spin during the amount of time
IParticleSpinAffector* CParticleSystemSceneNode::createSpinAffector(
		u32 spinTime, f32 variation)
{
	return irrnew CParticleSpinAffector( spinTime, variation );
}

void CParticleSystemSceneNode::OnAnimate(u32 timeMs)
{ 
	IParticleSystemSceneNode::OnAnimate(timeMs);
	if (!os::Timer::isStopped())
		doParticleSystem(os::Timer::getTime());
}

//! pre render event
void CParticleSystemSceneNode::OnRegisterSceneNode()
{
	if (IsVisible && (Particles.size() != 0))
	{
#ifdef _DEBUG
		if (DebugDataVisible)
		{
			SceneManager->registerNodeForRendering(this, 0, 0);
		}
#endif
		SceneManager->registerNodeForRendering(this,
											   &Buffer->Material,
											   (void*)1);
		ISceneNode::OnRegisterSceneNode();
	}
}


//! Sets global transparency of the node
void CParticleSystemSceneNode::setTransparency(f32 transparency)
{
	Transparency = transparency;
}

//! render
void CParticleSystemSceneNode::render(void* renderData)
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	ICameraSceneNode* camera = SceneManager->getActiveCamera();

	if (!camera || !driver)
		return;

	bool hasSizeAffector = false;
	bool hasSpinAffector = false;

	video::SColor TargetTransparencyColor;
	core::list<IParticleAffector*>::Iterator it = AffectorList.begin();
	for(; it != AffectorList.end(); it++)
	{
		if ((*it)->getType() == scene::EPAT_SIZE)
		{
			hasSizeAffector = true;
		}
		if ((*it)->getType() == scene::EPAT_SPIN)
		{
			hasSpinAffector = true;
		}
		if ((*it)->getType() == scene::EPAT_FADE_OUT)
		{
			CParticleFadeOutAffector* affector = (CParticleFadeOutAffector*) (*it);
			TargetTransparencyColor = affector->getTargetColor();
		}
	}	

	core::vector3df horizontal;
	core::vector3df vertical;
	core::vector3df view;

	if (BillboardType == EPBT_UP_VECTOR_FACING)
	{
		// calculate vectors for letting particles look up vector
		view = core::vector3df(camera->getTarget() - camera->getAbsolutePosition());
		view.normalize();

		horizontal = camera->getUpVector().crossProduct(view);
		horizontal.normalize();
		horizontal *= 0.5f * ParticleSize.Width;

		vertical = camera->getUpVector().crossProduct(horizontal);
		vertical.normalize();
		vertical *= 0.5f * ParticleSize.Height;

		view = camera->getUpVector();
	}
	else
	{
		// calculate vectors for letting particles look to camera
		const core::matrix4 &m = camera->getViewFrustum()->Matrices [ video::ETS_VIEW ];

		f32 f;

		f = 0.5f * ParticleSize.Width;
		horizontal = core::vector3df( m[0] * f, m[4] * f, m[8] * f );

		f = -0.5f * ParticleSize.Height;
		vertical = core::vector3df( m[1] * f, m[5] * f, m[9] * f );

		view = core::vector3df( -m[2], -m[6] , -m[10] );

		view *= -1.0f;
	}

#if 0
	// calculate vectors for letting particles look to camera
	core::vector3df view(camera->getTarget() - camera->getAbsolutePosition());
	view.normalize();

	core::vector3df horizontal = camera->getUpVector().crossProduct(view);
	horizontal.normalize();
	horizontal *= 0.5f * ParticleSize.Width;

	//core::vector3df vertical = horizontal.crossProduct(view);
	core::vector3df vertical = camera->getUpVector().crossProduct(horizontal);
	vertical.normalize();
	vertical *= 0.5f * ParticleSize.Height;

	//view *= -1.0f;

	view = camera->getUpVector();

//#else

	const core::matrix4 &m = camera->getViewFrustum()->Matrices [ video::ETS_VIEW ];

	f32 f;

	f = 0.5f * ParticleSize.Width;
	core::vector3df horizontal ( m[0] * f, m[4] * f, m[8] * f );

	f = -0.5f * ParticleSize.Height;
	core::vector3df vertical ( m[1] * f, m[5] * f, m[9] * f );

	core::vector3df view ( -m[2], -m[6] , -m[10] );

	view *= -1.0f;

#endif

	// reallocate arrays, if they are too small
	reallocateBuffers();

	// create particle vertex data
	s32 idx = 0;
	for (u32 i=0; i<Particles.size(); ++i)
	{
		if (Transparency < 1.0)
		{
			Particles[i].color = Particles[i].color.getInterpolated(TargetTransparencyColor, Transparency);
		}

		const SParticle& particle = Particles[i];

		core::vector3df hor = horizontal;
		core::vector3df ver = vertical;

		if (hasSizeAffector)
		{
			hor = horizontal * particle.size / ParticleSize.Width;
			ver = vertical * particle.size / ParticleSize.Height;
		}

		if (hasSpinAffector)
		{
			core::quaternion quaternion; 
			core::matrix4 matrix; 
			quaternion.fromAngleAxis(particle.spin, view); 
			quaternion.getMatrix(matrix); 
			matrix.rotateVect(hor); 
			matrix.rotateVect(ver); 
		}

#ifndef _IRR_USE_RIGHT_HAND_CONVENTION_
		Buffer->Vertices[0+idx].Pos = particle.pos + hor + ver;
		Buffer->Vertices[0+idx].Color = particle.color;
		Buffer->Vertices[0+idx].Normal = view;

		Buffer->Vertices[1+idx].Pos = particle.pos + hor - ver;
		Buffer->Vertices[1+idx].Color = particle.color;
		Buffer->Vertices[1+idx].Normal = view;

		Buffer->Vertices[2+idx].Pos = particle.pos - hor - ver;
		Buffer->Vertices[2+idx].Color = particle.color;
		Buffer->Vertices[2+idx].Normal = view;

		Buffer->Vertices[3+idx].Pos = particle.pos - hor + ver;
		Buffer->Vertices[3+idx].Color = particle.color;
		Buffer->Vertices[3+idx].Normal = view;
#else
		Buffer->Vertices[3+idx].Pos = particle.pos + hor + ver;
		Buffer->Vertices[3+idx].Color = particle.color;
		Buffer->Vertices[3+idx].Normal = view;

		Buffer->Vertices[2+idx].Pos = particle.pos + hor - ver;
		Buffer->Vertices[2+idx].Color = particle.color;
		Buffer->Vertices[2+idx].Normal = view;

		Buffer->Vertices[1+idx].Pos = particle.pos - hor - ver;
		Buffer->Vertices[1+idx].Color = particle.color;
		Buffer->Vertices[1+idx].Normal = view;

		Buffer->Vertices[0+idx].Pos = particle.pos - hor + ver;
		Buffer->Vertices[0+idx].Color = particle.color;
		Buffer->Vertices[0+idx].Normal = view;
#endif

		idx +=4;
	}

	// render all
	if (renderData)
	{
		core::matrix4 mat;

		if (!ParticlesAreGlobal)
			mat.setTranslation(AbsoluteTransformation.getTranslation());
		driver->setTransform(video::ETS_WORLD, mat);

		driver->setMaterial(Buffer->Material);

		driver->drawVertexPrimitiveList(
			Buffer->getVertices(),
			Buffer->getIndices(),
			0,
			Particles.size() * 4,
			Particles.size() * 2,
			video::EVT_STANDARD,
			Buffer->getPrimitiveType(),
			Buffer->getIndexType()
		);
	}
#ifdef _DEBUG
	// for debug purposes only:
	else if ( DebugDataVisible & scene::EDS_BBOX )
	{
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		video::SMaterial deb_m;
		deb_m.setFlag(video::EMF_LIGHTING, false);
		driver->setMaterial(deb_m);
		driver->draw3DBox(Buffer->BoundingBox, video::SColor(0,255,255,255));
	}
#endif
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CParticleSystemSceneNode::getBoundingBox() const
{
	return Buffer->getBoundingBox();
}


void CParticleSystemSceneNode::doParticleSystem(u32 time)
{
	if (LastEmitTime==0)
	{
		LastEmitTime = time;
		return;
	}

	u32 now = time;
	u32 timediff = time - LastEmitTime;
	LastEmitTime = time;

	// run emitter

	if (Emitter && IsVisible && Emitter->getMaxParticlesPerSecond())
	{
		SParticle* array = 0;
		s32 newParticles = Emitter->emitt(now, timediff, array);

		if (newParticles && array)
		{
			s32 j=Particles.size();
			if (newParticles > 16250-j)
				newParticles=16250-j;
			Particles.set_used(j+newParticles);
			for (s32 i=j; i<j+newParticles; ++i)
			{
				Particles[i]=array[i-j];
				AbsoluteTransformation.rotateVect(Particles[i].startVector);
				if (ParticlesAreGlobal)
					AbsoluteTransformation.transformVect(Particles[i].pos);
			}
		}
	}

	if (Particles.size())
	{
		// run affectors
		core::list<IParticleAffector*>::Iterator ait = AffectorList.begin();
		for (; ait != AffectorList.end(); ++ait)
			(*ait)->affect(now, Particles.pointer(), Particles.size());

		if (ParticlesAreGlobal)
			Buffer->BoundingBox.reset(AbsoluteTransformation.getTranslation());
		else
			Buffer->BoundingBox.reset(core::vector3df(0,0,0));

		// animate all particles
		f32 scale = (f32)timediff;

		for (u32 i=0; i<Particles.size();)
		{
			if (now > Particles[i].endTime)
				Particles.erase(i);
			else
			{
				Particles[i].pos += (Particles[i].vector * scale);
				Buffer->BoundingBox.addInternalPoint(Particles[i].pos);
				++i;
			}
		}

		const f32 m = (ParticleSize.Width > ParticleSize.Height ? ParticleSize.Width : ParticleSize.Height) * 0.5f;
		Buffer->BoundingBox.MaxEdge.X += m;
		Buffer->BoundingBox.MaxEdge.Y += m;
		Buffer->BoundingBox.MaxEdge.Z += m;

		Buffer->BoundingBox.MinEdge.X -= m;
		Buffer->BoundingBox.MinEdge.Y -= m;
		Buffer->BoundingBox.MinEdge.Z -= m;

		if (ParticlesAreGlobal)
		{
			core::matrix4 absinv( AbsoluteTransformation, core::matrix4::EM4CONST_INVERSE );
			absinv.transformBoxEx(Buffer->BoundingBox);
		}
	}
}


//! Sets if the particles should be global. If it is, the particles are affected by
//! the movement of the particle system scene node too, otherwise they completely
//! ignore it. Default is true.
void CParticleSystemSceneNode::setParticlesAreGlobal(bool global)
{
	ParticlesAreGlobal = global;
}


//! Sets the size of all particles.
void CParticleSystemSceneNode::setParticleSize(const core::dimension2d<f32> &size)
{
	ParticleSize = size;
}


void CParticleSystemSceneNode::reallocateBuffers()
{
	if (Particles.size() * 4 > Buffer->getVertexCount() ||
			Particles.size() * 6 > Buffer->getIndexCount())
	{
		u32 oldSize = Buffer->getVertexCount();
		Buffer->Vertices.set_used(Particles.size() * 4);

		u32 i;
		
		// fill remaining vertices
		for (i=oldSize; i<Buffer->Vertices.size(); i+=4)
		{
			Buffer->Vertices[0+i].TCoords.set(0.0f, 0.0f);
			Buffer->Vertices[1+i].TCoords.set(0.0f, 1.0f);
			Buffer->Vertices[2+i].TCoords.set(1.0f, 1.0f);
			Buffer->Vertices[3+i].TCoords.set(1.0f, 0.0f);
		}

		// fill remaining indices
		u32 oldIdxSize = Buffer->getIndexCount();
		u32 oldvertices = oldSize;
		Buffer->Indices.set_used(Particles.size() * 6);

		for (i=oldIdxSize; i<Buffer->Indices.size(); i+=6)
		{
			Buffer->Indices[0+i] = 0+oldvertices;
			Buffer->Indices[1+i] = 2+oldvertices;
			Buffer->Indices[2+i] = 1+oldvertices;
			Buffer->Indices[3+i] = 0+oldvertices;
			Buffer->Indices[4+i] = 3+oldvertices;
			Buffer->Indices[5+i] = 2+oldvertices;
			oldvertices += 4;
		}
	}
}


//! Writes attributes of the scene node.
void CParticleSystemSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	IParticleSystemSceneNode::serializeAttributes(out, options);

	out->addBool("GlobalParticles", ParticlesAreGlobal);
	out->addFloat("ParticleWidth", ParticleSize.Width);
	out->addFloat("ParticleHeight", ParticleSize.Height);

	// write emitter

	E_PARTICLE_EMITTER_TYPE type = EPET_COUNT;
	if (Emitter)
		type = Emitter->getType();

	out->addEnum("Emitter", (s32)type, ParticleEmitterTypeNames);

	if (Emitter)
		Emitter->serializeAttributes(out, options);

	// write affectors

	E_PARTICLE_AFFECTOR_TYPE atype = EPAT_NONE;

	for (core::list<IParticleAffector*>::ConstIterator it = AffectorList.begin();
		it != AffectorList.end(); ++it)
	{
		atype = (*it)->getType();

		out->addEnum("Affector", (s32)atype, ParticleAffectorTypeNames);

		(*it)->serializeAttributes(out);
	}

	// add empty affector to make it possible to add further affectors

	if (options && options->Flags & io::EARWF_FOR_EDITOR)
		out->addEnum("Affector", EPAT_NONE, ParticleAffectorTypeNames);
}


//! Reads attributes of the scene node.
void CParticleSystemSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	IParticleSystemSceneNode::deserializeAttributes(in, options);

	ParticlesAreGlobal = in->getAttributeAsBool("GlobalParticles");
	ParticleSize.Width = in->getAttributeAsFloat("ParticleWidth");
	ParticleSize.Height = in->getAttributeAsFloat("ParticleHeight");

	// read emitter

	int emitterIdx = in->findAttribute("Emitter");
	if (emitterIdx == -1)
		return;

	if (Emitter)
		Emitter->drop();
	Emitter = 0;

	E_PARTICLE_EMITTER_TYPE type = (E_PARTICLE_EMITTER_TYPE)
		in->getAttributeAsEnumeration("Emitter", ParticleEmitterTypeNames);

	switch(type)
	{
	case EPET_POINT:
		Emitter = createPointEmitter();
		break;
	case EPET_BOX:
		Emitter = createBoxEmitter();
		break;
	default:
		break;
	}

	u32 idx = 0;

	if (Emitter)
		idx = Emitter->deserializeAttributes(idx, in);

	++idx;

	// read affectors

	removeAllAffectors();
	u32 cnt = in->getAttributeCount();

	while(idx < cnt)
	{
		const char* name = in->getAttributeName(idx);

		if (!name || strcmp("Affector", name))
			return;

		E_PARTICLE_AFFECTOR_TYPE atype =
			(E_PARTICLE_AFFECTOR_TYPE)in->getAttributeAsEnumeration(idx, ParticleAffectorTypeNames);

		IParticleAffector* aff = 0;

		switch(atype)
		{
		case EPAT_FADE_OUT:
			aff = createFadeOutParticleAffector();
			break;
		case EPAT_GRAVITY:
			aff = createGravityAffector();
			break;
		case EPAT_SIZE:
			aff = createSizeAffector();
			break;
		case EPAT_SPIN:
			aff = createSpinAffector();
			break;
		case EPAT_NONE:
		default:
			break;
		}

		++idx;

		if (aff)
		{
			idx = aff->deserializeAttributes(idx, in, options);
			addAffector(aff);
			aff->drop();
		}
	}
	++idx;
}

//! Creates a clone of this scene node and its children.
ISceneNode* CParticleSystemSceneNode::clone()
{
	ISceneNode* newParent = Parent;
	ISceneManager* newManager = SceneManager;

	irr::core::vector3df rotation = RelativeRotation.getMatrix().getRotationDegrees();
	rotation *= irr::core::DEGTORAD;

	CParticleSystemSceneNode* nb = irrnew CParticleSystemSceneNode(false, 
		ID, RelativeTranslation, rotation, RelativeScale);

	nb->cloneMembers(this);

	nb->Buffer->Material = Buffer->Material;

	core::list<IParticleAffector*>::Iterator it = AffectorList.begin();
	for(; it != AffectorList.end(); it++)
	{
		nb->addAffector((*it));
	}	

	nb->ParticleSize = ParticleSize;
	nb->LastEmitTime = LastEmitTime;
	nb->MaxParticles = MaxParticles;
	nb->ParticlePrimitive = ParticlePrimitive;
	nb->ParticlesAreGlobal = ParticlesAreGlobal;

	switch(Emitter->getType()) 
	{
	case irr::scene::EPET_BOX: 
	{
		CParticleBoxEmitter* emitter = (CParticleBoxEmitter*) Emitter;
		IParticleEmitter* e = createBoxEmitter(
			emitter->getBox(),
			emitter->getDirection(), emitter->getMinParticlesPerSecond(),
			emitter->getMaxParticlesPerSecond(), emitter->getMinStartColor(),
			emitter->getMaxStartColor(), emitter->getMinLifeTime(), emitter->getMaxLifeTime(),
			emitter->getMaxAngleDegrees());
		nb->setEmitter(e);
		nb->Emitter->drop();							  
	}
	break;
	case irr::scene::EPET_ANIMATED_MESH:
	{
		CParticleAnimatedMeshSceneNodeEmitter* emitter = (CParticleAnimatedMeshSceneNodeEmitter*) Emitter;
		IAnimatedMeshSceneNode* animatedMesh = const_cast<IAnimatedMeshSceneNode*>(emitter->getAnimatedMeshSceneNode());
		IParticleEmitter* e = (IParticleEmitter*) createAnimatedMeshSceneNodeEmitter((IAnimatedMeshSceneNode*)animatedMesh->clone(),
			emitter->getUseNormalDirection(), emitter->getDirection(), emitter->getNormalDirectionModifier(),
			emitter->getMBNumber(), emitter->getEveryMeshVertex(), emitter->getMinParticlesPerSecond(),
			emitter->getMaxParticlesPerSecond(), emitter->getMinStartColor(),
			emitter->getMaxStartColor(), emitter->getMinLifeTime(), emitter->getMaxLifeTime(),
			emitter->getMaxAngleDegrees());
		nb->setEmitter(e);
		nb->Emitter->drop();							  
	}
	break;
	case irr::scene::EPET_POINT:
	{
		CParticlePointEmitter* emitter = (CParticlePointEmitter*) Emitter;
		IParticleEmitter* e = createPointEmitter(
			emitter->getDirection(), emitter->getMinParticlesPerSecond(),
			emitter->getMaxParticlesPerSecond(), emitter->getMinStartColor(),
			emitter->getMaxStartColor(), emitter->getMinLifeTime(), emitter->getMaxLifeTime(),
			emitter->getMaxAngleDegrees());
		nb->setEmitter(e);
		nb->Emitter->drop();							  
	}
	break;
	case irr::scene::EPET_CYLINDER:
	{
		CParticleCylinderEmitter* emitter = (CParticleCylinderEmitter*) Emitter;
		IParticleEmitter* e = createCylinderEmitter(
			emitter->getCenter(),
			emitter->getRadius(),
			emitter->getNormal(),
			emitter->getLength(),
			emitter->getOutlineOnly(),
			emitter->getDirection(), emitter->getMinParticlesPerSecond(),
			emitter->getMaxParticlesPerSecond(), emitter->getMinStartColor(),
			emitter->getMaxStartColor(), emitter->getMinLifeTime(), emitter->getMaxLifeTime(),
			emitter->getMaxAngleDegrees());
		nb->setEmitter(e);
		nb->Emitter->drop();							  
	}
	break;
	case irr::scene::EPET_MESH:
	{
		CParticleMeshEmitter* emitter = (CParticleMeshEmitter*) Emitter;
		IMesh* mesh = const_cast<IMesh*>(emitter->getMesh());

		IParticleEmitter* e = (IParticleEmitter*) createMeshEmitter(mesh,
			emitter->getUseNormalDirection(),
			emitter->getDirection(),
			emitter->getNormalDirectionModifier(),
			emitter->getMBNumber(),
			emitter->getEveryMeshVertex(), emitter->getMinParticlesPerSecond(),
			emitter->getMaxParticlesPerSecond(), emitter->getMinStartColor(),
			emitter->getMaxStartColor(), emitter->getMinLifeTime(), emitter->getMaxLifeTime(),
			emitter->getMaxAngleDegrees());
		nb->setEmitter(e);
		nb->Emitter->drop();							  
	}	
	break;
	case irr::scene::EPET_RING:
	{
		CParticleRingEmitter* emitter = (CParticleRingEmitter*) Emitter;
		IParticleEmitter* e = createRingEmitter(
			emitter->getCenter(),
			emitter->getRadius(),
			emitter->getRingThickness(),
			emitter->getDirection(), emitter->getMinParticlesPerSecond(),
			emitter->getMaxParticlesPerSecond(), emitter->getMinStartColor(),
			emitter->getMaxStartColor(), emitter->getMinLifeTime(), emitter->getMaxLifeTime(),
			emitter->getMaxAngleDegrees());
		nb->setEmitter(e);
		nb->Emitter->drop();							  
	}
	break;
	case irr::scene::EPET_SPHERE:
	{
		CParticleSphereEmitter* emitter = (CParticleSphereEmitter*) Emitter;
		IParticleEmitter* e = createSphereEmitter(
			emitter->getCenter(),
			emitter->getRadius(),
			emitter->getDirection(), emitter->getMinParticlesPerSecond(),
			emitter->getMaxParticlesPerSecond(), emitter->getMinStartColor(),
			emitter->getMaxStartColor(), emitter->getMinLifeTime(), emitter->getMaxLifeTime(),
			emitter->getMaxAngleDegrees());
		nb->setEmitter(e);
		nb->Emitter->drop();							  
	}
	default:
		nb->Emitter = 0;
		break;
	};

	return nb;
}

//! Sets the type of billboard in use
void CParticleSystemSceneNode::setBillboardType(E_PARTICLE_BILLBOARD_TYPE billboardType)
{
	BillboardType = billboardType;
}


} // end namespace scene
} // end namespace irr


