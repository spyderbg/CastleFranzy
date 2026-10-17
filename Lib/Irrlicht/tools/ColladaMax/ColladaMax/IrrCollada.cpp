#include "StdAfx.h"
#include "IrrCollada.h"
#include "ICObjectPropertyManager.h"
#include "ICPreferencesManager.h"
#include "ICPreferenceFileSyntax.h"
#include "ICWidget.h"
#include "UIHelpers.h"
#include "ColladaExporter.h"
#include "ipathconfigmgr.h"
#include "MAXHelpers.h"

#define PREVIEW_DAE_FILENAME "preview.dae"

IrrCollada g_irrCollada(
   IRRCOLLADA_FN_INTERFACE, _T("IrrCollada"), 0, NULL, FP_CORE,
   IrrCollada::pm_loadClass, _T("loadClass"), 0, TYPE_VOID, 0, 1,
      _T("class"), 0, TYPE_CLASS,
   properties,
      IrrCollada::pm_isVisible, IrrCollada::pm_setVisible, _T("visible"), 0, TYPE_BOOL,
   end);

IrrCollada *GetIrrColladaPtr()
{
	return &g_irrCollada;
}

IrrColladaClassDesc g_irrColladaClassDesc;
ClassDesc2 *GetIrrColladaClassDesc()
{
	return &g_irrColladaClassDesc;
}

void *IrrColladaClassDesc::Create(BOOL isLoading)
{
	g_irrCollada.Reset();
	return &g_irrCollada;
}

// Action Items Descriptor
class IrrColladaActions: public FPStaticInterface
{
public:     
	FPStatus DisplayObjectProperties()
	{ 
		g_irrCollada.SetVisible( TRUE, 0 ); 
		return FPS_OK;
	}

	FPStatus DisplayPreferences()
	{
		g_irrCollada.SetVisible( TRUE, 1 ); 
		return FPS_OK;
	}

	FPStatus RunMaxScript( const char *maxScriptFilename )
	{
		if ( !maxScriptFilename )
		{
			return FPS_FAIL;
		}

		fstring maxScriptPath = fstring( IPathConfigMgr::GetPathConfigMgr()->GetDir( APP_SCRIPTS_DIR ) ) + PATH_SEPARATOR + maxScriptFilename;
		g_irrCollada.ExecuteFileScript( const_cast<TCHAR *>( maxScriptPath.c_str() ) );
		return FPS_OK;
	}

	FPStatus RunAnimationSelector()
	{
		return RunMaxScript( "animationselector.ms" );
	}

	FPStatus RunUserPropertiesEditor()
	{
		return RunMaxScript( "UserPropertiesEditor.ms" );
	}

	FPStatus PreviewColladaEditor( BOOL suppressPrompts = FALSE )
	{
		// determine tmp .dae file path
		fstring fileName( PREVIEW_DAE_FILENAME );
		fstring filePath = fstring( MAXHelpers::GetTmpDir() ) + PATH_SEPARATOR + fileName;

		fstring cameraIdentifier;

		// export
		COLLADAExporter *pExporter = ( COLLADAExporter * ) GetCOLLADAExporterDesc()->Create();
		pExporter->SetExportMode( COLLADAExporter::EM_PREVIEW );
		pExporter->DoExport( filePath.c_str(), NULL, GetCOREInterface(), suppressPrompts );
		bool bExportSuccess = pExporter->GetExportSuccess();
		cameraIdentifier = pExporter->GetPreviewCameraIdentifier();
		SAFE_DELETE( pExporter );

		if ( bExportSuccess )
		{
			PROCESS_INFORMATION pi = g_irrCollada.GetColladaEditorProcessInfo();
			bool bTerminateProcess = false;
			if ( pi.hProcess )
			{
				DWORD exitCode;
				BOOL returnCode = GetExitCodeProcess( pi.hProcess, &exitCode );
				if ( returnCode )
				{
					bTerminateProcess = exitCode == STILL_ACTIVE;
				}

				if ( !bTerminateProcess )
				{
					CloseHandle( pi.hProcess );
				}
			}

			if ( bTerminateProcess )
			{
				// TODO : Use BroadcastSystemMessage to terminate process
				// See "Terminating a Process" in MSDN doc
				TerminateProcess( pi.hProcess, 0 );
				CloseHandle( pi.hProcess );
			}

			{
				STARTUPINFO si;
				ZeroMemory( &pi, sizeof(pi) );
				ZeroMemory( &si, sizeof(si) );
				si.cb = sizeof( si );
				si.wShowWindow = SW_NORMAL;
				si.dwFlags = STARTF_USESHOWWINDOW;

				FUStringBuilder builder;
				builder.append( "\"" );
				builder.append( filePath );
				builder.append( "\"" );

				if ( !cameraIdentifier.empty() )
				{
					builder.append( " /c " );
					builder.append( cameraIdentifier );
				}

				char commandLine[1024];
				strcpy( commandLine, builder.ToCharPtr() );

				HKEY hk;
				if ( RegOpenKeyExA( HKEY_CURRENT_USER, "Software\\Gameloft\\Irrlicht\\MeshViewer", 0, KEY_READ, &hk) )
				{
					return FPS_FAIL;
				}

				char szPath[MAX_PATH];
				DWORD dwBufLen = MAX_PATH;
				if ( RegQueryValueEx( hk,             // subkey handle 
					"Path",					// value name 
					NULL, NULL, 
					(LPBYTE) szPath,				// pointer to value data 
					(DWORD*) (&dwBufLen)))			// data size
				{
					RegCloseKey(hk); 
					return FPS_FAIL;
				}

				RegCloseKey(hk);
				if( !CreateProcess(szPath, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) )
				{
					return FPS_FAIL;
				}

				g_irrCollada.SetColladaEditorProcessInfo( pi );
			}
		}

		return FPS_OK;
	}

