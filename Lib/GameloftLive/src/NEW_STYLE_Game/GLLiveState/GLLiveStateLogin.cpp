#include "GLLiveStateLogin.h"
#include "GLLiveStateEdit.h"
#include "GLLiveStateMenu.h"
#include "GLLiveStateProfile.h"
#include "GLLiveStateTrophy.h"
#include "GLLive_Memory.h"
#include "MyGraphics.h"
#include "GLLive_Stream.h"

const static int LOGIN_EDITBOX_W = 266;
const static int LOGIN_EDITBOX_H = 25;

const static int REG_EDITBOX_W = 172;
const static int REG_EDITBOX_H = 25;

const static int START_Y = 60;

char	gll_country[3];

char* LANG_FOR_SERVER[6] = {"EN","EN","DE","FR","IT","ES"};//{"EN","JA","DE","FR","IT","ES"};
int getLangIndexFromServer(char* lang)
{
	for(int i = 0; i < 6; i++)
	{
		if(XP_API_STRCMP(lang, LANG_FOR_SERVER[i]) == 0)
			return i;
	}
	return 0;
}
GLLiveStateLogin::GLLiveStateLogin(GLLiveStateHandler * handler) : GLLiveState( handler )
{
	//Diable go button
	EnableGo(false);

	m_loginMenu = NULL;
	m_regMenu = NULL;
	m_forgotMenu = NULL;
	
	m_sub_state = ONLINE_NULL;
	m_last_state = ONLINE_NULL;
	SetSubState(ONLINE_LOGIN);

	if(m_pMsgBox == NULL)
	{
		m_pMsgBox = XP_API_NEW CGLLiveMessageBox();
	}

	m_timestamp = 0;
#ifdef GET_CONFIG_FROM_SERVER
	//get config from server
	int ret = 0;
	m_gl_serverConfig->SendGetSreverConfig(ret);
	if(ret == -1)
	{
		m_pMsgBox->SetTitle(GetString(STR_ERROR));
		m_pMsgBox->SetText("Please check you oconf.bar or contact with World Online Team or World GLLive Team for you project.\n(This should only be seen by Dev Team!)");
		m_last_state = m_sub_state;
		m_sub_state = ONLINE_ERROR;
	}
	else if(ret == 0)
	{
		SetSubState(ONLINE_PROCESS);
	}
	else if(ret == 1)
	{
		m_gl_login        = XP_API_NEW GLXPlayerLogin();
		m_gl_message      = XP_API_NEW GLXPlayerMessage();
		m_gl_user         = XP_API_NEW GLXPlayerUser();
		m_gl_userFriend   = XP_API_NEW GLXPlayerUserFriend();
		m_gl_chat         = XP_API_NEW GLXPlayerChat();
		RegisterOnlineObject();
#endif
		//Init user infor
		LoadProfile();
		SetRememberMe( (XP_API_STRLEN(m_password) > 0) );

		m_totalLoginTime = -1;
		if(m_bAutoLogin && XP_API_STRLEN(m_username) > 0)
		{
			m_gl_login->SendLogin(m_username,m_password,m_timestamp, 1, false, 
								  g_pGLLiveInstance->m_deviceToken, LANG_FOR_SERVER[g_pGLLiveInstance->m_Language], g_pGLLiveInstance->m_UDID);
			m_totalLoginTime = XP_API_GET_TIME();
			SetSubState(ONLINE_PROCESS);
		}
		else
		{
			XP_API_MEMSET(m_password, 0, sizeof(m_password));
		}
		SetRememberMe(true);
#ifdef GET_CONFIG_FROM_SERVER
	}
#endif
	m_trophyCount = 0;
	m_trophyList = NULL;

	//Init confirm menu
	_TouchRect rect;
	m_confirmMenu = CGLLiveMessageBox::GenerateConfirmMenu(this);

	m_protocolMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(90, 240, 370, 25);
	m_protocolMenu->AddMenuItem(this, IDC_REG_ACCEPT, GetString(STR_CONDITIONS_DETAIL), rect);
	m_protocolMenu->GetItem(0)->SetPic(SPRITE_MENU, Frame_menu_CHECK, rect.left, rect.top);
	m_protocolMenu->GetItem(0)->SetSelPic(SPRITE_MENU, Frame_menu_CHECK_ON, rect.left - 5, rect.top - 5);
	m_protocolMenu->GetItem(0)->SetFont(3);
	m_protocolMenu->GetItem(0)->SetTitlePos(30, rect.height / 2, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);

	rect.Set(355, 276, 113, 28);
	m_protocolMenu->AddMenuItem(this, IDC_REG_CONTINUE, NULL, rect);
	m_protocolMenu->GetItemById(IDC_REG_CONTINUE)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_CONTINUE2 + 3 * g_pGLLiveInstance->getLang());
	m_protocolMenu->GetItemById(IDC_REG_CONTINUE)->SetSelPic(SPRITE_MENU, Frame_menu_BUTTON_CONTINUE + 1 + 3 * g_pGLLiveInstance->getLang());

	m_confirming = false;
	m_confirmLogin = false;
	m_retrievlNote = false;

	m_isAcceptProtocol = false;
	m_gcContent = NULL;
	m_gcTime = NULL;
	m_isRereadGC = false;
	m_isLogin = false;
	m_scrollMoving = false;
}

GLLiveStateLogin::~GLLiveStateLogin(void)
{
	XP_API_DELETE_ARRAY(m_gcContent);
	XP_API_DELETE_ARRAY(m_gcTime);
	XP_API_DEL(m_protocolMenu);
	XP_API_DEL(m_loginMenu);
	XP_API_DEL(m_regMenu);
	XP_API_DEL(m_forgotMenu);
	XP_API_DEL(m_confirmMenu);
	XP_API_DELETE_ARRAY(m_trophyList);
}

bool GLLiveStateLogin::HandleEvent(_TOUCH_EVENT event)
{
	switch(m_sub_state)
	{
	case ONLINE_LOGIN:
		if(m_loginMenu->HandleEvent(event))
		{
			return true;
		}
		return GLLiveState::HandleEvent(event);
	case ONLINE_FORGET:
		if(m_forgotMenu->HandleEvent(event))
		{
			return true;
		}
		return GLLiveState::HandleEvent(event);
	case ONLINE_ERROR:
		if(m_confirming || m_confirmLogin)
		{
			if(m_confirmMenu->HandleEvent(event))
			{
				return true;
			}
		}
		else if(m_retrievlNote)
		{
			m_retrievlNote = false;
			if(event.eType == TET_Pressed)
			{
				m_sub_state = ONLINE_FORGET;
				SetSubState(ONLINE_LOGIN);
			}
			return true;
		}
		else if(m_pMsgBox != NULL && event.eType == TET_Pressed)
		{
			SetSubState( m_last_state );
			//no need to logout
			//if(m_isLogin)
			//{
			//	m_isLogin = false;
			//	m_gl_login->SendLogout();
			//	m_totalLoginTime = XP_API_GET_TIME();
			//	SetSubState(ONLINE_PROCESS);
			//}
			return true;
		}
		break;
	case ONLINE_PROCESS:
		break;
	case ONLINE_REGISTER:
		if(m_regMenu->HandleEvent(event))
		{
			return true;
		}
		return GLLiveState::HandleEvent(event);
	case ONLINE_PROTOCOL:
	{
		if(m_protocolMenu->HandleEvent(event))
		{
			return true;
		}
		_TouchRect rectView, rectScrollBar;
		rectView.Set(30, 60, 415, 160);
		rectScrollBar.Set(442 - 1 - 5, 65 - 10 + m_scrollBarPos - 1 - 5, 30, 30);
		if(event.eType == TET_Pressed)
		{
			//Begin moving sroll bar
			if(rectScrollBar.Contain(event.position))
			{
				m_scrollMoving = true;
			}
			//Begin moving
			else if(rectView.Contain(event.position))
			{
				m_touchPos = event.position.y;
				m_moving   = true;
			}
		}
		else if(event.eType == TET_Moving)
		{
			//Is moving scroll bar
			if(m_scrollMoving)
			{
				m_offset = (event.position.y - (65 - 10) - 10) * (m_totalHeight - rectView.height) / (rectView.height - m_scrollBarH );

				if(m_offset < 0 )
				{
					m_offset = 0;
					m_speed = 0;
				}
				else if(m_offset > m_totalHeight - rectView.height)
				{
					m_speed = 0;
					m_offset = m_totalHeight - rectView.height;
				}

			}
			//Is moving view
			else if(m_moving)
			{
				if(rectView.Contain(event.position))
				{
					m_oldOffset = m_offset;
					m_offset += (m_touchPos - event.position.y);
					if(m_offset < 0 )
					{
						m_offset = 0;
						m_speed = 0;
					}
					else if(m_offset > m_totalHeight - rectView.height)
					{
						m_speed = 0;
						m_offset = m_totalHeight - rectView.height;
					}
					m_touchPos = event.position.y;
				}
				else
				{
					m_moving = false;
				}
			}
			return true;
		}
		else if(event.eType == TET_Released)
		{
			m_moving = false;
			m_scrollMoving = false;
		}
		return GLLiveState::HandleEvent(event);
	}
	default:
		break;
	}
	return false;
}

