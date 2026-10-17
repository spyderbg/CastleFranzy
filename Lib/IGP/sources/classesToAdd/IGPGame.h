/*
 *  IGPGame.h
 *  IGP
 *
 *  Created by administrador on 25/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __IGPGAME_H__
#define __IGPGAME_H__

#include "IGPTexture.h"
#include "IGPDefines.h"
#include "IGPUtil.h"
#include <OpenGLES/ES1/gl.h>
#include <QuartzCore/QuartzCore.h>


typedef struct {
	char packName[15];
	int  posInFile;
} PackIndex;

class IGPGame
{
public:
	static void ReadPackIndex();
	static void FreePackIndex();
	static int  PackIndexForFile(char* fileName);
	static NSString* getMainShortTitle();
private:
	static PackIndex*	s_packs;
	static int			s_packCount;
	
	IGPTexture*	m_splash;
	
	NSString*	m_gameName;
	NSString*	m_gameShortName;
	NSString*	m_description;
	
	UIImage*	m_logo;
	UIImage*	m_icon;
	UIImage*	m_screenshots[3];

	char		m_url[255];
	char		m_md5[33];

	char		m_fileName[15];										//XXXX_YY_ZZ.IGP
public:
	NSString*	m_gamePrice;
	char		m_gameCode[5];
	int			m_priority[PRIOMAX];								//Priority in WN/BS/CATALOG

	bool		m_isAvailable;										//Game files are in client and can be shown, if false, game hasn't been downloaded yet
	bool		m_isUpdated;										//Game files are up to date
	bool		m_hasNewPrice;										//Game has new price flag
	bool		m_priceUpdated;
	bool		m_isNew;											//Game has just been updated flag
	bool		m_isLoaded;											//Splash, logo, and icon loaded
	bool		m_screenshotsLoaded;								//Screenshots are loaded
	int			m_filePackIndex;										
	IGPGame();
	~IGPGame();
	bool IsLandscape(int screenshotIndex);
	bool HasScreenshotsLoaded();
	void ReleaseScreenshots();
	void CheckForStatus();
	
	void SetHasNewPrice(bool b);
	void SetPriority(int prio, int prioIndex);
	void SetSplash(IGPTexture* splash);
	void SetScreenshot(UIImage* scr, int index);
	void SetLogo(UIImage* logo);
	void SetCode(char* code);
	void SetMD5(char* md5);
	void SetFileName(char* name);
	void SetURL(char* url);
	
	void LoadRes();
	void LoadScreenshots();
	int  GetPriority(int prioIndex);
	IGPTexture* GetSplash();
	UIImage*	GetScreenshot(int index);
	UIImage*	GetLogo();
	UIImage*	GetIcon();
	NSString*	GetName();
	NSString*	GetPrice();
	NSString*	GetShortName();
	NSString*	GetDescription();
	NSString*	GetURL();
	char*		GetFileName();
	
	void		OpenBuyLink(const char* from = "");
	void		OpenVideoLink(const char* from = "");

};

#endif