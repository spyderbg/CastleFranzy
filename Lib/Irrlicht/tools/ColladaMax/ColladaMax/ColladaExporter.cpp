/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the 3dsMax COLLADA Tools:
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "ColladaExporter.h"
#include "ICBatchExporter.h"
#include "ControllerExporter.h"
#include "DocumentExporter.h"
#include "FCDocument/FCDocument.h"
#include <locale.h>
#include "ColladaOptions.h"
#include "FArchiveXML.h"
#include "AnimationClipManager.h"
#include "NodeExporter.h"

extern FUStringBuilder g_warningLog ;
extern FUStringBuilder g_exportLog ;

//
// COLLADAExporter
//

COLLADAExporter::COLLADAExporter()
: m_bExportSuccess( false )
, m_exportMode( EM_DEFAULT )
{
}

COLLADAExporter::~COLLADAExporter() 
{
}

// SceneExport methods
int COLLADAExporter::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR* COLLADAExporter::Ext(int)
{		
	// Return the 'i-th' file name extension (i.e. "dae").
	return _T("dae");
}

const TCHAR* COLLADAExporter::LongDesc()
{
	// Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("COLLADA Document");
}
	
const TCHAR* COLLADAExporter::ShortDesc() 
{			
	// Return short ASCII description (i.e. "Targa")
	return _T("IrrCOLLADA");
}

const TCHAR* COLLADAExporter::AuthorName()
{			
	// Return ASCII Author name
	return _T("Gameloft");
}

const TCHAR* COLLADAExporter::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("Copyright 2008 Gameloft. Based on Feeling Software's ColladaMax exporter.");
}

const TCHAR* COLLADAExporter::OtherMessage1() 
{		
	// Return Other message #1 if any
	return _T("");
}

const TCHAR* COLLADAExporter::OtherMessage2() 
{		
	// Return other message #2 in any
	return _T("");
}

unsigned int COLLADAExporter::Version()
{				
	// Return Version number * 100 (e.g. v3.01 = 301)
	uint32 version = (FCOLLADA_VERSION >> 16) * 100;
	version += FCOLLADA_VERSION % 100;
	return version;
}

void COLLADAExporter::ShowAbout(HWND)
{			
	// Optional
}

BOOL COLLADAExporter::SupportsOptions(int, DWORD)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

fm::string COLLADAExporter::GetOptionFileName()
{
	fm::string fileName;
	
	if ( m_exportMode == EM_DEFAULT )
	{
		fileName = OPTION_FILENAME_DEFAULT;
	}
	else if ( m_exportMode == EM_PREVIEW )
	{
		fileName = OPTION_FILENAME_PREVIEW;
	}

	return fileName;
}

fm::string COLLADAExporter::GetExportDialogCaption()
{
	fm::string caption;
	
	if ( m_exportMode == EM_DEFAULT )
	{
		caption = EXPORT_DIALOG_CAPTION_DEFAULT;
	}
	else if ( m_exportMode == EM_PREVIEW )
	{
		caption = EXPORT_DIALOG_CAPTION_PREVIEW;
	}

	return caption;
}

// dummy function for progress bar
DWORD WINAPI fn(LPVOID)
{
	return 0;
}

void COLLADAExporter::ForceLowercaseExtension( std::string &name )
{
	int nameLength = name.size();
	if ( nameLength < 3 )
	{
		return;
	}

	int ext = name.rfind( '.' );
	FUAssert( ext != std::string::npos, return );
	name.replace( ext + 1, 3, "dae" );
}