void GLLiveStateLogin::SwitchState()
{
	SetRememberMe( m_bAutoLogin );
	if(m_sub_state == ONLINE_LOGIN)
	{
		SaveProfile();
	}
	m_totalLoginTime = -1;
}

void GLLiveStateLogin::Paint()
{
	m_handler->ClearScreen();
	switch(m_sub_state)
	{
	case ONLINE_LOGIN:
		PaintLogin();
		break;
	case ONLINE_ERROR:
		if(m_last_state == ONLINE_LOGIN)
		{
			PaintLogin();
		}
		else if(m_last_state == ONLINE_REGISTER)
		{
			PaintRegister();
		}
		else if(m_last_state == ONLINE_FORGET)
		{
			PaintForget();
		}
		else if(m_last_state == ONLINE_PROTOCOL)
		{
			PaintProtocol();
		}
		m_pMsgBox->Repaint(m_handler);
		if(m_confirming || m_confirmLogin)
		{
			m_confirmMenu->Repaint(m_handler);
		}
		break;
	case ONLINE_PROCESS:
		if(m_last_state == ONLINE_LOGIN)
		{
			PaintLogin();
		}
		else if(m_last_state == ONLINE_REGISTER)
		{
			PaintRegister();
		}
		else if(m_last_state == ONLINE_FORGET)
		{
			PaintForget();
		}
		PaintLoading();
		break;
	case ONLINE_REGISTER:
		PaintRegister();
		break;
	case ONLINE_FORGET:
		PaintForget();
		break;
	case ONLINE_PROTOCOL:
		PaintProtocol();
		break;
	default:
		break;
	}
}

void GLLiveStateLogin::PaintProtocol()
{
	CGLLiveGraphics * g = m_handler->GetGraphics();
	CGLLiveFont     * f7 = m_handler->GetFont(6);
	
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_GC, 0, 0, 0, 0, 0);

	g->BeginScale(0.55, 0.55, 295, 8);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LOGO2, 295 - 84, 8 - 18, 0, 0, 0);
	g->EndScale();


	int m_nLines = f7->InitPage(m_gcContent, 415);

	g->SetClip(30, 60, 415, 160);
	f7->DrawPage(g, m_gcContent, 30, 60 - m_offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	g->ResetClip();
	//g->SetClip(55, 25, 400, 198);
	if(true)
	{
		m_scrollBarPos = (float)(m_offset * (160 - m_scrollBarH )) / (m_totalHeight - 160) ;

		//paint H Scoll Bar
		if(m_scrollBarH < 10)
		{
			m_scrollBarH = 10;
		}

		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_SCROLL, 448, 65, 0, 0, 0);
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR1, 442, 65 - 10 + m_scrollBarPos, 0, 0, 0);
		//for(int a = 0; a < m_scrollBarH / 10; a++)
		//{
		//	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR1, 442, 60 - 10 + m_scrollBarPos + a * 10, 0, 0, 0);
		//}
		//if(m_scrollBarH % 10 > 0)
		//{
		//	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR1, 442, 60 - 10 + m_scrollBarPos + m_scrollBarH - 10, 0, 0, 0);
		//}
	}

	m_protocolMenu->Repaint(m_handler);
	GLLiveState::Paint();
}

void GLLiveStateLogin::PaintLogin()
{
	char temp[64];
	int x, y;
	const _TouchRect &editUsername = m_loginMenu->GetItemById(IDE_LOGIN_USERNAME)->GetRect();
	const _TouchRect &editPassword = m_loginMenu->GetItemById(IDE_LOGIN_PASSWORD)->GetRect();
	CGLLiveGraphics * g = m_handler->GetGraphics();
	CGLLiveFont     * f = m_handler->GetFont(3);
	//g->EndDraw2D();
	//g->BeginDraw2D(320, 480);
	//m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LOGO, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LOGO2, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_EDIT_LOGIN, 0, 0, 0, 0, 0);

	x = editUsername.left + 10;
	y = editUsername.top + (editUsername.height) / 2 + 15;
	f->DrawString(g, m_username, x, y, CGLLiveGraphics::VCENTER, editUsername.width);

	x = editPassword.left + 10;
	y = editPassword.top + (editPassword.height) / 2 + 15;
	XP_API_STRCPY(temp, m_password);
	XP_API_STRSET(temp, '*');
	f->DrawString(g, temp, x, y, CGLLiveGraphics::VCENTER);

	m_loginMenu->Repaint(m_handler);

	GLLiveState::Paint();
}

void GLLiveStateLogin::PaintRegister()
{
	char temp[64];
	CGLLiveGraphics * g = m_handler->GetGraphics();
	CGLLiveFont     * f = m_handler->GetFont(3);

	const _TouchRect &editUsername = m_regMenu->GetItemById(IDE_REG_USERNAME)->GetRect();
	const _TouchRect &editEmail    = m_regMenu->GetItemById(IDE_REG_EMAIL)->GetRect();
	const _TouchRect &editPassword = m_regMenu->GetItemById(IDE_REG_PASSWORD)->GetRect();
	const _TouchRect &editConfirm  = m_regMenu->GetItemById(IDE_REG_CONFIRM)->GetRect();

	//g->EndDraw2D();
	//g->BeginDraw2D(320, 480);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LOGO, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LOGO2, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_EDIT_REG, 0, 0, 0, 0, 0);
	
	f->DrawString(g, m_username, editUsername.left + 10, editUsername.top + editUsername.height / 2 + 11, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER, editUsername.width - 10);
	f->DrawString(g, m_email, editEmail.left + 10, editEmail.top + editEmail.height / 2 + 11, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER, editEmail.width - 10);

	XP_API_STRCPY(temp, m_password);
	XP_API_STRSET(temp, '*');
	f->DrawString(g, temp, editPassword.left + 10, editPassword.top + editPassword.height / 2 + 11, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER, editPassword.width - 10);

	XP_API_STRCPY(temp, m_confirmPwd);
	XP_API_STRSET(temp, '*');
	f->DrawString(g, temp, editConfirm.left + 10, editConfirm.top + editConfirm.height / 2 + 11, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER, editConfirm.width - 10);

	//Draw menu
	m_regMenu->Repaint(m_handler);

	GLLiveState::Paint();
}

