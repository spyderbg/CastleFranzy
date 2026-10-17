/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "stdafx.h"
#include "MaxScriptDeclarations.h"

#include "ColladaExporter.h"
#include "ICBatchExporter.h"
#include "ColladaOptions.h"
#include "ICAnimationSelector.h"

#include "maxscrpt/maxobj.h"
#include "maxscrpt/Numbers.h"
#include "maxscrpt/Arrays.h"
#include "maxscrpt/strings.h"
#include "maxscrpt/parser.h" 
#include "notetrck.h"
#include "Tab.h"
#include "IPathConfigMgr.h"
#include "tvnode.h"

#include <sstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>

// for mem tests
#include "ICAppDataChunkMemManager.h"

extern FUStringBuilder g_exportLog;
extern ICAnimationSelector g_animationSelector;

#define MAX_SCRIPT_FUNC_IMPL( name ) Value* name##_cf(Value **arg_list, int count)

#define MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( id )( (id) - 1 )
#define CPP_ARRAY_IDX_TO_MAX_ARRAY_IDX( id )( (id) + 1 )

// batch exporter
////////////////////////////////////////////////////////////////////////////////
MAX_SCRIPT_FUNC_IMPL( CM_ShowListenerWindow )
{ 
	show_listener();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_IsCloseMaxWhenBatchExportFinished )
{ 
	one_typed_value_local( Integer *anInt );

	bool_BatchOption *pBoolOption = GET_BATCH_OPTION( "CloseMaxWhenDone", bool );
	vl.anInt = new Integer( pBoolOption->m_value ? 1 : 0 );

	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_IsFileExportSkipped )
{ 
	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( BatchExporter::GetInst()->m_bSkippedFileExport ? 1 : 0 );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_GetNumCustomScripts )
{ 
	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( BatchExporter::GetInst()->m_customScripts.size() );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_GetCustomScriptFilePath )
{ 
	check_arg_count( CM_GetCustomScriptFilePath, 1, count );
	int customScriptId = arg_list[0]->to_int();

	if ( customScriptId < 0 || customScriptId > BatchExporter::GetInst()->m_customScripts.size() )
	{
		return &undefined;
	}

	one_typed_value_local( String *aStr );
	vl.aStr = NEW_MAX_STRING( BatchExporter::GetInst()->m_customScripts[customScriptId].c_str() );
	return_value( vl.aStr );
}

MAX_SCRIPT_FUNC_IMPL( CM_ClearExportLog )
{ 
	ClearExportLog();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_Log )
{ 
	check_arg_count( CM_Log, 1, count );
	const char *log = arg_list[0]->to_string();
	g_exportLog.append( log );
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_PrintExportLog )
{ 
	PrintExportLog();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_InitBatchExporter )
{ 
	BatchExporter::GetInst()->Reset();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_GetBatchExporterArgs )
{ 
	std::stringstream stream;
	char *exportSrc = __argv[4];
	if ( *exportSrc == '_' )
	{
		++exportSrc;
	}

	std::string optsString;
	for ( int i = 5; i < __argc; ++i )
	{
		optsString = __argv[i];
		if ( !_stricmp( optsString.c_str(), "/C" ) )
		{
			bool_BatchOption *pBoolOption = GET_BATCH_OPTION( "CloseMaxWhenDone", bool );
			pBoolOption->m_value = true;

			mprintf( "\nWarning!  Using deprecated command line option '/C'.  Please use 'CloseMaxWhenDone' option in [BATCH_OPTIONS] section of batch exporter .cfg file.\n" );
		}
	}

	int batchExportMode = -1;
	batchExportMode = BatchExporter::BE_FILE_LIST;

	int fd;

	// SRC
	if ( batchExportMode == BatchExporter::BE_FILE || batchExportMode == BatchExporter::BE_FILE_LIST )
	{
		fd = _open ( exportSrc, _O_RDONLY );
		if ( fd < 0 )
		{
			stream << "Export cfg '" << exportSrc << "' does not exist.";
			MessageBox(NULL, _T(stream.str().c_str()), _T("Collada Batch Exporter Arg Error"), MB_OK);
			return &false_value;
		}
		_close( fd );
	}

	BatchExporter::GetInst()->m_exportMode = batchExportMode;
	BatchExporter::GetInst()->m_exportSrc = std::string( exportSrc );
	
	return &true_value;

	// some code I might need later on

	//if ( __argc < 7 )
	//{
	//	return &false_value;
	//}

		//char *exportMode = __argv[4];
	//char *exportSrc = __argv[5];
	//char *exportOpts = __argv[6];
	//int batchExportMode = -1;

	// MODE
	//if ( !_stricmp( exportMode, "-f" ) )
	//{
	//	batchExportMode = BatchExporter::BE_FILE;
	//}
	//else if ( !_stricmp( exportMode, "-d" ) )
	//{
	//	batchExportMode = BatchExporter::BE_DIRECTORY;
	//}
	//else if ( !_stricmp( exportMode, "-l" ) )
	//{
	//	batchExportMode = BatchExporter::BE_FILE_LIST;
	//}
	//else
	//{
	//	stream << "Invalid export mode '" << exportMode << "'.  MODE must be either '-f', '-d', '-l' ( single file, files in directory or file list ).";
	//	MessageBox(NULL, _T(stream.str().c_str()), _T("Collada Batch Exporter Arg Error"), MB_OK);
	//	return &false_value;
	//}

	// OPTIONS
	//fd = _open ( exportOpts, _O_RDONLY );
	//if ( fd < 0 )
	//{
	//	stream << "Export options file '" << exportOpts << "' does not exist.";
	//	MessageBox(NULL, _T(stream.str().c_str()), _T("Collada Batch Exporter Arg Error"), MB_OK);
	//	return &false_value;
	//}
	//_close( fd );
	//BatchExporter::GetInst()->m_exportOpts = std::string( exportOpts );
}

