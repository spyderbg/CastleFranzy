#include "GLLiveStateChat.h"
#include "GLLiveStateEdit.h"
#include "GLLiveStateFriends.h"
#include "GLLiveMenu.h"
#include "GLLiveGame.h"

#define SYSTEM_MSG               "Gameloft"
#define AVATAR_OFF_X			2
#define MAX_CHAT_MEMBER         20
#define MAX_CHAT_ROOM_TIMEOUT   30000
#define SELTAB_INCREASE         0.75f
#define MSG_INTERVAL            5

#define CHAT_LOADING

static int g_chatColor[8] = 
					//{
					//	0xffffd1a0,
					//	0xffc8edff,
					//	0xffe8b7ff,
					//	0xffccffa6,
					//	0xffffd5eb,
					//	0xffe1e1e1,
					//	0xffe0ffd6,
					//	0xffcbe3ff
					//};
					{
						0xff692500,
						0xff00167e,
						0xff005483,
						0xff006a0e,
						0xff928000,
						0xff810259,
						0xff000000,
						0xffa30001
					};

GLLiveStateChat::GLLiveStateChat( GLLiveStateHandler * handler, GLXPlayerChatRoom *room) : GLLiveState(handler)
{
	XP_API_MEMSET(m_inputMsg, 0, sizeof(m_inputMsg));

	m_menu = XP_API_NEW GLLiveMenu(0, 0);
	_TouchRect rect;
	int index = 0;

	rect.Set(12, 88, 42, 44);
	m_menu->AddMenuItem(this, IDC_CHAT_ADD, NULL, rect);
	m_menu->GetItem(index)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_INVITE_ROOM);
	m_menu->GetItem(index)->SetSelPic(SPRITE_MENU, Frame_menu_BG_INVITE_ROOM);
	index++;

	rect.top = 135;
	m_menu->AddMenuItem(this, IDC_CHAT_NEW_ROOM, NULL, rect);
	m_menu->GetItem(index)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_NEW_CHAT);
	m_menu->GetItem(index)->SetSelPic(SPRITE_MENU, Frame_menu_BG_NEW_CHAT);
	index++;

	rect.top = 182;
	m_menu->AddMenuItem(this, IDC_CHAT_QUIT_ROOM, NULL, rect);
	m_menu->GetItem(index)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_EXIT_CHAT);
	m_menu->GetItem(index)->SetSelPic(SPRITE_MENU, Frame_menu_BG_EXIT_CHAT);
	index++;

	rect.top = 228;
	m_menu->AddMenuItem(this, IDC_CHAT_BACK, NULL, rect);
	m_menu->GetItem(index)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_QUIT_ROOM);
	m_menu->GetItem(index)->SetSelPic(SPRITE_MENU, Frame_menu_BG_QUIT_ROOM);
	index++;

	rect.Set(9, 275, 46, 34);
	m_menu->AddMenuItem(this, IDC_CHAT_COLOR, NULL, rect);
	m_menu->GetItem(index)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_COLOR);
	m_menu->GetItem(index)->SetSelPic(SPRITE_MENU, Frame_menu_BG_COLOR);
	index++;

	//rect.Set(406, 249, 62, 65);
	//m_menu->AddMenuItem(this, IDC_CHAT_SEND, NULL, rect);
	//m_menu->GetItem(index)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_SEND);
	//m_menu->GetItem(index)->SetSelPic(SPRITE_MENU, Frame_menu_BG_SEND);
	//index++;

	rect.Set(64, 277, 336, 30);
	m_menu->AddMenuItem(this, IDE_CHAT_INPUT, NULL, rect);
	m_menu->GetItem(index)->SetPic(SPRITE_MENU, Frame_menu_EDIT_CHAT);
	index++;

	m_setMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(202, 198, 79, 32);
	m_setMenu->AddMenuItem(this, IDC_MSG_OK, GetString(STR_GO), rect);
	m_setMenu->GetItem(0)->SetPic(1, NULL);
	m_setMenu->GetItem(0)->SetSelPic(1, NULL);

	//Init chat info list
	ChatRoomInfo newRoomInfo;
	m_roomInfoList[room->GetJid()] = newRoomInfo;
	m_curRoomInfo = &m_roomInfoList[room->GetJid()];

	m_curRoomInfo->m_msgList.push_back(MsgRecord(SYSTEM_MSG, GetString(ATTENTION_MINORS), 0, true));
	//if the user is the creator
	//int usernameLen = XP_API_STRLEN(m_gl_user->getUserName());
	char username[MAX_USERNAME];
	GLXPlayerChat::ToLower(m_gl_user->getUserName(), username);

	//if( XP_API_STRNCMP(room->GetJid(), username, usernameLen) == 0 && 
	//	(room->GetJid()[usernameLen] == '@' || room->GetJid()[usernameLen] == '-') && room->GetParticipantCount() < 2)
	//{
	//	m_curRoomInfo->m_state = CHAT_PROCESS;
	//	m_sub_state = CHAT_PROCESS;
	//}
	//else
	//{
	//	m_curRoomInfo->m_state = CHAT_CHAT;
	//	m_sub_state = CHAT_CHAT;
	//}
	m_curRoomInfo->m_state = CHAT_PROCESS;
	m_sub_state = CHAT_PROCESS;	
	//Start timer
	m_curRoomInfo->m_timer = XP_API_GET_TIME();

	//Init chat room list
	m_curRoom = room;
	m_chatRoomList = XP_API_NEW GLXPlayerChatRoomList();
	m_chatRoomList->Add( m_curRoom->GetJid(), m_curRoom );


	m_msgColor = 0;
	m_pressFrames = -1;

	//Show Tab bar
	m_bShowingTab = true;
	m_showedTab = 11;

	//Init scroll data
	m_offset = 0;
	m_oldOffset = 0;
	m_touchPos = 0;         //Y-
	m_moving = false;
	m_speed = 0;
	m_mobileHeight = 0;
	m_scrollBarHeight = 0;

	m_gettingAvatar = false;

	m_newChat = false;
	m_newFriend = NULL;

	if(m_pMsgBox == NULL)
	{
		m_pMsgBox = XP_API_NEW CGLLiveMessageBox();
	}
	
	//Init confirm menu
	m_confirmMenu = CGLLiveMessageBox::GenerateConfirmMenu(this);
	XP_API_MEMSET(m_addTalkFriend, 0, sizeof(m_addTalkFriend));
	m_friendUpdated = false;
	m_friendUpdating = false;
	m_isPressingInputArea = false;
	m_isPressingTab = false;
	m_isInviteFriend = false;
	m_scrollMoving = false;

	m_gettingAvatarRoom = NULL;
	m_isAddFriend = false;
}

GLLiveStateChat::~GLLiveStateChat(void)
{
	XP_API_DEL(m_confirmMenu);
	XP_API_DEL(m_menu);
	XP_API_DEL(m_setMenu);
	XP_API_DEL(m_newFriend);

	for(int i = 0; i < m_chatRoomList->Count(); i++)
	{
		m_chatRoomList->Get(i)->Leave();
	}
	XP_API_DEL(m_chatRoomList);
}

bool GLLiveStateChat::HandleEvent(_TOUCH_EVENT event)
{
	switch(m_sub_state)
	{
	case CHAT_CHAT:
		return HandleEventChat(event);
	case CHAT_SETCOLOR:
		return HandleEventSetColor(event);
	case CHAT_PROCESS:
		break;
	case CHAT_ERROR:
		if(event.eType == TET_Pressed)
		{
			m_sub_state = CHAT_CHAT;
		}
		break;
	case CHAT_EXIT_CONFIRM:
	case CHAT_QUIT_ROOM_CONFIRM:
	case CHAT_ADD_FRIEND_CONFIRM:
		return m_confirmMenu->HandleEvent(event);
	}
	return false;
}

