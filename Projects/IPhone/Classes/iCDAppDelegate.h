//
//  iCDAppDelegate.h
//
//  Castle Defence
//
//  Created by Plamen Chernev on 03/09/09.
//  Copyright Gameloft 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "IGP.h"
#import "GLLiveGame.h"
#import "TextInput.h" 

#import "Core/Application.h"

@class EAGLView;

#define k_accelerationAccuracy 100
#define kFilteringFactor 0.2
#define kAccelerometerFrequency 30.0 //Hz

@interface iCDAppDelegate : NSObject <UIApplicationDelegate, UIAccelerometerDelegate>
{
    UIWindow*	window;
    EAGLView*	glView;
}


extern IGP*					igp;
extern int					_launchIGPLang;

extern CGLLive*				glLive;
extern int					_launchGLLiveLang;

extern iCDAppDelegate*		appDelegatePtr;
extern UIApplication*		_application;
extern UIWindow*			_window;
extern EAGLView*			_glView;

extern int					g_nAppExit;

extern UIAccelerationValue	m_accelerometer[3];


@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;

+(iCDAppDelegate*)sharedInstance; 

- (void)acceleratedInX:(float)xx Y:(float)yy Z:(float)zz;

-(void)removeTextField:(TextInput*)params;
-(void)initTextInput:(TextInputInitParams*)params;

@end

