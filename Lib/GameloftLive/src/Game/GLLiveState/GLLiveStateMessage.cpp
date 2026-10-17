#include "GLLiveStateMessage.h"
#include "GLLiveStateEdit.h"
#include "GLLiveStateFriends.h"
#include "MyGraphics.h"


#define WRITE_BOX_MAX_LINES 7
#define WRITE_BOX_MAX_HEIGHT 162

#define READ_BOX_MAX_LINES 5
#define READ_BOX_MAX_HEIGHT 115

#define READ_BOX_OFF_X	90
#define READ_BOX_OFF_Y	108
#define READ_BOX_WIDTH	300

extern void changeIconBadgeNumber(int msgNumber);

GLLiveStateMessage::GLLiveStateMessage(GLLiveStateHandler * handler, char * buddy_name) : GLLiveState( handler )
{
	m_buddy_name = buddy_name;
	if(!m_pMsgBox)
	{
		m_pMsgBox = XP_API_NEW CGLLiveMessageBox();
	}
	if(m_buddy_name)
	{
		XP_API_STRCPY(m_receiver, m_buddy_name);
		XP_API_STRCPY(m_title, "");
		XP_API_STRCPY(m_body, "");
		EnableGo(true);
		m_sub_state = MESSAGE_WRITE;
	}
	else
	{
		EnableGo(false);
		m_sub_state = MESSAGE_BOX;
	}
	m_last_state = m_sub_state;
	if(m_gl_message->getMsgListCount() > 0)
	{
		m_selected = 0;
	}
	else
	{
		m_selected = -1;
	}

	m_commandBar->GetItem(1)->SetTitle(GetString(STR_SEND));
	
	m_lineHeight = 47;
	m_startY     = 58;
	m_height     = m_lineHeight * 4;
	m_scrollBarPos = 0;

	m_nLines	 = 0;
	m_offset     = 0;
	m_oldOffset  = 0;
	m_touchPos   = 0;
	m_moving     = false;
	m_moved      = false;
	m_speed      = 0;
	m_moved      = false;

	_TouchRect rect;
	//Init menu for message list
	m_listMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(114, 250, 50, 48);
	m_listMenu->AddMenuItem(this, IDC_MSG_NEW, NULL, rect);
	m_listMenu->GetItem(0)->SetPic(1, Frame_menu_NEW_MSG_EN  + g_pGLLiveInstance->getLang());
	m_listMenu->GetItem(0)->SetSelPic(1, Frame_menu_BG_MSG_BOX1);

	rect.Set(233, 250, 50, 48);
	m_listMenu->AddMenuItem(this, IDC_MSG_DEL, NULL, rect);
	m_listMenu->GetItem(1)->SetPic(1, Frame_menu_DEL_MSG_EN  + g_pGLLiveInstance->getLang());
	m_listMenu->GetItem(1)->SetSelPic(1, Frame_menu_BG_MSG_BOX2);

	//rect.Set(271, 250, 50, 48);
	//m_listMenu->AddMenuItem(this, IDC_MSG_READ, NULL, rect);
	//m_listMenu->GetItem(2)->SetPic(1, Frame_menu_READ_MSG);
	//m_listMenu->GetItem(2)->SetSelPic(1, Frame_menu_BG_MSG_BOX3);

	rect.Set(349, 250, 50, 48);
	m_listMenu->AddMenuItem(this, IDC_MSG_SEND, NULL, rect);
	m_listMenu->GetItem(2)->SetPic(1, Frame_menu_SENT_MSG_EN  + g_pGLLiveInstance->getLang());
	m_listMenu->GetItem(2)->SetSelPic(1, Frame_menu_BG_MSG_BOX4);

	rect.Set(380, 22, 60, 35);
	m_listMenu->AddMenuItem(this, IDC_MSG_REFRESH, NULL, rect);
	m_listMenu->GetItem(3)->SetPic(1, Frame_menu_REFRESH, 0, 2);
	m_listMenu->GetItem(3)->SetSelPic(1, Frame_menu_BG_REFRESH, 0, 2);

	//Init menu for sent message list
	m_sendBoxMenu = XP_API_NEW GLLiveMenu(0, 0);

	rect.Set(114, 250, 50, 48);
	m_sendBoxMenu->AddMenuItem(this, IDC_MSG_DEL, NULL, rect);
	m_sendBoxMenu->GetItem(0)->SetPic(1, Frame_menu_DEL_MSG_EN  + g_pGLLiveInstance->getLang(), 114 - 233, 0);
	m_sendBoxMenu->GetItem(0)->SetSelPic(1, Frame_menu_BG_MSG_BOX2, 114 - 233, 0);

	//rect.Set(349, 250, 50, 48);
	//m_sendBoxMenu->AddMenuItem(this, IDC_MSG_DELALL, NULL, rect);
	//m_sendBoxMenu->GetItem(1)->SetPic(1, Frame_menu_DEL_ALL_MSG_EN  + g_pGLLiveInstance->getLang());
	//m_sendBoxMenu->GetItem(1)->SetSelPic(1, Frame_menu_BG_MSG_BOX4);

	rect.Set(380, 22, 60, 35);
	m_sendBoxMenu->AddMenuItem(this, IDC_MSG_REFRESH, NULL, rect);
	m_sendBoxMenu->GetItem(1)->SetPic(1, Frame_menu_REFRESH, 0, 2);
	m_sendBoxMenu->GetItem(1)->SetSelPic(1, Frame_menu_BG_REFRESH, 0, 2);

	//Init menu for read message
	m_readMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(114, 232, 50, 48);
	m_readMenu->AddMenuItem(this, IDC_MSGREAD_REPLY, NULL, rect);
	m_readMenu->GetItem(0)->SetPic(1, Frame_menu_REPLY_MSG_EN  + g_pGLLiveInstance->getLang());
	m_readMenu->GetItem(0)->SetSelPic(1, Frame_menu_BG_MSG_READ1);

	rect.Set(215, 232, 50, 48);
	m_readMenu->AddMenuItem(this, IDC_MSGREAD_DEL, NULL, rect);
	m_readMenu->GetItem(1)->SetPic(1, Frame_menu_DEL_MSG2_EN  + g_pGLLiveInstance->getLang());
	m_readMenu->GetItem(1)->SetSelPic(1, Frame_menu_BG_MSG_READ2);

	rect.Set(315, 232, 50, 48);
	m_readMenu->AddMenuItem(this, IDC_MSGREAD_ADD2LIST, NULL, rect);
	m_readMenu->GetItem(2)->SetPic(1, Frame_menu_ADD2FRIENDLIST_EN  + g_pGLLiveInstance->getLang());
	m_readMenu->GetItem(2)->SetSelPic(1, Frame_menu_BG_MSG_READ3);


	//Init menu for write message
	m_writeMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(355, 65, 40, 30);
	m_writeMenu->AddMenuItem(this, IDC_MSGWRITE_FRIENDLIST, NULL, rect);
	m_writeMenu->GetItem(0)->SetSelPic(1, Frame_menu_BG_OK2);
	
	//Init box menu
	m_boxMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(136, 181 + 9, 79, 32);
	m_boxMenu->AddMenuItem(this, IDC_MSG_NO, GetString(STR_ACCEPT), rect);
	m_boxMenu->GetItem(0)->SetPic(1, Frame_menu_BUTTON_YES, 0, 9);
	m_boxMenu->GetItem(0)->SetSelPic(1, Frame_menu_BG_YES, 0, 9);

	rect.Set(263, 181 + 9, 79, 32);
	m_boxMenu->AddMenuItem(this, IDC_MSG_YES, GetString(STR_REFUSE), rect);
	m_boxMenu->GetItem(1)->SetPic(1, Frame_menu_BUTTON_NO, 0, 9);
	m_boxMenu->GetItem(1)->SetSelPic(1, Frame_menu_BG_NO, 0, 9);
	m_isMsgActived = false;
	m_deleteConfirm = false;
	m_delAllConfirm = false;
	m_sendingMsg = false;
	m_readingMsg = false;
	m_readingSentMsg = false;
	m_willGoToLink = false;
}

