/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_PREFERENCE_FILE_H_
#define _IC_PREFERENCE_FILE_H_

#include "VersionNumber.h"

class ICPreference;
typedef fm::pvector<ICPreference> PreferenceList;

class ICPreferenceFile
{
private:
	VersionNumber m_versionNumber;
	PreferenceList *m_pPreferences;

public:
	ICPreferenceFile()
		: m_versionNumber( 0, 0, 0, 0 )
		, m_pPreferences( NULL )
	{}
	~ICPreferenceFile(){}

	inline void SetVersionNumber( const VersionNumber &versionNumber )
	{
		m_versionNumber = versionNumber;
	}

	inline const VersionNumber &GetVersionNumber()
	{
		return m_versionNumber;
	}

	inline void SetPreferences( PreferenceList *pPreferences )
	{
		m_pPreferences = pPreferences;
	}

	inline PreferenceList *GetPreferences()
	{
		return m_pPreferences;
	}
};

#endif // _IC_PREFERENCE_FILE_H_


