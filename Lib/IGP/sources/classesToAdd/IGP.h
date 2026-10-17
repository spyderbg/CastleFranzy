/*
 *  IGP.h
 *  IGP
 *
 *  Created by Fernando Mazzon on 09/05/08.
 *  Copyright 2008 Gameloft. All rights reserved.
 *
 */

#ifndef __IGP_H__
#define __IGP_H__

#import "IGPTexture.h"
#import "IGPDefines.h"
#import "IGPLinkGrabber.h"
#import "IGPUpdater.h"
#import "IGPGame.h"

#import "WhatsNewViewController.h"
#import "CatalogViewController.h"
#import "GameInformationViewController.h"
#import "ReviewWriteViewController.h"
#import "IGPcStr.h"


typedef enum {
	IGP_STATE_NOTHING	= -2,
	IGP_STATE_ALLOC,
	IGP_STATE_RETURN,
	IGP_STATE_INITIALIZE,
	IGP_STATE_LOADING,
	IGP_STATE_RELOAD,
	IGP_STATE_POKE,
	IGP_STATE_UPDATER_GRAB_CONFIG,
	IGP_STATE_MAIN,
	IGP_STATE_MAIN_CYCLING,
	IGP_STATE_RETRIEVING_LINK,
	IGP_STATE_END
} IGPState;

//Touch wrapper (avoids needing a pointer to the UIView)
struct Touch {
	UITouchPhase phase;
	CGPoint prevLocation;
	CGPoint curLocation;
	int		tapCount;
};
typedef struct Touch Touch;

class IGP
{

	CFTimeInterval			m_stateStartTime;
	public:
	static IGP*				s_igpInstance;
	static char				s_buff[1024 * 512];
	bool				newCFGDownloaded;
	static char				s_igpDataGame[5];
	static char				s_igpDataVersion[6];
	static char				s_igpDataLang[3];
	static char				s_igpDataLocale[3];
	static char				s_igpDataLangs;
	static NSString*		s_mainGameShortTitle;

	NSAutoreleasePool*		m_igpPool;
	
	static UIImage*			m_arrowUpImage;
	static UIImage*			m_badgeImage;
	static UIImage*			m_arrowDownImage;
	static UIImage*			m_gameloftImage;
	static UIImage*			m_buyButtonImage;
	static UIImage*			m_infoButtonImage;
	static UIImage*			m_backButtonImage;
	static UIImage*			m_videoButtonImage;
	static UIImage*			m_sendButtonImage;
	static UIImage*			m_bgImage;
	static UIImage*			m_bsTabIcon;
	static UIImage*			m_wnTabIcon;
	static UIImage*			m_catalogTabIcon;
	static UIImage*			m_feedbackTabIcon;
	static UIImage*			m_screenshotBackgroundImage;
	
	IGPTexture*				m_tabBg;
	IGPTexture*				m_coverBack;
	IGPTexture*				m_coverNew;
	IGPTexture*				m_coverNewPrice;
	
	UIInterfaceOrientation	_orientationBeforeIGP;
	UIWindow*				_window;
	UIView*					_view;
	UIApplication*			_app;
	FILE*					_res;
	bool					_loadUpdatedGames;
	
	//Debug
	int					m_framesPainted;
	CFTimeInterval		m_framesStartTime;
	float				m_lastX;
	float				m_lastY;
	
	//State machine
	float				m_stateLength;
	IGPState			m_prevState;
	IGPState			m_state;
	IGPState			m_nextState;
	int					m_subState;
	int					m_newThings;
	
	//UIViewControllers for new design + UITabBarController
	UITabBarController*				_tabBarController;
	WhatsNewViewController*			_whatsnewtab;
	CatalogViewController*			_catalogtab;
	WhatsNewViewController*			_bestsellerstab;
	ReviewWriteViewController*		_reviewtab;
	
	//Main loading indicator
	UIActivityIndicatorView*		_activity;

