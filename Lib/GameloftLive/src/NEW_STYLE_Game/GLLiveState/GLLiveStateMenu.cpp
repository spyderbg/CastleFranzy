#include "GLLiveStdafx.h"
#include "GLLiveStateMenu.h"
#include "GLLiveStateLogin.h"
#include "GLLiveStateProfile.h"
#include "GLLiveStateTrophy.h"
#include "GLLiveStateMessage.h"
#include "GLLiveStateFriends.h"
#include "GLLiveStateChat.h"
#include "Porting.h"
#include "GLLive_Font.h"

//const int 15 = 15;
#define REFRESH_TIME      60000

#ifdef WIN32
void changeIconBadgeNumber(int msgNumber){}
#else
extern void changeIconBadgeNumber(int msgNumber);
#endif
extern CGLImage * g_pAvatarImage;

GLLiveStateMenu::GLLiveStateMenu( GLLiveStateHandler * handler) : GLLiveState( handler )
{
	m_isEnterFriendList = false;
	m_bLogout = false;
	m_bPressedLogout = false;
	m_bUpdating = false;
	m_startTime = XP_API_GET_TIME();;
	//Init rect
	m_trAvatar.Set(20, 30, 97, 97);
	m_trUserInfo.Set(136, 60, 125, 33);
	m_trNews.Set(193, 203, 285, 70);

	//Init menu
	m_menu = XP_API_NEW GLLiveMenu(0,0);
	_TouchRect rect;

	rect.Set(326, 10, 69, 71);
	m_menu->AddMenuItem(this, IDC_TROPHY, GetString(TITLE_5), rect);
	m_menu->GetItem(0)->SetSelPic(SPRITE_MENU, Frame_menu_BG_TROPHY);

	rect.left = 252;
	m_menu->AddMenuItem(this, IDC_PROFILE, GetString(STR_PROFILE), rect);
	m_menu->GetItem(1)->SetSelPic(SPRITE_MENU, Frame_menu_BG_PROFILE);

	rect.left = 400;
	m_menu->AddMenuItem(this, IDC_LOGOUT, GetString(STR_LOGOUT), rect);
	m_menu->GetItem(2)->SetSelPic(SPRITE_MENU, Frame_menu_BG_LOGOUT);

	rect.top = 96;
	m_menu->AddMenuItem(this, IDC_MESSAGE, GetString(STR_MESSAGE), rect);
	m_menu->GetItem(3)->SetSelPic(SPRITE_MENU, Frame_menu_BG_MESSAGE);

	rect.left = 252;
	m_menu->AddMenuItem(this, IDC_CHAT_ROOM, GetString(STR_CHAT), rect);
	m_menu->GetItem(4)->SetSelPic(SPRITE_MENU, Frame_menu_BG_CHAT);

	rect.left = 326;
	m_menu->AddMenuItem(this, IDC_FRIEND, GetString(STR_FRIEND), rect);
	m_menu->GetItem(5)->SetSelPic(SPRITE_MENU, Frame_menu_BG_FRIEND);

	for(int i = 0; i < 6; i++)
	{
		m_menu->GetItem(i)->SetFont(3, 0);
		m_menu->GetItem(i)->SetTitlePos(rect.width / 2, rect.height, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);
	}

	rect.Set(285, 281, 244, 28);
	m_menu->AddMenuItem(this, IDC_LEADER_BOARD, GetString(STR_LEADER_BOARD), rect);
	m_menu->GetItem(6)->SetPic(SPRITE_MENU, Frame_menu_Leaderboard);
	m_menu->GetItem(6)->SetSelPic(SPRITE_MENU, Frame_menu_Leaderboard + 1);
	m_menu->GetItem(6)->SetFont(5);



	//Init confirm menu
	m_confirmMenu = CGLLiveMessageBox::GenerateConfirmMenu(this);


	//Init messagebox
	if(m_pMsgBox == NULL)
	{
		m_pMsgBox = XP_API_NEW CGLLiveMessageBox();
	}

	EnableGo(false);
	m_commandBar->GetItem(0)->SetTitle("");

	m_sub_state = MAIN_NORMAL;

	//If there is new message, show notify
	int newMsgCount = GetNewMsgCount();
	changeIconBadgeNumber(newMsgCount);
	if(newMsgCount > 0)
	{
		m_sub_state = MAIN_NOTIFY;
	}

	m_trophyList = NULL;
	m_trophyCount = 0;
	m_isEnterTrophy = false;

	//Disable leader borad if nesseary
	m_menu->GetItemById(IDC_LEADER_BOARD)->SetVisible(g_pGLLiveInstance->m_canEnterLeaderboard);
	m_isEnterLeaderBoard = false;
}

