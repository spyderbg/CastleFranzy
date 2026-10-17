#include "GLLiveStdafx.h"
#include "GLLiveState.h"
#include "GLLiveStateChat.h"
#include "GLLiveStateMenu.h"
#include "GLLiveMenu.h"

#define MAX_KEEP_ALIVE_FAILED 3

//Static member init
GLXPlayerSereverConfig* GLLiveState::m_gl_serverConfig = NULL;
GLXPlayerLogin        * GLLiveState::m_gl_login = NULL;
GLXPlayerUser         * GLLiveState::m_gl_user = NULL;
GLXPlayerMessage      * GLLiveState::m_gl_message = NULL;
GLXPlayerUserFriend   * GLLiveState::m_gl_userFriend = NULL;
GLXPlayerChat         * GLLiveState::m_gl_chat = NULL;
//GLXPlayerChatRoomList * GLLiveState::m_gl_chatRoomList = NULL;
int                   GLLiveState::m_instanceCount = 0;
int                   GLLiveState::m_frameCount = 0;
//UserInfo              GLLiveState::m_loginUser;
bool                  GLLiveState::m_bAutoLogin = false;
int                   GLLiveState::m_iKeepFailedCount = 0;
bool                  GLLiveState::m_bLoginFromOtherPlace = false;

//Member function implement
GLLiveState::GLLiveState( GLLiveStateHandler * handler, bool cancel ) : m_handler( handler )
{
	CGLLiveGraphics *g = m_handler->GetGraphics();
	//Init command bar
	_TouchRect rBack, rGo;
	rBack.left = 12;
	rBack.top  = 251;
	rBack.width = m_handler->GetASprite(SPRITE_MENU)->GetFrameWidth(Frame_menu_BUTTON_BACK);
	rBack.height = m_handler->GetASprite(SPRITE_MENU)->GetFrameHeight(Frame_menu_BUTTON_BACK);

	rGo.left = 406;
	rGo.top  = 251;
	rGo.width = m_handler->GetASprite(SPRITE_MENU)->GetFrameWidth(Frame_menu_BUTTON_OK);
	rGo.height = m_handler->GetASprite(SPRITE_MENU)->GetFrameHeight(Frame_menu_BUTTON_OK);

	m_commandBar = new GLLiveMenu(0, 0);
	m_commandBar->AddMenuItem(this, IDC_BACK, GetString(STR_BACK), rBack);
	m_commandBar->GetItem(0)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_BACK);
	m_commandBar->GetItem(0)->SetSelPic(SPRITE_MENU, Frame_menu_BG_BACK);
	m_commandBar->GetItem(0)->SetFont(3, 0);
	m_commandBar->GetItem(0)->SetTitlePos(rBack.width, 0, CGLLiveGraphics::RIGHT | CGLLiveGraphics::TOP);

	m_commandBar->AddMenuItem(this, IDC_GO, GetString(STR_GO), rGo);
	m_commandBar->GetItem(1)->SetPic(SPRITE_MENU, Frame_menu_BUTTON_OK);
	m_commandBar->GetItem(1)->SetSelPic(SPRITE_MENU, Frame_menu_BG_OK);
	m_commandBar->GetItem(1)->SetFont(3, 0);
	m_commandBar->GetItem(1)->SetTitlePos(rGo.width / 2, -3, CGLLiveGraphics::HCENTER | CGLLiveGraphics::BASELINE);

	//Init static online object when create a instance first time
	if(m_instanceCount == 0)
	{
#ifdef GET_CONFIG_FROM_SERVER
		m_gl_serverConfig = XP_API_NEW GLXPlayerSereverConfig();
		m_gl_serverConfig->RegisterObserver(this);
#else
		m_gl_login        = XP_API_NEW GLXPlayerLogin();
		m_gl_message      = XP_API_NEW GLXPlayerMessage();
		m_gl_user         = XP_API_NEW GLXPlayerUser();
		m_gl_userFriend   = XP_API_NEW GLXPlayerUserFriend();
		m_gl_chat         = XP_API_NEW GLXPlayerChat();
#endif
		//m_gl_chatRoomList = XP_API_NEW GLXPlayerChatRoomList();
	}
	if(cancel)
	{
		CancelRequest();
	}
	RegisterOnlineObject();
	m_instanceCount++;

	m_frameCount = 0;

	m_pMsgBox = NULL;
}

