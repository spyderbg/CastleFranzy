#include "GLLiveStateChat.h"
#include "GLLiveStateEdit.h"
#include "GLLiveStateFriends.h"
#include "GLLiveMenu.h"
#include "GLLiveGame.h"

#define COLOR_FLAG               "\\"
#define SYSTEM_MSG               "Gameloft"
#define AVATAR_OFF_X			2
#define MAX_CHAT_MEMBER         20
#define MAX_CHAT_ROOM_TIMEOUT   30000

#define CHAT_LOADING

GLLiveStateChat::GLLiveStateChat( GLLiveStateHandler * handler, GLXPlayerChatRoom *room) : GLLiveState(handler)
{
	XP_API_MEMSET(m_inputMsg, 0, sizeof(m_inputMsg));

	m_menu = XP_API_NEW GLLiveMenu(0, 0);
	_TouchRect rect;
	rect.Set(405, 285, 72, 32);
	m_menu->AddMenuItem(this, IDC_CHAT_SEND, GetString(STR_SEND), rect);
	m_menu->GetItem(0)->SetPic(SPRITE_TEXTBOX, Frame_textbox_BUTTON_SEND, 405, 285);
	m_menu->GetItem(0)->SetSelPic(SPRITE_TEXTBOX, Frame_textbox_BG_SEND, 405 - 8, 285 - 8);

	rect.Set(5, 197, 69, 28);
	m_menu->AddMenuItem(this, IDC_CHAT_QUIT_ROOM, NULL, rect);
	m_menu->GetItem(1)->SetPic(1, Frame_menu_CHAT_EXIT, 5, 197);
	m_menu->GetItem(1)->SetSelPic(1, Frame_menu_BG_CHAT_BUTTON, 5, 197 );

	rect.Set(5, 285, 39, 28);
	m_menu->AddMenuItem(this, IDC_CHAT_COLOR, NULL, rect);
	m_menu->GetItem(2)->SetPic(SPRITE_TEXTBOX, Frame_textbox_BUTTON_COLOR, 5, 285);
	m_menu->GetItem(2)->SetSelPic(SPRITE_TEXTBOX, Frame_textbox_BG_BUTTON, 5 - 8, 285 - 8);

	rect.Set(5, 150, 69, 28);
	m_menu->AddMenuItem(this, IDC_CHAT_ADD, NULL, rect);
	m_menu->GetItem(3)->SetPic(1, Frame_menu_CHAT_ADD, 5, 150);
	m_menu->GetItem(3)->SetSelPic(1, Frame_menu_BG_CHAT_BUTTON, 5, 150);

	rect.Set(5, 242, 69, 28);
	m_menu->AddMenuItem(this, IDC_CHAT_BACK, NULL, rect);
	m_menu->GetItem(4)->SetPic(1, Frame_menu_CHAT_EXIT_ROOM, 5, 242);
	m_menu->GetItem(4)->SetSelPic(1, Frame_menu_BG_CHAT_BUTTON, 5, 242);

	m_setMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(202, 198, 79, 32);
	m_setMenu->AddMenuItem(this, IDC_MSG_OK, GetString(STR_GO), rect);
	m_setMenu->GetItem(0)->SetPic(1, Frame_menu_BUTTON_MSG_OK);
	m_setMenu->GetItem(0)->SetSelPic(1, Frame_menu_BG_MSG_OK);
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
	m_confirmMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(136, 181 + 9, 79, 32 );
	m_confirmMenu->AddMenuItem(this, IDC_MSG_NO, GetString(STR_NO), rect);
	m_confirmMenu->GetItem(0)->SetPic(1, Frame_menu_BUTTON_YES, 0, 9);
	m_confirmMenu->GetItem(0)->SetSelPic(1, Frame_menu_BG_YES, 0, 9);

	rect.Set(263, 181 + 9, 79, 32 );
	m_confirmMenu->AddMenuItem(this, IDC_MSG_YES, GetString(STR_YES), rect);
	m_confirmMenu->GetItem(1)->SetPic(1, Frame_menu_BUTTON_NO, 0, 9);
	m_confirmMenu->GetItem(1)->SetSelPic(1, Frame_menu_BG_NO, 0, 9);

	XP_API_MEMSET(m_addTalkFriend, 0, sizeof(m_addTalkFriend));
	m_friendUpdated = false;
	m_friendUpdating = false;
	m_isPressingInputArea = false;
	m_isPressingTab = false;
	m_isInviteFriend = false;

	m_gettingAvatarRoom = NULL;
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
		int row = (event.position.y - 100) / 53;
		int col = (event.position.x - (68 + 23)) / (68 + 10);

		int select = row * 4 + col;
		if(select >= 0 && select < 8)
		{
			m_tempMsgColor = select;
			return true;
		}
	}

	if(m_setMenu->HandleEvent(event))
	{
		return true;
	}
	return false;
}

