#pragma once

#include "Core/irrlicht.h"

class ParticleSystem;

class ShamanAura
{
public:
	ShamanAura (const stringc& name, float scale, ISceneNode* dummy);
	~ShamanAura ();

	void update (float secs);
	void draw ();

	void hide ();
	void show ();

	void hideImmediately ();

private:
	stringc			m_effectName;
	float			m_effectScale;
	bool			m_hiding;
	float			m_currentScale;
	ParticleSystem* m_particleSystem;
	ISceneNode*		m_dummy;
};
