#pragma once

#include "Game/Objects/GameObject.h"

#include "Core/irrlicht.h"

#include "CColladaMeshSceneNode.h"
#include "Game/ColladaControl.h"
#include "Sprites/ASpriteInstance.h"
#include "UnitParts/UnitKind.h"
#include "UnitParts/DamageSource.h"

class CBomb;
class ParticleSystem;
class CTouchElement;
class CProjectile;

typedef enum 
{
	US_Walking,
	US_Dragged,
	US_Falling,
	US_FallingOnHydra,
	US_FallingInWater,
	US_ToBeEeatenByHydra,
	US_BeingEatenByHydra,
	US_Petrified,
	US_Stunned,
	US_Stumble,
	US_Attacking,
	US_Dying,
	US_Dead,
	US_Victory,
	US_Riding,
	US_TurningAroundToRunAway,
	US_RunningAwayThrowingStuffAround,
	US_MindControlling,
	US_ElfWallGrab,
	US_ElfWallFall,
	US_SkeletonAppear,
	US_SkeletonLand,
	US_Tornado,
	US_Earthquake,
	US_RunningAway,
	US_None,
} EUnitState;

typedef enum
{
	UA_Walk_Normal,
	UA_Walk_Injured,
	UA_Walk_Victory,
	UA_Walk_Fast,
	UA_Walk_Charge,
	UA_Dragged,
	UA_DraggedMindControl,
	UA_Falling,
	UA_Flying,
	UA_FlyingToFalling,
	UA_Stunned,
	UA_Stumble,
	UA_Attack,
	UA_Die_GroundFall,
	UA_Die_GroundSmash,
	UA_Die_WallHit,	
	UA_Riding_Walk,
	UA_Riding_Attack,
	UA_Riding_Dragged,
	UA_Riding_Stunned,
	UA_Riding_Victory,
	UA_TurnAround,
	UA_MindControl,
	UA_ElfWallGrab,
	UA_ElfWallFall,
	UA_ElfWallLand,
	UA_ElfJump,
	UA_ElfUnhangHigh,
	UA_ElfUnhangLow,
	UA_ElfSlide,
	UA_SkeletonAppear,
	UA_SkeletonLand,
	UA_Earthquake,
	UA_TotalCount,
	UA_Invalid = -1,
} EUnitAnimation;

class ShamanAura;

class CUnit : public CGameObject
{
public:
	CUnit(UnitParameters* params, ISceneNode *root = NULL );
	virtual ~CUnit();

	virtual void		update(f32 time);
	virtual void		draw();

	void				setPath(const line3df & path);
	line3df &			getPath() { return m_path; }

	void				start();

	void				setState(EUnitState state);
	EUnitState			getState();

	void				setAnimation(EUnitAnimation anim, bool loop = false);

	bool				isAnimationOver ();

	bool				isDragable();
	void				startDrag(CTouchElement * touch);

	position2di			getScreenPosition();
	position2di			getDragPosition();

	float				damageUnit(f32 damage, EUnitDamageSource source); // returns the actual damage done
	void				healUnit (f32 health);

	void				throwOnHydra ();
	void				throwUnit (v3f velocity, bool harmless);
	void				throwUnitOnForTime (v3f pos, float flyingTime);
	void				throwUnitInWater (v3f pos, float time = 0.75);
	void				bleed (float scale);
	void				shamanExplode();
	void				trollStun(bool canKill = false);
	void				boarStun(bool canKill = false);

	v3f					getFallPosition (v3f vel);
	v3f					projectVectorToFlyingPlane (v3f vec);
	bool				isPositionOnPath (v3f pos, float tolerance = 1.0f);

	bool				canBeSmashedBy (EUnitDamageSource source);
	bool				canBeStunnedBy (EUnitDamageSource source);
	bool				canBeFrozenByIceball ();
	bool				canBeDamagedByFireball ();
	bool				canBeThrownByFreedBoar ();
	bool				canBeEatenByHydra ();
	bool				canBeEatenByHydraFromHand ();
	bool				canBeDamagedByShamanBall ();
	bool				canBeSlowedByIceball ();
	bool				canBeKilledByLightning ();
	bool				canBeTakenByTornado ();
	bool				canBeKilledByGhostArmy ();
	bool				canBeStunnedByEarthquake ();

	bool				canBePetrified();
	bool				isPetrified();
	void				petrify(f32 duration, bool stoned);
	void				unpetrify(bool kill);

	void				setMount (CUnit* mount);
	void				freeBoar ();
	CUnit*				getRider () { return m_rider; }
	void				setRider (CUnit* rider) { m_rider = rider; }

	void				setMindControlMaster (CUnit* master);
	void				freeFromMindControl ();

	void				freeAllSlavesFromMindControl ();

	void				setMPosition (v3f position);

	vector3df			getVelocity ();
	void				setVelocity (v3f velocity);

	int					m_lane;

	int					m_positionInList;
	int					m_id;

	UnitKind			getUnitKind ();

	void				ropePhysics (CUnit* connectedUnit);

	inline int			getSouls() { return m_statSouls; }

	void				elfJump (float power);

	inline ISceneNode*	getChestDummy() { return m_chestDummy; }

	int					getDragPriority ();

	void				setIntroUnit ();
	void				showIntroUnitInfo ();

