#include "GLLiveStdafx.h"
#include "GLLiveGame.h"
#include "Image.h"
#include "GLLive_Stream.h"
#include "MyDisplay.h"
#include "MyGraphics.h"
#include "GLLive_ResFile.h"
#include "GLLive_Font.h"
#include "ResourceManager.h"

#include "TouchEvent.h"
#include "GLLiveDEF.h"
#include "Porting.h"
#include "GLLiveStateLogin.h"
#include "GLLiveStateChat.h"
#include "GLLiveStateMenu.h"

#ifdef OS_WIN32
#include "Win32Socket.h"
#else
#include "IPhoneSocket.h"
#endif

#define TROPHY_SAVE_FILE   "trophy.sav"
#define TROPHY_DATA_HEAD   "_Trophy_Header_"
#define TROPHY_DATA_LENGTH   1024

#if PLATFORM == PLATFORM_MAC_OS_X
#include <OpenGLES/ES1/gl.h>
#elif IPHONE > 0
#include "gl.h"
#else
#include "gl.h"
#endif

#include "glt_math_alg.h"



#if (PLATFORM==PLATFORM_MAC_OS_X)
#include <sys/time.h>
#include <sys/stat.h>
#endif

const int szFontCharSpace[] =
{
	-2,
	0,
	0,
	0,
	0,
	0,
	0,
	-4,
	0,
	0,
	0,
	-3,
	0,
	0,
};

#define BACKGROUND_COLOR 0xFF000000 //0xFF00FFFF
#define BUTTON_COLOR 0xFF0000FF

#ifdef WIN32

u32	g_screenWidth = SCREEN_WIDTH;
u32	g_screenHeight= SCREEN_HEIGHT;
u32	g_screenZoom = 1;
u32 g_device = 0;
int g_screenAngle = 0;
HWND g_hWnd = NULL; //...czd


int GLLiveShowMessageBox(const char* message, const char* code, const char* file, const long line)
{
	char szTemp[256];
	
	sprintf(szTemp, "Code:\t%s\nFile:\t%s\nLine:\t%d\nContinue?", code, file, line);
	
	//int ret = MessageBoxA(g_hWnd, szTemp, message, MB_YESNO);

	printf(szTemp);

	return /*ret != */ IDYES;
}
#else

extern void setStatusBarOrientation();
extern void resetStatusBarOrientation();
extern void DisplayIndicator(void* window, bool flag);
extern void checkLocal();
#endif
CGLLive * g_pGLLiveInstance = NULL;

CGLLive::CGLLive(void* app, void* window, void* view, int lang, char* deviceToken, bool canEnterLeaderboard)
: m_pDisplay(NULL)
, m_pGraphics(NULL)
, m_nScreenWidth(kScreenSize[0][0])
, m_nScreenHeight(kScreenSize[0][1])
, m_bGLInitialized(false)
, m_LastTick(0)
, m_frameCounter(0)
, m_fps(0)
, m_bIsTerminal(false)
, m_pResFile(NULL)
, m_pResManager(NULL)
, m_bShowFps(true)
, m_pSaveFile(NULL)
, m_pTouchpad(NULL)
, m_gameSSTop(0)
, m_bPopState(false)
, m_pInviteState(NULL)
, m_pDiconnectState(NULL)
{
	g_pGLLiveInstance = this;
	m_app = app;
	m_window = window;
	m_view = view;
	m_Language = lang;
	m_deviceToken = deviceToken;
	m_canEnterLeaderboard = canEnterLeaderboard;
	m_enableInviteToPlay = true;
	m_dropGameModeChooseScreen = false;
	
	m_mouseX = 0;
	m_mouseY = 0;
	m_bMousePressed = false;
	m_AD_file = NULL;
	
	m_imgNum = 0;
	for(int i = 0; i < NUM_OF_SPRITES; i++)
	{
		m_pSprites[i] = NULL;
	}

	InitGLLiveStateStack();

#ifdef OS_WIN32
	CWin32Socket::Startup();
#else
	CIPhoneSocket::Startup();
	setStatusBarOrientation();
	DisplayIndicator(m_window, true);
	checkLocal();
	
	// initialize game
	srand(time(NULL));
#endif
}
extern CGLImage * g_pAvatarImage;
CGLLive::~CGLLive(void)
{
	SAFE_DELETE(g_pAvatarImage);
	SAFE_DELETE(m_AD_file);

	SAFE_DELETE(m_pResFile);
	SAFE_DELETE(m_pResManager);

	SAFE_DELETE(m_pDisplay);
	SAFE_DELETE(m_pGraphics);

	for (int i = 0; i < NUM_OF_FONTS; i++) 
	{
		SAFE_DELETE(m_pFont[i]);
	}
	SAFE_DELETE(m_pSaveFile);

	SAFE_DELETE(m_pTouchpad);
	
	SAFE_DELETE(pStringManager);

	for(int i = 0; i < NUM_OF_SPRITES; i++)
	{
		SAFE_DELETE( m_pSprites[i] );
	}

	for(int i = 0; i < MAX_GAME_STATE_STACK; i++)
	{
		SAFE_DELETE(m_GLLiveStateStack[i]);
	}
	
	SAFE_DELETE(m_pInviteState);
	SAFE_DELETE(m_pDiconnectState);

#ifdef OS_WIN32
	CWin32Socket::Clearup();
#else
	CIPhoneSocket::Clearup();
	resetStatusBarOrientation();
#endif
}

