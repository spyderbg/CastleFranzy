//
//  WhatsNewViewController.h
//  IGP
//
//  Created by administrador on 22/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "IGPDefines.h"
#import "ScreenshotView.h"
#include "IGPGame.h"
class IGP;

@interface WhatsNewViewController : UINavigationController<UITabBarControllerDelegate> {

	IGPGame*					m_currentGame;							//Pointer to game of which we show screenshots
	int							m_currentScreenshot;					//Index of current screenshot
	int							m_coverIndex;
	CFTimeInterval				m_lastChange;							//Time since last change
	bool						m_isLoading;
	
	//Allocated data
	UIImageView*				m_screenshotBgImageView;
	ScreenshotView*				m_screenshotImageView;
	UIActivityIndicatorView*	m_activity;
	NSTimer*					m_screenshotChangeTimer;				//Invalidated on release
	UILabel*					m_titleLabel[2];
	UIViewController*			m_subVC;
	UIButton*					m_buyButton;
	UIButton*					m_videoButton;
#ifdef USE_INFO_BUTTON		
	UIButton*					m_infoButton;
#endif
	UILabel*					m_buyButtonLabel;
	//UIImageView*				m_glLogoImage;
	UIButton*					m_backButton;
	BOOL						m_isChangingView;						//wether is changing the current uiviewcontroller at the time (tab bat pressed)
	UIView*						m_originalGlView;					//glview that game passes as parameter
	
	int 						m_type;
}

@property 	(nonatomic,retain) ScreenshotView*		m_screenshotImageView;

- (id)initWithView:(UIView*) glView coverIndex:(int) coverIndex viewId:(int) type;

- (void) killTimer;
- (void) hide;
- (void) show;
- (void) changeScreen;
- (void) loadScreenshots;
- (void) grayScreen;
- (void) setGame:(IGPGame*) game;
- (void) showTitle;
- (bool) shouldRender;
- (IBAction) openBuyLink:(id) sender;
- (IBAction) openVideoLink:(id) sender;
- (void) openGameInformation;
-(void) showTitleOneLine;
- (void ) enableBackButton;
- (BOOL) isChangingView;
- (void) setChangingView:(BOOL)value;
//- (void) quitIGP;
@end