GLLiveState::~GLLiveState(void)
{
	XP_API_DEL(m_commandBar);
	XP_API_DEL(m_pMsgBox);
	m_instanceCount--;
	//Destroy static online object when there is no instance
	if(m_instanceCount == 0)
	{
		XP_API_DEL(m_gl_serverConfig);
		XP_API_DEL(m_gl_message);
		//m_gl_login->SendLogout();
		//m_gl_login->Update();
		XP_API_DEL(m_gl_login);
		XP_API_DEL(m_gl_user);
		XP_API_DEL(m_gl_userFriend);
		if(m_gl_chat)
		{
			m_gl_chat->Logout();
			XP_API_DEL(m_gl_chat);
		}
		//XP_API_DEL(m_gl_chatRoomList);
	}
}

void GLLiveState::CancelRequest()
{
	//Cancel all request
	if(m_gl_login)
	{
		//m_gl_login->Cancel();
		m_gl_message->Cancel();
		m_gl_user->Cancel();
		m_gl_userFriend->Cancel();
		//m_gl_chat
	}
}

void GLLiveState::Paint()
{
	if(m_commandBar != NULL)
	{
		m_commandBar->Repaint(m_handler);
	}
}

bool GLLiveState::HandleEvent(_TOUCH_EVENT event)
{
	if(m_commandBar != NULL && m_commandBar->HandleEvent(event))
	{
		return true;
	}
	return false;
}

void GLLiveState::OnMenuItemClicked(int id)
{
	//Back
	if(id == IDC_BACK)
	{
		m_handler->PopGLLiveState();
		return;
	}
}

void GLLiveState::EnableGo(bool enable)
{
	m_commandBar->GetItem(1)->SetVisible(enable);
}

void GLLiveState::RegisterOnlineObject()
{	
	if(m_gl_login)
	{
		m_gl_login->RegisterObserver( this );
		m_gl_message->RegisterObserver( this );
		m_gl_userFriend->RegisterObserver( this );
		m_gl_user->RegisterObserver( this );
		m_gl_chat->RegisterChatObserver(this);
		m_gl_chat->RegisterChatRoomObserver( this );
	}
}

void GLLiveState::UpdateAnimation()
{
	if(m_gl_serverConfig)
		m_gl_serverConfig->Update();
	if(m_gl_login)
	{
		m_gl_login->Update();
		m_gl_message->Update();
		m_gl_userFriend->Update();
		m_gl_user->Update();
		m_gl_chat->Update();
	}
	//if(m_pMsgBox != NULL)
	//{
	//	m_pMsgBox->Update();
	//}

	m_frameCount++;

	if(m_commandBar->Update())
	{
		return;
	}
}

void GLLiveState::PaintLoading()
{
	CGLLiveGraphics *g = m_handler->GetGraphics();

	g->EndDraw2D();
	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
	//g->EnableTextureBlend(true);
	//g->SetARGBColor(0x80000000);
	//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0);
	//g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	g->EnableTextureBlend(true);
	g->SetARGBColor(0x88f08008);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_LOADING_BG, 0, 0, 0, 0, 0);
	g->SetARGBColor(0x99f08008);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_LOADING_1 + (m_frameCount + 0) % 12, 0, 0, 0, 0, 0);
	g->SetARGBColor(0xccf08008);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_LOADING_1 + (m_frameCount + 1) % 12, 0, 0, 0, 0, 0);
	g->SetARGBColor(0xfff08008);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_LOADING_1 + (m_frameCount + 2) % 12, 0, 0, 0, 0, 0);
	g->EnableTextureBlend(false);
}

void GLLiveState::PaintAvatar(CGLImage* img, int x, int y, int scalex, int scaley)
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
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_AVATAR_LOADING0 + m_frameCount % 8, x, y, 0);
	}
	//the avatar is downloaded, but the avatar is not a picture
	else if(img->IsNull())
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_AVATAR_NULL, x, y, 0);//Frame_menu_BUTTON_PROFILE_EN  + g_pGLLiveInstance->getLang()
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