GLLiveStateMenu::~GLLiveStateMenu(void)
{
	XP_API_DEL(m_menu);
	XP_API_DEL(m_confirmMenu);
	XP_API_DELETE_ARRAY(m_trophyList);
}

bool GLLiveStateMenu::HandleEvent( _TOUCH_EVENT event )
{
	//if message box is shown, ignore all event
	switch(m_sub_state)
	{
	case MAIN_NORMAL:
		if(event.eType == TET_Pressed)
		{
			//click avatar to set profile
			if(m_trAvatar.Contain(event.position))
			{
				m_sub_state = MAIN_PROCESS;
				m_gl_user->sendGetUserInfo();
				//m_handler->PushGLLiveState(XP_API_NEW GLLiveStateProfile(m_handler));
				return true;
			}
			_TouchRect rect;
			rect.Set(130, 65, 120, 35);
			if(rect.Contain(event.position))
			{
				//Open reputation details
				m_sub_state = MAIN_PROCESS;

				m_gl_user->sendGetReputation();
				return true;
			}
		}
		if(m_menu->HandleEvent(event))
		{
			return true;
		}
		return GLLiveState::HandleEvent(event);
	case MAIN_ERROR:
		if(event.eType == TET_Pressed)
		{
			m_sub_state = MAIN_NORMAL;
			return true;
		}
		break;
	case MAIN_PROCESS:
		return true;
	case MAIN_NOTIFY:
		if(event.eType == TET_Pressed)
		{
			m_sub_state = MAIN_NORMAL;
			_TouchRect rect;
			rect.Set(99, 64, 282, 200);
			if(rect.Contain(event.position))
			{
				m_sub_state = MAIN_PROCESS;
				m_gl_message->SendGetMessageHeader(63, 0, 0, 0);
				return true;
			}
		}
		break;
	case MAIN_REPUTATION_DETAIL:
		if(event.eType == TET_Pressed)
		{
			m_sub_state = MAIN_NORMAL;
		}
		break;
	case MAIN_BIND_CONFIRM:
		return m_confirmMenu->HandleEvent(event);
	default:
		break;
	}
	return false;
}

void GLLiveStateMenu::SwitchState()
{
	m_bUpdating = true;
	m_sub_state = MAIN_PROCESS;
	m_gl_user->sendGetReputation();
}

void GLLiveStateMenu::PaintMain()
{
	CGLLiveGraphics * g = m_handler->GetGraphics();
	CGLLiveFont     * f = m_handler->GetFont(3);
	CGLLiveFont     * f2= m_handler->GetFont(1);
	CGLLiveFont     * f5= m_handler->GetFont(4);
	CGLLiveFont     * f1= m_handler->GetFont(0);

	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BUTTON_MAIN, 0, 0, 0);
	//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_main, 0, 0, 0);
#ifdef WIN32
#else
	if(g_pGLLiveInstance->m_AD_file != NULL)
	{
		g->DrawRegion(g_pGLLiveInstance->m_AD_file, 
				0, 0, 200, 146, 
				CGLLiveGraphics::TRANS_MIRROR_ROT180, 
				0, 320 - 146, 0);
	}
