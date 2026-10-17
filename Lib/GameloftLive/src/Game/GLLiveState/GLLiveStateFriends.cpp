#include "GLLiveStdafx.h"
#include "GLLiveStateFriends.h"
#include "GLLiveStateMessage.h"
#include "GLLiveStateEdit.h"
#include "GLLiveStateChat.h"
#include "GLLiveMenu.h"

#define TIME_BUF_SIZE     32

extern int getLangIndexFromServer(char* lang);


GLLiveStateFriends::GLLiveStateFriends(GLLiveStateHandler * handler, char * receiver): GLLiveState(handler)
{
	m_receiver = receiver;
	EnableGo(false);

	m_updating = false;
	m_updated = false;
	m_select = -1;
	m_updateFriend = -1;
	m_selectedFriend = -1;

	//Init list menu
	m_menu = XP_API_NEW GLLiveMenu(0, 0);
	XP_API_MEMSET(m_addUser, 0, sizeof(m_addUser));

	_TouchRect rect;
	GLLiveMenuItem *item;
	rect.Set(390, 20, 42, 37);
	m_menu->AddMenuItem(this, IDC_ADD, NULL, rect);
	item = m_menu->GetItem(0);
	item->SetPic(1, Frame_menu_BUTTON_ADD, 0, 2);
	item->SetSelPic(1, Frame_menu_BG_ADD, 0, 2);

	//Init detail menu
	m_detailMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(379, 38, 36, 36);
	m_detailMenu->AddMenuItem(this, IDC_DEL, NULL, rect);
	item = m_detailMenu->GetItem(0);
	item->SetPic(1, Frame_menu_BUTTON_DEL);
	item->SetSelPic(1, Frame_menu_BG_DEL);

	if(g_pGLLiveInstance->m_enableInviteToPlay)
	{
		rect.Set(68, 135, 157, 56);
		m_detailMenu->AddMenuItem(this, IDC_INVITE_PLAY, GetString(STR_FRIEND_INVITE_PLAY), rect);
		item = m_detailMenu->GetItem(1);
		item->SetPic(1, Frame_menu_BUTTON_INVITE_PLAY);
		item->SetSelPic(1, Frame_menu_BG_INVITE_PLAY);
		item->SetFont(0);

		rect.Set(256, 135, 157, 56);
		m_detailMenu->AddMenuItem(this, IDC_INVITE_CHAT, GetString(STR_FRIEND_INVITE_CHAT), rect);
		item = m_detailMenu->GetItem(2);
		item->SetPic(1, Frame_menu_BUTTON_INVITE_CHAT);
		item->SetSelPic(1, Frame_menu_BG_INVITE_CHAT);
		item->SetFont(0);
	}
	else
	{
		rect.Set(68, 135, 157, 56);
		m_detailMenu->AddMenuItem(this, IDC_INVITE_PLAY, GetString(STR_FRIEND_INVITE_PLAY), rect);
		m_detailMenu->GetItem(1)->SetVisible(false);

		rect.Set(256 - (256 - 68) / 2, 135, 157, 56);
		m_detailMenu->AddMenuItem(this, IDC_INVITE_CHAT, GetString(STR_FRIEND_INVITE_CHAT), rect);
		item = m_detailMenu->GetItem(2);
		item->SetPic(1, Frame_menu_BUTTON_INVITE_CHAT, - (256 - 68) / 2);
		item->SetSelPic(1, Frame_menu_BG_INVITE_CHAT, - (256 - 68) / 2);
		item->SetFont(0);
	}

	rect.Set(68, 212, 157, 56);
	m_detailMenu->AddMenuItem(this, IDC_SEND_MSG, GetString(STR_FRIEND_SENDMSG), rect);
	item = m_detailMenu->GetItem(3);
	item->SetPic(1, Frame_menu_BUTTON_SEND_MSG);
	item->SetSelPic(1, Frame_menu_BG_SEND_MSG);
	item->SetFont(0);

	rect.Set(256, 212, 157, 56);
	m_detailMenu->AddMenuItem(this, IDC_COMPARE, GetString(STR_FRIEND_COMPARE), rect);
	item = m_detailMenu->GetItem(4);
	item->SetPic(1, Frame_menu_BUTTON_COMPARE);
	item->SetSelPic(1, Frame_menu_BG_COMPARE);
	item->SetFont(0);

	//Init delete menu
	m_deleteMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(136, 181 + 9, 79, 32);
	m_deleteMenu->AddMenuItem(this, IDC_MSG_NO, GetString(STR_NO), rect);
	item = m_deleteMenu->GetItem(0);
	item->SetPic(1, Frame_menu_BUTTON_YES, 0, 9);
	item->SetSelPic(1, Frame_menu_BG_YES, 0, 9);

	rect.Set(263, 181 + 9, 79, 32);
	m_deleteMenu->AddMenuItem(this, IDC_MSG_YES, GetString(STR_YES), rect);
	item = m_deleteMenu->GetItem(1);
	item->SetPic(1, Frame_menu_BUTTON_NO, 0, 9);
	item->SetSelPic(1, Frame_menu_BG_NO, 0, 9);

	//Init game set menu
	m_gameSetMenu = XP_API_NEW GLLiveMenu(0,0);
	rect.Set( 240 - 164 / 2, 138, 165, 35);
	m_gameSetMenu->AddMenuItem(this, IDC_QUICK_PLAY, GetString(STR_INVITE_QUICK), rect);
	item = m_gameSetMenu->GetItem(0);
	item->SetPic(1, Frame_menu_GAME_SET, 240 - 164 / 2, 138);
	item->SetSelPic(1, Frame_menu_BG_GAME_SET, 240 - 164 / 2, 138);
	item->SetFont(0);

	rect.Set( 240 - 164 / 2, 192, 165, 35);
	m_gameSetMenu->AddMenuItem(this, IDC_CUSTOM_PLAY, GetString(STR_INVITE_CUSTOM), rect);
	item = m_gameSetMenu->GetItem(1);
	item->SetPic(1, Frame_menu_GAME_SET, 240 - 164 / 2, 192);
	item->SetSelPic(1, Frame_menu_BG_GAME_SET, 240 - 164 / 2, 192);
	item->SetFont(0);

	//Init evaluate menu
	//m_evaluateMenu = XP_API_NEW GLLiveMenu(0, 0);
	//rect.Set(202, 198, 79, 32);
	//m_evaluateMenu->AddMenuItem(this, IDC_MSG_OK, GetString(STR_GO), rect);
	//item = m_evaluateMenu->GetItem(0);
	//item->SetPic(1, Frame_menu_BUTTON_MSG_OK);
	//item->SetSelPic(1, Frame_menu_BG_MSG_OK);

	//Init list scroll data
	m_lineHeight[SCR_LIST] = 47;
	m_startY[SCR_LIST]     = 53;
	m_height[SCR_LIST]     = 235;
	m_itemsPerPage[SCR_LIST]= 5;
	m_barX[SCR_LIST]       = 434;

	m_offset[SCR_LIST]     = 0;
	m_oldOffset[SCR_LIST]  = 0;
	m_touchPos[SCR_LIST]   = 0;
	m_moving[SCR_LIST]     = false;
	m_moved[SCR_LIST]      = false;
	m_speed[SCR_LIST]      = 0;

	//Init compare scroll data
	m_lineHeight[SCR_COMPARE] = 40;
	m_startY[SCR_COMPARE]     = 130;
	m_height[SCR_COMPARE]     = 188;
	m_itemsPerPage[SCR_COMPARE]= 4;
	m_barX[SCR_COMPARE]       = 428;

	m_offset[SCR_COMPARE]     = 0;
	m_oldOffset[SCR_COMPARE]  = 0;
	m_touchPos[SCR_COMPARE]   = 0;
	m_moving[SCR_COMPARE]     = false;
	m_moved[SCR_COMPARE]      = false;
	m_speed[SCR_COMPARE]      = 0;

	//Init compare detail scroll data
	m_lineHeight[SCR_COMPARE_DETAIL] = 40;
	m_startY[SCR_COMPARE_DETAIL]     = 130;
	m_height[SCR_COMPARE_DETAIL]     = 188;
	m_itemsPerPage[SCR_COMPARE_DETAIL]= 4;
	m_barX[SCR_COMPARE_DETAIL]       = 428;

	m_offset[SCR_COMPARE_DETAIL]     = 0;
	m_oldOffset[SCR_COMPARE_DETAIL]  = 0;
	m_touchPos[SCR_COMPARE_DETAIL]   = 0;
	m_moving[SCR_COMPARE_DETAIL]     = false;
	m_moved[SCR_COMPARE_DETAIL]      = false;
	m_speed[SCR_COMPARE_DETAIL]      = 0;

	if(m_pMsgBox == NULL)
	{
		m_pMsgBox = XP_API_NEW CGLLiveMessageBox();
	}

	m_trEvaluate.Set(145, 42,138, 72);
	m_startEvaluate = 82;
	m_evaluate = 0;
	m_evaluating = false;

	m_sub_state  = FRIENDS_LIST;
	m_last_state = m_sub_state;
	
	m_friendAvatar = NULL;
	m_myGameList = NULL;
	m_friendGameList = NULL;
	m_commonGameList = NULL;
	m_commonGamePIDList = NULL;
	m_totalTrophyCount = NULL;
	m_commonTrophyCount = NULL;

	ChangeFriendCount(m_gl_userFriend->GetFriendsCount());
	
	m_isInviteGame = false;
	m_isInviteChat = false;
	m_isEvaluate = false;

	m_gameType = 1;
	m_isLogout = false;
}