MAX_SCRIPT_FUNC_IMPL( CM_ParseFileList )
{ 
	BatchExporter::GetInst()->m_pDefaultColladaOptions->Reset();
	BatchExporter::GetInst()->m_fileOptionsMap.clear();

	BatchExporter::GetInst()->ParseCfg();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_GetClassName )
{ 
	one_typed_value_local( String *aStr );
	vl.aStr = NEW_MAX_STRING( GetCOLLADAExporterDesc()->ClassName() );
	return_value( vl.aStr );
}

MAX_SCRIPT_FUNC_IMPL( CM_GetFilesToExport )
{ 
	one_typed_value_local( Array *anArray );
	vl.anArray = new Array( BatchExporter::GetInst()->m_fileOptionsMap.size() );
	for ( int i = 0; i < BatchExporter::GetInst()->m_fileOptionsMap.size(); i++ )
	{
		vl.anArray->append( NEW_MAX_STRING( BatchExporter::GetInst()->m_fileOptionsMap[i].m_srcFilePath.c_str() ));
		vl.anArray->append( NEW_MAX_STRING( BatchExporter::GetInst()->m_fileOptionsMap[i].m_dstFilePath.c_str() ));
	}
	return_value( vl.anArray );
}

MAX_SCRIPT_FUNC_IMPL( CM_PreExportFile )
{ 
	check_arg_count( CM_PreExport, 1, count );
	int fileId = arg_list[0]->to_int();
	BatchExporter::GetInst()->m_currentFileId = fileId;
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_GetBatchExporterMode )
{ 
	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( BatchExporter::GetInst()->m_exportMode );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_GetBatchExporterSrc )
{ 
	one_typed_value_local( String *aStr );
	vl.aStr = NEW_MAX_STRING( BatchExporter::GetInst()->m_exportSrc.c_str() );
	return_value( vl.aStr );
}

MAX_SCRIPT_FUNC_IMPL( CM_ExportFile )
{ 
	check_arg_count( CM_ExportFile, 1, count );
	const char *filePath = arg_list[0]->to_string();
	
	COLLADAExporter *pExporter = ( COLLADAExporter * ) GetCOLLADAExporterDesc()->Create();
	pExporter->DoExport( filePath, NULL, GetCOREInterface(), TRUE );
	bool bExportSuccess = pExporter->GetExportSuccess();
	SAFE_DELETE( pExporter );	

	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( bExportSuccess ? 1 : 0 );
	return_value( vl.anInt );
}

// animation selector
////////////////////////////////////////////////////////////////////////////////
#define ASSERT_ARRAY_ACCESS( index, arraySize, fallback ) \
if ( (index) < 0  \
	|| (index) >= (arraySize) ) \
{ \
	fallback; \
}

