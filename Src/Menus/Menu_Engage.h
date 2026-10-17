///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IMenu.h"

#include "Game/Config/UserProfile.h"

///////////////////////////////////////////////////////////////////////

class MenuPopupMM;
class Menu_LevelSelect;
class VisualControlButton;

///////////////////////////////////////////////////////////////////////
/// Menu_Engage
///////////////////////////////////////////////////////////////////////

class Menu_Engage : public IMenu
{
public:
	Menu_Engage(IMenu* parent);
	~Menu_Engage();

	void Initialize();
	void Destroy();

	virtual void    GainFocus();

	void Draw();
	void Update( float dt );

	void DrawChapterInfo();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

private:
	bool					m_Finish;
};
//---------------------------------------------------------------------------------------------------------
