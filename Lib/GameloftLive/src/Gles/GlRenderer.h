#ifndef __CGLLiveGlRenderer_H_INCLUDED__
#define __CGLLiveGlRenderer_H_INCLUDED__

#if PLATFORM == PLATFORM_MAC_OS_X
#include <OpenGLES/ES1/glext.h>
#elif IPHONE > 0
#include "gl.h"
#else
#include "egl.h"
#define ___EGL_ENABLE___
#endif

#include "Image.h"
using namespace gllive;

//class CGLLiveImage;
//class CGLLiveMutableImage;
//class CGLLiveGraphics;
class CGLLiveGL3DCamera;
class CGLLive;
class CGLLiveGlRenderer
{
public:
	CGLLiveGlRenderer();
	~CGLLiveGlRenderer();
	
	bool Init(CGLLive* pGame, CGLLiveMutableImage* pImage);
	void Free();

	void SwapBuffers();

	CGLLiveImage* GetImage();
	const CGLLiveImage* GetImage() const;

	const CGLLiveGL3DCamera*	GetCamera() const			{return m_camera;}
	void			SetCamera(const CGLLiveGL3DCamera* cam)	{m_camera = cam;}

	unsigned int		GetCancelatorMask() const		{return m_cancelatorMask;}
	void			SetCancelatorMask(unsigned int m)	{m_cancelatorMask = m;}

	void			BackFaceCulling(bool) const;
	void			PerspectiveCorrection(bool) const;

	void			BeginRender();
	void			EndRender();
	void			InvertAspect(CGLLive* pGame, CGLLiveMutableImage* pImage);

private:
#ifdef ___EGL_ENABLE___
	EGLDisplay 	m_display;
	EGLConfig 	m_config;
	EGLSurface 	m_surface;
	EGLContext	m_context;
#endif
	CGLLiveGraphics*	m_graphics;
	CGLLive*		m_pGame;

	const CGLLiveGL3DCamera*	m_camera;
	unsigned int	m_cancelatorMask;
public:
	int		m_debugData;
};

#endif