void CGLLive::InitFont()
{
	char temp[10] = {0};
	for (int i = 0; i < NUM_OF_FONTS; i++)
	{
		m_pFont[i] = NEW CGLLiveFont();
		XP_API_MEMSET(temp, 0, sizeof(temp));
		XP_API_SPRINTF(temp, "font%d", i + 1);
		m_pFont[i]->Load(temp, "font");
		m_pFont[i]->SetCharSpacing(szFontCharSpace[i]);
	}
}

#if PLATFORM==PLATFORM_MAC_OS_X
extern void InitMacPathForGLL();
extern void GLLGetUDID(char* udid);
#endif

void CGLLive::Initialize()
{

#if (PLATFORM==PLATFORM_WIN32)
	XP_DEBUG_INIT(); //open console to show info
	unsigned int curTick = GetTickCount();
	char cmd[128];
	sprintf(cmd, "if not exist %s md %s", GLLIVE_SAVE_FOLDER, GLLIVE_SAVE_FOLDER);
	system(cmd);
	m_UDID[0] = 0;
#elif (PLATFORM==PLATFORM_MAC_OS_X)
	//unsigned int curTick = clock();
	InitMacPathForGLL();
	GLLGetUDID(m_UDID);
#ifdef MAC_SAVE_FOLDER
	//Create gllive data dir
	extern char g_gllSavePath[512];
	XP_API_STRCAT(g_gllSavePath, GLLIVE_SAVE_FOLDER);
	//Check the dir exist
	if(access(g_gllSavePath, F_OK) != 0)
	{
		if(mkdir(g_gllSavePath, S_IRWXU) != 0)
		{
			printf("[CGLLive::Initialize]Create data dir %s failed!\n", GLLIVE_SAVE_FOLDER);
		}
	}
	//Add /
	XP_API_STRCAT(g_gllSavePath, "/");
#endif
#endif

	m_pDisplay = NEW CGLLiveDisplay();
	m_pDisplay->InitializeScreen();

	m_pGraphics = NEW CGLLiveGraphicsGL;

#ifdef WIN32
	UpdateOrientation();
#else
	m_pDisplay->SetDeviceOrientation(0);
	UpdateOrientation();
#endif

	m_pResManager = NEW CGLLiveResourceManager();
	m_pResFile = NEW CGLLiveResFile();
	if(!m_pResFile->Open(FONT_PAK_FILENAME))
	{
		m_pResFile->Close();
	}
	if(m_pResFile != NULL)
	{
		m_pResManager->AddResFile(m_pResFile);
	}
	InitFont();
	m_pResManager->RemoveResFile(m_pResFile);
	m_pResFile->Close();

	if(!m_pResFile->Open(RES_PAK_FILENAME))
	{
		SAFE_DELETE(m_pResFile);
	}
	if(m_pResFile != NULL)
	{
		m_pResManager->AddResFile(m_pResFile);
	}

	m_pTouchpad = NEW CGLLiveTouchpad();
	//load sprite
	for(int i = 0; i < NUM_OF_SPRITES; i++)
	{
		m_pSprites[i] = NEW CGLLiveASprite(k_sprites_name[i]);
	}

	//String manager
	pStringManager = NEW StringManager();
	CGLLiveResFile*	_pStrFile = NEW CGLLiveResFile();
	if(!_pStrFile->Open(STR_PAK_FILENAME))
	{
		SAFE_DELETE(_pStrFile);
	}
	if(_pStrFile != NULL)
	{
		m_pResManager->AddResFile(_pStrFile);
	}
	pStringManager->load();
	m_pResManager->RemoveResFile(_pStrFile);
	SAFE_DELETE(_pStrFile);

#ifndef WIN32
	DisplayIndicator(m_window, false);
#endif
	//Init as main menu state
	PushGLLiveState(new GLLiveStateLogin(this));
}

