// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "stdafx.h"
#include "CFpsParticleSystemSceneNode.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"
#include "IVideoDriver.h"

#include "CFpsParticleBoxEmitter.h"
#include "CFpsParticleAttractionAffector.h"
#include "CFpsParticleFadeOutAffector.h"
#include "CFpsParticleGravityAffector.h"
#include "CFpsParticleRotationAffector.h"
#include "CFpsParticleSizeAffector.h"
#include "CFpsParticleSpinAffector.h"
#include "SViewFrustum.h"

namespace irr
{

using namespace scene;

#define PARTICLES_GROWTH_RATE 64

//! constructor
CFpsParticleSystemSceneNode::CFpsParticleSystemSceneNode(bool createDefaultEmitter,
	s32 id,
	const core::vector3df& position, const core::vector3df& rotation,
	const core::vector3df& scale)
	: IFpsParticleSystemSceneNode(id, position, rotation, scale),
	Emitter(0), LastEmitTime(0), MaxParticles(0xffff), Buffer(0),
	ParticlesAreGlobal(true), BillboardType(irr::EPBT_CAMERA_FACING),
	Transparency(1.0), DirectionalRotation(true), ProjectDirection(false),
	SysMinLifeTime(-1), SysMaxLifeTime(-1), RestartTimeMin(-1), RestartTimeMax(-1),
	StartDelay(0)
{
	#ifdef _DEBUG
	setDebugName("CFpsParticleSystemSceneNode");
	#endif

	Buffer = irrnew SMeshBuffer();
	if (createDefaultEmitter)
	{
		IFpsParticleEmitter* e = createBoxEmitter();
		setEmitter(e);
		e->drop();
	}

	// reduce copies when the buffers grow
	Particles.set_alloc_chunk_count( PARTICLES_GROWTH_RATE );
	Buffer->Vertices.set_alloc_chunk_count( PARTICLES_GROWTH_RATE * 4 );
	Buffer->Indices.set_alloc_chunk_count( PARTICLES_GROWTH_RATE * 4 );	

	SetRandomLifeTime();

	m_bPaused = false;
}


//! destructor
CFpsParticleSystemSceneNode::~CFpsParticleSystemSceneNode()
{
	if (Emitter)
		Emitter->drop();
	if (Buffer)
		Buffer->drop();

	removeAllAffectors();
}


void CFpsParticleSystemSceneNode::SetRandomLifeTime()
{
	TotalEmitTime = 0;
	TotalRestartTime = 0;
	TotalDelay = 0;

	if (SysMinLifeTime == SysMaxLifeTime)
	{
		RandomSysLifeTime = SysMinLifeTime;
	}
	else
	{
		RandomSysLifeTime = SysMinLifeTime + (os::Randomizer::rand() % (SysMaxLifeTime - SysMinLifeTime));
	}

	if (RestartTimeMin == RestartTimeMax)
	{
		RandomRestartTime = RestartTimeMin;
	}
	else
	{
		RandomRestartTime = RestartTimeMin + (os::Randomizer::rand() % (RestartTimeMax - RestartTimeMin));
	}
}


//! Gets the particle emitter, which creates the particles.
IFpsParticleEmitter* CFpsParticleSystemSceneNode::getEmitter()
{
	return Emitter;
}


//! Sets the particle emitter, which creates the particles.
void CFpsParticleSystemSceneNode::setEmitter(IFpsParticleEmitter* emitter)
{
	if (Emitter)
		Emitter->drop();

	Emitter = emitter;

	if (Emitter)
		Emitter->grab();
}


//! Adds new particle effector to the particle system.
void CFpsParticleSystemSceneNode::addAffector(IFpsParticleAffector* affector)
{
	affector->grab();
	AffectorList.push_back(affector);
}

//! Removes particle affector from particle system.
void CFpsParticleSystemSceneNode::removeAffector(IFpsParticleAffector* affector)
{
	core::list<IFpsParticleAffector*>::Iterator it = AffectorList.begin();
	while (it != AffectorList.end())
	{
		if ((*it) == affector)
		{
			(*it)->drop();
			AffectorList.erase(it);
			break;
		}

		it++;
	}
}

//! Removes all particle affectors in the particle system.
void CFpsParticleSystemSceneNode::removeAllAffectors()
{
	core::list<IFpsParticleAffector*>::Iterator it = AffectorList.begin();
	while (it != AffectorList.end())
	{
		(*it)->drop();
		it = AffectorList.erase(it);
	}
}


//! Returns the material based on the zero based index i.
video::SMaterial& CFpsParticleSystemSceneNode::getMaterial(u32 i)
{
	return Buffer->Material;
}


//! Returns amount of materials used by this scene node.
u32 CFpsParticleSystemSceneNode::getMaterialCount() const
{
	return 1;
}


//! Creates a box particle emitter.
IFpsParticleBoxEmitter* CFpsParticleSystemSceneNode::createBoxEmitter(
	const core::aabbox3df& box, const core::vector3df& direction, u32 speedVariation,
	u32 minParticlesPerSecond, u32 maxParticlesPerSecond, f32 particleWidth, f32 particleHeight, s32 sizeVariation,
	const video::SColor& minStartColor, const video::SColor& maxStartColor,
	u32 lifeTimeMin, u32 lifeTimeMax,
	s32 maxAngleDegreesXY, s32 maxAngleDegreesYZ, s32 maxAngleDegreesXZ,
	s32 initialRotMin, s32 initialRotMax)
{
	return irrnew CFpsParticleBoxEmitter(box, direction, speedVariation, minParticlesPerSecond,
		maxParticlesPerSecond, particleWidth, particleHeight, sizeVariation, minStartColor, maxStartColor,
		lifeTimeMin, lifeTimeMax, maxAngleDegreesXY, maxAngleDegreesYZ, maxAngleDegreesXZ, initialRotMin, initialRotMax);
}


//! Creates a point attraction affector. This affector modifies the positions of the
//! particles and attracts them to a specified point at a specified speed per second.
IFpsParticleAttractionAffector* CFpsParticleSystemSceneNode::createAttractionAffector(
	const core::vector3df& point, f32 speed, bool attract,
	bool affectX, bool affectY, bool affectZ )
{
	return irrnew CFpsParticleAttractionAffector( point, speed, attract, affectX, affectY, affectZ );
}


//! Creates a fade out particle affector.
IFpsParticleFadeOutAffector* CFpsParticleSystemSceneNode::createFadeOutParticleAffector(
		const video::SColor& targetColor, f32 startTimePercent, f32 endTimePercent)
{
	return irrnew CFpsParticleFadeOutAffector(targetColor, startTimePercent, endTimePercent);
}


//! Creates a gravity affector.
IFpsParticleGravityAffector* CFpsParticleSystemSceneNode::createGravityAffector(
		const core::vector3df& gravity, f32 startTimePercent, f32 endTimePercent)
{
	return irrnew CFpsParticleGravityAffector(gravity, startTimePercent, endTimePercent);
}


//! Creates a rotation affector. This affector rotates the particles around a specified pivot
//! point.  The speed represents Degrees of rotation per second.
IFpsParticleRotationAffector* CFpsParticleSystemSceneNode::createRotationAffector(
	const core::vector3df& speed, const core::vector3df& pivotPoint )
{
	return irrnew CFpsParticleRotationAffector( speed, pivotPoint );
}

//! Creates a size affector. This affector changes the size of the particles
//! from 0 to the target size during the amount of time
IFpsParticleSizeAffector* CFpsParticleSystemSceneNode::createSizeAffector(
		f32 targetWidth, f32 targetHeight, u32 variation, f32 startTimePercent, f32 endTimePercent)
{
	return irrnew CFpsParticleSizeAffector( targetWidth, targetHeight, variation, startTimePercent, endTimePercent );
}

//! Creates a spin affector. This affector spins a particule performing a full
//! spin during the amount of time
IFpsParticleSpinAffector* CFpsParticleSystemSceneNode::createSpinAffector(
		s32 minSpin, s32 maxSpin, f32 startTimePercent, f32 endTimePercent)
{
	return irrnew CFpsParticleSpinAffector( minSpin, maxSpin, startTimePercent, endTimePercent );
}

void CFpsParticleSystemSceneNode::OnAnimate(u32 timeMs)
{ 
	IFpsParticleSystemSceneNode::OnAnimate(timeMs);
	/*if (!os::Timer::isStopped())
		doParticleSystem(os::Timer::getTime());*/

	doParticleSystem(timeMs);
}

//! pre render event
void CFpsParticleSystemSceneNode::OnRegisterSceneNode()
{
	if (IsVisible && (Particles.size() != 0))
	{
		SceneManager->registerNodeForRendering(this, &Buffer->Material, 0, scene::ESNRP_TRANSPARENT);
		ISceneNode::OnRegisterSceneNode();
	}
}


//! Sets global transparency of the node
void CFpsParticleSystemSceneNode::setTransparency(f32 transparency)
{
	Transparency = transparency;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFpsParticleSystemSceneNode::project3DTo2D (core::vector3df pt, core::vector2df* result, ICameraSceneNode* camera)
{
	if (camera == 0)
		camera = SceneManager->getActiveCamera();
	core::rect<s32> viewport = SceneManager->getVideoDriver()->getViewPort();
	core::dimension2di dim = viewport.getSize() / 2;

	core::matrix4 trans = camera->getProjectionMatrix() * camera->getViewMatrix();
	f32 transformedPos[4] = { pt.X, pt.Y, pt.Z, 1.0f };
	trans.multiplyWith1x4Matrix(transformedPos);

	if (transformedPos[3] < 0.01) return false;

	result->X = dim.Width * (1.0f + transformedPos[0] / transformedPos[3]);
	result->Y = dim.Height * (1.0f - transformedPos[1] / transformedPos[3]);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//! render
void CFpsParticleSystemSceneNode::render(void*)
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	ICameraSceneNode* camera = SceneManager->getActiveCamera();

	if (!camera || !driver)
		return;

	bool hasSpinAffector = false;

	video::SColor TargetTransparencyColor;
	core::list<IFpsParticleAffector*>::Iterator it = AffectorList.begin();
	for(; it != AffectorList.end(); it++)
	{
		if ((*it)->getType() == FPS_EPAT_SPIN)
		{
			hasSpinAffector = true;
		}
		if ((*it)->getType() == FPS_EPAT_FADE_OUT)
		{
			CFpsParticleFadeOutAffector* affector = (CFpsParticleFadeOutAffector*) (*it);
			TargetTransparencyColor = affector->getTargetColor();
		}
	}	

	core::vector3df horizontal;
	core::vector3df vertical;
	core::vector3df view;

	//if (BillboardType == EPBT_UP_VECTOR_FACING)
	//{
	//	// calculate vectors for letting particles look up vector
	//	view = core::vector3df(camera->getTarget() - camera->getAbsolutePosition());
	//	view.normalize();

	//	horizontal = camera->getUpVector().crossProduct(view);
	//	horizontal.normalize();
	//	horizontal *= 0.5f * ParticleSize.Width;

	//	vertical = camera->getUpVector().crossProduct(horizontal);
	//	vertical.normalize();
	//	vertical *= 0.5f * ParticleSize.Height;

	//	view = camera->getUpVector();
	//}
	//else
	//{
		// calculate vectors for letting particles look to camera
		const core::matrix4 &m = camera->getViewFrustum()->Matrices [ video::ETS_VIEW ];

		//f32 f;

		//f = 0.5f * ParticleSize.Width;
		//horizontal = core::vector3df( m[0] * f, m[4] * f, m[8] * f );

		//f = -0.5f * ParticleSize.Height;
		//vertical = core::vector3df( m[1] * f, m[5] * f, m[9] * f );

		view = core::vector3df( -m[2], -m[6] , -m[10] );

		view *= -1.0f;
	//}

	// reallocate arrays, if they are too small
	reallocateBuffers();

	// create particle vertex data
	s32 idx = 0;
	for (u32 i=0; i<Particles.size(); ++i)
	{
		const SFpsParticle& particle = Particles[i];

		///////////////////////////////////////////////////////////////
		f32 f;

		f = 0.5f * particle.width;
		horizontal = core::vector3df( m[0] * f, m[4] * f, m[8] * f );

		f = -0.5f * particle.height;
		vertical = core::vector3df( m[1] * f, m[5] * f, m[9] * f );
		///////////////////////////////////////////////////////////////

#ifdef OS_IPHONE
		//RNDasakalov TODO: remove this patch for iphone
		horizontal = horizontal * particle.height / particle.width;
		vertical = vertical * particle.width / particle.height;
#endif

		if (Transparency < 1.0)
		{
			Particles[i].color = Particles[i].color.getInterpolated(TargetTransparencyColor, Transparency);
		}

		core::vector3df hor = horizontal;
		core::vector3df ver = vertical;

		if (particle.initialRotation)
		{
			core::quaternion q;
			q.fromAngleAxis(particle.initialRotation * core::DEGTORAD, view);
			core::matrix4 matrix = q.getMatrix();
			matrix.rotateVect(hor);
			matrix.rotateVect(ver);
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
		else if (DirectionalRotation)
		{
			if (!ProjectDirection)
			{
				core::quaternion quaternion;
				core::matrix4 matrix;
				quaternion.rotationFromTo(SystemRotation * (Emitter->getDirection() * SystemScale), particle.pos - particle.oldPos);
				quaternion.getMatrix(matrix); 
				matrix.rotateVect(hor);
				matrix.rotateVect(ver);
			}
			else
			{
				core::vector2df p1;
				project3DTo2D(particle.pos, &p1);
				core::vector2df p2;
				project3DTo2D(particle.oldPos, &p2);

				float angleRad = atan2((p1.X - p2.X), (p1.Y - p2.Y));

				core::quaternion q;
				q.fromAngleAxis(angleRad, view);
				core::matrix4 matrix = q.getMatrix();
				matrix.rotateVect(hor);
				matrix.rotateVect(ver);

/*
				//core::vector3df v1 = Emitter->getDirection();
				//v1.normalize();
				core::vector3df v2 = particle.pos - particle.oldPos;
				v2.normalize();
				//float angleRad = acos(v1.dotProduct(v2));
				core::plane3df plane(core::vector3df(0, 0, 0), view);
				core::vector3df outIntersection;
				plane.getIntersectionWithLine(v2, view, outIntersection);
				if (outIntersection.getLengthSQ() > 0)
				{
					outIntersection.normalize();
					core::vector3df up( m[1], m[5], m[9] );
					float angleRad = acos(up.dotProduct(outIntersection));

					core::vector3df cross = up.crossProduct(outIntersection);
					f32 dot = cross.dotProduct(view);
					if (dot < 0) 
					{
						angleRad = -angleRad;
					}

					//core::vector3df cross = v1.crossProduct(v2);
					//f32 dot = cross.dotProduct(view);
					//if (dot < 0) 
					//{
					//	angleRad = -angleRad;
					//}

					core::quaternion q;
					q.fromAngleAxis(angleRad, view);
					core::matrix4 matrix = q.getMatrix();
					matrix.rotateVect(hor);
					matrix.rotateVect(ver);
				}*/
			}
		}

		core::vector3df tmpV;
		video::E_ORIENTATION curOrien = SceneManager->getVideoDriver()->getOrientation3D();
		switch(curOrien)
		{
		case video::EOO_0:
			break;
		case video::EOO_270:
			tmpV = ver;
			ver = hor;
			hor = -tmpV;
			break;

		case video::EOO_90:
			tmpV = ver;
			ver = -hor;
			hor = tmpV;
			break;
		case video::EOO_180:
			tmpV = ver;
			ver = -hor;
			hor = -tmpV;
			break;
		default:
			break;
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
	core::matrix4 mat;

	if (!ParticlesAreGlobal)
		mat.setTranslation(AbsoluteTransformation.getTranslation());
	driver->setTransform(video::ETS_WORLD, mat);

	driver->setMaterial(Buffer->Material);

	driver->drawVertexPrimitiveList(
		Buffer->getVertices(),
		Buffer->getIndices(),
		0, 
		Particles.size() * 6,
		Particles.size() * 2,
		video::EVT_STANDARD,
		Buffer->getPrimitiveType(),
		Buffer->getIndexType()
	);
#ifdef _DEBUG
	// for debug purposes only:
	if ( DebugDataVisible & scene::EDS_BBOX )
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
const core::aabbox3d<f32>& CFpsParticleSystemSceneNode::getBoundingBox() const
{
	return Buffer->getBoundingBox();
}


void CFpsParticleSystemSceneNode::doParticleSystem(u32 time)
{
	if (LastEmitTime==0)
	{
		LastEmitTime = time;
	}

	u32 now = time;
	u32 timediff = time - LastEmitTime;
	LastEmitTime = time;

	if (m_bPaused) return;

	AllocatorsEnable(true);

	bool bDelay = false;

	if (TotalDelay < StartDelay)
	{
		TotalDelay += timediff;
		bDelay = true;
	}

	bool bEmit = true;

	//-1 stands for infinit run
	if (!bDelay && SysMinLifeTime != -1 && SysMaxLifeTime != -1)
	{
		if (TotalEmitTime > RandomSysLifeTime)
		{
			//-1 stands for no restart
			if (RestartTimeMin != -1 && RestartTimeMax != -1)
			{
				TotalRestartTime += timediff;

				if (TotalRestartTime > RandomRestartTime)
				{
					Restart();
				}
			}

			bEmit = false;
		}
	}

	// run emitter
	float systemScale = SystemScale;//RelativeScale.X;
	const core::quaternion& systemRotation = SystemRotation;//RelativeRotation;
	core::vector3df oldPos = getPosition();
	setPosition(systemRotation * (oldPos * systemScale));
	updateAbsolutePosition();

	if (Emitter && IsVisible && bEmit && !bDelay)
	{
		SFpsParticle* array = 0;
		u32 time = timediff;
		if (time == 0 && StartDelay < 0) 
		{
			time = -StartDelay;
		}

		s32 newParticles = Emitter->emitt(now, time, array, systemScale, systemRotation);

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

	if (DirectionalRotation)
	{
		for (u32 i=0; i<Particles.size();i++)
		{
			Particles[i].oldPos = Particles[i].pos;
		}
	}

	// run affectors
	core::list<IFpsParticleAffector*>::Iterator ait = AffectorList.begin();
	for (; ait != AffectorList.end(); ++ait)
		(*ait)->affect(now - timediff, now, Particles.pointer(), Particles.size(), systemScale, systemRotation);

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

	NodeFlags |= ESNF_TRANSFORMED_BBOX_DIRTY;

	//const f32 m = (ParticleSize.Width > ParticleSize.Height ? ParticleSize.Width : ParticleSize.Height) * 0.5f;
	//Buffer->BoundingBox.MaxEdge.X += m;
	//Buffer->BoundingBox.MaxEdge.Y += m;
	//Buffer->BoundingBox.MaxEdge.Z += m;

	//Buffer->BoundingBox.MinEdge.X -= m;
	//Buffer->BoundingBox.MinEdge.Y -= m;
	//Buffer->BoundingBox.MinEdge.Z -= m;

	if (ParticlesAreGlobal)
	{
		core::matrix4 absinv( AbsoluteTransformation, core::matrix4::EM4CONST_INVERSE );
		absinv.transformBoxEx(Buffer->BoundingBox);
	}

	if (!bDelay && bEmit)
	{
		TotalEmitTime += timediff;
	}

	setPosition(oldPos);

	AllocatorsEnable(false);
}


//! Sets if the particles should be global. If it is, the particles are affected by
//! the movement of the particle system scene node too, otherwise they completely
//! ignore it. Default is true.
void CFpsParticleSystemSceneNode::setParticlesAreGlobal(bool global)
{
	ParticlesAreGlobal = global;
}


void CFpsParticleSystemSceneNode::reallocateBuffers()
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
void CFpsParticleSystemSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	IFpsParticleSystemSceneNode::serializeAttributes(out, options);

	out->addBool("GlobalParticles", ParticlesAreGlobal);

	out->addBool("DirectionalRotation", DirectionalRotation);
	out->addBool("ProjectDirection", ProjectDirection);

	out->addInt("SysMinLifeTime", SysMinLifeTime);
	out->addInt("SysMaxLifeTime", SysMaxLifeTime);
	out->addInt("RestartTimeMin", RestartTimeMin);
	out->addInt("RestartTimeMax", RestartTimeMax);
	out->addInt("StartDelay", StartDelay);

	// write emitter

	E_FPS_PARTICLE_EMITTER_TYPE type = FPS_EPET_COUNT;
	if (Emitter)
		type = Emitter->getType();

	out->addEnum("Emitter", (s32)type, FpsParticleEmitterTypeNames);

	if (Emitter)
		Emitter->serializeAttributes(out, options);

	// write affectors

	E_FPS_PARTICLE_AFFECTOR_TYPE atype = FPS_EPAT_NONE;

	for (core::list<IFpsParticleAffector*>::ConstIterator it = AffectorList.begin();
		it != AffectorList.end(); ++it)
	{
		atype = (*it)->getType();

		out->addEnum("Affector", (s32)atype, FpsParticleAffectorTypeNames);

		(*it)->serializeAttributes(out);
	}

	// add empty affector to make it possible to add further affectors

	if (options && options->Flags & io::EARWF_FOR_EDITOR)
		out->addEnum("Affector", FPS_EPAT_NONE, FpsParticleAffectorTypeNames);
}


//! Reads attributes of the scene node.
void CFpsParticleSystemSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	IFpsParticleSystemSceneNode::deserializeAttributes(in, options);

	ParticlesAreGlobal = in->getAttributeAsBool("GlobalParticles");

	DirectionalRotation = in->getAttributeAsBool("DirectionalRotation");
	ProjectDirection = in->getAttributeAsBool("ProjectDirection");

	if (in->existsAttribute("SysMinLifeTime"))
	{
		SysMinLifeTime = in->getAttributeAsInt("SysMinLifeTime");
	}
	else
	{
		SysMinLifeTime = -1;
	}

	if (in->existsAttribute("SysMaxLifeTime"))
	{
		SysMaxLifeTime = in->getAttributeAsInt("SysMaxLifeTime");
	}
	else
	{
		SysMaxLifeTime = -1;
	}

	if (in->existsAttribute("RestartTimeMin"))
	{
		RestartTimeMin = in->getAttributeAsInt("RestartTimeMin");
	}
	else
	{
		RestartTimeMin = -1;
	}

	if (in->existsAttribute("RestartTimeMax"))
	{
		RestartTimeMax = in->getAttributeAsInt("RestartTimeMax");
	}
	else
	{
		RestartTimeMax = -1;
	}

	StartDelay = in->getAttributeAsInt("StartDelay");

	SetRandomLifeTime();

	// read emitter

	int emitterIdx = in->findAttribute("Emitter");
	if (emitterIdx == -1)
		return;

	if (Emitter)
		Emitter->drop();
	Emitter = 0;

	E_FPS_PARTICLE_EMITTER_TYPE type = (E_FPS_PARTICLE_EMITTER_TYPE)
		in->getAttributeAsEnumeration("Emitter", FpsParticleEmitterTypeNames);

	switch(type)
	{
	case FPS_EPET_BOX:
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

		E_FPS_PARTICLE_AFFECTOR_TYPE atype =
			(E_FPS_PARTICLE_AFFECTOR_TYPE)in->getAttributeAsEnumeration(idx, FpsParticleAffectorTypeNames);

		IFpsParticleAffector* aff = 0;

		switch(atype)
		{
		case FPS_EPAT_ATTRACT:
			aff = createAttractionAffector();
			break;
		case FPS_EPAT_FADE_OUT:
			aff = createFadeOutParticleAffector();
			break;
		case FPS_EPAT_GRAVITY:
			aff = createGravityAffector();
			break;
		case FPS_EPAT_SIZE:
			aff = createSizeAffector();
			break;
		case FPS_EPAT_ROTATE:
			aff = createRotationAffector();
			break;
		case FPS_EPAT_SPIN:
			aff = createSpinAffector();
			break;
		case FPS_EPAT_NONE:
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
ISceneNode* CFpsParticleSystemSceneNode::clone()
{
	ISceneNode* newParent = Parent;
	ISceneManager* newManager = SceneManager;

	irr::core::vector3df rotation = RelativeRotation.getMatrix().getRotationDegrees();
	rotation *= irr::core::DEGTORAD;

	CFpsParticleSystemSceneNode* nb = irrnew CFpsParticleSystemSceneNode(false, 
		ID, RelativeTranslation, rotation, RelativeScale);

	nb->setDirectionalRotation(DirectionalRotation);
	nb->setProjectDirection(ProjectDirection);
	nb->setSysMinLifeTime(SysMinLifeTime);
	nb->setSysMaxLifeTime(SysMaxLifeTime);
	nb->setRestartTimeMin(RestartTimeMin);
	nb->setRestartTimeMax(RestartTimeMax);
	nb->setStartDelay(StartDelay);

	nb->SetRandomLifeTime();

	nb->cloneMembers(this);

	nb->Buffer->Material = Buffer->Material;

	core::list<IFpsParticleAffector*>::Iterator it = AffectorList.begin();
	for(; it != AffectorList.end(); it++)
	{
		IFpsParticleAffector* aff = 0;

		switch ((*it)->getType())
		{
			case FPS_EPAT_ATTRACT:
				{
					CFpsParticleAttractionAffector* pAff = (CFpsParticleAttractionAffector*)(*it);
					aff = createAttractionAffector(pAff->getPoint(), pAff->getSpeed(), pAff->getAttract(), 
						pAff->getAffectX(), pAff->getAffectY(), pAff->getAffectZ());
				}
				break;
			case FPS_EPAT_FADE_OUT:
				{
					CFpsParticleFadeOutAffector* pAff = (CFpsParticleFadeOutAffector*)(*it);
					aff = createFadeOutParticleAffector(pAff->getTargetColor(), pAff->getStartTimePercent(), 
						pAff->getEndTimePercent());
				}
				break;
			case FPS_EPAT_GRAVITY:
				{
					CFpsParticleGravityAffector* pAff = (CFpsParticleGravityAffector*)(*it);
					aff = createGravityAffector(pAff->getGravity(), pAff->getStartTimePercent(), pAff->getEndTimePercent());
				}
				break;
			case FPS_EPAT_SIZE:
				{
					CFpsParticleSizeAffector* pAff = (CFpsParticleSizeAffector*)(*it);
					aff = createSizeAffector(pAff->getTargetWidth(), pAff->getTargetHeight(), pAff->getVariation(), pAff->getStartTimePercent(),
						pAff->getEndTimePercent());
				}
				break;
			case FPS_EPAT_ROTATE:
				{
					CFpsParticleRotationAffector* pAff = (CFpsParticleRotationAffector*)(*it);
					aff = createRotationAffector(pAff->getSpeed(), pAff->getPivotPoint());
				}
				break;
			case FPS_EPAT_SPIN:
				{
					CFpsParticleSpinAffector* pAff = (CFpsParticleSpinAffector*)(*it);
					aff = createSpinAffector(pAff->getMinSpin(), pAff->getMaxSpin(), pAff->getStartTimePercent(), pAff->getEndTimePercent());
				}
				break;
		}

		if (aff) 
		{
			nb->addAffector(aff);
			aff->drop();
		}
		
	}	

	nb->LastEmitTime = LastEmitTime;
	nb->MaxParticles = MaxParticles;
	nb->ParticlePrimitive = ParticlePrimitive;
	nb->ParticlesAreGlobal = ParticlesAreGlobal;

	switch(Emitter->getType()) 
	{
	case FPS_EPET_BOX: 
	{
		CFpsParticleBoxEmitter* emitter = (CFpsParticleBoxEmitter*) Emitter;
		IFpsParticleEmitter* e = createBoxEmitter(
			emitter->getBox(),
			emitter->getDirection(), emitter->getSpeedVariation(), 
			emitter->getMinParticlesPerSecond(), emitter->getMaxParticlesPerSecond(),
			emitter->getParticleWidth(), emitter->getParticleHeight(), emitter->getSizeVariation(),
			emitter->getMinStartColor(), emitter->getMaxStartColor(),
			emitter->getMinLifeTime(), emitter->getMaxLifeTime(),
			emitter->getMaxAngleDegreesXY(), emitter->getMaxAngleDegreesYZ(), emitter->getMaxAngleDegreesXZ(),
			emitter->getInitialRotMin(), emitter->getInitialRotMax());
		nb->setEmitter(e);
		nb->Emitter->drop();							  
	}
	break;

	default:
		nb->Emitter = 0;
		break;
	};

	return nb;
}

//! Sets the type of billboard in use
void CFpsParticleSystemSceneNode::setBillboardType(E_FPS_PARTICLE_BILLBOARD_TYPE billboardType)
{
	BillboardType = billboardType;
}

//! Returns true if particle system does not emit particles anymore
bool CFpsParticleSystemSceneNode::IsEnded()
{
	if (Particles.size() == 0 && TotalEmitTime > RandomSysLifeTime)
	{
		//-1 stands for no restart
		if (RestartTimeMin == -1 || RestartTimeMax == -1)
		{
			return true;
		}
	}

	return false;
}

//! Restarts the particle system after is ended
void CFpsParticleSystemSceneNode::Restart()
{
	//for (u32 i=0; i < Particles.size(); i++)
	//{
	//	Particles.erase(i);
	//}

	Emitter->Reset();

	m_bPaused = false;

	LastEmitTime = 0;

	SetRandomLifeTime();
}

//! Pause the particle system
void CFpsParticleSystemSceneNode::Pause()
{
	m_bPaused = true;
}

//! Unpause the particle system
void CFpsParticleSystemSceneNode::UnPause()
{
	m_bPaused = false;
}

void CFpsParticleSystemSceneNode::SetSystemScale (float scale)
{
	SystemScale = scale;
}

void CFpsParticleSystemSceneNode::SetSystemRotation (const core::quaternion& rotation)
{
	SystemRotation = rotation;
}


}