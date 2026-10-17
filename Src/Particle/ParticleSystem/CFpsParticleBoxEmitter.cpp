// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "stdafx.h"
#include "CFpsParticleBoxEmitter.h"
#include "IAttributes.h"
#include "irrMath.h"

namespace irr
{

//! constructor
CFpsParticleBoxEmitter::CFpsParticleBoxEmitter(
	const core::aabbox3df& box,
	const core::vector3df& direction, u32 speedVariation,
	u32 minParticlesPerSecond, u32 maxParticlesPerSecond,
	f32 particleWidth, f32 particleHeight, s32 sizeVariation,
	video::SColor minStartColor, video::SColor maxStartColor,
	u32 lifeTimeMin, u32 lifeTimeMax,
	s32 maxAngleDegreesXY, s32 maxAngleDegreesYZ, s32 maxAngleDegreesXZ,
	s32 initialRotMin, s32 initialRotMax)
 : Box(box), Direction(direction), SpeedVariation(speedVariation),
	MinParticlesPerSecond(minParticlesPerSecond), MaxParticlesPerSecond(maxParticlesPerSecond),
	ParticleWidth(particleWidth), ParticleHeight(particleHeight), SizeVariation(sizeVariation),
	MinStartColor(minStartColor), MaxStartColor(maxStartColor),
	MinLifeTime(lifeTimeMin), MaxLifeTime(lifeTimeMax), Time(0), Emitted(0),
	MaxAngleDegreesXY(maxAngleDegreesXY), MaxAngleDegreesYZ(maxAngleDegreesYZ), MaxAngleDegreesXZ(maxAngleDegreesXZ),
	InitialRotMin(initialRotMin), InitialRotMax(initialRotMax)
{
	#ifdef _DEBUG
	setDebugName("CFpsParticleBoxEmitter");
	#endif
}



//! Prepares an array with new particles to emitt into the system
//! and returns how much new particles there are.
s32 CFpsParticleBoxEmitter::emitt(u32 now, u32 timeSinceLastCall, SFpsParticle*& outArray, float scale, const irr::core::quaternion& rotation)
{
	Time += timeSinceLastCall;

	u32 pps = (MaxParticlesPerSecond - MinParticlesPerSecond);
	f32 perSecond = pps ? (f32)MinParticlesPerSecond + (os::Randomizer::rand() % pps) : MinParticlesPerSecond;
	f32 everyWhatMillisecond = 1000.0f / perSecond;

	if (Time > everyWhatMillisecond)
	{
		// scale and rotate box, direction and particle width/height
		core::vector3df edges[8];
		Box.getEdges(edges);
		for (int i = 0; i < 8; ++i)
			edges[i] = rotation * (edges[i] * scale);
		core::aabbox3df myBox;
		myBox.reset(edges[0]);
		for (int i = 1; i < 8; ++i)
			myBox.addInternalPoint(edges[i]);
		core::vector3df myDirection = rotation * (Direction * scale);
		float myParticleWidth  = ParticleWidth  * scale;
		float myParticleHeight = ParticleHeight * scale;

		Particles.set_used(0);
		u32 amount = (u32)((Time / everyWhatMillisecond) + 0.5f);
		Time = 0;
		SFpsParticle p;
		const core::vector3df& extent = myBox.getExtent();

		if (amount > MaxParticlesPerSecond*2)
			amount = MaxParticlesPerSecond * 2;

		for (u32 i=0; i<amount; ++i)
		{
			p.pos.X = myBox.MinEdge.X + fmodf((f32)os::Randomizer::rand(), extent.X);
			p.pos.Y = myBox.MinEdge.Y + fmodf((f32)os::Randomizer::rand(), extent.Y);
			p.pos.Z = myBox.MinEdge.Z + fmodf((f32)os::Randomizer::rand(), extent.Z);

			p.startTime = now;
			p.vector = myDirection;

			p.width = myParticleWidth;
			p.height = myParticleHeight;

			p.spin = 0;

			int deltaRot = InitialRotMax - InitialRotMin;
			p.initialRotation = InitialRotMin;

			if (deltaRot)
			{
				p.initialRotation = InitialRotMin + (os::Randomizer::rand() % (deltaRot));
			}

			if (SizeVariation != 0)
			{
				int rnd = os::Randomizer::rand() % (SizeVariation << 1) - SizeVariation;
				p.width = p.width + p.width * rnd / 100;
				p.height = p.height + p.height * rnd / 100;
			}
			
			core::vector3df tgt = myDirection;

			if (MaxAngleDegreesXY)
			{
				tgt.rotateXYBy((os::Randomizer::rand()%(MaxAngleDegreesXY*2)) - MaxAngleDegreesXY);
			}

			if (MaxAngleDegreesYZ)
			{
				tgt.rotateYZBy((os::Randomizer::rand()%(MaxAngleDegreesYZ*2)) - MaxAngleDegreesYZ);
			}

			if (MaxAngleDegreesXZ)
			{
				tgt.rotateXZBy((os::Randomizer::rand()%(MaxAngleDegreesXZ*2)) - MaxAngleDegreesXZ);
			}

			p.vector = tgt;

			if (MaxLifeTime - MinLifeTime == 0)
				p.endTime = now + MinLifeTime;
			else
				p.endTime = now + MinLifeTime + (os::Randomizer::rand() % (MaxLifeTime - MinLifeTime));

			p.color = MinStartColor.getInterpolated(
				MaxStartColor, (os::Randomizer::rand() % 100) / 100.0f);

			p.startColor = p.color;

			if (SpeedVariation > 0)
			{
				s32 rnd = os::Randomizer::rand();
				p.vector += ((rnd % SpeedVariation) / 100.0f) * p.vector;
			}

			p.startVector = p.vector;

			Particles.push_back(p);
		}

		outArray = Particles.pointer();

		return Particles.size();
	}

	return 0;
}


//! Writes attributes of the object.
void CFpsParticleBoxEmitter::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	core::vector3df b = Box.getExtent();
	b *= 0.5f;
	out->addVector3d("Box", b);
	out->addVector3d("Direction", Direction);
	out->addInt("SpeedVariation", SpeedVariation);
	out->addInt("MinParticlesPerSecond", MinParticlesPerSecond);
	out->addInt("MaxParticlesPerSecond", MaxParticlesPerSecond);
	out->addFloat("ParticleWidth", ParticleWidth);
	out->addFloat("ParticleHeight", ParticleHeight);
	out->addInt("SizeVariation", SizeVariation);
	out->addColor("MinStartColor", MinStartColor);
	out->addColor("MaxStartColor", MaxStartColor);
	out->addInt("MinLifeTime", MinLifeTime);
	out->addInt("MaxLifeTime", MaxLifeTime);
	out->addInt("MaxAngleDegXY", MaxAngleDegreesXY);
	out->addInt("MaxAngleDegYZ", MaxAngleDegreesYZ);
	out->addInt("MaxAngleDegXZ", MaxAngleDegreesXZ);
	out->addInt("InitialRotMin", InitialRotMin);
	out->addInt("InitialRotMax", InitialRotMax);
}