#endif
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_AD_COVER, 0, 320 - 146, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_NEWS_TITLE, 0, 0, 0, 0, 0);

	m_menu->Repaint(m_handler);

	//Draw news title
	f2->DrawString(g, GetString(STR_MAIN_NEWS), 203, 195, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);

	//Draw news
	if(g_pGLLiveInstance->m_AD_title != NULL)
	{
		f5->InitPage(g_pGLLiveInstance->m_AD_title, m_trNews.width);
		f5->DrawPage(g, g_pGLLiveInstance->m_AD_title, m_trNews.left + m_trNews.width / 2, m_trNews.top + m_trNews.height / 2, 
			CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER);
	}

	//Draw user name
	f1->DrawString(g, m_gl_user->getUserName(), 136, 30, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP, 105);

	//Draw reputation
	for(int i = 0; i < 5; i++)
	{
		if(i < m_gl_user->getUserReputation())
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_REPUTATION, m_trUserInfo.left + 21 * i, m_trUserInfo.top, 0);
		}
		else
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_REPUTATION_NULL, m_trUserInfo.left + 21 * i, m_trUserInfo.top, 0);
		}
	}

	//Draw trophy score
	f1->DrawNumber(g, m_gl_user->getTotalTrophy(), 136, 100, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCORE_UNIT_MID, 136 + f1->GetStringSize() + 3, 95, 0);

	//Draw avatar
	PaintAvatar(g_pAvatarImage, m_trAvatar.left, m_trAvatar.top, 97, 97);

	//Draw friend count
	if(m_gl_userFriend->GetFriendsCount() > 99)
		f2 = m_handler->GetFont(FONT_WHITE_SMALL);
	else
		f2 = m_handler->GetFont(FONT_WHITE_MID);
	f2->DrawNumber(g, m_gl_userFriend->GetFriendsCount(), 383, 164, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE);
	//Draw unread msg count
	if(GetNewMsgCount() > 99)
		f2 = m_handler->GetFont(FONT_WHITE_SMALL);
	else
		f2 = m_handler->GetFont(FONT_WHITE_MID);
	f2->DrawNumber(g, GetNewMsgCount(), 457, 164, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE);

	GLLiveState::Paint();
}

void GLLiveStateMenu::PaintReputationDetail()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     * fTitle = m_handler->GetFont(2);
	CGLLiveFont     * fText= m_handler->GetFont(6);

	PaintMain();

	g->EndDraw2D();
	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
	g->EnableTextureBlend(true);
	g->SetARGBColor(0x80000000);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0);
	g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	//Paint evaluate bg
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_evaluate, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_good, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_bad, 0, 0 , 0, 0, 0);
	//Paint title
	fTitle->DrawString(g, GetString(STR_REPUTATION), 200, 100, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);

	char tmp[32] = {0};
	XP_API_SPRINTF(tmp, "%d %s",m_gl_user->getUserGoodReputationCount() , m_gl_user->getUserGoodReputationCount() != 1 ? GetString(STR_VOTES) : GetString(STR_VOTE));
	fText->DrawString(g, tmp, 182, 220, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER);

	XP_API_SPRINTF(tmp, "%d %s",m_gl_user->getUserBadReputationCount(), m_gl_user->getUserBadReputationCount() != 1 ? GetString(STR_VOTES) : GetString(STR_VOTE));
	fText->DrawString(g, tmp, 298, 220, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER);
}

void GLLiveStateMenu::PaintNotify()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     * fTitle = m_handler->GetFont(2);
	CGLLiveFont     * fText= m_handler->GetFont(6);

	PaintMain();

	g->EndDraw2D();
	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
	g->EnableTextureBlend(true);
	g->SetARGBColor(0x80000000);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0);
	g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_MAIN_MSG, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_MAIN_MSG, 0, 0, 0, 0, 0);

	//Paint title
	fTitle->DrawString(g, GetString(STR_NEWMSG), 210, 87, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);

	char temp[32];
	XP_API_MEMSET(temp, 0, sizeof(temp));
	XP_API_SPRINTF(temp, GetString(STR_NEWMSG_NOTE), GetNewMsgCount());
	fText->DrawString(g, temp, 240, 220, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);
}