extern void updateDeviceOrientation();
bool CGLLive::Update()
{
	if(!m_pGraphics)
		Initialize();
	//m_timerCount = SDL_GetTicks();
	_TOUCH_EVENT event;
	event.eType = TET_Idle;
	event.isClicked = false;
	event.position.x = 0;
	event.position.y = 0;

	//updateDeviceOrientation();
	m_pTouchpad->Update(m_mouseX, m_mouseY, m_bMousePressed);
	m_pTouchpad->SetTouchEvent(event);

	//for exit
	//_TouchRect oExitRect;
	//oExitRect.left = 350;
	//oExitRect.top = 280;
	//oExitRect.width = 120;
	//oExitRect.height = 20;

	//if(m_pTouchpad->IsClicked() 
	//	&& oExitRect.Contain(m_pTouchpad->GetPosition())
	//	)
	//{
	//	TerminateGame();
	//}

	if(m_bPopState)
	{
		PopGLLiveStateAction();
		if(m_pInviteState != NULL)
		{
			m_pInviteState->SetLastState(GetCurState());
		}
	}

	if(GetCurState() == NULL)
	{
		TerminateGame();
		//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		XP_DEBUG_FINALIZE();
		return m_bIsTerminal;
	}

	if(m_pInviteState != NULL && GetCurState()->State() != DEF::GAME_STATE_CHAT_INVITATION && !GetCurState()->IsProcess())
	{
		PushGLLiveState(m_pInviteState);
		m_pInviteState->CancelRequest();
		m_pInviteState->RegisterOnlineObject();
		//m_pInviteState will be set to NULL in GLLiveStateChatInvitation's destroyer
	}
	
	if(m_pDiconnectState != NULL && GetCurState()->State() != DEF::GAME_STATE_DISCONNECT)
	{
		PushGLLiveState(m_pDiconnectState);
		m_pDiconnectState = NULL;
	}

	if(GetCurState() != NULL)
	{
		GetCurState()->UpdateAnimation();
	}

	if(GetCurState() != NULL)
	{
		GetCurState()->HandleEvent(event);
	}

	//if( (SDL_GetTicks() - m_timerCount) < 1000 / m_framesPerSecond )
	//{
	//	SDL_Delay( ( 1000 / m_framesPerSecond ) - (SDL_GetTicks() - m_timerCount) );
	//}
	Paint();
	return m_bIsTerminal;
}

unsigned int CGLLive::GetCurMTime()
{
#if (PLATFORM==PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE)
	return GetTickCount();
#elif (PLATFORM == PLATFORM_MAC_OS_X)
	timeval t;
	gettimeofday(&t, NULL);
	return (t.tv_sec& 0xffff) * 1000 + t.tv_usec / 1000;
#endif
}