bool GLLiveStateChat::HandleEventSetColor(_TOUCH_EVENT event)
{

	if(event.eType == TET_Pressed)
	{
		_TouchRect rect;
		rect.Set(100, 59, 283, 199);
		if(rect.Contain(event.position))
		{
			int offsetY = (event.position.y - 132) % (34 + 22);
			int offsetX = (event.position.x - 135) % (34 + 22);
			if( offsetX <= 34 && offsetY <= 34)
			{
				int row = (event.position.y - 132) / (34 + 22);
				int col = (event.position.x - 135) / (34 + 22);

				int select = row * 4 + col;
				if(select >= 0 && select < 8)
				{
					m_tempMsgColor = select;
					m_isSelectColor = true;
					return true;
				}
			}
		}
		else
		{
			m_isExitColor = true;
		}
	}
	else if(event.eType == TET_Moving)
	{
		if(m_isSelectColor)
		{
			_TouchRect rect;
			rect.Set(100, 59, 283, 199);
			if(rect.Contain(event.position))
			{
				int offsetY = (event.position.y - 132) % (34 + 22);
				int offsetX = (event.position.x - 135) % (34 + 22);
				if( offsetX <= 34 && offsetY <= 34)
				{
					int row = (event.position.y - 132) / (34 + 22);
					int col = (event.position.x - 135) / (34 + 22);

					int select = row * 4 + col;
					if(select >= 0 && select < 8)
					{
						m_tempMsgColor = select;
						m_isSelectColor = true;
						return true;
					}
				}
			}
			else
			{
				m_isSelectColor = false;
			}
		}

		else if(m_isExitColor)
		{
			_TouchRect rect;
			rect.Set(100, 59, 283, 199);
			if(rect.Contain(event.position))
			{
				m_isExitColor = false;
			}
		}
	}
	else if(event.eType == TET_Released)
	{
		if(m_isSelectColor)
		{
			m_isSelectColor = false;
			m_msgColor = m_tempMsgColor;
			m_sub_state = CHAT_CHAT;
		}
		else if(m_isExitColor)
		{
			m_sub_state = CHAT_CHAT;
		}
	}

	//if(m_setMenu->HandleEvent(event))
	//{
	//	return true;
	//}
	return false;
}

bool GLLiveStateChat::HandleEventChat(_TOUCH_EVENT event)
{
	_TouchRect rect;
	if(m_menu->HandleEvent(event))
	{
		return true;
	}
	switch(event.eType)
	{
	case TET_Pressed:
		//Click the scroll bar
		rect.Set(388 - 5 - 1 - 5,
			     70 + m_offset * /*(260 - m_scrollBarHeight )*/ 181 / m_mobileHeight - 1 - 5,
			     30, 
				 30);
		if(rect.Contain(event.position))
		{
			m_scrollMoving = true;
		}
		//Click the tab bar
		else if(event.position.y >= 19 && event.position.y <= (19 + 32) && event.position.x >= 136)
		{
			m_isPressingTab = true;
			m_bShowingTab = true;
		}
		//Click the friend area to add friend
		else if( event.position.x > 395 && event.position.y > 63 && event.position.y < 242 )//&& (!m_curRoomInfo->m_showAvatar))
		{
			const char *selTalker = GetPosTalker(event.position.y);
			
			if(selTalker != NULL)
			{
				XP_DEBUG_OUT("[GLLiveStateChat] Click user -- %s\n", selTalker);
			}

			if(selTalker != NULL)
			{
				bool isFriend = false;
				for(int i = 0; i < m_gl_userFriend->GetFriendsCount(); i++)
				{
					if(XP_API_STRICMP(m_gl_userFriend->GetName(i) , selTalker) == 0)
					{
						isFriend = true;
						break;
					}
				}
				if(!isFriend)
				{
					char msg[128];
					XP_API_SPRINTF(msg, GetString(STR_CHATROOM_ADD), selTalker);
					m_pMsgBox->SetTitle(GetString(STR_WARNING));
					m_pMsgBox->SetText(msg);
					m_pMsgBox->SetButtonType(1);
					m_sub_state = CHAT_ADD_FRIEND_CONFIRM;
					XP_API_STRCPY(m_addTalkFriend, selTalker);
					return true;
				}
			}
		}
		else if(m_mobileHeight > 0)
		{
			//Begin moving
			m_touchPos = event.position.y;
			m_moving   = true;
		}
		break;
	case TET_Moving:
		if(m_scrollMoving)
		{
			m_offset = (event.position.y - 70 - 10) * m_mobileHeight / 181;

			if(m_offset < 0 )
			{
				m_offset = 0;
				m_speed = 0;
			}
			else if(m_offset > m_mobileHeight)
			{
				m_speed = 0;
				m_offset = m_mobileHeight;
			}
		}
		//Handle scroll data
		else if(m_moving)
		{
			m_oldOffset = m_offset;
			m_offset += (m_touchPos - event.position.y);
			if(m_offset < 0 )
			{
				m_offset = 0;
				m_speed = 0;
			}
			else if(m_offset > m_mobileHeight)
			{
				m_speed = 0;
				m_offset = m_mobileHeight;
			}
			m_touchPos = event.position.y;

			//if move distance is more than 5, begin to hide tab bar
			if( m_pressFrames > -1 && ABS(m_offset - m_oldOffset) >= 5 )
			{
				m_pressFrames = -1;
				m_bShowingTab = false;
			}
		}
		else if(m_isPressingTab)
		{
			if( !(event.position.y >= 19 && event.position.y <= (19 + 32) && event.position.x >= 136) )
			{
				m_isPressingTab = false;
			}
		}
		break;
	case TET_Released:
		if(m_scrollMoving)
		{
			m_scrollMoving = false;
		}
		if(m_moving)
		{
			m_moving = false;
		}
		else if(m_isPressingTab)
		{
			if(event.position.y >= 19 && event.position.y <= (19 + 32) && event.position.x >= 136)
			{
				int roomCount = m_chatRoomList->Count();
				float tabWidth = 344.0f / (roomCount + SELTAB_INCREASE);;
				float tempX = 480 - 344;
				int index = 0;
				while(index < roomCount)
				{
					float width = tabWidth * (( m_chatRoomList->Get(index) == m_curRoom) ? (1 + SELTAB_INCREASE) : 1);
					if(event.position.x >= tempX && event.position.x <= tempX + width)
					{
						break;
					}
					
					tempX += width;
					index++;
				}
				//Exist Chat room
				if(index < roomCount)
				{
					m_curRoom = m_chatRoomList->Get(index);
					m_curRoomInfo = &m_roomInfoList[m_curRoom->GetJid()];
					UpdateMobileHeight();
					m_offset = m_mobileHeight;
				}
				//New Chat
				else
				{
				}
				m_isPressingTab = false;
				m_bShowingTab = false;
			}
		}
		m_pressFrames = -1;
		break;
	}
	return false;
}

