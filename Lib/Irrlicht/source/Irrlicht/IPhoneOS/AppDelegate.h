// Copyright (C) 2005-2008 Etienne Petitjean
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#import "CIrrDeviceIPhoneOS.h"
#import <UIKit/UIKit.h>

@class EAGLView;

@interface AppDelegate : NSObject <UIApplicationDelegate> {
	IBOutlet UIWindow *window;
	IBOutlet EAGLView *glView;
	BOOL					_quit;
	irr::CIrrDeviceIPhoneOS	*_device;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) EAGLView *glView;

- (id)initWithDevice:(irr::CIrrDeviceIPhoneOS *)device;
- (BOOL)isQuit;


@end
