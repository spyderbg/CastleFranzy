#include "stdafx.h"
#include "UnitKind.h"
#include "Game/LevelInfo.h"
#include "Game/Objects/Unit.h"
#include "Game/Config/GameConfig.h"

UnitKind StringToUnitKind (const char* str)
{
	if (strcmp(str, "Goblin") == 0) return UK_Goblin;
	if (strcmp(str, "Boar") == 0) return UK_Boar;
	if (strcmp(str, "BoarRider") == 0) return UK_BoarRider;
	if (strcmp(str, "Shaman") == 0) return UK_Shaman;
	if (strcmp(str, "Troll") == 0) return UK_Troll;
	if (strcmp(str, "Elf") == 0) return UK_Elf;
	if (strcmp(str, "Skeleton") == 0) return UK_Skeleton;
	if (strcmp(str, "Bomblin") == 0) return UK_Bomblin;
	return UK_Unknown;
}

const char* UnitKindToString (UnitKind kind)
{
	switch (kind)
	{
	case UK_Goblin: return "Goblin";
	case UK_Boar: return "Boar";
	case UK_BoarRider: return "BoarRider";
	case UK_Shaman: return "Shaman";
	case UK_Troll: return "Troll";
	case UK_Elf: return "Elf";
	case UK_Skeleton: return "Skeleton";
	case UK_Bomblin: return "Bomblin";
	default: return "Unknown";
	}
}

static map<stringw, UnitParameters*> s_unitParameters;