extern CGLImage * g_pAvatarImage;
void GLLiveStateChat::PaintChat()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     * fName = m_handler->GetFont(FONT_BLACK_BIG);
	CGLLiveFont     * fContent = m_handler->GetFont(FONT_WHITE_SMALL);

	int posY = 0;

	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_TITLE, 0, 0, 0);
	//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_EDIT_CHAT, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_CHAT_ROOM, 0, 0, 0);
	//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHAT_SEPARATOR, 395, 65, 0);
	//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHAT_SEPARATOR, 395, 245, 0);


	PaintAvatar(g_pAvatarImage, 5, 15, 50, 50);

	fName->DrawString(g, m_gl_user->getUserName(), 60, 36, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP, 80);

	m_menu->Repaint(m_handler);

	//Draw friend's avatar
	posY = 70;
	if(m_curRoomInfo->m_showAvatar)
	{
		//Draw friend if there is only one
		//int i = m_curRoom->GetParticipantCount();
		if(m_curRoom->GetParticipantCount() == 2)
		{
			//Get Friend name
			char *friendName = NULL;
			if(XP_API_STRICMP(m_curRoom->GetParticipant(0)->name, m_gl_user->getUserName()) != 0)
			{
				friendName = m_curRoom->GetParticipant(0)->name;
			}
			else
			{
				friendName = m_curRoom->GetParticipant(1)->name;
			}

			//Draw avatar
			int offx_right = 413;
			PaintAvatar(m_curRoomInfo->m_friendAvatar, offx_right, posY, 52, 52);
			fName->DrawString(g, friendName, offx_right + 50 / 2, posY + 50 + 7, CGLLiveGraphics::TOP | CGLLiveGraphics::HCENTER, 70);
		}

		//Paint + if is not friend
		if(!m_curRoomInfo->m_isFriend)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHAT_ADD_FRIEND, 405 - 10, posY - 2, 0, 0, 0);
		}
	}

	//Paint chat content
	g->SetClip(0, 72, SCREEN_WIDTH, 196);
	LIST_MSG *m_curMsgList = &m_curRoomInfo->m_msgList;
	IT_LIST_MSG it = m_curMsgList->begin();

	posY = 77 - m_offset;
	while(it != m_curMsgList->end())
	{
		float textHeight = 0;
		//float textWidth = 0;

		//Beyond the visible range, stop paint
		if(posY > 70 + 200)
		{
			break;
		}

		//Generate text height and width
		int lines = fContent->InitPage((char*)it->content.c_str(), 293 - 10);
		textHeight =  lines * fContent->GetLineHeight() + MSG_INTERVAL;
		//Less than ONE line
		//if(lines <= 1)
		//{
		//	fContent->UpdateStringSize((char*)it->content.c_str());
		//	textWidth = fContent->GetStringSize() + 10;
		//}
		//else
		//{
		//	textWidth = 293;
		//}

		//Paint visible msg
		if(posY + textHeight >= 65)
		{
			//g->BeginScale( 1.0f, textHeight / 105, 80, posY);
			//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHAT_MSG_BG0, 80, posY, 0);
			//g->EndScale();

			//System msg
			if(it->from == SYSTEM_MSG)
			{
				//fContent->DrawPage(g, (char*)it->content.c_str(), 85, posY + textHeight / 2, CGLLiveGraphics::VCENTER | CGLLiveGraphics::LEFT);
			}
			//Left
			else if(XP_API_STRICMP(it->from.c_str(),  m_gl_user->getUserName()) == 0)
			{
				//Paint Message
				//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHAT_POINT_LEFT0, 80 - 14 + 1, posY + 2, 0);
				PaintMessageRect(posY, textHeight);
			}
			//Right
			else
			{
				//Paint Talker if participents is more than 2
				if(!m_curRoomInfo->m_showAvatar)
				{
					//g->SetClip(0, 67, SCREEN_WIDTH, 178);
					//m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TALKER, 400, posY, 0, 0, 0);
					fName->DrawString(g, (char*)it->from.c_str(), 405 + 6, posY - 7, CGLLiveGraphics::TOP | CGLLiveGraphics::LEFT, 70);
					if(!it->isFriend)
					{
						m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHAT_ADD_FRIEND, 405 - 10, posY - 8, 0, 0, 0);
					}
					//g->SetClip(0, 75, SCREEN_WIDTH, 186);
				}
				//Paint Message
				//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHAT_POINT_RIGHT0, 372, posY + 2, 0);
				PaintMessageRect(posY, textHeight, false);
			}

			fContent->SetFontColor(g_chatColor[it->color]);
			fContent->DrawPage(g, (char*)it->content.c_str(), 85, posY + textHeight / 2, CGLLiveGraphics::VCENTER | CGLLiveGraphics::LEFT);
			fContent->SetFontColor(0xffffffff);
		}
	
		posY += textHeight + MSG_INTERVAL;
		it++;
	}

	g->ResetClip();

	//Paint scroll bar
	if(/*m_moving && */m_mobileHeight > 0)
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_CHAT, 388, 75, 0);

		int scrollPosY = m_offset * /*(260 - m_scrollBarHeight )*/ 181 / m_mobileHeight;
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR1, 
			                     388 - 6,
								 70 + scrollPosY,
								 0);

		scrollPosY += 3;

		//g->SetClip(400, scrollPosY, 10, (m_scrollBarHeight - 6));
		//int height = 0;
		//while(height < m_scrollBarHeight - 6)
		//{
		//	m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_SCROLL_MID, 
		//	                     400,
		//						 scrollPosY + height,
		//						 0);

		//	height += 25;
		//}
		//g->ResetClip();

		//scrollPosY += m_scrollBarHeight - 6;

		//m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_SCROLL_DOWN, 
		//	                     400,
		//						 scrollPosY,
		//						 0);
	}

	//Draw Tab Bar
	if(true)//m_showedTab > 0)
	{
		CGLLiveFont *fTitle = m_handler->GetFont(FONT_WHITE_MID);
		CGLLiveFont *fTitle2= m_handler->GetFont(FONT_WHITE_MID_GRAY);
		int roomCount = m_chatRoomList->Count();
		float tabWidth = 344.0f / (roomCount + SELTAB_INCREASE);;
		float tempX = 480 - 344;
		for(int j = 0; j < roomCount; j++)
		{
			float width;
			if(m_curRoom == m_chatRoomList->Get(j))
			{
				width = (1 + SELTAB_INCREASE) * tabWidth;
				fTitle->DrawString(g, m_chatRoomList->Get(j)->GetName(), tempX + 16 + (width - 16) / 2, 35, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER, width - 16 * 2);
			}
			else
			{
				width = tabWidth;
				fTitle2->DrawString(g, m_chatRoomList->Get(j)->GetName(), tempX + 16 + (width - 16) / 2, 35, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER, width - 16 * 2);
			}

			tempX += width;

			if(j < roomCount - 1)
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_ROOM_SEPARATOR, tempX, 19, 0);
			}
		}
	}

	if((m_frameCount / 15) % 2 == 0)
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_INPUT_INDICATOR, 77, 281, 0);
	}
}

void GLLiveStateChat::PaintMessageRect(int y, int height, bool isLeft)
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	int frame_top;
	int frame_mid;
	int frame_bottom;
	y -= 2;

	if(isLeft)
	{
		frame_top = Frame_menu_CHAT_LEFT_CORNER;
		frame_mid = Frame_menu_CHAT_LEFT_LINE;
		frame_bottom = Frame_menu_CHAT_LEFT_BOTTOM;
	}
	else
	{
		frame_top = Frame_menu_CHAT_RIGHT_CORNER;
		frame_mid = Frame_menu_CHAT_RIGHT_LINE;
		frame_bottom = Frame_menu_CHAT_RIGHT_BOTTOM;
	}

	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, frame_top, 0, y, 0);
	if(height > 23)
	{
		int h = (height - 23);
		for(int i = 0; i < h; i += 9)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, frame_mid, 0, y + i, 0);
		}
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, frame_bottom, 0, y + h, 0);
	}
	else
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, frame_bottom, 0, y, 0);
	}
}

void GLLiveStateChat::Paint()
{
	m_handler->ClearScreen();
	switch(m_sub_state)
	{
	case CHAT_CHAT:
		PaintChat();
		break;
	case CHAT_PROCESS:
		PaintChat();
		PaintLoading();
		break;
	case CHAT_SETCOLOR:
		PaintSetColor();
		break;
	case CHAT_ERROR:
		PaintChat();
		m_pMsgBox->Repaint(m_handler);
		break;
	case CHAT_EXIT_CONFIRM:
	case CHAT_QUIT_ROOM_CONFIRM:
	case CHAT_ADD_FRIEND_CONFIRM:
		PaintChat();
		m_pMsgBox->Repaint(m_handler);
		m_confirmMenu->Repaint(m_handler);
		break;
	}
}

void GLLiveStateChat::PaintSetColor()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont *fTitle = m_handler->GetFont(FONT_WHITE_BIG);

	PaintChat();

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

	//Paint bg
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_CHAT_COLOR, 0, 0, 0);
	//Paint title
	fTitle->DrawString(g, GetString(STR_FONT_COLOR), 190, 83, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);

	int sepWidth = 22;
	for(int i = 0; i < 8; i++)
	{
		if(i == m_tempMsgColor)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHAT_COLOR_SELECT, 
				135 + (i % 4) * (sepWidth + 34) - 6,
				132 + i / 4 * (34 + sepWidth) - 5,
				0,
				0);
		}
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHAT_COLOR0 + i, 
			135 + (i % 4) * (sepWidth + 34),
			132 + i / 4 * (34 + sepWidth),
			0,
			0);
	}

	//m_setMenu->Repaint(m_handler);
}

void GLLiveStateChat::UpdateAnimation()
{
	switch(m_sub_state)
	{
	case CHAT_CHAT:
		UpdateAnimationChat();
		break;
	case CHAT_SETCOLOR:
		m_setMenu->Update();
		break;
	case CHAT_EXIT_CONFIRM:
	case CHAT_QUIT_ROOM_CONFIRM:
	case CHAT_ADD_FRIEND_CONFIRM:
		m_confirmMenu->Update();
		break;
	default:
		break;
	}
	GLLiveState::UpdateAnimation();

	if( !m_friendUpdated && !m_friendUpdating)
	{
		m_gl_userFriend->sendGetUserFriends();
		m_friendUpdating = true;
	}

	//Update Timer
	long curTime = XP_API_GET_TIME();
	MAP_CHAT_ROOM_INFO::iterator it = m_roomInfoList.begin();
	while(it != m_roomInfoList.end())
	{
		if(it->second.m_timer > -1)
		{
			if( curTime - it->second.m_timer > MAX_CHAT_ROOM_TIMEOUT)
			{
				it->second.m_state = CHAT_CHAT;
				it->second.m_msgList.push_back(MsgRecord(SYSTEM_MSG, GetString(STR_CHAT_TIMEOUT), 0, true));
				if(XP_API_STRCMP(it->first.c_str(), m_curRoom->GetJid()) == 0)
				{
					it->second.m_timer = -1;        //Stop timer
					if(m_sub_state == CHAT_PROCESS)
					{
						m_sub_state = CHAT_CHAT;
					}
					UpdateMobileHeight();
					m_offset = m_mobileHeight;
				}
			}
		}
		it++;
	}
}