GLLiveStateMessage::~GLLiveStateMessage(void)
{
	XP_API_DEL(m_listMenu);
	XP_API_DEL(m_readMenu);
	XP_API_DEL(m_writeMenu);
	XP_API_DEL(m_boxMenu);
	XP_API_DEL(m_sendBoxMenu);
}

void GLLiveStateMessage::OnRequestSuccess( int func_id, char * data, int data_len )
{
	switch(func_id)
	{
	case FUNCTION_READ_MY_SENT_MESSAGE:
		m_sub_state = MESSAGE_READ;
		m_readMenu->GetItemById(IDC_MSGREAD_REPLY)->SetVisible(false);
		m_readMenu->GetItemById(IDC_MSGREAD_ADD2LIST)->SetVisible(false);
		ChangeOffset(0);
		{
			char* temp = NULL;
			int size;
			base64_decode(m_gl_message->getMsgBody(), XP_API_STRLEN(m_gl_message->getMsgBody()), temp, size);
			memcpy(m_gl_message->getMsgBody(), temp, size);
			m_gl_message->getMsgBody()[size] = 0;
			XP_API_DELETE_ARRAY(temp);
		}
		break;
	case FUNCTION_READ_MESSAGE:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Get Message Successfully!\n" );
		if(m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_GAMEMESSAGE
			|| m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_GAMEMESSAGE_AVATAR
			|| m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_USERMESSAGE)
		{
			//m_sub_state = MESSAGE_READ;
			//Get the friends list
			//m_last_state = MESSAGE_BOX;
			if(m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_USERMESSAGE)
			{
				char* temp = NULL;
				int size;
				base64_decode(m_gl_message->getMsgBody(), XP_API_STRLEN(m_gl_message->getMsgBody()), temp, size);
				memcpy(m_gl_message->getMsgBody(), temp, size);
				m_gl_message->getMsgBody()[size] = 0;
				XP_API_DELETE_ARRAY(temp);
			}
			m_gl_userFriend->sendGetUserFriends();
		}
		else
		{
			m_pMsgBox->SetTitle(" ");
			m_pMsgBox->SetText(m_gl_message->getMsgBody());
			m_isMsgActived = true;
			m_boxMenu->GetItem(0)->SetTitle(GetString(STR_REFUSE));
			m_boxMenu->GetItem(1)->SetTitle(GetString(STR_ACCEPT));
			m_last_state = MESSAGE_BOX;
			m_sub_state = MESSAGE_ERROR;
		}
		checkHttpLink(m_gl_message->getMsgBody(), m_linkIndex1, m_linkIndex2);
		if(m_linkIndex1 != -1)
		{
			m_link = XP_API_NEW char[m_linkIndex2 - m_linkIndex1 + 1];
			XP_API_MEMCPY(m_link, m_gl_message->getMsgBody() + m_linkIndex1, m_linkIndex2 - m_linkIndex1);
			m_link[m_linkIndex2 - m_linkIndex1] = 0;
		}
		break;
	case FUNCTION_GET_MY_SENT_MESSAGE_HEADER:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Get Send Message Successfully!\n" );
		ChangeOffset(0);
		m_readingSentMsg = false;
		for(int j = 0; j < m_gl_message->getMsgListCount(); j++)
		{
			//if(m_gl_message->getMsgType(j) == USER_MESSAGE_TYPE_USERMESSAGE)
			{
				char* temp = NULL;
				int size;
				base64_decode(m_gl_message->getMsgTitle(j), XP_API_STRLEN(m_gl_message->getMsgTitle(j)), temp, size);
				memcpy(m_gl_message->getMsgTitle(j), temp, size);
				m_gl_message->getMsgTitle(j)[size] = 0;
				XP_API_DELETE_ARRAY(temp);
			}
		}
		m_sub_state = MESSAGE_SENDBOX;
		if(m_gl_message->getMsgListCount() > 0)
		{
			m_selected = 0;
		}
		else
		{
			m_selected = -1;
		}
		break;
	case FUNCTION_SEND_MESSAGE:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Send Message Successfully!\n" );
		if(m_buddy_name)
		{
			//After sending msg successful
			m_pMsgBox->SetTitle(GetString(STR_SUCCEED));
			m_pMsgBox->SetText(GetString(STR_MESSAGE_SENT));
			m_last_state = MESSAGE_WRITE;
			m_sub_state = MESSAGE_ERROR;
		}
		else
		{
			m_gl_message->SendGetMessageHeader(63, 0, 0, 0);
			m_last_state = MESSAGE_WRITE;
			m_sub_state = MESSAGE_PROCESS;
		}
		m_sendingMsg = true;
		break;
	case FUNCTION_DEL_MY_SENT_MESSAGE:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Del Sent Message Successfully!\n" );
		//ChangeOffset(0);
		m_gl_message->SendGetMySentMessageHeader(USER_MESSAGE_TYPE_USERMESSAGE);
		m_last_state = MESSAGE_SENDBOX;
		m_sub_state = MESSAGE_PROCESS;
		break;
	case FUNCTION_DEL_MESSAGE:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Del Message Successfully!\n" );
		if(m_last_state == MESSAGE_BOX)
		{
			//ChangeOffset(0);
			m_gl_message->SendGetMessageHeader(63, 0, 0, 0);
			m_last_state = MESSAGE_BOX;
			m_sub_state = MESSAGE_PROCESS;
		}
		else if(m_last_state == MESSAGE_READ)
		{
			//ChangeOffset(0);
			m_gl_message->SendGetMessageHeader(63, 0, 0, 0);
			m_last_state = MESSAGE_BOX;
			m_sub_state = MESSAGE_PROCESS;
		}
		break;
	case FUNCTION_ADD_USER_FRIEND:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Add Friend Successfully!\n" );
		m_sub_state = MESSAGE_READ;
		ChangeOffset(0);
		break;
	case FUNCTION_GET_MESSAGE_HEADER:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Reget Message Successfully!\n" );
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
				XP_API_DELETE_ARRAY(temp);
			}
//			for(int i = 0; i < XP_API_STRLEN(m_gl_message->getMsgTitle(j)); i++)
//			{
//				if(m_gl_message->getMsgTitle(j)[i] == 1)
//					m_gl_message->getMsgTitle(j)[i] = '^';
//			}
		}
		changeIconBadgeNumber(GetNewMsgCount());
		m_sub_state = MESSAGE_BOX;
		EnableGo(false);
		//reading msg
		if(m_readingMsg)
		{
			m_readingMsg = false;
			m_sub_state = MESSAGE_PROCESS;
			m_gl_message->SendReadMessage(m_gl_message->getMsgID(m_selected));
		}
		//After sending msg successful
		if(m_sendingMsg)
		{
			m_pMsgBox->SetTitle(GetString(STR_SUCCEED));
			m_pMsgBox->SetText(GetString(STR_MESSAGE_SENT));
			m_last_state = MESSAGE_BOX;
			m_sub_state = MESSAGE_ERROR;
			m_sendingMsg = false;
		}
		ChangeOffset(0);
		if(m_gl_message->getMsgListCount() > 0)
		{
			m_selected = 0;
		}
		else
		{
			m_selected = -1;
		}
		break;
	case FUNCTION_CONFIRM_USER_FRIEND:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Confirm Friend Successfully!\n" );
		//server already done
		//m_gl_message->SendDeleteMessage(m_gl_message->getCurrentMsgID());
		m_gl_message->SendGetMessageHeader(63, 0, 0, 0);
		m_last_state = MESSAGE_BOX;
		m_sub_state = MESSAGE_PROCESS;
		break;
	case FUNCTION_GET_USER_FRIENDS:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Get friends list OK!\n" );
		if(m_last_state == MESSAGE_WRITE)
		{
			m_sub_state = MESSAGE_WRITE;
			ChangeOffset(0);
			m_handler->PushGLLiveState(XP_API_NEW GLLiveStateFriends(m_handler, m_receiver));
		}
		else if( m_last_state == MESSAGE_BOX )
		{
			m_readMenu->GetItemById(IDC_MSGREAD_REPLY)->SetVisible(true);
			m_readMenu->GetItemById(IDC_MSGREAD_ADD2LIST)->SetVisible(true);
			//Check if msg is from your friend
			for(int i = 0; i < m_gl_userFriend->GetFriendsCount(); i++)
			{
				if(XP_API_STRCMP(m_gl_userFriend->GetName(i), m_gl_message->getMsgSender(m_gl_message->getCurrentMsgPosition())) == 0 )
				{
					m_readMenu->GetItemById(IDC_MSGREAD_ADD2LIST)->SetVisible(false);
					break;
				}
			}

			//yourself
			if(XP_API_STRCMP(m_gl_user->getUserName(), m_gl_message->getMsgSender(m_gl_message->getCurrentMsgPosition())) == 0)
			{
				m_readMenu->GetItemById(IDC_MSGREAD_ADD2LIST)->SetVisible(false);
			}
			//the system message
			if(m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_GAMEMESSAGE
				|| m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_GAMEMESSAGE_AVATAR)
			{
				m_readMenu->GetItemById(IDC_MSGREAD_REPLY)->SetVisible(false);
				m_readMenu->GetItemById(IDC_MSGREAD_ADD2LIST)->SetVisible(false);
			}
			m_sub_state = MESSAGE_READ;
			ChangeOffset(0);
		}
		break;