void GLLiveStateMenu::Paint()
{
	m_handler->ClearScreen();
	switch(m_sub_state)
	{
	case MAIN_NORMAL:
		PaintMain();
		break;
	case MAIN_NOTIFY:
		PaintNotify();
		break;
	case MAIN_ERROR:
		PaintMain();
		m_pMsgBox->Repaint(m_handler);
		break;
	case MAIN_PROCESS:
		PaintMain();
		PaintLoading();
		break;
	case MAIN_REPUTATION_DETAIL:
		PaintReputationDetail();
		break;
	case MAIN_BIND_CONFIRM:
		PaintMain();
		m_pMsgBox->Repaint(m_handler);
		m_confirmMenu->Repaint(m_handler);
		break;
	default:
		break;
	}
}

void GLLiveStateMenu::UpdateAnimation()
{
	if(m_sub_state == MAIN_BIND_CONFIRM)
	{
		m_confirmMenu->Update();
		GLLiveState::UpdateAnimation();
		return;
	}
	if(m_menu->Update())
	{
		GLLiveState::UpdateAnimation();
		return;
	}
	GLLiveState::UpdateAnimation();

	if(!m_bUpdating)
	{
		long curTime = XP_API_GET_TIME();
		if(curTime - m_startTime > REFRESH_TIME)
		{
			m_bUpdating = true;
			m_gl_user->sendGetReputation();
		}
	}
}

void GLLiveStateMenu::OnMenuItemClicked( int id )
{
	switch(id)
	{
	case IDC_BACK:
		m_sub_state = MAIN_PROCESS;
		m_bLogout = false;
		//Send login out
		XP_DEBUG_OUT( "[GLLiveStateMenu] Login out...\n" );
		m_gl_login->SendLogout();
		m_gl_chat->Logout();
		m_bPressedLogout = true;
		break;
	case IDC_TROPHY:
	{
		//else
		{
			m_sub_state = MAIN_PROCESS;
			m_gl_user->sendGetUserGameList();
		}
		break;
	}
	case IDC_MESSAGE:
		m_sub_state = MAIN_PROCESS;
		m_gl_message->SendGetMessageHeader(63, 0, 0, 0);
		//m_handler->PushGLLiveState(XP_API_NEW GLLiveStateMessage(m_handler));
		break;
	case IDC_CHAT_ROOM:
	{
		//Create chat room
		char pTmp[32];
		XP_API_SPRINTF(pTmp, "1-%s", m_gl_user->getUserName());
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateChat(m_handler, m_gl_chat->CreateChatRoom(pTmp, m_gl_user->getUserName())));
	}
		return;
	case IDC_FRIEND:
		m_isEnterFriendList = true;
		m_sub_state = MAIN_PROCESS;
		m_gl_user->sendGetUserGameList();
		return;
	case IDC_LOGOUT:
		m_sub_state = MAIN_PROCESS;
		m_bLogout = true;
		//Send login out
		XP_DEBUG_OUT( "[GLLiveStateMenu] Login out...\n" );
		m_gl_login->SendLogout();
		m_gl_chat->Logout();
		m_bPressedLogout = true;
		return;
	case IDC_PROFILE:
		m_sub_state = MAIN_PROCESS;
		m_gl_user->sendGetUserInfo();
		//m_handler->PushGLLiveState(XP_API_NEW GLLiveStateProfile(m_handler));
		return;
	case IDC_MSG_YES:
		//Never bind----Bind and upload trophy
		if(CGLLive::ReadTrophyAccount() == -1)
		{
			CGLLive::BindTrophyAccount(m_gl_user->getUserNumber());
			m_sub_state = MAIN_PROCESS;
			m_gl_user->sendGetGameTrophyList();
		}
		else
		{
			CGLLive::BindTrophyAccount(m_gl_user->getUserNumber());
			m_sub_state = MAIN_PROCESS;
			m_gl_user->sendGetUserGameList();
		}
		return;
	case IDC_MSG_NO:
		m_sub_state = MAIN_NORMAL;
		return;
	case IDC_LEADER_BOARD:
		m_isEnterLeaderBoard = true;
		m_sub_state = MAIN_PROCESS;
		m_gl_login->SendLogout();
		return;
	}
}