//! Reads attributes of the object.
s32 CFpsParticleBoxEmitter::deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	// read data and correct input values here

	core::vector3df b = in->getAttributeAsVector3d("Box");

	//if (b.X <= 0)
	//	b.X = 1.0f;
	//if (b.Y <= 0)
	//	b.Y = 1.0f;
	//if (b.Z <= 0)
	//	b.Z = 1.0f;

	if (b.X < 0)
		b.X = 0.0f;
	if (b.Y < 0)
		b.Y = 0.0f;
	if (b.Z < 0)
		b.Z = 0.0f;

	Box.MinEdge.X = -b.X;
	Box.MinEdge.Y = -b.Y;
	Box.MinEdge.Z = -b.Z;
	Box.MaxEdge.X = b.X;
	Box.MaxEdge.Y = b.Y;
	Box.MaxEdge.Z = b.Z;

	Direction = in->getAttributeAsVector3d("Direction");
	//if (Direction.getLength() == 0)
	//	Direction.set(0,0.01f,0);

	SpeedVariation = in->getAttributeAsInt("SpeedVariation");

	MinParticlesPerSecond = in->getAttributeAsInt("MinParticlesPerSecond");
	MaxParticlesPerSecond = in->getAttributeAsInt("MaxParticlesPerSecond");

	MinParticlesPerSecond = core::max_(1u, MinParticlesPerSecond);
	MaxParticlesPerSecond = core::max_(MaxParticlesPerSecond, 1u);
	MaxParticlesPerSecond = core::min_(MaxParticlesPerSecond, 200u);
	MinParticlesPerSecond = core::min_(MinParticlesPerSecond, MaxParticlesPerSecond);

	ParticleWidth = in->getAttributeAsFloat("ParticleWidth");
	ParticleHeight = in->getAttributeAsFloat("ParticleHeight");
	SizeVariation = in->getAttributeAsInt("SizeVariation");

	MinStartColor = in->getAttributeAsColor("MinStartColor");
	MaxStartColor = in->getAttributeAsColor("MaxStartColor");
	MinLifeTime = in->getAttributeAsInt("MinLifeTime");
	MaxLifeTime = in->getAttributeAsInt("MaxLifeTime");
	MaxAngleDegreesXY = in->getAttributeAsInt("MaxAngleDegXY");
	MaxAngleDegreesYZ = in->getAttributeAsInt("MaxAngleDegYZ");
	MaxAngleDegreesXZ = in->getAttributeAsInt("MaxAngleDegXZ");
	InitialRotMin = in->getAttributeAsInt("InitialRotMin");
	InitialRotMax = in->getAttributeAsInt("InitialRotMax");

	MinLifeTime = core::max_(0u, MinLifeTime);
	MaxLifeTime = core::max_(MaxLifeTime, MinLifeTime);
	MinLifeTime = core::min_(MinLifeTime, MaxLifeTime);

	return in->findAttribute("InitialRotMax");
}

}