#define ASSERT_ANIM_CLIP_ID( clipId, fallback ) ASSERT_ARRAY_ACCESS( clipId, ( int )g_animationSelector.GetNumAnimationClips(), fallback )
#define ASSERT_EVENT_GROUP_ID( eventGroupId, fallback ) ASSERT_ARRAY_ACCESS( eventGroupId, ( int )g_animationSelector.GetNumEventGroups(), fallback )
#define ASSERT_EVENT_ID( eventId, fallback ) ASSERT_ARRAY_ACCESS( eventId, ( int )eventGroup.GetNumEvents(), fallback )

MAX_SCRIPT_FUNC_IMPL( CM_GetNumAnimationClips )
{
	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( g_animationSelector.GetNumAnimationClips() );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_GetAnimationClipName )
{ 
	check_arg_count( CM_GetAnimationClipName, 1, count );

	int curArg = 0;
	int clipId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );

	ASSERT_ANIM_CLIP_ID( clipId, return &undefined );
	
	one_typed_value_local( String *aStr );
	vl.aStr = NEW_MAX_STRING( g_animationSelector.GetAnimationClipName( clipId ).c_str() );
	return_value( vl.aStr );
}

MAX_SCRIPT_FUNC_IMPL( CM_GetAnimationClipStart )
{ 
	check_arg_count( CM_GetAnimationClipStart, 1, count );

	int curArg = 0;
	int clipId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );

	ASSERT_ANIM_CLIP_ID( clipId, return &undefined );

	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( g_animationSelector.GetAnimationClipStart( clipId ) );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_GetAnimationClipEnd )
{ 
	check_arg_count( CM_GetAnimationClipEnd, 1, count );

	int curArg = 0;
	int clipId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );

	ASSERT_ANIM_CLIP_ID( clipId, return &undefined );

	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( g_animationSelector.GetAnimationClipEnd( clipId ) );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_GetAnimationClipIndexByName )
{ 
	check_arg_count( CM_GetAnimationClipIndexByName, 1, count );

	int curArg = 0;
	const char *pClipName = arg_list[curArg++]->to_string();

	if ( !pClipName )
	{
		return &undefined;
	}

	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( CPP_ARRAY_IDX_TO_MAX_ARRAY_IDX( g_animationSelector.GetAnimationClipIndexByName( pClipName ) ));
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_AddAnimationClip )
{
	check_arg_count( CM_AddAnimationClip, 3, count );
	
	int curArg = 0;
	const char *pName = arg_list[curArg++]->to_string();
	int startTime = arg_list[curArg++]->to_int();
	int endTime = arg_list[curArg++]->to_int();

	if ( !pName )
	{
		return &undefined;
	}

	g_animationSelector.AddAnimationClip( pName, startTime, endTime );
	SetSaveRequiredFlag();

	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( CPP_ARRAY_IDX_TO_MAX_ARRAY_IDX( g_animationSelector.GetNumAnimationClips() - 1 ));
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_DeleteAnimationClip )
{ 
	check_arg_count( CM_DeleteAnimationClip, 1, count );

	int curArg = 0;
	int clipId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );

	ASSERT_ANIM_CLIP_ID( clipId, return &undefined );

	g_animationSelector.DeleteAnimationClip( clipId );
	SetSaveRequiredFlag();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_DeleteAnimationClipByName )
{ 
	check_arg_count( CM_DeleteAnimationClipByName, 1, count );

	int curArg = 0;
	const char *pClipName = arg_list[curArg++]->to_string();

	if ( !pClipName )
	{
		return &undefined;
	}

	int index = g_animationSelector.GetAnimationClipIndexByName( pClipName );
	if ( index < 0 )
	{
		return &undefined;
	}

	g_animationSelector.DeleteAnimationClip( index );
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_SetAnimationClipName )
{ 
	check_arg_count( CM_SetAnimationClipName, 2, count );

	int curArg = 0;
	int clipId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );
	const char *pClipName = arg_list[curArg++]->to_string();

	if ( !pClipName )
	{
		return &undefined;
	}

	ASSERT_ANIM_CLIP_ID( clipId, return &undefined );

	g_animationSelector.SetAnimationClipName( clipId, pClipName );
	SetSaveRequiredFlag();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_SetAnimationClipStart )
{ 
	check_arg_count( CM_SetAnimationClipStart, 2, count );

	int curArg = 0;
	int clipId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );
	int startTime = arg_list[curArg++]->to_int();

	ASSERT_ANIM_CLIP_ID( clipId, return &undefined );

	g_animationSelector.SetAnimationClipStart( clipId, startTime );
	SetSaveRequiredFlag();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_SetAnimationClipEnd )
{ 
	check_arg_count( CM_SetAnimationClipEnd, 2, count );

	int curArg = 0;
	int clipId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );
	int endTime = arg_list[curArg++]->to_int();

	ASSERT_ANIM_CLIP_ID( clipId, return &undefined );

	g_animationSelector.SetAnimationClipEnd( clipId, endTime );
	SetSaveRequiredFlag();
	return &ok;
}