GLLiveStateFriends::~GLLiveStateFriends(void)
{
	XP_API_DEL(m_menu);
	XP_API_DEL(m_detailMenu);
	XP_API_DEL(m_deleteMenu);
	XP_API_DEL(m_gameSetMenu);
	//XP_API_DEL(m_evaluateMenu);
	XP_API_DELETE_ARRAY(m_myGameList);
	XP_API_DELETE_ARRAY(m_friendGameList);
	XP_API_DELETE_ARRAY(m_commonGameList);
	XP_API_DELETE_ARRAY(m_commonGamePIDList);
	XP_API_DELETE_ARRAY(m_totalTrophyCount);
	XP_API_DELETE_ARRAY(m_commonTrophyCount);

	if(m_friendAvatar != NULL)
	{
		for(int i = 0; i < m_itemsCount[SCR_LIST]; i++)
		{
			XP_API_DEL(m_friendAvatar[i]);
		}
		XP_API_DEL(m_friendAvatar);
	}
}

void GLLiveStateFriends::SwitchState()
{
	UpdateFriendAvatar();
}

bool GLLiveStateFriends::HandleEvent(_TOUCH_EVENT event)
{
	switch(m_sub_state)
	{
	case FRIENDS_LIST:
		return HandleEventList(event, SCR_LIST);
	case FRIENDS_DETAIL:
		return HandleEventDetail(event);
	case FRIENDS_ERROR:
		if(event.eType == TET_Pressed)
		{
			if(m_isEvaluate)
			{
				m_isEvaluate = false;
				m_gl_userFriend->sendGetUserFriends();
				m_last_state = FRIENDS_DETAIL;
				break;
			}
			m_sub_state = m_last_state;
		}
		break;
	case FRIENDS_DELETE:
		return HandleEventDelete(event);
	case FRIENDS_EVALUATE:
		return HandleEventEvaluate(event);
	case FRIENDS_COMPARE:
		//return HandleEventCompare(event);
		return HandleEventList(event, SCR_COMPARE);
	case FRIENDS_COMPARE_DETAIL:
		return HandleEventList(event, SCR_COMPARE_DETAIL);
	case FRIENDS_GAME_SET:
		{
			_TouchRect rect;
			rect.Set(69, 59, 344, 194);
			if(event.eType == TET_Pressed && !rect.Contain(event.position))
			{
				m_sub_state = FRIENDS_DETAIL;
				return true;
			}
			return m_gameSetMenu->HandleEvent(event);
		}
	default:
		return false;
	}
	return false;
}

bool GLLiveStateFriends::HandleEventCompare(_TOUCH_EVENT event)
{
	return GLLiveState::HandleEvent(event);
}

bool GLLiveStateFriends::HandleEventDetail(_TOUCH_EVENT event)
{
	if(m_detailMenu->HandleEvent(event))
	{
		return true;
	}

	if(event.eType == TET_Pressed && m_trEvaluate.Contain(event.position))
	{
		m_last_state = m_sub_state;
		m_gl_user->sendGetReputation(m_gl_userFriend->GetName(m_select));
		m_sub_state = FRIENDS_PROCESS;
		m_evaluate = 0;
		return true;
	}

	return GLLiveState::HandleEvent(event);
}

void GLLiveStateFriends::Evaluate(int posX)
{
	if(posX < 240)
	{
		m_evaluate = 5;
	}
	else
	{
		m_evaluate = 1;
	}
}

bool GLLiveStateFriends::HandleEventEvaluate(_TOUCH_EVENT event)
{
	if(event.eType == TET_Pressed )
	{
		_TouchRect rect;
		rect.Set(80, 85, 324, 152);
		if(!rect.Contain(event.position))
		{
			m_sub_state = FRIENDS_DETAIL;
			return true;
		}
		else if( event.position.y > 90 && event.position.y < 195)
		{
			m_evaluating = true;
			Evaluate(event.position.x);
			return true;
		}
	}

	if(event.eType == TET_Moving && m_evaluating)
	{
		if(event.position.x < 80 || event.position.x > 400)
		{
			m_evaluating = false;
			return true;
		}
		Evaluate(event.position.x);
		return true;
	}

	if(event.eType == TET_Released)
	{
		if(m_evaluating)
		{
			//point reputation
			m_evaluating = false;
			m_sub_state = FRIENDS_PROCESS;
			m_gl_user->sendPointUserReputation(m_gl_userFriend->GetName(m_select), m_evaluate);
			return true;
		}
	}
	return false;
	//return m_evaluateMenu->HandleEvent(event);
}

bool GLLiveStateFriends::HandleEventDelete(_TOUCH_EVENT event)
{
	if(m_deleteMenu->HandleEvent(event))
	{
		return true;
	}
	return GLLiveState::HandleEvent(event);
}

bool GLLiveStateFriends::HandleEventList( _TOUCH_EVENT event , GLLiveStateFriends::FriendScroll type)
{
	if(GLLiveState::HandleEvent(event))
		return true;
	switch(event.eType)
	{
	case TET_Pressed:
		if(event.position.y >= m_startY[type] && event.position.y <= m_startY[type] + m_lineHeight[type] * m_itemsPerPage[type])
		{
			m_touchPos[type] = event.position.y;
			m_moving[type]   = true;
			return true;
		}
		break;
	case TET_Moving:
		if(m_moving[type])
		{
			m_oldOffset[type] = m_offset[type];
			//m_offset[type] += m_touchPos[type] - event.position.y;
			ChangeOffset(m_offset[type] + (m_touchPos[type] - event.position.y), type);
			if(m_touchPos[type] - event.position.y > 2 || m_touchPos[type] - event.position.y < -2)
			{
				m_moved[type] = true;
			}
			m_touchPos[type] = event.position.y;
			//m_curPage = -1 * m_offset[type] / m_width;
			return true;
		}
		break;
	case TET_Released:
		if(m_moving[type])
		{
			m_moving[type] = false;
			if(m_moved[type])
			{
				m_moved[type] = false;
				return false;
			}
			else
			{
				if(type == SCR_COMPARE_DETAIL)
					return true;
				_TouchRect rect;
				rect.left  = 34;
				rect.top   = m_startY[type];
				rect.width = 413;
				rect.height= m_lineHeight[type] * m_itemsPerPage[type];
				if(rect.Contain(event.position))
				{
					m_select = (event.position.y - m_startY[type] + m_offset[type]) / m_lineHeight[type];
					if(m_select >= 0 && m_select < m_itemsCount[type])
					{
						if(type == SCR_LIST && m_gl_userFriend->GetState(m_select) != 4)
						{
							if(m_receiver == NULL)
							{
								m_sub_state = FRIENDS_DETAIL;//FRIENDS_PROCESS;
								//Update avatar
								//m_gl_user->sendGetUserPhoto(m_gl_userFriend->GetName(m_select));
								//m_gl_user->sendDownloadAvatar(m_gl_userFriend->GetAvatarId(m_select));
								//m_updateFriend = m_select;
								m_selectedFriend = m_select;
							}
							else
							{
								m_updating = false;
								XP_API_STRCPY(m_receiver, m_gl_userFriend->GetName(m_select));
								m_handler->PopGLLiveState();
							}
						}
						else if(type == SCR_COMPARE)
						{
							m_itemsCount[SCR_COMPARE_DETAIL] = m_commonTrophyCount[m_select];
							if(m_itemsCount[SCR_COMPARE_DETAIL] > m_itemsPerPage[SCR_COMPARE_DETAIL])
							{
								int height = m_lineHeight[SCR_COMPARE_DETAIL] * m_itemsPerPage[SCR_COMPARE_DETAIL];
								m_scrollBarH[SCR_COMPARE_DETAIL] = height * height / (m_itemsCount[SCR_COMPARE_DETAIL] * m_lineHeight[SCR_COMPARE_DETAIL]);
								if(m_scrollBarH[SCR_COMPARE_DETAIL] < 10)
								{
									m_scrollBarH[SCR_COMPARE_DETAIL] = 10;
								}
								m_scrollBarPos[SCR_COMPARE_DETAIL] = 0;
							}
							m_sub_state = FRIENDS_COMPARE_DETAIL;
							m_getTrophyIconCount = 0;
							LoadTrophyIcon();
						}
						m_speed[type] = 0;
					}
					return true;
				}
			}
			return true;
		}
		break;
	}
	if(m_receiver == NULL)
	{
		return m_menu->HandleEvent(event);
	}
	else
	{
		return false;
	}
}

void GLLiveStateFriends::Paint()
{
	m_handler->ClearScreen();
	
	switch(m_sub_state)
	{
	case FRIENDS_LIST:
		PaintList();
		break;
	case FRIENDS_PROCESS:
		if(m_last_state == FRIENDS_LIST)
		{
			PaintList();
		}
		else
		{
			PaintDetail();
		}
		PaintLoading();
		break;
	case FRIENDS_ERROR:
		if(m_last_state == FRIENDS_LIST)
		{
			PaintList();
		}
		else
		{
			PaintDetail();
		}
		m_pMsgBox->Repaint(m_handler);
		break;
	case FRIENDS_DETAIL:
		PaintDetail();
		break;
	case FRIENDS_DELETE:
		PaintDelete();
		break;
	case FRIENDS_EVALUATE:
		PaintEvaluate();
		break;
	case FRIENDS_COMPARE:
		PaintCompare();
		break;
	case FRIENDS_COMPARE_DETAIL:
		PaintCompareDetail();
		break;
	case FRIENDS_GAME_SET:
		PaintGameSet();
		break;
	default:
		break;
	}
}


