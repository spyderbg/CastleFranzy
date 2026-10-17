/*
 *  IGPGame.cpp
 *  IGP
 *
 *  Created by administrador on 25/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#define IGP_LITE	0

#include "IGPGame.h"
#include "IGP.h"
int			IGPGame::s_packCount;
PackIndex*	IGPGame::s_packs;

void IGPGame::ReadPackIndex()
{
	FILE* res = openFile("textures");
	fread(&s_packCount, 1, sizeof(int), res);
	s_packs = new PackIndex[s_packCount];

	for (int i = 0; i < s_packCount; i++)
	{
		fread(&s_packs[i].packName, 15, sizeof(char), res);
		fread(&s_packs[i].posInFile, 1, sizeof(int), res);
	}
	fclose(res);
}

void IGPGame::FreePackIndex()
{
	SAFE_DELETE_ARRAY(s_packs);
	s_packCount = 0;
}

int  IGPGame::PackIndexForFile(char* fileName)
{
	int ret = -1;
	for (int i = 0; i < s_packCount; i++)
	{
		if (strcmp(fileName, s_packs[i].packName) == 0)
		{
			return i;
		}
	}
	return ret;
}
IGPGame::~IGPGame()
{
	if (strlen(m_gameCode))
	{
		trace(DEBUG_CPPCMEM, "Releasing %s\n", m_gameCode);
	}
	SAFE_DELETE(m_splash);
	SAFE_DELETE_OBJC(m_logo);
	SAFE_DELETE_OBJC(m_icon);
	SAFE_DELETE_OBJC(m_gameName);
	SAFE_DELETE_OBJC(m_gameShortName);
	SAFE_DELETE_OBJC(m_description);
	
	[m_gamePrice release];

	for (int i = 0; i < 3; i++)
	{
		if (IGP_LITE)
		{
			for(int j=i+1;j<3;j++)
			{
				if(m_screenshots[j] == m_screenshots[i])
					m_screenshots[j] = nil;
			}		
		}
		SAFE_DELETE_OBJC(m_screenshots[i]);
	}
}
IGPGame::IGPGame()
{
	int i;
	m_isLoaded			= false;
	m_isNew				= false;
	m_hasNewPrice		= false;
	m_screenshotsLoaded = false;
	m_splash		= NULL;
	m_logo			= NULL;
	m_icon			= NULL;
	m_gameName		= NULL;
	m_gameShortName	= NULL;
	m_gamePrice	= NULL;
	m_description	= NULL;
	m_gameCode[0]	= 0;
	for(i = 0; i < PRIOMAX; i++)
	{
		m_priority[i] = rand() % 10;
	}
	for (i = 0; i < 3; i++)
	{
		m_screenshots[i] = NULL;
	}
}

//will get the short name for the main game
 NSString* IGPGame::getMainShortTitle()
{
	
	char fileName[15];	
	int filePackIndex;
	
	sprintf(fileName, "%s_%s_%s.IGP", IGP::s_igpDataGame, IGP::s_igpDataLang, IGP::s_igpDataLocale);
	filePackIndex = PackIndexForFile(fileName);
	if (filePackIndex == -1)
	{
		sprintf(fileName, "%s_%s.IGP", IGP::s_igpDataGame, IGP::s_igpDataLang);
		filePackIndex = PackIndexForFile(fileName);
	}

	int length;
	FILE* res = openFile("textures");
	fseek(res, s_packs[filePackIndex].posInFile, SEEK_SET);
	
	skipTexture(res);	//skip splash
	
	//IGPTexture *m_splash = loadTexture(res, IGP::s_buff, 0, K_FORMAT_FOR_SPLASHES);
	//SAFE_DELETE(m_splash);
	
	skipTexture(res);	//skip icon
	
	fread(&length, 1, sizeof(int), res);
	fread(IGP::s_igpInstance->s_buff, length, sizeof(char), res);
	NSString *m_gameName = [[NSString alloc] initWithCharacters:(unichar*)IGP::s_igpInstance->s_buff length:length/2];
		
	fread(&length, 1, sizeof(int), res);
	fread(IGP::s_igpInstance->s_buff, length, sizeof(char), res);
	
	//NSString *m_gameShortName = [[NSString alloc] initWithCharacters:(unichar*)IGP::s_igpInstance->s_buff length:length/2];
	
	fclose(res);
	return m_gameName;
}








void IGPGame::LoadRes()
{
	FILE* res;
	int length;
	if (m_isAvailable && !m_isLoaded)
	{
		if (m_filePackIndex != -1)
		{
			res = openFile("textures");
			fseek(res, s_packs[m_filePackIndex].posInFile, SEEK_SET);
		}
		else
		{
			res = openFile(m_fileName);
			fseek(res, 0, SEEK_SET);
			//CARGA DE ARCHIVOS INDIVIDUALES
		}
	}
	else
	{
		return;
	}
	SAFE_DELETE(m_splash);
	
	//[m_logo release];
	[m_icon release];
	[m_gameName release];
	[m_gameShortName release];
	[m_description release];

	if (m_priority[PRIO_BS] || m_priority[PRIO_WN])
	{
		trace(DEBUG_GAME, "Loading splash for %s\n", m_gameCode);
		m_splash = loadTexture(res, IGP::s_buff, 0, K_FORMAT_FOR_SPLASHES);
	}
	else
	{
		trace(DEBUG_GAME, "Skipping splash for %s\n", m_gameCode);
		skipTexture(res);
	}
	m_icon	 = [loadImage(res) retain];
	
	fread(&length, 1, sizeof(int), res);
	fread(IGP::s_igpInstance->s_buff, length, sizeof(char), res);
	m_gameName = [[NSString alloc] initWithCharacters:(unichar*)IGP::s_igpInstance->s_buff length:length/2];

	fread(&length, 1, sizeof(int), res);
	fread(IGP::s_igpInstance->s_buff, length, sizeof(char), res);
	m_gameShortName = [[NSString alloc] initWithCharacters:(unichar*)IGP::s_igpInstance->s_buff length:length/2];
	
	fread(&length, 1, sizeof(int), res);
	fread(IGP::s_igpInstance->s_buff, length, sizeof(char), res);
	m_description = [[NSString alloc] initWithCharacters:(unichar*)IGP::s_igpInstance->s_buff length:length/2];
	
	m_isLoaded = true;
	trace(DEBUG_GAME, "Game %s was succesfully loaded!\n", m_gameCode);
	fclose(res);
}
void IGPGame::CheckForStatus()
{
	/*Either we loaded from original config file which means we have no connectivy, or the file isn't available in the server,
	 we tag everything as up to date and available
	 */
	if (strlen(m_fileName) == 0)			
	{
		sprintf(m_fileName, "%s_%s_%s.IGP", m_gameCode, IGP::s_igpDataLang, IGP::s_igpDataLocale);
		m_filePackIndex = PackIndexForFile(m_fileName);
		if (m_filePackIndex == -1)
		{
			sprintf(m_fileName, "%s_%s.IGP", m_gameCode, IGP::s_igpDataLang);
			m_filePackIndex = PackIndexForFile(m_fileName);
		}
		
		if (m_filePackIndex != -1)
		{
			trace(DEBUG_GAME, "Game %s is available at original files at index %d\n", m_gameCode, m_filePackIndex);
			m_isAvailable = true;
			m_isUpdated   = true;
		}
		else
		{
			trace(DEBUG_GAME, "Game %s is not available at original files, but original config was loaded... This shouldn't ever happen report please!\n", m_gameCode);

			m_isAvailable = false;
			m_isUpdated   = false;
		}
		
	}
	else if (fileExists(m_fileName))		//Loading from updated config file and file is in the server.
	{
		
		m_isAvailable = true;
		m_filePackIndex = -1;
		if (MD5Matches(m_fileName, m_md5))
		{
			trace(DEBUG_GAME, "Game %s is available at it's own file and updated\n", m_gameCode);
			m_isUpdated = true;
		}
		else
		{
			trace(DEBUG_GAME, "Game %s is available at it's own file but not updated\n", m_gameCode);

//			if (sizeofm_md5)
			if(sizeof(m_md5)/sizeof(char) == 33)
			{
				m_isUpdated = false;
			}
			else
			{
				trace(DEBUG_GAME, "ERROR: Game %s's MD5 erroneous, will skip update\n", m_gameCode);
				m_isUpdated = true;
				
			}
		}
	}	
	else									//Check the original files for the pack we are looking for
	{
		trace(DEBUG_GAME, "Game %s is not available at it's own file... Checking original files\n", m_gameCode);
		m_filePackIndex = PackIndexForFile(m_fileName);
		if (m_filePackIndex != -1)
		{
			trace(DEBUG_GAME, "Game %s is available at original files at index %d\n", m_gameCode, m_filePackIndex);
			m_isAvailable = true;
			m_isUpdated = false;//TODO check md5
		}
		else
		{
			m_isAvailable	= false;
			m_isUpdated		= false;
			trace(DEBUG_GAME, "Game %s is not available\n", m_gameCode);
		}
	}
}

