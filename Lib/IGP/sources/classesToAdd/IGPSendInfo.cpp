/*
 *  IGPSendInfo.cpp
 *  IGP
 *
 *  Created by Catalin Cucu on 3/18/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
//#import "IGP.h"
#include "IGPUtil.h"
#include "IGPSendInfo.h"
#include "IGPInfoSender.h"


//#include "IGPUtil.h"

void IGPSendInfo::IGPSendGameInfo(int langIndex, char* gameVersion)
{
	NSAutoreleasePool* p = [[NSAutoreleasePool alloc] init];
	
	FILE* res = openFile("igpdata");
	char gameCode[5];
	char igpVersion[6];
	char lang[3] = {0};
	char pathToSave[255];
	
	
	fread(&gameCode, sizeof(char), 5, res); //read game code
	fread(&igpVersion, sizeof(char), 6, res);	//read version
	
	fseek(res, 12+(langIndex*2), SEEK_SET);
	fread(lang, sizeof(char), 2, res);		//read 1 lang
	fclose(res);
	
	
	NSString* localeIdentifier = [[NSLocale currentLocale] objectForKey:NSLocaleCountryCode];
	
	NSString* temp = [[NSString alloc] initWithString:[[UIDevice currentDevice] model]];
	NSString* deviceType = nil;
	
	@try 
	{
		deviceType = [temp stringByReplacingOccurrencesOfString:@" " withString: @"_"];
	}
	@catch (id theException)
	{
		deviceType = [[NSString alloc] initWithString:@"iPhone_unknown"];
	}
	@finally
	{
		
	}
	
	[temp release];
	
	NSString* firmware = [[UIDevice currentDevice] systemVersion];
	NSString* UDID_phone = [[UIDevice currentDevice] uniqueIdentifier];
	NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
	[formatter setDateFormat:@"yyyy-MM-dd_HH:mm:ss"];
	NSString* dateToSend;
	
	GetFileName("dateI",true,pathToSave);
	if([[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithUTF8String:pathToSave]])
	{
		NSError **err1;
		dateToSend = [NSString stringWithContentsOfFile:[NSString stringWithUTF8String:pathToSave] encoding:NSUTF8StringEncoding error:err1];
	}
	else
	{
		NSError **err1;
		dateToSend = [formatter stringFromDate:[NSDate date]];

		[dateToSend writeToFile:[NSString stringWithUTF8String:pathToSave] atomically:YES encoding:NSUTF8StringEncoding error:err1];
	}
	
	NSString* linkToVisit = [[NSString alloc] initWithFormat:@"http://ingameads.gameloft.com/redir/iphoneloading.php?game=GAMECODE&country=CTRY&lg=LANGUAGE&ver=IGPVERSION&d=IPHONE&f=FIRMWARE&udid=UDIDPHONE&date_r=INSTALLDATE&g_ver=GAMEVERSION"];
	NSString* linkToVisit1 = [linkToVisit stringByReplacingOccurrencesOfString:@"GAMECODE" withString:[NSString stringWithUTF8String:gameCode]];
	
	[linkToVisit release];
	linkToVisit = nil;

	if (localeIdentifier == nil)
	{
		localeIdentifier = @"XX";	//wrong country
	}
	
	NSString* linkToVisit2;
	@try 
	{
		linkToVisit2 = [linkToVisit1 stringByReplacingOccurrencesOfString:@"CTRY" withString:localeIdentifier];
	}
	@catch (id theException)
	{
		linkToVisit2 = [linkToVisit1 stringByReplacingOccurrencesOfString:@"CTRY" withString:@"XX"];
	}
	@finally
	{
		
	}

	NSString* linkToVisit3 = [linkToVisit2 stringByReplacingOccurrencesOfString:@"LANGUAGE" withString:[NSString stringWithUTF8String:lang]];
	NSString* linkToVisit4 = [linkToVisit3 stringByReplacingOccurrencesOfString:@"IGPVERSION" withString:[NSString stringWithUTF8String:igpVersion]];
	NSString* linkToVisit5 = [linkToVisit4 stringByReplacingOccurrencesOfString:@"IPHONE" withString:deviceType];
	NSString* linkToVisit6 = [linkToVisit5 stringByReplacingOccurrencesOfString:@"FIRMWARE" withString:firmware];
	NSString* linkToVisit7 = [linkToVisit6 stringByReplacingOccurrencesOfString:@"UDIDPHONE" withString:UDID_phone];
	NSString* linkToVisit8 = [linkToVisit7 stringByReplacingOccurrencesOfString:@"INSTALLDATE" withString:dateToSend];
	NSString* linkToVisit9 = [linkToVisit8 stringByReplacingOccurrencesOfString:@"GAMEVERSION" withString:[[NSString stringWithUTF8String:gameVersion] stringByReplacingOccurrencesOfString:@" " withString:@""]];
	NSString* linkToVisit10 = [linkToVisit9 stringByReplacingOccurrencesOfString:@" " withString:@""];	
						  
	IGPInfoSender* infoSender = [[IGPInfoSender alloc] initWithLink: linkToVisit10];
	[NSThread detachNewThreadSelector:@selector(sendInfo) toTarget:infoSender withObject:nil];

	[infoSender release];
	infoSender = nil;
	[p release];
}


IGPSendInfo::IGPSendInfo()
 {
	// bb = true;
 }
IGPSendInfo::~IGPSendInfo()
 {
 }
