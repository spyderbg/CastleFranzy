#include "stdafx.h"
#include "Game.h"
#include "Image.h"
#include "Stream.h"
#include "MyDisplay.h"
#include "MyGraphics.h"
#include "ResFile.h"
#include "Font.h"
#include "ResourceManager.h"

#include "TouchEvent.h"
#include "DEF.h"
#include "../../include/Porting.h"

#ifdef OS_WIN32
#include "../../include/Win32Socket.h"
#else
#include "../../include/IPhoneSocket.h"
#endif

#define RECORD_FILENAME  "rfbsav"
#define LRECORD_FILENAME  L"rfbsav"

#if PLATFORM == PLATFORM_MAC_OS_X
#include <OpenGLES/ES1/gl.h>
#elif IPHONE > 0
#include "../tools/rasteroid3.1/include/gles/gl.h"
#else
#include "gl.h"
#endif

#include "glt_math_alg.h"


#if (PLATFORM==PLATFORM_MAC_OS_X)
#include <sys/time.h>
#endif


const char szFontFileList[][20] =
{
	"font_12",
	"font_15",
	"font_20",
	"font_25",
};

#define BACKGROUND_COLOR 0xFF000000 //0xFF00FFFF
#define BUTTON_COLOR 0xFF0000FF
#define RECT_COLOR   0xFFFF0000

CGame::CGame(void)
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
, m_gameState(NULL)
, m_currState(DEF::GAME_STATE_NULL)
, m_nextState(DEF::GAME_STATE_NULL)
{
	m_pSprites[0] = NULL;
	m_pSprites[1] = NULL;

#ifdef OS_WIN32
	CWin32Socket::Startup();
#else
	CIPhoneSocket::Startup();
#endif
}

CGame::~CGame(void)
{
	SAFE_DELETE(m_pResFile);
	SAFE_DELETE(m_pResManager);

	SAFE_DELETE(m_pDisplay);
	SAFE_DELETE(m_pGraphics);

	for (int i = 0; i < CFont::FS_Count; i++) 
	{
		SAFE_DELETE(m_pFont[i]);
	}
	SAFE_DELETE(m_pSaveFile);

	SAFE_DELETE(m_pTouchpad);

	SAFE_DELETE(m_gameState);

	SAFE_DELETE( m_pSprites[0] );
	SAFE_DELETE( m_pSprites[1] );

#ifdef OS_WIN32
	CWin32Socket::Clearup();
#else
	CIPhoneSocket::Clearup();
#endif
}

void CGame::InitFont()
{
	for (int i = 0; i < CFont::FS_Count; i++)
	{
		m_pFont[i] = NEW CFont();
		m_pFont[i]->Load(szFontFileList[i], 1);
	}
}

#if PLATFORM==PLATFORM_MAC_OS_X
extern void InitMacSavePath();
#endif

void CGame::Initialize()
{

#if (PLATFORM==PLATFORM_WIN32)
	unsigned int curTick = GetTickCount();
#elif (PLATFORM==PLATFORM_MAC_OS_X)
	unsigned int curTick = clock();
#endif

	m_pResFile = NEW CResFile();

	if(!m_pResFile->Open(RES_PAK_FILENAME))
	{
		SAFE_DELETE(m_pResFile);
	}

	m_pResManager = NEW CResourceManager();

	if(m_pResFile != NULL)
	{
		m_pResManager->AddResFile(m_pResFile);
	}

	m_pDisplay = NEW CDisplay();
	m_pDisplay->InitializeScreen();

	m_pGraphics = NEW CGraphicsGL;

#if PLATFORM == PLATFORM_WIN32
	UpdateOrientation();
#endif

	InitFont();

#if PLATFORM==PLATFORM_MAC_OS_X
	InitMacSavePath();
#endif
	m_pSaveFile = NEW CFileStream();
	if(!m_pSaveFile->OpenMacOtherPath(RECORD_FILENAME))
	{
		m_pSaveFile->OpenMacOtherPath(RECORD_FILENAME, CRawFile::MODE_CREATE);
	}
	m_pSaveFile->Close();

	m_pTouchpad = NEW CTouchpad();
	//load sprite
	m_pSprites[0] = NEW ASprite("PadButton");
	m_pSprites[1] = NEW ASprite("menu_bar");

	m_currState = DEF::GAME_STATE_LOGO;
	m_gameState = GameStateFactory::CreateGameState( this,DEF::GAME_STATE_LOGO );

	XP_DEBUG_INIT(); //open console to show info
}

void CGame::Update()
{
	//m_timerCount = SDL_GetTicks();
	_TOUCH_EVENT event;
	event.isClicked = false;
	event.position.x = 0;
	event.position.y = 0;

	m_pTouchpad->Update();
	if(m_pTouchpad->IsClicked())
	{
		m_pTouchpad->SetTouchEvent(event);
	}

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

	if(m_currState == DEF::GAME_STATE_EXIT)
	{
		TerminateGame();
		XP_DEBUG_FINALIZE();
	}

	m_gameState->SwitchState();

	ChangeState();

	if(m_gameState == NULL)
		return;

	m_gameState->UpdateAnimation();

	m_gameState->HandleEvent(event);

	//if( (SDL_GetTicks() - m_timerCount) < 1000 / m_framesPerSecond )
	//{
	//	SDL_Delay( ( 1000 / m_framesPerSecond ) - (SDL_GetTicks() - m_timerCount) );
	//}
}

bool CGame::UpdateQuit()
{
	if(m_gameState == NULL)
		return true;

	return m_gameState->UpdateQuitAnimation();
}

void CGame::BeginQuit()
{
	if(m_gameState == NULL)
		return;

	m_gameState->BeginQuitAnimation();
}