char* GLLiveState::GetErrorString(int errorCode)
{
	int stringId;
	switch(errorCode)
	{
	case ERROR_CONNECTION:
	case ERROR_NO_RESPONSE:
		stringId = STR_ERROR_CONNECTION;
		break;
	case ERROR_REQUEST_TIME_OUT:
		stringId = STR_REQUEST_TIME_OUT;
		break;
	case ERROR_NO_USERNAME:
		stringId = STR_NO_USERNAME;
		break;
	case ERROR_NO_PASSWORD:
		stringId = STR_NO_PASSWORD;
		break;
	case ERROR_REG_NO_PASSWORD:
		stringId = STR_REG_NO_PASSWORD;
		break;
	case ERROR_USERNAME_TAKEN:
		stringId = STR_USERNAME_TAKEN;
		break;
	case ERROR_INVALID_USERNAMEORPASSWORD:
		stringId = STR_INVALID_USERNAMEORPASSWORD;
		break;
	case ERROR_NO_EMAIL:
		stringId = STR_EMAIL_Err;
		break;
	case ERROR_USER_HAS_LOGIN:
		stringId = STR_USER_HAS_LOGIN;
		break;
	case ERROR_HAS_NOT_LOGIN:
		stringId = STR_HAS_NOT_LOGIN;
		break;
	case ERROR_FRIEND_NOT_EXISTS:
		stringId = STR_FRIEND_NOT_EXISTS;
		break;
	case ERROR_DATA_SIZE_INVALID:
		stringId = STR_DATA_SIZE_INVALID;
		break;
	case ERROR_PASSWORD_CONFIRM_FAILED:
		stringId = STR_PASSWORD_CONFIRM_FAILED;
		break;
	case ERROR_NO_CURRENT_PWD:
		stringId = STR_NO_CURRENT_PWD;
		break;
	case ERROR_NO_NEW_PWD:
		stringId = STR_NO_NEW_PWD;
		break;
	case ERROR_USERNAME_NOTEXISTS:
		stringId = STR_NO_USER;
		break;
	case ERROR_REPUTATION_REPEAT:
		stringId = STR_REPUTATION_ERR;
		break;
	case ERROR_WRONG_EMAIL:
		stringId = STR_REG_ERROR3;
		break;
	case ERROR_WRONG_INPUT:
		stringId = STR_INPUT_LIMIT;
		break;
	case ERROR_BAD_USER_NAME:
		stringId = STR_BADNAME;
		break;
	case ERROR_FRIENDS_TOO_MANY:
		stringId = STR_FRIEND_FULL;
		break;
	case ERROR_EMAIL_USED:
		stringId = STR_REG_ERROR2;
		break;
	default:
		stringId = STR_ERROR_CONNECTION;//STR_UNKNOWN_ERROR;
		break;
	}
	return GetString(stringId);
}

void GLLiveState::OnNetworkError()
{
	OnRequestFailure( 0, ERROR_CONNECTION );
}

void GLLiveState::OnRequestTimeout(int func_id)
{
	OnRequestFailure( func_id, ERROR_REQUEST_TIME_OUT );
}

void GLLiveState::OnRequestFailure(int func_id, int error_code)
{
	if(m_iKeepFailedCount >= MAX_KEEP_ALIVE_FAILED)
	{
		return;
	}

	if( State() != DEF::GAME_STATE_LOGIN && (func_id == FUNCTION_KEEP_ALIVE || error_code == ERROR_CONNECTION ))
	{
		m_iKeepFailedCount++;
		XP_DEBUG_OUT("[GLLiveState] Keep Alive Failed Count----------- %d\n", m_iKeepFailedCount);
		if(m_iKeepFailedCount >= MAX_KEEP_ALIVE_FAILED)
		{
			//Pop the edit state if cur state is edit
			if(m_handler->GetCurState()->State() == DEF::GAME_STATE_EDIT)
			{
				m_handler->PopGLLiveState(false);
			}
			g_pGLLiveInstance->m_pDiconnectState = XP_API_NEW GLLiveStateDisconnect(m_handler, m_handler->GetCurState());
			return;
		}
	}
}

