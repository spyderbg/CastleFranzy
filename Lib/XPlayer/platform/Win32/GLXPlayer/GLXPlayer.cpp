// iRF10.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <GdiPlus.h>
#include <time.h>
#include "GLXPlayer.h"
#include "Predefine.h"
#include "Game.h"
#include "MyGraphics.h"
#include "TouchRect.h"

using namespace Gdiplus;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

char	 g_moudlePath[512] = "";
HWND	 g_hWnd = NULL;
HWND	 g_settingWnd = NULL;
CGame*	 g_pGameApp = NULL;
u32		 g_tickCounter = 0;
u32		 g_frameCounter = 0;
u32		 g_fps = 0;
u32		 g_screenWidth = kScreenSize[0][0];
u32		 g_screenHeight = kScreenSize[0][1];
u32		 g_screenZoom = 1;
u32		 g_device = 0;
float	 g_maxFps = FPS;
bool     g_bPrintFlag = false;
bool	 g_showTouch = false;
HMENU	 g_popMenu = NULL;
int		 g_screenAngle = 0;

unsigned int g_keyPressed;
unsigned int g_currentPack = 0;

ULONG_PTR g_gdiplusToken;

int g_iCurMouseID = 0;

void InitializeGdiplus();
void ReleaseGdiplus();

#if D3DENABLE

LPDIRECT3D9         g_pD3D = NULL;
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL;
LPDIRECT3DSURFACE9  g_pd3dBackSurface = NULL;
LPDIRECT3DSURFACE9  g_pd3dRenderSurface = NULL;

HRESULT InitializeD3D(HWND hWnd);
HRESULT ReleaseD3D();

#endif

HRESULT ResetWindow(int nOrientation);
HRESULT ResetWindowSize(int width, int height, int zoom);


EGLDisplay	g_eglDisplay;
EGLConfig	g_eglConfig;
EGLContext	g_eglContextOpenGLES;
EGLSurface	g_eglWindowSurface;
HBITMAP		g_eglBitmap;
void*		g_eglBitmapPixel;

HRESULT InitEGL();
HRESULT FreeEGL();


void PrintScreen();
#if TOUCH_MODE > TOUCH_NONE
void UpdateTouchRects(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif
void UpdateAccelerometer();

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Setting(HWND, UINT, WPARAM, LPARAM);

VOID ShowSetting();
VOID InitModulePath();

struct _TouchActionTable
{
	TouchAction act;
	const char* text;
} TouchActionTable[TouchActionCount] = {
	{TouchActionSelectContinue, "Select&Continue"},
	{TouchActionMoveLeft, "Move Left"},
	{TouchActionMoveRight, "Move Right"},
	{TouchActionMoveUp, "Move Up"},
	{TouchActionMoveDown, "Move Down"},
	{TouchActionNext, "Next"},
	{TouchActionBack, "Back"},
	{TouchActionPageUp, "Page Up"},
	{TouchActionPageDown, "Page Down"},
	{TouchActionSelectOnly, "Select Only"},
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IRF10, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRF10));

	g_popMenu =	LoadMenu(NULL, MAKEINTRESOURCE(IDR_POPMENU));

	ShowSetting();

	srand((unsigned)time(NULL));

	g_tickCounter = GetTickCount();

	unsigned int nLastTime = g_tickCounter;

#if (MEMORY_DEBUG_LEVEL == 1) && ((PLATFORM != PLATFORM_MAC_OS_X)||defined(WIN32))
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |_CRTDBG_LEAK_CHECK_DF);
#endif

	InitializeGdiplus();

#if D3DENABLE
	InitializeD3D(g_hWnd);
