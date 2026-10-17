///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////


#include "Entities/IMenu.h"

///////////////////////////////////////////////////////////////////////

class VisualControlButton;
class VisualControlText;

class CSpriteInstance;

//---------------------------------------------------------------------------------------------------------
// Dialog Result
//---------------------------------------------------------------------------------------------------------
typedef enum 
{	
	RESULT_NONE = 0,
	RESULT_YES, 
	RESULT_NO,

} DialogResult;

///////////////////////////////////////////////////////////////////////
/// Menu_Confirmation
///////////////////////////////////////////////////////////////////////

class Menu_Confirmation : public IMenu
{
public:
	Menu_Confirmation(IMenu* parent);
	virtual ~Menu_Confirmation();

	void Initialize();
	void Destroy();

	void GainFocus();
	void LoseFocus();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void ChangeCurrentMenu(IMenu* menu, bool gain_focus = true);

	void SetConfirmationText(s32 id)	{ m_textID = id; UpdateTextFrame(); }
	DialogResult GetDialogResult()		{ return m_result; }
	
	void SetBackGroundFrame( s32 bgrFrame );
	void UpdateTextFrame();
private:
	void DrawBackground();

	s32					m_bgrFrame; 

private:
	s32					m_textID;
	VisualControlText*	m_text;

	DialogResult		m_result;
};