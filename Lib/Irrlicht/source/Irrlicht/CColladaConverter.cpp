#include "stdafx.h"
#include "CColladaConverter.h"
#include <irrlicht.h>

#ifdef _IRR_WINDOWS_API_
#include <windows.h>
#endif
#if defined(_IRR_WINDOWS_API_) || defined(_IRR_XBOX_PLATFORM_)

#include <tchar.h>
#include <conio.h>
#include <fstream>
#include <stdio.h>
#endif



namespace irr
{
namespace collada
{

#ifdef _IRR_WINDOWS_API_
const char g_pBatchExporterCommand[] = 
"[DEFAULT_OPTIONS] \n\
bakeMatrices=1,decomposeMatrices=1,relativePaths=0,removePaths=1,replacePaths=0,replacementPath=,normals=1,triangulate=1,xrefs=0,tangents=0,deindexVertexData=1,animations=1,sampleAnim=1,createClip=0,removeZeroWeights=1,removeDuplicateWeights=1,exportUpAxis=1,librariesToStrip=0x0,warnOnNegativeScale=0 \n\
[FILES] \n\
src=";

int WriteBatchExporter(const char *pMaxFilename, const char *pCFGFilename)
{
	std::ofstream out;
	out.open(pCFGFilename);
	out << g_pBatchExporterCommand << pMaxFilename << std::endl << std::endl;
	out.close();
	return 0;
}


const char g_pMaxCommand[] = "\"%s3dsmax.exe\" -U MAXScript %s\\tools\\BatchExporter\\batchexporter.ms _%s /c";
const char g_pCFGFilename[] = "%s\\tools\\BatchExporter\\MaxConverter.cfg";
#endif

int ConvertMAX2BDAE(const char *pSourceFile, const char *pTargetFile)
{
#ifndef _IRR_WINDOWS_API_
	return -1;
#else
	STARTUPINFO si;
	
	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.wShowWindow = SW_MINIMIZE;
	si.dwFlags = STARTF_USESHOWWINDOW;

	HKEY hk;
	DWORD dwBufLen = MAX_PATH;

	if (RegOpenKeyExA(HKEY_CURRENT_USER, 
		"Software\\Gameloft\\Irrlicht", 
          0, KEY_READ, &hk))
   {
      printf("Could not open the registry key."); 
      return 1;
   }
	
	char szPath[MAX_PATH];
	if (RegQueryValueEx(hk,             // subkey handle 
          "Path",					// value name 
          NULL, NULL, 
          (LPBYTE) szPath,				// pointer to value data 
          (DWORD*) (&dwBufLen)))			// data size
   {
      printf("Could not get the event message file."); 
      RegCloseKey(hk); 
      return 1;
   }

	RegCloseKey(hk);

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Autodesk\\3dsMax\\9.0\\MAX-1:409\\AdLM", 
          0, KEY_READ, &hk))
   {
      printf("Could not open the registry key."); 
      return 1;
   }
	
	char maxPath[MAX_PATH];
	dwBufLen = MAX_PATH;
	if (RegQueryValueEx(hk,             // subkey handle 
          "InfoPath",					// value name 
          NULL, NULL, 
          (LPBYTE) maxPath,				// pointer to value data 
          (DWORD*) (&dwBufLen)))			// data size
   {
      printf("Could not get the event message file."); 
      RegCloseKey(hk); 
      return -1;
   }

	RegCloseKey(hk);

	char commandLine[MAX_PATH];
	char cfgFilename[MAX_PATH];
	sprintf(cfgFilename, g_pCFGFilename, szPath);
	sprintf(commandLine, g_pMaxCommand, maxPath, szPath, cfgFilename);

	WriteBatchExporter(pSourceFile, cfgFilename);

	std::string maxRoot;
	maxRoot = maxPath;
	maxRoot += "3dsmax.exe";
	if(!CreateProcess(maxRoot.c_str(), commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		printf( "MAX Convertion failed (%d)\n", GetLastError() );
		_IRR_DEBUG_BREAK_IF("Converter may not be in the current directory");
		return -1;
	}

	// Wait until child processes exit.
	WaitForSingleObject( pi.hProcess, INFINITE );

	CloseHandle( pi.hProcess );

	std::string daeFile;
	std::string bdaeFile;
	daeFile = pSourceFile;
	int iFind = daeFile.rfind(".");
	bdaeFile = daeFile = daeFile.substr(0, iFind);
	daeFile += ".dae";
	bdaeFile += ".max.bdae";
	return ConvertDAE2BDAE(daeFile.c_str(), bdaeFile.c_str());
#endif
}

int ConvertDAE2BDAE(const char *pSourceFile, const char *pTargetFile)
{
#ifndef _IRR_WINDOWS_API_
	return -1;
#else
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof(pi) );
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.wShowWindow = SW_MINIMIZE;
	si.dwFlags = STARTF_USESHOWWINDOW;

	char commandLine[1024];
	sprintf(commandLine, "-i \"%s\" -o \"%s\"", pSourceFile, pTargetFile);

	HKEY hk;
	if (RegOpenKeyExA(HKEY_CURRENT_USER, 
		"Software\\Gameloft\\Irrlicht\\ColladaConverter", 
          0, KEY_READ, &hk))
   {
      printf("Could not open the registry key."); 
      return FALSE;
   }
	
	char szPath[MAX_PATH];
	DWORD dwBufLen = MAX_PATH;
	if (RegQueryValueEx(hk,             // subkey handle 
          "Path",					// value name 
          NULL, NULL, 
          (LPBYTE) szPath,				// pointer to value data 
          (DWORD*) (&dwBufLen)))			// data size
   {
      printf("Could not get the event message file."); 
      RegCloseKey(hk); 
      return FALSE;
   }

	RegCloseKey(hk);

	if(!CreateProcess(szPath, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		printf( "DAE Convertion failed (%d)\n", GetLastError() );
		_IRR_DEBUG_BREAK_IF("Converter may not be in the current directory");
		return -1;
	}

	// Wait until child processes exit.
	WaitForSingleObject( pi.hProcess, INFINITE );

	CloseHandle( pi.hProcess );

	return 0;
#endif
}

}; // namespace collada
}; // namespace irr