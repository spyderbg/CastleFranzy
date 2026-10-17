//
//  WhatsNewViewController.m
//  IGP
//
//  Created by administrador on 22/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//
#include "IGP.h"
#include "IGPUtil.h"
#import "WhatsNewViewController.h"

static float  originalGlRect[4] = {-1,-1,-1,-1};


@implementation WhatsNewViewController

@synthesize m_screenshotImageView;

- (id)initWithView:(UIView*) glView coverIndex:(int) coverIndex viewId: (int) type
{
	m_type = type;
	m_originalGlView = glView;
	m_isChangingView = NO;
	CGRect cgRct = CGRectMake(0.0, 0.0, 320, 480); 
	if (self = [super init]) 
	{
		[self setNavigationBarHidden:YES];
		
		m_subVC = [[UIViewController alloc] init];
		
		[glView removeFromSuperview];
		m_coverIndex = coverIndex;
		
		if (m_coverIndex == 0)
		{
			self.title	  =IGPcStr::GetNSString(TXT_WHATS_NEW);
			m_subVC.title = IGPcStr::GetNSString(TXT_WHATS_NEW);
			self.tabBarItem.image = IGP_IMAGE(m_wnTabIcon);
		}
		else
		{
			self.title = IGPcStr::GetNSString(TXT_BESTSELLERS);
			m_subVC.title = IGPcStr::GetNSString(TXT_BESTSELLERS);
			self.tabBarItem.image = IGP_IMAGE(m_bsTabIcon);
			m_backButton = [[UIButton alloc] initWithFrame:CGRectMake(2, 5, 70, 36)];
			[m_backButton setImage: IGP_IMAGE(m_backButtonImage) forState:UIControlStateNormal];
			[ m_backButton setEnabled: NO];
			//hide the button from stressing chinamen
			[m_backButton setAlpha:0];
			m_screenshotBgImageView	= [[UIImageView alloc] initWithImage:IGP_IMAGE(m_screenshotBackgroundImage)];
		}

		
		m_subVC.view = [[UIView alloc] initWithFrame:cgRct];
		m_subVC.view.autoresizesSubviews = NO;
		[m_subVC.view addSubview: glView];

		//backup original view rect
		if(originalGlRect[2] == -1)//means it hasn't been initialized
		{
			originalGlRect[0] = glView.frame.origin.x;
			originalGlRect[1] = glView.frame.origin.y;
			originalGlRect[2] = glView.frame.size.width;
			originalGlRect[3] = glView.frame.size.height;
		}
		[glView setFrame:CGRectMake(0.0, 0.0, 320, 431)];
		
		//m_glLogoImage = [[UIImageView alloc] initWithImage:IGP_IMAGE(m_gameloftImage)];
		//[m_glLogoImage setFrame:CGRectMake(77, 4, m_glLogoImage.image.size.width, m_glLogoImage.image.size.height)];

		
		
		[self showTitleOneLine];
		
		m_screenshotImageView	= [[ScreenshotView alloc] init];
		
		m_activity = [[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
		[m_activity setFrame : SCREENSHOT_LOAD_RECT];
		[m_activity startAnimating];
#ifdef USE_INFO_BUTTON			
		m_infoButton= [[UIButton buttonWithType:UIButtonTypeCustom] retain];
		[m_infoButton setImage:IGP_IMAGE(m_infoButtonImage) forState:UIControlStateNormal];
		[m_infoButton setFrame:INFOBUTTON_RECT];
#endif	
		m_buyButton = [[UIButton buttonWithType:UIButtonTypeCustom] retain];
		[m_buyButton setFrame:BUYBUTTON_RECT];
		[m_buyButton setImage:IGP_IMAGE(m_buyButtonImage) forState:UIControlStateNormal];
		
	/*	if (cStr::isJapaneseCurrentLanguage(IGP::s_igpInstance->_currentLanguage))
		{
			//[m_buyButtonLabel setFont: [UIFont fontWithName:@"Papyrus" size:14]];
			m_buyButtonLabel = [[UILabel alloc] initWithFrame:CGRectMake(m_buyButton.frame.origin.x + 29,m_buyButton.frame.origin.y+  (m_buyButton.frame.size.height/2) - 26, m_buyButton.frame.size.width - 25, 60)];
		}
		else*/
			m_buyButtonLabel = [[UILabel alloc] initWithFrame:CGRectMake(m_buyButton.frame.origin.x + 33,m_buyButton.frame.origin.y+  (m_buyButton.frame.size.height/2) - 29, m_buyButton.frame.size.width - 36, 60)];

		
			
		
		[m_buyButtonLabel setBackgroundColor:[UIColor clearColor]];
		
		[m_buyButtonLabel setAdjustsFontSizeToFitWidth: YES];
		m_buyButtonLabel.textAlignment = UITextAlignmentCenter;
		
		
		
		[m_buyButtonLabel setText: IGPcStr::GetNSString(TXT_BUY)];
		[m_buyButtonLabel setTextColor:[UIColor whiteColor]];
		[m_buyButtonLabel setBackgroundColor:[UIColor clearColor]];
		//int a =cStr::getLanguageBinSizeOnIndex(0);
		//sendLabel.font = [UIFont fontWithName:@"Times New Roman" size:14 - (sendLabel.text.length - 4)];
		[m_buyButtonLabel setAdjustsFontSizeToFitWidth: YES];

		
		
		m_buyButtonLabel.textAlignment = UITextAlignmentCenter;
		
	
		m_videoButton = [[UIButton buttonWithType:UIButtonTypeCustom] retain];
		[m_videoButton setFrame:VIDEOBUTTON_RECT];
		[m_videoButton setImage:IGP_IMAGE(m_videoButtonImage) forState:UIControlStateNormal];
		
	
		[m_screenshotBgImageView setUserInteractionEnabled:YES];
		[m_screenshotBgImageView setFrame:SCREENSHOT_BG_RECT];
		[m_screenshotImageView setFrame:SCREENSHOT_RECT];
		[m_screenshotImageView setBackgroundColor:[UIColor clearColor]];

		
		[m_screenshotBgImageView addSubview:m_screenshotImageView];
		[m_buyButton addTarget:self action:@selector(openBuyLink:) forControlEvents:UIControlEventTouchUpInside];
#ifdef USE_INFO_BUTTON		
		[m_infoButton addTarget:self action:@selector (openGameInformation2) forControlEvents:UIControlEventTouchUpInside];
#endif
		[m_videoButton addTarget:self action:@selector(openVideoLink:) forControlEvents:UIControlEventTouchUpInside];
		[m_backButton addTarget:self action:@selector(quitIGP:) forControlEvents:UIControlEventTouchUpInside];

		if (m_coverIndex == 1)
		{	
			[glView addSubview:m_screenshotBgImageView];
#ifdef USE_INFO_BUTTON					
			[glView addSubview:m_infoButton];
#endif

			[glView addSubview:m_buyButton];
			[glView addSubview:m_buyButtonLabel];
			[glView addSubview:m_videoButton];
			//[glView addSubview:m_glLogoImage];
			[glView addSubview:m_backButton];

		}
	
		m_currentGame = NULL;
		m_screenshotChangeTimer =  [NSTimer scheduledTimerWithTimeInterval:(1.0/20.0) target:self selector:@selector(changeScreen) userInfo:nil repeats:YES];
		m_isLoading = false;
		[self hide];
		[self pushViewController:m_subVC animated:YES];
			[m_screenshotImageView addSubview:m_activity];
		
	}
	return self;
}

- (void) hide
{
	[m_titleLabel[0] setAlpha:0];
	[m_titleLabel[1] setAlpha:0];
	[m_buyButton setAlpha:0];
#ifdef USE_INFO_BUTTON			
	[m_infoButton setAlpha:0];
#endif
	[m_videoButton setAlpha:0];
	[m_screenshotBgImageView setAlpha:0];
	[m_titleLabel[0] setAlpha:0];
	[m_titleLabel[1] setAlpha:0];
	[m_buyButtonLabel setAlpha:0];
}

- (void) show
{
	if (m_coverIndex == 1)
	{
		[m_titleLabel[0] setAlpha:1];
		[m_titleLabel[1] setAlpha:1];
#ifdef USE_INFO_BUTTON		
		[m_infoButton setAlpha:1];
#endif
		[m_buyButton setAlpha:1];
		[m_videoButton setAlpha:1];
		[m_screenshotBgImageView setAlpha:1];
		[m_titleLabel[0] setAlpha:1];
		[m_titleLabel[1] setAlpha:1];
		[m_buyButtonLabel setAlpha:1];
	}
}
-(void) openGameInformation2
{
	if (IGP::s_igpInstance->_currentTab != NULL)
	{
		[(WhatsNewViewController*)IGP::s_igpInstance->_currentTab openGameInformation];
	}
	
}

- (void) refreshMe
{
	//empty, needed to avoid crash.
}

- (void) openGameInformation
{
	if (m_currentGame == NULL)
		return;
	IGP::s_igpInstance->FreeScreenshots();
	m_currentGame->LoadScreenshots();

	if (m_coverIndex == 1)
	{
		[[IGP::s_igpInstance->_catalogtab m_infoViewController] setFrom:"BS&ctg=INFO"];
	}
	else
	{
		[[IGP::s_igpInstance->_catalogtab m_infoViewController] setFrom:"WN&ctg=INFO"];
	}

	[[IGP::s_igpInstance->_catalogtab m_infoViewController] setGame:m_currentGame from:(UINavigationController*)self isNewPrice:m_currentGame->m_priceUpdated];
	
	m_currentGame->m_priceUpdated = false;
	char fileSave[255];
	char fileSavePath[255];
	
	sprintf(fileSave, "%s_%s_save",m_currentGame->m_gameCode,IGP::s_igpDataLang);
	GetFileName(fileSave, true, fileSavePath);
	
	if(fileExists(fileSavePath))
	{
		remove(fileSavePath);
	}
	IGP::s_igpInstance->savePricesStates();
	[IGP::s_igpInstance->_catalogtab loadIconList];
	
	[self pushViewController:[IGP::s_igpInstance->_catalogtab m_infoViewController] animated:YES];

}


- (void)viewDidDisappear:(BOOL)animated
{
	[m_titleLabel[0] setAlpha:0];
	[m_titleLabel[1] setAlpha:0];
	//this was made to cover for a retain inc that happens on each view enter
	[self release];
	

}



- (void)viewDidAppear:(BOOL)animated
{
	IGP::s_igpInstance->m_selectedIndex = m_type;

	if (IGP::s_igpInstance->m_state == IGP_STATE_MAIN || IGP::s_igpInstance->m_state == IGP_STATE_MAIN_CYCLING)
	{//bug 	2252568
		[m_titleLabel[0] setAlpha:1];
		[m_titleLabel[1] setAlpha:1];
	}
	m_isChangingView = false;
	
}

- (void)viewWillAppear:(BOOL)animated
{
	[m_subVC.view addSubview: IGP::s_igpInstance->_view];

	if (IGP::s_igpInstance->m_state == IGP_STATE_MAIN)
	{
		[m_titleLabel[0] setAlpha:1];
		[m_titleLabel[1] setAlpha:1];
	}

	if (self.view != m_subVC.view)
	{
		[self popViewControllerAnimated:animated];
	}
	
	/*if(m_coverIndex == 0)
		[glview addSubview:m_backButton]; */

	[super viewWillAppear:animated];
}

-(void) showTitleOneLine
{
	
	
	m_titleLabel[0] = nil;
	NSString *labelTxt = m_coverIndex == 0? IGPcStr::GetNSString(TXT_WHATS_NEW): IGPcStr::GetNSString(TXT_BESTSELLERS_TITLES);	
	m_titleLabel[0] = [[UILabel alloc] initWithFrame:CGRectMake(90,12,220,25)];
	[m_titleLabel[0] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
	[m_titleLabel[0] setText:labelTxt];
	m_titleLabel[0].textColor=[UIColor colorWithRed:242/255.0 green:62/255.0 blue:32/255.0 alpha:1];
	[m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:22]];
	m_titleLabel[0].textAlignment= UITextAlignmentRight;
	[IGP::s_igpInstance->_view addSubview:m_titleLabel[0]];
	
}


-(void) showTitle
{
		
		NSString *labelTxt = m_coverIndex == 0? IGPcStr::GetNSString(TXT_WHATS_NEW): IGPcStr::GetNSString(TXT_BESTSELLERS_TITLES);
		NSArray *labelAux=[labelTxt componentsSeparatedByString:@" "];
		int posxStartLabel=120;
		NSString *labelUP=NULL;
		NSString *labelDown=NULL;
		if ([labelAux count]>1)
		{
			labelUP= [labelAux objectAtIndex:0];
			m_titleLabel[0] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel+20,0,160,20)];
			[m_titleLabel[0] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
			[m_titleLabel[0] setText:labelUP];
			m_titleLabel[0].textColor=[UIColor colorWithRed:242/255.0 green:62/255.0 blue:32/255.0 alpha:1];
			[m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:19]];
			m_titleLabel[0].textAlignment= UITextAlignmentRight;
			[IGP::s_igpInstance->_view addSubview:m_titleLabel[0]];
		}

		if ([labelAux count]==1)
		{
			m_titleLabel[1] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel,10,180,20)];
			labelDown= [labelAux objectAtIndex:0];
			[m_titleLabel[1] setFont:[UIFont boldSystemFontOfSize:22]];
		}
		else if([labelAux count]>=2)//concatenate string [2] " " and [3] and ....
		{
			m_titleLabel[1] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel,19,180,20)];
			labelDown=[labelAux objectAtIndex:1];
			for (int i=2;i<[labelAux count];i++)
			{
				labelDown=[labelDown stringByAppendingString:@" "];
				labelDown=[labelDown stringByAppendingString:[labelAux objectAtIndex:i]];	
			}
			[m_titleLabel[1] setFont:[UIFont boldSystemFontOfSize:19]];
		}
	
		[m_titleLabel[1] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
		[m_titleLabel[1] setText:labelDown];
		m_titleLabel[1].textColor=[UIColor colorWithRed:242/255.0 green:62/255.0 blue:32/255.0 alpha:1];
		m_titleLabel[1].textAlignment= UITextAlignmentRight;
		[IGP::s_igpInstance->_view addSubview:m_titleLabel[1]];
	}