void GLLiveStateLogin::PaintForget()
{
	int x, y;
	CGLLiveGraphics * g = m_handler->GetGraphics();
	CGLLiveFont     * f4 = m_handler->GetFont(3);

	const _TouchRect &editUsername = m_forgotMenu->GetItemById(IDE_FORGOT_USERNAME)->GetRect();

	//g->EndDraw2D();
	//g->BeginDraw2D(320, 480);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LOGO, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LOGO2, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_INPUT_AREA, editUsername.left, editUsername.top + 25, 0);

	x = editUsername.left + 10;
	y = editUsername.top + editUsername.height / 2 + 14;
	f4->DrawString(g, m_username, x, y, CGLLiveGraphics::VCENTER);

	x = editUsername.left;
	y = editUsername.top + editUsername.height + 10;
	f4->InitPage(GetString(STR_LOGIN_FORGET_NOTE), editUsername.width);
	f4->DrawPage(g,GetString(STR_LOGIN_FORGET_NOTE), x, y, CGLLiveGraphics::LEFT);

	m_forgotMenu->Repaint(m_handler);

	GLLiveState::Paint();
}

void GLLiveStateLogin::UpdateAnimation()
{
	if(m_loginMenu != NULL && m_loginMenu->Update())
	{
		return;
	}
	if(m_regMenu != NULL && m_regMenu->Update())
	{
		return;
	}
	if(m_forgotMenu != NULL && m_forgotMenu->Update())
	{
		return;
	}

 	//if mail address is null, email flag must be false
	if(m_sub_state == ONLINE_REGISTER && m_emailFlag && XP_API_STRLEN(m_email) == 0)
	{
		SetEmailFlag( false );
 	}

	if((m_confirming || m_confirmLogin) && m_confirmMenu != NULL)
	{
		m_confirmMenu->Update();
	}

	if(m_sub_state == ONLINE_PROTOCOL)
	{
		//Update for scroll view
		if(m_moving)
		{
			m_speed = m_offset - m_oldOffset;/* / 1 */
			m_oldOffset = m_offset;
		}
		else
		{
			if( m_speed != 0 )
			{
				//Change speed
				if(ABS(m_speed) <= 5)
				{
					m_speed = 0;
				}
				else if(ABS(m_speed) > 5)
				{
					m_speed -= (m_speed / ABS(m_speed)) * 5;
				}

				//Change offset
				m_offset += m_speed;
				if(m_offset < 0)
				{
					m_offset = 0;
					m_speed = 0;
				}
				else if(m_offset > m_totalHeight - 188)
				{
					m_offset = m_totalHeight - 188;
					m_speed = 0;
				}
			}
		}
		//Update GLLiveMenu
		m_protocolMenu->Update();
	}

	if(m_totalLoginTime > -1)
	{
		if(XP_API_GET_TIME() - m_totalLoginTime > 90000)
		{
			m_totalLoginTime = -1;
			if(m_gl_login)
			{
				OnRequestTimeout(0);
				CancelRequest();
			}
		}
	}

	GLLiveState::UpdateAnimation();
}

