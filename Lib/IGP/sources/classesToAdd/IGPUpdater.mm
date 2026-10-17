//
//  IGPUpdater.mm
//  IGP
//
//  Created by Fernando.Mazzon on 20/06/08.
//  Copyright 2008 Gameloft. All rights reserved.
//

#import "IGPUpdater.h"
#import "IGP.h"
#import "IGPUtil.h"
#import "IGPDefines.h"
#import "Reachability.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
@implementation IGPUpdater

- (void)initAndDownloadConfig
{
//	state		= UPDATER_STATE_ERROR;
//	return;
	state		= UPDATER_STATE_INIT;

	m_receiveBuffer = new char[512 * 1024];
	m_abortConnection = false;
	m_connectionAlive = false;
	state		= UPDATER_STATE_INIT;
	[[Reachability sharedReachability] setHostName:@"www.google.com"];
	if ([[Reachability sharedReachability] internetConnectionStatus] == ReachableViaWiFiNetwork)
	{
		wifiAvailable = true;
		trace(DEBUG_UPDATER, "Connectivity found!\n");
	}
	else
	{
		wifiAvailable = false;
		if([[Reachability sharedReachability] internetConnectionStatus] == ReachableViaCarrierDataNetwork)
		{
			trace(DEBUG_UPDATER, "Connectivity found!\n");
			EDGEAvailable = true;
		}
		else
		{
			EDGEAvailable = false;
			trace(DEBUG_UPDATER, "Connectivity not found!\n");
		}
	}
	NSString* cfgURL = [self getCfgUrl];
	if (wifiAvailable || EDGEAvailable)
	{
		state = UPDATER_STATE_DL_CFG;
		trace(DEBUG_UPDATER, "Grabbing config file from:%s\n", [cfgURL UTF8String]);
		sprintf(m_fileToGrab, [cfgURL UTF8String]);
		[NSThread detachNewThreadSelector:@selector(getFile) toTarget:self withObject:nil];
	}
	else
	{
		state = UPDATER_STATE_ERROR;
	}
	

}

- (void) CancelConnection
{
	m_abortConnection = true;
	while (m_connectionAlive)
	{
		printf("-");
	}
}
- (void) UpdateGame:(IGPGame*) game
{
	m_connectionAlive = true;
//	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	if (state == UPDATER_STATE_READY)
	{
		m_gameBeingUpdated = game;
		state = UPDATER_STATE_BUSY;
		sprintf(m_fileToGrab, [m_gameBeingUpdated->GetURL() UTF8String]);
		[NSThread detachNewThreadSelector:@selector(getFile) toTarget:self withObject:nil];
		
	}
	m_connectionAlive = false;
//	[pool release];
}

/* Builds and returns the url that should be queried to get
** the game data that corresponds to this game/lang/region
** Also generates the config file name*/
- (NSString*)getCfgUrl
{
	NSString* link;
	
	sprintf(cfgFileName, "IGPCFG_%s_%s", IGP::s_igpDataLang, IGP::s_igpDataLocale);

	link = K_IGP_CFGURL;
	link = [link stringByReplacingOccurrencesOfString:@"VERSION" withString:[NSString stringWithUTF8String:IGP::s_igpDataVersion]];
	link = [link stringByReplacingOccurrencesOfString:@"FROM"  withString:[NSString stringWithUTF8String:IGP::s_igpDataGame]];
	link = [link stringByReplacingOccurrencesOfString:@"LANG"  withString:[NSString stringWithUTF8String:IGP::s_igpDataLang]];
	link = [link stringByReplacingOccurrencesOfString:@"COUNTRY"  withString:[NSString stringWithUTF8String:IGP::s_igpDataLocale]];
	link = [link stringByReplacingOccurrencesOfString:@"DEVICE"  withString:[[[UIDevice currentDevice] model]stringByReplacingOccurrencesOfString:@" " withString: @"_"]      ];
	link = [link stringByReplacingOccurrencesOfString:@"FIRMWARE"  withString:[[UIDevice currentDevice] systemVersion]];
	link = [link stringByReplacingOccurrencesOfString:@"UDIDPHONE"  withString:[[UIDevice currentDevice] uniqueIdentifier]];

	return link;
}

/* Destructor*/
- (void)dealloc
{
	trace(DEBUG_UPDATER, "Releasing updater resources\n");
	SAFE_DELETE_ARRAY(m_receiveBuffer);

	[super dealloc];
}