- (void) grayScreen
{
	m_isLoading = true;
	[m_screenshotImageView setFrame:SCREENSHOT_RECT];
	[m_screenshotImageView setImage:nil];
	[m_screenshotImageView setBackgroundColor:[UIColor clearColor]];
//	[m_screenshotImageView addSubview:m_activity];
}

- (void) loadScreenshots
{	
	
	if (IGP::s_igpInstance->m_state != IGP_STATE_END) //pup m avoid crash
	{
		NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
		if (IGP::s_igpInstance) IGP::s_igpInstance->LoadScreenshots();
		[pool release];
	}
}

- (bool) shouldRender
{
	return [self visibleViewController] == m_subVC;
}

- (void) changeScreen
{
	if (!m_isLoading)
	{
		if (CFAbsoluteTimeGetCurrent() - m_lastChange> 4)
		{
			if (m_currentGame)
			{
				m_lastChange = CFAbsoluteTimeGetCurrent();
				m_currentScreenshot = (m_currentScreenshot + 1) % 3;
				if (m_currentGame->IsLandscape(m_currentScreenshot))
				{
					[m_screenshotImageView setFrame:SCREENSHOT_RECT_L];
				}
				else
				{
					[m_screenshotImageView setFrame:SCREENSHOT_RECT];
				}
				[m_screenshotImageView setImage:m_currentGame->GetScreenshot(m_currentScreenshot)];
			}
		}
		if (CFAbsoluteTimeGetCurrent() - m_lastChange > 3)
		{
			[m_screenshotImageView setAlpha:lineal(CFAbsoluteTimeGetCurrent() - m_lastChange - 3, 1, K_ALPHA_FULL, K_ALPHA_NONE)];
		}
		else
		{
			[m_screenshotImageView setAlpha:lineal(CFAbsoluteTimeGetCurrent() - m_lastChange, 1, K_ALPHA_NONE, K_ALPHA_FULL)];
		}
	}
	else
	{
		[m_screenshotImageView setAlpha:K_ALPHA_FULL];
	}

}

