//
//  IGPUpdater.h
//  IGP
//
//  Created by Fernando.Mazzon on 20/06/08.
//  Copyright 2008 Gameloft. All rights reserved.
//

#ifndef __IGPUPDATER_H__
#define __IGPUPDATER_H__

#import <UIKit/UIKit.h>
#include "IGPGame.h"

typedef enum {
	UPDATER_STATE_ERROR = -1,
	UPDATER_STATE_INIT,
	UPDATER_STATE_DL_CFG,
	UPDATER_STATE_READY,
	UPDATER_STATE_BUSY
} UpdaterState;

@interface IGPUpdater : NSObject {
	//Current state
	UpdaterState	state;	
	int				subState;

	char			cfgFileName[255];		//Name of the config file that should be used
	

	IGPGame*		m_gameBeingUpdated;
	//Used to keep the config file in memory until all downloads are complete
	char			m_fileToGrab[255];
	char*			m_receiveBuffer;
	int				m_receiveBufferLength;
	bool			m_abortConnection;
	bool			m_connectionAlive;

	bool			wifiAvailable;
	bool			EDGEAvailable;
}

static int s_gamesUpdatedInThisSession = 0;

- (void) CancelConnection;
- (void) UpdateGame:(IGPGame*) game;
- (void) initAndDownloadConfig;
- (UpdaterState) getState;
- (NSString*) getCfgUrl;
- (void) getFile;
- (void)connectionDidFinishLoading;
- (bool) saveDownloadedData;
- (bool)saveCfgFile:(char*) fileName;
- (bool)isEDGEAvailable;
- (bool)iswifiAvailable;

@end

#endif
