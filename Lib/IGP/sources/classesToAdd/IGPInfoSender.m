//
//  IGPInfoSender.m
//  IGP
//
//  Created by Catalin Cucu on 3/19/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "IGPInfoSender.h"


@implementation IGPInfoSender

- (id) initWithLink: (NSString*) link
{
	if (self = [super init])
	{
		linkToSend = [[NSString alloc] initWithString:link];
	}
	return self;
}

-(void) sendInfo
{
	NSAutoreleasePool* p = [[NSAutoreleasePool alloc] init];
	NSURL* the_url = [NSURL URLWithString:linkToSend];
	NSData* release_me = [[NSData alloc] initWithContentsOfURL:the_url];
	[linkToSend release];
	linkToSend = nil;
	[release_me release];
	release_me = nil;
	[p release];
}
@end
