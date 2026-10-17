#include <windows.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <conio.h>
using namespace std;


wchar_t setupDirectory[MAX_PATH];
int setupMeshViewer();
int setupIrrlicht();
int setupColladaConverter();
int setupDAEOpen();
int setupOpenGLES();


void main(void)
{
	_wgetcwd(setupDirectory, MAX_PATH);

	cout << "IrrLicht Registers Setup" << endl;
	
	if(setupIrrlicht())
	{
		cout << "Setup Irrlicht Path - Failed" << endl;
	}
	else
	{
		cout << "Setup Irrlicht Path - Succeed" << endl;
	}

	if(setupMeshViewer())
	{
		cout << "Setup MeshViewer Path - Failed" << endl;
	}
	else
	{
		cout << "Setup MeshViewer Path - Succeed" << endl;
	}

	if(setupColladaConverter())
	{
		cout << "Setup ColladaConverter Path - Failed" << endl;
	}
	else
	{
		cout << "Setup ColladaConverter Path - Succeed" << endl;
	}

	if(setupDAEOpen())
	{
		cout << "Setup .bdae/.dae Context - Failed" << endl;
	}
	else
	{
		cout << "Setup .bdae/.dae Context - Succeed" << endl;
	}

	if(setupOpenGLES())
	{
		cout << "Setup OpenGLES DLL - Failed" << endl;
	}
	else
	{
		cout << "Setup OpenGLES DLL - Succeed" << endl;
	}

	cout << "Press any key to quit" << endl;
	getch();
};

int setupIrrlicht()
{
	HKEY hk; 
	DWORD dwData, dwDisp;

	if (RegCreateKeyEx (HKEY_CURRENT_USER, 
	L"Software\\Gameloft\\Irrlicht", 0, NULL, REG_OPTION_NON_VOLATILE,
		  KEY_WRITE, NULL, &hk, &dwDisp))
	{
		printf("Could not create the registry key."); 
		return -1;
	}

	std::wstring irrlicht;
	irrlicht = setupDirectory;

	if (RegSetValueEx(hk,			// subkey handle 
		L"Path",					// value name 
		0,							// must be zero 
		REG_EXPAND_SZ,				// value type 
		(LPBYTE) irrlicht.c_str(),			// pointer to value data 
		(DWORD) (lstrlen(irrlicht.c_str())+1)*sizeof(TCHAR))) // data size
	{
	  RegCloseKey(hk); 
	  return -1;
	}

	RegCloseKey(hk); 
	return 0;
}

int setupMeshViewer()
{
	HKEY hk; 
	DWORD dwData, dwDisp;

	if (RegCreateKeyEx (HKEY_CURRENT_USER, 
	L"Software\\Gameloft\\Irrlicht\\MeshViewer", 0, NULL, REG_OPTION_NON_VOLATILE,
		  KEY_WRITE, NULL, &hk, &dwDisp))
	{
		return -1;
	}

	std::wstring meshViewerPath;
	meshViewerPath = setupDirectory;
	meshViewerPath += L"\\bin\\Win32-VisualStudio\\18.ColladaEditor.exe";

	if (RegSetValueEx(hk,			// subkey handle 
		L"Path",					// value name 
		0,							// must be zero 
		REG_EXPAND_SZ,				// value type 
		(LPBYTE) meshViewerPath.c_str(),			// pointer to value data 
		(DWORD) (lstrlen(meshViewerPath.c_str())+1)*sizeof(TCHAR))) // data size
	{
	  printf("Could not set the event message file."); 
	  RegCloseKey(hk); 
	  return -1;
	}

	RegCloseKey(hk); 
	return 0;

};

int setupColladaConverter()
{
	HKEY hk; 
	DWORD dwData, dwDisp;

	if (RegCreateKeyEx (HKEY_CURRENT_USER, 
	L"Software\\Gameloft\\Irrlicht\\ColladaConverter", 0, NULL, REG_OPTION_NON_VOLATILE,
		  KEY_WRITE, NULL, &hk, &dwDisp))
	{
		return -1;
	}

	std::wstring colladaConvertr;
	colladaConvertr = setupDirectory;
	colladaConvertr += L"\\tools\\ColladaConverter\\bin\\Release\\ColladaConverter_2008.exe";

	if (RegSetValueEx(hk,			// subkey handle 
		L"Path",					// value name 
		0,							// must be zero 
		REG_EXPAND_SZ,				// value type 
		(LPBYTE) colladaConvertr.c_str(),			// pointer to value data 
		(DWORD) (lstrlen(colladaConvertr.c_str())+1)*sizeof(TCHAR))) // data size
	{
	  printf("Could not set the event message file."); 
	  RegCloseKey(hk); 
	  return -1;
	}

	RegCloseKey(hk); 
	return 0;

};