void GLLiveStateFriends::PaintGameSet()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     *f3 = m_handler->GetFont(2);
	CGLLiveFont     *f7 = m_handler->GetFont(6);

	PaintDetail();

	g->EndDraw2D();
	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
	g->EnableTextureBlend(true);
	g->SetARGBColor(0x80000000);
	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);
	g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	//Paint bg
	m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_BG_EVALUATION, 0, 0, 0, 0, 0);
	//Paint title
	f3->DrawString(g, GetString(STR_FRIEND_INVITE_TILTE), 480 / 2, 60 + 30 / 2, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);

	f7->DrawString(g, GetString(STR_INVITE_CONTENT), 480 / 2, 110, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);

	m_gameSetMenu->Repaint(m_handler);
}

extern int        g_iAvatarId;
extern CGLImage * g_pAvatarImage;

void GLLiveStateFriends::PaintCompare()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     * f5 = m_handler->GetFont(4);
	CGLLiveFont     * f4 = m_handler->GetFont(3);
	CGLLiveFont     * f11 = m_handler->GetFont(10);
	char        pTemp[20];
	int			title_y = 45;

	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_GAME_COMPARE, 0, 0, 0);

	//Draw my avatar
	PaintAvatar(g_pAvatarImage, 50, title_y - 10);

	//Draw my name
	f5->DrawString(g, m_gl_user->getUserName(), 135, 45, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP, 90);
	
	//Draw My score
	XP_API_MEMSET(pTemp, 0, sizeof(pTemp));
	//...
	XP_API_SPRINTF(pTemp, "%d", m_gl_user->getTotalTrophy());
	f11->DrawString(g, pTemp, 135, 100, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE);
	f11->UpdateStringSizeEN(pTemp);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCORE_UINT,135 + f11->GetStringSize() + 5, 93 - 17, 0, 0, 0);
	
	//Draw title
	f4->DrawString(g, GetString(STR_SCORE), 62, title_y + 83, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE);
	f4->DrawString(g, GetString(STR_ACHIEVEMENT), 125, title_y + 83, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE);

	//Draw friend avatar
	PaintAvatar(m_friendAvatar[m_selectedFriend], 353, title_y - 10);

	//Draw friend name
	f5->DrawString(g, m_gl_userFriend->GetName(m_selectedFriend), 344, 45, CGLLiveGraphics::RIGHT | CGLLiveGraphics::TOP, 90);

	//Draw friend score
	XP_API_MEMSET(pTemp, 0, sizeof(pTemp));
	//...
	XP_API_SPRINTF(pTemp, "%d", m_gl_userFriend->GetTotalTrophyScore(m_selectedFriend));
	f11->DrawString(g, pTemp, 282, 100, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE);
	f11->UpdateStringSizeEN(pTemp);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCORE_UINT,282 + f11->GetStringSize() + 5, 93 - 17, 0, 0, 0);
	
	//Draw friend title
	f4->DrawString(g, GetString(STR_SCORE), 273, title_y + 83, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE);
	f4->DrawString(g, GetString(STR_ACHIEVEMENT), 334, title_y + 83, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE);

	//Draw trophy compare
	int off_y = 0;
	g->SetClip(0, m_startY[SCR_COMPARE], SCREEN_WIDTH, m_lineHeight[SCR_COMPARE] * 4);
	for(int i = 0; i < m_itemsCount[SCR_COMPARE]; i++)
	{
		off_y = m_startY[SCR_COMPARE] + m_lineHeight[SCR_COMPARE] * i - m_offset[SCR_COMPARE];
		if(off_y + m_lineHeight[SCR_COMPARE] < m_startY[SCR_COMPARE] || off_y > m_startY[SCR_COMPARE] + m_lineHeight[SCR_COMPARE] * m_itemsPerPage[SCR_COMPARE])
			continue;
		GLLiveGameInfo* myGame = GetCommonGameInfo(m_myGameList, m_myGameCount, m_commonGamePIDList[i]);
		GLLiveGameInfo* friendGame = GetCommonGameInfo(m_friendGameList, m_friendGameCount, m_commonGamePIDList[i]);
		char str[10] = {0};

		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_COMPARE_BG_LINE1 + i % 2, 32, off_y, 0);
		if(m_commonGameList[i].icon)
		{
#ifdef WIN32
			g->BeginScale(40.0 / 74, 40.0 / 74, 220, off_y + 1);
			m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TROPHY_LOADING0, 220, off_y + 1, 0);
			g->EndScale();
#else
			g->BeginScale(40.0 / 57, 40.0 / 57, 220, off_y + 1);
			g->DrawRegion(m_commonGameList[i].icon, 0, 0, 57, 57, CGLLiveGraphics::TRANS_MIRROR_ROT180, 
					220, 
					off_y + 1,
					0);
			g->EndScale();
#endif
		}
		else
		{
			m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TROPHY_LOADING0 + m_frameCount % 8, 
					227, off_y + 10, 0);
		}
		f4->DrawNumber(g, myGame->score, 82, off_y + 15, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);
		f4->DrawNumber(g, friendGame->score, 292, off_y + 15, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);
		XP_API_SPRINTF(str, "%d/%d", myGame->trophyCount, m_totalTrophyCount[i]);
		f4->DrawString(g, str, 170, off_y + 15, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);
		XP_API_MEMSET(str, 0, 10);
		XP_API_SPRINTF(str, "%d/%d", friendGame->trophyCount, m_totalTrophyCount[i]);
		f4->DrawString(g, str, 380, off_y + 15, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);
	}
	//when items count less than 4, fix bg
	for(int j = m_itemsCount[SCR_COMPARE]; j < m_itemsPerPage[SCR_COMPARE]; j++)
	{
		off_y = m_startY[SCR_COMPARE] + m_lineHeight[SCR_COMPARE] * j - m_offset[SCR_COMPARE];
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_COMPARE_BG_LINE1 + j % 2, 32, off_y, 0);
	}
	g->ResetClip();
	//Paint frame corner
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_COMPARE_CORNER, 0, 0, 0);
	//Paint scroll bar
	PaintScrollBar(SCR_COMPARE);

	GLLiveState::Paint();
}

void GLLiveStateFriends::PaintCompareDetail()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     * f5 = m_handler->GetFont(4);
	CGLLiveFont     * f4 = m_handler->GetFont(3);
	CGLLiveFont     * f11 = m_handler->GetFont(10);
	CGLLiveASprite   * sprite_menu = m_handler->GetASprite(SPRITE_MENU);
	GLLiveGameInfo* myGame = GetCommonGameInfo(m_myGameList, m_myGameCount, m_commonGamePIDList[m_select]);
	GLLiveGameInfo* friendGame = GetCommonGameInfo(m_friendGameList, m_friendGameCount, m_commonGamePIDList[m_select]);
	char        pTemp[20] = {0};
	int			offx = 290;
	int			title_y = 50;

	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);
	sprite_menu->PaintFrame(g, Frame_menu_GAME_COMPARE, 0, 0, 0);

	//Draw game icon
	if(m_commonGameList[m_select].icon)
	{
#ifdef WIN32
		m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TROPHY_LOADING0, 50, title_y - 5, 0);
#else
		g->BeginScale(74.0 / 57, 74.0 / 57, 50, title_y - 5);
		g->DrawRegion(m_commonGameList[m_select].icon, 0, 0, 57, 57, CGLLiveGraphics::TRANS_MIRROR_ROT180, 
				50, 
				title_y - 5,
				0);
		g->EndScale();
#endif
	}
	else
	{
		m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TROPHY_LOADING0 + m_frameCount % 8, 
				57, title_y, 0);
	}
	//Draw Game name
	f5->DrawString(g, m_commonGameList[m_select].name, 130, title_y, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP, 120);
	//Draw names
	f5->DrawString(g, m_gl_user->getUserName(), offx, title_y, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP, 80);
	f5->DrawString(g, m_gl_userFriend->GetName(m_selectedFriend), offx + 100, title_y, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP, 80);
		
	//Draw title
	f4->DrawString(g, GetString(STR_SCORE), 130, title_y + 45, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE);
	f4->DrawString(g, GetString(STR_ACHIEVEMENT), 130, title_y + 70, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE);
	
	//score
	f4->DrawNumber(g, myGame->score, offx, title_y + 45, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE);
	f4->DrawNumber(g, friendGame->score, offx + 100, title_y + 45, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE);
	XP_API_SPRINTF(pTemp, "%d/%d", myGame->trophyCount, m_totalTrophyCount[m_select]);
	f4->DrawString(g, pTemp, offx, title_y + 70, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE);
	XP_API_MEMSET(pTemp, 0, sizeof(pTemp));
	XP_API_SPRINTF(pTemp, "%d/%d", friendGame->trophyCount, m_totalTrophyCount[m_select]);
	f4->DrawString(g, pTemp, offx + 100, title_y + 70, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE);
	
	//Draw trophy compare detail
	int off_y = 0;
	g->SetClip(0, m_startY[SCR_COMPARE_DETAIL], SCREEN_WIDTH, m_lineHeight[SCR_COMPARE_DETAIL] * 4);
	int trophy_index = 0;
	for(int i = 0; i < m_itemsCount[SCR_COMPARE_DETAIL]; i++, trophy_index++)
	{
		bool iHave = HasTheTrophy(myGame->trophyList, myGame->trophyCount, m_commonGameList[m_select].trophyList[trophy_index].id);
		bool heHas = HasTheTrophy(friendGame->trophyList, friendGame->trophyCount, m_commonGameList[m_select].trophyList[trophy_index].id);
		//if we all don't have the trophy, continue, no need to draw
		if(!iHave && !heHas)
		{
			//trophy_index++;
			i--;
			continue;
		}
		off_y = m_startY[SCR_COMPARE_DETAIL] + m_lineHeight[SCR_COMPARE_DETAIL] * i - m_offset[SCR_COMPARE_DETAIL];
		//if we can't see the trophy, continue, no need to draw
		if(off_y + m_lineHeight[SCR_COMPARE_DETAIL] < m_startY[SCR_COMPARE_DETAIL] || off_y > m_startY[SCR_COMPARE_DETAIL] + m_lineHeight[SCR_COMPARE_DETAIL] * 4)
			continue;

		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_COMPARE_BG_LINE1 + i % 2, 32, off_y, 0);
