global gScriptRootDir 		= getDir #scripts + "\\dae_exporter\\"
global gScriptIniFile 		= "dae_export.ini"
global gSrcDir 						= undefined
global gExportDir 				= undefined
global gFilePrefixes			= #()
global gExportLog					= stringStream ""

/*
 * function: getSettings
 * get settings from the ini file
 */
fn getSettings =
(
	-- read the *.ini file
	local file = gScriptRootDir+gScriptIniFile
	local key = (getIniSetting file "PREFERENCES")

	-- reads the key/value pairs
	for i in key do (
		local v = getIniSetting file "PREFERENCES" i
		case i of (
			"SRC_DIR":
				gSrcDir = (v as string)
			"EXPORT_DIR":
				gExportDir = (v as string)
		)
	)
)

fn toLowerCase str =
(
	upper="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	lower="abcdefghijklmnopqrstuvwxyz"

	for i = 1 to str.count do
	(
		notFound = true
		for j = 1 to upper.count while notFound do
		(
			if (str[i] == upper[j]) then
			(
				str[i] = lower[j]
				notFound = false
			)
		)
	)
	
	return str
)

function getSubDirectoriesRecurs dir =
(
	/* get the child directories*/
	childDirectories = getDirectories (dir+"*")
	
	/* get the descendant subdirectories*/
	descendantDirectories = #()
	for i = 1 to childDirectories.count do
	(
		append descendantDirectories childDirectories[i]
		tmpDescendantDirectories = (getSubDirectoriesRecurs (childDirectories[i]))
		if tmpDescendantDirectories != #() then 
		(
			join descendantDirectories tmpDescendantDirectories;		
		)
	)
	
	return descendantDirectories
)

/*
 * function: getFilesInDirectory
 *  get *.max files from directory
 */
-- TODO
function getFilesInDirectory dir = 
(	
	-- get all descendant directories + dir
	local subDirectories = #(dir)
	join subDirectories (getSubDirectoriesRecurs (dir))
		
	maxFiles = #()
	for i = 1 to subDirectories.count do
	(
		-- GET FILES
		files = (getFiles (subDirectories[i] + "*.max"))
		join maxFiles files
		
		-- DETERMINE EXPORT FILE PREFIX
		filePrefix = ""
		if (i > 1) then
		(
			splitPath = filterString subDirectories[i] "\\"
			
			-- REPLACE SPACES WITH UNDERSCORES
			tmp = filterString (splitPath[splitPath.count]) " "
			for j = 1 to tmp.count do
			(
				filePrefix = filePrefix + tmp[j] + "_"
			)	
			
			-- TO LOWER CASE
			filePrefix = toLowerCase filePrefix
		)
		
		for j = 1 to files.count do
		(
			append gFilePrefixes filePrefix
		)	
	)

	return maxFiles
)

function GetColladaExporterClassIdx =
(
	colladaExporterClassName = CM_GetClassName()
	colladaExporterClassIdx = -1;
	for i = 1 to exporterPlugin.classes.count do
	(
		if ( exporterPlugin.classes[i] as string ) == colladaExporterClassName then
		(
			colladaExporterClassIdx = i
			break
		)
	)
	
	return colladaExporterClassIdx
)

function ExecuteCustomScripts = 
(
	numCustomScript = CM_GetNumCustomScripts()
	if numCustomScript < 1 then
	(
		return -1
	)
	
	for i = 1 to numCustomScript by 1 do
	(
		customScript = CM_GetCustomScriptFilePath( i - 1 )
		fileIn customScript
	)
)

fn FlushExportLog =
(
	CM_Log( gExportLog as string )
	gExportLog = StringStream ""
)