int setupOpenGLES()
{
	std::wstring openGLESDLLPath;
	std::wstring openGLESDLLPathCL;
	std::wstring openGLESDLLPathCM;
	std::wstring systemRoot;
	openGLESDLLPath = setupDirectory;

	openGLESDLLPath += L"\\source\\Irrlicht\\PowerVR\\OGLES\\WindowsPC\\Lib\\";
	systemRoot += _wgetenv(L"SYSTEMROOT");
	systemRoot += L"\\System32\\";
	CopyFile((openGLESDLLPath + L"libgles_cl.dll").c_str(), 
		(systemRoot + L"libgles_cl.dll").c_str(), 
		false);
	CopyFile((openGLESDLLPath + L"libgles_cm.dll").c_str(), 
		(systemRoot + L"libgles_cm.dll").c_str(), 
		false);

	return 0;
}

int setupDAEOpen()
{
	HKEY hk; 
	DWORD dwData, dwDisp;

	if (RegCreateKeyEx (HKEY_CLASSES_ROOT, 
	L".dae", 0, NULL, REG_OPTION_NON_VOLATILE,
		  KEY_WRITE, NULL, &hk, &dwDisp))
	{
		return -1;
	}

	if (RegSetValueEx(hk,			// subkey handle 
		0,							// value name 
		0,							// must be zero 
		REG_EXPAND_SZ,				// value type 
		(LPBYTE) L"dae_auto_file",			// pointer to value data 
		(DWORD) (lstrlen(L"dae_auto_file")+1)*sizeof(TCHAR))) // data size
	{
	  printf("Could not set the event message file."); 
	  RegCloseKey(hk); 
	  return -1;
	}

	RegCloseKey(hk); 

	if (RegCreateKeyEx (HKEY_CLASSES_ROOT, 
	L".bdae", 0, NULL, REG_OPTION_NON_VOLATILE,
		  KEY_WRITE, NULL, &hk, &dwDisp))
	{
		return -1;
	}

	if (RegSetValueEx(hk,			// subkey handle 
		0,							// value name 
		0,							// must be zero 
		REG_EXPAND_SZ,				// value type 
		(LPBYTE) L"bdae_auto_file",			// pointer to value data 
		(DWORD) (lstrlen(L"bdae_auto_file")+1)*sizeof(TCHAR))) // data size
	{
	  printf("Could not set the event message file."); 
	  RegCloseKey(hk); 
	  return -1;
	}

	RegCloseKey(hk); 

	if (RegCreateKeyEx (HKEY_CLASSES_ROOT, 
	L"dae_auto_file\\shell\\open\\command", 0, NULL, REG_OPTION_NON_VOLATILE,
		  KEY_WRITE, NULL, &hk, &dwDisp))
	{
		return -1;
	}

	std::wstring meshViewerPath;
	meshViewerPath = setupDirectory;
	meshViewerPath += L"\\bin\\Win32-VisualStudio\\18.ColladaEditor.exe \"%1\"";

	if (RegSetValueEx(hk,			// subkey handle 
		0,							// value name 
		0,							// must be zero 
		REG_SZ,				// value type 
		(LPBYTE) meshViewerPath.c_str(),			// pointer to value data 
		(DWORD) (lstrlen(meshViewerPath.c_str())+1)*sizeof(TCHAR))) // data size
	{
	  printf("Could not set the event message file."); 
	  RegCloseKey(hk); 
	  return -1;
	}

	RegCloseKey(hk); 

	if (RegCreateKeyEx (HKEY_CLASSES_ROOT, 
	L"dae_auto_file\\shell\\convert\\command", 0, NULL, REG_OPTION_NON_VOLATILE,
		  KEY_WRITE, NULL, &hk, &dwDisp))
	{
		return -1;
	}

	std::wstring colladaConvertr;
	colladaConvertr = setupDirectory;
	colladaConvertr += L"\\tools\\ColladaConverter\\ColladaConverter.bat \"%1\"";

	if (RegSetValueEx(hk,			// subkey handle 
		0,							// value name 
		0,							// must be zero 
		REG_SZ,				// value type 
		(LPBYTE) colladaConvertr.c_str(),			// pointer to value data 
		(DWORD) (lstrlen(colladaConvertr.c_str())+1)*sizeof(TCHAR))) // data size
	{
	  printf("Could not set the event message file."); 
	  RegCloseKey(hk); 
	  return -1;
	}

	RegCloseKey(hk); 

	if (RegCreateKeyEx (HKEY_CLASSES_ROOT, 
	L"bdae_auto_file\\shell\\open\\command", 0, NULL, REG_OPTION_NON_VOLATILE,
		  KEY_WRITE, NULL, &hk, &dwDisp))
	{
		return -1;
	}

	meshViewerPath = setupDirectory;
	meshViewerPath += L"\\bin\\Win32-VisualStudio\\18.ColladaEditor.exe \"%1\"";

	if (RegSetValueEx(hk,			// subkey handle 
		0,							// value name 
		0,							// must be zero 
		REG_SZ,				// value type 
		(LPBYTE) meshViewerPath.c_str(),			// pointer to value data 
		(DWORD) (lstrlen(meshViewerPath.c_str())+1)*sizeof(TCHAR))) // data size
	{
	  printf("Could not set the event message file."); 
	  RegCloseKey(hk); 
	  return -1;
	}

	RegCloseKey(hk); 
	return 0;
}