#endif

	InitEGL();

	InitModulePath();

	g_pGameApp = NEW CGame();

	g_pGameApp->Initialize();

	//SendMessage(g_hWnd, WM_COMMAND, ID_ZOOM_2X, 0);

	//CWin32Socket::Startup();

	char wstrWindowText[30];

	bool isGameQuit = false;

	long quitBeginTime = 0;

	// Main message loop:
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) /*|| !g_bActive*/)
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				if (msg.message == WM_QUIT)
				{
					isGameQuit = true;
					quitBeginTime = GetTickCount();
					g_pGameApp->BeginQuit();
					//break;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		else
		{
			const float kFpsMax = g_maxFps;

			if (GetTickCount() <= nLastTime + (1000.0f / kFpsMax) * g_frameCounter)
			{
				Sleep(10);
				continue;
			}
			if (g_pGameApp->IsTerminal())
			{
				PostQuitMessage(0);
			}

			UpdateAccelerometer();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			if(isGameQuit)
			{
				if(g_pGameApp->UpdateQuit())
				{
					break;
				}

				if(GetTickCount() - quitBeginTime > 1500)
				{
					break;
				}
			}
			else
			{
				g_pGameApp->Update();
			}

			g_pGameApp->Paint();
	
#if D3DENABLE	
			if(g_device == 0)
			{
				D3DLOCKED_RECT d3dLockRect;
				RECT rect = {0, 0, g_screenWidth, g_screenHeight};
				g_pd3dRenderSurface->LockRect(&d3dLockRect, &rect, D3DLOCK_DISCARD);

				u32* s = (u32*)g_eglBitmapPixel;
				u32* d = (u32*)d3dLockRect.pBits;

				for(int i = 0; i < (int)g_screenHeight; i++)
				{
					for(int j = 0; j < (int)g_screenWidth; j++)
					{
						*(d + j) = *(s + g_screenHeight - 1 - i + j * g_screenHeight);
					}

					d += g_screenWidth;
				}

				g_pd3dRenderSurface->UnlockRect();
				g_pd3dDevice->UpdateSurface(g_pd3dRenderSurface, NULL, g_pd3dBackSurface, NULL);
				g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
			}
			else
#endif		
			{
				CDisplay* pDisplay = g_pGameApp->GetDisplay();

				u32* s = (u32*)g_eglBitmapPixel;
				u16* d = (u16*)pDisplay->GetScreenImage(0)->GetPixelData();

				for(int i = 0; i < (int)g_screenHeight; i++)
				{
					for(int j = 0; j < (int)g_screenWidth; j++)
					{
						u32 c = *(s + g_screenHeight - 1 - i + j * g_screenHeight);
					
						*(d + j) = ((c >> 8) & 0xF800) | ((c >> 5) & 0x7E0) | ((c >> 3) & 0x1F);
					}

					d += g_screenWidth;
				}
				
				
				g_pGameApp->GetDisplay()->UpdateScreen();
			}
	

			if(g_tickCounter + 1000 < GetTickCount())
			{
				g_tickCounter = GetTickCount();
				nLastTime = g_tickCounter;
				g_fps = g_frameCounter;
				g_frameCounter = 0;
			}
			else
			{
				g_frameCounter++;
			}

			//wsprintf(wstrWindowText, L"fps:%d", g_fps);
			sprintf(wstrWindowText, "fps:%d", g_fps);
			//SetWindowText(g_hWnd, wstrWindowText);

			if(g_bPrintFlag)
			{
				g_bPrintFlag = false;
				PrintScreen();
			}
		}
	}

	SAFE_DELETE(g_pGameApp);

#if (MEMORY_DEBUG_LEVEL == 1) && ((PLATFORM != PLATFORM_MAC_OS_X)||defined(WIN32))
	_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_DEBUG);
#endif

	FreeEGL();
	ReleaseGdiplus();

#if D3DENABLE
	ReleaseD3D();
#endif

	//CWin32Socket::Clearup();

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IRF10));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_IRF10);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   int sizeX = g_screenWidth;
   int sizeY = g_screenHeight;
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX ,
      CW_USEDEFAULT, CW_USEDEFAULT, sizeX, sizeY, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   RECT clientRect;
  
   GetClientRect(hWnd, &clientRect);

   sizeX += sizeX - clientRect.right;
   sizeY += sizeY - clientRect.bottom;

   int ScreenW = GetSystemMetrics(SM_CXSCREEN);
   int ScreenH = GetSystemMetrics(SM_CYSCREEN);

   SetWindowPos(hWnd, 0, (ScreenW - sizeX) / 2, (ScreenH - sizeY) / 2, sizeX, sizeY, 0);