// animation clip event groups
MAX_SCRIPT_FUNC_IMPL( CM_GetNumEventGroups )
{
	check_arg_count( CM_GetNumEventGroups, 0, count );
	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( g_animationSelector.GetNumEventGroups() );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_AddEventGroup )
{
	check_arg_count( CM_AddEventGroup, 1, count );
	
	int curArg = 0;
	int eventGroupTime = arg_list[curArg++]->to_int();

	FCDEventGroup<int> eventGroup( eventGroupTime );
	g_animationSelector.AddEventGroup( eventGroup );

	SetSaveRequiredFlag();

	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( CPP_ARRAY_IDX_TO_MAX_ARRAY_IDX( g_animationSelector.GetNumEventGroups() - 1 ) );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_DeleteEventGroup )
{
	check_arg_count( CM_DeleteEventGroup, 1, count );
	
	int curArg = 0;
	int eventGroupId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );

	ASSERT_EVENT_GROUP_ID( eventGroupId, return &undefined );
	g_animationSelector.DeleteEventGroup( eventGroupId );

	SetSaveRequiredFlag();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_GetEventGroupTime )
{
	check_arg_count( CM_GetEventGroupTime, 1, count );
	
	int curArg = 0;
	int eventGroupId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );

	ASSERT_EVENT_GROUP_ID( eventGroupId, return &undefined );
	const FCDEventGroup<int> &eventGroup = g_animationSelector.GetEventGroup( eventGroupId );

	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( eventGroup.GetTime() );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_SetEventGroupTime )
{
	check_arg_count( CM_SetEventGroupTime, 2, count );
	
	int curArg = 0;
	int eventGroupId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );
	int eventGroupTime = arg_list[curArg++]->to_int();

	ASSERT_EVENT_GROUP_ID( eventGroupId, return &undefined );
	FCDEventGroup<int> &eventGroup = g_animationSelector.GetEventGroup( eventGroupId );
	eventGroup.SetTime( eventGroupTime );

	SetSaveRequiredFlag();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_GetNumEventGroupEvents )
{
	check_arg_count( CM_GetNumEventGroupEvents, 1, count );
	
	int curArg = 0;
	int eventGroupId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );

	ASSERT_EVENT_GROUP_ID( eventGroupId, return &undefined );
	const FCDEventGroup<int> &eventGroup = g_animationSelector.GetEventGroup( eventGroupId );

	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( eventGroup.GetNumEvents() );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_AddEventGroupEvent )
{
	check_arg_count( CM_AddEventGroupEvent, 2, count );
	
	int curArg = 0;
	int eventGroupId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );
	const char *pEvent = arg_list[curArg++]->to_string();

	ASSERT_EVENT_GROUP_ID( eventGroupId, return &undefined );
	FCDEventGroup<int> &eventGroup = g_animationSelector.GetEventGroup( eventGroupId );
	eventGroup.AddEvent( pEvent );

	SetSaveRequiredFlag();

	one_typed_value_local( Integer *anInt );
	vl.anInt = new Integer( CPP_ARRAY_IDX_TO_MAX_ARRAY_IDX( eventGroup.GetNumEvents() - 1 ) );
	return_value( vl.anInt );
}

