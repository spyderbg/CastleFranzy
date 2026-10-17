///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IMenu.h"

#include "IO/TouchScreen/TouchPad.h"

///////////////////////////////////////////////////////////////////////

#define MAX_USER_LEN			16
#define MAX_PASSWORD_LEN		16

///////////////////////////////////////////////////////////////////////

class VisualControlEditBox;
class VisualControlButton;
class VisualControlText;
class WTextInput;

///////////////////////////////////////////////////////////////////////
/// Menu_Login
///////////////////////////////////////////////////////////////////////

class Menu_Login : public IMenu
{
	enum
	{
		LIS_WaitForLoginButton,
		LIS_Logging,
		LIS_Error,
		LIS_Success
	};

public:
	Menu_Login( IMenu* parent );
	virtual ~Menu_Login();

	void Initialize();
	void Destroy();

	void GainFocus();
	void LoseFocus();

	void Draw();
	void Update( float dt );

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

private:
	void initTextInputUsername( video::E_ORIENTATION orientaion );
	void initTextInputPassword( video::E_ORIENTATION orientaion );	
	void destroyTextInputs();
	void changeStatusText( s32 status );
	
private:
	video::E_ORIENTATION	m_currentOrientation;
	
	char					m_username[MAX_USER_LEN];
	char					m_password[MAX_PASSWORD_LEN];

#ifdef OS_IPHONE 

	WTextInput*				m_textInputUsername;
	WTextInput*				m_textInputPassword;

#else

	VisualControlEditBox*	m_textInputUsername; 
	VisualControlEditBox*	m_textInputPassword;

#endif

	VisualControlText*		m_statusText;
	VisualControlButton	*	m_rememberMe;
	bool					m_rememberCheck;

	s32						m_loginState;

};