//void GLLiveStateMenu::OnNetworkError()
//{
//	m_pMsgBox->SetTitle(GetString(STR_ERROR));
//	m_pMsgBox->SetText(GetErrorString(ERROR_CONNECTION));
//	m_sub_state = MAIN_ERROR;
//	XP_DEBUG_OUT( "[GLLiveStateMenu] OnNetworkError.\n");
//}

//void GLLiveStateMenu::OnRequestTimeout( int func_id )
//{
//	m_pMsgBox->SetTitle(GetString(STR_ERROR));
//	m_pMsgBox->SetText(GetErrorString(ERROR_REQUEST_TIME_OUT));
//	m_sub_state = MAIN_ERROR;
//	XP_DEBUG_OUT( "[GLLiveStateMenu] OnRequestTimeout.\n");
//}

void GLLiveStateMenu::OnRequestSuccess( int func_id, char * data, int data_len )
{
	switch(func_id)
	{
	case FUNCTION_LOGOUT:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Login out OK!\n" );
		if(m_isEnterLeaderBoard)
		{
			m_isEnterLeaderBoard = false;
#ifndef WIN32
			extern void enterLeaderBoard();
			enterLeaderBoard();
#endif
			m_handler->TerminateGame();
			return;
		}
		else if(m_bLogout)
		{
			m_bAutoLogin = false;
			m_handler->PopGLLiveState();
		}
		else
		{
			m_handler->TerminateGame();
		}
		return;
	case FUNCTION_GET_MESSAGE_HEADER:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get Message list OK!\n" );
		//convert '\1' to '^'
		for(int j = 0; j < m_gl_message->getMsgListCount(); j++)
		{
			if(m_gl_message->getMsgType(j) == USER_MESSAGE_TYPE_USERMESSAGE)
			{
				char* temp = NULL;
				int size;
				base64_decode(m_gl_message->getMsgTitle(j), XP_API_STRLEN(m_gl_message->getMsgTitle(j)), temp, size);
				memcpy(m_gl_message->getMsgTitle(j), temp, size);
				m_gl_message->getMsgTitle(j)[size] = 0;
				XP_API_DEL(temp);
			}
//			for(int i = 0; i < XP_API_STRLEN(m_gl_message->getMsgTitle(j)); i++)
//			{
//				if(m_gl_message->getMsgTitle(j)[i] == 1)
//					m_gl_message->getMsgTitle(j)[i] = '^';
//			}
		}
		m_sub_state = MAIN_NORMAL;
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateMessage(m_handler));
		break;
	case FUNCTION_GET_USER_FRIENDS:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get friends list OK!\n" );
		m_sub_state = MAIN_NORMAL;
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateFriends(m_handler));
		break;
	case FUNCTION_GET_USER_FRIENDS_COUNT:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get friend count OK!\n" );
		m_sub_state = MAIN_NORMAL;
		if(m_bUpdating)
		{
			m_bUpdating = false;
			//Reset time
			m_startTime = XP_API_GET_TIME();
		}
		break;
	case FUNCTION_GET_MESSAGE_COUNT:
		changeIconBadgeNumber(GetNewMsgCount());
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get Message count OK!\n" );
		//m_gl_userFriend->sendGetUserFriends();
		m_gl_userFriend->sendGetUserFriendsCount();
		break;
	case FUNCTION_GET_USER_INFO:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get user info OK!\n" );
		if(m_isEnterTrophy)
		{
			m_isEnterTrophy = false;
			//Begin to go to trophy state
			m_gl_user->sendGetUserGameList();
		}
		else
		{
			m_sub_state = MAIN_NORMAL;
			m_handler->PushGLLiveState(XP_API_NEW GLLiveStateProfile(m_handler));
		}
		return;
	case FUNCTION_GET_USER_GAMELIST:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get user game list OK!\n" );
		if(m_isEnterFriendList)
		{
			m_isEnterFriendList = false;
			//m_handler->PushGLLiveState(XP_API_NEW GLLiveStateFriends(m_handler));
			m_sub_state = MAIN_PROCESS;
			m_gl_userFriend->sendGetUserFriends();
		}
		else
		{
			int bindUserNo = CGLLive::ReadTrophyAccount();
			if(bindUserNo != m_gl_user->getUserNumber())
			{
				m_pMsgBox->SetTitle(GetString(STR_WARNING));

				char msg[256];
				//Never Bind
				if(bindUserNo == -1)
				{
					XP_API_SPRINTF(msg, GetString(STR_TROPHY_CONFRIM), m_gl_user->getUserGameName(0), m_gl_user->getUserGameName(0));
				}
				//Change Bind
				else
				{
					XP_API_SPRINTF(msg, GetString(STR_TROPHY_CHANGE), m_gl_user->getUserGameName(0), m_gl_user->getUserGameName(0));
				}
				m_pMsgBox->SetText(msg);
				m_pMsgBox->SetButtonType(1);
				m_sub_state = MAIN_BIND_CONFIRM;
			}
			else
			{
				m_sub_state = MAIN_NORMAL;
				m_handler->PushGLLiveState(XP_API_NEW GLLiveStateTrophy(m_handler));
				//m_gl_user->sendGetGameTrophyList(XP_API_ATOI(m_gl_user->getUserGamePID(0)));
			}
		}
		return;
	case FUNCTION_GET_GAME_TROPHYLIST:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get cur game trophy list OK!\n" );
		//Parese trophy list
		m_trophyList = GLLiveStateTrophy::processTrophyList(data, data_len, m_trophyCount);
		if( GLLiveStateLogin::UploadTrophyData(m_trophyList, m_trophyCount) < 0 )
		{
			m_gl_user->sendGetUserGameList();
		}
		return;
	case FUNCTION_GET_REPUTATION:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get friend count OK!\n" );
		if(m_bUpdating)
		{
			m_gl_message->SendGetMessageCount();
		}
		else
		{
			m_sub_state = MAIN_REPUTATION_DETAIL;
		}
		break;
	case FUNCTION_AWARD_TROPHY:
		XP_DEBUG_OUT( "[GLLiveStateLogin] Upload trophy list OK!\n" );
		m_gl_user->sendGetUserInfo();
		m_isEnterTrophy = true;
		break;
	default:
		GLLiveState::OnRequestSuccess(func_id, data, data_len);
		break;
	}
}