bool GLLiveStateChat::HandleEventChat(_TOUCH_EVENT event)
{
	if(m_menu->HandleEvent(event))
	{
		return true;
	}

	switch(event.eType)
	{
	case TET_Pressed:
		if(!m_bShowingTab)
		{
			//Begin increasement pressed frames
			m_pressFrames = 0;
		}
		else
		{
			//Stop increasement
			m_pressFrames = -1;
			m_bShowingTab = false;
		}
		//Click the input area
		if(event.position.y > SCREEN_HEIGHT - 40)
		{
			m_pressFrames = -1;
			m_isPressingInputArea = true;
		}
		//Click the tab bar when it is shown
		else if(/*m_showedTab == 11 && */event.position.y <= 22)
		{
			m_isPressingTab = true;
			m_bShowingTab = true;
		}
		//Click the friend area to add friend
		else if( event.position.x > 400 && event.position.y > 22 && event.position.y < 280 )//&& (!m_curRoomInfo->m_showAvatar))
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
		//Handle scroll data
		if(m_moving)
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
		if(m_isPressingInputArea)
		{
			if(event.position.y < SCREEN_HEIGHT - 40)
			{
				m_isPressingInputArea = false;
			}
		}
		else if(m_isPressingTab)
		{
			if( !(/*m_showedTab == 11 && */event.position.y <= 22) )
			{
				m_isPressingTab = false;
			}
		}
		break;
	case TET_Released:
		if(m_moving)
		{
			m_moving = false;
		}
		if(m_isPressingInputArea)
		{
			if(event.position.y > SCREEN_HEIGHT - 40)
			{
#ifdef WIN32
				m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, "Message", m_inputMsg,false, 256));
#else
				m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, m_inputMsg, this));
