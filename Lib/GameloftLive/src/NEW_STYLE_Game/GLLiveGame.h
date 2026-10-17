#ifndef _GAME_H_
#define _GAME_H_

#include "GLLivePredefine.h"
#include "GlRenderer.h"
#include "GL3DCamera.h"
#include "GLLive_ASprite.h"
#include "ResourceManager.h"
#include "StringManager.h"
#include "TouchRect.h"
#include "Touchpad.h"

#include "GLLiveStateHandler.h"
#include "GLLiveState.h"
//#include "GLLiveStateFactory.h"


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

#define MAX_GAME_STATE_STACK   20

#define FONT_PAK_FILENAME	"gll_font.pak"
#define RES_PAK_FILENAME	"gll_res.pak"
#define STR_PAK_FILENAME	"gll_str.pak"

//#define SPRITE_BG			0
//#define SPRITE_MENU			1
//#define SPRITE_FLAG			2
//#define SPRITE_TEXTBOX		3
//#define SPRITE_EVALUATE     4
//#define NUM_OF_SPRITES		5

#define SPRITE_MENU			0
#define SPRITE_FLAG			1
#define NUM_OF_SPRITES		2
//#define SPRITE_BG			2
//#define SPRITE_TEXTBOX		2

#define FONT_BLACK_HUGE		0
#define FONT_WHITE_MID		1
#define FONT_WHITE_BIG		2
#define FONT_RED_SMALL		3
#define FONT_BLACK_MID		4
#define FONT_RED_BIG		5
#define FONT_RED_MID		6
#define FONT_BLACK_MID_GRAY	7
#define FONT_RED_MID_GRAY	8
#define FONT_WHITE_SMALL	9
#define FONT_WHITE_MID_GRAY	10
#define FONT_BLACK_BIG		11

#define NUM_OF_FONTS		14//14

const char k_sprites_name[NUM_OF_SPRITES][20] = {
//	"BG1",
	"menu",
	"flag",
//	"textbox",
//	"evaluate"
};

//////////////////////////////////////////
// add temp map for lang change
////////////////////////////////////////////
// before                  {"EN","DE","FR","SP","IT","JP"};
// now                     {"EN","EN","DE","FR","IT","SP"};
const int UNO_LANG_MAP[] = {   0,  0,  1,    2,   4,   3};

class CGLLiveASprite;
//class CGLLiveDisplay;
//class CGLLiveGraphics;
//class CGLLiveIndexedImage;
//class CGLLiveResourceManager;
class CGLLiveRFCamera;
//class CGLLiveResFile;
//class CGLLiveTouchpad;
class CGLLiveSaveLoad;
class GLLiveStateChatInvitation;
class GLLiveStateDisconnect;


class CGLLive : public GLLiveStateHandler
{
public:
	CGLLive(void* app = NULL, void* window = NULL, void* view = NULL, int lang = 0, char* deviceToken = NULL, bool canEnterLeaderboard = false);

	~CGLLive(void);

	void Initialize();
	void InitFont();

	bool Update();
	void Paint();
	void handleTouch(float x, float y, bool pressed);
	void SetOrientation(int type);//0: landscapeLeft, 1:landscapeRight, GLLive only suport the two
	void SetInviteToPlay(bool enable);// if there is no MP in your game, set enable = false; the default is true
	void DropGameModeChooseScreen();// if your game only have one game mode, apply this.(some game could customize the rule before create room)
#ifdef WIN32
	void handleTouch(float x, float y);
#endif

	void Pause();
	void Resume();

public:
	inline CGLLiveDisplay* GetDisplay(){return m_pDisplay;};
	inline CGLLiveGraphics* GetGraphics(){return m_pGraphics;};

	inline CGLLiveFont* GetFont(int fs){return m_pFont[fs];};

	inline int GetViewportWidth(){return m_nScreenWidth;};
	inline int GetViewportHeight(){return m_nScreenHeight;};

	inline int ScreenWidth(){return m_nScreenWidth;};
	inline int ScreenHeight(){return m_nScreenHeight;};

	inline CGLLiveGlRenderer& GetGLRender(){return m_gl;};
	inline CGLLiveGL3DCamera& GetGLCamera(){return m_camera;};

	static int Random(int nMin,int nMax);
	static int Random(int nMax);
	static unsigned int GetCurMTime();

    int getLang() { return UNO_LANG_MAP[m_Language] ; };

	inline void SetRFCamera(CGLLiveRFCamera* pCamera){ASSERT(m_pRFCamera != NULL);m_pRFCamera = pCamera;};
	inline CGLLiveRFCamera* GetRFCamera(){return m_pRFCamera;};

	inline bool IsTerminal(){return m_bIsTerminal;}
	inline void TerminateGame(){m_bIsTerminal = true;};

	void UpdateOrientation();
	inline CGLLiveResourceManager& GetResourceManager(){return *m_pResManager;};
	void SetupScreen(DisplayMode mode);
	DisplayMode GetCurrentDisplayMode();
	void SetupLandscape(int orientation);