void GLLiveStateMenu::OnRequestFailure( int func_id, int error_code )
{
	if(func_id != FUNCTION_KEEP_ALIVE)
	{
		m_pMsgBox->SetTitle(GetString(STR_ERROR));
		m_pMsgBox->SetText(GetErrorString(error_code));
		m_sub_state = MAIN_ERROR;
	}
	switch(func_id)
	{
	case FUNCTION_LOGOUT:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Login out failed!\n" );
		if(m_isEnterLeaderBoard)
		{
			m_isEnterLeaderBoard = false;
#ifndef WIN32
			extern void enterLeaderBoard();
			enterLeaderBoard();
#endif
			m_handler->TerminateGame();
			return;
		}
		if(m_bLogout)
		{
			m_bAutoLogin = false;
			m_handler->PopGLLiveState();
		}
		else
		{
			m_handler->TerminateGame();
		}
		return;
	case FUNCTION_GET_MESSAGE_HEADER:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get Message list failed\n" );
		break;
	case FUNCTION_GET_USER_FRIENDS:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get friends list failed!\n" );
		break;
	case FUNCTION_GET_USER_FRIENDS_COUNT:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get friend count failed!\n" );
		break;
	case FUNCTION_GET_MESSAGE_COUNT:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Get Message count failed!\n" );
		break;
	case FUNCTION_AWARD_TROPHY:
		XP_DEBUG_OUT( "[GLLiveStateMenu] Award trophy failed!\n" );
		break;
	case 0:
		if(error_code = ERROR_CONNECTION && m_bPressedLogout)
		{
			XP_DEBUG_OUT( "[GLLiveStateMenu] Login out failed!\n" );
			m_bAutoLogin = false;
			if(m_bLogout)
			{
				m_handler->PopGLLiveState();
			}
			else
			{
				m_handler->TerminateGame();
			}
			return;
		}
		break;
	default:
		GLLiveState::OnRequestFailure(func_id, error_code);
	}
}