#ifdef WIN32
		//trophy id and name
		f4->DrawNumber(g, m_commonGameList[m_select].trophyList[trophy_index].id, 45, off_y+15, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
		f4->DrawString(g, m_commonGameList[m_select].trophyList[trophy_index].name, 80, off_y+15, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
#else
		//trophy icon
		if(m_commonGameList[m_select].trophyList[trophy_index].icon != NULL)
		{
			g->BeginScale(40.0 / 48, 40.0 / 48, 70, off_y + 1);
			g->DrawRegion(m_commonGameList[m_select].trophyList[trophy_index].icon, 0, 0, 48, 48, CGLLiveGraphics::TRANS_MIRROR_ROT180, 
					70, 
					off_y + 1,
					0);
			g->EndScale();
		}
		else
		{
			m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TROPHY_LOADING0 + m_frameCount % 8, 
					77, off_y + 10, 0);
		}
#endif
		//my state
		if(iHave)
		{
			sprite_menu->PaintFrame(g, Frame_menu_ACHIEVED, offx - 20, off_y + 5, 0);
		}
		else
		{
			sprite_menu->PaintFrame(g, Frame_menu_UNACHIEVED, offx - 20, off_y + 5, 0);
		}
		//friend's state
		if(heHas)
		{
			sprite_menu->PaintFrame(g, Frame_menu_ACHIEVED, offx + 80, off_y + 5, 0);
		}
		else
		{
			sprite_menu->PaintFrame(g, Frame_menu_UNACHIEVED, offx + 80, off_y + 5, 0);
		}
	}
	//when items count less than 4, fix bg
	for(int j = m_itemsCount[SCR_COMPARE_DETAIL]; j < m_itemsPerPage[SCR_COMPARE_DETAIL]; j++)
	{
		off_y = m_startY[SCR_COMPARE_DETAIL] + m_lineHeight[SCR_COMPARE_DETAIL] * j - m_offset[SCR_COMPARE_DETAIL];
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_COMPARE_BG_LINE1 + j % 2, 32, off_y, 0);
	}
	g->ResetClip();
	//Paint frame corner
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_COMPARE_CORNER, 0, 0, 0);
	//Paint scroll bar
	PaintScrollBar(SCR_COMPARE_DETAIL);

	GLLiveState::Paint();
}
void GLLiveStateFriends::PaintDelete()
{
	PaintDetail();

	m_pMsgBox->Repaint(m_handler);

	m_deleteMenu->Repaint(m_handler);
}

void GLLiveStateFriends::PaintEvaluate()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     * f3 = m_handler->GetFont(2);
	CGLLiveFont     * f4 = m_handler->GetFont(3);

	PaintDetail();

	g->EndDraw2D();
	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
	g->EnableTextureBlend(true);
	g->SetARGBColor(0x80000000);
	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);
	g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	//Paint evaluate bg
	m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_BG_EVALUATION, 0, 0, 0, 0, 0);
	//Paint title
	f3->DrawString(g, GetString(STR_CHAT_EVALUATE_TITLE), 480 / 2, 60 + 30 / 2, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);
	
	int fid = 0;
	if(m_evaluating)
	{
		fid = m_frameCount % 4;
		if(m_evaluate == 5)
		{
			m_handler->GetASprite(SPRITE_EVALUATE)->PaintAFrame(g, 2, fid, 100, 115, 0, 0, 0);
			m_handler->GetASprite(SPRITE_EVALUATE)->PaintAFrame(g, 1, 0, 290, 115, 0, 0, 0);
		}
		else
		{
			m_handler->GetASprite(SPRITE_EVALUATE)->PaintAFrame(g, 0, 0, 100, 115, 0, 0, 0);
			m_handler->GetASprite(SPRITE_EVALUATE)->PaintAFrame(g, 3, fid, 290, 115, 0, 0, 0);
		}
	}
	else
	{
		fid = m_frameCount % 50;
		if(fid < 15)
		{
			m_handler->GetASprite(SPRITE_EVALUATE)->PaintAFrame(g, 0, fid % 5, 100, 115, 0, 0, 0);
			m_handler->GetASprite(SPRITE_EVALUATE)->PaintAFrame(g, 1, 0, 290, 115 , 0, 0, 0);
		}
		else if(fid > 25 && fid < 40)
		{
			m_handler->GetASprite(SPRITE_EVALUATE)->PaintAFrame(g, 0, 0, 100, 115, 0, 0, 0);
			m_handler->GetASprite(SPRITE_EVALUATE)->PaintAFrame(g, 1, (fid - 25) % 5, 290, 115 , 0, 0, 0);
		}
		else
		{
			m_handler->GetASprite(SPRITE_EVALUATE)->PaintAFrame(g, 0, 0, 100, 115, 0, 0, 0);
			m_handler->GetASprite(SPRITE_EVALUATE)->PaintAFrame(g, 1, 0, 290, 115 , 0, 0, 0);
		}
	}

	char tmp[64] = {0};
	XP_API_SPRINTF(tmp, "%d %s",m_gl_user->getUserGoodReputationCount() , m_gl_user->getUserGoodReputationCount() > 1 ? GetString(STR_VOTES) : GetString(STR_VOTE));
	f4->DrawString(g, tmp, 140, 220, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER);

	XP_API_SPRINTF(tmp, "%d %s",m_gl_user->getUserBadReputationCount(), m_gl_user->getUserGoodReputationCount() > 1 ? GetString(STR_VOTES) : GetString(STR_VOTE));
	f4->DrawString(g, tmp, 335, 220, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER);
}

void GLLiveStateFriends::PaintDetail()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     * f5 = m_handler->GetFont(4);
	CGLLiveFont     * f6 = m_handler->GetFont(5);
	CGLLiveFont     * f11 = m_handler->GetFont(10);
	char      pTemp[64];

	//Init user infor
	char *name     = m_gl_userFriend->GetName(m_selectedFriend);
	int reputation = m_gl_userFriend->GetReputation(m_selectedFriend);
	int score      = m_gl_userFriend->GetTotalTrophyScore(m_selectedFriend);
	int country    = CountryToIndex(m_gl_userFriend->GetCountry(m_selectedFriend));
	int state      = m_gl_userFriend->GetState(m_selectedFriend);
	char *curGame  = m_gl_userFriend->GetCurrentGame(m_selectedFriend);


	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0, 0, 0);
	//Paint bg
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0, 0, 0);
	//Paint avatar
	PaintAvatar(m_friendAvatar[m_selectedFriend], 67, 40);

	//Paint user info
	f5->DrawString(g, name, 154, 40 + 5, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP, 130);
	for(int i = 0; i < 5; i++)
	{
		if( i < reputation)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_REPUTATION, 154 + i * 20 , 70, 0, 0, 0);
		}
		else
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_REPUTATION_NULL, 154 + i * 20 , 70, 0, 0, 0);
		}
	}
	
	XP_API_MEMSET(pTemp, 0, sizeof(pTemp));
	XP_API_SPRINTF(pTemp, "%d", score);
	f11->DrawString(g, pTemp, 154, 50 + 72, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE);
	f11->UpdateStringSizeEN(pTemp);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCORE_UINT,154 + f11->GetStringSize() + 8, 40 + 72 - 15, 0, 0, 0);
	//Paint country
	g->BeginScale(0.6, 0.6, 290, 40);
	m_handler->GetASprite(SPRITE_FLAG)->PaintFrame(g, country, 290, 40, 0, 0, 0);
	g->EndScale();
	//Paint state
	switch(state)
	{
	case XPLAYER_USER_STATE_OFFLINE:
		f6->DrawString(g, GetString(STR_FRIEND_OFFLINE), 330, 102, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_OFFLINE, 297, 90, 0, 0, 0);
		break;
	case XPLAYER_USER_STATE_ONLINE:
		f6->DrawString(g, GetString(STR_FRIEND_ONLINE), 330, 102, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_ONLINE, 297, 90, 0, 0, 0);
		break;
	case XPLAYER_USER_STATE_PLAYING:
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_ONLINE, 297, 90, 0, 0, 0);

		XP_API_MEMSET(pTemp, 0, sizeof(pTemp));
		XP_API_SPRINTF(pTemp, GetString(STR_FRIEND_PLAYING), curGame);
		f6->InitPage(pTemp, 85);
		f6->DrawPage(g, pTemp, 330, 102, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
		break;
	case 4:
		f6->InitPage(GetString(STR_FRIEND_WAIT_CONFIRM), 90);
		f6->DrawPage(g, GetString(STR_FRIEND_WAIT_CONFIRM), 330, 102, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_OFFLINE, 297, 90, 0, 0, 0);
		break;
	}

#ifdef SHOW_TOUCH_RECT
		g->EndDraw2D();
		g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
		g->SetARGBColor(0x88880000);
		g->FillRect(m_trEvaluate.left, m_trEvaluate.top, m_trEvaluate.width, m_trEvaluate.height);
#endif

	//Paint menu
	m_detailMenu->Repaint(m_handler);

	GLLiveState::Paint();
}

