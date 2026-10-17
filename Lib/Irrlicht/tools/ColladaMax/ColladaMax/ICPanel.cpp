#include "StdAfx.h"
#include "ICPanel.h"
#include "ICWidget.h"
#include "ICObjectProperty.h"
#include "ICPreference.h"

void ICObjectPropertyPanel::Init( IRollupWindow *pRollupWindow, DLGPROC dlgProc, ICObjectPropertyManager *pObjectPropertyManager )
{
	FUAssert( pRollupWindow, return );

	m_panelRollupId = pRollupWindow->AppendRollup( hInstance, MAKEINTRESOURCE(m_dialogIdd), dlgProc, GetString(m_dialogTitleIds), ( LPARAM )pObjectPropertyManager );
	HWND rollupPanelHWnd = pRollupWindow->GetPanelDlg( m_panelRollupId );

	for ( size_t propId = 0; propId < m_objectProperties.size(); ++propId )
	{
		ICObjectProperty &objectProperty = *m_objectProperties[propId];
		objectProperty.InitWidget( rollupPanelHWnd );
	}
}

void ICPreferencesPanel::Init( IRollupWindow *pRollupWindow, DLGPROC dlgProc, ICPreferencesManager *pPreferenceManager )
{
	FUAssert( pRollupWindow, return );

	m_panelRollupId = pRollupWindow->AppendRollup( hInstance, MAKEINTRESOURCE(m_dialogIdd), dlgProc, GetString(m_dialogTitleIds), ( LPARAM )pPreferenceManager );
	HWND rollupPanelHWnd = pRollupWindow->GetPanelDlg( m_panelRollupId );

	for ( size_t propId = 0; propId < m_preferences.size(); ++propId )
	{
		ICPreference &preference = *m_preferences[propId];
		preference.InitWidget( rollupPanelHWnd );
	}	
}