#if FPS == 15
   SendMessage(hWnd, WM_COMMAND, ID_FPS_15FPS, 0);
//#elif FPS == 20
//	SendMessage(hWnd, WM_COMMAND, ID_FPS_20FPS, 0);
//#elif FPS == 30
//   SendMessage(hWnd, WM_COMMAND, ID_FPS_30FPS, 0);
//#endif

#else
   SendMessage(hWnd, WM_COMMAND, ID_FPS_20FPS, 0);
#endif

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

VOID ShowSetting()
{
	if(!g_showTouch)
		return;

	if(g_settingWnd == NULL)
		g_settingWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_TOUCH_SETTING), NULL, Setting); 
	
	RECT rect;
	GetClientRect(g_hWnd, &rect);

	POINT point;
	point.x = rect.right;
	point.y = rect.top;
	ClientToScreen(g_hWnd, &point);

	GetClientRect(g_settingWnd, &rect);
	SetWindowPos(g_settingWnd, 0, point.x + 4, point.y - 46, 70, 80, 1);
	
	ShowWindow(g_settingWnd, SW_SHOW);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;


	//if(g_pGameApp != NULL)
	//{
	//	CBluetoothWin32* pBluetooth = (CBluetoothWin32*)g_pGameApp->GetBluetooth();

	//	if(pBluetooth != NULL)
	//		pBluetooth->HandleEvent(hWnd, message, lParam);
	//}
#if TOUCH_MODE > TOUCH_NONE
	UpdateTouchRects(hWnd, message, wParam, lParam);
#endif
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_DISPLAY_DIRECT3D:
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_DIRECT3D, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_GDI, MF_UNCHECKED);
			g_device = 0;
			break;

		case ID_DISPLAY_GDI:
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_DIRECT3D, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_GDI, MF_CHECKED);
			g_device = 1;
			break;

		case ID_DISPLAY_ANGLE_0:
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_0, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_90, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_180, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_270, MF_UNCHECKED);
			ResetWindow(DISP_ORIENTATION_ANGLE_0);
			break;

		case ID_DISPLAY_ANGLE_90:
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_0,   MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_90,  MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_180, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_270, MF_UNCHECKED);
			ResetWindow(DISP_ORIENTATION_ANGLE_90);
			break;

		case ID_DISPLAY_ANGLE_180:
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_0,   MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_90,  MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_180, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_270, MF_UNCHECKED);
			ResetWindow(DISP_ORIENTATION_ANGLE_180);
			break;

		case ID_DISPLAY_ANGLE_270:
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_0,   MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_90,  MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_180, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_DISPLAY_ANGLE_270, MF_CHECKED);
			ResetWindow(DISP_ORIENTATION_ANGLE_270);
			break;


		case ID_FPS_15FPS:
			CheckMenuItem(GetMenu(hWnd), ID_FPS_15FPS, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_FPS_20FPS, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_FPS_30FPS, MF_UNCHECKED);
			g_maxFps = 15.0f;
			break;

		case ID_FPS_20FPS:
			CheckMenuItem(GetMenu(hWnd), ID_FPS_15FPS, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_FPS_20FPS, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_FPS_30FPS, MF_UNCHECKED);
			g_maxFps = 25.0f;
			break;

		case ID_FPS_30FPS:
			CheckMenuItem(GetMenu(hWnd), ID_FPS_15FPS, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_FPS_20FPS, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_FPS_30FPS, MF_CHECKED);
			g_maxFps = 30.0f;
			break;

		case ID_ZOOM_1X:
			CheckMenuItem(GetMenu(hWnd), ID_ZOOM_1X, MF_CHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_ZOOM_2X, MF_UNCHECKED);
			g_screenZoom = 1;
			ResetWindowSize(g_screenWidth, g_screenHeight, g_screenZoom);
			break;

		case ID_ZOOM_2X:
			CheckMenuItem(GetMenu(hWnd), ID_ZOOM_1X, MF_UNCHECKED);
			CheckMenuItem(GetMenu(hWnd), ID_ZOOM_2X, MF_CHECKED);
			g_screenZoom = 2;
			ResetWindowSize(g_screenWidth, g_screenHeight, g_screenZoom);
			break;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
		//UpdateKey(wParam, message == WM_KEYDOWN);
		break;

	case WM_MOVE:
	case WM_SIZE:
		ShowSetting();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}



