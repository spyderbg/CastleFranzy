#pragma once

#include "Core/irrlicht.h"
#include "Game/ColladaControl.h"
#include "IO/TouchScreen/TouchPad.h"

class CTouchElement;

class CBomb : public ColladaControl
{
public:
	CBomb (f32 damage, ISceneNode *rootobj=NULL );
	~CBomb();

	void		update (f32 time);
	void		draw2D ();
	void		draw3D ();

	bool		isDead ();
	void		explode (bool forceDamageCastle);
	void		explodeLater ();
	void		dropOnGround (v3f scale);

	bool		isFree ();

	v3f			getVelocity ();
	void		setVelocity (v3f velocity);

	bool		isForHelpMenu();

private:
	ISceneNode *m_sceneroot;
	bool		m_free;
	f32			m_damage;
	bool		m_dead;
	float		m_time;
	CTouchElement m_touch;
	v3f			m_velocity;
	bool		m_isForHelpMenu;
};