void GLLiveStateFriends::PaintList()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     * f2 = m_handler->GetFont(1);
	CGLLiveFont     * f5 = m_handler->GetFont(4);
	CGLLiveFont     * f11= m_handler->GetFont(10);
	CGLLiveFont     * f6 = m_handler->GetFont(5);

	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0, 0, 0);
	//Paint bg
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_TITLE_FRIENDS, 1, -1, 0, 0, 0);
	//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LINE1, 0, -5, 0, 0, 0);
	//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG2_FRIENDS, 0, -5, 0, 0, 0);

	//Paint Title
	f2->DrawString(g, GetString(STR_FRIEND_LIST), 235, 39,CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);

	//Paint menu(add friend)
	if(m_receiver == NULL)
		m_menu->Repaint(m_handler);

	//Paint friends list
	g->SetClip(0, m_startY[SCR_LIST], SCREEN_WIDTH, m_lineHeight[SCR_LIST] * m_itemsPerPage[SCR_LIST]);
	int offset = -m_offset[SCR_LIST];
	char temp[64];
	for(int i = 0; i < m_itemsCount[SCR_LIST]; i++)
	{
		if(offset >= m_lineHeight[SCR_LIST] * m_itemsPerPage[SCR_LIST])
		{
			break;
		}
		//Paint the friends infor that are visible
		if(offset  + m_lineHeight[SCR_LIST] >= 0)
		{
			//Bg
			if(i % 2 == 0)
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LINE1, 32, 53 + offset, 0, 0, 0);
			}
			else
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LINE2, 32, 53 + offset, 0, 0, 0);
			}
			//Avatar
			int avatar_offx = 56;
			PaintAvatar(m_friendAvatar[i], avatar_offx, 58 + offset, 37, 37);

			//Country
			int country_offx = 105;
			g->BeginScale(0.6, 0.6, country_offx, 58 + offset);
			m_handler->GetASprite(SPRITE_FLAG)->PaintFrame(g, CountryToIndex(m_gl_userFriend->GetCountry(i)), country_offx, 58 + offset, 0, 0, 0);
			g->EndScale();
			//Reputation
			int name_offx = 155;
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_FRIEND_REPUTATION, name_offx, 83 + offset, 0, 0, 0);
			for(int j = 0; j < m_gl_userFriend->GetReputation(i); j++)
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_FRIEND_REPUTATION, name_offx + j * 11, 83 + offset, 0, 0, 0);
			}
			//User name
			f5->DrawString(g, m_gl_userFriend->GetName(i), name_offx, 82 + offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE, 85);
			//Score
			XP_API_MEMSET(temp, 0, sizeof(temp));
			XP_API_SPRINTF(temp, "%d", m_gl_userFriend->GetTotalTrophyScore(i));
			f11->DrawString(g, temp, 286, 70 + offset, CGLLiveGraphics::RIGHT | CGLLiveGraphics::TOP);
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCORE_UINT, 291, 68 + offset,0, 0, 0);
			//State
			switch(m_gl_userFriend->GetState(i))
			{
			case XPLAYER_USER_STATE_OFFLINE:
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_OFFLINE, 400, 65 +  offset, 0, 0, 0);
				f6->InitPage(GetString(STR_FRIEND_OFFLINE), 80);
				f6->DrawPage(g, GetString(STR_FRIEND_OFFLINE), 320, 78 + offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
				break;
			case XPLAYER_USER_STATE_ONLINE:
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_ONLINE, 400, 65 +  offset, 0, 0, 0);
				f6->InitPage(GetString(STR_FRIEND_ONLINE), 80);
				f6->DrawPage(g, GetString(STR_FRIEND_ONLINE), 320, 78 + offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
				break;
			case XPLAYER_USER_STATE_PLAYING:
				XP_API_MEMSET(temp, 0, sizeof(temp));
				XP_API_SPRINTF(temp, GetString(STR_FRIEND_PLAYING), m_gl_userFriend->GetCurrentGame(i));
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_ONLINE, 400, 65 + offset, 0, 0, 0);
				f6->InitPage(temp, 80);
				f6->DrawPage(g, temp, 320, 78 + offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
				break;
			case 4:
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_OFFLINE, 400, 65 +  offset, 0, 0, 0);
				f6->InitPage(GetString(STR_FRIEND_WAIT_CONFIRM), 80);
				f6->DrawPage(g, GetString(STR_FRIEND_WAIT_CONFIRM), 320, 78 + offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
				break;
			}
		}
		offset += m_lineHeight[SCR_LIST];
	}

	//when friends count less than 5, fix bg
	for(int j = m_itemsCount[SCR_LIST]; j < m_itemsPerPage[SCR_LIST]; j++)
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LINE1 + j % 2, 32, 53 + j * 47, 0, 0, 0);
	}
	g->ResetClip();
	//Paint frame corner
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_FRIEND_CORNER, 0, 0, 0);
	//Paint scroll bar
	PaintScrollBar(SCR_LIST);

	GLLiveState::Paint();
}

void GLLiveStateFriends::UpdateAnimation()
{
	switch(m_sub_state)
	{
	case FRIENDS_LIST:
		UpdateAnimationList(SCR_LIST);
		break;
	case FRIENDS_DETAIL:
		m_detailMenu->Update();
		break;
	case FRIENDS_DELETE:
		m_deleteMenu->Update();
		break;
	case FRIENDS_EVALUATE:
		//m_evaluateMenu->Update();
		break;
	case FRIENDS_COMPARE:
		//UpdateAnimationCompare();
		UpdateAnimationList(SCR_COMPARE);
		break;
	case FRIENDS_COMPARE_DETAIL:
		//UpdateAnimationCompare();
		UpdateAnimationList(SCR_COMPARE_DETAIL);
		break;
	case FRIENDS_GAME_SET:
		m_gameSetMenu->Update();
		break;
	default:
		break;
	}

//	if(!m_updating && !m_updated)
//	{
//		UpdateFriendAvatar();
//	}
	GLLiveState::UpdateAnimation();
}

void GLLiveStateFriends::UpdateAnimationCompare()
{

}

void GLLiveStateFriends::UpdateAnimationList(GLLiveStateFriends::FriendScroll type)
{
	//Update for scroll view
	if(m_moving[type])
	{
		m_speed[type] = m_offset[type] - m_oldOffset[type];/* / 1 */
		m_oldOffset[type] = m_offset[type];
	}
	else
	{
		if( ABS(m_speed[type]) > 0 && ABS(m_speed[type]) <= m_itemsPerPage[type])
		{
			ChangeOffset(m_offset[type] + m_speed[type], type);
			if(m_offset[type] % m_lineHeight[type] != 0)
			{
				if(ABS(m_speed[type]) > 1)
				{
					m_speed[type] = (m_speed[type] / ABS(m_speed[type])) * (ABS(m_speed[type]) - 1);
				}
			}
			else
			{
				m_speed[type] = 0;
			}
		}
		else if(ABS(m_speed[type]) > m_itemsPerPage[type])
		{
			//m_offset[type] += m_speed[type];
			ChangeOffset(m_offset[type] + m_speed[type], type);
			m_speed[type] -= (m_speed[type] / ABS(m_speed[type])) * m_itemsPerPage[type];
			//m_speed[type] /= 1.8;
		}
	}


	//Update for add button
	if(m_receiver == NULL)
	{
		m_menu->Update();
	}

	//Start to add friend
	if(XP_API_STRLEN(m_addUser) != 0)
	{
		if(XP_API_STRICMP(m_addUser, m_gl_user->getUserName()) == 0)
		{
			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetString(STR_ADD_YOURSELF));
			m_sub_state = FRIENDS_ERROR;
			m_last_state = FRIENDS_LIST;
			
		}
		else if(!CheckInput(m_addUser))
		{
			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetErrorString(ERROR_USERNAME_NOTEXISTS));
			m_sub_state = FRIENDS_ERROR;
			m_last_state = FRIENDS_LIST;
		}
		else
		{
			m_gl_userFriend->sendAddUserFriend(m_addUser, GetString(STR_ADD_REQUEST_MESSAGE));
			m_last_state = m_sub_state;
			m_sub_state = FRIENDS_PROCESS;
		}
		XP_API_MEMSET(m_addUser, 0, MAX_USERNAME);
	}
}

