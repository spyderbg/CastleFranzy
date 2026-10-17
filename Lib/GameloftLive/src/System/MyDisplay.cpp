#include "GLLiveStdafx.h"
#include "MyDisplay.h"
#include "MyGraphics.h"
#include "Image.h"


#if CHECK_UPDATESCREEN_TIME
#include "GLLiveGame.h"
#endif

#if PLATFORM == PLATFORM_SYMBIAN
	#include <gdi.h>
	extern unsigned int g_nScreenWidth;
	extern unsigned int g_nScreenHeight;
	extern unsigned int g_nScreenLinePitch;
#elif PLATFORM == PLATFORM_WINDOWS_MOBILE
	#include <ddraw.h>
	#include <gx.h>
#elif PLATFORM == PLATFORM_KDDI
	#include <AEEBitmap.h>
	#include "Global.h"
	#include "RealFootball2009.h"
#endif

#if PLATFORM == PLATFORM_WINDOWS_MOBILE
extern HWND	g_hWnd;
extern u32	g_screenWidth;
extern u32	g_screenHeight;

extern LPDIRECTDRAW            g_lpDD;
extern LPDIRECTDRAWSURFACE     g_lpFrontBuffer;
extern LPDIRECTDRAWSURFACE     g_lpBackBuffer;
extern BOOL                    g_bSingleBuffer;
extern int					   g_iDeviceMode;
#elif (PLATFORM == PLATFORM_WIN32) || ((PLATFORM == PLATFORM_MAC_OS_X)&&defined(WIN32))


extern HWND	g_hWnd;

#if D3DENABLE

extern LPDIRECT3DDEVICE9   g_pd3dDevice;
extern LPDIRECT3DSURFACE9  g_pd3dBackSurface;
extern LPDIRECT3DSURFACE9  g_pd3dRenderSurface;

#endif

extern u32	g_screenWidth;
extern u32	g_screenHeight;
extern u32	g_screenZoom;
extern u32  g_device;
#endif

namespace gllive
{
	CGLLiveDisplay::CGLLiveDisplay()
	#ifdef SCREEN_240_320_FORCE_FLIP
	: m_deviceOrientation(DISP_ORIENTATION_ANGLE_90)
	#else
		#ifdef INIT_LANDSCAPE
	: m_deviceOrientation(DISP_ORIENTATION_ANGLE_90)
		#else
	: m_deviceOrientation(DISP_ORIENTATION_ANGLE_0)
		#endif
	#endif
	#if CHECK_UPDATESCREEN_TIME
	, m_nFrameCounter(0)
	, m_UpdateScreenTime(0)
	#endif
	{
		for(int i = 0; i < DISP_COUNT; i++)
		{
	#if PLATFORM == PLATFORM_NGAGE
			m_pScreenBuffer[i] = NULL;
	#elif (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X) || (PLATFORM == PLATFORM_KDDI)
			m_pScreenImage[i] = NULL;
	#endif		
		}
	}

	CGLLiveDisplay::~CGLLiveDisplay()
	{
		for(int i = 0; i < DISP_COUNT; i++)
		{
	#if PLATFORM == PLATFORM_NGAGE
			SAFE_DELETE(m_pScreenBuffer[i]);
			SAFE_DELETE(m_pScreenImage[i]);
	#elif PLATFORM == PLATFORM_KDDI
			if(i == 0)
			{
				SAFE_DELETE(m_pScreenImage[i]);
			}
			else
			{
				SAFE_DELETE(m_pScreenBuffer[i]);
				SAFE_DELETE(m_pScreenImage[i]);
			}
	#else
			SAFE_DELETE(m_pScreenImage[i]);
	#endif
		}
	}

