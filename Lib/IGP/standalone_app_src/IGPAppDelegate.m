
#import "IGPAppDelegate.h"
#import "EAGLView.h"
#import "IGPUtil.h"
#import "IGPSendInfo.h"
#import "Reachability.h"
@implementation IGPAppDelegate

@synthesize window;

// Sets up the frame rate and starts animating the sprite.
- (void)applicationDidFinishLaunching:(UIApplication *)application 
{
	igp = NULL;
	[[UIApplication sharedApplication] setStatusBarHidden:YES];
//	[[UIApplication sharedApplication] setApplicationIconBadgeNumber:6];
	int len;
	char * buff;
	char lang[3] = {0};
	
	FILE* bgPng = openFile("bg.png");
	len = ftell(bgPng);
	fseek( bgPng, 0L, SEEK_END);
	len = ftell(bgPng) - len + 1;
	fseek( bgPng, 0L, SEEK_SET);
	buff = new char[len];
	fread(buff, sizeof(char), len, bgPng);
	bg  = new IGPTexture(buff, len, false, K_FORMAT_RGB888);
	SAFE_DELETE_ARRAY(buff);
	
	FILE* res = openFile("igpdata");
	fseek(res, 11, SEEK_SET);					//skip number of games
	fread(&langs, sizeof(char), 1, res);		//read number of languages
	buttons = new UIButton*[langs];
	for(int i = 0; i < langs;i++)
	{
		fread(lang, sizeof(char), 2, res);		//read 1 lang
		buttons[i] = [self roundedButtonWithText:[[NSString alloc] initWithUTF8String:lang] andIndex:i];
	}
	fclose(res);
	IGPSendInfo* info = new IGPSendInfo();
	info->IGPSendGameInfo(3, "1.2.2");
	
	delete info;
	info = nil;
	
	glClearColor(0, 0, 0, 0);
	application = application;
	[self initInfoBox];
	[self enableLanguageButtons];
	[NSTimer scheduledTimerWithTimeInterval:(1.0 / 60.0) target:self selector:@selector(draw) userInfo:nil repeats:YES];

}

- (void) initInfoBox
{
	bool wifiAvailable = false;
	bool EDGEAvailable = false;
	[[Reachability sharedReachability] setHostName:@"www.google.com"];
		int currentConnection = [[Reachability sharedReachability] internetConnectionStatus] ;
		//pup m make it available for EDGE also
	switch(currentConnection)	
	{
		case NotReachable:
			break;
		case   ReachableViaCarrierDataNetwork:
				EDGEAvailable = true;
				break;
		case ReachableViaWiFiNetwork:
			wifiAvailable= true;		
			break;
	}
/*	if ([[Reachability sharedReachability] internetConnectionStatus] > NotReachable)
	{
		if 
		wifiAvailable = true;
	}*/
	
	CGRect frame = CGRectMake(0, 140, 320, 240);
	
	infoView = [[[UITextView alloc] initWithFrame:frame] autorelease];

	infoView.textColor = [UIColor blackColor];
	infoView.backgroundColor = [UIColor clearColor];
	infoView.text = [@"Country:" stringByAppendingString:[[NSLocale currentLocale] objectForKey:NSLocaleCountryCode]];
	infoView.text = [infoView.text stringByAppendingString:@"\nWifi:"];
	infoView.text = [infoView.text stringByAppendingString:(wifiAvailable?@"Yes":@"No")];
	infoView.text = [infoView.text stringByAppendingString:@"\nEDGE:"];
	infoView.text = [infoView.text stringByAppendingString:(EDGEAvailable?@"Yes":@"No")];
	infoView.font = [UIFont fontWithName:@"Arial" size:12];
	infoView.alpha = 1;
	infoView.textColor = [UIColor blackColor];
	infoView.returnKeyType = UIReturnKeyDefault;
	[infoView setEditable:NO];
	[window addSubview:infoView];
}

- (void) enableLanguageButtons
{
	for(int i = 0; i < langs; i++)
	{
		[window addSubview:buttons[i]];
	}
	infoView.frame = CGRectMake(0, 140, 320, 240);
}

- (void) disableLanguageButtons
{
	for(int i = 0; i < langs; i++)
	{
		[buttons[i] removeFromSuperview];
	}
	infoView.frame = CGRectMake(0, 0, 0, 0);
}

- (UIButton*)roundedButtonWithText:(NSString*)text andIndex:(int)index
{
	UIButton* b = [[UIButton buttonWithType:UIButtonTypeRoundedRect] retain];
	b.frame = CGRectMake(index * 40, 0.0, 40, 100);
	[b setTitle:text forState:UIControlStateNormal];
	b.backgroundColor = [UIColor clearColor];
	[b addTarget:self action:@selector(myAction:) forControlEvents:UIControlEventTouchUpInside];
	return b;
}

-(IBAction)myAction:(id)sender
{
	for(int i = 0; i < langs; i++)
	{
		if (sender == buttons[i])
		{
			[self initIGPWithLanguageIndex:i];
		}
	}
}

- (void) initIGPWithLanguageIndex:(int) index
{
	[self disableLanguageButtons];	
	igp = new IGP(application, window, glView, index);

}

- (void) draw
{
	if (igp == NULL || igp->MustRender())
	{
		[glView pre];
		glViewport(0, 0, K_SCREEN_W, K_SCREEN_H);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrthof(-K_SCREEN_W/2, K_SCREEN_W/2, K_SCREEN_H/2, -K_SCREEN_H/2, K_FRUSTRUM_Z_NEAR, K_FRUSTRUM_Z_FAR);
		glMatrixMode(GL_MODELVIEW);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (!igp)
		{
			
			bg->paint(0, 0, 0, 1, 0 , 1, 1, false);
		}
		//YOUR STATE MACHINE
		
		//	switch(_state)
		//	{
		//		case STATE_IGP:
		//		{
		//Here you should paint your splash or menu for it to be seen behind igp window.
		if (igp)
		{
			bool b = igp->PaintIGP();
			if (b)
			{
				SAFE_DELETE(igp);
				//IGP::ReleaseRemainingStuff();
				[self performSelector:@selector(enableLanguageButtons) withObject:nil afterDelay:0];
//				[self enableLanguageButtons];
			}
		}
		//			break;
		//		}
		//	}
		
		[glView post];
	}

}

- (void)applicationWillResignActive:(UIApplication *)application {
	application = application;
	glView.animationInterval = 1.0 / 5.0;
	if (igp)
	{
		igp->HideNotify();
	}
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	application = application;
	glView.animationInterval = 1.0 / 120.0;
	if (igp)
	{
		igp->ShowNotify();
	}
}

- (void)dealloc {
	SAFE_DELETE(igp);
	SAFE_DELETE_ARRAY(buttons);
	[glView stopAnimation];
	[window release];
	[super dealloc];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return NO;
}


@end