unsigned int CGame::GetCurMTime()
{
#if (PLATFORM==PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE)
	return GetTickCount();
#elif (PLATFORM == PLATFORM_MAC_OS_X)
	timeval t;
	gettimeofday(&t, NULL);
	return (t.tv_sec& 0xffff) * 1000 + t.tv_usec / 1000;
#endif
}

void CGame::Paint()
{
	unsigned int curTick = 0;

	if(m_bShowFps)
		curTick = GetCurMTime();
	CGraphics * g = GetGraphics();

	g->BeginDraw2D(320, 480);

	////draw something
	//g->SetClip(0, 0, 480, 320);
	//g->SetARGBColor(BACKGROUND_COLOR);
	//g->FillRect(0, 0, 480, 320);


	//g->SetARGBColor(0xFFFFFFFF);
	//m_pSprites[1]->PaintFrame(g, 0, 20, 20, 0, 0, 0);


	//char szStr[256] = {0};
	//sprintf(szStr, "FPS:%d", m_fps);
	//GetFont(CFont::FS_Large)->SetFontColor(0);
	//GetFont(CFont::FS_Large)->DrawString(g, szStr, 160, 30, CGraphics::HCENTER);


	//
	//sprintf(szStr, "X = %d, Y = %d", m_pTouchpad->GetPosition().x, m_pTouchpad->GetPosition().y);
	//GetFont(CFont::FS_Large)->SetFontColor(2);
	//GetFont(CFont::FS_Large)->DrawString(g, szStr, 20, 80, 0);

	//g->SetARGBColor(0xFFFFFFFF);
	//m_pSprites[1]->PaintFrame(g, 2, 350, 280, 0, 0, 0);
	//GetFont(CFont::FS_Large)->SetFontColor(0);
	//GetFont(CFont::FS_Large)->DrawString(g, "EXIT", 350 + m_pSprites[1]->GetFrameWidth(2) / 2, 285, CGraphics::HCENTER);

	if(m_gameState != NULL)
	{
		m_gameState->Paint();
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

void CGame::UpdateOrientation()
{
	SetupScreen(m_pDisplay->GetDisplayMode());
}


void CGame::SetupScreen(DisplayMode mode)
{
	m_nScreenWidth = kScreenSize[mode][0];
	m_nScreenHeight = kScreenSize[mode][1];

#if IPHONE
	CMutableImage* pScreenImage = NULL;
#else
	CMutableImage* pScreenImage = m_pDisplay->GetScreenImage(mode & 0x1);
	pScreenImage->SetSize(m_nScreenWidth, m_nScreenHeight, m_nScreenWidth * sizeof(NativePixel));
	m_pGraphics->SetImage(pScreenImage);
#endif

	m_camera.Init(this, false);
	m_gl.SetCamera(&m_camera);
	m_gl.InvertAspect(this, pScreenImage);


	m_gl.InvertAspect(this, pScreenImage);
}

int CGame::Random(int nMin,int nMax)
{
	return Math::Random(nMin, nMax);
}

int CGame::Random(int nMax)
{
	return Math::Random(0, nMax);
}

DisplayMode CGame::GetCurrentDisplayMode()
{
	return m_pDisplay->GetDisplayMode();
}


void CGame::SetupLandscape(int orientation)
{
	GetDisplay()->SetDeviceOrientation(orientation);
	UpdateOrientation();
}

void CGame::SetGameState(DEF::GAME_STATE_ENUM state)
{
	if ( m_nextState != DEF::GAME_STATE_EXIT )
	{
		m_nextState = state;
	}
}

void CGame::ChangeState()
{
	if ( m_nextState != DEF::GAME_STATE_EXIT && m_nextState != m_currState )
	{
		delete m_gameState;
		m_gameState = NULL;
		m_gameState = GameStateFactory::CreateGameState( this, m_nextState );
	}

	m_currState = m_nextState;
	m_nextState = DEF::GAME_STATE_NULL;
}

void CGame::ClearScreen()
{
	CGraphics * g = GetGraphics();

	g->SetClip(0, 0, 480, 320);
	g->SetARGBColor(BACKGROUND_COLOR);
	g->FillRect(0, 0, 480, 320);
}

void CGame::DrawImage(CImage *pSrc, int x, int y, int anchor)
{
	CGraphics * g = GetGraphics();
	g->DrawImage(pSrc, x, y, anchor);
}

void CGame::DrawString(int posX, int posY, char *str, bool smallFont)
{
	DrawStringPlus(posX,posY,str,smallFont,CGraphics::TOP | CGraphics::LEFT,0);
}
 void  CGame::DrawStringPlus(int posX, int posY, char *str, bool smallFont,int anchor,int distance)
{

	CGraphics * g = GetGraphics();
	CFont *font = NULL;

	if(smallFont)
	{
		font = GetFont(CFont::FS_Small);
	}
	else
	{
		font = GetFont(CFont::FS_Middle);
	}

	font->SetFontColor(0);
	//GetFont(CFont::FS_Large)->DrawString(g, str, posX, posY, CGraphics::VCENTER | CGraphics::LEFT);


	font->InitPage(str, 500);
	font->DrawPage(g, str, posX, posY, anchor, distance);

}
void CGame::DrawButton(_TouchRect button)
{
	CGraphics * g = GetGraphics();

	g->SetARGBColor(BUTTON_COLOR);
	g->FillRect(button.left, button.top, button.width, button.height);
}

void CGame::DrawRect(_TouchRect button)
{
	CGraphics * g = GetGraphics();

	g->SetARGBColor(RECT_COLOR);
	g->FillRect(button.left, button.top, button.width, button.height);
}