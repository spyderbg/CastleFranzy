#ifndef __GLRENDERER_H_INCLUDED__
#define __GLRENDERER_H_INCLUDED__

#if PLATFORM == PLATFORM_MAC_OS_X
#include <OpenGLES/ES1/glext.h>
#elif IPHONE > 0
#include "../tools/rasteroid3.1/include/gles/gl.h"
#else
#include "egl.h"
#define ___EGL_ENABLE___
#endif


class CImage;
class CMutableImage;
class CGraphics;
class GL3DCamera;
class CGame;
class GlRenderer
{
public:
	GlRenderer();
	~GlRenderer();
	
	bool Init(CGame* pGame, CMutableImage* pImage);
	void Free();

	void SwapBuffers();

	CImage* GetImage();
	const CImage* GetImage() const;

	const GL3DCamera*	GetCamera() const			{return m_camera;}
	void			SetCamera(const GL3DCamera* cam)	{m_camera = cam;}

	unsigned int		GetCancelatorMask() const		{return m_cancelatorMask;}
	void			SetCancelatorMask(unsigned int m)	{m_cancelatorMask = m;}

	void			BackFaceCulling(bool) const;
	void			PerspectiveCorrection(bool) const;

	void			BeginRender();
	void			EndRender();
	void			InvertAspect(CGame* pGame, CMutableImage* pImage);

private:
#ifdef ___EGL_ENABLE___
	EGLDisplay 	m_display;
	EGLConfig 	m_config;
	EGLSurface 	m_surface;
	EGLContext	m_context;
#endif
	CGraphics*	m_graphics;
	CGame*		m_pGame;

	const GL3DCamera*	m_camera;
	unsigned int	m_cancelatorMask;
public:
	int		m_debugData;
};

#endif

