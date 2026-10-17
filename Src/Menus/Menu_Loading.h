///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
#include "Entities/IMenu.h"
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

class VisualControlSlider;
class VisualControlText;
class CSpriteInstance;

enum ELoadingHints
{
	LH_1,
	LH_2,
	LH_3,
	LH_4,
	LH_5,
	LH_6,
	LH_7,
	LH_8,
	LH_9,
	LH_10,
	LH_11,
	LH_12,
	LH_13,
	LH_14,
	LH_15,
	LH_Boulder,
	LH_Lightning,
	LH_GhostArmy,
	LH_Tornado,
	LH_Count,
};

//---------------------------------------------------------------------------------------------------------
// Handles bottom bar and works as container for all main menus
//---------------------------------------------------------------------------------------------------------
class MenuLoading : public IMenu
{
public:
	MenuLoading(IMenu* parent);
	virtual ~MenuLoading();

	void LoadResources();
	void FreeResources();
	
	void Initialize();
	void Destroy();

	void GainFocus();
	void LoseFocus();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void SetProgress( s32 progress );
	bool LoadingDone();

private:
	void DrawBackground();
	void DrawHint();
	void DrawLoading();

private:
	ASprite*				m_BackGround;
	VisualControlSlider*	m_LoadingBar;

	f32						m_loadingDoneTimer;

	bool					IsHintAppropriate(int hint);
	void					SelectLoadingHint();
	int						m_hint;
	static array<int>		s_hints;
};
//---------------------------------------------------------------------------------------------------------

