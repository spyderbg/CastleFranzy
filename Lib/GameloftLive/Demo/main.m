//
//  main.m
//  iPhoneOnlineDemo
//
//  Created by Mou Bao Yu on 09-1-7.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

char g_moudlePath[512];

int main(int argc, char *argv[]) {
    
	sprintf(g_moudlePath, argv[0]);
	size_t pos = strlen(g_moudlePath);
	while(pos > 0)
	{
		if(g_moudlePath[pos--] == '/')
			break;
	}
	g_moudlePath[pos + 2] = 0;
	
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, nil);
    [pool release];
    return retVal;
}