	FPStatus PreviewColladaEditorQuick()
	{
		return PreviewColladaEditor( TRUE );
	}

	FPStatus PreviewTextEditor( BOOL suppressPrompts = FALSE )
	{
		// determine tmp .dae file path
		fstring fileName( PREVIEW_DAE_FILENAME );
		fstring filePath = fstring( MAXHelpers::GetTmpDir() ) + PATH_SEPARATOR + fileName;

		// export
		COLLADAExporter *pExporter = ( COLLADAExporter * ) GetCOLLADAExporterDesc()->Create();
		pExporter->SetExportMode( COLLADAExporter::EM_PREVIEW );
		pExporter->DoExport( filePath.c_str(), NULL, GetCOREInterface(), suppressPrompts );
		bool bExportSuccess = pExporter->GetExportSuccess();
		SAFE_DELETE( pExporter );

		if ( bExportSuccess )
		{
			PROCESS_INFORMATION pi = g_irrCollada.GetTextEditorProcessInfo();
			//bool bTerminateProcess = false;
			//if ( pi.hProcess )
			//{
			//	DWORD exitCode;
			//	BOOL returnCode = GetExitCodeProcess( pi.hProcess, &exitCode );
			//	if ( returnCode )
			//	{
			//		bTerminateProcess = exitCode == STILL_ACTIVE;
			//	}

			//	if ( !bTerminateProcess )
			//	{
			//		CloseHandle( pi.hProcess );
			//	}
			//}

			//if ( bTerminateProcess )
			//{
			//	// TODO : Use BroadcastSystemMessage to terminate process
			//	// See "Terminating a Process" in MSDN doc
			//	TerminateProcess( pi.hProcess, 0 );
			//	CloseHandle( pi.hProcess );
			//}

			{
				STARTUPINFO si;
				ZeroMemory( &pi, sizeof(pi) );
				ZeroMemory( &si, sizeof(si) );
				si.cb = sizeof( si );
				si.wShowWindow = SW_NORMAL;
				si.dwFlags = STARTF_USESHOWWINDOW;

				// hack
				ICPreferencesManager &preferencesManager = *( ICPreferencesManager * )g_irrCollada.GetICManagers()[1];

				char szPath[MAX_PATH];
				{
					ICPreference &preference = *preferencesManager.GetPreferenceByXMLElementName( ICPREF_TEXTEDITOR_PATH );
					fm::string editorPath;
					preference.SetStringFromValue( editorPath );
					strcpy( szPath, editorPath.c_str() );
				}

				char commandLine[MAX_PATH];
				{
					ICPreference &preference = *preferencesManager.GetPreferenceByXMLElementName( ICPREF_TEXTEDITOR_ARGUMENTS );
					fm::string arguments;
					preference.SetStringFromValue( arguments );

					// variable replacement
					{
						fm::string varName = preferencesManager.GetArgumentVariableString( IC_PREFERENCES_ARGVAR_FILE );
						//size_t varStart	= arguments.find( IC_PREFERENCES_ARGVAR_BEGIN );
						size_t varStart = arguments.find( varName );
						while( varStart != fm::string::npos )
						{
							//size_t varEnd = arguments.find( IC_PREFERENCES_ARGVAR_END, varStart );
							//if ( varEnd != fm::string::npos )
							//{
								//size_t varNameStart = varStart + strlen( IC_PREFERENCES_ARGVAR_BEGIN );
								//size_t varNameEnd = varEnd - strlen( IC_PREFERENCES_ARGVAR_END );
								//fm::string potentialVarName = arguments.substr( varNameStart, varNameEnd - varNameStart );
								//_stricmp( potentialVarName.c_str(), IC_PREFERENCES_ARGVAR_FILE );
							
								arguments = arguments.substr( 0, varStart ? varStart + 1 : 0 )
									+ filePath
									+ arguments.substr( varStart + strlen( varName ) );
							//}
							varStart = arguments.find( varName );
						}
					}
					sprintf( commandLine, " \"%s\"", arguments.c_str() );
					//strcpy( commandLine, arguments.c_str() );
				}

				//sprintf(commandLine, " \"%s\"", filePath.c_str() );
				//strcpy( szPath, "C:\\Program Files\\Notepad++\\notepad++.exe" );
				//strcpy( szPath, "C:\\WINDOWS\\system32\\notepad.exe" );

				if( !CreateProcess(szPath, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) )
				{
					return FPS_FAIL;
				}

				g_irrCollada.SetTextEditorProcessInfo( pi );
			}
		}

		return FPS_OK;
	}