#ifndef WIN32
	//case FUNCTION_LOGOUT:
	//	XP_DEBUG_OUT( "[GLLiveStateMessage] Login out OK!\n" );
	//	extern void autoStartGame(int suspend, const char* name = "");
	//	autoStartGame(2, m_gl_message->getMsgSender(m_gl_message->getCurrentMsgPosition()));
	//	m_handler->TerminateGame();
	//	break;
#endif
	default:
		GLLiveState::OnRequestSuccess(func_id, data, data_len);
		break;
	}
}

//void GLLiveStateMessage::OnNetworkError()
//{
//	m_pMsgBox->SetTitle(GetString(STR_ERROR));
//	m_pMsgBox->SetText(GetErrorString(ERROR_CONNECTION));
//	m_sub_state = MESSAGE_ERROR;
//	XP_DEBUG_OUT( "[GLLiveStateMessage] OnNetworkError.\n");
//}

//void GLLiveStateMessage::OnRequestTimeout( int func_id )
//{
//	m_pMsgBox->SetTitle(GetString(STR_ERROR));
//	m_pMsgBox->SetText(GetErrorString(ERROR_REQUEST_TIME_OUT));
//
//	m_sub_state = MESSAGE_ERROR;;
//	XP_DEBUG_OUT( "[GLLiveStateMessage] OnRequestTimeout.\n");
//}

void GLLiveStateMessage::OnRequestFailure( int func_id, int error_code )
{
	if(func_id != FUNCTION_KEEP_ALIVE)
	{
		m_pMsgBox->SetTitle(GetString(STR_ERROR));
		m_pMsgBox->SetText(GetErrorString(error_code));
		m_sub_state = MESSAGE_ERROR;
	}

	switch(func_id)
	{
	case FUNCTION_READ_MESSAGE:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Get Message Failed!\n" );
		break;
	case FUNCTION_SEND_MESSAGE:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Send Message Failed!\n" );
		//EnableGo(true);
		if(error_code == ERROR_UUID_NOTEXISTS)
		{
			m_pMsgBox->SetText(GetErrorString(ERROR_USERNAME_NOTEXISTS));
		}
		break;
	case FUNCTION_DEL_MESSAGE:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Del Message Failed!\n" );
		break;
	case FUNCTION_ADD_USER_FRIEND:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Add Friend Failed!\n" );
		break;
	case FUNCTION_GET_MESSAGE_HEADER:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Reget Message Failed!\n" );
		//EnableGo(false);
		break;
	case FUNCTION_CONFIRM_USER_FRIEND:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Confirm Friend Failed!\n" );
		break;
	case FUNCTION_GET_USER_FRIENDS:
		XP_DEBUG_OUT( "[GLLiveStateMessage] Get friends list Failed!\n" );
	default:
		GLLiveState::OnRequestFailure(func_id, error_code);
		break;
	}
}

