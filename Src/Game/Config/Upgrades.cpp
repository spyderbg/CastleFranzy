////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////

#include "Game/Config/Upgrades.h"
#include "Game/Config/GameConfig.h"
#include "Game/Playfield.h"

#include "Common/StringTable.h"

////////////////////////////////////////////////////////

IAttributes * SUpgradeInfo::getAttributes() const
{
	return CGameConfig::GetInstance()->getAttributes(UpgradeType, UpgradeID);
}

int SUpgradeInfo::getPriceGold() const
{
	const char* price[] = { "PriceGoldEasy", "PriceGoldMedium", "PriceGoldHard" };
	int difficulty = (int)(CPlayfield::GetInstance()->getDifficulty());
	return getAttributes()->getAttributeAsInt( price[difficulty] );
}

int SUpgradeInfo::getPriceSouls() const
{
	const char* price[] = { "PriceSoulsEasy", "PriceSoulsMedium", "PriceSoulsHard" };
	int difficulty = (int)(CPlayfield::GetInstance()->getDifficulty());
	return getAttributes()->getAttributeAsInt( price[difficulty] );
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

SUpgradeInfo WEAPON_LEVEL_INFO[WT_Count][WL_Count] = 
{
	/// Gate
	{
		{ _TXT_ID(WEAPON_DOOR_NAME_L1), _TXT_ID(WEAPON_DOOR_DESC_L1), "gate", "Gate02" },
		{ _TXT_ID(WEAPON_DOOR_NAME_L2), _TXT_ID(WEAPON_DOOR_DESC_L2), "gate", "Gate03" },
		{ _TXT_ID(WEAPON_DOOR_NAME_L3), _TXT_ID(WEAPON_DOOR_DESC_L3), "gate", "Gate04" },
	},

	/// Hydra
	{
		{ _TXT_ID(WEAPON_HYDRA_NAME_L1), _TXT_ID(WEAPON_HYDRA_DESC_L1), "weapon", "Hydra_1" },
		{ _TXT_ID(WEAPON_HYDRA_NAME_L2), _TXT_ID(WEAPON_HYDRA_DESC_L2), "weapon", "Hydra_2" },
		{ _TXT_ID(WEAPON_HYDRA_NAME_L3), _TXT_ID(WEAPON_HYDRA_DESC_L3), "weapon", "Hydra_3" },
	},

	/// Fireball
	{
		{ _TXT_ID(WEAPON_FIREBALL_NAME_L1), _TXT_ID(WEAPON_FIREBALL_DESC_L1), "weapon", "Fireball_1" },
		{ _TXT_ID(WEAPON_FIREBALL_NAME_L2), _TXT_ID(WEAPON_FIREBALL_DESC_L2), "weapon", "Fireball_2" },
		{ _TXT_ID(WEAPON_FIREBALL_NAME_L3), _TXT_ID(WEAPON_FIREBALL_DESC_L3), "weapon", "Fireball_3" },
	},

	/// Iceball
	{
		{ _TXT_ID(WEAPON_ICEBALL_NAME_L1), _TXT_ID(WEAPON_ICEBALL_DESC_L1), "weapon", "Iceball_1" },
		{ _TXT_ID(WEAPON_ICEBALL_NAME_L2), _TXT_ID(WEAPON_ICEBALL_DESC_L2), "weapon", "Iceball_2" },
		{ _TXT_ID(WEAPON_ICEBALL_NAME_L3), _TXT_ID(WEAPON_ICEBALL_DESC_L3), "weapon", "Iceball_3" },
	},

	/// Griffin
	{
		{ _TXT_ID(WEAPON_GRIFFIN_NAME_L1), _TXT_ID(WEAPON_GRIFFIN_DESC_L1), "weapon", "Griffin_1" },
		{ _TXT_ID(WEAPON_GRIFFIN_NAME_L2), _TXT_ID(WEAPON_GRIFFIN_DESC_L2), "weapon", "Griffin_2" },
		{ _TXT_ID(WEAPON_GRIFFIN_NAME_L3), _TXT_ID(WEAPON_GRIFFIN_DESC_L3), "weapon", "Griffin_3" },
	},
};

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

SUpgradeInfo SPELL_INFO[ST_Count + 1] = 
{
	{ _TXT_ID(WEAPON_BOULDER_NAME),		_TXT_ID(WEAPON_BOULDER_DESC),	"spell", "None" },
	{ _TXT_ID(WEAPON_BOULDER_NAME),		_TXT_ID(WEAPON_BOULDER_DESC),	"spell", "Boulder" },
	{ _TXT_ID(WEAPON_LIGHTNING_NAME),	_TXT_ID(WEAPON_LIGHTNING_DESC),	"spell", "Lightning" },
	{ _TXT_ID(WEAPON_GHOST_NAME),		_TXT_ID(WEAPON_GHOST_DESC),		"spell", "GhostArmy" },
	{ _TXT_ID(WEAPON_TORNADO_NAME),		_TXT_ID(WEAPON_TORNADO_DESC),	"spell", "Tornado" },
};

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

const SUpgradeInfo& GetWeaponInfo(EWeaponType weapon, int level)
{
	if (weapon == WT_Spell)
		return SPELL_INFO[level+1];
	else
		return WEAPON_LEVEL_INFO[weapon][level];
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////