void GLLiveStateChat::UpdateAnimationChat()
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
			else if(m_offset > m_mobileHeight)
			{
				m_offset = m_mobileHeight;
				m_speed = 0;
			}
		}
	}

	//Update for menu
	m_menu->Update();

	if(!m_gettingAvatar)
	{
		MAP_CHAT_ROOM_INFO::iterator it = m_roomInfoList.begin();
		while(it != m_roomInfoList.end())
		{
			//Geting friend avatar
			if(it->second.m_showAvatar && it->second.m_friendAvatar == NULL)
			{
				GLXPlayerChatRoom *room = m_chatRoomList->Find(it->first.c_str());
				if(room->GetParticipantCount() == 2)
				{
					//Get Friend name
					char *friendName = NULL;
					if(XP_API_STRICMP(room->GetParticipant(0)->name, m_gl_user->getUserName()) != 0)
					{
						friendName = room->GetParticipant(0)->name;
					}
					else
					{
						friendName = room->GetParticipant(1)->name;
					}
					//Download from server
					m_gl_user->sendGetUserPhoto(friendName);
					m_gettingAvatar = true;
					m_gettingAvatarRoom = &(it->second);
					break;
				}
			}
			//Next
			it++;
		}
	}
}

void GLLiveStateChat::SwitchState()
{
	m_gettingAvatar = false;
	//Back from eidt state
	if(XP_API_STRLEN(m_inputMsg) > 0)
	{
		OnMenuItemClicked(IDC_CHAT_SEND);
	}

	//Back from friend list
	if(m_newFriend != NULL)
	{
		if(XP_API_STRLEN(m_newFriend) > 0)
		{
			//Add friend to new chat room
			if(m_newChat)
			{
			}
			//Add friend to current chat room
			else
			{
				//Find if he has been in the room
				bool hasIn = false;
				for(int i = 0; i < m_curRoom->GetParticipantCount(); i++)
				{
					if(XP_API_STRICMP(m_curRoom->GetParticipant(i)->name, m_newFriend) == 0)
					{
						hasIn = true;
						break;
					}
				}
				if(hasIn)
				{
					XP_DEBUG_OUT("[GLLiveStateChat] %s has been in the room.\n", m_newFriend);
				}
				else
				{
					char lname[MAX_USERNAME];
					GLXPlayerChat::ToLower(m_newFriend, lname);
					extern int getLangIndexFromServer(char* lang);
					//get the index of the friend
					int i = 0;
					for(; i < m_gl_userFriend->GetFriendsCount(); i++)
					{
						if(XP_API_STRCMP(m_newFriend, m_gl_userFriend->GetName(i)) == 0)
							break;
					}
					
					if(m_gl_userFriend->GetState(i) == XPLAYER_USER_STATE_ONLINE)
					{
						m_curRoom->Invite(lname, GetString(STR_CHAT_INVITE_MSG, getLangIndexFromServer(m_gl_userFriend->GetCurrentLanguage(i))), m_gl_user->getUserName());
					}
					else
					{
						char* title = NULL;
						char* body = NULL;
						char* inviteTitle = GetString(STR_CHAT_INVITE_TITLE , getLangIndexFromServer(m_gl_userFriend->GetCurrentLanguage(i)));
						char* temp_msg = GetString(STR_CHAT_INVITE_MSG, getLangIndexFromServer(m_gl_userFriend->GetCurrentLanguage(i)));
						int size1;
						int size2;
						base64_encode(inviteTitle, XP_API_STRLEN(inviteTitle), title, size1);
						base64_encode(temp_msg, XP_API_STRLEN(temp_msg), body, size2);
						m_gl_message->SendOnlineMessage(m_gl_userFriend->GetName(i), USER_MESSAGE_TYPE_USERMESSAGE, title, body, XP_API_STRLEN(body));
						XP_API_DELETE_ARRAY(title);
						XP_API_DELETE_ARRAY(body);
					}
				}
			}
		}
		XP_API_DEL(m_newFriend);
	}
}

void GLLiveStateChat::OnRequestFailure(int func_id, int error_code)
{
	switch(func_id)
	{
	case FUNCTION_DOWNLOAD_USER_AVATAR:
	case FUNCTION_GET_USER_AVATAR:
		XP_DEBUG_OUT("[GLLiveStateChat] Get friend avatar failed!\n");
		if(m_gettingAvatarRoom->m_friendAvatar == NULL)
		{
			m_gettingAvatarRoom->m_friendAvatar = XP_API_NEW CGLImage();
		}
		m_gettingAvatar = false;
		m_gettingAvatarRoom  = NULL;
		break;
	case FUNCTION_GET_USER_FRIENDS:
	case FUNCTION_ADD_USER_FRIEND:
	case 0:
		XP_DEBUG_OUT( "[GLLiveStateChat]  Get friends list failed!\n" );
		m_sub_state = CHAT_ERROR;
		m_pMsgBox->SetTitle(GetString(STR_ERROR));
		m_pMsgBox->SetText(GetErrorString(error_code));
		m_isInviteFriend = false;
		return;
	case FUNCTION_SEND_MESSAGE:
		break;
	default:
		GLLiveState::OnRequestFailure(func_id, error_code);
		break;
	}
}

void GLLiveStateChat::OnRequestSuccess(int func_id, char *data, int data_len)
{
	switch(func_id)
	{
	case FUNCTION_GET_USER_AVATAR:
		XP_DEBUG_OUT("[GLLiveStateChat] Get friend avatar id OK!\n");
		m_gettingAvatarRoom->m_friendAvatar = LoadPicFromLocal(m_gl_user->getOtherUserAvatar());
		if(m_gettingAvatarRoom->m_friendAvatar == NULL)
		{
			m_gl_user->sendDownloadAvatar(m_gl_user->getOtherUserAvatar());
		}
		else
		{
			m_gettingAvatarRoom  = NULL;
			m_gettingAvatar = false;
		}
		break;
	case FUNCTION_DOWNLOAD_USER_AVATAR:
		XP_DEBUG_OUT("[GLLiveStateChat] Get friend avatar OK!\n");

		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			//nothing to do
			//SavePicToLocal(m_gl_user->getOtherUserAvatar(), NULL, 0);
			m_gettingAvatarRoom->m_friendAvatar = XP_API_NEW CGLImage();
		}
		else
		{
			char *	base64_buf = NULL;
			int		base64_size;
			base64_decode(data + 1, data_len - 1, base64_buf, base64_size);
			m_gettingAvatarRoom->m_friendAvatar = NEW CGLImage(base64_buf, base64_size);

			SavePicToLocal(m_gl_user->getOtherUserAvatar(), base64_buf, base64_size);

			XP_API_DEL(base64_buf);
		}
		m_gettingAvatarRoom  = NULL;
		m_gettingAvatar = false;
		break;
	case FUNCTION_GET_USER_FRIENDS:
		XP_DEBUG_OUT( "[GLLiveStateChat]  Get friends list OK!\n" );
		if( !m_friendUpdated )
		{
			//Parse the message list
			MAP_CHAT_ROOM_INFO::iterator room_it = m_roomInfoList.begin();
			while(room_it != m_roomInfoList.end())
			{
				IT_LIST_MSG msg_it = room_it->second.m_msgList.begin();
				while(msg_it != room_it->second.m_msgList.end())
				{
					bool isFriend = false;
					for(int i = 0; i < m_gl_userFriend->GetFriendsCount(); i++)
					{
						if( XP_API_STRICMP(m_gl_userFriend->GetName(i), msg_it->from.c_str()) == 0)
						{
							isFriend = true;
							break;
						}
					}
					msg_it->isFriend = isFriend;
					msg_it++;
				}

				//Parse friend if there is 2 person
				GLXPlayerChatRoom *pRoom = m_chatRoomList->Find(room_it->first.c_str());
				if(pRoom->GetParticipantCount() == 2 && room_it->second.m_showAvatar)
				{
					const char *partName = NULL;
					if(XP_API_STRICMP(pRoom->GetParticipant(0)->name, m_gl_user->getUserName()) != 0)
					{
						partName = pRoom->GetParticipant(0)->name;
					}
					else
					{
						partName = pRoom->GetParticipant(1)->name;
					}
					room_it->second.m_isFriend = false;
					for(int i = 0; i < m_gl_userFriend->GetFriendsCount(); i++)
					{
						char *name = m_gl_userFriend->GetName(i);
						if(XP_API_STRICMP(partName, name) == 0)
						{
							room_it->second.m_isFriend = true;
							break;
						}
					}
				}
				room_it++;
			}
			m_friendUpdated = true;
			m_friendUpdating = false;
			if(m_isAddFriend)
			{
				m_sub_state = CHAT_CHAT;
			}
		}
		if(m_isInviteFriend)
		{
			m_isInviteFriend = false;
			m_newFriend = XP_API_NEW char[MAX_USERNAME];
			XP_API_MEMSET(m_newFriend, 0, MAX_USERNAME);
			m_sub_state = CHAT_CHAT;
			m_handler->PushGLLiveState( XP_API_NEW GLLiveStateFriends(m_handler, m_newFriend));
			m_gettingAvatarRoom  = NULL;
			m_gettingAvatar = false;			
		}
		return;
	case FUNCTION_ADD_USER_FRIEND:
		//m_sub_state = CHAT_CHAT;
		m_gl_userFriend->sendGetUserFriends();
		m_friendUpdated = false;
		m_friendUpdating = true;
		m_isAddFriend = true;
		break;
	case FUNCTION_SEND_MESSAGE:
		break;
	default:
		GLLiveState::OnRequestSuccess(func_id, data, data_len);
		break;
	}
}

