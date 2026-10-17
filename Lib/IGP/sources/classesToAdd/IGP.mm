/*
 *  IGP.mm
 *  IGP
 *
 *  Created by Fernando.Mazzon on 09/05/08.
 *  Copyright 2008 Gameloft. All rights reserved.
 *
 */

#import "IGP.h"
#import "IGPUtil.h"
#import "EN.h"
#include <sys/time.h>
//UITextView* IGP::_debugBox;
IGP*		IGP::s_igpInstance;
char		IGP::s_buff[1024 * 512];
char		IGP::s_igpDataGame[5];
char		IGP::s_igpDataVersion[6];
char		IGP::s_igpDataLang[3];
char		IGP::s_igpDataLocale[3];
char		IGP::s_igpDataLangs;
NSString*   IGP::s_mainGameShortTitle;

UIImage*	IGP::m_arrowUpImage;
UIImage*	IGP::m_badgeImage;
UIImage*	IGP::m_arrowDownImage;
UIImage*	IGP::m_gameloftImage;
UIImage*	IGP::m_buyButtonImage;
UIImage*	IGP::m_infoButtonImage;
UIImage*	IGP::m_backButtonImage;
UIImage*	IGP::m_videoButtonImage;
UIImage*	IGP::m_sendButtonImage;
UIImage*	IGP::m_bgImage;
UIImage*	IGP::m_bsTabIcon;
UIImage*	IGP::m_wnTabIcon;
UIImage*	IGP::m_catalogTabIcon;
UIImage*	IGP::m_feedbackTabIcon;
UIImage*	IGP::m_screenshotBackgroundImage;

void IGP::ReleaseRemainingStuff()
{
	SAFE_DELETE_OBJC2(IGP::m_arrowUpImage);
	SAFE_DELETE_OBJC2(IGP::m_arrowDownImage);
	SAFE_DELETE_OBJC2(IGP::m_gameloftImage);
	SAFE_DELETE_OBJC2(IGP::m_buyButtonImage);
	SAFE_DELETE_OBJC2(IGP::m_infoButtonImage);
	SAFE_DELETE_OBJC2(IGP::m_backButtonImage);
	SAFE_DELETE_OBJC2(IGP::m_videoButtonImage);
	SAFE_DELETE_OBJC2(IGP::m_sendButtonImage);
	SAFE_DELETE_OBJC2(IGP::m_bgImage);
	SAFE_DELETE_OBJC2(IGP::m_bsTabIcon);
	SAFE_DELETE_OBJC2(IGP::m_wnTabIcon);
	SAFE_DELETE_OBJC2(IGP::m_catalogTabIcon);
	SAFE_DELETE_OBJC2(IGP::m_feedbackTabIcon);
	SAFE_DELETE_OBJC2(IGP::m_screenshotBackgroundImage);
}

IGP::IGP(UIApplication* app, UIWindow* window, UIView* view, int lang)
{
	m_igpPool				= [[NSAutoreleasePool alloc] init];
	s_igpInstance			= this;
	_window					= window;
	_view					= view;
	_app					= app;
	newCFGDownloaded		= false;
	
	_currentLoadedGameScreenshots = -1;
	_currentLoadingGameScreenshots = -1;
	_loadInProgress			= false;
//	_debugBox				= initDebugBox();
	_currentLanguage		= lang;
	
	_currentGamePositions	= NULL;
	_currentGameLight		= NULL;
	_baseGamePositions		= NULL;
	_currentGameAngles		= NULL;
	_currentGameScales		= NULL;

	_linkGrabber			= NULL;
	_updater				= NULL;
	m_stateLength			= 1;
	_cheatEnabled			= false;
	_debugEnabled			= true;
	m_openedUrl				= false;
	m_touchEnabled			= true;
	_mustSet				= false;
	m_updateCompleted		= 0;
	m_updatedGames			= 0;
	
	m_cheatPos				= 0;
	_activeTabWhenPaintingStarted = nil;
	_bestsellerstab			= nil;
	_whatsnewtab			= nil;
	m_newThings				= 0;
	
//	IGPInitialize();
	char pathToSave[255];
	GetFileName("dateI",true,pathToSave);
	if(![[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithUTF8String:pathToSave]])
	{
		NSLog(@"ERROR: Make sure you have called 'IGPSendGameInfo' from a IGPSendInfo object");
		return;
	}
		
		
	setState(IGP_STATE_ALLOC);
	


}
// UITextView* IGP::initDebugBox()
// {
	// //CGRect frame = CGRectMake(0.0, 120, 300, 260);
	// CGRect frame = CGRectMake(0, 0, 0, 0);
	// UITextView *textView = [[[UITextView alloc] initWithFrame:frame] autorelease];
	// [textView setTransform:CGAffineTransformMakeRotation(3.14/2)];
	// textView.textColor = [UIColor blackColor];
	// textView.backgroundColor = [UIColor whiteColor];
	// textView.text = @"";
	// textView.font = [UIFont fontWithName:@"Arial" size:9];
	// textView.alpha = 0.8;
	// textView.textColor = [UIColor blueColor];
	// textView.returnKeyType = UIReturnKeyDefault;
	// [textView setEditable:NO];

	// [_window addSubview:textView];
	// return textView;
// }

IGP::~IGP()
{
	printf("IGP Destructor!\n");
	
	//app changes
	[blackScreen removeFromSuperview];
	[blackScreen release];
	//[_activeTabWhenPaintingStarted release];
	[[UIApplication sharedApplication] setStatusBarOrientation:_orientationBeforeIGP];
//	[_debugBox removeFromSuperview];
//	_debugBox = nil;
	[_tabBarController.view removeFromSuperview];

	[_view removeFromSuperview];
	[_window addSubview:_view];
	[_view setFrame:CGRectMake(0, 0, 320, 480)];
	[_window setDelegate: NULL];
		
	//objects release
	SAFE_DELETE_OBJC(_updater);
	SAFE_DELETE_OBJC(_linkGrabber);

//salcideon - this caused a crash - should be release by pool, need to check
//	SAFE_DELETE_OBJC(_tabBarController)
	SAFE_DELETE_OBJC(_activity);
	SAFE_DELETE_OBJC(_catalogtab)
	SAFE_DELETE_OBJC(_reviewtab)
#ifndef AVOID_BSWH_VIEWS_RELEASE
	//while(_whatsnewtab != nil && _whatsnewtab.retainCount > 0)
		SAFE_DELETE_OBJC(_whatsnewtab)

	//while(_bestsellerstab != nil && _bestsellerstab.retainCount > 0)
			SAFE_DELETE_OBJC(_bestsellerstab)
	//	_activeTabWhenPaintingStarted = NULL;
#endif	
	 
	SAFE_DELETE_ARRAY(_currentGamePositions);
	SAFE_DELETE_ARRAY(_currentGameAngles);
	SAFE_DELETE_ARRAY(_currentGameScales);
	SAFE_DELETE_ARRAY(_baseGamePositions);
	SAFE_DELETE_ARRAY(_currentGameLight);

	IGPGame::FreePackIndex();
	for (int i = 0; i < K_MAX_GAME; i++)
	{
		SAFE_DELETE(_allGames[i]);
	}

	SAFE_DELETE_OBJC(m_gameloftImage);
	SAFE_DELETE_OBJC(m_buyButtonImage);
	SAFE_DELETE_OBJC(m_infoButtonImage);
	SAFE_DELETE_OBJC(m_backButtonImage);
	SAFE_DELETE_OBJC(m_videoButtonImage);
	SAFE_DELETE_OBJC(m_sendButtonImage);
	SAFE_DELETE_OBJC(m_bgImage);
	SAFE_DELETE_OBJC(m_bsTabIcon);
	SAFE_DELETE_OBJC(m_wnTabIcon);
	SAFE_DELETE_OBJC(m_catalogTabIcon);
	SAFE_DELETE_OBJC(m_feedbackTabIcon);
	SAFE_DELETE_OBJC(m_screenshotBackgroundImage);

	SAFE_DELETE_OBJC(m_arrowDownImage);
	SAFE_DELETE_OBJC(m_arrowUpImage);
	
	SAFE_DELETE(m_tabBg);
	SAFE_DELETE(m_coverBack);
	SAFE_DELETE(m_coverNew);
	SAFE_DELETE(m_coverNewPrice);

	IGPcStr::Free();
	[m_igpPool release];
	IGP::s_igpInstance = NULL;
	[IGP::m_badgeImage release];

}

