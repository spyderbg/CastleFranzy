#include "stdafx.h"
#include "ICParticleSystemBridge.h"
#include "ICBridgeableParticleSystemParams.h"
#include "Common/ConversionFunctions.h"

ICParticleSystemBridge *ICParticleSystemBridge::s_pParticleSystemBridge = NULL;

void ICParticleSystemBridge::PreUpdate( TimeValue tFrames, irr::ps::IParticleSystem<irr::ps::SParticle> *pParticleSystem, ICBridgeableParticleSystemInParams &inParams )
{
	for ( InParamList::iterator paramIter = inParams.m_uiParams.begin(); paramIter != inParams.m_uiParams.end(); ++paramIter )
	{
		(*paramIter)->UpdateSystemValueUsingPBlockValue( tFrames, FOREVER );
	}

	pParticleSystem->setWorldMatrixPtr( const_cast<irr::core::matrix4 *>(&inParams.m_emitterWorldTM) );
	//pParticleSystem->Init();
}

void ICParticleSystemBridge::PostUpdate( TimeValue tFrames, irr::ps::IParticleSystem<irr::ps::SParticle> *pParticleSystem, ICBridgeableParticleSystemOutParams &outParams )
{
	// original
	float *pPtr = 0;
	size_t stride, pos3Ofs, vel3Ofs, colorOfs, age1Ofs, sizeOfs;
	outParams.m_particleCount = pParticleSystem->getPGeneralPointers( pPtr, stride, pos3Ofs, vel3Ofs, colorOfs, age1Ofs, sizeOfs);
	outParams.m_pParticlePositions = pPtr + pos3Ofs;
	outParams.m_pParticleVelocities = pPtr + vel3Ofs;
	outParams.m_pParticleColors = pPtr + colorOfs;
	outParams.m_pParticleAges = pPtr + age1Ofs;
	outParams.m_pParticleSizes = pPtr + sizeOfs;
	outParams.m_particleStride = stride;

	//// Get Irrlicht Particle System out params
	//int numParticles = 0; // Irrlicht_GetNumParticles

	//if ( numParticles <= 0 )
	//{
	//	outParams.m_particles.clear();
	//	return;
	//}

	//outParams.m_particles.resize( numParticles );
	//for ( size_t particleId = 0; particleId < ( unsigned )numParticles; ++particleId )
	//{
	//	ICBridgeableParticleParamsOut &outParticle = outParams.m_particles[particleId];

	//	outParticle.m_position.X = 0.f; // Irrlicht_GetParticlePosition
	//	outParticle.m_position.Y = 0.f;
	//	outParticle.m_position.Z = 0.f;

	//	outParticle.m_velocity.X = 0.f; // Irrlicht_GetParticleVelocity
	//	outParticle.m_velocity.Y = 0.f;
	//	outParticle.m_velocity.Z = 0.f;

	//	outParticle.m_age = 0.f; // Irrlicht_GetParticleAge
	//	outParticle.m_radius = 0.f; // Irrlicht_GetParticleRadius
	//	outParticle.m_tension = 0.f; // Irrlicht_GetParticleTension
	//}

	//outParams.m_particleSize = 0.f; // TODO : How do I set this variable
}


void ICParticleSystemBridge::Update( TimeValue tFrames, irr::ps::IParticleSystem<irr::ps::SParticle> *pParticleSystem, ICBridgeableParticleSystemInParams &inParams, ICBridgeableParticleSystemOutParams &outParams )
{
	PreUpdate( tFrames, pParticleSystem, inParams );
	//irr::ps::PSIncrementalUpdate(pParticleSystem, 0.0f, FSConvert::MaxTickToSecs(tFrames), FSConvert::MaxTickToSecs(GetTicksPerFrame()));
	pParticleSystem->update( FSConvert::MaxTickToSecs( tFrames ) );
	PostUpdate( tFrames, pParticleSystem, outParams );
}

irr::core::matrix4 ICParticleSystemBridge::MaxMatrix3ToIrrCoreMatrix4( const Matrix3 &m )
{
#define NUM_ROWS 4

	irr::core::matrix4 im = irr::core::matrix4( irr::core::matrix4::EM4CONST_NOTHING );
	
	const MRow *maxMPtr = m.GetAddr();
	float *irrMPtr = im.pointer();

	for ( int row = 0; row < NUM_ROWS; ++row )
	{
		irrMPtr[ row * NUM_ROWS + 0 ] = maxMPtr[row][0];
		irrMPtr[ row * NUM_ROWS + 1 ] = maxMPtr[row][1];
		irrMPtr[ row * NUM_ROWS + 2 ] = maxMPtr[row][2];
		irrMPtr[ row * NUM_ROWS + 3 ] = 0.f;
	}

	return im;

#undef NUM_ROWS
}