void CGLLive::Paint()
{
	unsigned int curTick = 0;

	if(m_bShowFps)
		curTick = GetCurMTime();
	CGLLiveGraphics * g = GetGraphics();

	g->BeginDraw2D(320, 480);
	g->ResetClip();

	////draw something
	//g->SetClip(0, 0, 480, 320);
	//g->SetARGBColor(BACKGROUND_COLOR);
	//g->FillRect(0, 0, 480, 320);

	//char szStr[256] = {0};
	//sprintf(szStr, "FPS:%d", m_fps);
	//GetFont(FS_Large)->SetFontColor(0);
	//GetFont(FS_Large)->DrawString(g, szStr, 160, 30, CGLLiveGraphics::HCENTER);

	//sprintf(szStr, "X = %d, Y = %d", m_pTouchpad->GetPosition().x, m_pTouchpad->GetPosition().y);
	//GetFont(FS_Large)->SetFontColor(2);
	//GetFont(FS_Large)->DrawString(g, szStr, 20, 80, 0);

	//g->SetARGBColor(0xFFFFFFFF);
	//GetFont(FS_Large)->SetFontColor(0);
	//GetFont(FS_Large)->DrawString(g, "EXIT", 350 + m_pSprites[1]->GetFrameWidth(2) / 2, 285, CGLLiveGraphics::HCENTER);
	
	if(GetCurState() != NULL)
	{
		GetCurState()->Paint();
	}


	g->EndDraw2D();

	if(m_bShowFps)
	{
		if(m_LastTick + 1000 < curTick)
		{
			m_LastTick = curTick;
			m_fps = m_frameCounter;
			m_frameCounter = 0;
		}
		else
		{
			m_frameCounter++;
		}
	}
}

void CGLLive::handleTouch(float x, float y, bool pressed)
{
	m_mouseX = x;
	m_mouseY = y;
	m_bMousePressed = pressed;
}

#ifdef WIN32
void CGLLive::handleTouch(float x, float y)
{
	m_mouseX = x;
	m_mouseY = y;
}
#endif

extern void gllSetOrientation(int type);
void CGLLive::SetOrientation(int type)
{
	gllSetOrientation(type);
}

void CGLLive::SetInviteToPlay(bool enable)
{
	m_enableInviteToPlay = enable;
}

void CGLLive::DropGameModeChooseScreen()
{
	m_dropGameModeChooseScreen = true;
}

void CGLLive::UpdateOrientation()
{
	SetupScreen(m_pDisplay->GetDisplayMode());
}


void CGLLive::SetupScreen(DisplayMode mode)
{
	m_nScreenWidth = kScreenSize[mode][0];
	m_nScreenHeight = kScreenSize[mode][1];

#if IPHONE
	CGLLiveMutableImage* pScreenImage = NULL;
#else
	CGLLiveMutableImage* pScreenImage = m_pDisplay->GetScreenImage(mode & 0x1);
	pScreenImage->SetSize(m_nScreenWidth, m_nScreenHeight, m_nScreenWidth * sizeof(NativePixel));
	m_pGraphics->SetImage(pScreenImage);
#endif
	
	m_camera.Init(this, false);
	m_gl.SetCamera(&m_camera);
	m_gl.InvertAspect(this, pScreenImage);

	
	m_gl.InvertAspect(this, pScreenImage);
}

int CGLLive::Random(int nMin,int nMax)
{
	return Math::Random(nMin, nMax);
}

int CGLLive::Random(int nMax)
{
	return Math::Random(0, nMax);
}

DisplayMode CGLLive::GetCurrentDisplayMode()
{
	return m_pDisplay->GetDisplayMode();
}


void CGLLive::SetupLandscape(int orientation)
{
	GetDisplay()->SetDeviceOrientation(orientation);
	UpdateOrientation();
}

bool CGLLive::PushGLLiveState(GLLiveState *pState)
{
	if(m_gameSSTop + 1 < MAX_GAME_STATE_STACK)
	{
		m_gameSSTop++;
		m_GLLiveStateStack[m_gameSSTop] = pState;
		return true;
	}
	else
	{
		printf("Too more game state! Push failed!\n");
		return false;
	}
}