void GLLiveStateChat::OnChatRoomMessage(const char *room, const char *nickName, const char *message)
{
	XP_DEBUG_OUT( "[GLLiveStateChat] Receive Message!\n" );
	//Resolve the color
	char pTemp[8];
	int color = 0;
	int index = 0;
	if(message[0] == CHAT_SEPARATOR_FLAG[0])
	{
		index = 0;
		while(message[index + 1] != CHAT_SEPARATOR_FLAG[0])
		{
			pTemp[index] = message[index + 1];
			index++;
		}
		pTemp[index] = '\0';
		color = XP_API_ATOI(pTemp);

		index += 2;
	}


	if(XP_API_STRLEN(nickName) == 0)
	{
#ifdef CHAT_LOADING
		//If unlocked, starting chat
		//if(XP_API_STRCMP(message, "This room is now unlocked.") == 0 || XP_API_STRCMP(message, "This room is not anonymous.") == 0)
		//{
		//	m_sub_state = CHAT_CHAT;
		//}
#endif
	}
	else
	{
		ChatRoomInfo *tempRoom = &m_roomInfoList[room];
		
		if(tempRoom->m_showAvatar)
		{
			GLXPlayerChatRoom *pRoom = m_chatRoomList->Find(room);
			bool isIn = false;
			for(int i = 0; i < pRoom->GetParticipantCount(); i++)
			{
				if(XP_API_STRICMP(pRoom->GetParticipant(i)->name, nickName) == 0)
				{
					isIn = true;
					break;
				}
			}
			if(!isIn)
			{
				tempRoom->m_showAvatar = false;
			}
		}

		bool isFriend = false;
		if(m_friendUpdated)
		{
			if( XP_API_STRICMP(m_gl_user->getUserName(), nickName) != 0 )
			{
				for(int i = 0; i < m_gl_userFriend->GetFriendsCount(); i++)
				{
					if(XP_API_STRICMP(m_gl_userFriend->GetName(i), nickName) == 0)
					{
						isFriend = true;
						break;
					}
				}
			}
			else
			{
				isFriend = true;
			}
		}
		else
		{
			isFriend = true;
		}
		tempRoom->m_msgList.push_back(MsgRecord(nickName, message + index, color, isFriend));
	}
	UpdateMobileHeight();
	m_offset = m_mobileHeight;
}

void GLLiveStateChat::CheckShowAvatar(const char *room_jid, const char *newMember)
{
	ChatRoomInfo *pTempRoom = &m_roomInfoList[room_jid];
	//if(!pTempRoom->m_showAvatar)
	//{
	//	return;
	//}
	if(m_chatRoomList->Find(room_jid)->GetParticipantCount() > 2)
	{
		pTempRoom->m_showAvatar = false;
	}
	//Check if there is the third person has said words
	else if(m_chatRoomList->Find(room_jid)->GetParticipantCount() == 2)
	{
		const char *user1 = m_chatRoomList->Find(room_jid)->GetParticipant(0)->name;
		const char *user2 = m_chatRoomList->Find(room_jid)->GetParticipant(1)->name;
		bool isIn = false;
		IT_LIST_MSG it = pTempRoom->m_msgList.begin();
		while(it != pTempRoom->m_msgList.end())
		{
			if( XP_API_STRICMP(it->from.c_str(),  SYSTEM_MSG) != 0 && XP_API_STRICMP(it->from.c_str(),  user1) != 0 && XP_API_STRICMP(it->from.c_str(),  user2) != 0)
			{
				isIn = true;
				break;
			}
			it++;
		}
		if(isIn)
		{
			pTempRoom->m_showAvatar  = false;
		}
		else
		{
			pTempRoom->m_showAvatar  = true;
		}
	}
}


//This function will be called only when someone enter the chat room
void GLLiveStateChat::OnChatRoomPresence(const char *room_jid, const char *user_name, int state)
{
	m_chatRoomList->Find(room_jid)->OnParticipantPresence( user_name, state );
	switch(state)
	{
	case GL_XPLAYER_PARTICIPANT_ONLINE:
		{
		ChatRoomInfo *pTempRoom = &m_roomInfoList[room_jid];

		char temp[64];
		XP_API_MEMSET(temp, 0, 64);
		XP_API_SPRINTF(temp, GetString(STR_CHAT_JOIN_MSG), user_name);
		pTempRoom->m_msgList.push_back(MsgRecord(SYSTEM_MSG, temp, 0, true));
		UpdateMobileHeight();
		m_offset = m_mobileHeight;

		CheckShowAvatar(room_jid, user_name);

		m_roomInfoList[room_jid].m_state = CHAT_CHAT;
		m_roomInfoList[room_jid].m_timer = -1;             //Stop timer
		if(XP_API_STRCMP(room_jid, m_curRoom->GetJid()) == 0)
		{
			m_sub_state = CHAT_CHAT;
		}


		if(m_friendUpdated && m_chatRoomList->Find(room_jid)->GetParticipantCount() <= 2 && XP_API_STRICMP(user_name, m_gl_user->getUserName()) != 0)
		{
			pTempRoom->m_isFriend = false;
			for(int i = 0; i < m_gl_userFriend->GetFriendsCount(); i++)
			{
				char *name = m_gl_userFriend->GetName(i);
				if(XP_API_STRICMP(user_name, name) == 0)
				{
					pTempRoom->m_isFriend = true;
					break;
				}
			}
		}

		break;
		}
	case GL_XPLAYER_PARTICIPANT_CHAT:
		break;
	case GL_XPLAYER_PARTICIPANT_AWAY:
		break;
	case GL_XPLAYER_PARTICIPANT_BUSY:
		break;
	case GL_XPLAYER_PARTICIPANT_OFFLINE:
		
		ChatRoomInfo *pTempRoom = &m_roomInfoList[room_jid];

		char temp2[64];
		XP_API_MEMSET(temp2, 0, 64);
		XP_API_SPRINTF(temp2, GetString(STR_CHAT_LEFT_MSG), user_name);
		pTempRoom->m_msgList.push_back(MsgRecord(SYSTEM_MSG, temp2, 0, true));
		UpdateMobileHeight();
		m_offset = m_mobileHeight;

		pTempRoom->m_showAvatar = false;
		XP_API_DEL(pTempRoom->m_friendAvatar);
		break;
	}
}

bool GLLiveStateChat::IsExist(const char *roomName)
{
	MAP_CHAT_ROOM_INFO::iterator it = m_roomInfoList.begin();
	int len = XP_API_STRLEN(roomName);
	while(it != m_roomInfoList.end() )
	{
		if( XP_API_STRICMP(it->first.c_str(), roomName, len) == 0 && it->first.at(len) == '@')
		{
			return true;
		}
		it++;
	}
	return false;
}

void GLLiveStateChat::OnChatRoomInvitationDecline( const char * room, const char * invitee, const char * reason )
{
	XP_DEBUG_OUT("[GLLiveStateChat] OnChatRoomInvitationDecline!");
}