void GLLiveStateLogin::OnMenuItemClicked(int id)
{
	switch(id)
	{
	case IDC_LOGIN:
		//Error:No user name
		if(XP_API_STRLEN(m_username) <= 0)
		{
			m_errorCode = ERROR_NO_USERNAME;
			SetSubState(ONLINE_ERROR);
		}
		else if(XP_API_STRLEN(m_password) <= 0)
		{
			m_errorCode = ERROR_NO_PASSWORD;
			SetSubState(ONLINE_ERROR);
		}
		else
		{
			m_gl_login->SendLogin(m_username,m_password, m_timestamp, 1, false, 
								  g_pGLLiveInstance->m_deviceToken, LANG_FOR_SERVER[g_pGLLiveInstance->m_Language], g_pGLLiveInstance->m_UDID);
			m_totalLoginTime = XP_API_GET_TIME();
			SetSubState(ONLINE_PROCESS);
			//m_handler->PushGLLiveState(XP_API_NEW GLLiveStateMenu(m_handler));
		}
		return;
	case IDC_CREATE:
		//Error:No user name
		if(XP_API_STRLEN(m_username) <= 0)
		{
			m_errorCode = ERROR_NO_USERNAME;
			SetSubState(ONLINE_ERROR);
		}
		else if(!CheckInput(m_username) || !CheckInput(m_password) || !CheckInput(m_confirmPwd) )
		{
			m_errorCode = ERROR_WRONG_INPUT;
			SetSubState(ONLINE_ERROR);
		}
		else if(XP_API_STRLEN(m_password) <= 0 || XP_API_STRLEN(m_confirmPwd) <= 0)
		{
			m_errorCode = ERROR_REG_NO_PASSWORD;//ERROR_NO_PASSWORD;
			SetSubState(ONLINE_ERROR);
		}
		//Error:Password confirm failed
		else if(XP_API_STRCMP(m_password, m_confirmPwd) != 0)
		{
			m_errorCode = ERROR_PASSWORD_CONFIRM_FAILED;
			SetSubState(ONLINE_ERROR);
		}
		//Error: Email is empty
		else if(XP_API_STRLEN(m_email) <= 0)
		{
			m_errorCode = ERROR_NO_EMAIL;
			SetSubState(ONLINE_ERROR);
		}
		else if(!CheckEmail(m_email))
		{
			m_errorCode = ERROR_WRONG_EMAIL;
			SetSubState(ONLINE_ERROR);
		}
		else
		{
			//SetSubState(ONLINE_PROTOCOL);
			SendCreate();
		}
		return;
	case IDC_REGISTER:
		//SetSubState(ONLINE_REGISTER);
		//SetSubState(ONLINE_PROTOCOL);
		m_gl_user->sendGetGeneralCondition(LANG_FOR_SERVER[g_pGLLiveInstance->m_Language]);
		m_totalLoginTime = XP_API_GET_TIME();
		m_sub_state = ONLINE_PROCESS;
		m_last_state = ONLINE_LOGIN;
		return;
	case IDC_FORGET:
		SetSubState(ONLINE_FORGET);
		return;
	case IDC_BACK:
		if(m_sub_state == ONLINE_REGISTER || m_sub_state == ONLINE_FORGET)
		{
			SetSubState(ONLINE_LOGIN);
			return;
		}
		else if(m_sub_state == ONLINE_PROTOCOL)
		{
			m_isRereadGC = false;
			m_sub_state = ONLINE_LOGIN;
			return;
		}
		//exit GL Live, draw loading screen for feedback
		PaintLoading();
		break;
	case IDC_GO:
		if(m_sub_state == ONLINE_FORGET)
		{
			if(XP_API_STRLEN(m_username) <= 0)
			{
				m_errorCode = ERROR_NO_USERNAME;
				SetSubState(ONLINE_ERROR);
			}
			else
			{
				m_gl_user->sendRetrievalPassword(m_username);
				m_totalLoginTime = XP_API_GET_TIME();
				SetSubState(ONLINE_PROCESS);
			}
		}
		break;
	case IDE_LOGIN_USERNAME:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_LOGIN_USERNAME), m_username, false, MAX_USERNAME - 1));
		//m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, m_username, this));
		return;
	case IDE_LOGIN_PASSWORD:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_LOGIN_PASSWORD), m_password, true, MAX_PASSWORD - 1));
		return;
	case IDE_REG_USERNAME:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_REG_USERNAME), m_username, false, MAX_USERNAME - 1));
		return;
	case IDE_REG_EMAIL:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_REG_EMAIL), m_email, false, MAX_EMAIL - 1));
		return;
	case IDE_REG_PASSWORD:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_REG_PASSWORD), m_password, true, MAX_PASSWORD - 1));
		return;
	case IDE_REG_CONFIRM:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_REG_CONFIRM), m_confirmPwd, true, MAX_PASSWORD - 1));
		return;
	case IDC_REG_EMAIL_FLAG:
	{
		bool newFlag = !m_emailFlag;
		//if mail address is null, email flag must be false
		if(XP_API_STRLEN(m_email) == 0)
		{
			newFlag = false;
		}
		SetEmailFlag(newFlag);
	}
		return;
	case IDE_FORGOT_USERNAME:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_LOGIN_USERNAME), m_username, false, MAX_USERNAME - 1));
		return;
	case IDC_REMEMBER_ME:
		SetRememberMe( !m_bAutoLogin );
		return;
	case IDC_MSG_YES:
		if(m_confirming)
		{
			m_confirming = false;
			SendCreate();
			//SetSubState(ONLINE_PROTOCOL);
		}
		else if(m_confirmLogin)
		{
			m_gl_login->SendLogin(m_username,m_password,m_timestamp, 1, false, 
								  g_pGLLiveInstance->m_deviceToken, LANG_FOR_SERVER[g_pGLLiveInstance->m_Language], g_pGLLiveInstance->m_UDID);
			m_totalLoginTime = XP_API_GET_TIME();
			m_sub_state = ONLINE_PROCESS;
			m_confirmLogin = false;
		}
		break;
	case IDC_MSG_NO:
		if(m_confirming)
		{
			m_confirming = false;
			m_sub_state = m_last_state;
		}
		else if(m_confirmLogin)
		{
			SetSubState(ONLINE_LOGIN);
			m_confirmLogin = false;
		}
		break;
	case IDC_REG_ACCEPT:
	{
		GLLiveMenuItem *pItem = m_protocolMenu->GetItemById(IDC_REG_ACCEPT);
		m_isAcceptProtocol = !m_isAcceptProtocol;
		if(m_isAcceptProtocol)
		{
			pItem->SetPic(SPRITE_MENU, Frame_menu_CHECKED, pItem->GetRect().left, pItem->GetRect().top);
			m_protocolMenu->GetItemById(IDC_REG_CONTINUE)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_CONTINUE + 3 * g_pGLLiveInstance->getLang());
			m_protocolMenu->GetItemById(IDC_REG_CONTINUE)->SetEnable(true);
		}
		else
		{
			pItem->SetPic(SPRITE_MENU, Frame_menu_CHECK, pItem->GetRect().left, pItem->GetRect().top);
			m_protocolMenu->GetItemById(IDC_REG_CONTINUE)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_CONTINUE2 + 3 * g_pGLLiveInstance->getLang());
			m_protocolMenu->GetItemById(IDC_REG_CONTINUE)->SetEnable(false);
		}
	}
		break;
	case IDC_REG_CONTINUE:
		//SendCreate();
		if(m_isRereadGC)
		{
			m_isRereadGC = false;
			m_gl_user->sendGetAttention();
			m_totalLoginTime = XP_API_GET_TIME();

			//change the state back to Login
			{
				m_last_state = ONLINE_LOGIN;
				m_sub_state = ONLINE_PROCESS;
			}
			//save the timestamp
			char saveName[32] = {0};
			XP_API_SPRINTF(saveName, "gcTime_%s", m_username);
			GLLiveStateTrophy::SaveFileToLocal(saveName, m_gcTime, XP_API_STRLEN(m_gcTime));
		}
		else
		{
			SetSubState(ONLINE_REGISTER);
		}
		break;
	default:
		break;
	}
	GLLiveState::OnMenuItemClicked(id);
}