void GLLiveState::OnRequestSuccess( int func_id, char * data, int data_len )
{
	if(func_id == FUNCTION_KEEP_ALIVE)
	{
		m_iKeepFailedCount = 0;
	}
}

void GLLiveState::OnChatRoomInvitation( const char * room, const char * invitor, const char * reason,
		const char * password, const char *realInvitor )
{
	char invitorName[128];
	XP_API_MEMSET(invitorName, 0, sizeof(invitorName));
	if(realInvitor != NULL && XP_API_STRLEN(realInvitor) > 0)
	{
		XP_API_STRCPY(invitorName, realInvitor);
	}
	else
	{
		XP_API_PARSE_DATA(invitor, invitorName, 0, '@');
	}
	XP_DEBUG_OUT( "[GLLiveState] Receive Chat Room Invitation!\n" );
	XP_DEBUG_OUT( "[GLLiveState] Room    : %s\n", room );
	XP_DEBUG_OUT( "[GLLiveState] Invitor : %s\n", invitorName );
	XP_DEBUG_OUT( "[GLLiveState] Reason  : %s\n", reason );
	XP_DEBUG_OUT( "[GLLiveState] Password: %s\n", password );

	if(m_handler->GetCurState()->State() == DEF::GAME_STATE_LOGIN)
	{
		return;
	}

	if(m_handler->GetCurState()->State() == DEF::GAME_STATE_EDIT)
	{
		m_handler->PopGLLiveState(false);
	}

	if(g_pGLLiveInstance->m_pInviteState == NULL)
	{
		g_pGLLiveInstance->m_pInviteState = XP_API_NEW GLLiveStateChatInvitation(m_handler, this);
		//Register the observer to curstate, the invitation state has not been pushed.
		RegisterOnlineObject();
	}

	((GLLiveStateChatInvitation*)(g_pGLLiveInstance->m_pInviteState))->PushInvitation(room, invitorName, reason, invitor);
}

void GLLiveState::OnChatRoomMessage(const char *room, const char *nickname, const char *message)
{
	GLLiveState *pStateChat = g_pGLLiveInstance->GetChatState();
	if(pStateChat != NULL)
	{
		pStateChat->OnChatRoomMessage(room, nickname, message);
	}
}

#ifdef OS_IPHONE
extern void GLLGetSaveFilePath(char* filePath, const char* filename);
#endif
bool GLLiveState::SaveFileToLocal(char *file, const char *data, int dataLen)
{
	XP_FILE_HANDLE pFile;

	if(data == NULL || dataLen <= 0)
	{
		XP_DEBUG_OUT("[GLLiveState]SaveFile(), Error data!\n");
		return false;
	}

	//Open file
#ifdef OS_IPHONE
	char temp[FILENAME_MAX];
	GLXPlayerChat::ToLower(file, temp);
	char buffer[FILENAME_MAX];
	GLLGetSaveFilePath(buffer, temp);
	pFile = XP_API_FILE_OPEN( buffer, "wb+");	
#else
	char buffer[FILENAME_MAX];
	sprintf(buffer, "%s\\%s", GLLIVE_SAVE_FOLDER, file);
	pFile = XP_API_FILE_OPEN( buffer, "wb");
#endif
	if(!pFile)
	{
		XP_DEBUG_OUT("[GLLiveState]SaveFile(), Error creating file %s \n", file);
		return false;
	}

	XP_API_FILE_WRITE((void*)data,	1, dataLen,		pFile);

	XP_API_FILE_CLOSE(pFile);

	return true;
}


bool GLLiveState::LoadFileFromLocal(char *file, char *& data, int &dataLen)
{
	XP_FILE_HANDLE pFile;

	//Open file
#ifdef OS_IPHONE
	char temp[FILENAME_MAX];
	GLXPlayerChat::ToLower(file, temp);
	char buffer[FILENAME_MAX];
	GLLGetSaveFilePath(buffer, temp);
	pFile = XP_API_FILE_OPEN( buffer, "rb+");	
#else
	char buffer[FILENAME_MAX];
	sprintf(buffer, "%s\\%s", GLLIVE_SAVE_FOLDER, file);
	pFile = XP_API_FILE_OPEN( buffer, "rb");
#endif
	if(!pFile)
	{
		XP_DEBUG_OUT("LoadFile(), Error creating file %s \n", file);
		return false;
	}

	dataLen = XP_API_FILE_GET_LENGTH(pFile);
	data = XP_API_NEW char[dataLen];

	XP_API_FILE_READ((void*)data,	1, dataLen,		pFile);

	XP_API_FILE_CLOSE(pFile);

	return true;
}