- (void) setGame:(IGPGame*) game
{
	m_currentGame		= game;
	m_currentScreenshot = 0;
	if (m_currentGame->IsLandscape(m_currentScreenshot))
	{
		[m_screenshotImageView setFrame:SCREENSHOT_RECT_L];
	}
	else
	{
		[m_screenshotImageView setFrame:SCREENSHOT_RECT];
	}
	[m_activity setFrame : SCREENSHOT_LOAD_RECT];
//	[m_activity removeFromSuperview];
	[m_activity stopAnimating];
	m_isLoading = false;
	m_lastChange = CFAbsoluteTimeGetCurrent();
	[m_screenshotImageView setAlpha:K_ALPHA_NONE];
	[m_screenshotImageView setImage:m_currentGame->GetScreenshot(m_currentScreenshot)];
	
	if(m_currentGame->GetPrice())
		[m_buyButtonLabel setText:m_currentGame->GetPrice()];
	else
		[m_buyButtonLabel setText: IGPcStr::GetNSString(TXT_BUY)];
	
	if(m_currentGame->m_hasNewPrice)
		[m_buyButtonLabel setTextColor:[UIColor yellowColor]];
	else
		[m_buyButtonLabel setTextColor:[UIColor whiteColor]];
	
	[m_originalGlView addSubview:m_buyButtonLabel];
	[m_buyButtonLabel setNeedsDisplay];
	

}

