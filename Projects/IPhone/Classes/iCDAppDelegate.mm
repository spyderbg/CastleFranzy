//
//  iCDAppDelegate.mm
//
//  Castle Defence
//
//  Created by Plamen Chernev on 03/09/09.
//  Copyright Gameloft 2009. All rights reserved.
//

#import "iCDAppDelegate.h"
#import "EAGLView.h"

#include "Common/StringTable.h"
#import "Core/Application.h"
#import "IO/TouchScreen/TouchPad.h"
#import "IO/Accelerometer/AccelerometerBase.h"

IGP*				igp;
int					_launchIGPLang;

CGLLive*			glLive;
int					_launchGLLiveLang;

iCDAppDelegate*		appDelegatePtr;
UIApplication*		_application;
UIWindow*			_window;
EAGLView*			_glView;

UIAccelerationValue	m_accelerometer[3];


static int s_deviceType = -1; 

char devTokenStr[128] = {0};

int g_nAppExit = 0;

///////////////////////////////////////////////////////////////////////

@implementation iCDAppDelegate

@synthesize window;
@synthesize glView;

//externals 

extern "C++" void SetCurrentDeviceModel(char *modelString);


///////////////////////////////////////////////////////////////////////
/// Application
///////////////////////////////////////////////////////////////////////

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
	glLive				= NULL;
	appDelegatePtr		= self;
	_application		= application;
	_launchIGPLang		= -1;
	_launchGLLiveLang	= -1;
	_window				= window;
	_glView				= glView;
	
	[application setStatusBarHidden:YES animated:NO];
	//[application setStatusBarOrientation:UIInterfaceOrientationLandscapeRight];
	
	//Configure and start accelerometer
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / 10)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
	[self enableAccelerometer:YES];


	glView.animationInterval = UPDATE_TIMER;
	[glView startAnimation];
	
	char temp[256];	
	strcpy(temp, [ [UIDevice currentDevice].model cString]);
	SetCurrentDeviceModel(temp);
	
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	g_nAppExit = 1; 
	
	__APP()->Quit();	
	
	[glView stopAnimation];	
}

-(void)applicationWillResignActive:(UIApplication *)application
{	
	IGP_MUTEX_LOCK
	
	glView.animationInterval = 1.0 / 5.0;
	
	[glView pause];

	IGP_MUTEX_UNLOCK	
}

-(void)applicationDidBecomeActive:(UIApplication *)application
{
	IGP_MUTEX_LOCK
	
	glView.animationInterval = 1.0 / 60.0;
	
	[glView resume];
	
	IGP_MUTEX_UNLOCK	
}

///////////////////////////////////////////////////////////////////////

extern char devTokenStr[128];
void convertBufferToStr( const void * buffer, int size, char* str)
{	
	char map[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	
	int i = 0;
	for(; i < size; ++i)
	{
		char ch = ((char*)buffer)[i];
		
		str[i*2] = map[(ch>>4)&0x0f] ;
		str[i*2+1] = map[ch&0x0f];
	}
	str[i*2] = '\0';
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
	NSLog(@"deviceToken: %@", deviceToken); 
	
	convertBufferToStr([deviceToken bytes], [deviceToken length], devTokenStr);
}

- (void)dealloc {
	[window release];
	[glView release];
	[super dealloc];
}

+(iCDAppDelegate*)sharedInstance
{
	return appDelegatePtr;
} 


-(void)initTextInput:(TextInputInitParams*)params 
{
	[params->ti initWithTextBuffer:(char*)params->textBuff WithSize:params->buffSize AtX:params->x Y:params->y Width:params->w Height:params->h PasswordFlag:params->pass Orientation:params->orientationTf];
}

-(void)removeTextField:(TextInput*)params 
{
	[params Remove];
}


///////////////////////////////////////////////////////////////////////
/// Accelerated
///////////////////////////////////////////////////////////////////////

- (void) enableAccelerometer:(BOOL)enable {
	if (enable)
	{
		[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(ACCELEROMETRE_TIMER)];
		[[UIAccelerometer sharedAccelerometer] setDelegate:self];
	}
	else
	{
		[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(0)];
		[[UIAccelerometer sharedAccelerometer] setDelegate:nil];
	}
}

extern "C++" void activateAccelerometer(bool enable)
{
	if (enable)
	{
		[appDelegatePtr enableAccelerometer:YES];
	}
	else
	{
		[appDelegatePtr enableAccelerometer:NO];
	}
}

- (void)acceleratedInX:(float)xx Y:(float)yy Z:(float)zz
{
	__APP()->GetAccelerometer()->moved( xx, yy, zz );	
}

- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	m_accelerometer[0] = (acceleration.x * kFilteringFactor) + m_accelerometer[0] * (1.0 - kFilteringFactor);
	m_accelerometer[1] = (acceleration.y * kFilteringFactor) + m_accelerometer[1] * (1.0 - kFilteringFactor);
	m_accelerometer[2] = (acceleration.z * kFilteringFactor) + m_accelerometer[2] * (1.0 - kFilteringFactor);
	
	CTouchPad::EventSetTilt((int)(m_accelerometer[0] * k_accelerationAccuracy), (int)(m_accelerometer[1] * k_accelerationAccuracy), (int)(m_accelerometer[2] * k_accelerationAccuracy));
	
	CTouchPad::EventSetTiltAcceleration((acceleration.x * kFilteringFactor), (acceleration.y * kFilteringFactor), (acceleration.z * kFilteringFactor));
	
	[self acceleratedInX:acceleration.x Y:acceleration.y Z:acceleration.z];
}


