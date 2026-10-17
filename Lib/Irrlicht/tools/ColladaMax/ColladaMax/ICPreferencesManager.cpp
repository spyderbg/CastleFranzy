#include "StdAfx.h"
#include "ICPreferencesManager.h"
#include "ICPreferenceFile.h"
#include "ICPreferenceFileSyntax.h"
#include "ICPreferenceFileParser.h"
#include "ICPreferenceFileWriter.h"
#include "IrrCollada.h"
#include "resource.h"
#include "ICWidget.h"
#include "ICPanel.h"
#include "DrawLayerManager.h"
#include "UIHelpers.h"
#include "MAXHelpers.h"
#include "IPathConfigMgr.h"
#include "FUtils/FUDaeEnum.h"
#include "FUtils/FUFileManager.h"

ICPreferencesManager::~ICPreferencesManager()
{
	Release();
}

void ICPreferencesManager::Release()
{
	ICUIManager::Release();
	CLEAR_POINTER_VECTOR( m_preferences );
	CLEAR_POINTER_VECTOR( m_panels );
}

void ICPreferencesManager::Init()
{
	Release();

	ICPreferencesPanel *pPanel = NULL;

	// ----------------------------------------------------------------------
	// Preview Panel
	// ----------------------------------------------------------------------
	pPanel = new ICPreferencesPanel( IDD_PREFERENCES_PREVIEW_ROLLUP, IDS_IC_PREVIEW_ROLLUP_TITLE );
	m_panels.push_back( pPanel );

	// text editor path
	{
		fm::string default( "C:\\WINDOWS\\system32\\notepad.exe" );
		ICPreference *pPreference = new ICPreferenceT<fm::string>( ICWidget::FileChooser, default, ICPREF_TEXTEDITOR_PATH );
		ICFileChooser &widgetRef = ( ICFileChooser & ) pPreference->GetWidgetRef();
		widgetRef.Construct( IDC_IC_TEXT_EDITOR_PATH_LABEL, IDC_IC_TEXT_EDITOR_PATH_EDITTEXT, IDC_IC_TEXT_EDITOR_PATH_BUTTON, default );
		m_preferences.push_back( pPreference );
		pPanel->AddPreference( pPreference );
	}

	// text editor arguments
	{
		fm::string default( GetArgumentVariableString( IC_PREFERENCES_ARGVAR_FILE ) );
		ICPreference *pPreference = new ICPreferenceT<fm::string>( ICWidget::EditText, default, ICPREF_TEXTEDITOR_ARGUMENTS );
		ICEditText &widgetRef = ( ICEditText & ) pPreference->GetWidgetRef();
		widgetRef.Construct( IDC_IC_TEXT_EDITOR_ARGUMENTS_LABEL, IDC_IC_TEXT_EDITOR_ARGUMENTS_EDITTEXT, default );
		m_preferences.push_back( pPreference );
		pPanel->AddPreference( pPreference );
	}

	ReadPreferencesFile();
}

fm::string ICPreferencesManager::GetPreferencesFilePath()
{
	fm::string filePath = IPathConfigMgr::GetPathConfigMgr()->GetDir( APP_PLUGCFG_DIR );
	filePath = ( filePath + PATH_SEPARATOR ) + IC_PREFERENCES_FILE_PATH;
	return filePath;
}

void ICPreferencesManager::ReadPreferencesFile()
{
	ICPreferenceFile preferenceFile;
	preferenceFile.SetVersionNumber( VersionNumber( 0, 0, 0, 0 ) );
	preferenceFile.SetPreferences( &m_preferences );

	ICPreferenceFileParser parser;
	parser.Read( GetPreferencesFilePath(), preferenceFile );
}

void ICPreferencesManager::WritePreferencesFile()
{
	ICPreferenceFile preferenceFile;
	preferenceFile.SetVersionNumber( VersionNumber( 0, 0, 0, 0 ) );
	preferenceFile.SetPreferences( &m_preferences );

	ICPreferenceFileWriter writer;
	writer.Write( GetPreferencesFilePath(), preferenceFile );
}

void ICPreferencesManager::InitPreferences()
{
	ReadPreferencesFile();

	// clearing all dirty flags
	for ( PreferenceList::iterator prefIter = m_preferences.begin(); prefIter != m_preferences.end(); ++prefIter )
	{
		ICPreference *pPreference = *prefIter;
		pPreference->SetFlags( 0 );
	}
}

void ICPreferencesManager::DisplayDialog( BOOL show )
{
	if ( show )
	{
		if ( !m_hWnd )
		{
			InitPreferences();

			m_hWnd = CreateDialogParam( hInstance, MAKEINTRESOURCE(IDD_PREFERENCES_DIALOG), GetCOREInterface()->GetMAXHWnd(), DLG_PROCS_NAME( ICPreferencesDialog ), (LPARAM)this );
			GetCOREInterface()->RegisterDlgWnd( m_hWnd ); // Max doc recommends to call this for floating dialogs
		}
		else
		{
			WINDOWINFO windowInfo;
			int retVal;
			if ( !(retVal = GetWindowInfo( m_hWnd, &windowInfo )))
			{
				return;
			}

			SetActiveWindow( m_hWnd );
			if ( TEST_FLAG( windowInfo.dwStyle, WS_MINIMIZE ) )
			{
				ShowWindow( m_hWnd, SW_RESTORE );
			}
		}
	}
}

