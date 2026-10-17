#include "stdafx.h"
#include "LevelInfo.h"
#include "Game/Config/GameConfig.h"
#include <stack>
#include "Utils/StringUtils.h"

static IXMLReader* s_reader = 0;
static std::stack<stringw> s_xmlStack;

array<UnitType*> g_unitTypes;

void readBeginElement (const wchar_t* element)
{
	s_xmlStack.push(element);
	while (s_reader->read())
	{
		if (s_reader->getNodeType() == EXN_ELEMENT && wcscmp(s_reader->getNodeName(), element) == 0)
			break;
	}
}

bool readBeginElementOrEndParent (const wchar_t* element)
{
	while (s_reader->read())
	{
		if (s_reader->getNodeType() == EXN_ELEMENT && wcscmp(s_reader->getNodeName(), element) == 0)
		{
			s_xmlStack.push(element);
			return true;
		}
		if (s_reader->getNodeType() == EXN_ELEMENT_END && wcscmp(s_reader->getNodeName(), s_xmlStack.top().c_str()) == 0)
		{
			s_xmlStack.pop();
			return false;
		}
	}
	return false;
}

void readSimpleElement (const wchar_t* element, int* value)
{
	while (s_reader->read())
	{
		if (s_reader->getNodeType() == EXN_ELEMENT && wcscmp(s_reader->getNodeName(), element) == 0)
			break;
	}
	*value = s_reader->getAttributeValueAsInt(L"value");
}

void readSimpleElement (const wchar_t* element, stringw* value)
{
	while (s_reader->read())
	{
		if (s_reader->getNodeType() == EXN_ELEMENT && wcscmp(s_reader->getNodeName(), element) == 0)
			break;
	}
	*value = s_reader->getAttributeValueSafe(L"value");
}

void readSimpleElement (const wchar_t* element, bool* value)
{
	while (s_reader->read())
	{
		if (s_reader->getNodeType() == EXN_ELEMENT && wcscmp(s_reader->getNodeName(), element) == 0)
			break;
	}
	if (wcscmp(s_reader->getAttributeValueSafe(L"value"), L"true") == 0)
		*value = true;
	else
		*value = false;
}

void readEndElement ()
{
	while (s_reader->read())
	{
		if (s_reader->getNodeType() == EXN_ELEMENT_END && wcscmp(s_reader->getNodeName(), s_xmlStack.top().c_str()) == 0)
			break;
	}
	s_xmlStack.pop();
}

static IXMLWriter* s_writer = 0;

void writeBeginElement (const wchar_t* element)
{
	s_xmlStack.push(element);
	s_writer->writeElement(element);
	s_writer->writeLineBreak();
}

void writeSimpleElement (const wchar_t* element, int value)
{
	s_writer->writeElement(element, true, L"value", stringw(value).c_str());
	s_writer->writeLineBreak();
}

void writeSimpleElement (const wchar_t* element, const stringw& value)
{
	s_writer->writeElement(element, true, L"value", value.c_str());
	s_writer->writeLineBreak();
}

void writeSimpleElement (const wchar_t* element, bool value)
{
	s_writer->writeElement(element, true, L"value", value ? L"true" : L"false");
	s_writer->writeLineBreak();
}

void writeEndElement ()
{
	s_writer->writeClosingTag(s_xmlStack.top().c_str());
	s_writer->writeLineBreak();
	s_xmlStack.pop();
}