	FPStatus PreviewTextEditorQuick()
	{
		return PreviewTextEditor( TRUE );
	}

	// action dispatch
	DECLARE_DESCRIPTOR(IrrColladaActions)
	enum ActionID 
	{ 
		pm_displayObjectProperties
		, pm_runAnimationSelector
		, pm_runUserPropertiesEditor
		, pm_previewColladaEditor
		, pm_previewColladaEditorQuick
		, pm_previewTextEditor
		, pm_previewTextEditorQuick
		, pm_displayPreferences
		, pm_count 
	};

	BEGIN_FUNCTION_MAP
	FN_ACTION(pm_displayObjectProperties, DisplayObjectProperties);
	FN_ACTION(pm_runAnimationSelector, RunAnimationSelector);
	FN_ACTION(pm_runUserPropertiesEditor, RunUserPropertiesEditor);
	FN_ACTION(pm_previewColladaEditor, PreviewColladaEditor);
	FN_ACTION(pm_previewColladaEditorQuick, PreviewColladaEditorQuick);
	FN_ACTION(pm_previewTextEditor, PreviewTextEditor);
	FN_ACTION(pm_previewTextEditorQuick, PreviewTextEditorQuick);
	FN_ACTION(pm_displayPreferences, DisplayPreferences);
	END_FUNCTION_MAP
};

static IrrColladaActions g_irrColladaActions
(
   IRRCOLLADA_ACT_INTERFACE, _T("IrrColladaAction"), IDS_IC_CLASS_NAME, &g_irrColladaClassDesc, FP_ACTIONS, kActionMainUIContext, 
      IrrColladaActions::pm_displayObjectProperties, _T("displayObjectProperties"), IDS_IC_OBJECT_PROPERTIES_MENU_ITEM_TITLE, 0,end,
      IrrColladaActions::pm_runAnimationSelector, _T("runAnimationSelector"), IDS_IC_RUN_ANIMATION_SELECTOR_MENU_ITEM_TITLE, 0,end,
      IrrColladaActions::pm_runUserPropertiesEditor, _T("runUserPropertiesEditor"), IDS_IC_RUN_USER_PROPERTIES_EDITOR_MENU_ITEM_TITLE, 0,end,
      IrrColladaActions::pm_previewColladaEditor, _T("previewColladaEditor"), IDS_IC_PREVIEW_COLLADA_EDITOR_MENU_ITEM_TITLE, 0,end,
      IrrColladaActions::pm_previewColladaEditorQuick, _T("previewColladaEditorQuick"), IDS_IC_PREVIEW_COLLADA_EDITOR_QUICK_MENU_ITEM_TITLE, 0,end,
      IrrColladaActions::pm_previewTextEditor, _T("previewTextEditor"), IDS_IC_PREVIEW_TEXT_EDITOR_MENU_ITEM_TITLE, 0,end,
      IrrColladaActions::pm_previewTextEditorQuick, _T("previewTextEditorQuick"), IDS_IC_PREVIEW_TEXT_EDITOR_QUICK_MENU_ITEM_TITLE, 0,end,
      IrrColladaActions::pm_displayPreferences, _T("displayPreferences"), IDS_IC_PREFERENCES_MENU_ITEM_TITLE, 0,end,
   end
);

IrrCollada::IrrCollada(BOOL isLoading)
{
	Reset();
}

IrrCollada::~IrrCollada()
{
	CLEAR_POINTER_VECTOR( m_ICManagers );
}