bool GLLiveStateMessage::HandleEvent(_TOUCH_EVENT event)
{
	if(m_sub_state == MESSAGE_SENDBOX)
	{
		if(event.eType == TET_Pressed)
		{
			if(event.position.y >= m_startY && event.position.y <= m_startY + m_height)
			{
				m_touchPos = event.position.y;
				m_moving   = true;
				return true;
			}
		}
		else if(event.eType == TET_Moving)
		{
			if(m_moving)
			{
				m_oldOffset = m_offset;
				//m_offset += m_touchPos - event.position.y;
				ChangeOffset(m_offset + (m_touchPos - event.position.y));
				if(m_touchPos - event.position.y > 2 || m_touchPos - event.position.y < -2)
				{
					m_moved = true;
				}
				m_touchPos = event.position.y;
				//m_curPage = -1 * m_offset / m_width;
				return true;
			}
		}
		else if(event.eType == TET_Released)
		{
			if(m_moving)
			{
				m_moving = false;
				if(m_moved)
				{
					m_moved = false;
					return false;
				}
				else
				{
					_TouchRect rect;
					rect.left  = 34;
					rect.top   = m_startY;
					rect.width = 413;
					rect.height= m_height;
					if(rect.Contain(event.position))
					{
						int select = (event.position.y - m_startY + m_offset) / m_lineHeight;
						if(select >= 0 && select < m_gl_message->getMsgListCount())
						{
							if(select == m_selected)
							{
								m_gl_message->SendReadMySentMessage(m_gl_message->getMsgID(m_selected));
								m_last_state = MESSAGE_SENDBOX;
								m_readingSentMsg = true;
								m_sub_state = MESSAGE_PROCESS;
							}
							else
							{
								m_selected = select;
							}
							m_speed = 0;
						}
						return true;
					}
				}
				return true;
			}
		}

		if(m_sendBoxMenu->HandleEvent(event))
		{
			return true;
		}
	}
	else if(m_sub_state == MESSAGE_WRITE)
	{
		//zyx 
		if(event.eType == TET_Pressed)
		{
			if(event.position.y >= 102 && event.position.y <= 102 + WRITE_BOX_MAX_HEIGHT)
			{
				m_touchPos = event.position.y;
				m_moving   = true;
				return true;
			}
		}
		else if(event.eType == TET_Moving)
		{
			if(m_moving)
			{
				m_oldOffset = m_offset;
				//m_offset += m_touchPos - event.position.y;
				ChangeMsgBoxOffset(m_offset + (m_touchPos - event.position.y),m_nLines,WRITE_BOX_MAX_LINES,m_handler->GetFont(3),WRITE_BOX_MAX_HEIGHT);
				if(m_touchPos - event.position.y > 2 || m_touchPos - event.position.y < -2)
				{
					m_moved = true;
				}
				m_touchPos = event.position.y;
				//m_curPage = -1 * m_offset / m_width;
				return true;
			}
		}
		else if(event.eType == TET_Released)
		{
			if(m_moving)
			{
				m_moving = false;
				if(m_moved)
				{
					m_moved = false;
					return false;
				}
				else
				{
					_TouchRect rectBody;
					rectBody.left  = 80;
					rectBody.top   = 100;
					rectBody.width = 320;
					rectBody.height= 160;
					if(rectBody.Contain(event.position))
					{
						m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_MESSAGE), m_body, false, WIRTE_MSG_MAX_LEN - 1, true));
						return true;
					}
				}
				return true;

			}
			else
			{
					_TouchRect rectName;
					rectName.left  = 107;
					rectName.top   = 65;
					rectName.width = 270;
					rectName.height= 25;
					if(rectName.Contain(event.position))
					{
						m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_RECEIVER), m_receiver, false, MAX_USERNAME - 1));
						return true;
					}
			 }
		}
		m_writeMenu->HandleEvent(event);
	}
	else if(m_sub_state == MESSAGE_READ)
	{
		//zyx 
		if(event.eType == TET_Pressed)
		{
			if(event.position.y >= 102 && event.position.y <= 102 + READ_BOX_MAX_HEIGHT)
			{
				m_touchPos = event.position.y;
				m_moving   = true;
				if(IsTouchInLinkArea(event.position.x, event.position.y))
				{
					m_willGoToLink = true;
				}
				return true;
			}
		}
		else if(event.eType == TET_Moving)
		{
			if(m_moving)
			{
				m_oldOffset = m_offset;
				//m_offset += m_touchPos - event.position.y;
				ChangeMsgBoxOffset(m_offset + (m_touchPos - event.position.y),m_nLines,READ_BOX_MAX_LINES,m_handler->GetFont(3),READ_BOX_MAX_HEIGHT);
				if(m_touchPos - event.position.y > 2 || m_touchPos - event.position.y < -2)
				{
					m_moved = true;
				}
				m_touchPos = event.position.y;
				//m_curPage = -1 * m_offset / m_width;
				return true;
			}
		}
		else if(event.eType == TET_Released)
		{
			if(m_moving)
			{
				m_moving = false;
				if(IsTouchInLinkArea(event.position.x, event.position.y) && m_willGoToLink)
				{
					extern void GLLGoToLink(const char* link);
					GLLGoToLink(m_link);
				}
				if(m_moved)
				{
					m_moved = false;
					return false;
				}
				return true;
			 }
		}			

		if(m_readMenu->HandleEvent(event))
		{
			return true;
		}
	}
	else if(m_sub_state == MESSAGE_BOX)
	{
		if(event.eType == TET_Pressed)
		{
			if(event.position.y >= m_startY && event.position.y <= m_startY + m_height)
			{
				m_touchPos = event.position.y;
				m_moving   = true;
				return true;
			}
		}
		else if(event.eType == TET_Moving)
		{
			if(m_moving)
			{
				m_oldOffset = m_offset;
				//m_offset += m_touchPos - event.position.y;
				ChangeOffset(m_offset + (m_touchPos - event.position.y));
				if(m_touchPos - event.position.y > 2 || m_touchPos - event.position.y < -2)
				{
					m_moved = true;
				}
				m_touchPos = event.position.y;
				//m_curPage = -1 * m_offset / m_width;
				return true;
			}
		}
		else if(event.eType == TET_Released)
		{
			if(m_moving)
			{
				m_moving = false;
				if(m_moved)
				{
					m_moved = false;
					return false;
				}
				else
				{
					_TouchRect rect;
					rect.left  = 34;
					rect.top   = m_startY;
					rect.width = 413;
					rect.height= m_height;
					if(rect.Contain(event.position))
					{
						int select = (event.position.y - m_startY + m_offset) / m_lineHeight;
						if(select >= 0 && select < m_gl_message->getMsgListCount())
						{
							if(select == m_selected)
							{
								if(m_gl_message->getMsgType(m_selected) == USER_MESSAGE_TYPE_GAMEMESSAGE
									|| m_gl_message->getMsgType(m_selected) == USER_MESSAGE_TYPE_USERMESSAGE
									|| m_gl_message->getMsgType(m_selected) == USER_MESSAGE_TYPE_GAMEMESSAGE_AVATAR)
								{
									m_gl_message->SendReadMessage(m_gl_message->getMsgID(m_selected));
								}
								else
								{
									//refresh the Sender for invitation
									m_gl_message->SendGetMessageHeader(63, 0, 0, 0);
									m_readingMsg = true;
								}
								m_last_state = MESSAGE_BOX;
								m_sub_state = MESSAGE_PROCESS;
								m_readingSentMsg = false;
							}
							else
							{
								m_selected = select;
							}
							m_speed = 0;
						}
						return true;
					}
				}
				return true;
			}
		}

		if(m_listMenu->HandleEvent(event))
		{
			return true;
		}
	}
	else if(m_sub_state == MESSAGE_PROCESS)
	{
		return true;
	}
	else
	{
		if(m_isMsgActived)
		{
			if(m_boxMenu->HandleEvent(event))
			{
				return true;
			}
		}
		if(m_pMsgBox != NULL && event.eType == TET_Pressed)
		{
			m_sub_state = m_last_state;
			if(m_buddy_name && m_sendingMsg)
			{
				m_sendingMsg = false;
				m_handler->PopGLLiveState();
			}
			if(m_sub_state == MESSAGE_BOX && m_isMsgActived)
			{
				m_gl_message->SendGetMessageHeader(63, 0, 0, 0);
				m_last_state = m_sub_state;
				m_sub_state = MESSAGE_PROCESS;
			}
			m_isMsgActived = false;
			return true;
		}
	}
	return GLLiveState::HandleEvent(event);
}

