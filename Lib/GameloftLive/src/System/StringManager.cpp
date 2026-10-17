////////////////////////////////////////////////////////////////////////////////////////////////////
//	StringManager.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GLLiveStdafx.h"
#include "StringManager.h"
#include "GLLiveGame.h"
#include "Porting.h"

#define LANGUAGE_NUM		6
#define NEW_RO				NEW
#define SAFE_DEL_ARRAY(p)	do{ if (p) { delete[] (p); p = NULL; } }while(0);

extern CGLLive * g_pGLLiveInstance;

namespace gllive
{
	////////////////////////////////////////////////////////////////////////////////////////////////////

	StringManager::StringManager()
	{
		for(int i = 0; i < LANGUAGE_NUM; i++ )
		{
			pOffsets[i]	=	NULL;
			pStrings[i]	=	NULL;
		}
		nStrings = 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	StringManager::~StringManager()
	{
		for(int i = 0; i < LANGUAGE_NUM; i++ )
		{
			SAFE_DEL_ARRAY(pOffsets[i]);
			SAFE_DEL_ARRAY(pStrings[i]);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//Loads the strings data from a file
	int	StringManager::load(int lang)
	{
		char* LANG[6] = {"EN","EN","DE","FR","IT","SP"};//{"EN","JP","DE","FR","IT","SP"};
		char FileName[256] = {0};
		//Open strings library
		strcpy(FileName, LANG[lang]);
		strcat(FileName, ".off");
		CGLLiveStream* pFS = NEW CResourceStream(FileName);

		char* pData1 = NEW char[pFS->GetLength()];
		pFS->Read(pData1, pFS->GetLength());

		//Read the number of strings
		nStrings	=	(short)((pData1[0]&0xFF) + ((pData1[1]&0xFF)<<8));
		//Alloc and read the string offsets
		pOffsets[lang]	=	NEW_RO u16[nStrings];
		if(!pOffsets[lang])
		{
			ASSERT(false);
			pFS->Close();
			SAFE_DELETE(pFS);
			return -3;
		}
		MEMCPY(pOffsets[lang], pData1 + 2, sizeof(u16) * nStrings);
		SAFE_DELETE(pData1);
		pFS->Close();
		SAFE_DELETE(pFS);

		//Alloc and read the strings
		strcpy(FileName, LANG[lang]);
		strcat(FileName, ".bin");
		pFS = NEW CResourceStream(FileName);
		pStrings[lang]	=	NEW_RO char[pFS->GetLength()];
		if(!pStrings[lang])
		{
			ASSERT(false);
			pFS->Close();
			SAFE_DELETE(pFS);
			return -4;
		}
		pFS->Read(pStrings[lang], sizeof(char) * pFS->GetLength());

		for(int i = 0; i < sizeof(char) * pFS->GetLength() ; i++)
		{
			//int c = GetChar(pStrings[lang], i);
			//if(c == '|')
			if(pStrings[lang][i] == '|')//0x7C, 11111100
			{
				pStrings[lang][i] = '\n';
			}
		}
		
		//No error
		pFS->Close();
		SAFE_DELETE(pFS);
		return 0;
	}

	int	StringManager::load()
	{
		for(int i = 0; i < LANGUAGE_NUM; i++ )
			load(i);
		return 0;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//Returns the string at the given index

	const char * StringManager::getString(int idx, int lang)
	{
		if(idx < 0 || idx >= NUM_STRINGS_EN)
			return "?";
		//int lang = Game::getGame()->m_Language ;//LANGUAGE_SPANLISH
		//ASSERT(Game::getGame()->m_Language >= LANGUAGE_ENGLISH && Game::getGame()->m_Language <= LANGUAGE_JAPAN);
		if(lang == -1)
			return (pStrings[g_pGLLiveInstance->m_Language] + (idx ? pOffsets[g_pGLLiveInstance->m_Language][idx - 1] : 0));
		else
			return (pStrings[lang] + (idx ? pOffsets[lang][idx - 1] : 0));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//Returns the string at the given index

	char* GetString(int index, int lang)
	{
		return (char*)g_pGLLiveInstance->pStringManager->getString(index, lang);
	}
}