INT_PTR CALLBACK Setting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{	
	return (INT_PTR)FALSE;
}


VOID InitModulePath()
{
	GetModuleFileNameA(NULL, (char*)&g_moudlePath, sizeof(g_moudlePath));

	size_t pos = strlen(g_moudlePath);

	while(pos > 0)
	{
		if(g_moudlePath[pos--] == '\\')
		{
			break;
		}
	}

	g_moudlePath[pos + 2] = 0;
}

int ShowMessageBox(const char* message, const char* code, const char* file, const long line)
{
	char szTemp[256];
	
	sprintf(szTemp, "Code:\t%s\nFile:\t%s\nLine:\t%d\nContinue?", code, file, line);
	
	int ret = MessageBoxA(g_hWnd, szTemp, message, MB_YESNO);

	return ret != IDYES;
}

void Trace(char* format, ...)
{
	char szTemp[1024];

	va_list ap;
	va_start(ap, format);
	_vsnprintf(szTemp, sizeof(szTemp), format, ap);
	va_end(ap);

	//MessageBoxA(g_hWnd, szTemp, "", 0);
}

void  InitializeGdiplus()
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
}

void  ReleaseGdiplus()
{
	GdiplusShutdown(g_gdiplusToken);
}

#if D3DENABLE

HRESULT InitializeD3D(HWND hWnd)
{
	// Create the D3D object.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the D3DDevice
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	// Create the D3DSurface
	if( FAILED( g_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO,  &g_pd3dBackSurface)))
	{
		return E_FAIL;
	}
	
	D3DDISPLAYMODE d3ddm;
	
	if( FAILED( g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)) )
	{
		return E_FAIL;
	}

	// Create the D3DSurface
	if( FAILED( g_pd3dDevice->CreateOffscreenPlainSurface(g_screenWidth, g_screenHeight, d3ddm.Format, D3DPOOL_SYSTEMMEM , &g_pd3dRenderSurface, NULL)) )
	{
		return E_FAIL;
	}

	return S_OK;
}


HRESULT ReleaseD3D()
{
	if(g_pD3D)
		g_pD3D->Release();

	if(g_pd3dDevice)
		g_pd3dDevice->Release();

	if(g_pd3dBackSurface)
		g_pd3dBackSurface->Release();

	if(g_pd3dRenderSurface)
		g_pd3dRenderSurface->Release();

	return S_OK;
}
#endif

#define SAMPLE_COLOR_DEPTH 32

HBITMAP CreateHBITMAP(HWND hWnd)
{
	HDC hDC = GetDC(hWnd);

	const size_t	bmiSize = sizeof(BITMAPINFO) + 256U*sizeof(RGBQUAD);
	BITMAPINFO*		bmi;

	bmi = (BITMAPINFO*)malloc(bmiSize);
	memset(bmi, 0, bmiSize);

	bmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth			= (int)g_screenHeight;
	bmi->bmiHeader.biHeight			= -(int)g_screenWidth;
	bmi->bmiHeader.biPlanes			= (short)1;
	bmi->bmiHeader.biBitCount		= (unsigned short)SAMPLE_COLOR_DEPTH;
	bmi->bmiHeader.biCompression	= BI_RGB;
	bmi->bmiHeader.biSizeImage		= 0;
	bmi->bmiHeader.biXPelsPerMeter	= 0;
	bmi->bmiHeader.biYPelsPerMeter	= 0;
	bmi->bmiHeader.biClrUsed		= 3;
	bmi->bmiHeader.biClrImportant	= 0;

	HBITMAP hBitmap = CreateDIBSection(hDC, bmi, DIB_RGB_COLORS, (void**)&g_eglBitmapPixel, NULL, 0);

	free(bmi);
	ReleaseDC(hWnd, hDC);

	return hBitmap;
}