void GLLiveStateMessage::OnMenuItemClicked( int id )
{
	switch(id)
	{
	case IDC_BACK:
		if(m_sub_state != MESSAGE_BOX)
		{
			if(m_sub_state == MESSAGE_READ && m_readingSentMsg)
			{
				m_gl_message->SendGetMySentMessageHeader(USER_MESSAGE_TYPE_USERMESSAGE);
				m_last_state = m_sub_state;
				m_sub_state = MESSAGE_PROCESS;
				return;
			}

			if(m_buddy_name)
			{
				m_handler->PopGLLiveState();
				return;
			}
			m_gl_message->SendGetMessageHeader(63, 0, 0, 0);
			EnableGo(false);
			m_last_state = m_sub_state;
			m_sub_state = MESSAGE_PROCESS;
			return;
		}
		break;
	case IDC_GO:
		if(XP_API_STRLEN(m_receiver) == 0)
		{
			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetString(STR_MESSAGE_TO));
			m_last_state = MESSAGE_WRITE;
			m_sub_state = MESSAGE_ERROR;
			break;
		}
		else if(!CheckInput(m_receiver))
		{
			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetErrorString(ERROR_USERNAME_NOTEXISTS));
			m_last_state = MESSAGE_WRITE;
			m_sub_state = MESSAGE_ERROR;
			break;
		}
		else if(XP_API_STRLEN(m_body) == 0)
		{
			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetString(STR_NULL_MESSAGE));
			m_last_state = MESSAGE_WRITE;
			m_sub_state = MESSAGE_ERROR;
			break;
		}
		XP_API_MEMSET(m_title, 0, 32);
		XP_API_MEMCPY(m_title, m_body, MIN(XP_API_STRLEN(m_body), 31));
		for(int i = 0; i < MIN(XP_API_STRLEN(m_body), 31); i++)
		{
//			if(m_title[i] == '^')
//				m_title[i] = 1;
//			else 
			if(m_title[i] == '\n')
				m_title[i] = 0;
		}
		m_last_state = m_sub_state;
		m_sub_state = MESSAGE_PROCESS;
		if(false)
		{
		   m_gl_message->SendOnlineMessage(m_receiver, USER_MESSAGE_TYPE_USERMESSAGE, m_title, m_body, XP_API_STRLEN(m_body));
		}
		else
		{
			char* title = NULL;
			char* body = NULL;
			int size1;
			int size2;
			base64_encode(m_title, XP_API_STRLEN(m_title), title, size1);
			base64_encode(m_body, XP_API_STRLEN(m_body), body, size2);
			m_gl_message->SendOnlineMessage(m_receiver, USER_MESSAGE_TYPE_USERMESSAGE, title, body, XP_API_STRLEN(body));
			XP_API_DELETE_ARRAY(title);
			XP_API_DELETE_ARRAY(body);
		}
		break;
	case IDC_MSGREAD_REPLY:
		XP_API_STRCPY(m_receiver, m_gl_message->getMsgSender(m_gl_message->getCurrentMsgPosition()));
		XP_API_STRCPY(m_title, "");
		XP_API_STRCPY(m_body, "");
		EnableGo(true);
		m_sub_state = MESSAGE_WRITE;
		ChangeOffset(0);
		break;
	//case IDC_MSGREAD_DEL:
	//	//m_gl_message->SendDeleteMessage(m_gl_message->getCurrentMsgID());
	//	//m_sub_state = MESSAGE_PROCESS;
	//	//Show confirm screen
	//	m_pMsgBox->SetTitle(GetString(STR_WARNING));
	//	m_pMsgBox->SetText(GetString(STR_DELETE_CONFRIMATION));
	//	m_isMsgActived = true;
	//	m_deleteConfirm = true;
	//	m_last_state = m_sub_state;
	//	m_sub_state = MESSAGE_ERROR;
	//	break;
	case IDC_MSGREAD_ADD2LIST:
		m_gl_userFriend->sendAddUserFriend(m_gl_message->getMsgSender(m_gl_message->getCurrentMsgPosition()), GetString(STR_ADD_REQUEST_MESSAGE));
		m_last_state = MESSAGE_READ;
		m_sub_state = MESSAGE_PROCESS;
		break;
	case IDC_MSG_NEW:
		XP_API_STRCPY(m_receiver, "");
		XP_API_STRCPY(m_title, "");
		XP_API_STRCPY(m_body, "");
		EnableGo(true);
		m_sub_state = MESSAGE_WRITE;
		ChangeOffset(0);
		break;
	case IDC_MSGREAD_DEL:
	case IDC_MSG_DEL:
		//m_gl_message->SendDeleteMessage(m_gl_message->getMsgID(m_selected));
		if( m_gl_message->getMsgListCount() < 1 || m_selected < 0)
		{
			break;
		}
		//Show confirm screen
		m_pMsgBox->SetTitle(GetString(STR_WARNING));
		m_pMsgBox->SetText(GetString(STR_DELETE_CONFRIMATION));
		m_isMsgActived = true;
		m_boxMenu->GetItem(0)->SetTitle(GetString(STR_NO));
		m_boxMenu->GetItem(1)->SetTitle(GetString(STR_YES));
		m_deleteConfirm = true;
		m_last_state = m_sub_state;
		m_sub_state = MESSAGE_ERROR;
		break;
	case IDC_MSG_DELALL:
		break;
		//m_gl_message->SendDeleteMessage(-1);
		//m_last_state = m_sub_state;
		//m_sub_state = MESSAGE_PROCESS;
		//Show confirm screen
		m_pMsgBox->SetTitle(GetString(STR_WARNING));
		m_pMsgBox->SetText(GetString(STR_MESSAGE_CLEAR));//GetString(STR_DELETE_CONFRIMATION));
		m_isMsgActived = true;
		m_boxMenu->GetItem(0)->SetTitle(GetString(STR_NO));
		m_boxMenu->GetItem(1)->SetTitle(GetString(STR_YES));
		m_delAllConfirm = true;
		m_last_state = m_sub_state;
		m_sub_state = MESSAGE_ERROR;
		break;
	case IDC_MSG_REFRESH:
		if(m_sub_state == MESSAGE_BOX)
		{
			m_gl_message->SendGetMessageHeader(63, 0, 0, 0);
		}
		else if( m_sub_state == MESSAGE_SENDBOX )
		{
			m_gl_message->SendGetMySentMessageHeader(USER_MESSAGE_TYPE_USERMESSAGE);
		}
		m_last_state = m_sub_state;
		m_sub_state = MESSAGE_PROCESS;
		break;
	case IDC_MSG_READ:
		m_gl_message->SendReadMessage(m_gl_message->getMsgID(m_selected));
		m_last_state = m_sub_state;
		m_sub_state = MESSAGE_PROCESS;
		break;
	case IDC_MSGWRITE_FRIENDLIST:
		m_gl_userFriend->sendGetUserFriends();
		m_last_state = MESSAGE_WRITE;
		m_sub_state = MESSAGE_PROCESS;
		break;
	case IDC_MSG_YES:
		m_isMsgActived = false;
		if(m_deleteConfirm)
		{
			if(m_last_state == MESSAGE_SENDBOX || m_readingSentMsg)
			{
				m_gl_message->SendDeleteMySentMessage(m_gl_message->getMsgID(m_selected));
				m_sub_state = MESSAGE_PROCESS;
				m_deleteConfirm = false;
			}
			else
			{
				m_gl_message->SendDeleteMessage(m_gl_message->getMsgID(m_selected));
				m_sub_state = MESSAGE_PROCESS;
				m_deleteConfirm = false;
			}
		}
		else if(m_delAllConfirm)
		{
			m_gl_message->SendDeleteMessage(-1);
			m_sub_state = MESSAGE_PROCESS;
			m_delAllConfirm = false;
		}
		else if(m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION)
		{
			m_gl_userFriend->sendConfirmUserFriend(m_gl_message->getMsgSender(m_gl_message->getCurrentMsgPosition()), 1);
			m_sub_state = MESSAGE_PROCESS;
		}
		else
		{
			m_sub_state = m_last_state;
			m_deleteConfirm = false;
		}
		break;
	case IDC_MSG_NO:
		m_isMsgActived = false;
		if(m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION)
		{
			//nothing to do
			m_sub_state = MESSAGE_PROCESS;
		}
		if(m_deleteConfirm)
		{
			m_sub_state = m_last_state;
			m_deleteConfirm = false;
		}
		else if(m_delAllConfirm)
		{
			m_sub_state = m_last_state;
			m_delAllConfirm = false;
		}
		else if(m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION)
		{
			m_gl_userFriend->sendConfirmUserFriend(m_gl_message->getMsgSender(m_gl_message->getCurrentMsgPosition()), 0);
			m_sub_state = MESSAGE_PROCESS;
		}
		else
		{
			m_sub_state = m_last_state;
			m_deleteConfirm = false;
		}
		break;
	case IDC_MSG_SEND:
		m_gl_message->SendGetMySentMessageHeader(USER_MESSAGE_TYPE_USERMESSAGE);
		m_sub_state = MESSAGE_PROCESS;
		m_last_state = MESSAGE_BOX;
		break;
	}
	GLLiveState::OnMenuItemClicked(id);
}