///////////////////////////////////////////////////////////////////////
/// Buffers
///////////////////////////////////////////////////////////////////////

void setRenderbuffer()
{
	iCDAppDelegate* appDelegate = (iCDAppDelegate*) [[UIApplication sharedApplication] delegate];
	[appDelegate.glView setRenderbuffer];
}

void presentRenderbuffer()
{
	iCDAppDelegate* appDelegate = (iCDAppDelegate*) [[UIApplication sharedApplication] delegate];
	[appDelegate.glView presentRenderbuffer];
}


///////////////////////////////////////////////////////////////////////
/// Ohter extern
///////////////////////////////////////////////////////////////////////

extern "C++" int GetDeviceType()
{
	if (s_deviceType >= 0)
		return s_deviceType;

	char temp[256];
	strcpy(temp, [ [UIDevice currentDevice].model cString]);

	if ( strcmp(temp,"iPod touch")==0 )
		s_deviceType = DEVICE_TYPE_ITOUCH;
	else
		s_deviceType = DEVICE_TYPE_IPHONE;

	return s_deviceType;
}

extern "C++" void DeviceLanguage(char* lang )
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
    NSArray* languages = [defs objectForKey:@"AppleLanguages"];
    NSString* preferredLang = [languages objectAtIndex:0];
    
    sprintf(lang, "%s", [preferredLang UTF8String]);
}

extern "C++" void showStatusBar(bool bShow)
{
	[_application setStatusBarHidden:!bShow animated:YES];
}

extern "C++" void setStatusBarOrientation(int orient)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	UIInterfaceOrientation orientation;
		
	switch (orient)
	{
		case STATUS_BAR_ORIENTATION_PORTRAIT:
		default:
			orientation = UIInterfaceOrientationPortrait;
			break;
		case STATUS_BAR_ORIENTATION_IPHONE:
			orientation = UIInterfaceOrientationLandscapeRight;
			break;
		case STATUS_BAR_ORIENTATION_IPODTOUCH:
			orientation = UIInterfaceOrientationLandscapeLeft;
			break;
	}
	
	[[UIApplication sharedApplication] setStatusBarOrientation:orientation];
	
	[pool release];
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
extern "C++" void SetCurrentDeviceModel(char *modelString)
{
	if(strcmp(modelString,"iPod touch") == 0)
	{	
		Application::isDeviceIPod = true;
	}
	else if ( strcmp( modelString, "iPhone" ) == 0 )
	{
		Application::isDeviceIPod = true;
	}
	else
	{
		Application::isDeviceIPod = false;
	}
	
	Application::isDeviceSetted = true;
}


//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

extern "C++" void native_StatusBar_UpdateOrientation( )
{
	UIInterfaceOrientation orientation;
	switch (CTouchPad::s_currentDeviceOrientation)
	{
		case EOO_90:
			orientation = UIInterfaceOrientationLandscapeLeft;
			break;
		case EOO_270:
			orientation = UIInterfaceOrientationLandscapeRight;
			break;
	}
	
	[[UIApplication sharedApplication] setStatusBarOrientation:orientation];	
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------



/////////////////////////////////////////////////////////////////////// 
/// IGP
///////////////////////////////////////////////////////////////////////

extern "C++" void launchIGP(int langID)
{	
	_launchIGPLang = langID;
};

extern "C++" void createIGP(int langID)
{	
	igp = new IGP(_application, _window, _glView, langID);
};

extern "C++" bool paintIGP()
{
	bool return_value = false;
	if (igp && igp->MustRender())
	{
		return_value = igp->PaintIGP();
	}
	return return_value;
};

extern "C++" void releaseIGP()
{
	if (igp)
	{
		delete(igp);
		igp = 0;
	}
};


///////////////////////////////////////////////////////////////////////
/// Gameloft live
///////////////////////////////////////////////////////////////////////

extern "C++" void launchGLLive(int langID)
{	
	_launchGLLiveLang = langID;
};

extern "C++" void createGLLive(int langID)
{	
	switch( langID )
	{
		case 0: langID = 0; break;
		case 1:	langID = 3; break;
		case 2: langID = 5; break;
		case 3: langID = 2; break;
		case 4: langID = 4; break;
		case 5: langID = 1; break;			
	}
	
	glLive = new CGLLive([UIApplication sharedApplication], _window, _glView, langID, devTokenStr, false);
	glLive->SetInviteToPlay( false );
	if ( CTouchPad::s_currentDeviceOrientation == EOO_90 ) 
		glLive->SetOrientation( 1 );
	else 
		glLive->SetOrientation( 0 ) ;
};

extern "C++" bool paintGLLive()
{
	bool return_value = glLive->Update();
	
	if ( CTouchPad::s_currentDeviceOrientation == EOO_90 ) 
		glLive->SetOrientation( 1 );
	else 
		glLive->SetOrientation( 0 ) ;
	
	return return_value;
};

extern "C++" void releaseGLLive()
{
	if (glLive)
	{
		delete(glLive);
		glLive = 0;
	}
};

extern "C++" void NotifyTrophy( int trophy )
{
	if (glLive)
		glLive->NotifyTrophy( trophy );
}

@end