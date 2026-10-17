#include "StdAfx.h"
#include "ICPreference.h"
#include "ICWidget.h"
#include "MAXHelpers.h"
#include "IrrCollada.h"

ICPreference::ICPreference( int widgetType, const fm::string &xmlElementName )
: m_xmlElementName( xmlElementName )
, m_flags( NULL )
{
	m_pWidget = ICWidgetFactory::Create( ( ICWidget::Type )widgetType );
}


ICPreference::~ICPreference()
{
	SAFE_DELETE( m_pWidget );
}

BOOL ICPreference::DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		//case WM_INITDIALOG: 
		//{
		//	return TRUE;
		//}
		default:
		{
			if ( m_pWidget->DialogProc( hWnd, message, wParam, lParam ) )
			{
				SetValueFromWidget( hWnd );
				return TRUE;
			}
		}
	}

	return FALSE;
}