//void GLLiveStateLogin::OnNetworkError()
//{
//	m_errorCode = ERROR_CONNECTION;
//	SetSubState(ONLINE_ERROR);
//	XP_DEBUG_OUT( "[GLLiveStateLogin] OnNetworkError: %d\n", m_errorCode);
//}
void getXMLContent(char* dst, char* src, const char* label)
{
	int index1 = 0;
	int index2 = 0;
	int label_len = XP_API_STRLEN(label);
	if(label_len < 1)
		return;

	//search the start
	int i = 0;
	while(i < XP_API_STRLEN(src))
	{
		if(src[i] == '<')
		{
			i++;
			int j;
			for(j = 0; j < label_len; j++)
			{
				if(src[i + j] != label[j])
					break;//break the for
			}
			if(j == label_len)
			{
				while(src[i] != '>')
					i++;
				index1 = i + 1;
				break;//break the while
			}
		}
		else
			i++;
	}

	//search the end
	while(i < XP_API_STRLEN(src))
	{
		if(src[i] == '<')
		{
			i++;
			if(src[i] == '/')
			{
				int j;
				for(j = 0; j < label_len; j++)
				{
					if(src[i + 1 + j] != label[j])
						break;//break the for
				}
				if(j == label_len)
				{
					index2 = i - 1;
					break;//break the while
				}
			}
		}
		else
			i++;
	}
	XP_API_MEMCPY(dst, src + index1, index2 - index1);
}
extern CGLImage * g_pAvatarImage;
extern int        g_iAvatarId;
void GLLiveStateLogin::OnRequestSuccess( int func_id, char * data, int data_len )
{
	XP_DEBUG_OUT( "[GLLiveStateLogin] OnRequestSuccess, fun_id = %d\n", func_id);
	if(m_totalLoginTime == -1)
	{
		XP_DEBUG_OUT( "[GLLiveStateLogin] OnRequestSuccess, fun_id = %d, but already time out!", func_id);
		return;
	}
	switch(func_id)
	{
		case FUNCTION_GET_WEBCONFIG:
			m_gl_login        = XP_API_NEW GLXPlayerLogin();
			m_gl_message      = XP_API_NEW GLXPlayerMessage();
			m_gl_user         = XP_API_NEW GLXPlayerUser();
			m_gl_userFriend   = XP_API_NEW GLXPlayerUserFriend();
			m_gl_chat         = XP_API_NEW GLXPlayerChat();
			RegisterOnlineObject();

			//Init user infor
			LoadProfile();
			SetRememberMe( (XP_API_STRLEN(m_password) > 0) );

			m_totalLoginTime = -1;
			if(m_bAutoLogin && XP_API_STRLEN(m_username) > 0)
			{
				m_gl_login->SendLogin(m_username,m_password,m_timestamp, 1, false, 
									  g_pGLLiveInstance->m_deviceToken, LANG_FOR_SERVER[g_pGLLiveInstance->m_Language], g_pGLLiveInstance->m_UDID);
				m_totalLoginTime = XP_API_GET_TIME();
				SetSubState(ONLINE_PROCESS);
			}
			else
			{
				XP_API_MEMSET(m_password, 0, sizeof(m_password));
				SetSubState(ONLINE_LOGIN);
			}
			SetRememberMe(true);
			break;
		case GL_XPLAYER_FUNCTION_LOGIN:
			//Go to main menu after login successful
			XP_DEBUG_OUT( "[GLLiveStateLogin] Login OK!!!\n" );

			m_gl_user->setPassword(m_password);
			m_gl_user->SetUID( m_gl_login->GetUID() );
			m_gl_message->SetUID(m_gl_login->GetUID());
			m_gl_userFriend->SetUID(m_gl_login->GetUID());

			m_timestamp = m_gl_login->GetTimeStamp();
			SaveProfile();
#if 1
			//Get login information
			//m_loginUser.Clear();
			//XP_API_STRCPY(m_loginUser.username, m_username);
			//XP_API_STRCPY(m_loginUser.password, m_password);
			//Get user info
			//m_gl_user->sendGetUserInfo();
			//gct|time
			if(data_len > 0)
			{
				int count = 0;
				for(int i = 0; i < data_len; i++)
				{
					if(data[i] == '|')
						count++;
				}
				if(count > 0)
				{
					for(int j = 0; j < count; j++)
					{
						char temp[128];
						XP_API_MEMSET(temp, 0, 128);
						getValue(data, temp, j, '|');
						if(XP_API_STRCMP(temp, "gct") == 0)
						{
							XP_API_DELETE_ARRAY(m_gcTime);
							m_gcTime = XP_API_NEW char[128];
							XP_API_MEMSET(m_gcTime, 0, 128);
							getValue(data, m_gcTime, j + 1, '|');
							j++;
						}
						else if(XP_API_STRCMP(temp, "un") == 0)
						{
							char pTmp[128];
							XP_API_MEMSET(pTmp, 0, 128);
							getValue(data, pTmp, j + 1, '|');
							m_gl_user->setUserNumber(pTmp);
							j++;
						}
					}
				}
				//load save time
				char* temp2 = NULL;
				int len = 0;
				char saveName[32] = {0};
				XP_API_SPRINTF(saveName, "gcTime_%s", m_username);
				GLLiveStateTrophy::LoadFileFromLocal(saveName, temp2, len);
				char* localtime = XP_API_NEW char[len + 1];
				XP_API_MEMCPY(localtime, temp2, len);
				XP_API_DELETE_ARRAY(temp2);
				localtime[len] = 0;
				//compare
				m_isRereadGC = (XP_API_STRCMP(m_gcTime, localtime) != 0);
				XP_API_DELETE_ARRAY(localtime);
				if(m_isRereadGC)
				{
					//general condition read
					m_gl_user->sendGetGeneralCondition(LANG_FOR_SERVER[g_pGLLiveInstance->m_Language]);
					break;
				}
			}
			m_isLogin = true;
			m_gl_user->sendGetAttention();
#else
			m_loginUser.Clear();

			SetSubState(ONLINE_LOGIN);
			m_handler->PushGLLiveState(XP_API_NEW GLLiveStateMenu(m_handler));
			//m_gl_login->SendLogout();
#endif
			break;		
		case GL_XPLAYER_FUNCTION_LOGOUT:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Logout OK!!!\n" );
			//Go back to main menu after login out
			//m_handler->PopGLLiveState();
			SetSubState( ONLINE_LOGIN );
			break;
		case FUNCTION_REGISTER:
			m_pMsgBox->SetTitle(GetString(STR_SUCCEED));
			m_pMsgBox->SetText(GetString(STR_REG_OK));
			m_pMsgBox->SetButtonType(1);
			m_sub_state = ONLINE_ERROR;
			m_last_state = ONLINE_REGISTER;
			m_confirmLogin = true;
			m_totalLoginTime = -1;

			
			{
				//save the timestamp
				char saveName[32] = {0};
				XP_API_SPRINTF(saveName, "gcTime_%s", m_username);
				GLLiveStateTrophy::SaveFileToLocal(saveName, m_gcTime, XP_API_STRLEN(m_gcTime));
			}

			//m_gl_login->SendLogin(m_username,m_password,1, false, g_pGLLiveInstance->m_deviceToken, LANG_FOR_SERVER[g_pGLLiveInstance->m_Language]);
			//SetSubState(ONLINE_PROCESS);
			break;
		case FUNCTION_GET_USER_INFO:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get user info OK!\n" );
			//Check current avatar
			SAFE_DELETE(g_pAvatarImage);
			g_pAvatarImage = LoadPicFromLocal(m_gl_user->getAvatarId());
			if( g_pAvatarImage != NULL )
			{
				m_gl_user->sendGetAdvRSS();
			}
			else
			{
				m_gl_user->sendDownloadAvatar( m_gl_user->getAvatarId() );
			}
			break;
		case FUNCTION_DOWNLOAD_USER_AVATAR:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Download user avatar OK!\n" );
			SAFE_DELETE(g_pAvatarImage);
			if(data == NULL || data_len <= 0)
			{
				g_pAvatarImage = LoadPicFromLocal(NULL);
			}
			else
			{
				//Remove the 0x22 first
				data++;
				data_len--;

				char *	base64_buf = NULL;
				int		base64_size;
				base64_decode(data, data_len, base64_buf, base64_size);
				g_pAvatarImage = NEW CGLImage(base64_buf, base64_size);
				SavePicToLocal(m_gl_user->getAvatarId(), base64_buf, base64_size);
				XP_API_DELETE_ARRAY(base64_buf);
			}
			m_gl_user->sendGetAdvRSS();
			break;
		case FUNCTION_GET_USER_FRIENDS_COUNT:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get friend count OK!\n" );
			//Get new message count
			m_gl_message->SendGetMessageCount();
			break;
		case FUNCTION_GET_MESSAGE_COUNT:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get message count OK!\n" );
			//Get game total score
			m_gl_user->sendGetGameTrophyList();
			break;
		case FUNCTION_GET_USER_GAMELIST:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get game score OK!\n" );
#if 0
			//Login chat server
			//m_gl_chat->Login(m_gl_user->getUserName(), m_gl_user->getPassword());
			m_gl_user->sendGetAdvRSS();
#else
			m_gl_user->sendGetGameTrophyList();
#endif
			break;
		case FUNCTION_RETRIEVAL_PASSWORD:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Retrieval passowrd OK!\n" );
			//SetSubState(ONLINE_LOGIN);
			m_pMsgBox->SetTitle(GetString(STR_SUCCEED));
			m_pMsgBox->SetText(GetString(STR_FIND_PW));
			m_last_state = ONLINE_FORGET;
			m_sub_state = ONLINE_ERROR;
			m_retrievlNote = true;
			m_totalLoginTime = -1;
			break;
		case FUNCTION_GET_GAME_TROPHYLIST:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get current game all trophy list OK!\n" );
			//Parese trophy list
			m_trophyList = GLLiveStateTrophy::processTrophyList(data, data_len, m_trophyCount);
			if( UploadTrophyData(m_trophyList, m_trophyCount) < 0 )
			{
				//Login chat server
				//m_gl_chat->Login(m_gl_user->getUserName(), m_gl_user->getPassword());
				//m_gl_user->sendGetAdvRSS();
				m_gl_user->sendGetUserInfo();
			}
			break;
		case FUNCTION_AWARD_TROPHY:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Upload trophy list OK!\n" );
			//Login chat server
			//m_gl_chat->Login(m_gl_user->getUserName(), m_gl_user->getPassword());
			//m_gl_user->sendGetAdvRSS();
			m_gl_user->sendGetUserInfo();
			break;
		case FUNCTION_KEEP_ALIVE:
			break;
		case FUNCTION_GET_ADV_RSS:
		{
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get adv OK!\n" );
			//Parse data
			if(data == NULL || XP_API_STRLEN(data) <= 0)
			{
				//nothing to do
			}
			else
			{
				char item[1024] = {0};
				getXMLContent(item, data, "item");
				
				XP_API_MEMSET(g_pGLLiveInstance->m_AD_title, 0, sizeof(g_pGLLiveInstance->m_AD_title));
				getXMLContent(g_pGLLiveInstance->m_AD_title, item, "title");
				XP_API_MEMSET(g_pGLLiveInstance->m_AD_description, 0, sizeof(g_pGLLiveInstance->m_AD_description));
				getXMLContent(g_pGLLiveInstance->m_AD_description, item, "description");
				XP_API_MEMSET(g_pGLLiveInstance->m_AD_link, 0, sizeof(g_pGLLiveInstance->m_AD_link));
				getXMLContent(g_pGLLiveInstance->m_AD_link, item, "link");
				XP_API_MEMSET(g_pGLLiveInstance->m_AD_fileType, 0, sizeof(g_pGLLiveInstance->m_AD_fileType));
				getXMLContent(g_pGLLiveInstance->m_AD_fileType, item, "fileType");

				char temp[10];
				XP_API_MEMSET(temp, 0, sizeof(temp));
				getXMLContent(temp, item, "attachment_id");
				g_pGLLiveInstance->m_AD_attachmentID = XP_API_ATOI(temp);
				XP_API_MEMSET(temp, 0, sizeof(temp));
				getXMLContent(temp, item, "fileSize");
				g_pGLLiveInstance->m_AD_fileSize = XP_API_ATOI(temp);
				XP_API_MEMSET(temp, 0, sizeof(temp));
				getXMLContent(temp, item, "width");
				g_pGLLiveInstance->m_AD_width = XP_API_ATOI(temp);
				XP_API_MEMSET(temp, 0, sizeof(temp));
				getXMLContent(temp, item, "height");
				g_pGLLiveInstance->m_AD_height = XP_API_ATOI(temp);
				XP_API_MEMSET(temp, 0, sizeof(temp));
				getXMLContent(temp, item, "times");
				g_pGLLiveInstance->m_AD_times = XP_API_ATOI(temp);
			}
#ifdef WIN32
			m_gl_chat->Login(m_gl_user->getUserName(), m_gl_user->getPassword());
#else
#ifdef USE_AD_FROM_SERVER
			//Load from local first
			char  fileName[20];
			char *data = NULL;
			int   dataLen = 0;

			XP_API_MEMSET(fileName, 0, sizeof(fileName));
			XP_API_SPRINTF(fileName, "AD-%d.%s", g_pGLLiveInstance->m_AD_attachmentID, g_pGLLiveInstance->m_AD_fileType);
			
			if(GLLiveStateTrophy::LoadFileFromLocal(fileName, data, dataLen))
			{
				g_pGLLiveInstance->m_AD_file = NEW CGLImage(data, dataLen);
				XP_API_DELETE_ARRAY(data);
				//Login chat server
				m_gl_chat->Login(m_gl_user->getUserName(), m_gl_user->getPassword());
				//m_startTime = XP_API_GET_TIME();
			}
			else
			{
				m_gl_user->sendGetAdvAttachment(g_pGLLiveInstance->m_AD_attachmentID);
			}
#else
			XP_FILE_HANDLE pFile;
			char buffer[FILENAME_MAX];
			extern void GLLGetConfigFilePath(char* filePath, const char* filename);
			GLLGetConfigFilePath(buffer, "ad.png");
			pFile = XP_API_FILE_OPEN( buffer, "rb");
			if(pFile)
			{
				int dataLen = XP_API_FILE_GET_LENGTH(pFile);
				char* data = XP_API_NEW char[dataLen];
				XP_API_FILE_READ((void*)data,	1, dataLen,		pFile);
				XP_API_FILE_CLOSE(pFile);
				g_pGLLiveInstance->m_AD_file = NEW CGLImage(data, dataLen);
			}
			
			m_gl_chat->Login(m_gl_user->getUserName(), m_gl_user->getPassword());
#endif
#endif
		}
			break;
		case FUNCTION_GET_ADV_ATTACHMENT:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get attachment OK!\n" );
			//Parse data
			if(data == NULL || XP_API_STRLEN(data) <= 0)
			{
				//nothing to do
			}
			else
			{
				char *	base64_buf = NULL;
				int		base64_size;
				base64_decode(data, data_len, base64_buf, base64_size);
				g_pGLLiveInstance->m_AD_file = NEW CGLImage(base64_buf, base64_size);

				//Save to local
				char fileName[20];
				XP_API_MEMSET(fileName, 0, sizeof(fileName));
				XP_API_SPRINTF(fileName, "AD-%d.%s", g_pGLLiveInstance->m_AD_attachmentID, g_pGLLiveInstance->m_AD_fileType);
				GLLiveStateTrophy::SaveFileToLocal(fileName, base64_buf, base64_size);
				//Free base64 buf
				XP_API_DELETE_ARRAY(base64_buf);
			}
			//Login chat server
			m_gl_chat->Login(m_gl_user->getUserName(), m_gl_user->getPassword());
			break;
		case FUNCTION_GET_GENERAL_CONDITIONS:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get general condition OK!\n" );
			m_totalLoginTime = -1;
			XP_API_DELETE_ARRAY(m_gcContent);
			m_gcContent = XP_API_NEW char[data_len];
			XP_API_MEMSET(m_gcContent, 0, data_len);
			getValue(data, m_gcContent, 1, '|');
			XP_API_DELETE_ARRAY(m_gcTime);
			m_gcTime = XP_API_NEW char[128];
			XP_API_MEMSET(m_gcTime, 0, 128);
			getValue(data, m_gcTime, 3, '|');
			
			SetSubState(ONLINE_PROTOCOL);
			break;
		case FUNCTION_GET_CHAT_WARNING:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get attention OK!\n" );
			//Get Friend count
			m_gl_userFriend->sendGetUserFriendsCount();
			break;
		default:
			GLLiveState::OnRequestSuccess(func_id, data, data_len);
			break;		
	}
}