void GLLiveStateMessage::Paint()
{
	m_handler->ClearScreen();
	switch(m_sub_state)
	{
	case MESSAGE_BOX:
		PaintMessageBox();
		break;
	case MESSAGE_READ:
		PaintMessageRead();
		break;
	case MESSAGE_WRITE:
		PaintMessageWrite();
		break;
	case MESSAGE_PROCESS:
		switch(m_last_state)
		{
		case MESSAGE_BOX:
			PaintMessageBox();
			break;
		case MESSAGE_READ:
			PaintMessageRead();
			break;
		case MESSAGE_WRITE:
			PaintMessageWrite();
			break;
		case MESSAGE_SENDBOX:
			PaintMessageSendBox();
			break;
		}
		PaintLoading();
		break;
	case MESSAGE_ERROR:
		switch(m_last_state)
		{
		case MESSAGE_BOX:
			PaintMessageBox();
			break;
		case MESSAGE_READ:
			PaintMessageRead();
			break;
		case MESSAGE_WRITE:
			PaintMessageWrite();
			break;
		case MESSAGE_SENDBOX:
			PaintMessageSendBox();
			break;
		}
		m_pMsgBox->Repaint(m_handler);
		if(m_isMsgActived)
		{
			m_boxMenu->Repaint(m_handler);
		}
		break;
	case MESSAGE_SENDBOX:
		PaintMessageSendBox();
		break;
	default:
		break;
	}
}

void GLLiveStateMessage::PaintMessageBox()
{
	CGLLiveGraphics *g = m_handler->GetGraphics();
	CGLLiveFont     * f = m_handler->GetFont(0);
	CGLLiveFont     * f1;
	CGLLiveFont     * f2;
	CGLLiveFont     * f3;

	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0, 0, 0);
	
	int count = m_gl_message->getMsgListCount();
	g->SetClip(0, m_startY, SCREEN_WIDTH, m_height);
	int offset = -m_offset;
	bool isRead;
	for(int i = 0; i < count; i++)
	{
		if(offset >= m_height)
		{
			break;
		}
		//Paint the friends infor that are visible
		if(offset  + m_lineHeight >= 0)
		{
			if(m_selected == i)
			{
				m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_SELECTION_BAR_W + (i % 2 ? 0 : 1), 36, 54 + offset, 0, 0, 0);
			}
			else
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LINE1 + (i % 2 ? 1 : 0), 34, 58 + offset, 0, 0, 0);
			}

			isRead = m_gl_message->getMsgReadState(i);
			if(isRead)
			{
				f1 = m_handler->GetFont(12);
				f2 = m_handler->GetFont(5);
				f3 = m_handler->GetFont(5);
			}
			else
			{
				f1 = m_handler->GetFont(3);
				f2 = m_handler->GetFont(13);
				f3 = m_handler->GetFont(13);
			}
			
			if(isRead)
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_READED_MSG, 44, 65 + offset, 0, 0, 0);
			}
			else
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_UNREAD_MSG, 44, 70 + offset, 0, 0, 0);
			}
			f1->DrawString(g, m_gl_message->getMsgSender(i), 90,  82 + offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER, 90);
			f2->InitPage(m_gl_message->getMsgTitle(i), 178);
			f2->DrawPage(g, m_gl_message->getMsgTitle(i), 185, 82 + offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER, 2);
			f3->InitPage(m_gl_message->getMsgCreatedTime(i), 65);
			f3->DrawPage(g, m_gl_message->getMsgCreatedTime(i), 396,  82 + offset, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);
		}
		offset += m_lineHeight;
	}

	//when message count less than 4, fix bg
	for(int j = count; j < 4; j++)
	{
			if(j % 2 == 0)
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LINE1, 34, 58 + j * 47, 0, 0, 0);
			}
			else
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LINE2, 34, 58 + j * 47, 0, 0, 0);
			}
	}

	g->ResetClip();

	//Paint scroll bar
	if(count > 4)
	{

		m_scrollBarH = m_height * m_height / (count * m_lineHeight);
		if(m_scrollBarH < 10)
		{
			m_scrollBarH = 10;
		}

		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_MESSAGE, 219, 0, 0, 0, 0);
		for(int a = 0; a < m_scrollBarH / 10; a++)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR, 434, m_startY + m_scrollBarPos + a * 10, 0, 0, 0);
		}
		if(m_scrollBarH % 10 > 0)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR, 434, m_startY + m_scrollBarPos + m_scrollBarH - 10, 0, 0, 0);
		}
	}

	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_TITLE_FRIENDS, 2, 4, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BOTTOM, 2, 0, 0, 0, 0);
	f->DrawString(g, GetString(STR_MSG_TITLE), SCREEN_WIDTH >> 1, 30, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);

	m_listMenu->Repaint(m_handler);

	GLLiveState::Paint();
}

void GLLiveStateMessage::PaintMessageSendBox()
{
	CGLLiveGraphics *g = m_handler->GetGraphics();
	CGLLiveFont     * f = m_handler->GetFont(0);
	CGLLiveFont     * f1;
	CGLLiveFont     * f2;
	CGLLiveFont     * f3;

	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0, 0, 0);
	
	int count = m_gl_message->getMsgListCount();
	g->SetClip(0, m_startY, SCREEN_WIDTH, m_height);
	int offset = -m_offset;
	bool isRead;
	for(int i = 0; i < count; i++)
	{
		if(offset >= m_height)
		{
			break;
		}
		//Paint the friends infor that are visible
		if(offset  + m_lineHeight >= 0)
		{
			if(m_selected == i)
			{
				m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_SELECTION_BAR_W + (i % 2 ? 0 : 1), 36, 54 + offset, 0, 0, 0);
			}
			else
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LINE1 + (i % 2 ? 1 : 0), 34, 58 + offset, 0, 0, 0);
			}

			isRead = m_gl_message->getMsgReadState(i);
			if(isRead)
			{
				f1 = m_handler->GetFont(12);
				f2 = m_handler->GetFont(5);
				f3 = m_handler->GetFont(5);
			}
			else
			{
				f1 = m_handler->GetFont(3);
				f2 = m_handler->GetFont(13);
				f3 = m_handler->GetFont(13);
			}
			
			//if(isRead)
			//{
			//	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_READED_MSG, 44, 65 + offset, 0, 0, 0);
			//}
			//else
			//{
			//	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_UNREAD_MSG, 44, 70 + offset, 0, 0, 0);
			//}
			f1->DrawString(g, m_gl_message->getMsgSender(i), 90,  82 + offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER, 90);
			f2->InitPage(m_gl_message->getMsgTitle(i), 178);
			f2->DrawPage(g, m_gl_message->getMsgTitle(i), 185, 82 + offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
			f3->InitPage(m_gl_message->getMsgCreatedTime(i), 65);
			f3->DrawPage(g, m_gl_message->getMsgCreatedTime(i), 396,  82 + offset, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);
		}
		offset += m_lineHeight;
	}

	//when message count less than 4, fix bg
	for(int j = count; j < 4; j++)
	{
			if(j % 2 == 0)
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LINE1, 34, 58 + j * 47, 0, 0, 0);
			}
			else
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_LINE2, 34, 58 + j * 47, 0, 0, 0);
			}
	}

	g->ResetClip();

	//Paint scroll bar
	if(count > 4)
	{

		m_scrollBarH = m_height * m_height / (count * m_lineHeight);
		if(m_scrollBarH < 10)
		{
			m_scrollBarH = 10;
		}

		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_MESSAGE, 219, 0, 0, 0, 0);
		for(int a = 0; a < m_scrollBarH / 10; a++)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR, 434, m_startY + m_scrollBarPos + a * 10, 0, 0, 0);
		}
		if(m_scrollBarH % 10 > 0)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR, 434, m_startY + m_scrollBarPos + m_scrollBarH - 10, 0, 0, 0);
		}
	}

	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_TITLE_FRIENDS, 2, 4, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BOTTOM, 2, 0, 0, 0, 0);
	f->DrawString(g, GetString(STR_MSG_TITLE), SCREEN_WIDTH >> 1, 30, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);

	m_sendBoxMenu->Repaint(m_handler);

	GLLiveState::Paint();
}

