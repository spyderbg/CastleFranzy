/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _ICPANEL_H_
#define _ICPANEL_H_

class IRollupWindow;
class ICObjectProperty;
class ICObjectPropertyManager;
class ICPreferencesManager;
class ICPreference;

typedef fm::pvector<ICPreference> PreferenceList;

class ICPanel
{
protected:
	int m_dialogIdd;
	int m_dialogTitleIds;
	int m_panelRollupId;

public:
	ICPanel( int dialogIdd, int dialogTitleIds )
		: m_dialogIdd( dialogIdd )
		, m_dialogTitleIds( dialogTitleIds )
		, m_panelRollupId( -1 )
	{}

	inline int GetDialogIdd() const
	{
		return m_dialogIdd;
	}

	inline int GetDialogTitleIds() const
	{
		return m_dialogTitleIds;
	}

	inline int GetPanelRollupId() const
	{
		return m_panelRollupId;
	}
};

class ICObjectPropertyPanel
	: public ICPanel
{
private:
	fm::pvector<ICObjectProperty> m_objectProperties;

public:
	ICObjectPropertyPanel( int dialogIdd, int dialogTitleIds )
		: ICPanel( dialogIdd, dialogTitleIds )
	{}

	inline size_t GetNumObjectProperties() const
	{
		return m_objectProperties.size();
	}

	inline size_t AddObjectProperty( ICObjectProperty *pObjectProperty )
	{
		m_objectProperties.push_back( pObjectProperty );
		return m_objectProperties.size();
	}

	inline const ICObjectProperty &GetObjectPropertyRef( size_t index ) const
	{
		return *m_objectProperties[index];
	}

	inline ICObjectProperty &GetObjectPropertyRef( size_t index )
	{
		return *m_objectProperties[index];
	}

	void Init( IRollupWindow *pRollupWindow, DLGPROC dlgProc, ICObjectPropertyManager *pObjectPropertyManager );
};

class ICPreferencesPanel
	: public ICPanel
{
private:
	PreferenceList m_preferences;

public:
	ICPreferencesPanel( int dialogIdd, int dialogTitleIds )
		: ICPanel( dialogIdd, dialogTitleIds )
	{}

	inline size_t GetNumPreferences() const
	{
		return m_preferences.size();
	}

	inline size_t AddPreference( ICPreference *pPreference )
	{
		m_preferences.push_back( pPreference );
		return m_preferences.size();
	}

	inline const ICPreference &GetPreferenceRef( size_t index ) const
	{
		return *m_preferences[index];
	}

	inline ICPreference &GetPreferenceRef( size_t index )
	{
		return *m_preferences[index];
	}

	void Init( IRollupWindow *pRollupWindow, DLGPROC dlgProc, ICPreferencesManager *pPreferenceManager );
};

#endif // _ICPANEL_H_