void GLLiveStateLogin::OnRequestFailure( int func_id, int error_code )
{
	if(func_id != FUNCTION_KEEP_ALIVE && func_id != GL_XPLAYER_FUNCTION_LOGOUT)
	{
		if(func_id == FUNCTION_GET_WEBCONFIG)
		{
		}
		else
		{
			//Set login flag to false, never keep alive
			if(m_gl_login)
				m_gl_login->SetLoggedIn(false);
		}
		m_totalLoginTime = -1;
		m_errorCode = error_code;
		SetSubState(ONLINE_ERROR);

		XP_DEBUG_OUT( "[GLLiveStateLogin] OnRequestFailure: %d\n", error_code);
	}

	switch(func_id)
	{
		case GL_XPLAYER_FUNCTION_LOGIN:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Login failed!!!\n" );
			break;		
		case GL_XPLAYER_FUNCTION_LOGOUT:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Logout failed!!!\n" );
			SetSubState( ONLINE_LOGIN );
			//m_handler->PopGLLiveState();	
			return;
		case FUNCTION_REGISTER:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Register failed!!!\n" );
			break;		
		case FUNCTION_GET_REPUTATION:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get reputation failed!\n" );
			break;
		case FUNCTION_GET_USER_INFO:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get user info failed!\n" );
			break;
		case FUNCTION_GET_USER_AVATAR:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get user avatar failed!\n" );
			break;
		case FUNCTION_GET_USER_FRIENDS_COUNT:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get friend count failed!\n" );
			break;
		case FUNCTION_GET_MESSAGE_COUNT:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get message count failed!\n" );
			break;
		case FUNCTION_RETRIEVAL_PASSWORD:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Retrieval passowrd failed!\n" );
			break;
		case FUNCTION_GET_GENERAL_CONDITIONS:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get general condition failed!\n" );
			break;
		case FUNCTION_GET_CHAT_WARNING:
			XP_DEBUG_OUT( "[GLLiveStateLogin] Get attention failed!\n" );
			break;
		default:
			GLLiveState::OnRequestFailure(func_id, error_code);
			break;
	}
}