void GLLiveStateChat::OnMenuItemClicked(int id)
{
	m_showedTab = 0;
	m_pressFrames = -1;
	m_bShowingTab = false;
	switch(id)
	{
	case IDC_CHAT_SEND:
		if(XP_API_STRLEN(m_inputMsg) > 0)
		{
			char *pTemp = XP_API_NEW char[sizeof(m_inputMsg)];
			XP_API_MEMSET(pTemp, 0, sizeof(m_inputMsg));
			XP_API_SPRINTF(pTemp, "%s%d%s%s", CHAT_SEPARATOR_FLAG, m_msgColor, CHAT_SEPARATOR_FLAG,m_inputMsg); 
			bool result = m_curRoom->SendChatMessage(pTemp);
			XP_API_MEMSET(m_inputMsg, 0, sizeof(m_inputMsg));
			XP_API_DELETE_ARRAY(pTemp);
		}
		break;
	case IDC_CHAT_BACK:
		m_pMsgBox->SetTitle(GetString(STR_WARNING));
		m_pMsgBox->SetText(GetString(STR_BACK_MAIN));
		m_pMsgBox->SetButtonType(1);
		m_sub_state = CHAT_EXIT_CONFIRM;
		//Show Tab bar
		m_bShowingTab = true;
		m_showedTab = 11;
		return;
	case IDC_CHAT_COLOR:
		m_sub_state = CHAT_SETCOLOR;
		m_tempMsgColor = m_msgColor;
		m_isSelectColor = false;
		m_isExitColor = false;
		break;
	case IDC_MSG_OK:
		m_msgColor = m_tempMsgColor;
		m_sub_state = CHAT_CHAT;
		break;
	case IDC_CHAT_ADD:
		if(m_curRoom->GetParticipantCount() >= MAX_CHAT_MEMBER)
		{
			m_sub_state = CHAT_ERROR;

			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetString(STR_CHATROOM_FULL));
			break;
		}
		m_newChat = false;
		m_gl_userFriend->sendGetUserFriends();
		m_sub_state = CHAT_PROCESS;
		m_isInviteFriend = true;
		break;
	case IDC_CHAT_QUIT_ROOM:
		m_pMsgBox->SetTitle(GetString(STR_WARNING));
		m_pMsgBox->SetText(GetString(STR_EXIT_CHATROOM));
		m_pMsgBox->SetButtonType(1);
		m_sub_state = CHAT_QUIT_ROOM_CONFIRM;
		//Show Tab bar
		m_bShowingTab = true;
		m_showedTab = 11;
		break;
	case IDC_MSG_YES:
		if(m_sub_state == CHAT_EXIT_CONFIRM)
		{
			m_handler->PopGLLiveState();
			return;
		}
		else if(m_sub_state == CHAT_QUIT_ROOM_CONFIRM)
		{
			//Leave
			m_curRoom->Leave();
			//Delete msg record
			m_roomInfoList.erase(m_curRoom->GetJid());
			//Delete from chat room list
			m_curRoom = m_chatRoomList->Remove(m_curRoom->GetJid());
			if(m_curRoom == NULL)
			{
				m_handler->PopGLLiveState();
				return;
			}

			CancelRequest();
			
			m_curRoomInfo = &m_roomInfoList[m_curRoom->GetJid()];
			m_sub_state = m_curRoomInfo->m_state;
			UpdateMobileHeight();
			m_offset = m_mobileHeight;
			m_sub_state = CHAT_CHAT;
		}
		else if(m_sub_state == CHAT_ADD_FRIEND_CONFIRM && XP_API_STRLEN(m_addTalkFriend) > 0)
		{
			m_sub_state = CHAT_PROCESS;
			m_gl_userFriend->sendAddUserFriend(m_addTalkFriend, GetString(STR_ADD_REQUEST_MESSAGE));
			XP_API_MEMSET(m_addTalkFriend, 0, sizeof(m_addTalkFriend));
		}
		else
		{
			m_sub_state = CHAT_CHAT;
		}
		break;
	case IDC_MSG_NO:
		m_sub_state = CHAT_CHAT;
		if(m_sub_state == CHAT_ADD_FRIEND_CONFIRM)
		{
			XP_API_MEMSET(m_addTalkFriend, 0, sizeof(m_addTalkFriend));
		}
		break;
	case IDE_CHAT_INPUT:
#ifdef WIN32
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, "Message", m_inputMsg,false, 256));
#else
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, m_inputMsg, this));
#endif
		break;
	case IDC_CHAT_NEW_ROOM:
		if(m_chatRoomList->Count() < 8)
		{
			char roomName[64];
			XP_API_MEMSET(roomName, 0, sizeof(roomName));

			//Find a new chat room name that is not in cur room list
			int i = 1;
			do
			{
				XP_API_SPRINTF(roomName, "%d-%s", i, m_gl_user->getUserName());
				i++;
			}
			while( IsExist(roomName) );

			GLXPlayerChatRoom *newRoom = m_gl_chat->CreateChatRoom(roomName, m_gl_user->getUserName());
			//Add to room list
			AddChatRoom(newRoom);
			//Switch to new room
			m_curRoom = newRoom;
			m_curRoomInfo = &m_roomInfoList[m_curRoom->GetJid()];
			UpdateMobileHeight();
			m_offset = 0;
		}
		else
		{
			m_sub_state = CHAT_ERROR;

			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetString(STR_CHAT_FULL));
		}
		break;
	}
}

void GLLiveStateChat::UpdateMobileHeight()
{
	CGLLiveFont *f4 = m_handler->GetFont(9);
	int totalHeight = 0;

	IT_LIST_MSG it = m_curRoomInfo->m_msgList.begin();
	while(it != m_curRoomInfo->m_msgList.end())
	{
		//Message height
		int lineHeight = f4->GetLineHeight();
		int lines = f4->InitPage((char*)it->content.c_str(), 293 - 10);
		totalHeight += lines * lineHeight + MSG_INTERVAL;
		//Separator
		totalHeight += MSG_INTERVAL;
		it++;
	}

	totalHeight -= 5;

	//Update mobile height
	m_mobileHeight = totalHeight - 186;
	if(m_mobileHeight < 0)
	{
		m_mobileHeight = 0;
		m_scrollBarHeight = 0;
	}
	else
	{
		//m_scrollBarHeight = (SCREEN_HEIGHT - 40) * (SCREEN_HEIGHT - 40) / totalHeight;
		////At least 6 pix
		//if(m_scrollBarHeight < 6)
		//{
		//	m_scrollBarHeight;
		//}
	}
}

const char *GLLiveStateChat::GetPosTalker(int posY)
{
	if(m_curRoomInfo->m_showAvatar && m_curRoom->GetParticipantCount() == 2)
	{
		const char *friendName = NULL;
		if(posY >= 64 && posY <= 120)
		{
			if(XP_API_STRICMP(m_curRoom->GetParticipant(0)->name, m_gl_user->getUserName()) != 0)
			{
				friendName = m_curRoom->GetParticipant(0)->name;
			}
			else
			{
				friendName = m_curRoom->GetParticipant(1)->name;
			}
		}
		return friendName;
	}

	CGLLiveFont *fName = m_handler->GetFont(11);
	CGLLiveFont *fContent = m_handler->GetFont(9);
	int friendPos = 70;

	posY += m_offset;

	IT_LIST_MSG it = m_curRoomInfo->m_msgList.begin();
	while(it != m_curRoomInfo->m_msgList.end() && friendPos - 10 < posY)
	{
		if(posY >= friendPos - 10 && posY <= friendPos - 10 + 20)
		{
			if( XP_API_STRICMP(it->from.c_str(), SYSTEM_MSG) == 0 || XP_API_STRICMP(it->from.c_str(), m_gl_user->getUserName()) == 0)
			{
				return NULL;
			}
			else
			{
				return it->from.c_str();
			}
		}
		//Message height
		int lineHeight = fContent->GetLineHeight();
		int lines = fContent->InitPage((char*)it->content.c_str(), 293 - 10);
		friendPos += lines * lineHeight + MSG_INTERVAL;
		//Separator
		friendPos += MSG_INTERVAL;
		it++;
	}
	return NULL;
}

void GLLiveStateChat::OnChatDisconnect(int e)
{
	char temp[64];
	XP_API_MEMSET(temp, 0, 64);
	XP_API_SPRINTF(temp, GetString(STR_CHAT_DISCONNECT), e);
	//m_curMsgList->push_back(MsgRecord(SYSTEM_MSG, temp, false));

	//Show Error msg
	//m_sub_state = CHAT_ERROR;
	//m_pMsgBox->SetTitle(GetString(STR_ERROR));
	//m_pMsgBox->SetText(temp);

	//Add to msg list
	MAP_CHAT_ROOM_INFO::iterator it = m_roomInfoList.begin();
	while(it != m_roomInfoList.end())
	{
		it->second.m_msgList.push_back(MsgRecord(SYSTEM_MSG, temp, 0, true));
		it++;
	}

	UpdateMobileHeight();
	m_offset = m_mobileHeight;

	GLLiveState::OnChatDisconnect(e);
}

