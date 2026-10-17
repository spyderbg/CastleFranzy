///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

#include "Common/Singleton.h"

#include "Game/Config/LevelStats.h"
#include "Game/Config/UserProfile.h"
#include "Game/Objects/GameObject.h"
#include "Game/Objects/UnitParts/UnitKind.h"
#include "Game/Objects/Weapons/CastleGate.h"
#include "Game/LevelInfo.h"

#include "PlayfieldParts/Challenge.h"

#include "IColladaSceneNodeAnimator.h"

///////////////////////////////////////////////////////////////////////

class CUnit;
class CGameCamera;
class CTouchElement;
class CProjectile;
class ParticleSystem;

class CCastleGate;
class CHydra;
class CFireball;
class CIceball;
class CGriffin;
class CSpell;
class CBomb;

class ASprite;
class ASpriteInstance;
class CSpriteInstance;

class Menu_Ingame;

///////////////////////////////////////////////////////////////////////

typedef core::list<CUnit*> TUnitList;

///////////////////////////////////////////////////////////////////////

struct SEffect
{
	ASpriteInstance* inst;
	vector3df pos;
};

struct SMoney
{
	int amount;
	int bonus;
	vector2df position;
	const wchar_t* text;
	float time;
};

struct SSouls
{
	int amount;
	v3f start;
	float time;
	float duration;
	ASpriteInstance* inst;
};

struct SUnitAtTime
{
	int type;
	float time;
	int slaves;
	int riders;
	bool intro;
};

bool operator< (const SUnitAtTime& u1, const SUnitAtTime& u2);

typedef enum
{
	GD_Easy,
	GD_Normal,
	GD_Hard,
	GD_Num
} GameDifficulty;


///////////////////////////////////////////////////////////////////////
/// CPlayfield
///////////////////////////////////////////////////////////////////////

struct UnitTypeAndId
{
	int unitType;
	float time;
	int id;
};

class CPlayfield : public CSingleton<CPlayfield>
{
public:
	typedef enum
	{
		PS_None,
		PS_CutScene,
		PS_IntroUnitCamera,
		PS_Begin,
		PS_Play,
		PS_Pause,
		PS_Restart,

		PS_MissionOverTimer, // after this are mission over states
		PS_MissionOver,
		PS_Shop,

		PS_AfterInterrupt,

		PS_EnterIGM,
		PS_IGM,
		PS_ExitIGM,

		PS_Num
	} PlayState;

private:
	f32	_time;

public:
	CPlayfield();
	virtual ~CPlayfield();

	virtual void		update(f32 time);
	virtual void		draw();
	virtual void		draw2D();

private:
			void		_gameUpdate(f32 time );

public:

	void				DrawFade();

	void				Update_MissionOverCheck();
	void				Update_MissionOverTimer();	
	void				Update_MissionOver();
	void				Update_Shop();
	void				Update_CutScene();
	void				Update_IGM();

	void				pauseUnits ();
	void				resumeUnits ();

	CUnit*				findShaman ();

	CUnit*				spawnUnit(const stringw & id, int slaves, float time, int unitId, bool intro);

	void				addSpark (vector3df pos);
	void				addWalkDust (vector3df pos);
	void				addFallDust (vector3df pos);
	void				addDie (vector3df pos);

	void				addBomb (CBomb* bomb);
	void				addProjectile (CProjectile* projectile);

	bool				isValid () { return m_castle != 0; }

	inline const plane3df&		getCastlePlane() { return m_castlePlane; }
	inline const plane3df&		getFlyingPlane() { return m_flyingPlane; }
	inline const plane3df&		getGroundPlane() { return m_groundPlane; }
	inline const plane3df&		getOutOfScreenPlane() { return m_outOfScreenPlane; }

	line3df				getPath () const { return m_path; }
	const TUnitList&	getUnits () const;

	void				ClearProjectiles ( bool exit_level = false );

	inline CLevelStats&	GetLevelStats() { return m_levelStats; }