//void GLLiveStateLogin::OnRequestTimeout(int func_id)
//{
//	m_errorCode = ERROR_REQUEST_TIME_OUT;
//	SetSubState(ONLINE_ERROR);
//}

void GLLiveStateLogin::SetSubState(GLLiveStateLogin::OnlineSubState newState)
{
	if(newState == m_sub_state)
	{
		return;
	}

	OnlineSubState tempState = m_last_state;
	//chang sub state flag
	m_last_state = m_sub_state;
	m_sub_state = newState;

	EnableGo(false);

	switch(newState)
	{
	case ONLINE_LOGIN:
		if(m_last_state != ONLINE_ERROR  )
		{
			//Init login menu
			if(m_loginMenu == NULL)
			{
				GLLiveMenuItem *item;
				_TouchRect rect;

				rect.Set(174, 250, 163, 63);
				m_loginMenu = XP_API_NEW GLLiveMenu( 0, 0);
				m_loginMenu->AddMenuItem( this, IDC_LOGIN, GetString(STR_LOGIN_LOGIN), rect );
				item = m_loginMenu->GetItem(0);
				item->SetFont(5);
				item->SetTitlePos(100, rect.height / 2, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER, 100);
				item->SetPic(SPRITE_MENU, Frame_menu_BUTTON_CONNECT, 0, 0);
				item->SetSelPic(SPRITE_MENU, Frame_menu_BG_CONNECT, 0, 0);

				rect.Set(341, 249, 62, 65);
				m_loginMenu->AddMenuItem( this, IDC_REGISTER, GetString(STR_REG_CREATE), rect );
				item = m_loginMenu->GetItem(1);
				item->SetFont(3, 0);
				item->SetTitlePos(rect.width / 2, -3, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE);
				item->SetPic(SPRITE_MENU, Frame_menu_BUTTON_CREATE, -65, 0);
				item->SetSelPic(SPRITE_MENU, Frame_menu_BG_CREATE, -65, 0);

				rect.left = 406;
				m_loginMenu->AddMenuItem( this, IDC_FORGET, GetString(STR_LOGIN_FORGET), rect );
				item = m_loginMenu->GetItem(2);
				item->SetFont(3, 0);
				item->SetTitlePos(rect.width / 2, -3, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE);
				item->SetPic(SPRITE_MENU, Frame_menu_BUTTON_FORGOT);
				item->SetSelPic(SPRITE_MENU, Frame_menu_BG_FORGOT);

				rect.Set(58, 210, 210, 25);
				m_loginMenu->AddMenuItem(this, IDC_REMEMBER_ME, GetString(STR_REMEMBER_ME), rect);
				item = m_loginMenu->GetItemById(IDC_REMEMBER_ME);
				item->SetPic(SPRITE_MENU, Frame_menu_CHECK, rect.left, rect.top);
				item->SetSelPic(SPRITE_MENU, Frame_menu_CHECK_ON, rect.left - 5, rect.top - 5);
				item->SetFont(3);
				item->SetTitlePos(30, rect.height / 2, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);

				//Add edit box
				rect.Set(58, 117 - 25, 366, 28 + 25);
				m_loginMenu->AddMenuItem(this, IDE_LOGIN_USERNAME, GetString(STR_LOGIN_USERNAME), rect);
				item = m_loginMenu->GetItemById(IDE_LOGIN_USERNAME);
				item->SetFont(6);
				item->SetTitlePos(8, 5, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);

				rect.top = 174 - 25;
				m_loginMenu->AddMenuItem(this, IDE_LOGIN_PASSWORD, GetString(STR_LOGIN_PASSWORD), rect);
				item = m_loginMenu->GetItemById(IDE_LOGIN_PASSWORD);
				item->SetFont(6);
				item->SetTitlePos(8, 5, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
			}

			//Init edit user data
			XP_API_MEMSET(m_username, 0, sizeof(m_username));
			XP_API_MEMSET(m_password, 0, sizeof(m_password));
		}
		break;
	case ONLINE_REGISTER:
		if(m_last_state != ONLINE_ERROR && m_last_state != ONLINE_PROCESS )
		{
			//Init register menu
			if(m_regMenu == NULL)
			{
				GLLiveMenuItem *item;
				_TouchRect rect;

				m_regMenu = XP_API_NEW GLLiveMenu( 0, 0);
				rect.Set(406, 249, 62, 65);
				m_regMenu->AddMenuItem(this, IDC_CREATE, GetString(STR_REG_CREATE), rect);
				item = m_regMenu->GetItem(0);
				item->SetPic(SPRITE_MENU, Frame_menu_BUTTON_CREATE);
				item->SetSelPic(SPRITE_MENU, Frame_menu_BG_CREATE);
				item->SetFont(3, 0);
				item->SetTitlePos(rect.width / 2, -3, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE);

				//Add edit box
				rect.Set(36, 118 - 20, 200, 28 + 20);
				m_regMenu->AddMenuItem(this, IDE_REG_USERNAME, GetString(STR_REG_USERNAME), rect);
				item = m_regMenu->GetItemById(IDE_REG_USERNAME);
				item->SetFont(6);
				item->SetTitlePos(8, 5, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);

				rect.left = 243;
				m_regMenu->AddMenuItem(this, IDE_REG_EMAIL, GetString(STR_REG_EMAIL), rect);
				item = m_regMenu->GetItemById(IDE_REG_EMAIL);
				item->SetFont(6);
				item->SetTitlePos(8, 5, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);

				rect.left = 35;
				rect.top = 175 - 20;
				m_regMenu->AddMenuItem(this, IDE_REG_PASSWORD, GetString(STR_REG_PASSWORD), rect);
				item = m_regMenu->GetItemById(IDE_REG_PASSWORD);
				item->SetFont(6);
				item->SetTitlePos(8, 5, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);

				rect.left = 243;
				m_regMenu->AddMenuItem(this, IDE_REG_CONFIRM, GetString(STR_REG_CONFIRM), rect);
				item = m_regMenu->GetItemById(IDE_REG_CONFIRM);
				item->SetFont(6);
				item->SetTitlePos(8, 5, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);

				//Add check box
				rect.Set(35, 207, 300, 25);
				m_regMenu->AddMenuItem(this, IDC_REG_EMAIL_FLAG, GetString(STR_REG_EMAILFLAG), rect);
				item = m_regMenu->GetItemById(IDC_REG_EMAIL_FLAG);
				item->SetPic(SPRITE_MENU, Frame_menu_CHECK, rect.left, rect.top);
				item->SetSelPic(SPRITE_MENU, Frame_menu_CHECK_ON, rect.left - 5, rect.top - 5);
				item->SetFont(3);
				item->SetTitlePos(30, rect.height / 2, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
			}
			//Init user data
			XP_API_MEMSET(m_username, 0, sizeof(m_username));
			XP_API_MEMSET(m_password, 0, sizeof(m_password));
			XP_API_MEMSET(m_confirmPwd, 0, sizeof(m_confirmPwd));
			XP_API_MEMSET(m_email, 0, sizeof(m_email));
			SetEmailFlag(false);
		}
		break;
	case ONLINE_FORGET:
		EnableGo(true);
		m_commandBar->GetItem(1)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_SEND);
		m_commandBar->GetItem(1)->SetSelPic(SPRITE_MENU, Frame_menu_BG_SEND);
		m_commandBar->GetItem(1)->SetTitle(GetString(STR_SEND));

		if(m_forgotMenu == NULL)
		{
			_TouchRect rect;
			rect.Set(55, 120, 366, 28 + 25);
			m_forgotMenu = XP_API_NEW GLLiveMenu(0, 0);
			m_forgotMenu->AddMenuItem(this, IDE_FORGOT_USERNAME, GetString(STR_LOGIN_USERNAME), rect);
			m_forgotMenu->GetItem(0)->SetFont(6);
			m_forgotMenu->GetItem(0)->SetTitlePos(8, 5, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
		}
		XP_API_MEMSET(m_username, 0, sizeof(m_username));
		break;
	case ONLINE_ERROR:
		m_pMsgBox->SetTitle(GetString(STR_ERROR));
		m_pMsgBox->SetText(GetErrorString(m_errorCode));
		if(m_last_state == ONLINE_PROCESS)
		{
			m_last_state = tempState;
		}
		break;
	case ONLINE_PROCESS:
		//m_pMsgBox->SetText("Please Waiting...");
		//m_pMsgBox->SetTitle(GetString(STR_WAITING));
		break;
	case ONLINE_PROTOCOL:
		{
		m_isAcceptProtocol = false;
		m_protocolMenu->GetItemById(IDC_REG_ACCEPT)->SetPic(SPRITE_MENU, Frame_menu_CHECK, 90, 240);
		m_protocolMenu->GetItemById(IDC_REG_CONTINUE)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_CONTINUE2 + 3 * g_pGLLiveInstance->getLang());
		m_protocolMenu->GetItemById(IDC_REG_CONTINUE)->SetEnable(false);

		//Scroll Bar
		m_totalHeight = m_handler->GetFont(6)->InitPage(m_gcContent, 415) * m_handler->GetFont(6)->GetLineHeight();
		m_scrollBarH = 160 * 160 / m_totalHeight;
		m_scrollBarPos = 0;
		//Animation data
		m_offset = 0;
		m_oldOffset = 0;
		m_touchPos = 0;         //Y-
		m_moving = 0;
		m_speed = 0;
		m_scrollMoving = false;
		}
		break;
	}
}

bool GLLiveStateLogin::LoadProfile()
{
	return CGLLive::ReadUserInfo(m_username, sizeof(m_username), m_password, sizeof(m_username), &m_timestamp);
}

bool GLLiveStateLogin::SaveProfile()
{
	return CGLLive::SaveUserInfo(m_bAutoLogin, m_username, m_password, m_timestamp);
}

void GLLiveStateLogin::OnChatConnected()
{
	XP_DEBUG_OUT( "[GLLiveStateLogin] Login chat server OK!\n" );
	if(m_totalLoginTime > -1)
	{
//		SaveProfile();
		//Go to main menu
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateMenu(m_handler));
		//Enter profile state after register
		if(m_last_state == ONLINE_REGISTER)
		{
			m_handler->PushGLLiveState(XP_API_NEW GLLiveStateProfile(m_handler));
		}
		SetSubState(ONLINE_LOGIN);
		m_totalLoginTime = -1;
		XP_API_MEMSET(m_username, 0, sizeof(m_username));
		XP_API_MEMSET(m_password, 0, sizeof(m_password));
	}
	else
	{
		XP_DEBUG_OUT( "[GLLiveStateLogin] But it have been timeout already!\n" );
		//m_gl_chat->Logout();
	}
}