bool CGLLive::PopGLLiveState(bool callSwitch)
{
	if(m_gameSSTop - 1 >= 0)
	{
		m_bPopState = true;
		m_gameSSTop--;
		if(GetCurState() != NULL)
		{
			GetCurState()->CancelRequest();
			GetCurState()->RegisterOnlineObject();
			if(callSwitch)
			{
				GetCurState()->SwitchState();
			}
		}
		return true;
	}
	return false;
}


GLLiveState *CGLLive::GetChatState()
{
	for(int i = 0; i < MAX_GAME_STATE_STACK; i++)
	{
		if( m_GLLiveStateStack[i] != NULL && m_GLLiveStateStack[i]->State() == DEF::GAME_STATE_CHAT )
		{
			return m_GLLiveStateStack[i];
		}
	}
	return NULL;
}

bool CGLLive::PopToStateChat()
{
	if(GetChatState() != NULL)
	{
		while(m_GLLiveStateStack[m_gameSSTop]->State() != DEF::GAME_STATE_CHAT)
		{
			SAFE_DELETE(m_GLLiveStateStack[m_gameSSTop]);
			m_gameSSTop--;		
		}
		//Change online object observer to current GLLiveState
		if(GetCurState() != NULL)
		{
			GetCurState()->CancelRequest();
			GetCurState()->RegisterOnlineObject();
		}	
		return true;
	}
	else
	{
		return false;
	}
}

void CGLLive::PopGLLiveStateAction()
{
	for(int i = m_gameSSTop + 1; i < MAX_GAME_STATE_STACK; i++)
	{
		SAFE_DELETE(m_GLLiveStateStack[i]);
	}
	m_bPopState = false;
}

void CGLLive::InitGLLiveStateStack()
{
	m_GLLiveStateStack[0] = NULL;
	for(int i = 0; i < MAX_GAME_STATE_STACK; i++)
	{
		m_GLLiveStateStack[i] = NULL;
	}
	m_gameSSTop = 0;
}

void CGLLive::ClearScreen()
{
	CGLLiveGraphics * g = GetGraphics();

	g->SetClip(0, 0, 480, 320);
	g->SetARGBColor(BACKGROUND_COLOR);
	g->FillRect(0, 0, 480, 320);
}

void CGLLive::DrawImage(CGLLiveImage *pSrc, int x, int y, int anchor)
{
	CGLLiveGraphics * g = GetGraphics();
	g->DrawImage(pSrc, x, y, anchor);
}

void CGLLive::DrawButton(_TouchRect button)
{
	CGLLiveGraphics * g = GetGraphics();
	
	g->SetARGBColor(BUTTON_COLOR);
	g->FillRect(button.left, button.top, button.width, button.height);
}