MAX_SCRIPT_FUNC_IMPL( CM_DeleteEventGroupEvent )
{
	check_arg_count( CM_DeleteEventGroupEvent, 2, count );
	
	int curArg = 0;
	int eventGroupId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );
	int eventId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );

	ASSERT_EVENT_GROUP_ID( eventGroupId, return &undefined );
	FCDEventGroup<int> &eventGroup = g_animationSelector.GetEventGroup( eventGroupId );

	ASSERT_EVENT_ID( eventId, return &undefined );
	eventGroup.DeleteEvent( ( size_t )eventId );

	SetSaveRequiredFlag();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_DeleteAllEventGroupEvents )
{
	check_arg_count( CM_DeleteAllEventGroupEvents, 1, count );
	
	int curArg = 0;
	int eventGroupId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );

	ASSERT_EVENT_GROUP_ID( eventGroupId, return &undefined );
	FCDEventGroup<int> &eventGroup = g_animationSelector.GetEventGroup( eventGroupId );

	eventGroup.DeleteAllEvents();

	SetSaveRequiredFlag();
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_GetEventGroupEvent )
{
	check_arg_count( CM_GetEventGroupEvent, 2, count );
	
	int curArg = 0;
	int eventGroupId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );
	int eventId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );

	ASSERT_EVENT_GROUP_ID( eventGroupId, return &undefined );
	const FCDEventGroup<int> &eventGroup = g_animationSelector.GetEventGroup( eventGroupId );

	ASSERT_EVENT_ID( eventId, return &undefined );
	const fstring &event = eventGroup.GetEvent( ( size_t )eventId );

	one_typed_value_local( String *aStr );
	vl.aStr = NEW_MAX_STRING( event.c_str() );
	return_value( vl.aStr );
}

MAX_SCRIPT_FUNC_IMPL( CM_SetEventGroupEvent )
{
	check_arg_count( CM_SetEventGroupEvent, 3, count );
	
	int curArg = 0;
	int eventGroupId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );
	int eventId = MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX( arg_list[curArg++]->to_int() );
	const char *pEvent = arg_list[curArg++]->to_string();

	ASSERT_EVENT_GROUP_ID( eventGroupId, return &undefined );
	FCDEventGroup<int> &eventGroup = g_animationSelector.GetEventGroup( eventGroupId );

	ASSERT_EVENT_ID( eventId, return &undefined );
	eventGroup.SetEvent( ( size_t )eventId, pEvent );

	SetSaveRequiredFlag();
	return &ok;
}

// debug
////////////////////////////////////////////////////////////////////////////////
MAX_SCRIPT_FUNC_IMPL( CM_MemoryTestNote )
{
	INode *pRoot = GetCOREInterface()->GetRootNode();
	NoteTrack *pNoteTrack = NewDefaultNoteTrack();
	pRoot->AddNoteTrack( pNoteTrack );
	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_MemoryTestAddChunk )
{
	INode *pRoot = GetCOREInterface()->GetRootNode();

	size_t size = sizeof( int );
	int *testData = ( int * )MAX_MALLOC( size );
	*testData = 33333;

	pRoot->AddAppDataChunk( COLLADAEXPORTER_CLASS_ID, SCENE_EXPORT_CLASS_ID, 0, size, testData );

	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_MemoryTestGetChunk )
{
	INode *pRoot = GetCOREInterface()->GetRootNode();
	size_t size = sizeof( int );

	AppDataChunk *pChunk = pRoot->GetAppDataChunk( COLLADAEXPORTER_CLASS_ID, SCENE_EXPORT_CLASS_ID, 0 );
	if ( pChunk )
	{
		int testData;
		int chunkLength = pChunk->length;
		if ( chunkLength == size )
		{
			memcpy( &testData, pChunk->data, chunkLength );
		}
	}

	return &ok;
}

MAX_SCRIPT_FUNC_IMPL( CM_MemoryTestGetTrackViewRoot )
{
	ITrackViewNode *pNode = GetCOREInterface()->GetTrackViewRootNode();
	NoteTrack *pNoteTrack = pNode->GetNoteTrack( 0 );
	return &ok;
}

#undef ASSERT_EVENT_ID
#undef ASSERT_EVENT_GROUP_ID
#undef ASSERT_ANIM_CLIP_ID
#undef ASSERT_ARRAY_ACCESS
#undef MAX_ARRAY_IDX_TO_CPP_ARRAY_IDX
#undef CPP_ARRAY_IDX_TO_MAX_ARRAY_IDX
#undef MAX_SCRIPT_FUNC_IMPL