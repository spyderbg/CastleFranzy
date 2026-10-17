#pragma once
#include <stdint.h>

#include "Core/irrlicht.h"

struct UIID
{
	UIID (void* id = 0, int64_t index = 0, int64_t item = 0);
	UIID& setItem (int64_t item);

	void* id;
	int64_t index;
	int64_t item;
};

bool operator< (UIID a, UIID b);
bool operator== (UIID a, UIID b);

UIID uiGenId();
extern r2i AUTO_SIZE;

r2i uiLayoutAutoWidth (int w);
void uiLayoutBegin (p2i pos, int rowH);
void uiLayoutNextColumn (int space = 0);
void uiLayoutNextRow (int space = 1);
void uiLayoutSpace (int width);

p2i uiParentAbsolutePos ();
d2i uiParentSize ();
int uiParentWidth ();
int uiParentHeight ();

IGUISpriteBank* uiCreateSprites ();

void uiBegin (IGUIElement* root);
void uiEnd ();

ISceneManager* uiGetSceneManager ();
void uiDrawGridIfGroundVisible ();

void uiStaticText (const wchar_t* text, r2i rc, SColor color = 0xff000000);
void uiFilledRect (r2i rc, SColor color);
void uiIcon (int icon, r2i rc, SColor color = 0xffffffff);
void uiDrawRect (r2i rc, SColor color);
bool uiButton (UIID id, const wchar_t* text, r2i rc, bool enabled = true, const wchar_t* tooltip = 0);
bool uiPushButton (UIID id, bool* state, const wchar_t* text, r2i rc, bool enabled = true, const wchar_t* tooltip = 0, ITexture* image = 0, r2i imageRect = r2i(0,0,0,0));
void uiEditString (UIID id, stringw* text, r2i rc, int maxChars, bool autoscroll = true);
void uiEditInt (UIID id, int* value, r2i rc, int min, int max, int maxChars, bool autoscroll = true);
void uiEditFloat (UIID id, float* value, r2i rc, float min, float max, int maxChars, bool autoscroll = true);
void uiCheckbox (UIID id, bool* value, r2i rc, const wchar_t* text);

int uiBeginTabs (UIID id, r2i rc, ...);
void uiEndTabs (UIID id);

void uiBeginList (UIID id, r2i rc);
void uiListItem (UIID id, const wchar_t* item, int icon = -1);
void uiEndList (UIID id, int* selected);

void uiBeginComboBox (UIID id, r2i rc);
void uiComboBoxItem (UIID id, const wchar_t* item);
void uiEndComboBox (UIID id, int* selected);

// must be called with a list as a parent!
struct Wave;
void uiBeginTimeline (UIID id, int levelEndTime, r2i rc);
void uiTimelineItem (UIID id, Wave* item);
void uiEndTimeline (UIID id, int* selected);

enum UnitIconState
{
	UIS_Dead,
	UIS_Alive,
	UIS_NextToSpawn,
	UIS_Future
};
void uiBeginUnitIconTimeline (UIID id, int levelEndTime, r2i rc);
void uiUnitIconTimelineItem (UIID id, float time, int icon, UnitIconState state);
void uiEndUnitIconTimeline (UIID id, float globalTime);

bool canVisualizeWaves (int level, int day);
void uiVisualizeWaves (float timeScale, r2i rc);

void uiNotifyEvent (const SEvent& ev, r2i cam3dviewport);