fn batchExport =
(
	exportMode = CM_GetBatchExporterMode()
	exportSrc = CM_GetBatchExporterSrc()
	
	maxFiles = #()
	
	-- TODO: Expose export mode enums to maxscript
	if exportMode == 0 then
	(
		messageBox "Unsupported export mode"
		return -1
-- 		append maxFiles #( exportSrc, #() )
	)
	else
	if exportMode == 1 then
	(
		messageBox "Unsupported export mode"
		return -1	
-- 		maxFiles = getFilesInDirectory exportSrc
	)
	else
	if exportMode == 2 then
	(
		CM_ParseFileList()
 		maxFiles = CM_GetFilesToExport()
	)
	
	colladaExporterClassIdx = GetColladaExporterClassIdx()
	if colladaExporterClassIdx == -1 then
	(
		messagebox "Error getting ColladaMax exporter class id"
		return -1
	)

	-- doexport
	gExportLog = StringStream ""
	
	format "------------------------------\n" to:gExportLog
	format "- Begin Collada batch export -\n" to:gExportLog
	format "------------------------------\n" to:gExportLog
	format "\n" to:gExportLog
	
	FlushExportLog()
	
	for i = 1 to maxFiles.count by 2 do
	(
		srcFile = maxFiles[i]
		dstFile = maxFiles[i+1]
	
		format "Exporting % ...\n" srcFile to:gExportLog
		FlushExportLog()
		
		local loadOk = undefined
		local loadMaxFileSuccess = false
		while loadMaxFileSuccess == false do
		(
			try	
			(
				loadOk = loadMaxFile srcFile useFileUnits:true quiet:true
				loadMaxFileSuccess = true
			)
			catch ()
		)	
		
		if loadOk then
		(				
			ExecuteCustomScripts()
			FlushExportLog()
			
			CM_PreExportFile ( ( i - 1 ) / 2 )
			exportResult = exportFile dstFile #noprompt using:exporterPlugin.classes[colladaExporterClassIdx]
			
			if exportResult == false then
			(
				format "-- ERROR exporting to %\n" dstFile to:gExportLog
			)
			else
			if ( CM_IsFileExportSkipped != undefined and CM_IsFileExportSkipped() != 0 ) then
			(
				format "-- SKIPPED ==> %\n" dstFile to:gExportLog
			)
			else
			(
				format "-- SUCCESS ==> %\n" dstFile to:gExportLog
			)
		)
		else
		(
			format "-- ERROR loading %\n" srcFile to:gExportLog
		)
		
		FlushExportLog()
		
		-- Sometimes max throws an exception on resetMAXFile
		-- and does not properly reset the scene
		-- This loop indefinity attempts to reset max until
		-- successful
		hasResetFile = false
		while hasResetFile == false do
		(
			try	
			(
				resetMaxFile( #noPrompt )
				hasResetFile = true
			)
			catch ()
		)	
	)	
	
	format "\n" to:gExportLog
	format "----------------------------\n" to:gExportLog
	format "- End Collada batch export -\n" to:gExportLog
	format "----------------------------\n" to:gExportLog
	
	FlushExportLog()
	
	CM_PrintExportLog()
)

function OnEnd =
(
	if ( CM_IsCloseMaxWhenBatchExportFinished != undefined and CM_IsCloseMaxWhenBatchExportFinished() !=  0 ) then
	(
		quitMAX #noPrompt
	) 
)

function OnFatalError =
(
	OnEnd()
)

-- main
-- if CM_ShowListenerWindow != undefined then
-- ( 
-- 	CM_ShowListenerWindow()
-- )
-- clearListener()
if CM_ClearExportLog != undefined then
( 
	CM_ClearExportLog()
)

doExport = true
if CM_GetBatchExporterArgs == undefined do
(
	messagebox "ColladaMax.dle does not appear to be loaded.  Aborting batch export."
	doExport = false
	OnFatalError()
)

if CM_InitBatchExporter != undefined then
(
	CM_InitBatchExporter()
)

if CM_GetBatchExporterArgs() == false do
(
	messagebox "batchexporter.ms usage: 3dsmax.exe -U MAXScript Scripts/batchexporter.ms BATCH_EXPORT_CONFIG_FILEPATH."
-- 	messagebox "batchexporter.ms usage: 3dsmax.exe -U MAXScript Scripts/batchexporter.ms MODE SRC OPTIONS.\n\n\tMODE is either '-f', '-d', '-l' ( single file, files in directory or file list ).\n\tSRC is either a single max file, a directory or the path to a file containing a list of max files to export, depending on MODE.\n\tOPTIONS is a path to a file containing the default export options for your max files."
	doExport = false
	OnFatalError()
)

if doExport then 
(
	batchExport()
	OnEnd()
)