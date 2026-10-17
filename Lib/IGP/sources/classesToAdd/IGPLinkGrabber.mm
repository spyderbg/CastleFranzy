//
//  IGPUpdater.mm
//  IGP
//
//  Created by Fernando.Mazzon on 20/06/08.
//  Copyright 2008 Gameloft. All rights reserved.
//
#import "IGPLinkGrabber.h"
#import "Reachability.h"
#import "IGPDefines.h"
#import "IGPUtil.h"

@implementation IGPLinkGrabber

- (void)getRedirectLink:(NSString*) link
{
	trace(DEBUG_LINKGRABBER, "Attempting to get redirect link\n");
	status = STATE_WORKING;
	[[Reachability sharedReachability] setHostName:@"www.google.com"];
	if ([[Reachability sharedReachability] internetConnectionStatus] == NotReachable)
	{
		trace(DEBUG_LINKGRABBER, "Connectivity not found!\n");
		status = STATE_FAIL;
	}
	[self getLink:link];
}

- (void)getLink:(NSString*) url
{
	NSURLRequest *theRequest=[NSURLRequest requestWithURL:[NSURL URLWithString:url] cachePolicy:nil timeoutInterval:13];
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
    if (theConnection)
	{
		if (receivedData)
		{
			[receivedData release];
			receivedData = nil;
		}
		receivedData = [NSMutableData alloc];
    }
	else
	{
		trace(DEBUG_LINKGRABBER, "Unable to connect!\n");
		status = STATE_FAIL;
    }
}
- (void)pokeConnectivity
{
	NSURL* theUrl = [NSURL URLWithString:@"http://igpiphone.gameloft.com/empty.txt"];
	NSURLRequest *theRequest=[NSURLRequest requestWithURL:theUrl cachePolicy:nil timeoutInterval:10];
	NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
    if (theConnection)
	{
		trace(DEBUG_LINKGRABBER, "Trying to poke!\n");
		status = STATE_POKE;

		if (receivedData)
		{
			[receivedData release];
			receivedData = nil;
		}
		receivedData = [NSMutableData alloc];
    }
	else
	{
		trace(DEBUG_LINKGRABBER, "Unable to connect!\n");
		status = STATE_FAIL;
    }
	//[theUrl autorelease];
	//[theRequest autorelease];
	//[theConnection autorelease];
}
- (void)setDownloadResponse:(NSURLResponse *)aDownloadResponse
{
    [aDownloadResponse retain];
    [downloadResponse release];
    downloadResponse = aDownloadResponse;
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    bytesReceived = 0;
	[receivedData setLength:0];
    [self setDownloadResponse:response];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    bytesReceived = bytesReceived + [data length];
	[receivedData appendData:data];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    [connection release];
	status = STATE_FAIL;
	trace(DEBUG_LINKGRABBER, "Error while trying to connect\n");
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	if (status == STATE_POKE)
	{
		trace(DEBUG_LINKGRABBER, "Poke successful\n");
		status = STATE_IDLE;
		return;
	}
	trace(DEBUG_LINKGRABBER, "Link received\n");
	char* receivedBytes = (char*)[receivedData mutableBytes];
	for (int i = 0; i < bytesReceived;i++)
	{
		finalLink[i] = receivedBytes[i];
	}
	if (bytesReceived > sizeof(int))
	{
/*		if (*((int*)finalLink) == 1936553065 || *((int*)(finalLink + 11)) == 1953853305) //itms || yout
		{*/
			trace(DEBUG_LINKGRABBER, "Good link:%s\n", finalLink);
			status = STATE_SUCCESS;
	/*	}
		else
		{
			trace(DEBUG_LINKGRABBER, "Bad link:%s\n", finalLink);
			status = STATE_FAIL;
		}*/
	}
	else
	{
		trace(DEBUG_LINKGRABBER, "Bad link:%s\n", finalLink);
		status = STATE_FAIL;
	}

}

- (char*) getFinalLink
{
	return finalLink;
}

- (int) currentStatus
{
	return status;
}

- (void)dealloc
{
	trace(DEBUG_OBJCMEM, "Releasing LinkGrabber\n");
	[receivedData release];
	
	[super dealloc];
}


@end