bool IGP::getCheatState()
{
	return _cheatEnabled;
}
/* Loads necessary resources to display the IGP window*/
void IGP::loadWindow()
{
	int		len;

	_res = openFile("window");
	char filenamebadge[255];
	char pathbadge[255];
	
	sprintf(filenamebadge,"badge.png");
	GetFileName(filenamebadge, false, pathbadge);
	
	if(fileExists(pathbadge))
	{
		NSString* temp = [[NSString alloc] initWithUTF8String: pathbadge];
		IGP::m_badgeImage = [[UIImage alloc] initWithContentsOfFile:temp];
		[temp release];
		temp = nil;
	}
	UIImage *test123 = m_badgeImage;
	
	if (_res != NULL)
	{
		bufferFromFile(_res, s_buff, &len);
		m_arrowUpImage = imageFromBuffer(s_buff, len);
		bufferFromFile(_res, s_buff, &len);
		m_arrowDownImage = imageFromBuffer(s_buff, len);
		bufferFromFile(_res, s_buff, &len);
		m_gameloftImage = imageFromBuffer(s_buff, len);
		bufferFromFile(_res, s_buff, &len);
		m_buyButtonImage = imageFromBuffer(s_buff, len);
		bufferFromFile(_res, s_buff, &len);		
		m_backButtonImage = imageFromBuffer(s_buff, len);
		bufferFromFile(_res, s_buff, &len);
		m_videoButtonImage = imageFromBuffer(s_buff, len);		
		bufferFromFile(_res, s_buff, &len);
		m_sendButtonImage = imageFromBuffer(s_buff, len);	
		bufferFromFile(_res, s_buff, &len);
		m_infoButtonImage = imageFromBuffer(s_buff, len);
		bufferFromFile(_res, s_buff, &len);
		m_bgImage = imageFromBuffer(s_buff, len);	
		bufferFromFile(_res, s_buff, &len);
		m_bsTabIcon = imageFromBuffer(s_buff, len);	
		bufferFromFile(_res, s_buff, &len);
		m_wnTabIcon = imageFromBuffer(s_buff, len);	
		bufferFromFile(_res, s_buff, &len);
		m_catalogTabIcon = imageFromBuffer(s_buff, len);	
		bufferFromFile(_res, s_buff, &len);
		m_feedbackTabIcon = imageFromBuffer(s_buff, len);	
		bufferFromFile(_res, s_buff, &len);
		m_screenshotBackgroundImage = imageFromBuffer(s_buff, len);
		bufferFromFile(_res, s_buff, &len);
		m_tabBg = new IGPTexture(s_buff, len, false, K_FORMAT_FOR_WINDOW);
		bufferFromFile(_res, s_buff, &len);
		m_coverBack = new IGPTexture(s_buff, len, false, K_FORMAT_FOR_WINDOW);

		
		for(int i = 0; i <  s_igpDataLangs;i++)
		{
			if (i != _currentLanguage)
			{
				//SKIP
				bufferFromFile(_res, s_buff, &len);
				bufferFromFile(_res, s_buff, &len);
			}
			else
			{
				//READ
				bufferFromFile(_res, s_buff, &len);
				m_coverNew = new IGPTexture(s_buff, len, false, K_FORMAT_FOR_WINDOW);
				bufferFromFile(_res, s_buff, &len);
				m_coverNewPrice = new IGPTexture(s_buff, len, false, K_FORMAT_FOR_WINDOW);
			}
		}
	}
	fclose(_res);
	_res = NULL;
}

void IGP::openUrl(const char* lnk, bool displayLink, int msgNum)
{
	if (m_touchEnabled)
	{
		m_touchEnabled = false;
	}
	else
	{
		return;
	}

	char linkTrack[255];
	char linkNoTrack[255];
	
	NSString* linkTrackString;
	NSString* linkNoTrackString;
	NSString* FeedURL;

	
	strcpy(linkTrack, lnk);
	strcpy(linkNoTrack, lnk);
	strcat(linkNoTrack, K_NOTRACK_PARAMETER);
	
	linkTrackString = [[NSString alloc] initWithUTF8String:linkTrack];
	linkNoTrackString = [[NSString alloc] initWithUTF8String:linkNoTrack];

	FeedURL = _cheatEnabled?linkNoTrackString:linkTrackString;

	if (_cheatEnabled)
	{
		if (displayLink)
		{
			NSString* title = [[NSString alloc] initWithUTF8String:s_igpDataVersion];
			UIAlertView *alert = [[UIAlertView alloc] initWithTitle:title message:linkTrackString delegate:nil cancelButtonTitle:IGPcStr::GetNSString(TXT_OK) otherButtonTitles: nil];
			[alert show];
			[alert release];
			m_touchEnabled = true;
			[title release];
		}
		else
		{
			
			m_prevState = m_state;
			setState(IGP_STATE_RETRIEVING_LINK);
			m_messageNumber = msgNum;
			//[Bug id: 	2154567]
			_window.userInteractionEnabled = false;
			[_linkGrabber getRedirectLink:FeedURL];
			
		}
	}
	else
	{
		m_prevState = m_state;
		setState(IGP_STATE_RETRIEVING_LINK);
		m_messageNumber = msgNum;
		//[Bug id: 	2154567]
		_window.userInteractionEnabled = false;
		[_linkGrabber getRedirectLink:FeedURL];
	}
}

/*Allocates array memory*/
void IGP::InitArrayData()
{
	_currentGamePositions	= new CGPoint[K_MAX_GAME];//_allGamesCount];
	_baseGamePositions		= new CGPoint[K_MAX_GAME];//_allGamesCount];
	_currentGameLight		= new float[K_MAX_GAME];//_allGamesCount];
	_currentGameAngles		= new float[K_MAX_GAME];//_allGamesCount];
	_currentGameScales		= new float[K_MAX_GAME];//_allGamesCount];
	
}

/*Set the initial values for cover positions*/
void IGP::InitCoverStartPositions()
{
	for (int i = 0; i < GetGameCount(); i++)
	{
		_baseGamePositions[i] = GetGamePosition(i);

		_currentGamePositions[i] = _baseGamePositions[i];

		_currentGameScales[i] = K_IGP_SPLASH_SCALE_FACTOR2;
		_currentGameAngles[i] = 0;
		if (i < GetGameIndex())
		{
			_currentGameAngles[i] = K_IGP_SPLASH_ANGLE_MAX;
			_currentGameScales[i] = K_IGP_SPLASH_SCALE_FACTOR;
		}
		else if (i > GetGameIndex())
		{
			_currentGameAngles[i] = -K_IGP_SPLASH_ANGLE_MAX;
			_currentGameScales[i] = K_IGP_SPLASH_SCALE_FACTOR;
		}
		_currentGameLight[i] = parabolic(abs(_currentGamePositions[i].x), K_SCREEN_H, K_LIGHT_FULL, K_LIGHT_NONE);
	}
}

void IGP::InitIGPData()
{
	_res = openFile("igpdata");
	fread(s_igpDataGame, sizeof(char), 5, _res);		//read game name
	fread(&s_igpDataVersion, sizeof(char), 6, _res);	//read version
	fread(&s_igpDataLangs, sizeof(char), 1, _res);		//read number of languages
	for(int i = 0; i < s_igpDataLangs;i++)
	{
		if (i != _currentLanguage)
		{
			fread(s_buff, sizeof(char), 2, _res);		//read 1 lang
		}
		else
		{
			fread(s_igpDataLang, sizeof(char), 2, _res);
//			printf("Current language is %s\n", s_igpDataLang);
		}
	}
	NSString* localeIdentifier = [[NSLocale currentLocale] objectForKey:NSLocaleCountryCode];
	if (localeIdentifier == nil)
	{
		localeIdentifier = @"XX";
	}
	
	sprintf(s_igpDataLocale, [localeIdentifier UTF8String]);
//	printf("Current locale is %s\n", s_igpDataLocale);
	fclose(_res);
}

/*Loads game data from original pack files*/
bool IGP::loadData()
{
	trace(DEBUG_LOADING, "Allocating arrays\n");
	InitArrayData();
	
	trace(DEBUG_LOADING, "Loading error messages\n");

	trace(DEBUG_LOADING, "Calculating cover startup positions\n");

	InitCoverStartPositions();

	trace(DEBUG_LOADING, "Finished loading\n");
	_res = openFile("textures");
	return true;
}

