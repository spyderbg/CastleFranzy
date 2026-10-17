#pragma once

#include "GameObject.h"
#include "Sprites/ASpriteInstance.h"

class ParticleSystem;
class CTouchElement;

class CProjectile : public CGameObject
{
public:
	CProjectile (ASprite* sprite, int anim, const char* diePs, float scale, vector3df pos, vector3df vel, vector3df accel, float damage);
	CProjectile (const char* ps, const char* diePs, float scale, vector3df pos, vector3df vel, vector3df accel, float damage);
	CProjectile (const char* mesh, float damage, const char* diePs, float diePsScale);
	~CProjectile ();

	void update (f32 time);
	void draw ();

	bool isDead ();
	void setDead ();

	static bool s_antiFireballArrowShot;
	static bool s_antiIceballArrowShot;
	static bool s_antiGriffinArrowShot;

	static void reset ();

	void setAntiFireball (v3f target);
	void setAntiIceball (v3f target);
	void setAntiGriffin (v3f target);
	
	CTouchElement* getTouch ();

	void goElf ();

	void setActive ();

	vector3df m_velocity;
	vector3df m_acceleration;
	float m_scale;

protected:
	void onGroundHit ();
	void onCastleHit ();
	void onTargetHit ();

private:
	const char* m_diePs;
	ParticleSystem* m_ps;
	ASpriteInstance m_inst;
	float m_damage;
	bool m_dead;
	bool m_empty;
	bool m_antiFireball;
	bool m_antiIceball;
	bool m_antiGriffin;
	v3f m_target;
	bool m_active;
	v3f m_startPos;
	CTouchElement* m_touch;
	bool m_isElfArrow;
};