extern void GLLGetSaveFilePath(char *file, const char *);
bool CGLLive::NotifyTrophy(int trophyIndex)
{
	XP_FILE_HANDLE pFileRead;
	XP_FILE_HANDLE pFileWrite;

	if(trophyIndex < 0 || trophyIndex > TROPHY_DATA_LENGTH)
	{
		return false;
	}

	char *trophyData = XP_API_NEW char[TROPHY_DATA_LENGTH];
	XP_API_MEMSET(trophyData, 0,TROPHY_DATA_LENGTH); 

	//Generate save file name
	char trophyFile[128];
	XP_API_SPRINTF(trophyFile, TROPHY_SAVE_FILE);
	
#ifdef OS_IPHONE
	char fileName[FILENAME_MAX];
	GLLGetSaveFilePath(fileName , trophyFile);
#else
	char fileName[FILENAME_MAX];
	sprintf(fileName, "%s\\%s", GLLIVE_SAVE_FOLDER, trophyFile);
#endif

	pFileRead = XP_API_FILE_OPEN(fileName, "rb");
	if(pFileRead != NULL)
	{
		XP_API_FILE_READ((void*)trophyData,	1, TROPHY_DATA_LENGTH,		pFileRead);
		XP_API_FILE_CLOSE(pFileRead);
	}
	else
	{
		XP_DEBUG_OUT("[NotifyTrophy] No trophy profile, Create a new profile!\n");
	}

	//Check header
	int headLen = XP_API_STRLEN(TROPHY_DATA_HEAD);
	if(XP_API_STRNCMP(trophyData, TROPHY_DATA_HEAD, headLen) != 0)
	{
		XP_DEBUG_OUT("[NotifyTrophy] Header check failed! Clear ALL RECORD!\n");
		XP_API_MEMSET(trophyData, 0,TROPHY_DATA_LENGTH);
		XP_API_MEMCPY(trophyData, TROPHY_DATA_HEAD, headLen);
		//Set to no binding
		int nullUserNo = -1;
		XP_API_MEMCPY(trophyData + headLen, &nullUserNo, sizeof(int));
	}
	
	//Save to data buffer
	trophyData[headLen + sizeof(int) + trophyIndex] = true;

	//Save to file
	pFileWrite = XP_API_FILE_OPEN(fileName, "wb");
	if(!pFileWrite)
	{
		XP_DEBUG_OUT("SaveTrophyData(), Error creating file %s \n", TROPHY_SAVE_FILE);
		XP_API_DELETE_ARRAY(trophyData);
		return false;
	}
	XP_API_FILE_WRITE(trophyData, 1, TROPHY_DATA_LENGTH, pFileWrite);

	XP_API_FILE_CLOSE(pFileWrite);
	XP_API_DELETE_ARRAY(trophyData);
	return true;
}

bool* CGLLive::LoadTrophyData(int userno)
{
//	XP_FILE_HANDLE pFileRead;
//
//	//Generate save file name
//	char trophyFile[128];
//	XP_API_SPRINTF(trophyFile, TROPHY_SAVE_FILE);
//
//#ifdef OS_IPHONE
//	char fileName[FILENAME_MAX];
//	GetSaveFilePath(fileName , trophyFile);
//#else
//	char *fileName = trophyFile;
//#endif
//
//	pFileRead = XP_API_FILE_OPEN(fileName, "rb");
//	
	
	XP_FILE_HANDLE pFileRead;

	//Generate save file name
	char trophyFile[128];
	XP_API_SPRINTF(trophyFile, TROPHY_SAVE_FILE);
	
#ifdef OS_IPHONE
	char fileName[FILENAME_MAX];
	GLLGetSaveFilePath(fileName , trophyFile);
#else
	char fileName[FILENAME_MAX];
	sprintf(fileName, "%s\\%s", GLLIVE_SAVE_FOLDER, trophyFile);
#endif
	
	pFileRead = XP_API_FILE_OPEN(fileName, "rb");
	
	
	if(!pFileRead)
	{
		return NULL;
	}

	char *trophyData = XP_API_NEW char[TROPHY_DATA_LENGTH];
	XP_API_MEMSET(trophyData, 0,TROPHY_DATA_LENGTH); 
	XP_API_FILE_READ((void*)trophyData,	1, TROPHY_DATA_LENGTH,		pFileRead);
	XP_API_FILE_CLOSE(pFileRead);

	//Check header
	int headLen = XP_API_STRLEN(TROPHY_DATA_HEAD);
	if(XP_API_STRNCMP(trophyData, TROPHY_DATA_HEAD, headLen) != 0)
	{
		XP_API_DELETE_ARRAY(trophyData);
		return NULL;
	}

	//Check binding
	if( *((int*)(trophyData + headLen)) != userno)
	{
		XP_API_DELETE_ARRAY(trophyData);
		return NULL;
	}

	bool *trophy = XP_API_NEW bool[TROPHY_DATA_LENGTH - headLen - sizeof(int)];
	XP_API_MEMCPY(trophy, trophyData + headLen + sizeof(int), TROPHY_DATA_LENGTH - headLen - sizeof(int));
	XP_API_DELETE_ARRAY(trophyData);
	return trophy;
}

