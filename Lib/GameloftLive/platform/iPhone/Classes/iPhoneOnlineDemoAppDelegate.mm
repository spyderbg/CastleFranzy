//
//  iPhoneOnlineDemoAppDelegate.m
//  iPhoneOnlineDemo
//
//  Created by Mou Bao Yu on 09-1-7.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "iPhoneOnlineDemoAppDelegate.h"
#import "EAGLView.h"
#include <pthread.h>
#include "GLLiveGame.h"

@implementation iPhoneOnlineDemoAppDelegate

@synthesize window;
@synthesize glView;

extern bool isFW3();

pthread_t tId;
extern CGLLive* g_pGLLive;
extern void UIViewBeginRender();
extern void UIViewEndRender();

void* ThreadRoutine(void* param)
{
	NSAutoreleasePool * thread_pool = [[NSAutoreleasePool alloc] init];
	//new GLLive
	if(!g_pGLLive)
	{
		iPhoneOnlineDemoAppDelegate* appDelegate = (iPhoneOnlineDemoAppDelegate*)[[UIApplication sharedApplication] delegate];
		int lang = 0;
		g_pGLLive = new CGLLive([UIApplication sharedApplication], appDelegate.window, appDelegate.glView, lang, NULL, true);
	}
	while(true)
	{
		UIViewBeginRender();
		if(g_pGLLive)
		{
			if(g_pGLLive->Update())
			{
				delete g_pGLLive;
				g_pGLLive = NULL;
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
		}
		UIViewEndRender();
		CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1.0 / 25.0, false);
		sched_yield();
	}
	
	[thread_pool release];
}

void StartThread()
{
	int n = 0;
	//try to create thread, 30 times max.
	while(n++ < 30 && pthread_create(&tId, NULL, &ThreadRoutine, NULL))
	{
		usleep(1000);
	}
	usleep(1000);	
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	[[UIApplication sharedApplication] setStatusBarHidden:YES];
	glView.animationInterval = 1.0 / 25.0;
#define MULTI_THREAD
#ifdef MULTI_THREAD
	StartThread();
#else
	[glView startAnimation];
#endif
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