void IGPGame::LoadScreenshots()
{
	if (m_isAvailable && !m_screenshotsLoaded)
	{
		FILE* res;
		bool fromOriginalRes = false;
		if (m_filePackIndex != -1)
		{
			res = openFile("textures");
			fseek(res, s_packs[m_filePackIndex].posInFile, SEEK_SET);
			if (IGP_LITE)
			{
				fromOriginalRes = true;
			}
		}
		else
		{
			res = openFile(m_fileName);
			fseek(res, 0, SEEK_SET);
		}
		skipTexture(res);
		skipTexture(res);
	//	pup c avoid logo skipTexture(res);
		fread(&IGP::s_buff, 1, sizeof(int), res);
		fread(IGP::s_buff,((int*)IGP::s_buff)[0], sizeof(char), res);
		fread(&IGP::s_buff, 1, sizeof(int), res);
		fread(IGP::s_buff,((int*)IGP::s_buff)[0], sizeof(char), res);
		fread(&IGP::s_buff, 1, sizeof(int), res);
		fread(IGP::s_buff,((int*)IGP::s_buff)[0], sizeof(char), res);
		
		if (fromOriginalRes)
		{
			SetScreenshot([loadImage(res) retain], 0);
			SetScreenshot(GetScreenshot(0), 1);
			SetScreenshot(GetScreenshot(0), 2);			
		}
		else 
		{
			SetScreenshot([loadImage(res) retain], 0);
			SetScreenshot([loadImage(res) retain], 1);
			SetScreenshot([loadImage(res) retain], 2);
		}
		m_screenshotsLoaded = true;

		trace(DEBUG_GAME, "Screenshots for game %s were succesfully loaded!\n", m_gameCode);
		fclose(res);
		
	}
	else
	{
		return;
	}
}