bool CGLLive::BindTrophyAccount(int userno)
{
	XP_FILE_HANDLE pReadFile;
	XP_FILE_HANDLE pFile;
	//Generate save file name
	char trophyFile[128];
	char trophyData[TROPHY_DATA_LENGTH];
	XP_API_SPRINTF(trophyFile, TROPHY_SAVE_FILE);
	XP_API_MEMSET(trophyData, 0, sizeof(trophyData));

#ifdef OS_IPHONE
	char fileName[FILENAME_MAX];
	GLLGetSaveFilePath(fileName , trophyFile);
#else
	char fileName[FILENAME_MAX];
	sprintf(fileName, "%s\\%s", GLLIVE_SAVE_FOLDER, trophyFile);	
#endif
	
	pReadFile = XP_API_FILE_OPEN(fileName, "rb");
	if(pReadFile != NULL)
	{
		XP_API_FILE_READ(trophyData, 1, TROPHY_DATA_LENGTH, pReadFile);
		int headLen = XP_API_STRLEN(TROPHY_DATA_HEAD);
		//Check binding
		if( XP_API_STRNCMP(trophyData, TROPHY_DATA_HEAD, headLen) == 0 && *((int*)(trophyData + headLen)) != -1)
		{
			//Clear record if has binded to another account
			XP_API_MEMSET(trophyData + headLen + sizeof(int), 0, TROPHY_DATA_LENGTH - headLen - sizeof(int));
		}
	}
	

	pFile = XP_API_FILE_OPEN(fileName, "wb");
	if( pFile == NULL)
	{
		XP_DEBUG_OUT("BindTrophyAccount(), Error creating file %s \n", trophyFile);
	}

	int headLen = XP_API_STRLEN(TROPHY_DATA_HEAD);

	XP_API_MEMCPY(trophyData, TROPHY_DATA_HEAD, headLen);
	XP_API_MEMCPY(trophyData + headLen, &userno, sizeof(userno));

	XP_API_FILE_WRITE(trophyData, 1, TROPHY_DATA_LENGTH, pFile);
	XP_API_FILE_CLOSE(pFile);

	return true;
}

int CGLLive::ReadTrophyAccount()
{
	XP_FILE_HANDLE pFile;
	char head[128];
	int  bindUserNo = 0;
	//Generate save file name
	char trophyFile[128];
	XP_API_SPRINTF(trophyFile, TROPHY_SAVE_FILE);

#ifdef OS_IPHONE
	char fileName[FILENAME_MAX];
	GLLGetSaveFilePath(fileName , trophyFile);
#else
	char fileName[FILENAME_MAX];
	sprintf(fileName, "%s\\%s", GLLIVE_SAVE_FOLDER, trophyFile);	
#endif

	pFile = XP_API_FILE_OPEN(fileName, "rb");
	if( pFile == NULL)
	{
		return -1;
	}

	XP_API_FILE_READ(head, 1, XP_API_STRLEN(TROPHY_DATA_HEAD), pFile);
	XP_API_FILE_READ(&bindUserNo, 1, sizeof(int), pFile);
	XP_API_FILE_CLOSE(pFile);

	return bindUserNo;
}