void GLLiveStateMessage::ChangeOffset(int newOffset)
{
	if(m_sub_state == MESSAGE_WRITE)
	{
		ChangeMsgBoxOffset(newOffset,m_nLines,WRITE_BOX_MAX_LINES,m_handler->GetFont(3),WRITE_BOX_MAX_HEIGHT);
		return;
	}
	else if(m_sub_state == MESSAGE_READ)
	{
		ChangeMsgBoxOffset(newOffset,m_nLines,READ_BOX_MAX_LINES,m_handler->GetFont(3),READ_BOX_MAX_HEIGHT);
		return;
	}
	int count = m_gl_message->getMsgListCount();
	m_offset = newOffset;
	if(m_offset < 0 || count < 4)
	{
		m_offset = 0;
	}
	else if(m_offset > (count - 4) * m_lineHeight)
	{
		m_offset = (count - 4) * m_lineHeight;
	}
	//Update scroll bar pos
	if(count > 4)
	{
		m_scrollBarPos = (m_height - m_scrollBarH) * m_offset / ((count - 4)* m_lineHeight);
	}
}



void GLLiveStateMessage::ChangeMsgBoxOffset(int newOffset,int curLines,int maxLines,CGLLiveFont* f2,int BarMaxHeight )
{
	//CGLLiveFont     * f2 = m_handler->GetFont(3);
	int nLineH = f2->GetLineHeight();
	int nLineSpace = f2->GetLineSpacing();

	int nLines = curLines;
	m_offset = newOffset;
	if(m_offset < 0 || nLines < maxLines)
	{
		m_offset = 0;
	}
	else if(m_offset > ((nLines - maxLines) * nLineH - nLineSpace))
	{
		m_offset = (nLines - maxLines) * nLineH - nLineSpace;
	}
	//update link Position
	if(m_sub_state == MESSAGE_READ && m_linkIndex1 != -1)
		m_handler->GetFont(3)->InitLinkPosition(m_gl_message->getMsgBody(), READ_BOX_OFF_X, READ_BOX_OFF_Y - m_offset, READ_BOX_WIDTH - 8, m_linkIndex1, m_linkIndex2, m_linkPosition);
	//Update scroll bar pos
	if(nLines > maxLines)
	{
		m_scrollBarPos = (BarMaxHeight - m_scrollBarH) * m_offset / ((nLines - maxLines)* nLineH - nLineSpace);
	}
	else {
		m_scrollBarPos = 0;
	}
}

void GLLiveStateMessage::PaintHScoreBar(int ScrollFrmID,int posX,int posY,int& scrollBarH,int scrollBarPosY,int cur,int max,int BarMaxHeight)
{
		CGLLiveGraphics *g = m_handler->GetGraphics();

		scrollBarH = BarMaxHeight * max / cur;

		if(scrollBarH < 10)
		{
			scrollBarH = 10;
		}

		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, ScrollFrmID, 0, 0, 0, 0, 0);
		for(int a = 0; a < scrollBarH / 10; a++)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR, posX, posY + scrollBarPosY + a * 10, 0, 0, 0);
		}
		if(scrollBarH % 10 > 0)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR, posX, posY + scrollBarPosY + scrollBarH - 10, 0, 0, 0);
		}
}