	void				SetCameraShake (float time, float power);
	void				addMoney (int amount, int bonus, v3f position, const wchar_t* text = 0, float time = 2.0f);
	void				addMoney (int amount, int bonus, v2f position, const wchar_t* text = 0, float time = 2.0f);
	void				addSouls(int amount, v3f position);

	bool				checkCombo(int damageSource, UnitKind kind, int * money, const wchar_t ** text);

	ISceneNode*			getDummy (const char* name);

	void				checkHydraEatUnitFromHand(f32 time, CUnit* unit);

	float				getTime ();
	const array<SUnitAtTime>& getWaves ();
	int					getWavePos ();

	/// Gate
	inline CCastleGate*	getGate()		{ return m_gate; }

	/// Weapons
	inline CHydra*		getHydra()		{ return m_hydra; }
	inline CFireball*	getFireball()	{ return m_fireball; }
	inline CIceball*	getIceball()	{ return m_iceball; }
	inline CGriffin*	getGriffin()	{ return m_griffin; }
	inline CSpell*		getSpell()		{ return m_spell; }

	/// Camera
	inline ICameraSceneNode*	getCamera() { return m_camera; }

	Challenge*				getChallenge ();

	inline ASprite*			getSpriteIngame() { return m_sprIngame; }
	inline ASprite*			getSpriteStonedUnits() { return m_sprStonedUnits; }
	inline ASprite*			getSpriteIcedUnits() { return m_sprIcedUnits; }
	inline ASprite*			getSpriteEffects() { return m_sprEffects; }
	inline ASprite*			getSpriteDizzy() { return m_sprDizzy; }

	array<UnitTypeAndId>	m_extraSpawns;	

	float				m_damage;

	const array<CBomb*>& getBombs ();

	void				showIntroUnitInfo (CUnit* unit);

	/// Dificulty
private:		
	GameDifficulty		m_difficulty;

public:
	GameDifficulty		getDifficulty () { return m_isEndless ? GD_Hard : m_difficulty; }
	void				setDifficulty( GameDifficulty difficulty )	{ m_difficulty = difficulty; }

public:
	bool				isChallengeVisible() { return m_challenge.isVisible(); }

	void				SetEndlessScores (Menu_Ingame* menu);
	void				SetLevelScores (Menu_Ingame* menu);
	int					m_kills;
	
	UnitKind			m_comboUnitKind;

private:
	static void			cameraAnimEvents(const collada::STriggeredEvent &event, void *userData);
	static void			cameraClipEndCallback(ITimelineController*, void *);

	bool				checkUnitTouch(CUnit * unit);

	void				updateEffects();
	void				calc3D();
	void				loadWaves ();
	bool				canSpawnMoreUnits (int count);
	void				updateWaves ();

	int					findFreeLane ();
	void				allocateLane (int lane);
	void				freeLane (int lane);

	int					m_unitsInLane[8];

	void				updateIntroUnitCamera (f32 time);
	void				DrawIntroUnitCamera ();
	f32					m_introTime;
	CUnit*				m_introUnit;
	v3f					m_introUnitStartPos;
	quaternion			m_introUnitStartRot;
	CTouchElement*		m_backTouch;
	bool				m_introGoingBack;

	CUnit*				m_unitToEat;
	bool				m_unitHoveredOverHydra;
	float				m_unitHoveredTimer;

	TUnitList			m_units;
	line3df				m_path;
	plane3df			m_castlePlane;
	plane3df			m_flyingPlane;
	plane3df			m_groundPlane;
	plane3df			m_outOfScreenPlane;

	array<float>		getEndlessOdds (int day);

	int					m_lastDamageSource;
	int					m_currentBonus;

	bool				m_playChallengePopupSound;
	Challenge			m_challenge;

	bool				m_unitDragged;

	float				m_cameraShakeTimer;
	float				m_cameraShakePower;
	int					m_cameraShakeX;
	int					m_cameraShakeY;

	ICameraSceneNode*	m_camera;
	ISceneNode*			m_castle;
	ISceneNode*			m_cameraScene;
	ISceneNode*			m_castleBackground;
	ISceneNode*			m_castleForeground;