bool CGLLive::ReadUserInfo(char *username, int nameLen, char *password, int pwdLen, int *timestamp)
{
	XP_FILE_HANDLE pFile;

	//Check
	if(username == NULL || nameLen == 0 || password == NULL || pwdLen == 0)
	{
		return false;
	}

	//Open file
#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GLLGetSaveFilePath(buffer, PROFILE_FILE_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "rb" );	
#else
	char buffer[FILENAME_MAX];
	sprintf(buffer, "%s\\%s", GLLIVE_SAVE_FOLDER, PROFILE_FILE_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "rb" );	
#endif
		
	if(!pFile)
	{
		XP_DEBUG_OUT("LoadProfile(), File %s not found \n", PROFILE_FILE_NAME);
		return false;
	}

	//Check head
	int headLen = XP_API_STRLEN( PROFILE_FILE_HEAD ) + 1;
	int fileLen = XP_API_FILE_GET_LENGTH(pFile);
	char *tmpHead = NULL;
	if(fileLen <= headLen )
	{
		XP_API_FILE_CLOSE(pFile);
		return false;
	}
	tmpHead = XP_API_NEW char[headLen];
	XP_API_FILE_READ( tmpHead, 1, headLen, pFile );
	if(XP_API_STRCMP(tmpHead, PROFILE_FILE_HEAD) != 0)
	{
		XP_API_DELETE_ARRAY(tmpHead);
		XP_API_FILE_CLOSE(pFile);
		return false;
	}
	
	//Temp buffer
	char tmpUsername[MAX_USERNAME];
	char tmpPassword[MAX_PASSWORD];
	bool tmpAutoLogin = false;
	
	XP_API_MEMSET(username, 0, nameLen);
	XP_API_MEMSET(password, 0, pwdLen);
	XP_API_MEMSET(tmpUsername, 0, sizeof(tmpUsername));
	XP_API_MEMSET(tmpPassword, 0, sizeof(tmpPassword));

	//Read to temp buffer from config file
	XP_API_FILE_READ((void*)&tmpAutoLogin,	1, sizeof(bool),	pFile);
	XP_API_FILE_READ((void*)tmpUsername,	1, MAX_USERNAME,	pFile);
	XP_API_FILE_READ((void*)tmpPassword,	1, MAX_PASSWORD,	pFile);	

	if(timestamp != NULL)
	{
		if(fileLen > headLen + sizeof(bool) + MAX_USERNAME + MAX_PASSWORD )
		{
			XP_API_FILE_READ((void*)timestamp,	1, sizeof(int),	pFile);	
//			char tmp[4];
//			memset(tmp, 0, 4);
//			XP_API_FILE_READ((void*)tmp,	1, sizeof(int),	pFile);	
//			int a = 0;
		}
		else
		{
			*timestamp = 0;
		}
	}

	//Copy to target from temp
	XP_API_MEMCPY(username, tmpUsername, MIN(nameLen - 1, XP_API_STRLEN(tmpUsername)));
	if(tmpAutoLogin)
	{
		XP_API_MEMCPY(password, tmpPassword,  MIN(pwdLen - 1, XP_API_STRLEN(tmpPassword)));
	}

	XP_API_DELETE_ARRAY(tmpHead);
	XP_API_FILE_CLOSE(pFile);
	return true;

}

bool CGLLive::SaveUserInfo(bool autoLogin, const char *username, const char *password, int timestamp)
{
	XP_FILE_HANDLE pFile;
	
#ifdef OS_IPHONE
	char buffer[FILENAME_MAX];
	GLLGetSaveFilePath(buffer, PROFILE_FILE_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "wb+" );	
#else
	char buffer[FILENAME_MAX];
	sprintf(buffer, "%s\\%s", GLLIVE_SAVE_FOLDER, PROFILE_FILE_NAME);
	pFile = XP_API_FILE_OPEN( buffer, "wb" );	
#endif

	if(!pFile)
	{
		XP_DEBUG_OUT("SaveProfile(), Error   file %s \n", PROFILE_FILE_NAME);
		return false;
	}

	signed   int nWritten;
	nWritten = XP_API_FILE_WRITE((void*)PROFILE_FILE_HEAD, 1, XP_API_STRLEN(PROFILE_FILE_HEAD) + 1, pFile);
	nWritten = XP_API_FILE_WRITE((void*)&autoLogin,1, sizeof(autoLogin),	pFile);
	nWritten = XP_API_FILE_WRITE((void*)username,	1, MAX_USERNAME,		pFile);
	if(autoLogin)
	{
		nWritten = XP_API_FILE_WRITE((void*)password,	1, MAX_PASSWORD,		pFile);
	}
	else
	{
		char pNullPwd[MAX_PASSWORD];
		XP_API_MEMSET(pNullPwd, 0, sizeof(pNullPwd));
		nWritten = XP_API_FILE_WRITE((void*)pNullPwd,	1, MAX_PASSWORD,		pFile);
	}
	if( timestamp > 0)
	{
		nWritten = XP_API_FILE_WRITE((void*)&timestamp,1, sizeof(timestamp),	pFile);
	}

	XP_API_FILE_CLOSE(pFile);
	return true;
}