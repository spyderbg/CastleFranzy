//
//  IGPUpdater.h
//  IGP
//
//  Created by Fernando.Mazzon on 20/06/08.
//  Copyright 2008 Gameloft. All rights reserved.
//

#ifndef __IGPLINKGRABBER_H__
#define __IGPLINKGRABBER_H__

#define	STATE_FAIL		-1
#define	STATE_IDLE		0
#define	STATE_WORKING	1
#define	STATE_SUCCESS	2
#define	STATE_POKE		3

@interface IGPLinkGrabber : NSObject {
	
	int					status;
	long long			bytesReceived;
	char				finalLink[255];
	NSString*			downloadSource;
	NSString*			downloadDestination;
	NSURLResponse*		downloadResponse;
	NSMutableData*		receivedData;
}

- (void)	getLink:(NSString*) url;

- (void)	getRedirectLink:(NSString*) link;
- (int)		currentStatus;
- (char*)	getFinalLink;
- (void)pokeConnectivity;

@end

#endif