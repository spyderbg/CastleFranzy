#pragma once

#include "Core/irrlicht.h"
#include "Particle/ParticleSystem.h"
#include "Particle/ParticlePreset.h"

void ClearParticleSystem();

ParticleSystem* AddParticleSystem(const char *typeName);
void UpdateParticleSystem(f32 timestep);

int LoadParticlePresets();
void RemoveParticlePresets();
void SetObjectParticleType(irr::scene::ISceneNode* node , irr::core::stringw particleType);
bool IsParticleSystemAlive(ParticleSystem* particle);

void RemoveParticleSystem(ParticleSystem* particle);
