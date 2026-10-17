//
//  iPhoneOnlineDemoAppDelegate.m
//  iPhoneOnlineDemo
//
//  Created by Mou Bao Yu on 09-1-7.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "iPhoneOnlineDemoAppDelegate.h"
#import "EAGLView.h"

@implementation iPhoneOnlineDemoAppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	[[UIApplication sharedApplication] setStatusBarHidden:YES];
	glView.animationInterval = 1.0 / 25.0;
	[glView startAnimation];
}


- (void)applicationWillResignActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 25.0;
}


- (void)dealloc {
	[window release];
	[glView release];
	[super dealloc];
}

@end
