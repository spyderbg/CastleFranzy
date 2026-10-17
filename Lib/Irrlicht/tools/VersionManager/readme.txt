VersionManager Application
--------------------------------------------------------------------------------

USAGE : VersionManager MODE FILEPATH

MODE :

	- /g :	Prints out the version number of the given file in this format : MAJOR.MINOR.BUILD.REVISION
			FILEPATH is absolute or relative
			Prints out -1.-1.-1.-1 if version number could not be queried for any reason.

			Ex.:
	
			/> VersionManager /g foo.exe
			/> 35000.24.300.2000
			
	- /u :	Updates the provided Version Number Definition file.  It increments BUILD by 1.
			If BUILD >= 65535, incrementing it will set BUILD to 0 and increment MINOR by 1.
			MINOR has the same relationship with MAJOR than BUILD has with MINOR.
			if MAJOR >= 65535, incrementing it will set MAJOR to 0.
			
			Ex.:
	
			/> VersionManager /u foo.h
			
			Note that the term Version Number Definition File is not a standard term and was made up to designate 
			a header file containing version number macros arranged in a certain format.  This file is meant to 
			be included in a user defined resource file containing application version information in Visual Studio.
			
			See example/VersionNo.h for an example of a valid Version Number Definition File.
			Please respect the format found in this file to the letter ( the parser is not robust ).
			
			For additional info, see http://support.microsoft.com/kb/q237870/ and http://msdn.microsoft.com/en-us/library/6t3612sk(VS.80).aspx.
			
			