HRESULT InitEGL()
{
	g_eglBitmap = CreateHBITMAP(g_hWnd);

	static const EGLint s_configAttribs[] =
	{
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE, 	8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE, 	EGL_DONT_CARE,
		EGL_DEPTH_SIZE, 	16,
		EGL_STENCIL_SIZE,	EGL_DONT_CARE,
		EGL_SURFACE_TYPE,	EGL_PIXMAP_BIT,
		EGL_NONE
	};

	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;

	g_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(g_eglDisplay, &majorVersion, &minorVersion);
	eglGetConfigs(g_eglDisplay, NULL, 0, &numConfigs);
	eglChooseConfig(g_eglDisplay, s_configAttribs, &g_eglConfig, 1, &numConfigs);

	EGLContext eglContext = eglCreateContext(g_eglDisplay, g_eglConfig, NULL, NULL);
	g_eglWindowSurface = eglCreatePixmapSurface(g_eglDisplay, g_eglConfig, g_eglBitmap, NULL);
	eglMakeCurrent(g_eglDisplay, g_eglWindowSurface, g_eglWindowSurface, eglContext);

	return S_OK;
}

HRESULT FreeEGL()
{
	eglMakeCurrent(g_eglDisplay, NULL, NULL, NULL);
	eglDestroyContext(g_eglDisplay, g_eglContextOpenGLES);
	eglDestroySurface(g_eglDisplay, g_eglWindowSurface);
	eglTerminate(g_eglDisplay);

	DeleteObject(g_eglBitmap);

	return S_OK;
}


HRESULT ResetWindowSize(int width, int height, int zoom)
{
	width *= zoom;
	height *= zoom;

	RECT windowRect;
	GetWindowRect(g_hWnd, &windowRect);

	SetWindowPos(g_hWnd, 0, windowRect.left, windowRect.top, width, height, 0);

	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);

	width += width - clientRect.right;
	height += height - clientRect.bottom;

	SetWindowPos(g_hWnd, 0, windowRect.left, windowRect.top, width, height, 0);

	UpdateWindow(g_hWnd);

	return TRUE;
}

HRESULT ResetWindow(int nOrientation)
{
	if(nOrientation == DISP_ORIENTATION_ANGLE_0 || nOrientation == DISP_ORIENTATION_ANGLE_180)
	{
		g_screenWidth = kScreenSize[0][0];
		g_screenHeight = kScreenSize[0][1];
	}
	else// if(nOrientation == DISP_ORIENTATION_ANGLE_90 || nOrientation == DISP_ORIENTATION_ANGLE_270)
	{
		g_screenWidth = kScreenSize[1][0];
		g_screenHeight = kScreenSize[1][1];
	}

	
	ResetWindowSize(g_screenWidth, g_screenHeight, 1);

	////reset d3d surface
#if D3DENABLE
	ReleaseD3D();
	InitializeD3D(g_hWnd);
#endif	

//	if(g_pGameApp != NULL)
//	{
//		g_pGameApp->GetDisplay()->SetDeviceOrientation(nOrientation);
//		g_pGameApp->UpdateOrientation();
//		g_pGameApp->GetOrientationManager()->SetCurrentOrientation(nOrientation);
//	}

	ResetWindowSize(g_screenWidth, g_screenHeight, g_screenZoom);

	return TRUE;
}

unsigned int g_nScreenShotCount = 0;

int GetCodecClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure
	GetImageEncoders(num, size, pImageCodecInfo);
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	} // for
	free(pImageCodecInfo);
	return -1;  // Failure
} // GetCodecClsid