#endif
			}
			m_isPressingInputArea = false;
		}
		else if(m_isPressingTab)
		{
			if( /*m_showedTab == 11 && */event.position.y <= 22 )
			{
				int index = event.position.x / 60;
				//Exist Chat room
				if(index < m_chatRoomList->Count())
				{
					m_curRoom = m_chatRoomList->Get(index);
					m_curRoomInfo = &m_roomInfoList[m_curRoom->GetJid()];
					UpdateMobileHeight();
					m_offset = m_mobileHeight;
				}
				//New Chat
				else
				{
					if(m_chatRoomList->Count() < 8)
					{
						char roomName[64];
						XP_API_MEMSET(roomName, 0, sizeof(roomName));

						//Find a new chat room name that is not in cur room list
						int i = 2;
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

void GLLiveStateChat::PaintChat()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     * f4 = m_handler->GetFont(3);

	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 1, 0, 0, 0, 0, 0);

	//Draw message record
	int posY = 5 + 22;
	//int posX = 0;

	LIST_MSG *m_curMsgList = &m_curRoomInfo->m_msgList;

	IT_LIST_MSG it = m_curMsgList->begin();

	g->SetClip(0, -50, SCREEN_WIDTH, SCREEN_HEIGHT + 50);

	//Draw avatars if the participents is less than 3
	extern CGLImage * g_pAvatarImage;
	//Draw myself
	PaintAvatar(g_pAvatarImage, AVATAR_OFF_X, posY);

	f4->DrawString(g, m_gl_user->getUserName(), AVATAR_OFF_X + 79 / 2, posY + 74 + 5 + 26 / 2, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER, 79);

	//Draw friend's avatar
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
			int offx_right = SCREEN_WIDTH - AVATAR_OFF_X - 74;
			PaintAvatar(m_curRoomInfo->m_friendAvatar, offx_right, posY);
			f4->DrawString(g, friendName, offx_right + 79 / 2, posY + 74 + 5 + 26 / 2, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER, 79);
		}

		//Paint + if is not friend
		if(!m_curRoomInfo->m_isFriend)
		{
			m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_ADD_FRIEND, 400 - 2, 0 - 2, 0, 0, 0);
		}
	}

	posY = 5 + 22 - m_offset;
	while(it != m_curMsgList->end())
	{
		float textHeight = 0;
		float textWidth = 0;

		//Beyond the visible range, stop paint
		if(posY > SCREEN_HEIGHT - 40)
		{
			break;
		}

		//Generate text height and width
		int lines = f4->InitPage((char*)it->content.c_str(), 270 - 10);
		textHeight =  lines * f4->GetLineHeight() + 10;
		//Less than ONE line
		if(lines <= 1)
		{
			f4->UpdateStringSize((char*)it->content.c_str());
			textWidth = f4->GetStringSize() + 10;
		}
		else
		{
			textWidth = 276;
		}

		//Paint visible msg
		if(posY + textHeight >= 0)
		{
			//System msg
			if(it->from == SYSTEM_MSG)
			{
				f4->DrawPage(g, (char*)it->content.c_str(), 80 + 13 + 5, posY + textHeight / 2, CGLLiveGraphics::VCENTER | CGLLiveGraphics::LEFT);
			}
			//Left
			else if(XP_API_STRICMP(it->from.c_str(),  m_gl_user->getUserName()) == 0)
			{
				//Paint Message
				m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_MSG_POINT0 + it->color, 80 + 13, posY, FLAG_FLIP_X);

				g->BeginScale( textWidth / 276, textHeight / 104, 80 + 12, posY);
				m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_MSG_BG0 + it->color, 80 + 12, posY, 0);
				g->EndScale();

				f4->DrawPage(g, (char*)it->content.c_str(), 80 + 13 + 5, posY + textHeight / 2, CGLLiveGraphics::VCENTER | CGLLiveGraphics::LEFT);

			}
			//Right
			else
			{
				//Paint Talker if participents is more than 2
				if(!m_curRoomInfo->m_showAvatar)
				{
					m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TALKER, 400, posY, 0, 0, 0);
					f4->DrawString(g, (char*)it->from.c_str(), 400 + 79 / 2, posY + 26 / 2, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER, 79);
					if(!it->isFriend)
					{
						m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_ADD_FRIEND, 400 - 2, posY - 10, 0, 0, 0);
					}
				}

				//Paint Message
				m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_MSG_POINT0 + it->color, 400 - 13, posY, 0);

				g->BeginScale( textWidth / 276, textHeight / 104, 400 - 12, posY);
				m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_MSG_BG0 + it->color, 400 - 12 - 276, posY, 0);
				g->EndScale();

				f4->DrawPage(g, (char*)it->content.c_str(), 400 - 12 + 5 - textWidth, posY + textHeight / 2, CGLLiveGraphics::VCENTER | CGLLiveGraphics::LEFT);
			}
		}
	
		posY += textHeight + 10;
		it++;
	}

	//Paint scroll bar
	if(m_moving && m_mobileHeight > 0)
	{
		int scrollPosY = m_offset * (SCREEN_HEIGHT - 40 - m_scrollBarHeight ) / m_mobileHeight;
		m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_SCROLL_UP, 
			                     400,
								 scrollPosY,
								 0);

		scrollPosY += 3;

		g->SetClip(400, scrollPosY, 10, (m_scrollBarHeight - 6));
		int height = 0;
		while(height < m_scrollBarHeight - 6)
		{
			m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_SCROLL_MID, 
			                     400,
								 scrollPosY + height,
								 0);

			height += 25;
		}
		g->ResetClip();

		scrollPosY += m_scrollBarHeight - 6;

		m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_SCROLL_DOWN, 
			                     400,
								 scrollPosY,
								 0);
	}

	g->ResetClip();

	//Draw Tab Bar
	if(true)//m_showedTab > 0)
	{
		CGLLiveFont *f14 = m_handler->GetFont(13);
		int roomCount = m_chatRoomList->Count();
		int frameId;
		for(int j = 0; j < roomCount; j++)
		{
			if(m_curRoom == m_chatRoomList->Get(j))
			{
				frameId = Frame_textbox_TAB_SELECT;
			}
			else
			{
				frameId = Frame_textbox_TAB_COMMON;
			}
			m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, frameId, j * 60, 0/*m_showedTab * 2 - 22*/, 0);
			f14->DrawString(g, m_chatRoomList->Get(j)->GetName(), j * 60 + 30, 11/*m_showedTab * 2 - 22 + 11*/, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER, 55);
		}

		//New chat
		if(m_chatRoomList->Count() < 8)
		{
			m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TAB_COMMON + 8 - roomCount, roomCount * 60, 0/*m_showedTab * 2 - 22*/, 0);
			f14->DrawString(g, GetString(STR_CHAT_NEWCHAT), roomCount * 60 + (SCREEN_WIDTH - roomCount * 60) / 2, 11/*m_showedTab * 2 - 22 + 11*/, CGLLiveGraphics::VCENTER | CGLLiveGraphics::HCENTER, (SCREEN_WIDTH - roomCount * 60));
		}
	}

	//Paint Input box
	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 2, 0, 320 - 40, 0, 0, 0);

	//f4->InitPage(m_inputMsg, 480 - 170);
	//f4->DrawPage(g, m_inputMsg, 95, 285, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);

	if((m_frameCount / 15) % 2 == 0)
	{
		f4->DrawString(g, "|", 50, 285, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	}

	m_menu->Repaint(m_handler);
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

	PaintChat();

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

	m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_BG_EVALUATION, 0, 0, 0);
	int sepWidth = 10;
	for(int i = 0; i < 8; i++)
	{
		if(i == m_tempMsgColor)
		{
			m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_MSG_COLOR_FRAME0 + i, 
				68 + 23 - 10 + (i % 4) * (sepWidth + 68),
				97 - 10 + i / 4 * 53,
				0,
				0);
		}
		m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_MSG_COLOR0 + i, 
			68 + 23 + (i % 4) * (sepWidth + 68),
			97 + i / 4 * 53,
			0,
			0);
	}

	m_setMenu->Repaint(m_handler);
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
	
	//Update for tab bar data
	if(m_pressFrames >= 0)
	{
		m_pressFrames++;
		if( m_pressFrames == 20 )
		{
			m_bShowingTab = true;
			m_showedTab = 0;
		}
	}

	if(m_bShowingTab)
	{
			if(m_showedTab < 11)
			{
				m_showedTab++;
			}
	}
	else
	{
		if(m_showedTab > 0)
		{
			m_showedTab--;
		}
	}


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
						m_curRoom->Invite(lname, GetString(STR_CHAT_INVITE_MSG, getLangIndexFromServer(m_gl_userFriend->GetCurrentLanguage(i))));
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
		m_sub_state = CHAT_CHAT;
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
	if(message[0] == COLOR_FLAG[0])
	{
		index = 0;
		while(message[index + 1] != COLOR_FLAG[0])
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
		pTempRoom->m_msgList.push_back(MsgRecord(GetString(STR_CHAT_ADMIN), temp, 0, true));
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
		pTempRoom->m_msgList.push_back(MsgRecord(GetString(STR_CHAT_ADMIN), temp2, 0, true));
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
	int a = 0;
	a++;
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
			XP_API_SPRINTF(pTemp, "%s%d%s%s", COLOR_FLAG, m_msgColor, COLOR_FLAG,m_inputMsg); 
			bool result = m_curRoom->SendChatMessage(pTemp);
			XP_API_MEMSET(m_inputMsg, 0, sizeof(m_inputMsg));
			XP_API_DELETE_ARRAY(pTemp);
		}
		break;
	case IDC_CHAT_BACK:
		m_pMsgBox->SetTitle(GetString(STR_WARNING));
		m_pMsgBox->SetText(GetString(STR_BACK_MAIN));
		m_sub_state = CHAT_EXIT_CONFIRM;
		//Show Tab bar
		m_bShowingTab = true;
		m_showedTab = 11;
		return;
	case IDC_CHAT_COLOR:
		m_sub_state = CHAT_SETCOLOR;
		m_tempMsgColor = m_msgColor;
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
	}
}