CGPoint IGP::GetGamePosition(int game) 
{
	CGPoint ret;
	int sg;
	
	game -= GetGameIndex();
	ret.y = K_IGP_SPLASH_Y;
	ret.x = 0;
	float dtc = K_IGP_DISTANCE_TO_CENTER;
	if (abs(game) == 1)
	{
		ret.x = dtc * game;
	}
	else if (abs(game) > 1)
	{
		sg = SIGN(game);
		ret.x = (dtc * sg) + K_IGP_DISTANCE_BETWEEN * (abs(game) - 1) * sg;
	}
	return ret;
}


void IGP::setOrientation()
{	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glScalef(0.6, 0.6, 0.6);
	glTranslatef(0, -1.05, 0);
	glFrustumf(-K_SCREEN_W/2, K_SCREEN_W/2, K_SCREEN_H/2, -K_SCREEN_H/2, K_FRUSTRUM_Z_NEAR, K_FRUSTRUM_Z_FAR);
	
	glViewport(0, 0, K_SCREEN_W, K_SCREEN_H);

}
void IGP::setOrientationOrtho()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, K_SCREEN_W, K_SCREEN_H); 
#ifdef WRONG_SIZED_TEXTURES_ISSUE
	glOrthof(-K_SCREEN_W/2+50, K_SCREEN_W/2-50, K_SCREEN_H/2, -K_SCREEN_H/2+50, K_FRUSTRUM_Z_NEAR, K_FRUSTRUM_Z_FAR);
#else		
	glOrthof(-K_SCREEN_W/2, K_SCREEN_W/2, K_SCREEN_H/2, -K_SCREEN_H/2, K_FRUSTRUM_Z_NEAR, K_FRUSTRUM_Z_FAR);
#endif	
	
	
}


