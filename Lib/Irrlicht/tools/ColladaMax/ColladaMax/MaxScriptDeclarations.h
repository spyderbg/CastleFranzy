#ifndef _MAX_SCRIPT_DECLARATIONS_H_
#define _MAX_SCRIPT_DECLARATIONS_H_

// Note : This file needs to only be included once ( otherwise link errors ==> duplicate definitions )

#include "maxscrpt/MAXScrpt.h"
#include "maxscrpt/definsfn.h"

#define MAX_SCRIPT_FUNC_DECL( name ) def_visible_primitive( name, #name )

MAX_SCRIPT_FUNC_DECL( CM_ShowListenerWindow );
MAX_SCRIPT_FUNC_DECL( CM_IsCloseMaxWhenBatchExportFinished );
MAX_SCRIPT_FUNC_DECL( CM_IsFileExportSkipped );
MAX_SCRIPT_FUNC_DECL( CM_GetNumCustomScripts );
MAX_SCRIPT_FUNC_DECL( CM_GetCustomScriptFilePath );
MAX_SCRIPT_FUNC_DECL( CM_ClearExportLog );
MAX_SCRIPT_FUNC_DECL( CM_Log );
MAX_SCRIPT_FUNC_DECL( CM_PrintExportLog );
MAX_SCRIPT_FUNC_DECL( CM_InitBatchExporter );
MAX_SCRIPT_FUNC_DECL( CM_GetBatchExporterArgs );
MAX_SCRIPT_FUNC_DECL( CM_ParseFileList );
MAX_SCRIPT_FUNC_DECL( CM_GetClassName );
MAX_SCRIPT_FUNC_DECL( CM_GetFilesToExport );
MAX_SCRIPT_FUNC_DECL( CM_PreExportFile );
MAX_SCRIPT_FUNC_DECL( CM_GetBatchExporterMode );
MAX_SCRIPT_FUNC_DECL( CM_GetBatchExporterSrc );
MAX_SCRIPT_FUNC_DECL( CM_ExportFile );

// animation selector
MAX_SCRIPT_FUNC_DECL( CM_GetNumAnimationClips );
MAX_SCRIPT_FUNC_DECL( CM_GetAnimationClipName );
MAX_SCRIPT_FUNC_DECL( CM_GetAnimationClipStart );
MAX_SCRIPT_FUNC_DECL( CM_GetAnimationClipEnd );
MAX_SCRIPT_FUNC_DECL( CM_GetAnimationClipIndexByName );
MAX_SCRIPT_FUNC_DECL( CM_AddAnimationClip );
MAX_SCRIPT_FUNC_DECL( CM_DeleteAnimationClip );
MAX_SCRIPT_FUNC_DECL( CM_DeleteAnimationClipByName );
MAX_SCRIPT_FUNC_DECL( CM_SetAnimationClipName );
MAX_SCRIPT_FUNC_DECL( CM_SetAnimationClipStart );
MAX_SCRIPT_FUNC_DECL( CM_SetAnimationClipEnd );

// animation clip event groups
MAX_SCRIPT_FUNC_DECL( CM_GetNumEventGroups );
MAX_SCRIPT_FUNC_DECL( CM_AddEventGroup );
MAX_SCRIPT_FUNC_DECL( CM_DeleteEventGroup );
MAX_SCRIPT_FUNC_DECL( CM_GetEventGroupTime );
MAX_SCRIPT_FUNC_DECL( CM_SetEventGroupTime );
MAX_SCRIPT_FUNC_DECL( CM_GetNumEventGroupEvents );
MAX_SCRIPT_FUNC_DECL( CM_AddEventGroupEvent );
MAX_SCRIPT_FUNC_DECL( CM_DeleteEventGroupEvent );
MAX_SCRIPT_FUNC_DECL( CM_DeleteAllEventGroupEvents );
MAX_SCRIPT_FUNC_DECL( CM_GetEventGroupEvent );
MAX_SCRIPT_FUNC_DECL( CM_SetEventGroupEvent );

// debug
MAX_SCRIPT_FUNC_DECL( CM_MemoryTestNote );
MAX_SCRIPT_FUNC_DECL( CM_MemoryTestAddChunk );
MAX_SCRIPT_FUNC_DECL( CM_MemoryTestGetChunk );
MAX_SCRIPT_FUNC_DECL( CM_MemoryTestGetTrackViewRoot );

#endif // _MAX_SCRIPT_DECLARATIONS_H_