HRESULT SaveBitmap(Bitmap* pImage)
{
	if(pImage == NULL)
		return false;

	int w = pImage->GetWidth();
	int h = pImage->GetHeight();

	Bitmap *TempImage = new Bitmap(w, h, PixelFormat24bppRGB);

	Graphics g(TempImage);

	g.DrawImage(pImage, RectF(0, 0, (REAL)w, (REAL)h), 0, 0, (REAL)w, (REAL)h, UnitPixel, NULL);

	CLSID bmpClsid;

	GetCodecClsid(L"image/bmp", &bmpClsid);

	char fileNameBuffer[256];
	sprintf(fileNameBuffer, "ScreenShot_%d.bmp", g_nScreenShotCount);
	FILE* fp = fopen(fileNameBuffer, "r");

	while(fp != NULL)
	{
		fclose(fp);
		sprintf(fileNameBuffer, "ScreenShot_%d.bmp", ++g_nScreenShotCount);
		fp = fopen(fileNameBuffer, "r");
	}

	WCHAR wFileName[256];	
// TBI tiehua.lu
//	wsprintf(wFileName, L"ScreenShot_%d.bmp", g_nScreenShotCount);

	if( TempImage->Save(wFileName, &bmpClsid, NULL) != Ok)
	{
		SAFE_DELETE(TempImage);
		return false;
	}
	else
	{	
		SAFE_DELETE(TempImage);
		return true;
	}
}

VOID PrintScreen()
{
	Bitmap* pBitmap = new Bitmap(g_eglBitmap, NULL);
	pBitmap->RotateFlip(Rotate270FlipNone);
	SaveBitmap(pBitmap);
	delete pBitmap;
}


int  g_nFocusTouchRectIndex = -1;
int  g_bLBottomDown = false;

unsigned int	g_mouseX = 0;
unsigned int	g_mouseY = 0;
bool			g_bMousePressed = false;

POINT			g_mousePos;

void UpdateTouchRects(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	int mouse_x = LOWORD(lParam) / g_screenZoom;
	int mouse_y = HIWORD(lParam) / g_screenZoom;
	int dx, dy;

	switch(message)
	{
	case WM_RBUTTONDOWN:
		g_mousePos.x = mouse_x;
		g_mousePos.y = mouse_y;
		g_mouseX = mouse_x;
		g_mouseY = mouse_y;
		break;

	case WM_LBUTTONDOWN:
		g_bLBottomDown = true;	
		g_mousePos.x = mouse_x;
		g_mousePos.y = mouse_y;
	
		if(!g_showTouch)
		{	
			g_mouseX = mouse_x;
			g_mouseY = mouse_y;
			g_bMousePressed = true;
		}
		break;
	
	case WM_LBUTTONUP:
		g_bLBottomDown = false;

		if(!g_showTouch)
		{	
			//g_mouseX = 0;
			//g_mouseY = 0;
			g_bMousePressed = false;
		}

		break;


	case WM_MOUSEMOVE:
		{
			char text[256] = "";
			sprintf(text,"Mouse X:%d,Y:%d", mouse_x, mouse_y);
			SetWindowTextA(g_hWnd, text);
		}
		if(!g_showTouch)
		{
			g_mouseX = mouse_x;
			g_mouseY = mouse_y;
		}

		dx = mouse_x - g_mousePos.x;
		dy = mouse_y - g_mousePos.y;

		g_mousePos.x = mouse_x;
		g_mousePos.y = mouse_y;

		break;

	default:
		break;
	}
}

void UpdateAccelerometer()
{
	GLTvec3D	vecAcc1(GLT_FLOAT_TO_FIXED(-1.0f), GLT_FLOAT_TO_FIXED(0.0f), GLT_FLOAT_TO_FIXED(0.0f));
	GLTvec3D	vecAcc2;
	GLTmatrix	matRot;
	matRot.SetIdentity();
	matRot.SetRotate(GLT_FLOAT_TO_FIXED(g_screenAngle), GLT_INT_TO_FIXED(0),GLT_INT_TO_FIXED(0),GLT_INT_TO_FIXED(1));
	matRot.Mul(vecAcc1, vecAcc2);

	double acc_x = vecAcc2.x / 65536.0;
	double acc_y = vecAcc2.y / 65536.0;
	double acc_z = vecAcc2.z / 65536.0;	
}