void IrrCollada::RefreshSelectedNodes()
{
	m_selectedNodes.clear();

	int selNodeCount = GetCOREInterface()->GetSelNodeCount();
	for ( int nodeId = 0; nodeId < selNodeCount; ++nodeId )
	{
		m_selectedNodes.push_back( GetCOREInterface()->GetSelNode( nodeId ) );
	}
}

void IrrCollada::NotificationCallback( void *param, NotifyInfo *info )
{
	switch ( info->intcode )
	{
		case NOTIFY_SELECTIONSET_CHANGED :
		{
			g_irrCollada.RefreshSelectedNodes();
			//g_irrCollada.RefreshMenu();

			// hack
			fm::pvector<ICUIManager> &iCManagers = g_irrCollada.GetICManagers();
			iCManagers[0]->RefreshDialog();
			//for ( fm::pvector<ICUIManager>::iterator iCManagerIter = iCManagers.begin(); iCManagerIter != iCManagers.end(); ++iCManagerIter )
			//{
			//	(*iCManagerIter)->RefreshDialog();
			//}
			break;
		}
		case NOTIFY_SYSTEM_SHUTDOWN :
		{
			g_irrCollada.RemoveMenuEntries();
			break;
		}
		case NOTIFY_POST_NODES_CLONED:
		{
			struct PostCloneInfo
			{
				INodeTab *pOrigNodes;
				INodeTab *pClonedNodes;
				CloneType cloneType;
			};

			PostCloneInfo *pPostCloneInfo = ( PostCloneInfo * )info->callParam;

			fm::pvector<ICUIManager> &iCManagers = g_irrCollada.GetICManagers();
			for ( fm::pvector<ICUIManager>::iterator iCManagerIter = iCManagers.begin(); iCManagerIter != iCManagers.end(); ++iCManagerIter )
			{
				(*iCManagerIter)->PostCloneNotification( pPostCloneInfo->pOrigNodes, pPostCloneInfo->pClonedNodes, pPostCloneInfo->cloneType );
			}
			break;
		}
	}
}

void IrrCollada::RemoveMenuEntries()
{
	IMenuManager *pMenuManager = GetCOREInterface()->GetMenuManager();
	
	IMenuBarContext *pMenuContext = ( IMenuBarContext * )pMenuManager->GetContext( kMainMenuBar );
	IMenu *pMenu = pMenuContext->GetMenu();
	UIHelpers::RemoveSubMenu( pMenu, GetString(IDS_IC_SUBMENU_TITLE) );
	pMenuManager->UpdateMenuBar();
}

DWORD_PTR IrrCollada::Control( DWORD parameter ) {
   if (parameter==1) SetVisible(TRUE, 0);
   else if(parameter==0) SetVisible(FALSE, 0);
   return 0;
}

void IrrCollada::SetVisible(BOOL show, int icManagerId)
{
	m_ICManagers[icManagerId]->DisplayDialog( show );
}

BOOL IrrCollada::IsVisible()
{
   return FALSE;
}

ActionItem *IrrCollada::GetActionItem( int actionId )
{
	if ( actionId < 0
		|| actionId >= IrrColladaActions::pm_count )
	{
		return NULL;
	}

	return (*g_irrColladaActions.GetActionTable())[actionId];
}


IMenu *IrrCollada::CreateIrrColladaMenu( bool bRegisterMenu )
{
	IMenu *pIrrColladaMenu = UIHelpers::AddMenu( GetString(IDS_IC_SUBMENU_TITLE), bRegisterMenu );

	{
		UIHelpers::AddActionMenuItem( pIrrColladaMenu, GetActionItem( IrrColladaActions::pm_displayObjectProperties ) );
		UIHelpers::AddActionMenuItem( pIrrColladaMenu, GetActionItem( IrrColladaActions::pm_runAnimationSelector ) );
		UIHelpers::AddActionMenuItem( pIrrColladaMenu, GetActionItem( IrrColladaActions::pm_runUserPropertiesEditor ) );
	}

	UIHelpers::AddSeparator( pIrrColladaMenu );

	// preview sub menu
	{
		IMenu *pPreviewMenu = UIHelpers::AddMenu( GetString( IDS_IC_PREVIEW_SUBMENU_TITLE ), bRegisterMenu );
		UIHelpers::AddSubMenu( pIrrColladaMenu, pPreviewMenu );
		UIHelpers::AddActionMenuItem( pPreviewMenu, GetActionItem( IrrColladaActions::pm_previewColladaEditor ) );
		UIHelpers::AddActionMenuItem( pPreviewMenu, GetActionItem( IrrColladaActions::pm_previewColladaEditorQuick ) );
		UIHelpers::AddActionMenuItem( pPreviewMenu, GetActionItem( IrrColladaActions::pm_previewTextEditor ) );
		UIHelpers::AddActionMenuItem( pPreviewMenu, GetActionItem( IrrColladaActions::pm_previewTextEditorQuick ) );
	}

	UIHelpers::AddSeparator( pIrrColladaMenu );

	// preview sub menu
	{
		UIHelpers::AddActionMenuItem( pIrrColladaMenu, GetActionItem( IrrColladaActions::pm_displayPreferences ) );
	}


	return pIrrColladaMenu;
}