// main export function
int	COLLADAExporter::DoExport(const TCHAR* name, ExpInterface* UNUSED(ei), Interface* i, BOOL suppressPrompts, DWORD options)
{
	BOOL bRetVal = FALSE;
	i->ProgressStart(_T("COLLADA export..."), TRUE, fn, NULL);

	g_warningLog.clear();

	if ( BatchExporter::GetInst()->GetIrrColladaVersionNumber() == VersionNumber::Invalid() )
	{
		BatchExporter::GetInst()->FetchIrrColladaVersionNumberFromFile();
	}

	bool bIsBatchExporting = BatchExporter::GetInst()->IsBatchExporting();

#if !defined(_DEBUG)
	try
	{
#endif
		bool_BatchOption *pConditionalExportOption = GET_BATCH_OPTION( "ConditionalExport", bool );
		bool bDoConditionalExport = bIsBatchExporting ? pConditionalExportOption->m_value : false;

		FUErrorSimpleHandler handler;
		DocumentExporter document(options,this);

		int loadReturnCode = document.options->LoadOptions( GetOptionFileName() );

		// init options
		if ( bIsBatchExporting )
		{
			*document.options = *BatchExporter::GetInst()->m_fileOptionsMap[BatchExporter::GetInst()->m_currentFileId].m_pOptions;
			document.options->SetDocumentExporter( &document );
			document.options->SetMaxInterface( GetCOREInterface() );
		}

		// hack to make sure first quick preview exports are fine
		if ( m_exportMode == COLLADAExporter::EM_PREVIEW 
			&& suppressPrompts
			&& loadReturnCode == -1 )
		{
			document.options->ApplyColladaConverterCompliance( NULL );
		}

		document.options->SetExportSelected( ( options & SCENE_EXPORT_SELECTED ) == SCENE_EXPORT_SELECTED );

		// scene animation range calculation
		{
			document.GetNodeExporter()->SetWorldNode( GetCOREInterface()->GetRootNode() );
			document.GetNodeExporter()->CalculateExportNodes();
			ExportNodeMap &exportNodes = document.GetNodeExporter()->GetExportNodes();

			AnimationClipManager::GetInst()->CalculateSceneMixerKeyTimes( exportNodes );
			AnimationClipManager::GetInst()->CalculateSceneAnimationKeyTimes( exportNodes );
			AnimationClipManager::GetInst()->GetSceneAnimationClips( exportNodes );
		}

		if (document.ShowExportOptions(suppressPrompts))
		{
			document.PreExport();

			bool bExportFile = bDoConditionalExport ? BatchExporter::GetInst()->ShouldExportFile( BatchExporter::GetInst()->m_currentFileId ) : true;
			if ( bExportFile )
			{
				document.ExportCurrentMaxScene();

				FCDocument *pDocument = document.GetCOLLADADocument();

				FArchiveXMLOptions archiveXMLOptions = document.GetColladaOptions()->ConstructArchiveXMLOptions();
				FArchiveXML::SetOptions( archiveXMLOptions );

				std::string fileName( name );
				ForceLowercaseExtension( fileName );

				FCollada::SaveDocument(pDocument, fileName.c_str() );
				bRetVal = handler.IsSuccessful();
			}
			else
			{
				bRetVal = TRUE;
			}

			SetExportSuccess( true );
		}
		else
		{
			// Set to TRUE although nothing happened in other to avoid the "generic failure" dialog.
			bRetVal = TRUE; 
		}

		document.GetNodeExporter()->PostExportXRefs();

		g_exportLog.append( g_warningLog );
		if ( !bIsBatchExporting )
		{
			document.options->DisplayPostExportMessageBox();
		}

#if !defined(_DEBUG)
	} 
	catch (...)
	{
		// Add some check, here, for full UI-mode or batch-mode only.
		MessageBox(NULL, "Fatal Error: exception caught.", "ColladaMax", MB_OK);
	}
#endif

	BatchExporter::GetInst()->m_currentFileId = -1;
	AnimationClipManager::Release();

	// Restore the locale
	setlocale(LC_NUMERIC, "C");
	i->ProgressEnd();

	return bRetVal;
}

//
// ColladaExporterClassDesc
//

class ColladaExporterClassDesc : public ClassDesc2 
{
public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL isLoading = FALSE) { isLoading; return new COLLADAExporter(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME_E); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return COLLADAEXPORTER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY_E); }

	const TCHAR*	InternalName() { return _T("COLLADAExporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

ClassDesc2* GetCOLLADAExporterDesc()
{
	static ColladaExporterClassDesc description;
	return &description;
}