void saveDayInfo (const char* path, Day* level)
{
	while (!s_xmlStack.empty()) s_xmlStack.pop();

	s_writer = __IRR_DEVICE()->getFileSystem()->createXMLWriter(path);
	if (!s_writer) return;

	s_writer->writeXMLHeader();

	writeBeginElement(L"level");
		writeSimpleElement(L"dayEndTime", level->dayEndTime);
		writeSimpleElement(L"challenge", stringw(ChallengeTypeToString(level->challenge)));
		writeSimpleElement(L"difficulty", level->difficulty);
		writeBeginElement(L"waves");
			for (int i = 0; i < level->waves.size(); ++i)
			{
				Wave* wave = level->waves[i];
				writeBeginElement(L"wave");
					writeSimpleElement(L"name", wave->name);
					writeSimpleElement(L"timeStart", wave->timeStart);
					writeSimpleElement(L"timeInterval", wave->timeInterval);
					writeSimpleElement(L"repeatInterval", wave->repeatInterval);
					writeSimpleElement(L"repeatCount", wave->repeatCount);
					writeSimpleElement(L"packed", wave->packed);
					for (int j = 0; j < wave->variants.size(); ++j)
					{
						Variant* var = wave->variants[j];
						writeBeginElement(L"variant");
						writeSimpleElement(L"intro", var->intro);
						for (int k = 0; k < var->enemyTypes.size(); ++k)
						{
							EnemyType* enemy = var->enemyTypes[k];
							writeBeginElement(L"enemy");
								writeSimpleElement(L"type", g_unitTypes[enemy->unitType]->name);
								writeSimpleElement(L"min", enemy->minCount);
								writeSimpleElement(L"max", enemy->maxCount);
							writeEndElement();
						}
						writeEndElement();
					}
				writeEndElement();
			}
		writeEndElement();
	writeEndElement();

	s_writer->drop();
	s_writer = 0;
}

Day* loadDayInfo (const char* path)
{
	while (!s_xmlStack.empty()) s_xmlStack.pop();

	s_reader = __IRR_DEVICE()->getFileSystem()->createXMLReader(path);
	if (!s_reader) return 0;

	Day* level = new Day;

	readBeginElement(L"level");
		readSimpleElement(L"dayEndTime", &level->dayEndTime);
		stringw type;
		readSimpleElement(L"challenge", &type);
		level->challenge = StringToChallengeType(type.c_str());
		readSimpleElement(L"difficulty", &level->difficulty);
		readBeginElement(L"waves");
		while (readBeginElementOrEndParent(L"wave"))
		{
			Wave* wave = new Wave;
			readSimpleElement(L"name", &wave->name);
			readSimpleElement(L"timeStart", &wave->timeStart);
			readSimpleElement(L"timeInterval", &wave->timeInterval);
			readSimpleElement(L"repeatInterval", &wave->repeatInterval);
			readSimpleElement(L"repeatCount", &wave->repeatCount);
			readSimpleElement(L"packed", &wave->packed);
			while (readBeginElementOrEndParent(L"variant"))
			{
				Variant* var = new Variant;
				readSimpleElement(L"intro", &var->intro);
				while (readBeginElementOrEndParent(L"enemy"))
				{
					EnemyType* enemy = new EnemyType;
					enemy->unitType = 0;

					stringw type;
					readSimpleElement(L"type", &type);
					for (int i = 0; i < g_unitTypes.size(); ++i)
						if (g_unitTypes[i]->name == type)
						{
							enemy->unitType = i;
							break;
						}
					readSimpleElement(L"min", &enemy->minCount);
					readSimpleElement(L"max", &enemy->maxCount);
					var->enemyTypes.push_back(enemy);
					readEndElement();
				}
				wave->variants.push_back(var);
			}
			level->waves.push_back(wave);
		}
	readEndElement();

	s_reader->drop();
	s_reader = 0;

	return level;
}