void GLLiveStateFriends::OnMenuItemClicked( int id )
{
	switch(id)
	{
	case IDC_ADD:
		if(m_sub_state == FRIENDS_LIST)
		{
			XP_API_MEMSET(m_addUser, 0, MAX_USERNAME);
			m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_FRIEND_ENTER_NAME), m_addUser, false, MAX_USERNAME - 1));
		}
		return;
	case IDC_BACK:
		if(m_sub_state == FRIENDS_DETAIL)
		{
			m_sub_state = FRIENDS_LIST;
			return;
		}
		if(m_sub_state == FRIENDS_COMPARE)
		{
			m_offset[SCR_COMPARE] = 0;
			m_sub_state = FRIENDS_DETAIL;
			return;
		}
		if(m_sub_state == FRIENDS_COMPARE_DETAIL)
		{
			m_offset[SCR_COMPARE_DETAIL] = 0;
			m_sub_state = FRIENDS_COMPARE;
			return;
		}
		break;
	case IDC_DEL:
		m_pMsgBox->SetTitle(GetString(STR_WARNING));
		m_pMsgBox->SetText(GetString(STR_FRIEND_DEL_CONFIRM));

		m_last_state = m_sub_state;
		m_sub_state = FRIENDS_DELETE;
		return;
	case IDC_MSG_YES:
		m_sub_state = FRIENDS_PROCESS;
		m_gl_userFriend->sendDelUserFriend(m_gl_userFriend->GetName(m_select));
		return;
	case IDC_MSG_NO:
		m_sub_state = FRIENDS_DETAIL;
		return;
	case IDC_MSG_OK:
		m_sub_state = FRIENDS_DETAIL;
		return;
	case IDC_SEND_MSG:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateMessage(m_handler, m_gl_userFriend->GetName(m_selectedFriend)));
		return;
	case IDC_COMPARE:
		m_commonGameCount = 0;
		m_last_state = m_sub_state;
		m_sub_state = FRIENDS_PROCESS;
		if(m_myGameList == NULL)
		{
			//Get my game list
			m_gl_user->sendGetUserGameList();
		}
		else
		{
			//Get friend's game list
			m_gl_user->sendGetUserGameList(m_gl_userFriend->GetName(m_selectedFriend));
		}
		return;
	case IDC_INVITE_CHAT:
		m_last_state = m_sub_state;
		m_sub_state = FRIENDS_PROCESS;
		{
			m_isInviteChat = true;
			XP_API_STRCPY(m_inviteFriend, m_gl_userFriend->GetName(m_selectedFriend));
			m_gl_userFriend->sendGetUserFriends();
			return;
		}
	case IDC_INVITE_PLAY:
		if(g_pGLLiveInstance->m_dropGameModeChooseScreen)
		{
			m_gameType = 1;

			m_last_state = FRIENDS_DETAIL;
			m_sub_state = FRIENDS_PROCESS;
			m_isInviteGame = true;
			m_gl_userFriend->sendGetUserFriends();
		}
		else
		{
			m_sub_state = FRIENDS_GAME_SET;
		}
		XP_API_STRCPY(m_inviteFriend, m_gl_userFriend->GetName(m_selectedFriend));
		break;
	case IDC_QUICK_PLAY:
		m_gameType = 1;

		m_last_state = FRIENDS_DETAIL;
		m_sub_state = FRIENDS_PROCESS;
		m_isInviteGame = true;
		m_gl_userFriend->sendGetUserFriends();
		return;
	case IDC_CUSTOM_PLAY:
		m_gameType = 3;

		m_last_state = FRIENDS_DETAIL;
		m_sub_state = FRIENDS_PROCESS;
		m_isInviteGame = true;
		m_gl_userFriend->sendGetUserFriends();
		return;
	}
	GLLiveState::OnMenuItemClicked(id);
}

void GLLiveStateFriends::ChangeOffset(int newOffset, FriendScroll type)
{
	m_offset[type] = newOffset;
	if(m_offset[type] < 0 || m_itemsCount[type] < m_itemsPerPage[type])
	{
		m_offset[type] = 0;
	}
	else if(m_offset[type] > (m_itemsCount[type] - m_itemsPerPage[type]) * m_lineHeight[type])
	{
		m_offset[type] = (m_itemsCount[type] - m_itemsPerPage[type]) * m_lineHeight[type];
	}
	//Update scroll bar pos
	if(m_itemsCount[type] > m_itemsPerPage[type])
	{
		m_scrollBarPos[type] = (m_lineHeight[type] * m_itemsPerPage[type] - m_scrollBarH[type]) * m_offset[type] / ((m_itemsCount[type] - m_itemsPerPage[type]) * m_lineHeight[type]);
	}
}

void GLLiveStateFriends::ChangeFriendCount(int count)
{
	//Reload avatar file
	if(m_itemsCount[SCR_LIST] > 0)
	{
		if(m_friendAvatar != NULL)
		{
			for(int i = 0; i < m_itemsCount[SCR_LIST]; i++)
			{
				XP_API_DEL(m_friendAvatar[i]);
			}
			XP_API_DEL(m_friendAvatar);
		}
	}

	if(count > 0)
	{
		m_friendAvatar = XP_API_NEW CGLImage*[count];
		MEMSET(m_friendAvatar, 0, count * sizeof(CGLImage*));
	}

	m_itemsCount[SCR_LIST] = count;
	//update scroll bar
	if(m_itemsCount[SCR_LIST] > m_itemsPerPage[SCR_LIST])
	{
		int height = m_lineHeight[SCR_LIST] * m_itemsPerPage[SCR_LIST];
		m_scrollBarH[SCR_LIST] = height * height / (m_itemsCount[SCR_LIST] * m_lineHeight[SCR_LIST]);
		if(m_scrollBarH[SCR_LIST] < 10)
		{
			m_scrollBarH[SCR_LIST] = 10;
		}
		m_scrollBarPos[SCR_LIST] = 0;
	}

	LoadAllFriendAvatar();
	UpdateFriendAvatar();
}

