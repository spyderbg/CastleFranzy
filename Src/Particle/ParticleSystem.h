#pragma once

#include "Core/irrlicht.h"

class ParticleSystem : public irr::IReferenceCounted
{
public:
	ParticleSystem();
	ParticleSystem(const char *filename);
	virtual ~ParticleSystem();
	
	void Update(f32 timestep);
	void SetTimeToLive(f32 timeToLive);
	void UnloadParticleSystem();
	void LoadParticleSystem(const char *filename);
	bool IsAlive();

	void SetPosition (const irr::core::vector3df& v);
	void SetScale (float scale);
	void SetRotation (const quaternion& rotation);
	void SetZBuffer (bool enabled);
	void SetFogEnabled (bool enabled);

	float GetScale ();
	quaternion GetRotation ();

	void Pause();
	void UnPause();
	void Restart();

	ISceneNode* getNode ()
	{
		return m_node;
	}
	
protected:
	irr::scene::ISceneNode* m_node;
	f32 m_timeToLive;
	f32 m_lifeTime;
};