/* Writes the data in download buffer to the previously specified
** destination file. Returns true if operation was successful*/
- (bool)saveDownloadedData
{
	bool	success = true;
	char	path[255];
	int		written;
	
	
	GetFileName(m_gameBeingUpdated->GetFileName(), true, path);
	
	FILE* outFile = fopen(path, "wb");
	if (outFile)
	{
		trace(DEBUG_UPDATER, "Wrote %d bytes\n", m_receiveBufferLength);
		written = fwrite(m_receiveBuffer, sizeof(char), m_receiveBufferLength, outFile);
		if (written != m_receiveBufferLength)
		{
			success = false;
		}
		fclose(outFile);
	}
	else
	{
		success = false;
	}
	if(success)
	{
		char fileSaveNewGame[255];
		
		memset(path,0,255);
		sprintf(fileSaveNewGame, "%s_%s_save",m_gameBeingUpdated->m_gameCode,IGP::s_igpDataLang);
		GetFileName(fileSaveNewGame, true, path);
		outFile = fopen(path,"wb");
		fwrite(fileSaveNewGame,sizeof(char),strlen(fileSaveNewGame),outFile);
		fclose(outFile);
		
	}

	return success;
}

/* Writes the data in configFileBuffer to the specified file
** Returns true if the operation was successful*/
- (bool)saveCfgFile:(char*) fileName
{
	trace(DEBUG_UPDATER, "Writing config file to:%s\n", fileName);
	bool	success = true;
	char	path[255];
	int		written;
	
	GetFileName(fileName, true, path);
	
	if(fileExists(path))
	{
		char old_cfg[255];
		char old_cfgPath[255];
		sprintf(old_cfg,"%s_old", fileName);
		GetFileName(old_cfg, true, old_cfgPath);
		remove(old_cfgPath);
		rename(path,old_cfgPath);
		
	}
	FILE* outFile = fopen(path, "wb");
	//
	/*
	char	tmp[255];
	fread(tmp, 6, sizeof(char), outFile);
	
	if (strcmp(tmp, "v1.3.0"))
	{
		printf("cfgFileName:%s\n", cfgFileName);
		trace(DEBUG_LOADCONFIG, "Old or invalid config file, version:\n", tmp);
		//		NSFileManager* manager = [NSFileManager defaultManager];
		GetFileName(cfgFileName, true, tmp);
		//		[manager removeFileAtPath:[NSString stringWithUTF8String:tmp] handler:nil];
		fclose(outFile);
		//ReadConfig();
	}
	
	int totalGames;
	fread(&totalGames, 1, sizeof(int), outFile);
	printf("all games: %d:",totalGames);
	fclose(outFile);
	 outFile = fopen(path, "wb");
	*/
//	printf("blabla %d juegos nuevos",m_receiveBuffer[6] );
	//
	
	if (outFile)
	{
		written = fwrite(m_receiveBuffer, sizeof(char), m_receiveBufferLength, outFile);
		if (written != m_receiveBufferLength)
		{
			trace(DEBUG_UPDATER, "Error writing config file!\n");
			success = false;
		}
		fclose(outFile);
	}
	else
	{
		trace(DEBUG_UPDATER, "Error writing config file!\n");
		success = false;
	}
	
	return success;
}