void ICPreferencesManager::RefreshDialog()
{
	if ( m_hWnd )
	{
		// dialog caption
		fm::string caption = ConstructDialogCaption();
		SetWindowText( m_hWnd, caption.c_str() );

		bool bDirtyPreferences = false;
		for ( PreferenceList::iterator prefIter = m_preferences.begin(); prefIter != m_preferences.end(); ++prefIter )
		{
			ICPreference *pPreference = *prefIter;
			if ( TEST_FLAG( pPreference->GetFlags(), ICPreference::PF_DIRTY ) )
			{
				bDirtyPreferences = true;
				break;
			}
		}

		UIHelpers::SetControlEnabled( m_hWnd, IDAPPLY, bDirtyPreferences );
	}
}

DLG_PROC_IMPL( ICPreferencesManager, ICPreferencesRollout )
{
	// preference widgets
	{
		BOOL bMessageProcessed = FALSE;
		for ( size_t propId = 0; propId < m_preferences.size(); ++propId )
		{
			ICPreference &prop = *m_preferences[propId];
			if ( prop.DialogProc( hWnd, message, wParam, lParam ) )
			{
				bMessageProcessed = TRUE;
				RefreshDialog();
				break;
			}
		}

		if ( bMessageProcessed )
		{
			return bMessageProcessed;
		}
	}

	return FALSE;
}

DLG_PROC_IMPL( ICPreferencesManager, ICPreferencesDialog )
{
	BOOL bMessagedProcessed = ICUIManager::MainDlgProc( hWnd, message, wParam, lParam );

	int loWord = LOWORD( wParam );
	int hiWord = HIWORD( wParam );

	switch (message)
	{
		case WM_INITDIALOG: 
		{
			HWND rollupHWnd = GetDlgItem( hWnd, IDC_MAIN_ROLLUP );
			IRollupWindow *pRollup = GetIRollup( rollupHWnd );

			for ( size_t panelId = 0; panelId < m_panels.size(); ++panelId )
			{
				m_panels[panelId]->Init( pRollup, DLG_PROCS_NAME( ICPreferencesRollout ), this );	
			}

			pRollup->Show();
			ReleaseIRollup( pRollup );

			RefreshDialog();
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (loWord)
			{		
				case IDCANCEL:
				{
					return TRUE;
				}

				case IDOK:
				case IDAPPLY:
				{
					bool bDirtyPreferences;
					{
						HWND rollupHWnd = GetDlgItem( hWnd, IDC_MAIN_ROLLUP );
						IRollupWindow *pRollup = GetIRollup( rollupHWnd );

						for ( size_t panelId = 0; panelId < m_panels.size(); ++panelId )
						{
							ICPreferencesPanel &panel = *m_panels[panelId];
							int panelRollupId = panel.GetPanelRollupId();
							HWND panelHWnd = pRollup->GetPanelDlg( panelRollupId );

							size_t numPreferences = panel.GetNumPreferences();
							for ( size_t prefId = 0; prefId < numPreferences; ++prefId )
							{
								ICPreference &preference = panel.GetPreferenceRef( prefId );
								preference.SetValueFromWidget( panelHWnd );
								bDirtyPreferences |= TEST_FLAG( preference.GetFlags(), ICPreference::PF_DIRTY );
							}
						}
			
						ReleaseIRollup( pRollup );
					}

					bool bFileExists = false;
					{
						FUFileManager fileManager;
						bFileExists = fileManager.FileExists( GetPreferencesFilePath() );
					}

					if ( bDirtyPreferences || !bFileExists )
					{
						WritePreferencesFile();
					}

					InitPreferences();
					RefreshDialog();
					return TRUE;
				}
			}
			break;
		}
	}
	return bMessagedProcessed;
}

fm::string ICPreferencesManager::ConstructDialogCaption()
{
	return GetString( IDS_PREFERENCES_DIALOG_TITLE );
}

ICPreference *ICPreferencesManager::GetPreferenceByXMLElementName( const fm::string &xmlElementName)
{
	ICPreference *pReturnedPreference = NULL;
	for ( size_t propId = 0; m_preferences.size(); ++propId )
	{
		ICPreference *pPreference = m_preferences[propId];
		if ( IsEquivalent( pPreference->GetXMLElementName(), xmlElementName ) )
		{
			pReturnedPreference = pPreference;
			break;
		}
	}

	return pReturnedPreference;
}

fm::string ICPreferencesManager::GetArgumentVariableString( const fm::string &varName )
{
	return fm::string( IC_PREFERENCES_ARGVAR_BEGIN ) + varName + IC_PREFERENCES_ARGVAR_END;
}