CGLImage *GLLiveState::LoadPicFromLocal(const char *picName)
{
	CGLImage *pImage = NULL;
	char *pData = NULL;
	int dataLen = 0;

	if( picName == NULL || XP_API_STRLEN(picName) == 0)
	{
		pImage = XP_API_NEW CGLImage();
		return pImage;
	}

	char pFileName[FILENAME_MAX];
	XP_API_SPRINTF(pFileName, "%s.png", picName);
	if(  LoadFileFromLocal( pFileName, pData, dataLen) )
	{
		pImage = XP_API_NEW CGLImage(pData, dataLen);
		XP_API_DEL(pData);
	}
	return pImage;
}

bool GLLiveState::SavePicToLocal(const char *picName, const char *data, int dataLen)
{	
	if(picName == NULL)
	{
		XP_DEBUG_OUT("[GLLiveState] Save pic failed, for invalid data!\n");
		return false;
	}

	char pFileName[FILENAME_MAX];
	XP_API_SPRINTF(pFileName, "%s.png", picName);
	return SaveFileToLocal(pFileName, data, dataLen);
}

void GLLiveState::OnChatDisconnect(int e)
{
	//GLLiveState *pStateChat = g_pGLLiveInstance->GetChatState();
	//if(pStateChat != NULL)
	//{
	//	pStateChat->OnChatDisconnect(e);
	//}
	if(m_gl_chat && m_gl_chat->GetClient()->streamError() == 3)
	{
		m_bLoginFromOtherPlace = true;
	}
	m_iKeepFailedCount = MAX_KEEP_ALIVE_FAILED - 1;
	OnRequestFailure(FUNCTION_KEEP_ALIVE, 0);
}

void GLLiveState::OnChatRoomPresence(const char *room_jid, const char *user_name, int state)
{
	GLLiveState *pStateChat = g_pGLLiveInstance->GetChatState();
	if(pStateChat != NULL)
	{
		pStateChat->OnChatRoomPresence(room_jid, user_name, state);
	}
}

bool GLLiveState::CheckInput(const char *input)
{
	for(int i = 0; input[i] != '\0';  i++)
	{
		if( !((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= '0' && input[i] <= '9') || (input[i] == '_')) )
		{
			return false;
		}
	}
	return true;
}

bool GLLiveState::CheckEmail(const char *email)
{
	while(*email != '\0')
	{
		if(*email == ' ')
		{
			return false;
		}
		email++;
	}
	return true;
}

const char k_country[][3] = {
	 "US", "AR", "AU", "AT", "BE", "BW", "BR", "BG", "CF", "CM",

	 "CA", "CL", "CO", "HR", "CZ", "DK", "DO", "FI", "FR", "EC",

	 "EG", "SV", "EE", "DE", "GR", "GT", "GN", "HN", "HK", "HU",

	 "IN", "ID", "IE", "IT", "CI", "JM", "JP", "JO", "KE", "LV",

	 "LI", "LT", "LU", "MO", "MK", "MG", "MY", "ML", "MT", "MU",

	 "MX", "MD", "ME", "NL", "NZ", "NI", "NE", "NO", "PA", "PY",

	 "PE", "PH", "PL", "PT", "PR", "RO", "RU", "AS", "SN", "SG",

	 "ZA", "ES", "SI", "SE", "CH", "TW", "TH", "TR", "AE", "GB",

	 "UY", "VG", "VE"
};

int GLLiveState::CountryToIndex(char* country)
{
	int index = 1;
	if(country)
	{
		for(int i = 0; i < 83; i++)
		{
			if(strcmp(country, k_country[i]) == 0)
			{
				index = i + 1;
				break;
			}
		}
	}
	return index;
}

const char* GLLiveState::IndexToCountry(int index)
{
	if(index > 0 && index <= 83)
		return k_country[index - 1];
	else
		return "";
}