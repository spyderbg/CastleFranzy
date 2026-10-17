#pragma once

enum UnitKind
{
	UK_Goblin,
	UK_Boar,
	UK_BoarRider,
	UK_Shaman,
	UK_Troll,
	UK_Elf,
	UK_Skeleton,
	UK_Bomblin,
	UK_Unknown
};

UnitKind StringToUnitKind (const char* str);
const char* UnitKindToString (UnitKind kind);

// ----------------------------------------------------------------------------
struct UnitParameters
{
	void Init (stringw name, IAttributes* attributes);

	stringw					soundsPackName;
	UnitKind				unitKind;
	stringw					modelId;
	IAttributes*			modelAttr;
	stringc					chestDummy;
	stringc					dragDummy;
	stringc					energyBallDummy;
	stringc					modelFile;
	float					scale;
	stringc					shamanDieWaveModel;
	stringc					arrowMeshModel;
	stringc					arrowMeshDummy;
	quaternion				rotation;
	array<stringc>			animations;
	array<float>			animationSpeeds;
	float					statDistanceBeforeFallAnim;
	float					statStumbleDistance;
	stringc					shadowModel;
	stringc					auraDummyNodeName;
	stringc					auraDummyEffectName;
	float					auraDummyScale;
	array<stringc>			textureObjects;
	array<stringc>			textureFiles;
	float					damageByDifficulty[3];
	float					minSpeedByDifficulty[3];
	float					maxSpeedByDifficulty[3];
	float					minRange;
	float					maxRange;
	float					souls;
	bool					mustDragIntoGroundToKill;
	bool					noDeathFromFastFall;
	float					health;
	float					chargeHealth;
	float					fallStun;
	float					fallDie;
	bool					thrownByBoar;
	bool					isFreeBoar;
	bool					isWildBoar;
	bool					isMindControlled;
	float					gravity;
	float					maxFallSpeedY;
	float					charge;
	float					chargeWalkMin;
	float					chargeWalkMax;
	bool					fireImmunity;
	bool					iceImmunity;
	stringw					mindControl;
	stringw					rider;
};

void InitUnitParameters ();
UnitParameters* GetUnitParameters (const stringw& id);