	bool CGLLiveDisplay::InitializeScreen()
	{
	#if PLATFORM == PLATFORM_MAC_OS_X
		return true;
	#endif

		for(int i = 0; i < DISP_COUNT; i++)
		{		
			const int width = kScreenSize[i][0];
			const int height = kScreenSize[i][1];
	#if PLATFORM == PLATFORM_WIN32 || PLATFORM == PLATFORM_WINDOWS_MOBILE

			m_hDC[i] = CreateCompatibleDC(NULL);

			BITMAPINFO *pbi = (BITMAPINFO *)NEW char[sizeof(BITMAPINFOHEADER) + 4 * 3];

			pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pbi->bmiHeader.biWidth = width;
			pbi->bmiHeader.biHeight = -height;
			pbi->bmiHeader.biPlanes = 1;
			pbi->bmiHeader.biBitCount = sizeof(NativePixel) * 8;
			pbi->bmiHeader.biCompression = BI_BITFIELDS;
			pbi->bmiHeader.biSizeImage = 0;
			pbi->bmiHeader.biXPelsPerMeter = 0;
			pbi->bmiHeader.biYPelsPerMeter = 0;
			pbi->bmiHeader.biClrUsed = 0;
			pbi->bmiHeader.biClrImportant = 0;
			DWORD *p = (DWORD*)(&pbi->bmiColors);
	#if NATIVE_COLOR_DEPTH == 16
			p[0] = 0xF800;
			p[1] = 0x07E0;
			p[2] = 0x001F;
	#elif NATIVE_COLOR_DEPTH == 15
			p[0] = 0x7C00;
			p[1] = 0x03E0;
			p[2] = 0x001F;
	#elif NATIVE_COLOR_DEPTH == 12
			p[0] = 0x0F00;
			p[1] = 0x00F0;
			p[2] = 0x000F;
	#elif NATIVE_COLOR_DEPTH == 24
			p[0] = 0xFF0000;
			p[1] = 0x00FF00;
			p[2] = 0x0000FF;
	#else
			ASSERT(false);
	#endif

			void *pBits;

			m_hBmp[i] = CreateDIBSection(m_hDC[i], pbi, DIB_RGB_COLORS, (VOID**)&pBits, NULL, NULL);

			SAFE_DELETE_ARRAY(pbi);

			if(m_hBmp[i] == NULL)
			{
				DeleteDC(m_hDC[i]);
				m_hDC[i] = NULL;
				return false;
			}

			SelectObject(m_hDC[i], m_hBmp[i]);

			m_pScreenImage[i] = NEW CGLLiveMutableImage(width, height, (width * sizeof(NativePixel) + 3) & ~3, NATIVE_COLOR_DEPTH, pBits);

	#elif PLATFORM == PLATFORM_NGAGE
			m_pScreenBuffer[i] = NEW char[width * height * 2];
			m_pScreenImage[i] = NEW CGLLiveMutableImage(width, height, (width * sizeof(NativePixel) + 3) & ~3, NATIVE_COLOR_DEPTH, m_pScreenBuffer[i]);
	#elif PLATFORM == PLATFORM_SYMBIAN
			m_pScreenBuffer[i] = NEW char[width * height * 2];
			m_pScreenImage[i] = NEW CGLLiveMutableImage(width, height, (width * sizeof(NativePixel) + 3) & ~3, NATIVE_COLOR_DEPTH, m_pScreenBuffer[i]);		
	#elif PLATFORM == PLATFORM_KDDI
			if(i == 0)
			{
				IDIB* p1 = (IDIB*)GetApp()->m_pDeviceBitmap;
				m_pScreenBuffer[i] = (char*)p1->pBmp;
				m_pScreenImage[i] = NEW CGLLiveMutableImage(width, height, (width * sizeof(NativePixel) + 3) & ~3, NATIVE_COLOR_DEPTH, m_pScreenBuffer[i]);
			}
			else
			{
				m_pScreenBuffer[i] = NEW char[width * height * 2];
				m_pScreenImage[i] = NEW CGLLiveMutableImage(width, height, (width * sizeof(NativePixel) + 3) & ~3, NATIVE_COLOR_DEPTH, m_pScreenBuffer[i]);

			}
	#endif
		}

		return true;
	}