	//Online features related
	IGPLinkGrabber*		_linkGrabber;
	IGPUpdater*			_updater;
	
	//Cover flow disposition
	CGPoint*			_baseGamePositions;	
	CGPoint*			_currentGamePositions;
	float*				_currentGameAngles;
	float*				_currentGameScales;
	float*				_currentGameLight;
	
	//Main list of all games in no particular order
	IGPGame*			_allGames[K_MAX_GAME];
	
	//List of games that go in BS tab, in cover order(Center = highest prio)
	IGPGame*			_bestSellers[K_MAX_GAME];
	int					_bestSellersCount;
	//List of games that go in WN tab, in cover order(Center = highest prio)
	IGPGame*			_whatsNew[K_MAX_GAME];
	int					_whatsNewCount;
	//List of games that go in CATALOG tab, in normal order(first = highest prio)
	IGPGame*			_catalog[K_MAX_GAME];
	int					_catalogCount;
	IGPGame*			_tableCatalog[K_MAX_GAME];
	int					_tableCatalogCount;

	
	//Load thread management vars
	int					_currentLoadedGameScreenshots;
	int					_currentLoadingGameScreenshots;
	bool				_loadInProgress;
	bool				_mustSet;
	
	int					m_updatedGames;
	int					m_updateCompleted;
	bool				m_openedLink;
	bool				_cheatEnabled;
	bool				_debugEnabled;
	bool				m_touchEnabled;
	bool				m_openedUrl;
	int					m_cheatPos;
	int					_debugPos;
	int					_currentLanguage;

	int					_startDragIndex;
	int					_fingerDelta;
	int					_delta;
	int					_loadingFrame;
	int					_lastBarOrientation;
	int					m_messageNumber;
	bool				m_fingerDown;
	
//	static UITextView*	_debugBox;
	 UIView* blackScreen;//used for exit

	public:
	int					_allGamesCount;
	void	savePricesStates();
	bool	getSavePriceStateForGame(char* codeOfGame);
	void	setPriceUpdatedValues();
	
	

	static void ReleaseRemainingStuff();
	
	void	openUrl(const char* lnk, bool displayLink, int msgNum);
	IGP(UIApplication* app, UIWindow* window, UIView* view, int lang);
	~IGP();
	bool    PaintIGP();
	void	handleTouch(Touch touch);
	bool	getCheatState();
	private:


	CGPoint	translateCoords(CGPoint coord, bool b);
	CGPoint GetGamePosition(int game);
	char	getCorner(CGPoint coord);

	void    paintCover(int i);
	bool	loadData();
	void    loadWindow();

	void	setOrientation();
	void	setOrientationOrtho();
	void	IGPInitialize();
	void	update();
//	UITextView* initDebugBox();
	void	InitArrayData();
	/*NEW DESIGN*/
public:
	
	int			_curIndex[3];
	UIViewController*	_currentTab;
	UIViewController*	_activeTabWhenPaintingStarted;	
	
	void		ReadConfig();
	int			GetGameIndex();
	int			GetGameCount();
	int			GetCoverIndex();
	IGPGame**	GetGameList();
	void	InitCoverStartPositions();
	void	InitIGPData();
	void	LoadScreenshots();
	void	Reinit();
	void	InitTabs(UIView* glView);
	bool	MustRender();
	void	FreeScreenshots();
	bool	NeedsScreenshotsForGame(int i);
	void	MakeListOfGamesAccordingToPrio(IGPGame** sourceList, int sourceListCount, IGPGame** outList, int& outListCount, int prioIndex, bool centerToSides, int index = -1);
	/*NEW DESIGN*/
	
	//STATE MACHINE
	void	setNextState();
	void	setState(IGPState state);
	void	setState(IGPState state, IGPState nextState);
	
	void	HideNotify();
	void	ShowNotify();
	
	int		m_selectedIndex;
};

#endif