	CCastleGate*		m_gate;
	CHydra*				m_hydra;
	CFireball*			m_fireball;
	CIceball*			m_iceball;
	CGriffin*			m_griffin;
	CSpell*				m_spell;
	

	ASprite*			m_spriteSparks;
	ASprite*			m_spriteDust;
	ASprite*			m_spriteDie;

	s32					m_buttonClicked;

	array<SMoney>		m_flyingMoney;
	array<SSouls>		m_flyingSouls;
	array<SEffect>		m_effects;
	array<CProjectile*> m_projectiles;
	array<CBomb*>		m_bombs;

	ASprite*			m_sprIngame;
	ASprite*			m_sprStonedUnits;
	ASprite*			m_sprIcedUnits;
	ASprite*			m_sprEffects;
	ASprite*			m_sprDizzy;
	ASpriteInstance*	m_playPauseIcon;

	int					m_gold;
	float				m_slowGold;
	int					m_souls;
	float				m_slowSouls;

	float				m_hudAlphaTarget;
	float				m_hudAlpha;

	float				m_time;

	array<SUnitAtTime>	m_waves;
	int					m_wavePos;

	f32					m_missionOverCountdown;

	CLevelStats			m_levelStats;
	f32					m_dayTextTime;


///////////////////////////////////////////////////////////////////////
/// Fade
///////////////////////////////////////////////////////////////////////

public:
	void				fadeIn(f32 duration);
	void				fadeOut(f32 duration);
	inline bool			isFading() { return m_fadeDir != 0; }

private:
	void				UpdateFade(f32 time);
	int					m_fadeDir;
	f32					m_fadeDuration;
	f32					m_fadeTimer;
	int					m_fadeAlpha;	


///////////////////////////////////////////////////////////////////////
/// Cutscenes
///////////////////////////////////////////////////////////////////////

public:
	typedef enum
	{
		CS_Intro,
		CS_Hydra,
		CS_Hydra3Heads,
		CS_Griffin,
		CS_HydraGriffin,

		CS_Count,
		CS_Invalid = -1,
	} CutScene;

	void				SetNextCutscene(CutScene cutScene);

	bool				m_lightball;

private:
	void				StartCutscene();
	void				EndCutscene();
	CutScene			m_nextCutscene;


///////////////////////////////////////////////////////////////////////
/// Music system
///////////////////////////////////////////////////////////////////////

public:	
	void				PauseMusic();
	void				ResumeMusic();

	s32					GetMusic();
	void				SetMusic( s32 musicId, bool loop=false );
	void				SetNextMusic( s32 nextMusicId, bool loop=false );

protected:
	inline	bool		IsMusicIdCorrect( s32 musicId ) { return ( musicId >= 0 ); }

private:
	void				UpdateMusic();			

private:
	bool				m_MusicPause;
	s32					m_CurrentMusic;
	s32					m_PrevMusic;
	s32					m_NextMusic;
	s32					m_NextMusicLoop;
	

///////////////////////////////////////////////////////////////////////
/// Loading system
///////////////////////////////////////////////////////////////////////

public:
	inline	s32			GetProgress()	{ return m_LoadingProgress; }
	inline	bool		IsLevelLoaded() { return GetProgress() >= 102; }
			void		ReleaseLevel();
			void		ResetLevel();
			void		LoadLevel( f32 time );
			void		DrawLoadingBar();
			
	const	char*		GetLevelFieldStr( GameDifficulty gd ) const;
			s32&		UnlockLevel( GameDifficulty gd );

			void		ReadProfileStats();
			void		WriteProfileStats();

			void		StartNewLevel( s32 level );
			