void IrrCollada::AddIrrColladaMenuItemToQuadMenu( IQuadMenuContext::RightClickContext context, int menuId, IMenu *pMenu )//, IMenuItem *pMenuItem )
{
	IMenuManager *pMenuManager = GetCOREInterface()->GetMenuManager();
	IQuadMenuContext *pQuadMenuContext = ( IQuadMenuContext * )pMenuManager->GetContext( kViewportQuad );
	IQuadMenu *pQuadMenu = pQuadMenuContext->GetRightClickMenu( context );

	IMenu *pTransformMenu = pQuadMenu->GetMenu( menuId );
	IMenuItem *pIrrColladaMenuItem = UIHelpers::CreateSubMenuItem( pMenu );
	pTransformMenu->AddItem( pIrrColladaMenuItem );
}

DWORD IrrCollada::Start()
{
	IMenuManager *pMenuManager = GetCOREInterface()->GetMenuManager();
	if ( pMenuManager )
	{
		IMenu *pIrrColladaMenu = NULL;
		{
			IMenuBarContext *pMenuContext = ( IMenuBarContext * )pMenuManager->GetContext( kMainMenuBar );
			IMenu *pMainMenu = pMenuContext->GetMenu();

			//// not needed right now, may need it at some point
			//{
			//	pIrrColladaMenu = pMenuManager->FindMenu( GetString(IDS_IC_SUBMENU_TITLE) );
			//	while ( pIrrColladaMenu )
			//	{
			//		UIHelpers::RemoveSubMenu( pMainMenu, pIrrColladaMenu );
			//		pIrrColladaMenu = pMenuManager->FindMenu( GetString(IDS_IC_SUBMENU_TITLE) );
			//	}
			//	pMenuManager->UpdateMenuBar();
			//}

			pIrrColladaMenu = CreateIrrColladaMenu();
			int numItems = pMainMenu->NumItems();
			UIHelpers::AddSubMenu( pMainMenu, pIrrColladaMenu, numItems - 1 );
		}

		// quad menu
		{
			// transform menu is at index 0
			AddIrrColladaMenuItemToQuadMenu( IQuadMenuContext::kNonePressed, 0, pIrrColladaMenu );
			AddIrrColladaMenuItemToQuadMenu( IQuadMenuContext::kControlPressed, 0, pIrrColladaMenu );
		}

		pMenuManager->UpdateMenuBar();
		//pMenuContext->UpdateWindowsMenu(); // should not have to call this, however some samples called it
	}

	return GUPRESULT_KEEP; 
}

void IrrCollada::Stop()
{
}

void IrrCollada::Reset()
{
	UnRegisterNotification( &IrrCollada::NotificationCallback, NULL );
	RegisterNotification( &IrrCollada::NotificationCallback, NULL, NOTIFY_SELECTIONSET_CHANGED );
	RegisterNotification( &IrrCollada::NotificationCallback, NULL, NOTIFY_SYSTEM_SHUTDOWN );
	RegisterNotification( &IrrCollada::NotificationCallback, NULL, NOTIFY_POST_NODES_CLONED );

	m_selectedNodes.clear();

	CLEAR_POINTER_VECTOR( m_ICManagers );
	m_ICManagers.push_back( new ICObjectPropertyManager( this ) );
	m_ICManagers.push_back( new ICPreferencesManager( this ) );

	ZeroMemory( &m_colladaEditorProcessInfo, sizeof( m_colladaEditorProcessInfo ) );
	ZeroMemory( &m_textEditorProcessInfo, sizeof( m_textEditorProcessInfo ) );

//#define TERMINATE_PROCESS_MSG "IrrColladaTerminateProcess"
//	UINT msgId = RegisterWindowMessage( TERMINATE_PROCESS_MSG );
}

void IrrCollada::RefreshMenu()
{
	// disabling menu items is done through action tables
	// one of the methods is to subclass ActionTable and override 
	// IsEnabled( int cmdId ) method
}