// Copyright (C) 2005-2008 Etienne Petitjean
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#import "AppDelegate.h"

@implementation AppDelegate

@synthesize window;
@synthesize glView;

- (id)initWithDevice:(irr::CIrrDeviceIPhoneOS *)device
{
	self = [super init];
	if (self) _device = device;
	return (self);
}

- (void)applicationDidFinishLaunching:(UIApplication *)aNotification
{
	
	_quit = FALSE;
}

- (void)orderFrontStandardAboutPanel:(id)sender
{
}

- (void)unhideAllApplications:(id)sender
{

}

- (void)hide:(id)sender
{
}

- (void)hideOtherApplications:(id)sender
{
}

- (void)terminate:(id)sender
{
	_quit = TRUE;
}

- (void)windowDidResize:(NSNotification *)aNotification
{
}

- (BOOL)isQuit
{
	return (_quit);
}

@end
