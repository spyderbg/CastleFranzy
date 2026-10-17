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

extern bool isFW3();

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	[[UIApplication sharedApplication] setStatusBarHidden:YES];
	glView.animationInterval = 1.0 / 25.0;
	[glView startAnimation];
	
	//zyx add 2009-6-10
	//register Remote Notification	
	
	if(isFW3())
		[application registerForRemoteNotificationTypes:(UIRemoteNotificationType)(UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound | UIRemoteNotificationTypeAlert)]; 
	//zyx add 2009-6-10 end
}


- (void)applicationWillResignActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 25.0;
}

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

@end
