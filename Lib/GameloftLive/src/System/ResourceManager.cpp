// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min
#include "GLLiveStdafx.h"
#include "resourcemanager.h"

namespace gllive
{
	CGLLiveResourceManager::CGLLiveResourceManager()
	{
		memset(m_files, 0, sizeof(m_files));
	}

	CGLLiveResourceManager::~CGLLiveResourceManager()
	{
	#if DEBUG_LEVEL > 0
		//make sure all files are removed
		for(int i = 0; i < MAX_RESOURCE_FILE; i++)
			ASSERT(m_files[i] == NULL);
	#endif
	}

	void CGLLiveResourceManager::AddResFile(CGLLiveResFile *file)
	{
		int i;
		for(i = 0; i < MAX_RESOURCE_FILE; i++)
		{
			if(m_files[i] == file)
			{
				ASSERT(false);
				return;
			}
			if(m_files[i] == NULL)
			{
				m_files[i] = file;
				return;
			}
		}

		//too much resource files have been added
		ASSERT(false);
	}

	void CGLLiveResourceManager::RemoveResFile(CGLLiveResFile *file)
	{
		int i;
		for(i = 0; i < MAX_RESOURCE_FILE; i++)
		{
			if(m_files[i] == file)
			{
				m_files[i] = NULL;
				return;
			}
		}
	}

	CGLLiveStream* CGLLiveResourceManager::GetResourceAsStream(const char *name)
	{
		void*		pRecord = NULL;
		CGLLiveResFile*	pResFile = NULL;
		int		length = 0;

		FindRecord(name, &pRecord, &pResFile, length);

		if(pRecord != NULL)
		{
			CGLLiveMemoryStream *pStream = NEW CGLLiveMemoryStream(length);
			pResFile->ReadRecord(pRecord, pStream->GetBuffer(), length);
			return pStream;
		}
		else
		{
			CGLLiveFileStream fs;

			if(!fs.Open(name))
			{
				//file does not exist in either package or direct file
				ASSERT(false);
				return NULL;
			}

			length = fs.GetLength();
			CGLLiveMemoryStream* pStream = NEW CGLLiveMemoryStream(length);
			fs.Read(pStream->GetBuffer(), length);
			fs.Close();
			return pStream;
		}
	}

	void CGLLiveResourceManager::FindRecord(const char* name, void** pRecord, CGLLiveResFile** pResFile, int& length)
	{
		*pRecord = NULL;
		*pResFile = NULL;

		length = 0;

		for(int i = 0; i < MAX_RESOURCE_FILE; i++)
		{
			if(m_files[i] != NULL)
			{
				*pResFile = m_files[i];

				*pRecord = m_files[i]->FindRecord(name);

				if(*pRecord)
				{
					length =  m_files[i]->GetRecordSize(*pRecord);
					break;
				}
			}
		}
	}
}