	void CGLLiveDisplay::UpdateScreen(int aMode)
	{
	#if PLATFORM == PLATFORM_MAC_OS_X
		return;
	#endif

	#if CHECK_UPDATESCREEN_TIME
		m_UpdateScreenTime = GetGlobal().m_pGame->GetCurMTime();
	#endif

	#if PLATFORM == PLATFORM_WINDOWS_MOBILE
		UpdateOrientation();
		HWND hwnd = g_hWnd;
		RECT rc;
		GetClientRect(hwnd, &rc);
		HDC dc = GetDC(hwnd);
		UpdateOrientation();
		BitBlt(dc, 0, 0, g_screenWidth, g_screenHeight, m_hDC[m_nCurrentDisplay], 0, 0, SRCCOPY);	
		ReleaseDC(hwnd, dc);
	#elif PLATFORM == PLATFORM_KDDI
 		m_nCurrentDisplay = GetDisplayMode();
 		IDIB* p1 = (IDIB*)GetApp()->m_pDeviceBitmap;
		IDisplay* pDisp = GetApp()->a.m_pIDisplay;
	 
 		if(m_nCurrentDisplay == DISP_240X296)
 		{
	// 		u32* dst = (u32*)p1->pBmp;
	// 		u32* src = (u32*)m_pScreenImage[m_nCurrentDisplay]->GetPixelData();
	// 		u32* e = src + kScreenSize[m_nCurrentDisplay][0] * kScreenSize[m_nCurrentDisplay][1] / 2;
	// 
	// 		int p = 0;
	// 		int count = kScreenSize[m_nCurrentDisplay][0] * kScreenSize[m_nCurrentDisplay][1] / 2;
	// 
	// 		while(p < count)
	// 		{
	// 			dst[p] = src[p];	p++;
	// 			dst[p] = src[p];	p++;
	// 			dst[p] = src[p];	p++;
	// 			dst[p] = src[p];	p++;
	// 		}
 		}
		else if(m_nCurrentDisplay == DISP_296X240)
		{
			u16* dst = (u16*)p1->pBmp;
			u16* src = (u16*)m_pScreenImage[m_nCurrentDisplay]->GetPixelData();

			const int kWidth = kScreenSize[m_nCurrentDisplay][0];
			const int kHeight = kScreenSize[m_nCurrentDisplay][1];

			for(int i = 0; i < kWidth; i++)
			{
				u16* s = src + i + (kHeight - 1) * kWidth;
				for(int j = 0; j < kHeight; j++)
				{
					*dst++ = *s;
					s -= kWidth;
				}
			}
		}

		//draw pixel at left-up & right-down for refresh screen
 		IDISPLAY_DrawHLine(pDisp , 0, 0, 1);
 		IDISPLAY_DrawHLine(pDisp, p1->cx-1, p1->cy-1, 1);
 		IDISPLAY_UpdateEx(pDisp, FALSE);

	#elif PLATFORM == PLATFORM_WIN32

		UpdateOrientation();

	#if D3DENABLE
		if(g_device == 0)//use d3d to refresh frame	
		{

			HDC hdc;
			g_pd3dRenderSurface->GetDC(&hdc);
			BitBlt(hdc, 0, 0, g_screenWidth, g_screenHeight, m_hDC[m_nCurrentDisplay], 0, 0, SRCCOPY);	
			g_pd3dRenderSurface->ReleaseDC(hdc);
			g_pd3dDevice->UpdateSurface(g_pd3dRenderSurface, NULL, g_pd3dBackSurface, NULL);
			g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		}
		else if(g_device == 1)//use gdiplus to refresh frame
	#endif	
		{
			HWND hwnd = g_hWnd;
			RECT rc;
			GetClientRect(hwnd, &rc);
			HDC dc = GetDC(hwnd);
			StretchBlt(dc, 0, 0, rc.right, rc.bottom, m_hDC[m_nCurrentDisplay], 0, 0, g_screenWidth, g_screenHeight, SRCCOPY);
			ReleaseDC(hwnd, dc);

			//HWND hwnd = g_hWnd;
			//HDC dc = GetDC(hwnd);
			//Bitmap* pBitmap = m_pBitmap[1];
			//BitmapData lockData;
			//Rect lockRct(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
			//pBitmap->LockBits(&lockRct, ImageLockModeWrite, PixelFormat32bppPARGB, &lockData);	
			//u32* pDest = (u32*)lockData.Scan0;
			//u16* pSrc = (u16*)m_pBits;
			//
			//for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
			//{
			//	u16 col = *pSrc++ ;
			//	*pDest++ = ((col & 0xF800) << 8) | ((col & 0x7E0) << 5) | ((col & 0x1F) << 3) | 0xFF000000;
			//}
			//pBitmap->UnlockBits(&lockData);
			//Rect clipRct;
			//Graphics g(dc);
			//g.GetVisibleClipBounds(&clipRct);	
			//g.SetInterpolationMode(InterpolationModeNearestNeighbor);
			//g.SetCompositingMode(CompositingModeSourceCopy);
			//g.DrawImage(pBitmap, clipRct, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, UnitPixel);
			//ReleaseDC(hwnd, dc);
		}
		
	#elif (PLATFORM == PLATFORM_NGAGE)

		UpdateOrientation();

		const int mode = GetDisplayMode();

		if(m_pRenderBuffer && mode >= DISP_240X320 && mode < DISP_COUNT)
		{
			//memcpy((char*)m_pRenderBuffer, m_pScreenBuffer[m_nCurrentDisplay], m_nWidth * m_nHeight * sizeof(unsigned short));

			u32* d = (u32*)m_pRenderBuffer;
			u32* s = (u32*)m_pScreenBuffer[m_nCurrentDisplay];
			u32* e = s + m_nWidth * m_nHeight / 2;

			while( s < e)
			{
				*d++ = *s++;
				*d++ = *s++;
				*d++ = *s++;
				*d++ = *s++;
			}
		}

	#elif (PLATFORM == PLATFORM_SYMBIAN)

		UpdateOrientation();

		if(m_pRenderBuffer)
		{
			if(m_nDeviceDisplayMode == EColor16MU)
			{
				char*  	dst = (char*)m_pRenderBuffer;
				u16* 	src = (u16*)m_pScreenBuffer[m_nCurrentDisplay];

				for(int i = 0; i < m_nHeight; i++)
				{
					u16* s = src;
					u16* e = s + m_nWidth;
					u32* d = (u32*)dst;

					while( s < e)
					{
						*d = ((*s & 0xF800) << 8) | ((*s & 0x7E0) << 5) | ((*s & 0x1F) << 3);
						++d;++s;
					}

					src += m_nWidth;
					dst += m_nDeviceLinePitch;
				}
			}
			else if(m_nDeviceDisplayMode == EColor64K)
			{
				char*  	dst = (char*)m_pRenderBuffer;
				u16* 	src = (u16*)m_pScreenBuffer[m_nCurrentDisplay];

				for(int i = 0; i < m_nHeight; i++)
				{
					u16* s = src;
					u16* e = s + m_nWidth;
					u16* d = (u16*)dst;

					while( s < e)
					{
						*d++ = *s++;
					}

					src += m_nWidth;
					dst += m_nDeviceLinePitch;
				}
			}
			else
				{
				debug_out("CGLLiveDisplay::Update - else");
				}
		}
	#endif

	#if CHECK_UPDATESCREEN_TIME
		m_nFrameCounter++;
		m_UpdateScreenTime = GetGlobal().m_pGame->GetCurMTime() - m_UpdateScreenTime;
	#endif

	}