void saveLevelInfo (const char* path, Level* level)
{
	while (!s_xmlStack.empty()) s_xmlStack.pop();

	s_writer = __IRR_DEVICE()->getFileSystem()->createXMLWriter(path);
	if (!s_writer) return;
	s_writer->writeXMLHeader();

	wchar_t buf[128];
	writeBeginElement(L"level");
	for (int i = 0; i < WT_Count - 1; ++i)
	{
		swprintf(buf, 128, L"weapon-%d-starting-level", i + 1);
		writeSimpleElement(buf, (int)level->weaponStartingLevel[i]);
		swprintf(buf, 128, L"weapon-%d-available-in-shop", i + 1);
		writeSimpleElement(buf, level->weaponAvailableInShop[i]);
	}
	writeSimpleElement(L"spell-available", (int)level->spellAvailableInShop);
	writeSimpleElement(L"spell-bought", level->spellBought);
	writeEndElement();

	s_writer->drop();
	s_writer = 0;
}

Level* loadLevelInfo (const char* path)
{
	Level* result = new Level;
	result->spellAvailableInShop = ST_None;
	result->spellBought = false;
	for (int i = 0; i < WT_Count - 1; ++i)
	{
		result->weaponAvailableInShop[i] = true;
		result->weaponStartingLevel[i] = WL_None;
	}

	while (!s_xmlStack.empty()) s_xmlStack.pop();

	s_reader = __IRR_DEVICE()->getFileSystem()->createXMLReader(path);
	if (!s_reader) return result;

	wchar_t buf[128];
	readBeginElement(L"level");
	for (int i = 0; i < WT_Count - 1; ++i)
	{
		swprintf(buf, 128, L"weapon-%d-starting-level", i + 1);
		int value = 0;
		readSimpleElement(buf, &value);
		result->weaponStartingLevel[i] = (EWeaponLevel)value;

		swprintf(buf, 128, L"weapon-%d-available-in-shop", i + 1);
		readSimpleElement(buf, &result->weaponAvailableInShop[i]);
	}
	int value = 0;
	readSimpleElement(L"spell-available", &value);
	result->spellAvailableInShop = (ESpellType)value;
	readSimpleElement(L"spell-bought", &result->spellBought);
	readEndElement();

	s_reader->drop();
	s_reader = 0;

	return result;
}

void initUnitTypes ()
{
	CGameConfig::GetInstance()->importFile("config/models.xml");
	CGameConfig::GetInstance()->importFile("config/units.xml");
	CGameConfig::GetInstance()->importFile("config/sounds.xml");
	CGameConfig::GetInstance()->importFile("config/weapons.xml");
	CGameConfig::GetInstance()->importFile("config/gates.xml");
	CGameConfig::GetInstance()->importFile("config/spells.xml");
	CGameConfig::GetInstance()->importFile("config/sounds_config.xml");
	CGameConfig::GetInstance()->importFile("config/challenge_rewards.xml");

	for (int i = 0; i < g_unitTypes.size(); i++)
	{
		g_unitTypes[i]->attributes->drop();
		SAFE_DEL(g_unitTypes[i]);
	}
	g_unitTypes.clear();

	TAttributesMap& units = CGameConfig::GetInstance()->getAttributesMap(L"unit");
	for (TAttributesMap::Iterator it = units.getIterator(); !it.atEnd(); it++)
	{
		UnitType* type = new UnitType();
		type->name = it->getKey();
		type->attributes = it->getValue();
		type->attributes->grab();
		g_unitTypes.push_back(type);
	}

	InitUnitParameters();
}

int stringToUnitType (const stringw& str)
{
	for (int i = 0; i < g_unitTypes.size(); ++i)
		if (g_unitTypes[i]->name == str)
			return i;
	return -1;
}

stringw unitTypeToString (int unitType)
{
	return g_unitTypes[unitType]->name;
}

stringc getDayPath (int level, int day)
{
	GX_ASSERT( level >= 0 && day >= 0 );

	char buf[100];
	sprintf(buf, "Levels/level%02d-day%02d.xml", level + 1, day + 1);
	return buf;
}

stringc getLevelPath (int level)
{
	GX_ASSERT( level >= 0 );

	char buf[100];
	sprintf(buf, "Levels/level%02d-info.xml", level + 1);
	return buf;
}