- (IBAction) openBuyLink:(id) sender
{
	if (!m_isLoading)
	{
		m_currentGame->OpenBuyLink(IGP::s_igpInstance->_currentTab == IGP::s_igpInstance->_whatsnewtab?"WN":"BS");
	}

}

- (IBAction) openVideoLink:(id) sender
{
	if (!m_isLoading)
	{
		m_currentGame->OpenVideoLink(IGP::s_igpInstance->_currentTab == IGP::s_igpInstance->_whatsnewtab?"WN":"BS");
	}
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)killTimer
{
	[self popToRootViewControllerAnimated:NO];
	trace(DEBUG_OBJCMEM, "Killing timer\n");
	self.tabBarItem.image = nil;
	[m_screenshotChangeTimer invalidate];
	
	[m_screenshotImageView removeFromSuperview];
	if (m_coverIndex == 1)
	{
		[m_backButton setImage: nil forState:UIControlStateNormal];
		[m_buyButton setImage: nil forState:UIControlStateNormal];
		[m_videoButton setImage: nil forState:UIControlStateNormal];
#ifdef USE_INFO_BUTTON		
		[m_infoButton setImage: nil forState:UIControlStateNormal];
#endif
		[m_backButton removeFromSuperview];	
		[m_videoButton removeFromSuperview];
#ifdef USE_INFO_BUTTON		
		[m_infoButton removeFromSuperview];
#endif
		[m_buyButton removeFromSuperview];
		[m_buyButtonLabel removeFromSuperview];
		
		[m_screenshotBgImageView removeFromSuperview];
		
		[m_activity removeFromSuperview];

			
	}
	
	if (m_titleLabel[0])
	{
		[m_titleLabel[0] removeFromSuperview];
		
	}
	if (m_titleLabel[1])
	{
		[m_titleLabel[1] removeFromSuperview];
		
	}
}
- (void)didReceiveMemoryWarning
{
	trace(DEBUG_OBJCMEM, "WhatsNewViewController threw memory warning\n");
    [super didReceiveMemoryWarning];
}

