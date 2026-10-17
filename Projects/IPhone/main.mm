//
//  main.mm
//
//  Castle Defence
//
//  Created by Plamen Chernev on 03/09/09.
//  Copyright Gameloft 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

// global stuff required by XPlayerLib
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
