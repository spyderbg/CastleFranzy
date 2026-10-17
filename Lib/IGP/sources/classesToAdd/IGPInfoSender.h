//
//  IGPInfoSender.h
//  IGP
//
//  Created by Catalin Cucu on 3/19/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

//#import <Cocoa/Cocoa.h>


@interface IGPInfoSender : NSObject {

	NSString* linkToSend;
}

- (id) initWithLink: (NSString*) link;
-(void)sendInfo;

@end