void UnitParameters::Init (stringw name, IAttributes* attributes)
{
	soundsPackName = attributes->getAttributeAsStringW( "Sounds" );
	unitKind = StringToUnitKind(attributes->getAttributeAsString("Kind").c_str());

	modelId = attributes->getAttributeAsStringW("Model");
	modelAttr = CGameConfig::GetInstance()->getAttributes(L"model", modelId);
	modelFile = modelAttr->getAttributeAsString("File");

	chestDummy = modelAttr->getAttributeAsString("ChestDummy");
	dragDummy = modelAttr->getAttributeAsString("DragDummy");
	energyBallDummy = modelAttr->getAttributeAsString("EnergyBallDummy");

	scale = modelAttr->getAttributeAsFloat("Scale"); 

	shamanDieWaveModel = modelAttr->getAttributeAsString("ShamanDieWave");
	arrowMeshModel = modelAttr->getAttributeAsString("ArrowMesh");
	arrowMeshDummy = modelAttr->getAttributeAsString("ArrowDummy");

	if (modelAttr->existsAttribute("Rotation"))
		rotation = modelAttr->getAttributeAsQuaternion("Rotation");
	else
		rotation.makeIdentity();

	animations.clear();
	for (int i = 0; i < UA_TotalCount; ++i)
		animations.push_back("");

	animations[UA_Walk_Normal] =		modelAttr->getAttributeAsString("Anim_Walk_Normal");
	animations[UA_Walk_Injured] =		modelAttr->getAttributeAsString("Anim_Walk_Injured");
	animations[UA_Walk_Victory] =		modelAttr->getAttributeAsString("Anim_Walk_Victory");
	animations[UA_Walk_Fast] =			modelAttr->getAttributeAsString("Anim_Walk_Fast");
	animations[UA_Walk_Charge] =		modelAttr->getAttributeAsString("Anim_Walk_Charge");
	animations[UA_Dragged] =			modelAttr->getAttributeAsString("Anim_Dragged");
	animations[UA_DraggedMindControl] = modelAttr->getAttributeAsString("Anim_DraggedMindControl");
	animations[UA_Falling] =			modelAttr->getAttributeAsString("Anim_Falling");
	animations[UA_FlyingToFalling] =	modelAttr->getAttributeAsString("Anim_FlyingToFalling");
	animations[UA_Flying] =				modelAttr->getAttributeAsString("Anim_Flying");
	animations[UA_Stunned] =			modelAttr->getAttributeAsString("Anim_Stunned");
	animations[UA_Stumble] =			modelAttr->getAttributeAsString("Anim_Stumble");
	animations[UA_Attack] =				modelAttr->getAttributeAsString("Anim_Attack");
	animations[UA_Die_GroundFall] =		modelAttr->getAttributeAsString("Anim_Die_GroundFall");
	animations[UA_Die_GroundSmash] =	modelAttr->getAttributeAsString("Anim_Die_GroundSmash");
	animations[UA_Die_WallHit] =		modelAttr->getAttributeAsString("Anim_Die_WallHit");
	animations[UA_Riding_Walk] =		modelAttr->getAttributeAsString("Anim_Riding_Walk");
	animations[UA_Riding_Attack] =		modelAttr->getAttributeAsString("Anim_Riding_Attack");
	animations[UA_Riding_Dragged] =		modelAttr->getAttributeAsString("Anim_Riding_Dragged");
	animations[UA_Riding_Stunned] =		modelAttr->getAttributeAsString("Anim_Riding_Stunned");
	animations[UA_Riding_Victory] =		modelAttr->getAttributeAsString("Anim_Riding_Victory");
	animations[UA_TurnAround] =			modelAttr->getAttributeAsString("Anim_Turning");
	animations[UA_MindControl] =		modelAttr->getAttributeAsString("Anim_MindControl");
	animations[UA_ElfWallGrab] =		modelAttr->getAttributeAsString("Anim_Elf_WallGrab");
	animations[UA_ElfWallFall] =		modelAttr->getAttributeAsString("Anim_Elf_WallFall");
	animations[UA_ElfWallLand] =		modelAttr->getAttributeAsString("Anim_Elf_WallLand");
	animations[UA_ElfJump] =			modelAttr->getAttributeAsString("Anim_Elf_Jump");
	animations[UA_ElfUnhangHigh] =		modelAttr->getAttributeAsString("Anim_Elf_UnhangHigh");
	animations[UA_ElfUnhangLow] =		modelAttr->getAttributeAsString("Anim_Elf_UnhangLow");
	animations[UA_ElfSlide] =			modelAttr->getAttributeAsString("Anim_Elf_Slide");
	animations[UA_SkeletonAppear] =		modelAttr->getAttributeAsString("Anim_Skeleton_Appear");
	animations[UA_SkeletonLand]	=		modelAttr->getAttributeAsString("Anim_Skeleton_Land");
	animations[UA_Earthquake] =			modelAttr->getAttributeAsString("Anim_Earthquake");

	animationSpeeds.clear();
	for (int i = 0; i < UA_TotalCount; ++i)
		animationSpeeds.push_back(0.0f);

	animationSpeeds[UA_Walk_Normal] =		GetFloatWithDefault(modelAttr, "Speed_Walk", 1.0f);
	animationSpeeds[UA_Walk_Injured] =		GetFloatWithDefault(modelAttr, "Speed_Walk", 1.0f);
	animationSpeeds[UA_Walk_Victory] =		GetFloatWithDefault(modelAttr, "Speed_Walk", 1.0f);
	animationSpeeds[UA_Walk_Fast] =			GetFloatWithDefault(modelAttr, "Speed_WalkFast", 1.0f);
	animationSpeeds[UA_Walk_Charge] =		GetFloatWithDefault(modelAttr, "Speed_Charge", 1.0f);
	animationSpeeds[UA_Dragged] =			GetFloatWithDefault(modelAttr, "Speed_Dragged", 1.0f);
	animationSpeeds[UA_DraggedMindControl]=	GetFloatWithDefault(modelAttr, "Speed_Dragged", 1.0f);
	animationSpeeds[UA_Falling] =			GetFloatWithDefault(modelAttr, "Speed_Falling", 1.0f);
	animationSpeeds[UA_Flying] =			GetFloatWithDefault(modelAttr, "Speed_Flying", 1.0f);
	animationSpeeds[UA_FlyingToFalling] =	GetFloatWithDefault(modelAttr, "Speed_FlyingToFalling", 1.0f);
	animationSpeeds[UA_Stunned] =			GetFloatWithDefault(modelAttr, "Speed_Stunned", 1.0f);
	animationSpeeds[UA_Stumble] =			GetFloatWithDefault(modelAttr, "Speed_Stumble", 1.0f);
	animationSpeeds[UA_Attack] =			GetFloatWithDefault(modelAttr, "Speed_Attack", 1.0f);
	animationSpeeds[UA_Die_GroundFall] =	GetFloatWithDefault(modelAttr, "Speed_GroundFall", 1.0f);
	animationSpeeds[UA_Die_GroundSmash] =	GetFloatWithDefault(modelAttr, "Speed_Die", 1.0f);
	animationSpeeds[UA_Die_WallHit] =		GetFloatWithDefault(modelAttr, "Speed_Die", 1.0f);
	animationSpeeds[UA_Riding_Walk] =		GetFloatWithDefault(modelAttr, "Speed_Walk", 1.0f);
	animationSpeeds[UA_Riding_Attack] =		GetFloatWithDefault(modelAttr, "Speed_Riding_Attack", 1.0f);
	animationSpeeds[UA_Riding_Dragged] =	GetFloatWithDefault(modelAttr, "Speed_Dragged", 1.0f);
	animationSpeeds[UA_Riding_Stunned] =	GetFloatWithDefault(modelAttr, "Speed_Stunned", 1.0f);
	animationSpeeds[UA_Riding_Victory] =	GetFloatWithDefault(modelAttr, "Speed_Die", 1.0f);
	animationSpeeds[UA_TurnAround] =	    GetFloatWithDefault(modelAttr, "Speed_Die", 1.0f);
	animationSpeeds[UA_MindControl] =		GetFloatWithDefault(modelAttr, "Speed_MindControl", 1.0f);
	animationSpeeds[UA_ElfWallGrab] =		GetFloatWithDefault(modelAttr, "Speed_Elf_WallGrab", 1.0f);
	animationSpeeds[UA_ElfWallFall] =		GetFloatWithDefault(modelAttr, "Speed_Elf_WallFall", 1.0f);
	animationSpeeds[UA_ElfWallLand] =		GetFloatWithDefault(modelAttr, "Speed_Elf_WallLand", 1.0f);
	animationSpeeds[UA_ElfJump]	=			GetFloatWithDefault(modelAttr, "Speed_Elf_Jump", 1.0f);
	animationSpeeds[UA_ElfUnhangHigh] =		GetFloatWithDefault(modelAttr, "Speed_Elf_UnhangHigh", 1.0f);
	animationSpeeds[UA_ElfUnhangLow] =		GetFloatWithDefault(modelAttr, "Speed_Elf_UnhangLow", 1.0f);
	animationSpeeds[UA_ElfSlide] =			GetFloatWithDefault(modelAttr, "Speed_Elf_Slide", 1.0f);
	animationSpeeds[UA_SkeletonAppear] =	GetFloatWithDefault(modelAttr, "Speed_Skeleton_Appear", 1.0f);
	animationSpeeds[UA_SkeletonLand] =		GetFloatWithDefault(modelAttr, "Speed_Skeleton_Land", 1.0f);
	animationSpeeds[UA_Earthquake] =		GetFloatWithDefault(modelAttr, "Speed_Earthquake", 1.0f);

	statDistanceBeforeFallAnim = GetFloatWithDefault(modelAttr, "DistanceBeforeFallAnim", 0.0f);
	statStumbleDistance = GetFloatWithDefault(modelAttr, "StumbleDistance", 0.0f);

	shadowModel = modelAttr->getAttributeAsString("Shadow");

	auraDummyNodeName = modelAttr->getAttributeAsString("AuraDummy");
	auraDummyEffectName = modelAttr->getAttributeAsString("AuraEffect");
	auraDummyScale = modelAttr->getAttributeAsFloat("AuraScale");

	for (int i = 1; i < 100; ++i)
	{
		stringc file = stringc("TextureFile") + stringc(i);
		stringc object = stringc("TextureObject") + stringc(i);
		if (modelAttr->existsAttribute(file.c_str()))
		{
			textureObjects.push_back(modelAttr->getAttributeAsString(object.c_str()));
			textureFiles.push_back(modelAttr->getAttributeAsString(file.c_str()));
		}
		else
		{
			break;
		}
	}

	const char* damages[]   = { "EasyDamage",   "MediumDamage",   "HardDamage"   };
	const char* minSpeeds[] = { "EasyMinSpeed", "MediumMinSpeed", "HardMinSpeed" };
	const char* maxSpeeds[] = { "EasyMaxSpeed", "MediumMaxSpeed", "HardMaxSpeed" };
	for (int difficulty = 0; difficulty < 3; ++difficulty)
	{
		damageByDifficulty[difficulty] = attributes->getAttributeAsFloat(damages[difficulty]);
		minSpeedByDifficulty[difficulty] = attributes->getAttributeAsFloat(minSpeeds[difficulty]);
		maxSpeedByDifficulty[difficulty] = attributes->getAttributeAsFloat(maxSpeeds[difficulty]);
	}
	minRange = attributes->getAttributeAsFloat("RangeMin");
	maxRange = attributes->getAttributeAsFloat("RangeMax");
	souls = attributes->getAttributeAsFloat("Souls");
	mustDragIntoGroundToKill = attributes->getAttributeAsBool("MustDragIntoGroundToKill");
	noDeathFromFastFall = attributes->getAttributeAsBool("NoDeathFromFastFall");
	health = attributes->getAttributeAsFloat("Health");
	chargeHealth = attributes->getAttributeAsFloat("ChargeHealth");
	fallStun = attributes->getAttributeAsFloat("FallStun");
	fallDie	= attributes->getAttributeAsFloat("FallDie");
	thrownByBoar = !attributes->getAttributeAsBool("NotThrownByBoar");
	isFreeBoar = attributes->getAttributeAsBool("IsFreeBoar");
	isWildBoar = attributes->getAttributeAsBool("IsWildBoar");
	isMindControlled = attributes->getAttributeAsBool("IsMindControlled");
	gravity = GetFloatWithDefault(attributes, "Gravity", -9.8f * 8.0f * 0.9f);
	maxFallSpeedY = GetFloatWithDefault(attributes, "MaxFallSpeedY", 100.0f);
	charge = attributes->getAttributeAsFloat("Charge");
	chargeWalkMin = attributes->getAttributeAsFloat("ChargeWalkMin");
	chargeWalkMax = attributes->getAttributeAsFloat("ChargeWalkMax");
	fireImmunity = attributes->getAttributeAsBool("FireImmunity");
	iceImmunity = attributes->getAttributeAsBool("IceImmunity");

	mindControl = attributes->getAttributeAsStringW("MindControl");
	rider = attributes->getAttributeAsStringW("Rider");
}

void InitUnitParameters ()
{
	for (int i = 0; i < g_unitTypes.size(); ++i)
	{
		stringw name = g_unitTypes[i]->name;
		IAttributes* attr = g_unitTypes[i]->attributes;
		UnitParameters* params = new UnitParameters;
		params->Init(name, attr);
		s_unitParameters[name] = params;
	}
}

UnitParameters* GetUnitParameters (const stringw& id)
{
	return s_unitParameters.find(id)->getValue();
}
