#ifndef _GAME_H_
#define _GAME_H_

#include "GlRenderer.h"
#include "GL3DCamera.h"
#include "ASprite.h"
#include "ResourceManager.h"
#include "TouchRect.h"
#include "Touchpad.h"

#include "GameStateHandler.h"
#include "GameState.h"
#include "GameStateFactory.h"


#if IPHONE

#define GL_VIEWPORT_WIDTH	320	
#define GL_VIEWPORT_HEIGHT	480

#else

#define GL_VIEWPORT_WIDTH	480	
#define GL_VIEWPORT_HEIGHT	320

#endif

#define HAS_DEBUG_MODE				1
#define HAS_TRAINING_MODE			1
#define VIEW_ROT_FLAG_CACHE_NUM		4

#define LOAD_SUCESS			1
#define LOAD_FAILED			0

#define RES_PAK_FILENAME	"res.pak"


class ASprite;
class CDisplay;
class CGraphics;
class CIndexedImage;
class CResourceManager;
class CRFCamera;
class CResFile;
class CTouchpad;
class CSaveLoad;

class CFont;

class CGame :public GameStateHandler
{
public:
	CGame();

	~CGame(void);

	void Initialize();
	void InitFont();

	void Update();
	bool UpdateQuit();
	void Paint();

	void BeginQuit();

	void Pause();
	void Resume();

public:
	inline CDisplay* GetDisplay(){return m_pDisplay;};
	inline CGraphics* GetGraphics(){return m_pGraphics;};

	inline CFont* GetFont(int fs){return m_pFont[fs];};

	inline int GetViewportWidth(){return m_nScreenWidth;};
	inline int GetViewportHeight(){return m_nScreenHeight;};

	inline int ScreenWidth(){return m_nScreenWidth;};
	inline int ScreenHeight(){return m_nScreenHeight;};

	inline GlRenderer& GetGLRender(){return m_gl;};
	inline GL3DCamera& GetGLCamera(){return m_camera;};

	static int Random(int nMin,int nMax);
	static int Random(int nMax);
	static unsigned int GetCurMTime();


	inline void SetRFCamera(CRFCamera* pCamera){ASSERT(m_pRFCamera != NULL);m_pRFCamera = pCamera;};
	inline CRFCamera* GetRFCamera(){return m_pRFCamera;};

	inline bool IsTerminal(){return m_bIsTerminal;}
	inline void TerminateGame(){m_bIsTerminal = true;};

	void UpdateOrientation();
	inline CResourceManager& GetResourceManager(){return *m_pResManager;};
	void SetupScreen(DisplayMode mode);
	DisplayMode GetCurrentDisplayMode();
	void SetupLandscape(int orientation);

private:

	bool			m_bIsTerminal;
	CResFile*		m_pResFile;
	CResourceManager*	m_pResManager;

	CDisplay*		m_pDisplay;
	GlRenderer		m_gl;
	GL3DCamera		m_camera;

	CRFCamera*		m_pRFCamera;


	int			m_nScreenWidth;
	int			m_nScreenHeight;

	bool			m_bGLInitialized;
	
	enum DisplayMode	m_displayMode;

	CGraphics*			m_pGraphics;

	CFont*				m_pFont[8];
	CFileStream*		m_pSaveFile;
	
	CTouchpad*			m_pTouchpad;
	
	ASprite*			m_pSprites[2];

//------------------byhy----------------------
	DEF::GAME_STATE_ENUM m_currState;
	DEF::GAME_STATE_ENUM m_nextState;
	GameState * m_gameState;
	unsigned int m_timerCount;

public:
	void SetGameState( DEF::GAME_STATE_ENUM state );
	void ChangeState();
	virtual void ClearScreen();
	virtual void DrawString(int posX, int posY, char *str, bool smallFont = false);
    void DrawImage(CImage *pSrc, int x, int y, int anchor);
	virtual void DrawButton(_TouchRect button);
	virtual void DrawRect(_TouchRect button);
	virtual void DrawStringPlus(int posX, int posY, char *str, bool smallFont,int anchor,int distance);
//----------------------------------------------

private:
	unsigned int m_LastTick;
	unsigned int m_frameCounter;
	unsigned int m_nLastTimeTick;
	unsigned int m_nLastFrameTime;

	unsigned int m_fps;
	bool		m_bShowFps;
	
public:
	inline int GetFPS()
	{
		return m_fps;
	}
public :
	inline CTouchpad* getTouchPad()
	{
	return m_pTouchpad;
	}

};

extern CGame * g_pGameApp;

inline CResourceManager& GetResourceManager()
{
	return 	g_pGameApp->GetResourceManager();
};

#endif