void IGPGame::ReleaseScreenshots()
{
	if (m_screenshots[0] != NULL)
	{
		trace(DEBUG_GAME, "Releasing screenshots for %s\n", m_gameCode);
	}
	for(int i = 0; i < 3; i++)
	{
		if (m_screenshots[i])
		{
			if (IGP_LITE)
			{
				for(int j=i+1;j<3;j++)
				{
					if(m_screenshots[j] == m_screenshots[i])
						m_screenshots[j]= nil;
				}			
			}
			[m_screenshots[i] release];
			m_screenshots[i] = NULL;
		}
	}
	m_screenshotsLoaded = false;
}

int  IGPGame::GetPriority(int prioIndex)
{
	if (m_isAvailable)
	{
		return m_priority[prioIndex];
	}
	else	//If the game is not available it will not be in an
	{
		return 0;
	}
}

UIImage* IGPGame::GetIcon()
{
	return m_icon;
}

bool IGPGame::HasScreenshotsLoaded()
{
	return m_screenshotsLoaded;
}

bool IGPGame::IsLandscape(int screenshotIndex)
{
	return m_screenshots[screenshotIndex].size.width > m_screenshots[screenshotIndex].size.height;
}

NSString* IGPGame::GetDescription()
{
	return m_description;
}

NSString* IGPGame::GetName()
{
	return m_gameName;
}

NSString* IGPGame::GetPrice()
{
	return m_gamePrice;
}

NSString* IGPGame::GetShortName()
{
	return m_gameShortName;
}

char* IGPGame::GetFileName()
{
	return m_fileName;
}

NSString* IGPGame::GetURL()
{
	return [NSString stringWithUTF8String:m_url];
}
IGPTexture* IGPGame::GetSplash()
{
	return m_splash;
}

UIImage* IGPGame::GetLogo()
{
	return m_logo;
}

void IGPGame::SetHasNewPrice(bool b)
{
	m_hasNewPrice = false;//new price disabled after feedback
}

void IGPGame::SetPriority(int prio, int prioIndex)
{
	m_priority[prioIndex] = prio;
}

void IGPGame::SetURL(char* url)
{
	strcpy(m_url, url);
}

void IGPGame::SetFileName(char* name)
{
	strcpy(m_fileName, name);
}

void IGPGame::SetMD5(char* md5)
{
	strcpy(m_md5, md5);
}

