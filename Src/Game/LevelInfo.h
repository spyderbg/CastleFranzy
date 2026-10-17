#pragma once

#include "Core/irrlicht.h"
#include "Game/PlayfieldParts/Challenge.h"
#include "Game/Config/Upgrades.h"

struct EnemyType
{
	int unitType;
	int minCount;
	int maxCount;
};

struct Variant
{
	array<EnemyType*> enemyTypes;
	bool intro;

	~Variant()
	{
		for (int i = 0; i < enemyTypes.size(); i++)
			delete enemyTypes[i];
	}
};

struct Wave
{
	stringw name;
	int timeStart;
	int timeInterval;
	int repeatInterval;
	int repeatCount;
	bool packed;
	array<Variant*> variants;

	~Wave()
	{
		for (int i = 0; i < variants.size(); i++)
			delete variants[i];
	}
};

struct Day
{
	int dayEndTime;
	ChallengeType challenge;
	int difficulty;
	array<Wave*> waves;

	~Day()
	{
		for (int i = 0; i < waves.size(); i++)
			delete waves[i];
	}
};

struct Level
{
	EWeaponLevel	weaponStartingLevel[WT_Count - 1];
	bool			weaponAvailableInShop[WT_Count - 1];
	ESpellType		spellAvailableInShop; 
	bool			spellBought;
};

struct UnitType
{
	stringw name;
	IAttributes* attributes;
};

extern array<UnitType*> g_unitTypes;

void initUnitTypes ();
int stringToUnitType (const stringw& str);
stringw unitTypeToString (int unitType);

stringc getDayPath (int level, int day);
void saveDayInfo (const char* path, Day* level);
Day* loadDayInfo (const char* path);

stringc getLevelPath (int level);
void   saveLevelInfo (const char* path, Level* level);
Level* loadLevelInfo (const char* path);

const array<int>& getNumberOfDaysPerLevel ();
void saveNumberOfDaysPerLevel (const array<int>& arr);

Wave* cloneWave (Wave* wave);

// returns the new level max time
int appendDayToDay (Day* inOut, int time, Day* dayToAppend);

int getDayMaxTime (Day* level);

Day* cloneDay (Day* level);