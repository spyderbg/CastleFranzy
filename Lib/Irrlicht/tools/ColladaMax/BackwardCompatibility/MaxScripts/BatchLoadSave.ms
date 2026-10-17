-- Use batchLoadSave dir function to batch load/save .max files in supplied directory and subdirectories
-- Note that the path separator format is "\\"
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
	)

	return maxFiles
)

function batchLoadSave dir =
(	
	maxFiles = getFilesInDirectory dir	
	for i = 1 to maxFiles.count do
	(
		srcFile = maxFiles[i]
		dstFile = srcFile
	
		format "Processing % ...\n" srcFile

		local loadOk = false
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
			format "-- Loaded % ...\n" srcFile
			local saveOk = false
			local saveMaxFileSuccess = false
			while saveMaxFileSuccess == false do
			(
				try	
				(
					saveOk = saveMaxFile dstFile clearNeedSaveFlag:true useNewFile:true quiet:true
					saveMaxFileSuccess = true
				)
				catch ()
			)
			
			if saveOk then
			(
				format "-- Saved %\n" srcFile
			)
			else
			(
				format "-- ERROR saving %\n" srcFile
			)
		)
		else
		(
			format "-- ERROR loading %\n" srcFile
		)
		
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
)