void IGPGame::SetCode(char* code)
{
	strcpy(m_gameCode, code);
}

UIImage* IGPGame::GetScreenshot(int index)
{
	return m_screenshots[index];
}

void IGPGame::SetSplash(IGPTexture* splash)
{
	SAFE_DELETE(m_splash);
	m_splash = splash;
}

void IGPGame::SetLogo(UIImage* logo)
{
	m_logo = logo;
}

void IGPGame::SetScreenshot(UIImage* scr, int index)
{
	if (m_screenshots[index] != NULL)
	{
		[m_screenshots[index] release];
	}
	m_screenshots[index] = scr;
}

void IGPGame::OpenBuyLink(const char* from)
{
	NSString* link = K_LINK_APPSTORE;
	link = [link stringByReplacingOccurrencesOfString:@"GAME"  withString:[NSString stringWithUTF8String:m_gameCode]];
	link = [link stringByReplacingOccurrencesOfString:@"VERSION" withString:[NSString stringWithUTF8String:IGP::s_igpDataVersion]];
	link = [link stringByReplacingOccurrencesOfString:@"FROM"  withString:[NSString stringWithUTF8String:IGP::s_igpDataGame]];
	link = [link stringByReplacingOccurrencesOfString:@"LANG"  withString:[NSString stringWithUTF8String:IGP::s_igpDataLang]];
	link = [link stringByReplacingOccurrencesOfString:@"COUNTRY"  withString:[NSString stringWithUTF8String:IGP::s_igpDataLocale]];
	link = [link stringByReplacingOccurrencesOfString:@"DEVICE"  withString:[[[UIDevice currentDevice] model]stringByReplacingOccurrencesOfString:@" " withString: @"_"]      ];
	link = [link stringByReplacingOccurrencesOfString:@"FIRMWARE"  withString:[[UIDevice currentDevice] systemVersion]];
	link = [link stringByReplacingOccurrencesOfString:@"UDIDPHONE"  withString:[[UIDevice currentDevice] uniqueIdentifier]];
	if (strlen(from))
	{
		link = [link stringByAppendingFormat:@"&spec_cat=%s", from];
	}
	trace(DEBUG_GAME, "BuyLink: %s\n", [link UTF8String]);
	if (IGP::s_igpInstance->getCheatState())
	{
		IGP::s_igpInstance->openUrl([link UTF8String], true, TXT_ERR_CONN_APPSTORE);

	}
	else
	{
		IGP::s_igpInstance->openUrl([link UTF8String], false, TXT_ERR_CONN_APPSTORE);
	}
	
	
}
void IGPGame::OpenVideoLink(const char* from)
{
	NSString* link = K_LINK_YOUTUBE;
	link = [link stringByReplacingOccurrencesOfString:@"GAME"  withString:[NSString stringWithUTF8String:m_gameCode]];
	link = [link stringByReplacingOccurrencesOfString:@"VERSION" withString:[NSString stringWithUTF8String:IGP::s_igpDataVersion]];
	link = [link stringByReplacingOccurrencesOfString:@"FROM"  withString:[NSString stringWithUTF8String:IGP::s_igpDataGame]];
	link = [link stringByReplacingOccurrencesOfString:@"LANG"  withString:[NSString stringWithUTF8String:IGP::s_igpDataLang]];
	link = [link stringByReplacingOccurrencesOfString:@"COUNTRY"  withString:[NSString stringWithUTF8String:IGP::s_igpDataLocale]];
	link = [link stringByReplacingOccurrencesOfString:@"DEVICE"  withString:[[[UIDevice currentDevice] model]stringByReplacingOccurrencesOfString:@" " withString: @"_"]      ];
	link = [link stringByReplacingOccurrencesOfString:@"FIRMWARE"  withString:[[UIDevice currentDevice] systemVersion]];
	link = [link stringByReplacingOccurrencesOfString:@"UDIDPHONE"  withString:[[UIDevice currentDevice] uniqueIdentifier]];
	if (strlen(from))
	{
		link = [link stringByAppendingFormat:@"&spec_cat=%s", from];
	}
	trace(DEBUG_GAME, "VideoLink: %s\n", [link UTF8String]);
	if (IGP::s_igpInstance->getCheatState())
	{
		IGP::s_igpInstance->openUrl([link UTF8String], true, TXT_ERR_CONN_SERVER);
	}
	else
	{
		IGP::s_igpInstance->openUrl([link UTF8String], false, TXT_ERR_CONN_SERVER);
	}
}
