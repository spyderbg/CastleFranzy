//
//  Platform_iPhone.m
//  iRF09_NOE
//
//  Created by Lu Tie Hua on 1/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Platform_iPhone.h"

@implementation Platform_iPhone

extern char g_moudlePath[512];
//void GetSaveFilePath(char* filePath, const char* filename)
//{	
	//sprintf(filePath, "/%s", filename);
	
  //NSString *path = [[NSString alloc] initWithUTF8String:filePath];
	//NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	//NSString *documentsDirectory = [paths objectAtIndex:0];
	//path = [documentsDirectory stringByAppendingPathComponent:path];
	
	//sprintf(filePath, "%s", [path UTF8String]);
	//[path release];
//}
void GetSaveFilePath(char* filePath, const char* filename)
{	
	const char*     mySavePath;
	
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES); 
	NSString *documentsDirectory = [paths objectAtIndex:0];
	mySavePath = [documentsDirectory UTF8String];
	
	sprintf(filePath, "%s%s%s", mySavePath, "/", filename);
}

void GetConfigFilePath(char* filePath, const char* filename)
{	
	char*     myConfigPath = g_moudlePath;

	sprintf(filePath, "%s%s", myConfigPath, filename);
}

extern "C" void GetUserCountry(char* buf)
{	
	if(!buf)
		return;
	
	CFLocaleRef userLocaleRef = CFLocaleCopyCurrent();
	CFLocaleRef systemLocaleRef = CFLocaleGetSystem();
	
	CFStringRef c1 = (CFStringRef)CFLocaleGetValue(userLocaleRef, kCFLocaleCountryCode);
	
	if(c1 != nil)
	{
		const char* cc1 = [c1 UTF8String];
		
		//const void* c2 = CFLocaleGetValue(systemLocaleRef, kCFLocaleCountryCode);
		//const char* cc2 = (const char*)c2;	
		
		strcpy(buf, cc1);
	}
	else
	{
		buf[0] = 0;
	}
}

@end