	void CGLLiveDisplay::ResetScreen(int width, int height)
	{
	}

	DisplayMode CGLLiveDisplay::GetDisplayMode()
	{
	#if  SCREEN_SIZE == SCREEN_176_208
		return DISP_176X208;
	#elif  SCREEN_SIZE == SCREEN_320_320
		return DISP_320X320;
	#elif SCREEN_SIZE == SCREEN_320_480
		return DISP_480X320;
	#else
		int nOrientation = GetDeviceOrientation();

		if(nOrientation == DISP_ORIENTATION_ANGLE_0 || nOrientation == DISP_ORIENTATION_ANGLE_180)
		{
	#if SCREEN_SIZE == SCREEN_240_320
			return DISP_240X320;
	#elif SCREEN_SIZE == SCREEN_240_296
			return DISP_240X296;
	#endif
		}
		else// if(nOrientation == DISP_ORIENTATION_ANGLE_90 || nOrientation == DISP_ORIENTATION_ANGLE_270)
		{
	#if SCREEN_SIZE == SCREEN_240_320
			return DISP_320X240;
	#elif SCREEN_SIZE == SCREEN_240_296
			return DISP_296X240;
	#endif
		}
	#endif
	}

	void CGLLiveDisplay::UpdateOrientation()
	{
		m_nCurrentDisplay = GetDisplayMode();

	#if KDDI
	#else
		CGLLiveMutableImage*	pSrcImage	= GetScreenImage(m_nCurrentDisplay);
		CGLLiveMutableImage*	pDstImage	= GetScreenImage((m_nCurrentDisplay + 1) & 0x1);

		int srcWidth = kScreenSize[m_nCurrentDisplay][0];
		int srcHeight = kScreenSize[m_nCurrentDisplay][1];
		
		int nOrientation = GetDeviceOrientation();

		if(nOrientation == DISP_ORIENTATION_ANGLE_180/* || nOrientation == DISP_ORIENTATION_ANGLE_270*/)
		{
			NativePixel*	s = (NativePixel*)pSrcImage->GetPixelData();
			NativePixel*	d = s + srcWidth * srcHeight;

			for(int i = 0; i < srcHeight * srcWidth / 2; ++i)
			{
				NativePixel col = *s;
				*s = *d;
				*d = col;
				--d;++s;
			}
		}
	#endif
	}
}
