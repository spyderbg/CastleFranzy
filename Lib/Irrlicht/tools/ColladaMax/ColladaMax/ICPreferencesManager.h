/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_PREFERENCES_MANAGER_H_
#define _IC_PREFERENCES_MANAGER_H_

#include "ICUIManager.h"
#include "ICPreference.h"

#define IC_PREFERENCES_FILE_PATH "IrrColladaConfig.xml"
#define IC_PREFERENCES_ARGVAR_BEGIN "$("
#define IC_PREFERENCES_ARGVAR_END ")"
#define IC_PREFERENCES_ARGVAR_FILE "FILE"

class IrrCollada;
class ICPreferencesPanel;

class ICPreferencesManager : public ICUIManager
{
private:
	PreferenceList m_preferences;
	fm::pvector<ICPreferencesPanel> m_panels;

public:
	ICPreferencesManager( IrrCollada *pIrrCollada ) 
		: ICUIManager( ICUIManager::MT_PREFERENCES_MANAGER, pIrrCollada )
	{
		Init();
	}

	~ICPreferencesManager();
	
	void Release();
	void Init();
	void RefreshDialog();
	void DisplayDialog( BOOL show );
	fm::string ConstructDialogCaption();

	static fm::string GetPreferencesFilePath();
	void ReadPreferencesFile();
	void WritePreferencesFile();
	void InitPreferences();

	DLG_PROC_DECL( ICPreferencesDialog )
	DLG_PROC_DECL( ICPreferencesRollout )

	inline size_t GetNumPreferences() const
	{
		return m_preferences.size();
	}

	inline const ICPreference &GetPreferenceRef( size_t index ) const
	{
		return *m_preferences[index];
	}

	ICPreference *GetPreferenceByXMLElementName( const fm::string &xmlElementName );

	static fm::string GetArgumentVariableString( const fm::string &varName );
};

#endif // _IC_PREFERENCES_MANAGER_H_


;