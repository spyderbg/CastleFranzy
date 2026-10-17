#ifndef __MYDISPLAY_H__
#define __MYDISPLAY_H__


#if PLATFORM == PLATFORM_WIN32 || PLATFORM == PLATFORM_WINDOWS_MOBILE
	#include "stdafx.h"
#elif PLATFORM == PLATFORM_SYMBIAN
	#include <msvids.h>
#endif

#define	DISP_ORIENTATION_ANGLE_0	0x01
#define	DISP_ORIENTATION_ANGLE_90	0x02
#define	DISP_ORIENTATION_ANGLE_180	0x04
#define	DISP_ORIENTATION_ANGLE_270	0x08


#if PLATFORM == PLATFORM_SYMBIAN_S60_2ND || SCREEN_SIZE == SCREEN_176_208

enum DisplayMode{
	DISP_176X208 = 0,
	DISP_208X176,

	DISP_COUNT
};

const int kScreenSize[DISP_COUNT][2] =
{
	{176, 208},
	{208, 176}
};
#elif SCREEN_SIZE == SCREEN_240_296
enum DisplayMode{
	DISP_240X296 = 0,
	DISP_296X240,

	DISP_COUNT
};

const int kScreenSize[DISP_COUNT][2] =
{
	{240, 376},
	{376, 240},
};

#elif SCREEN_SIZE == SCREEN_320_320
enum DisplayMode{
	DISP_320X320 = 0,
	DISP_320X320_1,

	DISP_COUNT
};

const int kScreenSize[DISP_COUNT][2] =
{
	{320, 320},
	{320, 320}
};
#elif SCREEN_SIZE == SCREEN_320_480

//iPhone only use one frame buffer in landscape mode zhong yan

enum DisplayMode{
	DISP_480X320 = 0,

	DISP_COUNT
};

const int kScreenSize[DISP_COUNT][2] =
{
	{480, 320},
};
#else

enum DisplayMode{
	DISP_240X320 = 0,
	DISP_320X240,

	DISP_COUNT
};

const int kScreenSize[DISP_COUNT][2] =
{
	{240, 320},
	{320, 240},
};

#endif

inline int OrientationToIndex(int nOrientation)
{
	if(nOrientation == DISP_ORIENTATION_ANGLE_0)
	{
		return 0;
	}
	else if(nOrientation == DISP_ORIENTATION_ANGLE_90)
	{
		return 1;
	}
	else if(nOrientation == DISP_ORIENTATION_ANGLE_180)
	{
		return 2;
	}
	else if(nOrientation == DISP_ORIENTATION_ANGLE_270)
	{
		return 3;
	}
	else
	{
		ASSERT(0);
		return 0;
	}
}

class CMutableImage;
class CDisplay
{
public:
	CDisplay();
	~CDisplay();

	bool InitializeScreen();
	void UpdateScreen(int aMode = 0);
	void ResetScreen(int width, int height);

	inline CMutableImage* GetScreenImage(int index){return m_pScreenImage[index];};
	inline int GetDeviceOrientation(){return m_deviceOrientation;};
	inline void SetDeviceOrientation(int orientation){m_deviceOrientation = orientation;};
	inline void SetRenderBuffer(void* pBuffer, int w, int h){m_pRenderBuffer = pBuffer;m_nWidth = w;m_nHeight = h;};
	DisplayMode GetDisplayMode();


#if PLATFORM == PLATFORM_WIN32
	inline HBITMAP GetHBITMAP(){return m_hBmp[m_nCurrentDisplay];};
#elif PLATFORM == PLATFORM_WINDOWS_MOBILE

	inline HBITMAP GetHBITMAP(){return m_hBmp[m_nCurrentDisplay];};
	inline void SetDeviceInfo(int nWidth, int nHeight, int nLinePitch, int nDisplayMode)
	{
		m_nDeviceWidth = nWidth;
		m_nDeviceHeight = nHeight;
		m_nDeviceLinePitch = nLinePitch;
		m_nDeviceDisplayMode = nDisplayMode;
	}

#elif PLATFORM == PLATFORM_SYMBIAN
	inline void SetDeviceInfo(int nWidth, int nHeight, int nLinePitch, int nDisplayMode)
	{
		m_nDeviceWidth = nWidth;
		m_nDeviceHeight = nHeight;
		m_nDeviceLinePitch = nLinePitch;
		m_nDeviceDisplayMode = nDisplayMode;
	}
#endif
	
private:
	void UpdateOrientation();

private:
	//0--240x320;1--320x240;2--176x208 
	int	m_deviceOrientation;

#if (PLATFORM == PLATFORM_WIN32) || ((PLATFORM == PLATFORM_MAC_OS_X) && defined(WIN32))
	
	HDC		m_hDC[DISP_COUNT];
	HBITMAP m_hBmp[DISP_COUNT];

#elif PLATFORM == PLATFORM_WINDOWS_MOBILE

	HDC		m_hDC[DISP_COUNT];
	HBITMAP m_hBmp[DISP_COUNT];

	int		m_nDeviceWidth;
	int		m_nDeviceHeight;
	int		m_nDeviceLinePitch;
	int		m_nDeviceDisplayMode;

#elif PLATFORM == PLATFORM_NGAGE
	
	char*	m_pScreenBuffer[DISP_COUNT];
	
#elif PLATFORM == PLATFORM_SYMBIAN || PLATFORM == PLATFORM_KDDI
	
	char*	m_pScreenBuffer[DISP_COUNT];
	
	int		m_nDeviceWidth;
	int		m_nDeviceHeight;
	int		m_nDeviceLinePitch;
	int		m_nDeviceDisplayMode;

#endif

	CMutableImage*	m_pScreenImage[DISP_COUNT];
	
	void*	m_pRenderBuffer;
	int		m_nWidth;
	int		m_nHeight;
	int		m_nCurrentDisplay;

#if CHECK_UPDATESCREEN_TIME
public:
	int		m_nFrameCounter;
	int		m_UpdateScreenTime;
#endif
};

#define NATIVE_COLOR_DEPTH 16


#if NATIVE_COLOR_DEPTH == 12
typedef unsigned short NativePixel;
#elif NATIVE_COLOR_DEPTH == 16
typedef unsigned short NativePixel;
#elif NATIVE_COLOR_DEPTH == 24
typedef unsigned int NativePixel;
#else
#error NATIVE_COLOR_DEPTH must be defined!
#endif

#endif