int GLLiveStateMenu::GetNewMsgCount()
{
	int newMsgCount = 0;
	newMsgCount += m_gl_message->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE, USER_MESSAGE_READTYPE_NEW);
	newMsgCount += m_gl_message->getMsgCount(USER_MESSAGE_TYPE_GAMEMESSAGE, USER_MESSAGE_READTYPE_NEW);
	newMsgCount += m_gl_message->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION, USER_MESSAGE_READTYPE_NEW);
	newMsgCount += m_gl_message->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION, USER_MESSAGE_READTYPE_NEW);
	newMsgCount += m_gl_message->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION, USER_MESSAGE_READTYPE_NEW);
	newMsgCount += m_gl_message->getMsgCount(USER_MESSAGE_TYPE_GAMEMESSAGE_AVATAR, USER_MESSAGE_READTYPE_NEW);

	return newMsgCount;
}

void GLLiveStateMenu::PaintAvatar(CGLImage *img, int x, int y, int scalex, int scaley)
{
	CGLLiveGraphics	*g = m_handler->GetGraphics();
	bool			needScale = false;
	
	if(scalex > 0 && scaley > 0)
		needScale = true;
	
	if(needScale)
		g->BeginScale(scalex / 64.0, scaley / 64.0, x, y);

	//the avatar is not downloaded, draw loading
	if(img == NULL)
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_AVATAR_NULL, x, y, 0);
	}
	//the avatar is downloaded, but the avatar is not a picture
	else if(img->IsNull())
	{
		//this pic need not scale
		if(needScale)
		{
			g->EndScale();
			needScale = false;
		}
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BUTTON_AVATAR  + g_pGLLiveInstance->getLang(), x, y, 0);
	}
	//the avatar is downloaded
	else
	{
#ifdef WIN32
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_AVATAR_LOADING0, x, y, 0);
#else
		g->DrawRegion(img, 0, 0, 64, 64, CGLLiveGraphics::TRANS_MIRROR_ROT180, x, y, 0);
#endif
	}

	if(needScale)
		g->EndScale();
}



/****************GLLiveStateDisconnect*********************************/
GLLiveStateDisconnect::GLLiveStateDisconnect(GLLiveStateHandler * handler, GLLiveState *pLastState)
			: GLLiveState(handler)
{
	m_lastState = pLastState;

	//Init msg box
	if(m_pMsgBox == NULL)
	{
		m_pMsgBox = XP_API_NEW CGLLiveMessageBox();
	}

	
	if(m_bLoginFromOtherPlace)
	{
		m_bLoginFromOtherPlace = false;
		m_pMsgBox->SetText(GetString(STR_KEEP_ALIVE_FAIL_2));
	}
	else
	{
		m_pMsgBox->SetText(GetString(STR_KEEP_ALIVE_FAIL));
	}
	m_pMsgBox->SetTitle(GetString(STR_ERROR));
}

GLLiveStateDisconnect::~GLLiveStateDisconnect()
{
}

void GLLiveStateDisconnect::SwitchState()
{
}

void GLLiveStateDisconnect::Paint()
{
	m_lastState->Paint();

//	CGLLiveGraphics *g = m_handler->GetGraphics();
//
//	g->EndDraw2D();
//	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
//#ifdef USE_NEW_STYLE_INTERFACE
//	g->EnableTextureBlend(true);
//	g->SetARGBColor(0x80000000);
//	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0);
//	g->EnableTextureBlend(false);
//#else
//	g->SetARGBColor(0x80888888);
//	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
//#endif

	m_pMsgBox->Repaint(m_handler);
}

bool GLLiveStateDisconnect::HandleEvent(_TOUCH_EVENT event)
{
	if(event.eType == TET_Pressed)
	{
		//Pop to login state
		//g_pGLLiveInstance->TerminateGame();
		while(m_handler->GetCurState()->State() != DEF::GAME_STATE_LOGIN)
		{
			m_handler->PopGLLiveState(false);
		}
		m_gl_login->SetLoggedIn(false);
		m_gl_chat->Logout();
	}
	return true;
}

void GLLiveStateDisconnect::UpdateAnimation()
{
	GLLiveState::UpdateAnimation();
}