bool IGP::MustRender()
{
	if (m_state == IGP_STATE_RETRIEVING_LINK || m_state == IGP_STATE_END)
	{
		return true;
	}
	if ((_tabBarController != nil && _whatsnewtab != nil && _bestsellerstab != nil))
	{
		return ((/*[_tabBarController selectedIndex]*/m_selectedIndex == 1 
					&& [(WhatsNewViewController*)_whatsnewtab shouldRender]) 
				|| (/*[_tabBarController selectedIndex]*/m_selectedIndex == 0 
					&& [(WhatsNewViewController*)_bestsellerstab shouldRender]));
	}
	else
	{
		return true;
	}
}
void IGP::IGPInitialize()
{
	[[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationPortrait];
	[[UIApplication sharedApplication] setStatusBarHidden:YES];
	//[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	_orientationBeforeIGP = [[UIApplication sharedApplication] statusBarOrientation];

	_fingerDelta		= 0;
	m_fingerDown		= false;
	m_openedLink		= false;
	m_framesPainted		= 0;
	m_framesStartTime	= CFAbsoluteTimeGetCurrent();

	_updater			= [[IGPUpdater alloc] init];
	_linkGrabber		= [[IGPLinkGrabber alloc] init];	
	_activity			= [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
	[_activity setFrame : K_MAIN_ACTIVITY_RECT];
	[_activity startAnimating];
	[_activity setAlpha:K_ALPHA_FULL];
	[_view addSubview:_activity];
	InitIGPData();
	loadWindow();
	for(int i = 0; i < K_MAX_GAME; i++)
	{
		_allGames[i] = new IGPGame();
	}
	
	//text loading:
	

	IGPcStr::AllocStringBuffers();
	IGPcStr::LoadTexts(_currentLanguage);

	InitTabs(_view);	

	setOrientationOrtho();
	m_tabBg->paint(0, K_IGP_BG_Y, 0, 1, 0, K_ALPHA_FULL, K_LIGHT_FULL, false);
	setOrientation();
	_loadingFrame = 0;

	_window.userInteractionEnabled = false;
	[_linkGrabber pokeConnectivity];
//	setState(IGP_STATE_POKE);


}

///////////////
//STATE MACHINE
///////////////
void IGP::setState(IGPState state)
{
	if(state == IGP_STATE_MAIN)
	{
		_whatsnewtab.tabBarItem.enabled = YES;
		_catalogtab.tabBarItem.enabled = YES;
		_bestsellerstab.tabBarItem.enabled = YES;
		_reviewtab.tabBarItem.enabled = YES;
		[_bestsellerstab enableBackButton];
	}
	m_stateStartTime = CFAbsoluteTimeGetCurrent();
	m_state = state;
	m_subState = 0;
	m_nextState = IGP_STATE_NOTHING;

}

void IGP::setState(IGPState state, IGPState nextState)
{
	m_stateStartTime = CFAbsoluteTimeGetCurrent();
	setState(state);
	m_nextState = nextState;
}

void IGP::setNextState()
{
	setState(m_nextState);
}

//////////////
//////////////
bool IGP::PaintIGP()
{
	if(!MustRender() )
		return false;
	if(m_state != IGP_STATE_ALLOC)
	{//save pointer to the active tab, will use that for the rest of the cycle
		if(_activeTabWhenPaintingStarted)
			[_activeTabWhenPaintingStarted release];
		_activeTabWhenPaintingStarted = [[_tabBarController selectedViewController] retain];
	}
//	[_activeTabWhenPaintingStarted retain];
	if (m_framesPainted % 10 == 0)
	{
		//printf("FPS:%f\n", m_framesPainted/ (CFAbsoluteTimeGetCurrent() -m_framesStartTime));
		m_framesPainted = 0;
		m_framesStartTime = CFAbsoluteTimeGetCurrent();
	}
	m_framesPainted++;
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_PROJECTION);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
	setOrientation();

	update();

	setOrientationOrtho();

	switch (m_state) {

		case IGP_STATE_RETRIEVING_LINK:
		{
			if ((!(_activeTabWhenPaintingStarted == _whatsnewtab)) && (!(_activeTabWhenPaintingStarted == _bestsellerstab)))
			{
				//
				break;
			}
		}
		case IGP_STATE_MAIN:
		case IGP_STATE_MAIN_CYCLING:

		{
			if ((!(_activeTabWhenPaintingStarted == _whatsnewtab)) && (!(_activeTabWhenPaintingStarted == _bestsellerstab)))
			{
				break;
			}		

			setOrientationOrtho();
			m_tabBg->paint(0, K_IGP_BG_Y, 0, 1, 0, K_ALPHA_FULL, K_LIGHT_FULL, false);
			setOrientation();

			//salcideon - please don't crash if you dont find any game to paint cover
			if (((_activeTabWhenPaintingStarted == _whatsnewtab) && (_whatsNewCount == 0))
				||((_activeTabWhenPaintingStarted == _bestsellerstab) && (_bestSellersCount == 0)))
			{
				break;
			}

			
			for (int i =  0; i < GetGameIndex(); i ++)
			{
				paintCover(i);
			}
			for (int i =  GetGameCount() - 1; i > GetGameIndex(); i --)
			{
				paintCover(i);

			}
			paintCover(GetGameIndex());

			break;
		}
		case IGP_STATE_END:
		{
			if (m_subState == 0)
			{
				blackScreen = [[UIView alloc] initWithFrame: CGRectMake(0, 0, 320, 480)];
				blackScreen.backgroundColor = [UIColor blackColor];
				[_activity removeFromSuperview];
				[_updater CancelConnection];
				[_whatsnewtab killTimer];
				[_bestsellerstab killTimer];
				[_catalogtab removeViews];
				[_reviewtab removeViews];
				_tabBarController.view.alpha = 0;
				_whatsnewtab.tabBarItem.enabled = NO;
				_catalogtab.tabBarItem.enabled = NO;
				_bestsellerstab.tabBarItem.enabled = NO;
				_reviewtab.tabBarItem.enabled = NO;
				
				[_whatsnewtab.tabBarItem setTitle:@" "];
				[_catalogtab.tabBarItem setTitle:@" "];
				[_bestsellerstab.tabBarItem setTitle:@" "];
				[_reviewtab.tabBarItem setTitle:@" "];
				// if (_currentTab)
				// {
					// if (_currentTab.view)
					// {
						// _currentTab.view.backgroundColor = [UIColor blackColor];
					// }
				// }
				[_tabBarController.view setNeedsDisplay];
				[_tabBarController.view setNeedsLayout];
				[_tabBarController.view addSubview:blackScreen];
				
				m_subState++;
			}
			else
			{
				m_subState++;
				if (m_subState == 3)
				{
					
					return true;
				}
			}
			break;
		}
		case IGP_STATE_LOADING:
		case IGP_STATE_RELOAD:
		case IGP_STATE_UPDATER_GRAB_CONFIG:
		case IGP_STATE_POKE:
		{
			setOrientationOrtho();
			m_tabBg->paint(0, K_IGP_BG_Y, 0, 1, 0, K_ALPHA_FULL, K_LIGHT_FULL, false);
			setOrientation();
			break;
		}
		default:
			break;
	}
	setOrientationOrtho();
	glViewport(0, 0, K_SCREEN_W, K_SCREEN_H);
	return false;
}

void IGP::paintCover(int i)
{
	CGPoint p;
	p.x = _currentGamePositions[i].x;
	p.y = _currentGamePositions[i].y;
	float alpha = lineal(_currentGameLight[i], 1, 0.8, K_ALPHA_FULL);

	long a2;
	struct timeval	tp;
	gettimeofday(&tp, 0);
	a2 = (tp.tv_sec * 1000 + tp.tv_usec / 1000)% 1000;
	
	float alpha2 = 0;
	if (a2 < 500)
	{
		alpha2 = lineal(a2, 500, 0, alpha);
	}
	else
	{
		alpha2 = lineal(a2- 500, 500, alpha, 0);
	}

	if (GetGameList()[i]->m_hasNewPrice || GetGameList()[i]->m_isNew)
	{
		m_coverBack->paint(p.x, p.y, 1, _currentGameScales[i], _currentGameAngles[i], alpha2, _currentGameLight[i], true);
	}

	GetGameList()[i]->GetSplash()->paint(p.x, p.y, 1, _currentGameScales[i], _currentGameAngles[i], alpha, _currentGameLight[i], true);
	
	if (GetGameList()[i]->m_hasNewPrice)
	{
		m_coverNewPrice->paint(p.x, p.y, 1, _currentGameScales[i], _currentGameAngles[i], alpha, _currentGameLight[i], true);
	}
	if (GetGameList()[i]->m_isNew)
	{
		m_coverNew->paint(p.x, p.y, 1, _currentGameScales[i], _currentGameAngles[i], alpha, _currentGameLight[i], true);
	}
}

void IGP::update()
{ 
	/*	NSLog(@"retain count  _whatsnewtab: %d",[_whatsnewtab retainCount]);
		NSLog(@"retain count  _bestsellerstab: %d",[_bestsellerstab retainCount]);*/
	/*NSLog(@"retain count RW 4: %d",[_tab4 retainCount]);
	NSLog(@"retain count RW 1: %d",[_tab1 retainCount]);*/
	//NSLog(@"is changing views: %@",[_whatsnewtab isChangingView] ? @"YES" : @"NO");
	switch (m_state) {
		case IGP_STATE_RETURN:
		{

			break;
		}
		case IGP_STATE_ALLOC:
		{
			switch(m_subState)
			{
				case 0:		
					_activity			= [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
					[_activity setFrame : K_MAIN_ACTIVITY_RECT];
					[_activity startAnimating];
					[_activity setAlpha:K_ALPHA_FULL];
					[_view addSubview:_activity];
					m_subState++;
					

					break;
				case 1:	
										[[UIApplication sharedApplication] setStatusBarHidden:YES];
					//[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
					_orientationBeforeIGP = [[UIApplication sharedApplication] statusBarOrientation];
					
					_fingerDelta		= 0;
					m_fingerDown		= false;
					m_openedLink		= false;
					m_framesPainted		= 0;
					m_framesStartTime	= CFAbsoluteTimeGetCurrent();

					_updater			= [[IGPUpdater alloc] init];
					_linkGrabber		= [[IGPLinkGrabber alloc] init];	
					m_subState++;	
					break;
				case 2:	
					InitIGPData();
					m_subState++;
					break;
				case 3:	
					
					loadWindow();
					for(int i = 0; i < K_MAX_GAME; i++)
					{
						_allGames[i] = new IGPGame();
					}
					//text loading:
			
			
					IGPcStr::AllocStringBuffers();
					IGPcStr::LoadTexts(_currentLanguage);
					InitTabs(_view);	
					setOrientationOrtho();
					m_tabBg->paint(0, K_IGP_BG_Y, 0, 1, 0, K_ALPHA_FULL, K_LIGHT_FULL, false);
					setOrientation();
					_loadingFrame = 0;
			
					_window.userInteractionEnabled = false;
					[_linkGrabber pokeConnectivity];
					m_subState = 1;
					setState(IGP_STATE_POKE);
					
					break;
			}	
			break;
		}
		case IGP_STATE_UPDATER_GRAB_CONFIG:
		{
			switch([_updater getState])
			{
				case UPDATER_STATE_INIT:
				{
					if ((CFAbsoluteTimeGetCurrent() - m_stateStartTime > 16))
					{
						setState(IGP_STATE_LOADING);
					}
					break;
				}
				case UPDATER_STATE_DL_CFG:
				{
					if ((CFAbsoluteTimeGetCurrent() - m_stateStartTime > 16))
					{
						setState(IGP_STATE_LOADING);
					}
					//Do nothing and loop
					break;
				}
				case UPDATER_STATE_ERROR:
				{
					setState(IGP_STATE_LOADING);
					break;
				}
				case UPDATER_STATE_READY:
				{
					setState(IGP_STATE_LOADING);
					break;
				}
			}
			break;
		}
		case IGP_STATE_POKE:
		{
			if ([_linkGrabber currentStatus] != STATE_POKE)
			{
				[_updater initAndDownloadConfig]; 
			
				setState(IGP_STATE_UPDATER_GRAB_CONFIG);
			}
			break;
		}
		case IGP_STATE_LOADING:
		{
			if (m_subState == 1)
			{
				ReadConfig();
				setPriceUpdatedValues();
				savePricesStates();
				IGPGame::ReadPackIndex();

				for(int i = 0; i < _allGamesCount; i++)
				{
					_allGames[i]->CheckForStatus();
					_allGames[i]->LoadRes();
				}
				s_mainGameShortTitle = IGPGame::getMainShortTitle();
				

			//	while (true)
				{
				}
				
				for(int i = 0; i < _allGamesCount; i++)
				{
					for(int j = 0; j < i; j++)
					{
						if ([_allGames[i]->GetName() compare: _allGames[j]->GetName()] == NSOrderedAscending)
						{
							IGPGame* tmp = _allGames[i];
							_allGames[i] = _allGames[j];
							_allGames[j] = tmp;
						}
					}
				}
				MakeListOfGamesAccordingToPrio(_allGames, _allGamesCount, _whatsNew, _whatsNewCount, PRIO_WN, true, 0);
				MakeListOfGamesAccordingToPrio(_allGames, _allGamesCount, _bestSellers, _bestSellersCount, PRIO_BS, true, 1);
				MakeListOfGamesAccordingToPrio(_allGames, _allGamesCount, _catalog, _catalogCount, PRIO_CA, false);
				MakeListOfGamesAccordingToPrio(_allGames, _allGamesCount, _tableCatalog, _tableCatalogCount, PRIO_CA, false);
				

				/*for (int i = 0 ; i < _tableCatalogCount; i++)
				{
					printf("%s\n",_tableCatalog[i]->m_gameCode);
 				}*/
				if (sortCatalog)
				{
					for(int i = 0; i < _catalogCount; i++)
					{
						for(int j = 0; j < i; j++)
						{
							if ([_catalog[i]->GetName() compare: _catalog[j]->GetName()] == NSOrderedAscending)
							{
								IGPGame* tmp = _catalog[i];
								_catalog[i] = _catalog[j];
								_catalog[j] = tmp;
							}
						}
					}
				}
				if (!usePicker)
				{
					for(int i = 0; i < _tableCatalogCount; i++)
					{
						for(int j = 0; j < i; j++)
						{
							if ([_tableCatalog[i]->GetName() compare: _tableCatalog[j]->GetName()] == NSOrderedAscending)
							{
								IGPGame* tmp = _tableCatalog[i];
								_tableCatalog[i] = _tableCatalog[j];
								_tableCatalog[j] = tmp;
							}
						}
					}
				}
				[_catalogtab setGameList];
				[_reviewtab setGameList:_catalog count:_catalogCount];
			
				loadData();
				setState(IGP_STATE_MAIN);
				_window.userInteractionEnabled = true;
//				[_tab1 show];
				[_bestsellerstab show];
				[_activity setAlpha:0];
			}
			m_subState = 1;
			
			
			break;
		}
		case IGP_STATE_RELOAD:
		{
			for(int i = 0; i < _allGamesCount; i++)
			{
				_allGames[i]->CheckForStatus();
				_allGames[i]->LoadRes();
			}
			MakeListOfGamesAccordingToPrio(_allGames, _allGamesCount, _whatsNew, _whatsNewCount, PRIO_WN, true, 0);
			MakeListOfGamesAccordingToPrio(_allGames, _allGamesCount, _bestSellers, _bestSellersCount, PRIO_BS, true, 1);
			MakeListOfGamesAccordingToPrio(_allGames, _allGamesCount, _catalog, _catalogCount, PRIO_CA, false);
			MakeListOfGamesAccordingToPrio(_allGames, _allGamesCount, _tableCatalog, _tableCatalogCount, PRIO_CA, false);
			InitCoverStartPositions();
			if (sortCatalog)
			{
				for(int i = 0; i < _catalogCount; i++)
				{
					for(int j = 0; j < i; j++)
					{
						if ([_catalog[i]->GetName() compare: _catalog[j]->GetName()] == NSOrderedAscending)
						{
							IGPGame* tmp = _catalog[i];
							_catalog[i] = _catalog[j];
							_catalog[j] = tmp;
						}
					}
				}
			}	
			if (!usePicker)
			{
				for(int i = 0; i < _tableCatalogCount; i++)
				{
					for(int j = 0; j < i; j++)
					{
						if ([_tableCatalog[i]->GetName() compare: _tableCatalog[j]->GetName()] == NSOrderedAscending)
						{
							IGPGame* tmp = _tableCatalog[i];
							_tableCatalog[i] = _tableCatalog[j];
							_tableCatalog[j] = tmp;
						}
					}
				}
			}
			IGP::s_igpInstance->savePricesStates();
			[_catalogtab setGameList];
			[_reviewtab setGameList:_catalog count:_catalogCount];
			setState(IGP_STATE_MAIN);
			_currentLoadedGameScreenshots = -1;
			[_activity setAlpha:0];
			break;
		}
		case IGP_STATE_MAIN:
		case IGP_STATE_MAIN_CYCLING:
		{
			if (m_updateCompleted == 1 || m_updateCompleted == 4)
			{
				if (m_updatedGames > 0)
				{
					m_updatedGames = 0;
					[_activity setAlpha:1];
					setState(IGP_STATE_RELOAD);
				}
				m_updateCompleted = -1;
				break;
			}
			if ([_updater getState] == UPDATER_STATE_READY && ( [_updater iswifiAvailable] 
			#ifdef USE_3G
					|| [_updater isEDGEAvailable] 		
			#endif 				
			))
			{
				//First check for unavailable games
				for(int i = 0; i < _allGamesCount; i++)
				{
					if (!_allGames[i]->m_isAvailable)
					{
						[_updater UpdateGame:_allGames[i]];
						return;
					}
				}
				if (m_updateCompleted == 0)
				{
					if (m_updatedGames > 0)
					{
						m_updateCompleted = 1;
					}
					else
					{
						m_updateCompleted = 3;
					}
				}
		//disabled for now Liviu Barleanu
				//Then update other files
		//		for(int i = 0; i < _allGamesCount; i++)
		//		{
		//			if (_allGames[i]->m_isAvailable && !_allGames[i]->m_isUpdated)
		//			{
		//				[_updater UpdateGame:_allGames[i]];
		//				return;
		//			}
		//		}
				if (m_updateCompleted == 3) m_updateCompleted = 4;
				
			}/*
			else 
			{
				 trace(DEBUG_UPDATER, "Avoid game download, wifi\n");
			}*/
			if (_currentTab != _activeTabWhenPaintingStarted && !_loadInProgress)
			{ 
				_currentTab = _activeTabWhenPaintingStarted;
				InitCoverStartPositions();
				_loadInProgress = true;
				_currentLoadingGameScreenshots = GetGameIndex();
				[_whatsnewtab grayScreen];
				[_bestsellerstab grayScreen];
				[NSThread detachNewThreadSelector:@selector(loadScreenshots) toTarget:_whatsnewtab withObject:nil];
				
			}
			if (!_loadInProgress && _currentLoadedGameScreenshots != GetGameIndex())
			{
				_loadInProgress = true;
				_currentLoadingGameScreenshots = GetGameIndex();
				[_whatsnewtab grayScreen];
				[_bestsellerstab grayScreen];
				[NSThread detachNewThreadSelector:@selector(loadScreenshots) toTarget:_whatsnewtab withObject:nil];

			}
			if (_mustSet /*&& !m_fingerDown*/)
			{
				[_whatsnewtab setGame:GetGameList()[_currentLoadingGameScreenshots]];
				[_bestsellerstab setGame:GetGameList()[_currentLoadingGameScreenshots]];
				_mustSet = false;
			}

			if (!m_fingerDown)
			{
				_fingerDelta = blendDif(_fingerDelta, 0);
			}
			
			if (m_state == IGP_STATE_MAIN_CYCLING && (abs(_currentGamePositions[GetGameIndex()].x) < 10) && !m_fingerDown)
			{
				_fingerDelta = 0;
				m_fingerDown = false;
				for (int i = 0; i < GetGameCount(); i++)
				{
					_baseGamePositions[i] = _currentGamePositions[i];
					
				}
				setState(IGP_STATE_MAIN);
			}

			for (int i = 0; i < GetGameCount(); i ++)
			{
				float per = ((float)_fingerDelta / K_IGP_DISTANCE_BETWEEN_FINGER);
				float change = per * (i == GetGameIndex()?K_IGP_DISTANCE_TO_CENTER:K_IGP_DISTANCE_BETWEEN) / 5;
				
				_currentGamePositions[i].x -= blendDif(_currentGamePositions[i].x, (GetGamePosition(i).x) - change) ;
				_currentGamePositions[i].y  = K_IGP_SPLASH_Y;
				_currentGameLight[i]		= parabolic(abs(_currentGamePositions[i].x) - K_IGP_DISTANCE_TO_CENTER /2 , K_SCREEN_H, K_LIGHT_FULL, K_LIGHT_NONE);

				_currentGameAngles[i] -= blendDif(_currentGameAngles[i], -K_IGP_SPLASH_ANGLE_MAX * EXACTSIGN(i - GetGameIndex()));
				_currentGameScales[i] -= blendDif(_currentGameScales[i], i == GetGameIndex() ? K_IGP_SPLASH_SCALE_FACTOR2:K_IGP_SPLASH_SCALE_FACTOR);
			}
			break;
		}
		case IGP_STATE_RETRIEVING_LINK:
		{
			if ( !_cheatEnabled)   //pup a 	2180565 (don't show loading if is in cheat mode)
				[_activity setAlpha:1];
			switch ([_linkGrabber currentStatus])
			{
				case STATE_FAIL:
				case STATE_IDLE:
				{
					setState(m_prevState);
					
					NSString* s = IGPcStr::GetNSString(m_messageNumber);
					UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"" message:s delegate:nil cancelButtonTitle:IGPcStr::GetNSString(TXT_OK) otherButtonTitles: nil];
					[alert show];
					[alert release];
					[IGP::s_igpInstance->_catalogtab.m_infoViewController stopLoadingAnimation];
						//[IGP::s_igpInstance->_tab2.m_infoviewController.m_activity stopAnimating];
					m_touchEnabled = true;
					[_activity setAlpha:0];
					_window.userInteractionEnabled = true;
					break;
				}
				case STATE_WORKING:
				{
					break;
				}
				case STATE_SUCCESS:
				{
					if (!m_openedUrl)
					{
						NSString* url = [[NSString alloc] initWithUTF8String:[_linkGrabber getFinalLink]];
						[[UIApplication sharedApplication] openURL:[[NSURL alloc] initWithString:url]];
						//_window.userInteractionEnabled = true;
						m_openedUrl = true;
						
					}
					break;
				}
			}
			break;
		}
		default:
			break;
	}
}

CGPoint IGP::translateCoords(CGPoint coord, bool b) 
{
	return coord;
	/*
	CGPoint c;
	c.y = coord.x;
	c.x = coord.y;
	c.x-=rect.size.height/2;
	c.y-=rect.size.width/2;
	
	if (_flipX)
	{
		c.x = -c.x;
	}
	else if (_flipY)
	{
		c.y = -c.y;
	}
	return c;*/
}

void IGP::handleTouch(Touch touch)
{
	setOrientation();
	
	CGPoint touchLocation = touch.curLocation;
	CGPoint pTouchLocation = touch.prevLocation;
	m_lastX = touchLocation.x;
	m_lastY = touchLocation.y;
	
	if (touch.phase == UITouchPhaseBegan)
	{

		//if (!_cheatEnabled)
		{
			if (getCorner(touchLocation) == K_CHEAT_SEQUENCE[m_cheatPos])
			{
				m_cheatPos++;
				if (m_cheatPos >= strlen(K_CHEAT_SEQUENCE))
				{
					_cheatEnabled =  !_cheatEnabled;
					NSString *alMessage = [[NSString alloc] initWithString:K_CHEATMESSAGE];
					/*alMessage = [alMessage stringByAppendingString: @"\n "];
					
					for(int i=0;i < _allGamesCount;i++)
					{
						IGPGame *curGame = (IGP::s_igpInstance->_catalog)[i];
						alMessage = [alMessage stringByAppendingString:curGame->GetShortName()];
						alMessage = [alMessage stringByAppendingString: @" "];
						alMessage = [alMessage stringByAppendingString: @"CP: "];
						alMessage = [alMessage stringByAppendingString:[NSString stringWithFormat: @"%d", (*curGame).m_priority[0]]  ];
						
					}*/

					UIAlertView *alert = [[UIAlertView alloc] initWithTitle:K_CHEATTITLE message:alMessage delegate:nil cancelButtonTitle:IGPcStr::GetNSString(TXT_OK) otherButtonTitles: nil];
					[alert show];	
					[alert release];
					
				}
			}
			else
			{
				m_cheatPos = 0;
			}
		}
	}

	switch(m_state)
	{
		case IGP_STATE_MAIN:
		case IGP_STATE_MAIN_CYCLING:
		{
			switch(touch.phase)
			{
				case UITouchPhaseBegan:
				{
					if (pTouchLocation.y < 240) return;
					m_fingerDown	 = true;
					break;
				}
					
				case UITouchPhaseMoved:
				{
					
					if (m_state != IGP_STATE_MAIN_CYCLING)
					{
						for (int i = 0; i < GetGameCount(); i++)
						{
							_baseGamePositions[i] = _currentGamePositions[i];
						}
						setState(IGP_STATE_MAIN_CYCLING);
						_startDragIndex = GetGameIndex();
						m_stateLength = K_IGP_STATE_MAIN_CYCLING_TIME;
					}
					if (m_fingerDown)
					{
						touchLocation = translateCoords(touchLocation, true);
						pTouchLocation = translateCoords(pTouchLocation, true);
						_fingerDelta -= LIMIT(touchLocation.x - pTouchLocation.x, -50, 50);
					}
					if (abs(_fingerDelta) >= K_IGP_DISTANCE_BETWEEN_FINGER/2)
					{
						if (_fingerDelta > 0) 
						{
							if (GetGameIndex() < GetGameCount() - 1)
							{
								_curIndex[GetCoverIndex()] ++;
								for (int i = 0; i < GetGameCount(); i++)
								{
									_baseGamePositions[i] = _currentGamePositions[i];
								}
								_fingerDelta -= K_IGP_DISTANCE_BETWEEN_FINGER/2;
								m_stateLength = K_IGP_STATE_MAIN_CYCLING_TIME;
								return;
							}
							else
							{
								_fingerDelta = 0;
								return;
							}
						}
						if (_fingerDelta < 0)
						{

							if (GetGameIndex() > 0)
							{
								_curIndex[GetCoverIndex()] --;
								for (int i = 0; i < GetGameCount(); i++)
								{
									_baseGamePositions[i] = _currentGamePositions[i];
								}
								_fingerDelta += K_IGP_DISTANCE_BETWEEN_FINGER;
								m_stateLength = K_IGP_STATE_MAIN_CYCLING_TIME;
								return;
							}
							else
							{
								_fingerDelta = 0;
								return;
							}
						}
						for (int i = 0; i < GetGameCount(); i++)
						{
							_baseGamePositions[i] = GetGamePosition(i);
						}
					}
					break;
				}
				case UITouchPhaseCancelled:
				{
					_fingerDelta = 0;
					m_fingerDown	 = false;
					break;
					
				}
				case UITouchPhaseEnded:
				{
					m_fingerDown = false;
					if (m_state == IGP_STATE_MAIN)
					{
						
						if (abs(pTouchLocation.x - touchLocation.x) < K_TOUCHNOTMOVED_TOLERANCE)
						{
							for (int i = GetGameIndex() + 1; i < GetGameCount();i++)
							{
								if (GetGameList()[i]->GetSplash()->pointInRect(touchLocation.x, touchLocation.y))
								{
									_startDragIndex = GetGameIndex();
									_curIndex[GetCoverIndex()] = i;
									setState(IGP_STATE_MAIN_CYCLING);
									m_stateLength = K_IGP_STATE_MAIN_CYCLING_TIME * abs(GetGameIndex() - 1);
									return;
								}
							}
							
							//crash if this is nil
							if (!GetGameList()[GetGameIndex()])
								return;
							
							if (GetGameList()[GetGameIndex()]->GetSplash()->pointInRect(touchLocation.x, touchLocation.y))
							{
								for (int i = 0; i < GetGameCount(); i++)
								{
									_baseGamePositions[i] = GetGamePosition(i);
								}
								//Double tap on splash opens game information for currently selected game
							#ifdef USE_DOUBLE_TOUCH
								if (touch.tapCount == 2)
							#else
								if (touch.tapCount == 1)
							#endif 
								{
									[(WhatsNewViewController*)_currentTab openGameInformation];
								}
								return;
							}
							for (int i = GetGameIndex() - 1; i >= 0;i--)
							{
								if (GetGameList()[i]->GetSplash()->pointInRect(touchLocation.x, touchLocation.y))
								{
									_startDragIndex = GetGameIndex();
									_curIndex[GetCoverIndex()] = i;
									setState(IGP_STATE_MAIN_CYCLING);
									m_stateLength = K_IGP_STATE_MAIN_CYCLING_TIME * abs(GetGameIndex() - 1);
									return;
								}
							}
						}
					}	
					_fingerDelta = 0;

					for (int i = 0; i < GetGameCount(); i++)
					{
						_baseGamePositions[i] = _currentGamePositions[i];

					}
					break;
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
	
}

/*Used for cheat input*/
char IGP::getCorner(CGPoint p)
{
	p = translateCoords(p, false);
	int x, y;
	x = (p.x - K_CORNER1_X);
	y = (p.y - K_CORNER1_Y);
	if (x <=0 && y >=0)
	{
		return '1';
	}
	x = (p.x - K_CORNER2_X);
	y = (p.y - K_CORNER2_Y);
	if (x >=0 && y >= 50)
	{
		return '2';
	}
	return '3';

}

void IGP::InitTabs(UIView* glView)
{
	_reviewtab = [ReviewWriteViewController alloc];
	[_reviewtab performSelectorOnMainThread:@selector(initWithBackgroundImage:) withObject:IGP_IMAGE(m_bgImage) waitUntilDone:YES]; // initWithBackgroundImage: IGP_IMAGE(m_bgImage)];
	_whatsnewtab = [[WhatsNewViewController alloc] initWithView: glView coverIndex:0 viewId: 1];
	_bestsellerstab = [[WhatsNewViewController alloc] initWithView: glView coverIndex:1 viewId: 0];

	_catalogtab = [CatalogViewController alloc];
	[_catalogtab performSelectorOnMainThread:@selector(initWithBackgroundImage:) withObject:IGP_IMAGE(m_bgImage) waitUntilDone:YES]; // initWithBackgroundImage: IGP_IMAGE(m_bgImage)];
	//_tab3 = [[GameInformationViewController alloc] initWithBackgroundImage: _tabBackgroundImage];
	
	_tabBarController = [[UITabBarController alloc] init];
	_tabBarController.delegate = _whatsnewtab;

	[_tabBarController setViewControllers:[NSArray arrayWithObjects: _bestsellerstab, _whatsnewtab, _catalogtab, _reviewtab,nil]];
	//disable until all loaded
	_whatsnewtab.tabBarItem.enabled = NO;
	_catalogtab.tabBarItem.enabled = NO;
	_bestsellerstab.tabBarItem.enabled = NO;
	_reviewtab.tabBarItem.enabled = NO;

	[_window addSubview: _tabBarController.view];
//	NSLog(@"retain count RW: %d",[_tab5 retainCount]);
}

void IGP::FreeScreenshots()
{
	for (int i = 0; i < _allGamesCount; i++)
	{
		if (!NeedsScreenshotsForGame(i))
		{
			if (_allGames && _allGames[i])
			{
				_allGames[i]->ReleaseScreenshots();
			}
		}
	}
}

void IGP::LoadScreenshots()
{
	//salcideon - please don't crash
	if (!GetGameList()[_currentLoadingGameScreenshots])
	{
		_loadInProgress = false;
		return;
	}
		
	GetGameList()[_currentLoadingGameScreenshots]->LoadScreenshots();
	_currentLoadedGameScreenshots = _currentLoadingGameScreenshots;
	FreeScreenshots();
	_mustSet	= true;
	_loadInProgress = false;
}

int IGP::GetGameIndex()
{
	if (_activeTabWhenPaintingStarted == _whatsnewtab)
	{
		return _curIndex[0];
	}
	else if (_activeTabWhenPaintingStarted == _bestsellerstab)
	{
		return _curIndex[1];
	}
	else
	{
		return 0;
	}
}
int IGP::GetGameCount()
{
	if (_activeTabWhenPaintingStarted == _whatsnewtab)
	{
		return _whatsNewCount;
	}
	else if (_activeTabWhenPaintingStarted == _bestsellerstab)
	{
		return _bestSellersCount;
	}
	if (_activeTabWhenPaintingStarted == _catalogtab)
	{
		return _catalogCount;
	}
	else
	{
		return _tableCatalogCount;	
	}
}
int IGP::GetCoverIndex()
{
	if (_activeTabWhenPaintingStarted == _whatsnewtab)
	{
		return 0;
	}
	else if (_activeTabWhenPaintingStarted == _bestsellerstab)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
IGPGame** IGP::GetGameList()
{
	if (_activeTabWhenPaintingStarted == _whatsnewtab)
	{
		return _whatsNew;
	}
	else if (_activeTabWhenPaintingStarted == _bestsellerstab)
	{
		return _bestSellers;
	}
	if (_activeTabWhenPaintingStarted == _catalogtab)
	{
		return _catalog;
	}
	else
	{
		return _tableCatalog;	
	}

}

void IGP::MakeListOfGamesAccordingToPrio(IGPGame** sourceList, int sourceListCount, IGPGame** outList, int& outListCount, int prioIndex, bool centerToSides, int index)
{

	
	IGPGame* midList[sourceListCount];
	//Erase previous list
	outListCount = 0;
	for (int i = 0; i < sourceListCount; i++)
	{
		outList[i] = NULL;
	}
	//Add games to aux list
	for (int i = 0; i < sourceListCount; i++)
	{
		if (sourceList[i]->GetPriority(prioIndex) > 0)
		{
			midList[outListCount] = sourceList[i];
			outListCount++;
		}
	}

	//Sort the array
	int indexToPut;
	for (int i = 0; i < outListCount; i++)
	{
		int lPrioIndex = -1;
		int lPrio = 0XFFFFF;
		
		for (int j = 0; j < outListCount; j++)
		{
			if (midList[j] && midList[j]->GetPriority(prioIndex) < lPrio && midList[j]->GetPriority(prioIndex) > 0)
			{
				lPrioIndex = j;
				lPrio = midList[j]->GetPriority(prioIndex);

			}
		}
		
		if (centerToSides)
		{
			if (outListCount % 2 != 0)
			{
				indexToPut  = outListCount / 2+ (i % 2 == 0?1:-1) * (i + 1) / 2;
			}
			else
			{
				indexToPut  = outListCount / 2 + (i % 2 == 0?1:-1) * (i + 1) / 2;
			}
		}
		else
		{
			indexToPut = i;
		}
		outList[indexToPut] = midList[lPrioIndex];
		midList[lPrioIndex] = NULL;
	}
	if (index != -1)
	{
		_curIndex[index] = outListCount / 2 ;
	}
		
}

void IGP::savePricesStates()
{
	FILE *savePriceFile;
	char priceFileName[20];
	char priceFileNamePath[200];
	char tmp[1000];
	
	sprintf(priceFileName,"priceUpdate_%s_%s",IGP::s_igpDataLang,IGP::s_igpDataLocale);
	GetFileName(priceFileName, true, priceFileNamePath);
	
	savePriceFile = fopen(priceFileNamePath,"wb");
	
	m_newThings = 0;
	
	memset(tmp, 0, 1023);
	for(int i=0; i < _allGamesCount; i++)
	{
		sprintf(tmp, "%s%s%d",tmp,_allGames[i]->m_gameCode,_allGames[i]->m_priceUpdated?1:0);
//		if(_allGames[i]->m_priceUpdated)
//			m_newThings++;
		
		char fileSave[255];
		char fileSavePath[255];
		
		sprintf(fileSave, "%s_%s_save",_allGames[i]->m_gameCode,IGP::s_igpDataLang);
		GetFileName(fileSave, true, fileSavePath);
		
//		if(fileExists(fileSavePath))
//			m_newThings++;
	}
	
	fwrite(tmp, strlen(tmp), sizeof(char),savePriceFile);
	fclose(savePriceFile);
	
//	if(m_newThings>0)
//		_catalogtab.tabBarItem.badgeValue = [[NSString alloc] initWithFormat:@"%d",m_newThings];
//	else
//		_catalogtab.tabBarItem.badgeValue = nil;
	
	
}

bool IGP::getSavePriceStateForGame(char* codeOfGame)
{
	FILE *savePriceFile;
	char priceFileName[20];
	char priceFileNamePath[200];
	char tmp[1000];
	char *tmp2;
	int tmpLenght;
	
	sprintf(priceFileName,"priceUpdate_%s_%s",IGP::s_igpDataLang,IGP::s_igpDataLocale);
	GetFileName(priceFileName, true, priceFileNamePath);
	if (!fileExists(priceFileNamePath))
	{
		return false;
	}

	savePriceFile = fopen(priceFileNamePath,"rb");
	fseek(savePriceFile, 0, SEEK_END);
	tmpLenght = ftell(savePriceFile);
	rewind(savePriceFile);
	memset(tmp, 0, 1000);	
	fread(tmp,1,tmpLenght,savePriceFile);
	fclose(savePriceFile);
//	NSLog(@"savefile:%@", [[NSString alloc] initWithUTF8String:tmp]);
	
	tmp2 = strstr(tmp, codeOfGame);
//	NSLog(@"savefile:%@", [[NSString alloc] initWithUTF8String:tmp2]);
//	NSLog(@"savefile:%c", tmp2[4]);
	if (tmp2 !=nil)
		return ((tmp2[4]=='1'));
	else 
		return false;

}

void IGP::setPriceUpdatedValues()
{
	
	FILE*	cfg;
	char	cfgFileName[50];
	char	tmp[1023];
	char	len;
	int		priceLen;
//	int		prio;
	int		gamesCount;
	
	//reading old price to determine if we have a new price	
	//the old price should be in the IGPCFG_"lang"_"locale" of IGPConfig
	
		
	memset(cfgFileName, 0, 50); 
	sprintf(cfgFileName, "IGPCFG_%s_%s_old", IGP::s_igpDataLang, IGP::s_igpDataLocale);	
	 if (!fileExists(cfgFileName))
	 {
	 sprintf(cfgFileName, "IGPConfig");
	 }
	 cfg = openFile(cfgFileName);
	 
	 
	 memset(tmp, 0, 1023);
	 fread(tmp, 6, sizeof(char), cfg);
	 
	 if (strcmp(tmp, "v1.3.1"))
	 {
	 fclose(cfg);
	 }
	 
	 fread(&gamesCount, 1, sizeof(int), cfg);
	 
	 for (int i = 0; i < gamesCount; i++)
	 {
	 memset(tmp, 0, 1023);
	 fread(tmp, 4, sizeof(char), cfg);
	// _allGames[i]->SetCode(tmp);
	 char gcode[5];
	memset(gcode, 0,5);
	 strcpy(gcode,tmp);
	 
	 
	 for(int j = 0; j < PRIOMAX; j++)
	 {
	 fread(&tmp, 1, sizeof(int), cfg);
	 }
	 fread(&tmp, 1, sizeof(char), cfg);
	 //	_allGames[i]->SetHasNewPrice(len);
	 
	 
	 fread(&priceLen, 1, sizeof(int), cfg);
	 memset(tmp, 0, 1023);
	 fread(tmp, 1, priceLen, cfg);
	 
	 NSString *allPrices= [[NSString alloc] initWithCString:tmp encoding:NSUTF8StringEncoding];
	 
	 NSScanner *scanner = [NSScanner scannerWithString:allPrices];
	 [allPrices release];
	 allPrices = nil;
	 
	 NSString *baubau[200][2];
	 for (int kk = 0; kk < 200; kk++)
		 for (int jj = 0; jj < 2; jj++)
			 baubau[kk][jj] = nil;
		 
	 int couter1=0;
	 
	 while(![scanner isAtEnd])
	 {
		 if([scanner scanUpToCharactersFromSet:[NSCharacterSet characterSetWithCharactersInString:@":"] intoString:&baubau[couter1][0]]&&
			[scanner scanString:@":" intoString:NULL]&&
			[scanner scanUpToCharactersFromSet:[NSCharacterSet characterSetWithCharactersInString:@";"] intoString:&baubau[couter1][1]]&&
			[scanner scanString:@";" intoString:NULL])
			 couter1++;
	 }
		 
	NSString *countryloc = [[NSString alloc] initWithUTF8String:IGP::s_igpDataLocale];
	NSString *priceString = nil;
		 
	for(int kk=0;kk<couter1;kk++)
	{
		if(NSOrderedSame == [countryloc localizedCompare:baubau[kk][0]])
		
		if (priceString != nil)
		{
			[priceString release];
			priceString = nil;
		}
		priceString = [[NSString alloc] initWithString:baubau[kk][1]];
		
	}
	[countryloc release];
	
	if (priceString !=nil)
	{
		int gamePosition = -1;
		for(int ii=0;ii<_allGamesCount;ii++)
		{
		 if(strcmp(gcode,_allGames[ii]->m_gameCode) == 0)
			 gamePosition = ii;
		}
		 
		if(gamePosition > -1)
		{
			 if(NSOrderedSame == [priceString localizedCompare:_allGames[gamePosition]->m_gamePrice])
			 {
				 _allGames[gamePosition]->m_priceUpdated = getSavePriceStateForGame(gcode);//false;
			// if (_allGames[gamePosition]->m_priceUpdated)
				 NSLog(@"%d:%s:%s has price updated 1: %d",gamePosition,gcode, _allGames[gamePosition]->m_gameCode, _allGames[gamePosition]->m_priceUpdated?1:0);
			 } 
			else
				if(newCFGDownloaded)
				{
					_allGames[gamePosition]->m_priceUpdated = true;
					//	 if (_allGames[gamePosition]->m_priceUpdated)
					//	 NSLog(@"%s has price updated2",_allGames[gamePosition]->m_gameCode);
					NSLog(@"%d:%s:%s has price updated 2: %d",gamePosition,gcode, _allGames[gamePosition]->m_gameCode, _allGames[gamePosition]->m_priceUpdated?1:0);	 
				 }
				else
				{
					_allGames[gamePosition]->m_priceUpdated = getSavePriceStateForGame(gcode);
					// if (_allGames[gamePosition]->m_priceUpdated)
					//	 NSLog(@"%s has price updated3",_allGames[gamePosition]->m_gameCode);
					NSLog(@"%d:%s:%s has price updated 3: %d",gamePosition,gcode, _allGames[gamePosition]->m_gameCode, _allGames[gamePosition]->m_priceUpdated?1:0);
				}
		}
					 
	}
	
	 memset(tmp, 0, 1023);
	 fread(tmp, 32, sizeof(char), cfg);
	// _allGames[i]->SetMD5(tmp);
	// trace(DEBUG_LOADCONFIG, "MD5:%s\n",tmp);
		 
	 memset(tmp, 0, 1023);
	 fread(&len, 1, sizeof(char), cfg);
	 fread(tmp, len, sizeof(char), cfg);
	// _allGames[i]->SetURL(tmp);
	//trace(DEBUG_LOADCONFIG, "URL:%s\n",tmp);
	 memset(tmp, 0, 1023);
	 fread(&len, 1, sizeof(char), cfg);
	 fread(tmp, len, sizeof(char), cfg);
	// _allGames[i]->SetFileName(tmp);
	// trace(DEBUG_LOADCONFIG, "fileName:%s\n",tmp);
		 
	 [priceString release];
	 
	 }
	fclose(cfg);
	
	
}
void IGP::ReadConfig()
{
	FILE*	cfg;
	char	cfgFileName[50];
	char	tmp[1023];
	char	len;
	int		priceLen;
	int		prio;
	
	//TODO ACTIVAR UPDATER!
	sprintf(cfgFileName, "IGPCFG_%s_%s", IGP::s_igpDataLang, IGP::s_igpDataLocale);	
	if (!fileExists(cfgFileName))
	{
		sprintf(cfgFileName, "IGPConfig");
	}
	trace(DEBUG_LOADCONFIG,"Opened pack: %s\n",cfgFileName);
	cfg = openFile(cfgFileName);
	
	
	memset(tmp, 0, 1023);
	fread(tmp, 6, sizeof(char), cfg);
	
	if (strcmp(tmp, "v1.3.1"))
	{
		//printf("cfgFileName:%s\n", cfgFileName);
		trace(DEBUG_LOADCONFIG, "Old or invalid config file, version:\n", tmp);
//		NSFileManager* manager = [NSFileManager defaultManager];
		GetFileName(cfgFileName, true, tmp);
//		[manager removeFileAtPath:[NSString stringWithUTF8String:tmp] handler:nil];
		fclose(cfg);
		//ReadConfig();
	}
	
	
	fread(&_allGamesCount, 1, sizeof(int), cfg);
	
	
	trace(DEBUG_LOADCONFIG, "Loading %d games from %s\n", _allGamesCount, cfgFileName);
	for (int i = 0; i < _allGamesCount; i++)
	{
		memset(tmp, 0, 1023);
		fread(tmp, 4, sizeof(char), cfg);
		_allGames[i]->SetCode(tmp);
		trace(DEBUG_LOADCONFIG, "GameCode:%s\n", tmp);
		
		for(int j = 0; j < PRIOMAX; j++)
		{
			fread(&prio, 1, sizeof(int), cfg);
			_allGames[i]->SetPriority(prio, j);
		}
		fread(&len, 1, sizeof(char), cfg);
		_allGames[i]->SetHasNewPrice(len);
		
		fread(&priceLen, 1, sizeof(int), cfg);
		memset(tmp, 0, 1023);
		fread(tmp, 1, priceLen, cfg);
	
		NSString *allPrices= [[NSString alloc] initWithCString:tmp encoding:NSUTF8StringEncoding];
		
		NSScanner *scanner = [NSScanner scannerWithString:allPrices];
		[allPrices release];
		allPrices = nil;
		
		NSString *baubau[200][2];
		int couter1=0;

		while(![scanner isAtEnd]) {
		     if([scanner scanUpToCharactersFromSet:[NSCharacterSet characterSetWithCharactersInString:@":"] intoString:&baubau[couter1][0]]&&
			[scanner scanString:@":" intoString:NULL]&&
			[scanner scanUpToCharactersFromSet:[NSCharacterSet characterSetWithCharactersInString:@";"] intoString:&baubau[couter1][1]]&&
			[scanner scanString:@";" intoString:NULL])
					couter1++;
		}
		
		NSString *countryloc = [[NSString alloc] initWithUTF8String:IGP::s_igpDataLocale];
		
		for(int kk=0;kk<couter1;kk++)
		{
			if(NSOrderedSame == [countryloc localizedCompare:baubau[kk][0]])
				
				_allGames[i]->m_gamePrice = [[NSString alloc] initWithString:baubau[kk][1]];
				
		}
		
		[countryloc release];
		
		_allGames[i]->m_priceUpdated = false;
		memset(tmp, 0, 1023);
		fread(tmp, 32, sizeof(char), cfg);
		_allGames[i]->SetMD5(tmp);
		trace(DEBUG_LOADCONFIG, "MD5:%s\n",tmp);
		
		memset(tmp, 0, 1023);
		fread(&len, 1, sizeof(char), cfg);
		fread(tmp, len, sizeof(char), cfg);
		_allGames[i]->SetURL(tmp);
		trace(DEBUG_LOADCONFIG, "URL:%s\n",tmp);
		memset(tmp, 0, 1023);
		fread(&len, 1, sizeof(char), cfg);
		fread(tmp, len, sizeof(char), cfg);
		_allGames[i]->SetFileName(tmp);
		trace(DEBUG_LOADCONFIG, "fileName:%s\n",tmp);
	}
	
	fclose(cfg);
	
}

void IGP::Reinit()
{
	m_updatedGames++;
}

bool IGP::NeedsScreenshotsForGame(int i)
{
	if (_allGames[i] == _bestSellers[_curIndex[1]]) return true;
	if (_allGames[i] == _whatsNew[_curIndex[0]]) return true;
	return false;
}

void IGP::HideNotify()
{
	trace(DEBUG_INTERRUPTS, "HideNotify!\n");
	_window.userInteractionEnabled = false;
	[_catalogtab unhighlightIcons];
}

void IGP::ShowNotify()
{
	trace(DEBUG_INTERRUPTS, "ShowNotify!\n");
	if (!m_openedUrl)
	{
		_window.userInteractionEnabled = true;
	}
}
