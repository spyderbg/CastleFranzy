#pragma once

#include "Entities/IStateBase.h"
#include "Game/LevelInfo.h"
#include "Core/irrlicht.h"
#include "EditorUI.h"

class GS_Editor;

extern bool g_isInEditor;
extern GS_Editor* g_editor;

struct TimeUnitRow
{
	float time;
	int type;
	int row;
};

struct EditorLevel
{
	Level* levelInfo;
	array<Day*> days;
};

class GS_Editor : public IEventReceiver, public CSingleton<GS_Editor>
{
	friend class CTimelineRow;
	friend class CTimeline;

public:
	GS_Editor();
	virtual ~GS_Editor();

public:
	void init ();

	void enter ();
	void exit ();

	void update ();
	void draw();

	bool OnEvent (const SEvent& event);

	ITexture* m_icons;

private:
	struct WaveIndex
	{
		int i;
	};
	friend struct WaveIndex;
	friend bool operator< (WaveIndex a, WaveIndex b);

	void save ();
	void load ();

	void uiWavesTab ();
	void uiWave (Wave* wave);

	void uiWeaponsTab ();

	void uiModelsTab ();

	void insertNewWave ();
	void cloneWave (Wave* wave);
	void deleteWave (int level, int day, int wave);
	void deleteVariant (Wave* wave, int variant);
	void deleteEnemyType (Variant* variant, int type);

	void insertNewDay (Day* level = 0);
	void deleteDay (int level, int day);

	void insertNewLevel ();
	void deleteLevel (int level);

	bool editingEndless ();

	int m_activeTab;

	UIID m_waveEditId;
	UIID m_unitsDeleteId;

	// models
	int m_selectedModel;
	array<ISceneNode*> m_modelNodes;
	r2i m_viewport;
	bool m_showDummies;

	array<TimeUnitRow> m_unitsCurLev;
	void allocateRows ();

	// level info
	array<EditorLevel> m_levels;
	int m_currentLevel;
	int m_currentDay;
	int m_currentWave;

	Day* m_copiedDay;

	IGUIElement* m_root;

	bool m_firstStart;
};