	bool				isFreeBoar () { return m_statIsFreeBoar; }
	bool				isWildBoar () { return m_statIsWildBoar; }

	bool				isRedSkeleton () { return m_unitKind == UK_Skeleton && m_statFireImmunity; }
	bool				isMindControllingShaman () { return m_unitKind == UK_Shaman && !m_mindControlSlaves.empty(); }

	void				hideAura ();

	virtual void		UpdateShadow( f32 time );

protected:
	static void			eventsCallback (const collada::STriggeredEvent& ev, void* userData);
	static void			timelineCtrlCallback (ITimelineController* ctrl, void* self);

	virtual void		onGroundHit(f32 speed, bool dragged, bool fallfromhigh);
	virtual void		onWallHit(f32 speed, bool dragged);
	virtual void		onFortressReached();
	virtual void		onFreeFromMindControl ();

	bool				shouldShowStars ();

	void				shamanShoot ();
	void				elfShoot ();

	void				genericUnitSmash (int maxKills, float range, bool shake, EUnitDamageSource source);

	bool				m_harmlessFall;

	bool				m_thrownByBoar;

	UnitKind			m_unitKind;

	bool				m_isIntroUnit;

	stringw				m_SoundsPackName;

	stringc				m_animations[UA_TotalCount];
	float				m_animationSpeeds[UA_TotalCount];
	f32					m_statHealth;
	f32					m_statDamage;
	f32					m_statSpeed;
	f32					m_statFallStun;
	f32					m_statFallDie;
	f32					m_statRange;
	int					m_statSouls;
	bool				m_statMustDragIntoGroundToKill;
	bool				m_statNoDeathFromFastFall;
	bool				m_statIsFreeBoar;
	bool				m_statIsWildBoar;
	bool				m_statIsMindControlled;
	f32					m_statDistanceBeforeFallAnim;
	float				m_statStumbleDistance;
	f32					m_statCharge;
	f32					m_statChargeWalkMin;
	f32					m_statChargeWalkMax;
	f32					m_statChargeHealth;
	bool				m_statFireImmunity;
	bool				m_statIceImmunity;

	CUnit*				m_rider;
	CUnit*				m_mount;
	ISceneNode*			m_bip01;

	EUnitState			m_state;
	f32					m_stateTime;
	ParticleSystem*		m_bloodSystem;

	CProjectile*		m_projectile;

	EUnitAnimation		m_currentAnim;
	bool				m_currentAnimLoop;

	bool				m_brokenLink;
	float				m_randomAnimSpeed;

	bool				m_dieStartScaleGo;
	vector3df			m_dieStartScaleScale;
	vector3df			m_dieStartScalePos;
	vector3df			m_dieStartScaleChestPos;
	ISceneNode*			m_chestDummy;
	ISceneNode*			m_dragDummy;
	ISceneNode*			m_energyBallDummy;
	ISceneNode*			m_shamanStaffDummy;
	ISceneNode*			m_headDummy;
	CBomb*				m_bomb;

	float				m_maxFallSpeedY;

	f32					m_dustTime;

	f32					m_freezeDuration;

	line3df				m_path;
	plane3df			m_flyingPlane;

	float				m_gravity;
	vector3df			m_acceleration;
	vector3df			m_velocity;
	vector3df			m_rotationVelocity;
	vector3df			m_position;

	bool				m_charge;
	f32					m_chargeMinWalk;
	f32					m_chargeMaxDist;

	bool				m_fallingOnHydra;

	position2di			m_dragOffset;

	f32					m_health;
	EUnitDamageSource	m_lastDamageSource;
	bool				m_blood;

	CTouchElement *		m_touch;

	bool				m_isAnimationOver;
	bool				m_gaveMoney;

	quaternion			m_modelRotation;
	quaternion			m_startRotation;

	float				m_maxY;

	ShamanAura*			m_shamanAura;

	CUnit*				m_mindControlMaster;
	array<CUnit*>		m_mindControlSlaves;
	ASpriteInstance		m_shamanStaff;

	ASpriteInstance		m_stunnedStars;

	ColladaControl		m_shamanWave;

	int					m_unitsHit;
	int					m_unitsHitByKind[UK_Unknown + 1];

	bool				canBeStunnedByShaman ();

	bool				m_stoned;
	int					m_prerenderedFrame;

	int					m_catchTries;
	int					m_catchCounter;
	f32					m_catchTime;

	bool				m_ropePhysicsApplied;

	/// Shadows
protected:
	virtual void		RenderShadow();

protected:
	CColladaMeshSceneNode*		m_shadowMesh;
	ISceneNode*					m_shadow;
	array<vector3df>			m_shadowVert;

	stringc						m_arrowMesh;
	ISceneNode*					m_arrowDummy;

///////////////////////////////////////////////////////////////////////
/// Sounds
///////////////////////////////////////////////////////////////////////

private:
	virtual bool		PlayUnitSound( const char* soundName, bool loop = false );

///////////////////////////////////////////////////////////////////////
/// Creation
///////////////////////////////////////////////////////////////////////
private:
	void OptimizedCleanUnit (UnitParameters* params, ISceneNode* node);
	void OptimizedCleanShadow (UnitParameters* params, ISceneNode* node);
};
