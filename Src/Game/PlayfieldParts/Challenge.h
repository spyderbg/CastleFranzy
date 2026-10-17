#pragma once

#include "Game/Objects/UnitParts/UnitKind.h"

enum ChallengeType
{
	CT_None,
	CT_PerfectHealth,
	CT_GoblinWallSmash,
	CT_ShamanWallSmash,
	CT_BoarRampage,
	CT_GriffinFireballMaster,
	CT_GriffinFireballSuperMaster,
	CT_FlickMaster,
	CT_OneHitOneKill, // cancelled
	CT_ManyEnemiesWithSpecial,
	CT_ThatWasClose,
	CT_WallSmasher,
	CT_GroundSmasher,
	CT_MenuGoblins,
	CT_MenuTrolls,
	CT_MenuShamans,
	CT_ShamanCombo,
	CT_TrollCombo,
	CT_ComboMaster,
	CT_Count,
	CT_Force32Bit = 0x7fffffff
};

ChallengeType StringToChallengeType (const wchar_t* text);
const wchar_t* ChallengeTypeToString (ChallengeType type);

class Challenge
{
public:
	Challenge ();

	ChallengeType getType ();
	void setType (ChallengeType type);

	void gateDamaged ();
	void unitDied (int damageSource, UnitKind kind);
	void levelEnded (bool maxHealth, bool criticalHealth, bool win);
	void unitWasHitByBoar (int* unitsHitByKind);
	void unitsWereKilledByWeapon (int damageSource, int* unitsHitByKind);
	void specialWeaponUsed ();
	void hydraHasEatenUnit (UnitKind kind);
	void comboAchieved (UnitKind kind);
	void update (float time);
	void draw ();

	const TChar*		getDescription ();

	bool isChallengeCompleted() { return m_challengeComplete && !m_failed; }

	bool isVisible()			{ return m_scrollingTitle != 0; }
private:
	ChallengeType		m_type;

	const TChar*		m_scrollingTitle;
	float				m_scrollingTimer;
	void				completeChallenge ();
	void				failChallenge ();

	UnitKind			m_wallSmashKind;
	int					m_wallSmashCount;
	int					m_wallSmashGoal;

	bool				m_boarRampageComplete;

	bool				m_specialWeaponUsed;

	bool				m_challengeComplete;
	bool				m_failed;

	int					m_enemiesKilledBySpecial;
	int					m_enemiesKilledOnWall;
	int					m_enemiesKilledOnGround;
	int					m_combos;

	int					m_enemiesEaten;
};