void GLLiveStateChat::UpdateMobileHeight()
{
	CGLLiveFont *f4 = m_handler->GetFont(3);
	int totalHeight = 5 + 22;

	IT_LIST_MSG it = m_curRoomInfo->m_msgList.begin();
	while(it != m_curRoomInfo->m_msgList.end())
	{
		//Message height
		int lineHeight = f4->GetLineHeight();
		int lines = f4->InitPage((char*)it->content.c_str(), 270 - 10);
		totalHeight += lines * lineHeight + 10;
		//Separator
		totalHeight += 10;
		it++;
	}

	totalHeight -= 5;

	//Update mobile height
	m_mobileHeight = totalHeight - (SCREEN_HEIGHT - 40);
	if(m_mobileHeight < 0)
	{
		m_mobileHeight = 0;
		m_scrollBarHeight = 0;
	}
	else
	{
		m_scrollBarHeight = (SCREEN_HEIGHT - 40) * (SCREEN_HEIGHT - 40) / totalHeight;
		//At least 6 pix
		if(m_scrollBarHeight < 6)
		{
			m_scrollBarHeight;
		}
	}
}

const char *GLLiveStateChat::GetPosTalker(int posY)
{
	if(m_curRoomInfo->m_showAvatar && m_curRoom->GetParticipantCount() == 2)
	{
		const char *friendName = NULL;
		if(posY >= 22 && posY <= 74)
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

	CGLLiveFont *f4 = m_handler->GetFont(3);
	int friendPos = 5 + 22;

	posY += m_offset;

	IT_LIST_MSG it = m_curRoomInfo->m_msgList.begin();
	while(it != m_curRoomInfo->m_msgList.end() && friendPos < posY)
	{
		if(friendPos + 26 >= posY)
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
		int lineHeight = f4->GetLineHeight();
		int lines = f4->InitPage((char*)it->content.c_str(), 270 - 10);
		friendPos += lines * lineHeight + 10;
		//Separator
		friendPos += 10;
		it++;
	}
	return NULL;
}

void GLLiveStateChat::OnChatDisconnect(int e)
{
	char temp[64];
	XP_API_MEMSET(temp, 0, 64);
	XP_API_SPRINTF(temp, GetString(STR_CHAT_DISCONNECT), e);
	//m_curMsgList->push_back(MsgRecord(GetString(STR_CHAT_ADMIN), temp, 0));

	//Show Error msg
	//m_sub_state = CHAT_ERROR;
	//m_pMsgBox->SetTitle(GetString(STR_ERROR));
	//m_pMsgBox->SetText(temp);

	//Add to msg list
	MAP_CHAT_ROOM_INFO::iterator it = m_roomInfoList.begin();
	while(it != m_roomInfoList.end())
	{
		it->second.m_msgList.push_back(MsgRecord(GetString(STR_CHAT_ADMIN), temp, 0, true));
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
	rect.Set(136, 181 + 9, 79, 32);
	m_menu->AddMenuItem(this, IDC_MSG_NO, GetString(STR_NO), rect);
	item = m_menu->GetItem(0);
	item->SetPic(1, Frame_menu_BUTTON_YES, 0, 9);
	item->SetSelPic(1, Frame_menu_BG_YES, 0, 9);

	rect.Set(263, 181 + 9, 79, 32);
	m_menu->AddMenuItem(this, IDC_MSG_YES, GetString(STR_YES), rect);
	item = m_menu->GetItem(1);
	item->SetPic(1, Frame_menu_BUTTON_NO, 0, 9);
	item->SetSelPic(1, Frame_menu_BG_NO, 0, 9);

	//Init msg box
	if(m_pMsgBox == NULL)
	{
		m_pMsgBox = XP_API_NEW CGLLiveMessageBox();
	}

	MEMSET(m_invStack, 0, sizeof(m_invStack));
	m_top = -1;

	m_isLogout = false;
	m_acceptFailed = false;
}

GLLiveStateChatInvitation::~GLLiveStateChatInvitation()
{
	XP_API_DEL(m_menu);
	for(int i = 0; i < MAX_INVITATION_STACK; i++)
	{
		XP_API_DEL(m_invStack[i]);
	}
	g_pGLLiveInstance->m_pInviteState = NULL;
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
	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);
	g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	if(m_isLogout)
	{
		PaintLoading();
	}
	else
	{
		m_pMsgBox->Repaint(m_handler);
		if(!m_acceptFailed)
		{
			m_menu->Repaint(m_handler);
		}
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
						copyState->PushInvitation(m_invStack[i]->m_room, m_invStack[i]->m_invitor, m_invStack[i]->m_reason);
						XP_API_DEL(m_invStack[i]);
					}
				}
				m_top = -1;
				
				//Push a chat state
				m_handler->PushGLLiveState(XP_API_NEW GLLiveStateChat(m_handler, room));
				//Push the copy state
				m_handler->PushGLLiveState(copyState);
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
		m_gl_chat->RejectRoomInvitation(m_invStack[m_top]->m_room, m_invStack[m_top]->m_invitor, m_gl_user->getUserName());
		//m_handler->PopGLLiveState();
		//PopInvitation();
	}

	PopInvitation();
	if(m_top == -1)
	{
		m_handler->PopGLLiveState();
	}
}

bool GLLiveStateChatInvitation::PushInvitation(const char *room, const char *invitor, const char *reason)
{
	if(m_top + 1 > MAX_INVITATION_STACK - 1)
	{
		return false;
	}

	m_top++;
	m_invStack[m_top] = XP_API_NEW InvitorInfo(room, invitor, reason);

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
}

void GLLiveStateChatInvitation::OnRequestFailure(int func_id, int error_code)
{
	XP_DEBUG_OUT( "[GLLiveStateChatInvitation] Login out Failed!\n" );
	if(func_id == FUNCTION_LOGOUT || m_isLogout)
	{
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
	else
	{
		GLLiveState::OnRequestFailure(func_id, error_code);
	}
}

void GLLiveStateChatInvitation::SetLastState(GLLiveState *pLastState)
{
	m_lastState = pLastState;
}