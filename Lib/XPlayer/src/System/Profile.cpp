#include "StdAfx.h"
#include "Profile.h"

#if PROFILE_ENABLE

#if(PLATFORM==PLATFORM_NGAGE)
#include <e32std.h>
#elif (PLATFORM == PLATFORM_MAC_OS_X)
#include<time.h>
#endif

CProfile::CProfile(void)
: m_nCount(0)
{
}

CProfile::~CProfile(void)
{
}

void CProfile::Clear()
{
	m_nCount = 0;
}

#if PLATFORM==PLATFORM_NGAGE
#  include <e32std.h>
#endif

static unsigned int GetMTime()
{
#if (PLATFORM==PLATFORM_NGAGE) || (PLATFORM==PLATFORM_SYMBIAN)
	TTime time;
	time.HomeTime();
	TInt64 t = time.Int64() / 1000;
	return I64INT(t);
#elif (PLATFORM==PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE)
	return GetTickCount();
#elif (PLATFORM==PLATFORM_MAC_OS_X)
	return clock();
#elif (PLATFORM == PLATFORM_KDDI)
	return GETUPTIMEMS();
#endif
}

void CProfile::Begin(const char* text)
{
#if(PLATFORM==PLATFORM_MAC_OS_X)
	//todo:
	return;
#endif
	ASSERT(m_nCount < MAX_PROFILES);
	ASSERT(text != NULL);
	ASSERT(strlen(text) < 256);

	for(int i = 0; i < Count(); i++)
	{
		if(strcmp(m_profiles[i]._name, text) == 0)
		{
			return;
		}
	}

	strcpy(m_profiles[m_nCount]._name, text);
	m_profiles[m_nCount]._time1 = GetMTime();
	++m_nCount;
}

void CProfile::End(const char* text)
{
	if(text == NULL)
	{
		m_profiles[m_nCount]._time2 = GetMTime();
	}
	else
	{
		for(int i = 0; i < Count(); i++)
		{
			if(strcmp(m_profiles[i]._name, text) == 0)
			{
				m_profiles[i]._time2 = GetMTime();
			}
		}
	}
}

#endif