- (void)getFile
{	
    struct sockaddr_in servaddr;
    struct hostent *hp;
    int sock_id;
	const char HEADER_END[] = {13, 10, 13, 10};
    char buff[2048] = {0};

	if (state != UPDATER_STATE_DL_CFG)
	{
		[NSThread setThreadPriority:0.01];
	}
	
	int c = 0;
	int y = 0;
	sprintf(buff,"GET %s HTTP/1.0\r\nFrom: IGPClient!\r\nHost: www.gameloft.org\r\nUser-Agent: IGPClient\r\n\r\n", m_fileToGrab);
    //Get a socket
    if((sock_id = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
        trace(DEBUG_UPDATER, "Couldn't get a socket.\n");
		state		= UPDATER_STATE_ERROR;
		return;
    }
	
	struct hostent *hp1;
	char host_name[] = "igpiphone.gameloft.com";
	hp1 = gethostbyname(host_name);
	int ct = 0;
	while(hp1==NULL && ct<2)
	{
		hp1 = gethostbyname(host_name);
		ct++;
	}
	ct = 0;
	
	if(hp1 == NULL)
	{
		trace(DEBUG_UPDATER, "Couldn't do gethostbyname.\n");
		close(sock_id);
		state		= UPDATER_STATE_ERROR;
		return;
	}
	else
	{
		servaddr.sin_addr.s_addr = *((in_addr_t*)hp1->h_addr);
		trace(DEBUG_UPDATER, "gethostbyname is ok.\n");
	}
    servaddr.sin_port = htons(80);
    servaddr.sin_family = AF_INET;
	
	int connectionAttemptNumber = 0;
	int connectionResult = connect(sock_id, (struct sockaddr *)&servaddr, sizeof(servaddr));
	while (connectionResult && connectionAttemptNumber < 5)
	{
		trace(DEBUG_UPDATER, "Failed connection attempt nª%d\n", ++connectionAttemptNumber);
		connectionResult = connect(sock_id, (struct sockaddr *)&servaddr, sizeof(servaddr));
	}
    //make the connection
    if(connectionResult == 0)
	{
		 trace(DEBUG_UPDATER,"Attempting to download %s\n", m_fileToGrab);
    }
    else
	{
		trace(DEBUG_UPDATER, "Couldn't connect.\n");
		close(sock_id);
		state		= UPDATER_STATE_ERROR;
		return;    
    }
	

    write(sock_id, buff, strlen(buff));
	
	int     flags;
	
	/* Set a socket as nonblocking */
	if  ( (flags = fcntl (sock_id, F_GETFL, 0)) < 0) printf("F_GETFL error");
	flags |= O_NONBLOCK;
	if (fcntl(sock_id, F_SETFL, flags) < 0) printf("F_SETFL error");
	
	//printf("HEADER\n");
	while(y = recv(sock_id, buff, 1, 0))
	{
		if (m_abortConnection)
		{
			break;
		}
		if (y > 0)
		{
			if (buff[0] == HEADER_END[c])
			{
				c++;
				if (c == 4) 
				{
					break;
				}
			}
			else
			{
				c = 0;
			}
		}
	}

	m_receiveBufferLength = 0;

	while(y = recv(sock_id, buff, 512, 0))
	{
		if (m_abortConnection)
		{
			break;
		}
		
		if (y > 0)
		{
			memcpy(&m_receiveBuffer[m_receiveBufferLength], buff, y);
			m_receiveBufferLength += y;
		}

	}
	close(sock_id);

	if (!m_abortConnection)
	{
		[self connectionDidFinishLoading];
	}
	else
	{
		state = UPDATER_STATE_ERROR;
		trace(DEBUG_UPDATER,"Aborting download\n");
	}
	
}

/* Callback: called when a download is completed successfully*/
- (void)connectionDidFinishLoading
{

	trace(DEBUG_UPDATER, "Received %d bytes\n", m_receiveBufferLength);
	switch (state) 
	{
		case UPDATER_STATE_DL_CFG:
		{
			if ([self saveCfgFile:cfgFileName])
			{
				//Saved config successfully
				IGP::s_igpInstance->newCFGDownloaded = true;
				state = UPDATER_STATE_READY;
				subState = 0;
			}
			else
			{
				//Error reading new config file
				state = UPDATER_STATE_ERROR;
			}
			break;
		}
		case UPDATER_STATE_READY:
		{
			trace(DEBUG_UPDATER, "Download finished while state = ready?\n");
			break;
		}
		case UPDATER_STATE_BUSY:
		{
			if ([self saveDownloadedData])
			{
				if (!m_gameBeingUpdated->m_isAvailable)
				{
					m_gameBeingUpdated->m_isAvailable = true;
				//pup c new sign disabled after feedback	m_gameBeingUpdated->m_isNew = true;
					s_gamesUpdatedInThisSession++;
				}
				
				m_gameBeingUpdated->m_isUpdated = true;
				m_gameBeingUpdated->m_isLoaded  = false;
				state = UPDATER_STATE_READY;
				IGP::s_igpInstance->Reinit();
					if (IGP::s_igpInstance->m_updatedGames > 0)
					{
						IGP::s_igpInstance->m_updatedGames = 0;
						[IGP::s_igpInstance->_activity setAlpha:1];
						IGP::s_igpInstance->setState(IGP_STATE_RELOAD);
					}
					IGP::s_igpInstance->m_updateCompleted = -1;
					break;
			}
			else
			{
				state = UPDATER_STATE_ERROR;
			}
		}
	}
	
//	[receivedData release];
}

- (bool)isEDGEAvailable;
{
	return EDGEAvailable;
}

- (bool)iswifiAvailable;
{
	return wifiAvailable;
}

/* Returns current state*/
- (UpdaterState) getState
{
	return state;
}

@end