static array<int> s_numberOfDaysPerLevel;
static const char* k_numberOfDaysPerLevelPath = "Levels/info.xml";

array<int> loadNumberOfDaysPerLevel ()
{
	array<int> result;

	while (!s_xmlStack.empty()) s_xmlStack.pop();

	s_reader = __IRR_DEVICE()->getFileSystem()->createXMLReader(k_numberOfDaysPerLevelPath);
	if (!s_reader) return result;

	readBeginElement(L"levels");
	while (readBeginElementOrEndParent(L"level"))
	{
		int days = 0;
		readSimpleElement(L"days", &days);
		result.push_back(days);
		readEndElement();
	}

	s_reader->drop();
	s_reader = 0;

	return result;
}

void saveNumberOfDaysPerLevel (const array<int>& arr)
{
	s_numberOfDaysPerLevel = arr;

	while (!s_xmlStack.empty()) s_xmlStack.pop();

	s_writer = __IRR_DEVICE()->getFileSystem()->createXMLWriter(k_numberOfDaysPerLevelPath);
	if (!s_writer) return;

	s_writer->writeXMLHeader();

	writeBeginElement(L"levels");
	for (int i = 0; i < arr.size(); ++i)
	{
		writeBeginElement(L"level");
		writeSimpleElement(L"days", arr[i]);
		writeEndElement();
	}
	writeEndElement();

	s_writer->drop();
	s_writer = 0;
}

const array<int>& getNumberOfDaysPerLevel ()
{
	if (s_numberOfDaysPerLevel.size() == 0)
		s_numberOfDaysPerLevel = loadNumberOfDaysPerLevel();
	return s_numberOfDaysPerLevel;
}

Wave* cloneWave (Wave* wave)
{
	Wave* clone = new Wave;
	clone->name = wave->name;
	clone->timeStart = wave->timeStart;
	clone->timeInterval = wave->timeInterval;
	clone->repeatInterval = wave->repeatInterval;
	clone->repeatCount = wave->repeatCount;
	clone->packed = wave->packed;

	for (int i = 0; i < wave->variants.size(); ++i)
	{
		Variant* var = new Variant;
		var->intro = wave->variants[i]->intro;
		for (int j = 0; j < wave->variants[i]->enemyTypes.size(); ++j)
		{
			EnemyType* enemy = new EnemyType;
			enemy->unitType = wave->variants[i]->enemyTypes[j]->unitType;
			enemy->minCount = wave->variants[i]->enemyTypes[j]->minCount;
			enemy->maxCount = wave->variants[i]->enemyTypes[j]->maxCount;
			var->enemyTypes.push_back(enemy);
		}
		clone->variants.push_back(var);
	}

	return clone;
}

Day* cloneDay (Day* level)
{
	Day* result = new Day;
	result->dayEndTime = level->dayEndTime;
	result->challenge = level->challenge;
	result->difficulty = level->difficulty;
	for (int i = 0; i < level->waves.size(); ++i)
	{
		result->waves.push_back(cloneWave(level->waves[i]));
	}
	return result;
}

int appendDayToDay (Day* inOut, int timeOffset, Day* levelToAppend)
{
	for (int i = 0; i < levelToAppend->waves.size(); ++i)
	{
		Wave* wave = cloneWave(levelToAppend->waves[i]);
		wave->timeStart += timeOffset;
		inOut->waves.push_back(wave);
	}
	return timeOffset + getDayMaxTime(levelToAppend);
}

int getDayMaxTime (Day* level)
{
	int time = 0;
	for (int i = 0; i < level->waves.size(); ++i)
	{
		Wave* wave = level->waves[i];
		for (int j = 0; j < wave->variants.size(); ++j)
		{
			int waveMaxTime = wave->timeStart + wave->timeInterval + wave->repeatInterval * (wave->repeatCount - 1);
			time = MAX(time, waveMaxTime);
		}
	}
	return time + 1;
}