	//Game state stack operation
	bool PushGLLiveState(GLLiveState *pState);
	bool PopGLLiveState(bool callSwitch = true);
	//Just for GLLiveState chat, to ensure there is only ONE chat state
	bool PopToStateChat();
	GLLiveState *GetChatState();

	inline void InitGLLiveStateStack();
	inline GLLiveState * GetCurState(){return m_GLLiveStateStack[m_gameSSTop];}
	StringManager*	pStringManager;
	CGLLiveASprite*			m_pSprites[NUM_OF_SPRITES];

	//Interface for set local trophy data
	static bool NotifyTrophy(int trophyIndex);
	static bool* LoadTrophyData(int userno);
	static bool BindTrophyAccount(int userno);
	static int  ReadTrophyAccount();
	static bool ReadUserInfo(char *username, int userLen, char *password, int pwdLen, int *timestamp = NULL);
	static bool SaveUserInfo(bool autoLogin, const char *username, const char *password, int stamp = 0);
	
	//The invitation state
	GLLiveStateChatInvitation *m_pInviteState;
	GLLiveStateDisconnect     *m_pDiconnectState;

private:
	//For pop state safely
	void            PopGLLiveStateAction();
	bool            m_bPopState;


	bool			m_bIsTerminal;
	CGLLiveResFile*		m_pResFile;
	CGLLiveResourceManager*	m_pResManager;

	CGLLiveDisplay*		m_pDisplay;
	CGLLiveGlRenderer		m_gl;
	CGLLiveGL3DCamera		m_camera;

	CGLLiveRFCamera*		m_pRFCamera;


	int			m_nScreenWidth;
	int			m_nScreenHeight;

	bool			m_bGLInitialized;
	
	enum DisplayMode	m_displayMode;

	CGLLiveGraphics*			m_pGraphics;

	CGLLiveFont*				m_pFont[NUM_OF_FONTS];
	CGLLiveFileStream*		m_pSaveFile;
	
	CGLLiveTouchpad*			m_pTouchpad;
	
	

//------------------byhy----------------------
	//DEF::GAME_STATE_ENUM m_currState;
	//DEF::GAME_STATE_ENUM m_nextState;
	unsigned int m_timerCount;

//-------------Game state stack 
	GLLiveState *  m_GLLiveStateStack[MAX_GAME_STATE_STACK];   //Game state stack
	int                    m_gameSSTop;
public:
	void*		m_app;
	void*		m_window;
	void*		m_view;
	int			m_Language;
	char*		m_deviceToken;
	char		m_UDID[64];
	bool		m_canEnterLeaderboard;
	bool		m_enableInviteToPlay;
	bool		m_dropGameModeChooseScreen;
	
	char		m_AD_title[128];
	char		m_AD_description[128];
	char		m_AD_link[128];
	char		m_AD_fileType[10];
	int			m_AD_attachmentID;
	int			m_AD_fileSize;
	int			m_AD_width;
	int			m_AD_height;
	int			m_AD_times;
	CGLImage*	m_AD_file;

	virtual void ClearScreen();
	void DrawImage(CGLLiveImage *pSrc, int x, int y, int anchor);
	virtual void DrawButton(_TouchRect button);
	virtual CGLLiveASprite *GetASprite(int index){return m_pSprites[index];}
	
	void RegisterImage(char * name, CGLLiveImage * image)
	{
		strcpy(m_imgName[m_imgNum], name);
		m_GLimg[m_imgNum] = image;
		m_imgNum++;
	}
	void UnregisterImage(char * name)
	{
		for(int i = 0; i < m_imgNum; i++)
		{
			if (strcmp(m_imgName[i], name) == 0)
			{
				for(int j = i; j < m_imgNum - 1; j++)
				{
					strcpy(m_imgName[j], m_imgName[j + 1]);
					m_GLimg[j] = m_GLimg[j + 1];
				}
				break;
			}
		}
		m_imgNum--;
	}

	CGLLiveImage * GetImageByName(char * name)
	{
		for(int i = 0; i < m_imgNum; i++)
		{
			if (strcmp(m_imgName[i], name) == 0)
				return m_GLimg[i];
		}
		return NULL;
	}
//----------------------------------------------

private:
	unsigned int m_LastTick;
	unsigned int m_frameCounter;
	unsigned int m_nLastTimeTick;
	unsigned int m_nLastFrameTime;
	float	m_mouseX;
	float	m_mouseY;
	bool	m_bMousePressed;

	unsigned int m_fps;
	bool		m_bShowFps;

	char		m_imgName[20][20];
	CGLLiveImage *	m_GLimg[20];
	int			m_imgNum;

public:
	inline int GetFPS()
	{
		return m_fps;
	}

};

extern CGLLive * g_pGLLiveInstance;

inline CGLLiveResourceManager& GetResourceManager()
{
	return 	g_pGLLiveInstance->GetResourceManager();
};

#endif