- (void ) quitIGP:(id) sender
{
	
	if ( IGP::s_igpInstance->m_state == IGP_STATE_RETRIEVING_LINK || m_isChangingView)
		return;
	m_backButton.enabled = NO;
	[IGP::s_igpInstance->_tabBarController setSelectedIndex:0];
	IGP::s_igpInstance->_whatsnewtab.tabBarItem.enabled = NO;
	IGP::s_igpInstance->_catalogtab.tabBarItem.enabled = NO;
	IGP::s_igpInstance->_bestsellerstab.tabBarItem.enabled = NO;
	IGP::s_igpInstance->_reviewtab.tabBarItem.enabled = NO;
	//self.tabBar = Nil;
	[IGP::s_igpInstance->_tabBarController setViewControllers: nil];
	IGP::s_igpInstance->setState(IGP_STATE_END);
}


- (void)dealloc
{
	[m_screenshotBgImageView setImage:nil];
	[m_subVC.view release];
	[m_titleLabel[0] release];
	[m_titleLabel[1] release];	
	if (m_backButton) [m_backButton release];
	//[m_glLogoImage release];
	[m_activity release];
	[m_buyButtonLabel release];
#ifdef USE_INFO_BUTTON		
	[m_infoButton release];
#endif
	[m_buyButton release];
	[m_videoButton release];
	//[m_screenshotImageView release];
	[m_screenshotBgImageView release];
	//reestablish original rect
	
	[m_originalGlView setFrame:CGRectMake(originalGlRect[0], originalGlRect[1], originalGlRect[2], originalGlRect[3])];
	
    [super dealloc];
}