//void GLLiveStateFriends::OnNetworkError()
//{
//	m_pMsgBox->SetTitle(GetString(STR_ERROR));
//	m_pMsgBox->SetText(GetErrorString(ERROR_CONNECTION));
//	m_sub_state = FRIENDS_ERROR;
//	XP_DEBUG_OUT( "[GLLiveStateFriends] OnNetworkError.\n");
//}
//void GLLiveStateFriends::OnRequestTimeout( int func_id )
//{
//	m_pMsgBox->SetTitle(GetString(STR_ERROR));
//	m_pMsgBox->SetText(GetErrorString(ERROR_REQUEST_TIME_OUT));
//
//	m_sub_state = FRIENDS_ERROR;;
//	XP_DEBUG_OUT( "[GLLiveStateFriends] OnRequestTimeout.\n");
//}
void GLLiveStateFriends::OnRequestSuccess( int func_id, char * data, int data_len )
{
	switch(func_id)
	{
	case FUNCTION_ADD_USER_FRIEND:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Add friend OK!\n" );
		m_gl_userFriend->sendGetUserFriends();
		m_last_state = FRIENDS_LIST;
		break;
	case FUNCTION_DEL_USER_FRIEND:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Delete friend OK!\n" );
		m_gl_userFriend->sendGetUserFriends();
		m_sub_state = FRIENDS_PROCESS;
		m_last_state = FRIENDS_LIST;
		break;
	case FUNCTION_GET_USER_FRIENDS:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Reget friends OK!\n" );
		ChangeFriendCount(m_gl_userFriend->GetFriendsCount());
		//Check if the friend is exist in the list
		if(m_isInviteGame || m_isInviteChat)
		{
			bool isExist = false;
			int i = 0;
			for(i = 0; i < m_gl_userFriend->GetFriendsCount(); i++)
			{
				if(XP_API_STRICMP(m_inviteFriend, m_gl_userFriend->GetName(i)) == 0)
				{
					isExist = true;
					break;
				}
			}

			if(isExist)
			{
				m_selectedFriend = i;
				m_select = i;
			}
			else
			{
				m_selectedFriend = 0;
				m_select = 0;
				m_pMsgBox->SetTitle(GetString(STR_ERROR));
				m_pMsgBox->SetText(GetString(STR_FAILED));
				m_last_state = FRIENDS_LIST;
				m_sub_state = FRIENDS_ERROR;
				//clear the invite game flag
				m_isInviteGame = false;
				m_isInviteChat = false;
				return;
			}
		}

		if(m_isInviteGame)
		{
			m_isInviteGame = false;
			//char title[128] = {0};
			//char msg[128] = {0};
			//XP_API_SPRINTF(title, GetString(STR_GAME_INVITATION), m_gl_user->getUserName());
			//XP_API_SPRINTF(msg, GetString(STR_GAME_INVITATION_MSG, getLangIndexFromServer(m_gl_userFriend->GetCurrentLanguage(m_selectedFriend))), m_gl_user->getUserGameName(0));//0 means the latest one
			//if(m_gl_userFriend->GetState(m_selectedFriend) == XPLAYER_USER_STATE_ONLINE)
			//{
			//	//GLXPlayerChatRoom *room = m_gl_chat->CreateChatRoom(GAME_INVITAON_STR, m_gl_user->getUserName());
			//	//char lname[MAX_USERNAME];
			//	//GLXPlayerChat::ToLower(m_gl_userFriend->GetName(m_selectedFriend), lname);

			//	//room->Invite(lname, msg);
			//	////room->Leave();
			//	//XP_API_DEL(room);
			//}
			//else
			//{
			//	m_gl_message->SendOnlineMessage(m_gl_userFriend->GetName(m_selectedFriend), 
			//									USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION, 
			//									title, msg, XP_API_STRLEN(msg));				
			//}
#ifndef WIN32
			m_gl_login->SendLogout();
			m_isLogout = true;
			m_sub_state = FRIENDS_PROCESS;
#endif
			return;
		}
		else if(m_isInviteChat)
		{
			m_isInviteChat = false;
			char* temp_msg = GetString(STR_CHAT_INVITE_MSG, getLangIndexFromServer(m_gl_userFriend->GetCurrentLanguage(m_selectedFriend)));
			if(m_gl_userFriend->GetState(m_selectedFriend) == XPLAYER_USER_STATE_ONLINE)
			{
				//m_handler->PushGLLiveState(XP_API_NEW GLLiveStateChat(m_handler));
				m_sub_state = FRIENDS_DETAIL;
				//Create chat room
				GLXPlayerChatRoom *newRoom = m_gl_chat->CreateChatRoom(m_gl_user->getUserName(), m_gl_user->getUserName());
				//Invite friend
				char lname[MAX_USERNAME];
				GLXPlayerChat::ToLower(m_gl_userFriend->GetName(m_selectedFriend), lname);
				newRoom->Invite( lname, temp_msg);
				m_handler->PushGLLiveState(XP_API_NEW GLLiveStateChat(m_handler, newRoom));
			}
			else
			{
				char* title = NULL;
				char* body = NULL;
				char* inviteTitle = GetString(STR_CHAT_INVITE_TITLE , getLangIndexFromServer(m_gl_userFriend->GetCurrentLanguage(m_selectedFriend)));
				int size1;
				int size2;
				base64_encode(inviteTitle, XP_API_STRLEN(inviteTitle), title, size1);
				base64_encode(temp_msg, XP_API_STRLEN(temp_msg), body, size2);
				m_gl_message->SendOnlineMessage(m_gl_userFriend->GetName(m_selectedFriend), USER_MESSAGE_TYPE_USERMESSAGE, title, body, XP_API_STRLEN(body));
				XP_API_DELETE_ARRAY(title);
				XP_API_DELETE_ARRAY(body);
				//m_gl_message->SendOnlineMessage(m_gl_userFriend->GetName(m_selectedFriend), 
				//				USER_MESSAGE_TYPE_USERMESSAGE, 
				//				GetString(STR_CHAT_INVITE_TITLE), temp_msg, XP_API_STRLEN(temp_msg));
			}
			return;
		}
			
		m_sub_state = m_last_state;
		break;
	case FUNCTION_DOWNLOAD_USER_AVATAR:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Get user avatar OK!\n" );
		//n|friend name|data

		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			//nothing to do
			//SavePicToLocal(m_gl_userFriend->GetAvatarId(m_updateFriend), NULL, 0);
			m_friendAvatar[m_updateFriend] = XP_API_NEW CGLImage();
		}
		else
		{
			char *	base64_buf = NULL;
			int		base64_size;
			base64_decode(data + 1, data_len - 1, base64_buf, base64_size);
			m_friendAvatar[m_updateFriend] = NEW CGLImage(base64_buf, base64_size);

			SavePicToLocal(m_gl_userFriend->GetAvatarId(m_updateFriend), base64_buf, base64_size);

			XP_API_DEL(base64_buf);
		}
		UpdateFriendAvatar();
		break;
	case FUNCTION_GET_USER_GAMELIST:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Get user game list OK!\n" );
		if(m_myGameList == NULL)
		{
			m_myGameCount = m_gl_user->getNumberUserGameList();
			m_myGameList = XP_API_NEW GLLiveGameInfo[m_myGameCount];
			for(int i = 0; i < m_myGameCount; i++)
			{
				XP_API_STRCPY(m_myGameList[i].name, m_gl_user->getUserGameName(i));
				m_myGameList[i].pid = XP_API_ATOI(m_gl_user->getUserGamePID(i));
				m_myGameList[i].score = m_gl_user->getUserGameTrophy(i);
			}
			m_gl_user->sendGetUserGameList(m_gl_userFriend->GetName(m_selectedFriend));
		}
		else
		{
			m_friendGameCount = m_gl_user->getNumberUserGameList();
			XP_API_DELETE_ARRAY(m_friendGameList);
			m_friendGameList = XP_API_NEW GLLiveGameInfo[m_friendGameCount];
			for(int i = 0; i < m_friendGameCount; i++)
			{
				XP_API_STRCPY(m_friendGameList[i].name, m_gl_user->getUserGameName(i));
				m_friendGameList[i].pid = XP_API_ATOI(m_gl_user->getUserGamePID(i));
				m_friendGameList[i].score = m_gl_user->getUserGameTrophy(i);
			}
			XP_API_DEL(m_commonGamePIDList);
			m_commonGamePIDList = XP_API_NEW int[m_myGameCount];
			for(int j = 0; j < m_myGameCount; j++)
			{
				for(int k = 0; k < m_friendGameCount; k++)
				{
					if(m_myGameList[j].pid == m_friendGameList[k].pid)
					{
						m_commonGamePIDList[m_commonGameCount++] = m_myGameList[j].pid;
						break;
					}
				}
			}
			//Get game trophy
			m_getGameTrophyCount = 0;
			if(m_commonGameCount > 0)
			{
				m_isGettingMyTrophyList = true;
				m_gl_user->sendGetUserGameTrophyList(m_commonGamePIDList[m_getGameTrophyCount], m_gl_user->getUserName());
				//m_sub_state = FRIENDS_COMPARE;
			}
			else
			{
				m_itemsCount[SCR_COMPARE] = 0;
				m_sub_state = FRIENDS_COMPARE;
			}
		}
		break;
	case FUNCTION_GET_USER_GAME_TROPHYLIST:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Get user game trophy list OK!\n" );
		if(m_isGettingMyTrophyList)
		{
			m_isGettingMyTrophyList = false;
			m_myGameList[m_getGameTrophyCount].trophyList = GLLiveStateTrophy::processTrophyList(data, data_len, m_myGameList[m_getGameTrophyCount].trophyCount);
			//download friend's trophy list
			m_gl_user->sendGetUserGameTrophyList(m_commonGamePIDList[m_getGameTrophyCount], m_gl_userFriend->GetName(m_selectedFriend));
		}
		else
		{
			m_isGettingMyTrophyList = true;
			m_friendGameList[m_getGameTrophyCount].trophyList = GLLiveStateTrophy::processTrophyList(data, data_len, m_friendGameList[m_getGameTrophyCount].trophyCount);
			m_getGameTrophyCount++;
			//download next game
			if(m_getGameTrophyCount < m_commonGameCount)
			{
				m_gl_user->sendGetUserGameTrophyList(m_commonGamePIDList[m_getGameTrophyCount], m_gl_user->getUserName());
			}
			else
			{
				m_getGameTrophyCount = 0;
				m_gl_user->sendGetGameTrophyList(m_commonGamePIDList[m_getGameTrophyCount]);
				m_commonGameList = XP_API_NEW GLLiveGameInfo[m_commonGameCount];
				m_totalTrophyCount = XP_API_NEW int[m_commonGameCount];
				//GetCommonGameList();
				//m_sub_state = FRIENDS_COMPARE;
				//load trophy icons
				//LoadCurTrophyIcon();
			}
		}
		break;
	case FUNCTION_GET_GAME_TROPHYLIST:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Get game trophy list OK!\n" );
		m_commonGameList[m_getGameTrophyCount].trophyList = GLLiveStateTrophy::processTrophyList(data, data_len, m_totalTrophyCount[m_getGameTrophyCount]);
		m_getGameTrophyCount++;
		if(m_getGameTrophyCount < m_commonGameCount)
		{
			m_gl_user->sendGetGameTrophyList(m_commonGamePIDList[m_getGameTrophyCount]);
		}
		else
		{
			SortCommonGameList();
			m_itemsCount[SCR_COMPARE] = m_commonGameCount;
			if(m_itemsCount[SCR_COMPARE] > m_itemsPerPage[SCR_COMPARE])
			{
				int height = m_lineHeight[SCR_COMPARE] * m_itemsPerPage[SCR_COMPARE];
				m_scrollBarH[SCR_COMPARE] = height * height / (m_itemsCount[SCR_COMPARE] * m_lineHeight[SCR_COMPARE]);
				if(m_scrollBarH[SCR_COMPARE] < 10)
				{
					m_scrollBarH[SCR_COMPARE] = 10;
				}
				m_scrollBarPos[SCR_COMPARE] = 0;
			}
			m_sub_state = FRIENDS_COMPARE;
			m_getGameTrophyCount = 0;
			//m_gl_user->sendGetGameIcon(m_commonGamePIDList[m_getGameTrophyCount]);
			LoadGameIcon();
		}
		break;
	case FUNCTION_GET_GAME_ICON:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Get game icon OK!\n" );
		//Parse data
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			//nothing to do
		}
		else
		{
			// d|data|t|file type
			char	fileType[10] = {0};
			getValue(data, fileType, 3, '|');
			char *	base64_buf = NULL;
			int		base64_size;
			
			base64_decode(data + 2, data_len - 2 - (XP_API_STRLEN(fileType) + 3), base64_buf, base64_size);
			m_commonGameList[m_getGameTrophyCount].icon = NEW CGLImage(base64_buf, base64_size);
			//Save to local
			char fileName[20];
			XP_API_MEMSET(fileName, 0, sizeof(fileName));
			XP_API_SPRINTF(fileName, "Game-%d.png", m_commonGamePIDList[m_getGameTrophyCount]);
			GLLiveStateTrophy::SaveFileToLocal(fileName, base64_buf, base64_size);
	
			//Free base64 buf
			XP_API_DELETE_ARRAY(base64_buf);
		}

		//Next icon
		m_getGameTrophyCount++;
		LoadGameIcon();
		break;
	case FUNCTION_GET_TROPHY_ICON:
			XP_DEBUG_OUT( "[GLLiveStateFriends] Get trophy icon OK!\n" );
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
				m_commonGameList[m_select].trophyList[m_getTrophyIconCount].icon = NEW CGLImage(base64_buf, base64_size);

				//Save to local
				char fileName[20];
				XP_API_MEMSET(fileName, 0, sizeof(fileName));
				XP_API_SPRINTF(fileName, "Trophy-%d.png", m_commonGameList[m_select].trophyList[m_getTrophyIconCount].id);
				GLLiveStateTrophy::SaveFileToLocal(fileName, base64_buf, base64_size);
				
				//Free base64 buf
				XP_API_DELETE_ARRAY(base64_buf);
			}

			//Next icon
			m_getTrophyIconCount++;
			LoadTrophyIcon();
			break;
	case FUNCTION_POINT_USER_REPUTATION:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Point reputation OK!\n" );
		m_sub_state = FRIENDS_ERROR;
		m_pMsgBox->SetTitle("");
		m_pMsgBox->SetText(GetString(STR_EVALUATE_SUCCEED));		
		m_isEvaluate = true;
		//m_gl_userFriend->sendGetUserFriends();
		//m_last_state = FRIENDS_DETAIL;
		break;
	case FUNCTION_GET_REPUTATION:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Get friend reputation OK!\n" );
		m_sub_state = FRIENDS_EVALUATE;
		break;
	case FUNCTION_SEND_MESSAGE:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Send Message Successfully!\n" );
		if(m_receiver != NULL || m_selectedFriend < 0 )
		{
			return;
		}
		m_sub_state = FRIENDS_DETAIL;
		//m_gl_login->SendLogout();
		break;