void GLLiveStateChat::AddChatRoom(GLXPlayerChatRoom *newRoom)
{
	m_chatRoomList->Add(newRoom->GetJid(), newRoom);

	ChatRoomInfo newRoomInfo;
	m_roomInfoList[newRoom->GetJid()] = newRoomInfo;
	m_curRoomInfo = &m_roomInfoList[newRoom->GetJid()];

	//if the user is the creator
	//int usernameLen = XP_API_STRLEN(m_gl_user->getUserName());
	char username[MAX_USERNAME];
	GLXPlayerChat::ToLower(m_gl_user->getUserName(), username);

	//if( XP_API_STRNCMP(newRoom->GetJid() + 2, username, usernameLen) == 0 && 
	//	(newRoom->GetJid()[usernameLen + 2] == '@' || newRoom->GetJid()[1] == '-'))
	//{
	m_curRoomInfo->m_state = CHAT_PROCESS;
	m_sub_state = CHAT_PROCESS;
	//start Timer
	m_curRoomInfo->m_timer = XP_API_GET_TIME();

	//}
	//else
	//{
	//	m_curRoomInfo->m_state = CHAT_CHAT;
	//	m_sub_state = CHAT_CHAT;
	//}
	//Switch cur room to new room
	m_curRoom = newRoom;
	m_curRoomInfo = &m_roomInfoList[m_curRoom->GetJid()];
	UpdateMobileHeight();
	m_offset = 0;

	m_curRoomInfo->m_msgList.push_back(MsgRecord(SYSTEM_MSG, GetString(ATTENTION_MINORS), 0, true));

	//Show Tab bar
	m_bShowingTab = true;
	m_showedTab = 11;
}

void GLLiveStateChat::OnChatRoomConfigResult(const char *room, bool success)
{
	if(success)
	{
		m_roomInfoList[room].m_state = CHAT_CHAT;
		m_roomInfoList[room].m_timer = -1;            //Stop timer
		if(XP_API_STRCMP(room, m_curRoom->GetJid()) == 0)
		{
			m_sub_state = CHAT_CHAT;
		}
	}
	else
	{
		OnRequestTimeout(0);
	}
}

bool GLLiveStateChat::IsInRoom(const char *room_jid)
{
	return (m_chatRoomList->Find(room_jid) != NULL);
}

int GLLiveStateChat::RoomCount()
{
	return m_chatRoomList->Count();
}



/****************GLLiveStateChatInvitation*********************************/
GLLiveStateChatInvitation::GLLiveStateChatInvitation(GLLiveStateHandler * handler, GLLiveState *pLastState)
			: GLLiveState(handler, false)
{
	m_lastState = pLastState;

	//Init confirm menu
	_TouchRect rect;
	GLLiveMenuItem *item;
	m_menu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(107, 223, 63, 45);
	m_menu->AddMenuItem(this, IDC_MSG_NO, NULL, rect);
	item = m_menu->GetItem(0);
	item->SetPic(SPRITE_MENU, Frame_menu_BUTTON_CHAT_NO);
	item->SetSelPic(SPRITE_MENU, Frame_menu_BUTTON_CHAT_NO + 1);

	rect.left = 314;
	m_menu->AddMenuItem(this, IDC_MSG_YES, NULL, rect);
	item = m_menu->GetItem(1);
	item->SetPic(SPRITE_MENU, Frame_menu_BUTTON_CHAT_YES);
	item->SetSelPic(SPRITE_MENU, Frame_menu_BUTTON_CHAT_YES + 1);

	//Init msg box
	if(m_pMsgBox == NULL)
	{
		m_pMsgBox = XP_API_NEW CGLLiveMessageBox();
	}

	MEMSET(m_invStack, 0, sizeof(m_invStack));
	m_top = -1;

	m_isLogout = false;
	m_acceptFailed = false;

	m_fromAvatar = NULL;
	m_isGettingAvatar = false;
}

GLLiveStateChatInvitation::~GLLiveStateChatInvitation()
{
	XP_API_DEL(m_menu);
	for(int i = 0; i < MAX_INVITATION_STACK; i++)
	{
		XP_API_DEL(m_invStack[i]);
	}
	g_pGLLiveInstance->m_pInviteState = NULL;

	XP_API_DEL(m_fromAvatar);
}

void GLLiveStateChatInvitation::SwitchState()
{
	if(m_top == -1)
	{
		m_handler->PopGLLiveState();
	}
}

void GLLiveStateChatInvitation::Paint()
{
	m_lastState->Paint();

	CGLLiveGraphics *g = m_handler->GetGraphics();

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

	if(m_isLogout)
	{
		PaintLoading();
	}
	else if(m_acceptFailed)
	{
		m_pMsgBox->Repaint(m_handler);
	}
	else
	{
		CGLLiveFont *fTitle = m_handler->GetFont(FONT_WHITE_MID);
		CGLLiveFont *fFrom = m_handler->GetFont(FONT_RED_SMALL);
		CGLLiveFont *fInvitor = m_handler->GetFont(12);
		CGLLiveFont *fReason = m_handler->GetFont(FONT_RED_MID);
		char pMsg[128];

		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_INVITE, 0, 0, 0, 0, 0);
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_INVITE2, 0, 0, 0, 0, 0);

		//Avatar
		PaintAvatar(m_fromAvatar, 118, 93, 45, 45);

		//Title
		XP_API_MEMSET(pMsg, 0, 128);
		if(XP_API_STRNCMP(m_invStack[m_top]->m_room, GAME_INVITAON_STR, XP_API_STRLEN(GAME_INVITAON_STR)) == 0)
		{
			XP_API_SPRINTF(pMsg, "%s%d", GetString(STR_GAME_INVITE_TITLE),m_top + 1);
		}
		else
		{
			XP_API_SPRINTF(pMsg, "%s%d", GetString(STR_CHAT_INVITE_TITLE),m_top + 1);
		}
		fTitle->DrawString(g, pMsg, 280, 96, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);

		//From:
		fFrom->DrawString(g, GetString(STR_MSG_FROM), 170, 139, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE);

		//Invitor
		fInvitor->DrawString(g, m_invStack[m_top]->m_invitor, 204, 139, CGLLiveGraphics::LEFT | CGLLiveGraphics::BASELINE, 165);

		//Reason
		fReason->InitPage(m_invStack[m_top]->m_reason, 254);
		fReason->DrawPage(g, m_invStack[m_top]->m_reason, 240, 177, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);

		m_menu->Repaint(m_handler);
	}
}

bool GLLiveStateChatInvitation::HandleEvent(_TOUCH_EVENT event)
{
	//Is showing warning
	if(m_acceptFailed)
	{
		if(event.eType == TET_Pressed)
		{
			PopInvitation();
			if(m_top < 0)
			{
				m_handler->PopGLLiveState();
			}
			m_acceptFailed = false;
		}
		return true;
	}
	else if(!m_isLogout)
	{
		return m_menu->HandleEvent(event);
	}
	else
	{
		return true;
	}
}

void GLLiveStateChatInvitation::UpdateAnimation()
{
	if(m_fromAvatar == NULL && !m_isGettingAvatar && m_top >= 0)
	{
		m_gl_user->sendGetUserPhoto(m_invStack[m_top]->m_invitor);
		m_isGettingAvatar = true;
	}
	GLLiveState::UpdateAnimation();
	m_menu->Update();
}