			s32			getUnlockLevel( GameDifficulty gd );
public:
			s32			m_unlockLevelEasy;
			s32			m_unlockLevelNormal;
			s32			m_unlockLevelHard;
			s32			m_CurrentLevel;
			s32			m_CurrentDay;
			s32			m_EnterInShop;
			bool		m_isEndless;

private:
			s32			m_LoadingProgress;
			ASprite*	m_LoadingSprite;


///////////////////////////////////////////////////////////////////////
/// Day time
///////////////////////////////////////////////////////////////////////

public:
	inline	f32				GetDayTime() { return m_DayTime; }
	const core::vector3df	GetSunPos() const;
	ILightSceneNode*		GetSunLight() { return m_SceneLight; }

private:
	void					UpdateDayTime();
	void					SetDayTime( f32 day_time );

private:	
	s32						m_MaxUnits;
	f32						m_DayTime;
	ILightSceneNode*		m_SceneLight;


///////////////////////////////////////////////////////////////////////
/// Pause system
///////////////////////////////////////////////////////////////////////

public:
	inline	bool		isPaused()		{ return	GetPlayState() == PS_Pause || 
													GetPlayState() == PS_EnterIGM ||
													GetPlayState() == PS_IGM ||
													GetPlayState() == PS_ExitIGM; }

			void		pause();
			void		resume();
private:
			bool		isPauseButtonPressed();


///////////////////////////////////////////////////////////////////////
/// Play state
///////////////////////////////////////////////////////////////////////

public:
	inline	bool		IsMissionOver()		{ return GetPlayState() > PS_MissionOverTimer; }
	inline	bool		IsMissionSuccess()	{ return m_MissionSuccess; }
	inline	bool		IsLevelOver()		{ return m_isLevelOver; }

//protected:
	inline	PlayState	GetPlayState() { return m_PlayState; }
			void		SetPlayState( PlayState state );

private:
	PlayState			m_PlayState;


///////////////////////////////////////////////////////////////////////
/// Win lose mission
///////////////////////////////////////////////////////////////////////

private:
			void		SuccessMission();
			void		FaildMission();

			bool		m_MissionSuccess;
			bool		m_isLevelOver;


///////////////////////////////////////////////////////////////////////
/// Music system
///////////////////////////////////////////////////////////////////////

protected:
	
	struct SSfxInfo
	{
		s32 id;
		s32 numPlayingInstances;
		std::vector<s32> soundIndices;
		s32 priority;
		s32 maxPlayingInstances;
		f32 volume;
	};

	typedef std::map<s32, SSfxInfo*>	SfxList;
	typedef SfxList::iterator	SfxListIter;

public:

	s32 PlaySfx(s32 soundId, bool loop = false, s32 fadeInTime = 0, s32 decreasePriority = 0);
	void StopSfx(int soundId, int soundIndex = 0,  int fadeOutTime = 0);

protected:

	void UpdateSfx();
	bool CheckSoundPriority(SSfxInfo* info, s32 decreasePriority = 0);
	void StopFistSfxInstance(int soundId, int fadeOutTime);

protected:
	SfxList			m_sfxList;
	s32				m_totalPlayingInstances;


///////////////////////////////////////////////////////////////////////
/// First game
///////////////////////////////////////////////////////////////////////

public:	
			bool		isFirstGame() { return m_gameNumber == 0; }

private:
			s32			m_gameNumber;


///////////////////////////////////////////////////////////////////////
/// Game touch system
///////////////////////////////////////////////////////////////////////

public:
	CTouchElement*		getTouch();

	void				EnableIngameTouchInput();
	void				DisableIngameTouchInput();

	bool				isNowActivatedTouch();
	bool				isNowReleasedTouch();
	bool				isActiveTouch();
	void				reassignTouchTo(CTouchElement* touch);

private:
	void				updateTouch();

private:
	CTouchElement*		m_touch;
	bool				m_firstTouch;
	bool				m_secondTouch;
	bool				m_isNowActivated;
	bool				m_isNowReleased;


///////////////////////////////////////////////////////////////////////
/// Prerender system
///////////////////////////////////////////////////////////////////////

public:
	void				hideBackground();
	void				showBackground();
	void				hideForeground();
	void				showForeground();

	void				prerenderBackground();
private:
	void				initCamera();
	vector3df			m_cameraPosition;

	void				drawPrerenderedBackground();
	void				drawFakeZBuffer();

	ITexture*			m_prerenderedTexture;

};