void GLLiveStateMessage::PaintMessageRead()
{
	CGLLiveGraphics *g = m_handler->GetGraphics();
	CGLLiveFont     * f = m_handler->GetFont(0);
	CGLLiveFont     * f1 = m_handler->GetFont(3);
	CGLLiveFont     * f2 = m_handler->GetFont(3);
	CGLLiveFont     * f3 = m_handler->GetFont(12);//5

	m_nLines = 0;

	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG2, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_TITLE_FRIENDS, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_MESSAGE_BOX_R, 0, 0, 0, 0, 0);

	if(m_readingSentMsg)
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_MSG_TO_EN  + g_pGLLiveInstance->getLang(), 80, 70, 0, 0, 0);
	}
	else
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_MSG_FROM_EN  + g_pGLLiveInstance->getLang(), 80, 70, 0, 0, 0);
	}

	f->DrawString(g, GetString(STR_MESSAGE), SCREEN_WIDTH >> 1, 30, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);
	f1->DrawString(g, m_gl_message->getMsgSender(m_gl_message->getCurrentMsgPosition()), 140, 80, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);

	m_nLines = f2->InitPage(m_gl_message->getMsgBody(), 300 - 8);
	int _line = m_nLines;
	if(m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_USERMESSAGE)
	{
		m_nLines += (f3->InitPage(m_gl_user->getAttention(), 300 - 8) + 3);
	}

	g->SetClip(READ_BOX_OFF_X, READ_BOX_OFF_Y, READ_BOX_WIDTH + 15, READ_BOX_MAX_HEIGHT - 5);
	//draw the underlines
	if(m_linkIndex1 != -1)
	{
		int lineHeight = m_handler->GetFont(3)->GetLineHeight();
		int lineSpace = m_handler->GetFont(3)->GetLineSpacing();
		int linkLines = (m_linkPosition[3] - m_linkPosition[1]) / lineHeight;
		g->SetARGBColor(0xff000000);
		if(linkLines == 0)
		{
			g->DrawLine(m_linkPosition[0], m_linkPosition[1] + lineHeight - lineSpace, m_linkPosition[2], m_linkPosition[3] + lineHeight - lineSpace);
		}
		else
		{
			g->DrawLine(m_linkPosition[0], m_linkPosition[1] + lineHeight - lineSpace, READ_BOX_OFF_X + READ_BOX_WIDTH, m_linkPosition[1] + lineHeight - lineSpace);
			for(int i = 1; i < linkLines; i++)
				g->DrawLine(READ_BOX_OFF_X, m_linkPosition[1] + lineHeight * (i + 1) - lineSpace, READ_BOX_OFF_X + READ_BOX_WIDTH, m_linkPosition[1] + lineHeight * (i + 1) - lineSpace);
			g->DrawLine(READ_BOX_OFF_X, m_linkPosition[3] + lineHeight - lineSpace, m_linkPosition[2], m_linkPosition[3] + lineHeight - lineSpace);
		}
	}
	if(m_nLines <= READ_BOX_MAX_LINES)
	{
		f2->DrawPage(g, m_gl_message->getMsgBody()/*m_body*/, READ_BOX_OFF_X, READ_BOX_OFF_Y, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
		if(m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_USERMESSAGE)
		{
			f3->DrawPage(g, m_gl_user->getAttention(), READ_BOX_OFF_X, READ_BOX_OFF_Y + (f2->GetLineHeight()) * (_line + 3), CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
		}
	}
	else 
	{
		f2->DrawPage(g, m_gl_message->getMsgBody()/*m_body*/, READ_BOX_OFF_X, READ_BOX_OFF_Y - m_offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
		if(m_gl_message->getMsgType(m_gl_message->getCurrentMsgPosition()) == USER_MESSAGE_TYPE_USERMESSAGE)
		{
			f3->DrawPage(g, m_gl_user->getAttention(), READ_BOX_OFF_X, READ_BOX_OFF_Y + (f2->GetLineHeight()) * (_line + 3) - m_offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
		}
		g->ResetClip();
		//paint H Scoll Bar
		PaintHScoreBar(Frame_menu_MESSAGE_BOX_R_SCROLL,393 ,102, m_scrollBarH,m_scrollBarPos,m_nLines,READ_BOX_MAX_LINES,READ_BOX_MAX_HEIGHT);
	}

	g->ResetClip();
	
	m_readMenu->Repaint(m_handler);

	GLLiveState::Paint();
}

void GLLiveStateMessage::PaintMessageWrite()
{
	CGLLiveGraphics *g = m_handler->GetGraphics();
	CGLLiveFont     * f = m_handler->GetFont(0);
	CGLLiveFont     * f1 = m_handler->GetFont(3);
	CGLLiveFont     * f2 = m_handler->GetFont(3);

	m_nLines = 0;
	//int nLineH = f2->GetLineHeight();

	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG2, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_TITLE_FRIENDS, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_MESSAGE_BOX_W, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_MSG_TO_EN  + g_pGLLiveInstance->getLang(), 80, 70, 0, 0, 0);
	f->DrawString(g, GetString(STR_NEWMSG), SCREEN_WIDTH >> 1, 30, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);
	f1->DrawString(g, m_receiver, 140, 80, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER, 205);

	if((m_frameCount / 15) % 2 == 0)
	{
		XP_API_STRCPY(m_showBodyBuffer, m_body);
		XP_API_STRCAT(m_showBodyBuffer, "|");
	}
	else
	{
		XP_API_STRCPY(m_showBodyBuffer, m_body);
	}


	m_nLines = f2->InitPage(m_showBodyBuffer, 300 - 8);

	g->SetClip(READ_BOX_OFF_X, READ_BOX_OFF_Y, READ_BOX_WIDTH + 15, WRITE_BOX_MAX_HEIGHT - 5);
	if(m_nLines <= WRITE_BOX_MAX_LINES)
	{
		f2->DrawPage(g, m_showBodyBuffer, READ_BOX_OFF_X, READ_BOX_OFF_Y, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	}
	else 
	{
		
		f2->DrawPage(g, m_showBodyBuffer, READ_BOX_OFF_X, READ_BOX_OFF_Y - m_offset, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
		g->ResetClip();
		//paint H Scoll Bar
		PaintHScoreBar(Frame_menu_MESSAGE_BOX_W_SCROLL,393 ,102, m_scrollBarH,m_scrollBarPos,m_nLines,WRITE_BOX_MAX_LINES,WRITE_BOX_MAX_HEIGHT);

	}

	g->ResetClip();

	m_writeMenu->Repaint(m_handler);
	
	GLLiveState::Paint();
}

void GLLiveStateMessage::UpdateAnimation()
{
	if(m_moving)
	{
		m_speed = m_offset - m_oldOffset;/* / 1 */
		m_oldOffset = m_offset;
	}
	else
	{
		if( ABS(m_speed) > 0 && ABS(m_speed) <= 5)
		{
			ChangeOffset(m_offset + m_speed);
			if(m_offset % m_lineHeight != 0)
			{
				if(ABS(m_speed) > 1)
				{
					m_speed = (m_speed / ABS(m_speed)) * (ABS(m_speed) - 1);
				}
			}
			else
			{
				m_speed = 0;
			}
		}
		else if(ABS(m_speed) > 5)
		{
			//m_offset += m_speed;
			ChangeOffset(m_offset + m_speed);
			m_speed -= (m_speed / ABS(m_speed)) * 5;
			//m_speed /= 1.8;
		}
	}

	m_listMenu->Update();
	m_readMenu->Update();
	m_writeMenu->Update();
	m_boxMenu->Update();
	m_sendBoxMenu->Update();

	GLLiveState::UpdateAnimation();
}

int GLLiveStateMessage::GetNewMsgCount()
{
	int newMsgCount = 0;
	for(int i = 0; i < m_gl_message->getMsgListCount(); i++)
	{
		if(m_gl_message->getMsgReadState(i) == false)
		{
			newMsgCount++;
		}
	}
	
	return newMsgCount;
}

void GLLiveStateMessage::checkHttpLink(const char* src, int& index1, int& index2)
{
	index1 = -1;
	index2 = -1;

	//search the start
	const char* http_label = "http://";
	int i = 0;
	while(i < XP_API_STRLEN(src) - 7)
	{
		int j;
		for(j = 0; j < 7; j++)
		{
			char c;
			if(src[i + j] >= 'A' && src[i + j] <= 'Z')
			{
				c = src[i + j] + ('a' - 'A');
			}
			else
			{
				c = src[i + j];
			}
			if(c != http_label[j])
				break;//break the for
		}
		if(j == 7)
		{
			index1 = i;
			break;//break the while
		}
		i++;
	}

	//search the end
	if(index1 != -1)
	{
		i += 7;//"http://" length
		index2 = XP_API_STRLEN(src);
		while(i < XP_API_STRLEN(src))
		{
			if(src[i] == 0x09 || src[i] == 0x0A || src[i] == 0x0D || src[i] == 0x20)//tab, LF, CR, space
			{
				index2 = i;
				break;
			}
			i++;
		}
	}
}

bool GLLiveStateMessage::IsTouchInLinkArea(int x, int y)
{
	if(m_linkIndex1 == -1)
		return false;

	int lines = (m_linkPosition[3] - m_linkPosition[1]) / (m_handler->GetFont(3)->GetLineHeight());
	if(lines == 0)
	{
		if(x > m_linkPosition[0] && x < m_linkPosition[2] && y > m_linkPosition[1] && y < m_linkPosition[1] + m_handler->GetFont(3)->GetLineHeight())
			return true;
	}
	else
	{
		if((x > m_linkPosition[0] && x < READ_BOX_OFF_X + READ_BOX_WIDTH && y > m_linkPosition[1] && y < m_linkPosition[1] + m_handler->GetFont(3)->GetLineHeight())//first line
			|| (x > READ_BOX_OFF_X && x < READ_BOX_OFF_X + READ_BOX_WIDTH && y > m_linkPosition[1] + m_handler->GetFont(3)->GetLineHeight() && y < m_linkPosition[3])//middle lines
			|| (x > READ_BOX_OFF_X && x < m_linkPosition[2] && y > m_linkPosition[1] && y < m_linkPosition[3] + m_handler->GetFont(3)->GetLineHeight())//last line
			)
			return true;
	}

	return false;
}