void GLLiveStateChatInvitation::OnMenuItemClicked(int id)
{
	if(id == IDC_MSG_YES)
	{
		XP_DEBUG_OUT("[GLLiveStateChatInvitation] Accept invitation!");
		/***************************************************Game Invation***********************************************************/
		if(XP_API_STRNCMP(m_invStack[m_top]->m_room, GAME_INVITAON_STR, XP_API_STRLEN(GAME_INVITAON_STR)) == 0)
		{
			//Ensure the logout handler is this
			CancelRequest();
			RegisterOnlineObject();

			m_gl_login->SendLogout();
			m_isLogout = true;
			return;
		}

		/***************************************************Chat Invation***********************************************************/
		GLLiveStateChat *pChatState = (GLLiveStateChat*)(g_pGLLiveInstance->GetChatState());
		if(pChatState != NULL)
		{
			//Is full
			if(pChatState->RoomCount() >= 8)
			{
				XP_DEBUG_OUT("[GLLiveStateChatInvitation] Chat room is full!");
				m_gl_chat->RejectRoomInvitation(m_invStack[m_top]->m_room, m_invStack[m_top]->m_invitor, m_gl_user->getUserName());
				m_acceptFailed = true;
				//show warning
				m_pMsgBox->SetTitle(GetString(STR_WARNING));
				m_pMsgBox->SetText(GetString(STR_CHAT_FULL));
				return;
			}
			//Is in
			else if( pChatState->IsInRoom( m_invStack[m_top]->m_room ))
			{
				XP_DEBUG_OUT("[GLLiveStateChatInvitation] Chat room is exist!");
			}
			else
			{
				GLXPlayerChatRoom *room = m_gl_chat->AcceptRoomInvitation(m_invStack[m_top]->m_room, m_gl_user->getUserName());
				pChatState->AddChatRoom(room);
			}
		}
		else
		{
			GLXPlayerChatRoom *room = m_gl_chat->AcceptRoomInvitation(m_invStack[m_top]->m_room, m_gl_user->getUserName());

			PopInvitation();
			//There are at least 1 invitation
			if(m_top > -1)
			{
				//Copy current state and Clear all invitation - Then it will auto pop itself when switch to it.
				GLLiveStateChatInvitation *copyState = XP_API_NEW GLLiveStateChatInvitation(m_handler, m_lastState);
				for(int i = 0; i <= m_top; i++)
				{
					if(m_invStack[i] != NULL)
					{
						copyState->PushInvitation(m_invStack[i]->m_room, m_invStack[i]->m_invitor, m_invStack[i]->m_reason, m_invStack[i]->m_invitorJid);
						XP_API_DEL(m_invStack[i]);
					}
				}
				m_top = -1;
				
				//Push a chat state
				m_handler->PushGLLiveState(XP_API_NEW GLLiveStateChat(m_handler, room));
				//Push the copy state
				m_handler->PushGLLiveState(copyState);
				//copyState->CancelRequest();
				copyState->RegisterOnlineObject();
				return;
			}
			//There is no invitation
			else
			{
				//Push a chat state
				m_handler->PushGLLiveState(XP_API_NEW GLLiveStateChat(m_handler, room));
				g_pGLLiveInstance->m_pInviteState = NULL;
				//m_handler->PopGLLiveState();
				return;
			}
		}
	}
	else if(id == IDC_MSG_NO)
	{
		XP_DEBUG_OUT("[GLLiveStateChatInvitation] Refuse invitation!");
		m_gl_chat->RejectRoomInvitation(m_invStack[m_top]->m_room, m_invStack[m_top]->m_invitorJid, m_gl_user->getUserName());
		//m_handler->PopGLLiveState();
		//PopInvitation();
	}

	PopInvitation();
	if(m_top == -1)
	{
		m_handler->PopGLLiveState();
	}
}

bool GLLiveStateChatInvitation::PushInvitation(const char *room, const char *invitor, const char *reason, const char *invitorJid)
{
	if(m_top + 1 > MAX_INVITATION_STACK - 1)
	{
		return false;
	}

	m_top++;
	m_invStack[m_top] = XP_API_NEW InvitorInfo(room, invitor, reason, invitorJid);

	char *pMsg = XP_API_NEW char[128];;
	XP_API_MEMSET(pMsg, 0, 128);
	XP_API_SPRINTF(pMsg, "%s%s\n%s", GetString(STR_CHAT_INVITE_FROM),invitor, reason);
	m_pMsgBox->SetText(pMsg);

	XP_API_MEMSET(pMsg, 0, 128);
	if(XP_API_STRNCMP(room, GAME_INVITAON_STR, XP_API_STRLEN(GAME_INVITAON_STR)) == 0)
	{
		XP_API_SPRINTF(pMsg, "%s(%d)", GetString(STR_GAME_INVITE_TITLE),m_top + 1);
	}
	else
	{
		XP_API_SPRINTF(pMsg, "%s(%d)", GetString(STR_CHAT_INVITE_TITLE),m_top + 1);
	}
	m_pMsgBox->SetTitle(pMsg);

	XP_API_DELETE_ARRAY(pMsg);
	return true;
}

void GLLiveStateChatInvitation::PopInvitation()
{
	if(m_top >= 0)
	{
		XP_API_DEL(m_invStack[m_top]);
		m_top--;
	}

	//Reset the title
	if(m_top >= 0)
	{
		char *pMsg = XP_API_NEW char[128];;
		XP_API_MEMSET(pMsg, 0, 128);
		if(XP_API_STRNCMP(m_invStack[m_top]->m_room, GAME_INVITAON_STR, XP_API_STRLEN(GAME_INVITAON_STR)) == 0)
		{
			XP_API_SPRINTF(pMsg, "%s(%d)", GetString(STR_GAME_INVITE_TITLE),m_top + 1);
		}
		else
		{
			XP_API_SPRINTF(pMsg, "%s(%d)", GetString(STR_CHAT_INVITE_TITLE),m_top + 1);
		}
		m_pMsgBox->SetTitle(pMsg);

		XP_API_SPRINTF(pMsg, "%s%s\n%s", GetString(STR_CHAT_INVITE_FROM),m_invStack[m_top]->m_invitor, m_invStack[m_top]->m_reason);
		m_pMsgBox->SetText(pMsg);

		//Clear avatar
		XP_API_DEL(m_fromAvatar);
	}
}

void GLLiveStateChatInvitation::OnRequestSuccess(int func_id, char *data, int data_len)
{
	if(func_id == FUNCTION_LOGOUT)
	{
		XP_DEBUG_OUT( "[GLLiveStateChatInvitation] Login out OK!\n" );
#ifdef WIN32
		//PopInvitation();
		if(m_top == -1)
		{
			m_handler->PopGLLiveState();
		}
#else
		extern void autoStartGame(int suspend, const char* name = "");
		char invitorName[MAX_USERNAME];
		XP_API_MEMSET(invitorName, 0, sizeof(invitorName));
		char *totalName = m_invStack[m_top]->m_invitor;
		for(int i = 0; totalName[i] != '\0' && totalName[i] != '@'; i++)
		{
			invitorName[i] = totalName[i];
		}
		autoStartGame(2, invitorName);
		m_handler->TerminateGame();
#endif
	}
	else if(func_id == FUNCTION_GET_USER_AVATAR)
	{
		XP_DEBUG_OUT("[GLLiveStateChatInvitation] Get avatar id OK!\n");
		m_fromAvatar = LoadPicFromLocal(m_gl_user->getOtherUserAvatar());
		if(m_fromAvatar == NULL)
		{
			m_gl_user->sendDownloadAvatar(m_gl_user->getOtherUserAvatar());
		}
		else
		{
			m_isGettingAvatar = false;
		}
	}
	else if(func_id == FUNCTION_DOWNLOAD_USER_AVATAR)
	{
		XP_DEBUG_OUT("[GLLiveStateChatInvitation] Get friend avatar OK!\n");
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			//nothing to do
			//SavePicToLocal(m_gl_user->getOtherUserAvatar(), NULL, 0);
			m_fromAvatar = XP_API_NEW CGLImage();
		}
		else
		{
			char *	base64_buf = NULL;
			int		base64_size;
			base64_decode(data + 1, data_len - 1, base64_buf, base64_size);
			m_fromAvatar = NEW CGLImage(base64_buf, base64_size);

			SavePicToLocal(m_gl_user->getOtherUserAvatar(), base64_buf, base64_size);

			XP_API_DEL(base64_buf);
		}
		m_isGettingAvatar = false;
	}
}

void GLLiveStateChatInvitation::OnRequestFailure(int func_id, int error_code)
{
	if(func_id == FUNCTION_LOGOUT || m_isLogout)
	{
		XP_DEBUG_OUT( "[GLLiveStateChatInvitation] Login out Failed!\n" );
#ifdef WIN32
		//PopInvitation();
		if(m_top == -1)
		{
			m_handler->PopGLLiveState();
		}
#else
		extern void autoStartGame(int suspend, const char* name = "");
		char invitorName[MAX_USERNAME];
		XP_API_MEMSET(invitorName, 0, sizeof(invitorName));
		char *totalName = m_invStack[m_top]->m_invitor;
		for(int i = 0; totalName[i] != '\0' && totalName[i] != '@'; i++)
		{
			invitorName[i] = totalName[i];
		}
		autoStartGame(2, invitorName);
		m_handler->TerminateGame();
#endif
	}
	else if(func_id == FUNCTION_GET_USER_AVATAR || func_id == FUNCTION_DOWNLOAD_USER_AVATAR)
	{
		m_fromAvatar = XP_API_NEW CGLImage();
		m_isGettingAvatar = false;
	}
	else
	{
		GLLiveState::OnRequestFailure(func_id, error_code);
	}
}

void GLLiveStateChatInvitation::SetLastState(GLLiveState *pLastState)
{
	m_lastState = pLastState;
}