- (void ) enableBackButton
{
	if(m_coverIndex == 1 && m_backButton.superview != self.view)
	{
		[m_backButton setEnabled: YES];
		[m_backButton setAlpha:1];
		
	}
	
}

- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController
{
/*	IGP::s_igpInstance->_whatsnewtab.tabBarItem.enabled = NO;
	IGP::s_igpInstance->_catalogtab.tabBarItem.enabled = NO;
	IGP::s_igpInstance->_bestsellerstab.tabBarItem.enabled = NO;
	IGP::s_igpInstance->_reviewtab.tabBarItem.enabled = NO;*/
/*	IGP::s_igpInstance->_reviewtab.m_backButton.enabled = NO;
	m_backButton.enabled = NO;
	m_backButton.alpha = 0;*/
	//IGP::s_igpInstance->review

	//salcideon
    NSString* firmware = [[UIDevice currentDevice] systemVersion];
    NSString* major = [firmware substringToIndex:1];
    int sdkMajorVal = [major intValue];
    //NSLog(@"Major val %d\n\n", sdkMajorVal);
    if (sdkMajorVal >= 3)
	{
		[viewController retain];
	}
	else
	{
		//nothing
	}
	if (IGP::s_igpInstance->m_state == IGP_STATE_MAIN)
		m_isChangingView = YES;
	m_backButton.enabled = NO;


}

- (BOOL) isChangingView
{
	return m_isChangingView;
}

- (void) setChangingView:(BOOL)value
{
	m_isChangingView = value;
}

@end