void GLLiveStateLogin::OnChatDisconnect(int e)
{
	XP_DEBUG_OUT( "[GLLiveStateLogin] Login chat server failed, e_code = %d!\n", e );
	if(m_sub_state == ONLINE_PROCESS)
	{
		m_last_state = ONLINE_LOGIN;
		m_errorCode = e;
		SetSubState(ONLINE_ERROR);
	}
}

int GLLiveStateLogin::UploadTrophyData(TrophyInfo *& trophyList, int trophyCount)
{
	int score = 0;
	//Load form local
	bool *trophyData = CGLLive::LoadTrophyData(m_gl_user->getUserNumber());
	if(trophyData == NULL)
	{
		XP_API_DELETE_ARRAY(trophyList);
		return -1;
	}

	//Generate and upload trophy
	int *trophyIdList = XP_API_NEW int[trophyCount];
	XP_API_MEMSET(trophyIdList, 0, sizeof(int) * trophyCount);//15851
	int count = 0;
	for(int i = 0; i < trophyCount; i++)
	{
		if(trophyData[i])
		{
			trophyIdList[count] = trophyList[i].id;
			score += trophyList[i].score;
			count++;
		}
	}

	if(count < 1)
	{
		XP_API_DELETE_ARRAY(trophyData);
		XP_API_DELETE_ARRAY(trophyIdList);
		XP_API_DELETE_ARRAY(trophyList);
		return -1;
	}

	m_gl_user->sendAwardTrophy(trophyIdList, count);

	XP_API_DELETE_ARRAY(trophyData);
	XP_API_DELETE_ARRAY(trophyIdList);
	XP_API_DELETE_ARRAY(trophyList);
	trophyCount = 0;
	return score;
}

void GLLiveStateLogin::SendCreate()
{
	if(gll_country[0] == '\0')
	{
		gll_country[0] = 'U';
		gll_country[1] = 'S';
		gll_country[2] = '\0';
	}

	m_gl_user->sendRegister(m_username,m_password, m_email, gll_country, m_emailFlag, 
							g_pGLLiveInstance->m_deviceToken, LANG_FOR_SERVER[g_pGLLiveInstance->m_Language], g_pGLLiveInstance->m_UDID);
	m_totalLoginTime = XP_API_GET_TIME();
	SetSubState(ONLINE_PROCESS);
	m_last_state = ONLINE_REGISTER;
}

void GLLiveStateLogin::SetRememberMe(bool rem)
{
	//if(rem == m_bAutoLogin)
	//{
	//	return;
	//}

	GLLiveMenuItem *pItem = m_loginMenu->GetItemById(IDC_REMEMBER_ME);

	if(rem)
	{
		pItem->SetPic(SPRITE_MENU, Frame_menu_CHECKED, pItem->GetRect().left, pItem->GetRect().top);
	}
	else
	{
		pItem->SetPic(SPRITE_MENU, Frame_menu_CHECK, pItem->GetRect().left, pItem->GetRect().top);
	}

	m_bAutoLogin = rem;
}

void GLLiveStateLogin::SetEmailFlag(bool flag)
{
	if(flag == m_emailFlag)
	{
		return;
	}

	m_emailFlag = flag;

	if(m_regMenu == NULL)
	{
		return;
	}

	GLLiveMenuItem *pItem = m_regMenu->GetItemById(IDC_REG_EMAIL_FLAG);

	if(flag)
	{
		pItem->SetPic(SPRITE_MENU, Frame_menu_CHECKED, pItem->GetRect().left, pItem->GetRect().top);
	}
	else
	{
		pItem->SetPic(SPRITE_MENU, Frame_menu_CHECK, pItem->GetRect().left, pItem->GetRect().top);
	}

	
}