#ifndef WIN32
	case FUNCTION_LOGOUT:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Login out OK!\n" );
		extern void autoStartGame(int suspend, const char* name = "");
		autoStartGame(m_gameType, m_gl_userFriend->GetName(m_selectedFriend));
		m_handler->TerminateGame();
#endif
		break;
	default:
		GLLiveState::OnRequestSuccess(func_id, data, data_len);
		break;
	}
}

void GLLiveStateFriends::LoadGameIcon()
{
	if(m_getGameTrophyCount < m_commonGameCount)
	{
		//Load from local first
		char  fileName[20];
		char *data = NULL;
		int   dataLen = 0;

		XP_API_MEMSET(fileName, 0, sizeof(fileName));
		XP_API_SPRINTF(fileName, "Game-%d.png", m_commonGamePIDList[m_getGameTrophyCount]);
		
		if(GLLiveStateTrophy::LoadFileFromLocal(fileName, data, dataLen))
		{
			m_commonGameList[m_getGameTrophyCount].icon = NEW CGLImage(data, dataLen);
			XP_API_DELETE_ARRAY(data);
			m_getGameTrophyCount++;
			LoadGameIcon();
		}
		else
		{
			m_gl_user->sendGetGameIcon(m_commonGamePIDList[m_getGameTrophyCount]);
		}
	}
}

void GLLiveStateFriends::LoadTrophyIcon()
{
	if(m_getTrophyIconCount < m_totalTrophyCount[m_select])
	{
		GLLiveGameInfo* myGame = GetCommonGameInfo(m_myGameList, m_myGameCount, m_commonGamePIDList[m_select]);
		GLLiveGameInfo* friendGame = GetCommonGameInfo(m_friendGameList, m_friendGameCount, m_commonGamePIDList[m_select]);
		bool iHave = HasTheTrophy(myGame->trophyList, myGame->trophyCount, m_commonGameList[m_select].trophyList[m_getTrophyIconCount].id);
		bool heHas = HasTheTrophy(friendGame->trophyList, friendGame->trophyCount, m_commonGameList[m_select].trophyList[m_getTrophyIconCount].id);
		if(!iHave && !heHas)
		{
			m_getTrophyIconCount++;
			LoadTrophyIcon();
			return;
		}
		//Load from local first
		char  fileName[20];
		char *data = NULL;
		int   dataLen = 0;

		XP_API_MEMSET(fileName, 0, sizeof(fileName));
		XP_API_SPRINTF(fileName, "Trophy-%d.png", m_commonGameList[m_select].trophyList[m_getTrophyIconCount].id);
		
		if(GLLiveStateTrophy::LoadFileFromLocal(fileName, data, dataLen))
		{
			m_commonGameList[m_select].trophyList[m_getTrophyIconCount].icon = NEW CGLImage(data, dataLen);
			XP_API_DELETE_ARRAY(data);
			m_getTrophyIconCount++;
			LoadTrophyIcon();
		}
		else
		{
			m_gl_user->sendGetTrophyIcon(m_commonGameList[m_select].trophyList[m_getTrophyIconCount].id);
		}
	}
}

void GLLiveStateFriends::SortCommonGameList()
{
	m_commonTrophyCount = XP_API_NEW int[m_commonGameCount];
	XP_API_MEMSET(m_commonTrophyCount, 0, m_commonGameCount);
	for(int i = 0; i < m_commonGameCount; i++)
	{
		GLLiveGameInfo* temp = GetCommonGameInfo(m_myGameList, m_myGameCount, m_commonGamePIDList[i]);
		GLLiveGameInfo* temp2 = GetCommonGameInfo(m_friendGameList, m_friendGameCount, m_commonGamePIDList[i]);
		XP_API_STRCPY(m_commonGameList[i].name, temp->name);
		m_commonTrophyCount[i] = 0;
		for(int k = 0; k < m_totalTrophyCount[i]; k++)
		{
			if(HasTheTrophy(temp->trophyList, temp->trophyCount, m_commonGameList[i].trophyList[k].id)
				|| HasTheTrophy(temp2->trophyList, temp2->trophyCount, m_commonGameList[i].trophyList[k].id))
			{
				m_commonTrophyCount[i]++;
			}
		}
	}	
}

void GLLiveStateFriends::OnRequestFailure( int func_id, int error_code )
{
#ifndef WIN32
	//If logout failed, exit and goto game
	if(m_isLogout)
	{
		XP_DEBUG_OUT( "[GLLiveStateFriends] Logout failed!\n" );
		m_last_state = FRIENDS_DETAIL;
		m_sub_state = FRIENDS_PROCESS;
		m_isLogout = false;

		extern void autoStartGame(int suspend, const char* name = "");
		autoStartGame(m_gameType, m_gl_userFriend->GetName(m_selectedFriend));
		m_handler->TerminateGame();
	}
#endif
	if(func_id != FUNCTION_KEEP_ALIVE || func_id != FUNCTION_DOWNLOAD_USER_AVATAR)
	{
		m_pMsgBox->SetTitle(GetString(STR_ERROR));
		m_pMsgBox->SetText(GetErrorString(error_code));
		if(m_sub_state != FRIENDS_PROCESS && m_sub_state != FRIENDS_ERROR)
		{
			m_last_state = m_sub_state;
		}
		m_sub_state = FRIENDS_ERROR;
	}

	switch(func_id)
	{
	case FUNCTION_ADD_USER_FRIEND:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Add friend failed!\n" );
		break;
	case FUNCTION_DEL_USER_FRIEND:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Delete friend failed!\n" );
		break;
	case FUNCTION_GET_USER_FRIENDS:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Reget friends failed!\n" );
		break;
	case FUNCTION_DOWNLOAD_USER_AVATAR:
		if(error_code == ERROR_REQUEST_TIME_OUT)
		{
			XP_DEBUG_OUT( "[GLLiveStateFriends] Download user avatar failed!TimeOut -> Retry!\n" );
		}
		else
		{
			XP_DEBUG_OUT( "[GLLiveStateFriends] Download user avatar failed!Server Error -> user default avatar!\n" );
			m_friendAvatar[m_updateFriend] = NEW CGLImage();
		}
		UpdateFriendAvatar();
		break;
	case FUNCTION_GET_GAME_ICON:
		//Next game icon
		m_getGameTrophyCount++;
		LoadGameIcon();
		m_sub_state = m_last_state;
		break;
	case FUNCTION_GET_TROPHY_ICON:
		//Next trophy icon
		m_getTrophyIconCount++;
		LoadTrophyIcon();
		m_sub_state = m_last_state;
		break;
	case FUNCTION_POINT_USER_REPUTATION:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Point reputation failed!\n" );
		m_last_state = FRIENDS_DETAIL;
		break;
	case 0:
		UpdateFriendAvatar();
	default:
		GLLiveState::OnRequestFailure(func_id, error_code);
		break;
	}
}

void GLLiveStateFriends::LoadAllFriendAvatar()
{
	for(int i = 0; i < m_itemsCount[SCR_LIST]; i++)
	{
		m_friendAvatar[i] = LoadPicFromLocal(m_gl_userFriend->GetAvatarId(i));
	}
}

void GLLiveStateFriends::UpdateFriendAvatar()
{
	m_updated = false;
	m_updating = false;
	for(int i = 0; i < m_itemsCount[SCR_LIST]; i++)
	{
		if(m_friendAvatar[i] == NULL)
		{
			m_updateFriend = i;
			m_friendAvatar[i] = LoadPicFromLocal(m_gl_userFriend->GetAvatarId(i));
			if(m_friendAvatar[i] == NULL)
			{
				m_updating = true;
				m_gl_user->sendDownloadAvatar(m_gl_userFriend->GetAvatarId(m_updateFriend));
				//m_gl_user->sendGetUserPhoto(m_gl_userFriend->GetName(m_updateFriend));
				return;
			}
		}
	}
	m_updated = true;
}

GLLiveGameInfo* GLLiveStateFriends::GetCommonGameInfo(GLLiveGameInfo* gamelist, int length, int PID)
{
	for(int i = 0; i < length; i++)
	{
		if(gamelist[i].pid == PID)
		{
			return &gamelist[i];
		}
	}
	ASSERT(false);
	return NULL;
}

bool GLLiveStateFriends::HasTheTrophy(TrophyInfo* trophylist, int length, int trophyId)
{
	for(int i = 0; i < length; i++)
	{
		if(trophylist[i].id == trophyId)
		{
			return true;
		}
	}
	return false;
}

void GLLiveStateFriends::PaintScrollBar(GLLiveStateFriends::FriendScroll type)
{
	if(m_itemsCount[type] > m_itemsPerPage[type])
	{
		CGLLiveGraphics * g  = m_handler->GetGraphics();
		g->SetClip(m_barX[type], m_startY[type], 10, m_lineHeight[type] * m_itemsPerPage[type]);
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL, m_barX[type], m_startY[type], 0);
		for(int a = 0; a < m_scrollBarH[type] / 10; a++)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR, m_barX[type], m_startY[type] + m_scrollBarPos[type] + a * 10, 0);
		}
		if(m_scrollBarH[type] % 10 > 0)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR, m_barX[type], m_startY[type] + m_scrollBarPos[type] + m_scrollBarH[type] - 10, 0);
		}
		g->ResetClip();
	}
}