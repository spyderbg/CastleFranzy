#include "GLLiveStdafx.h"
#include "gll.h"
//#include "egl.h"
#include "glt_math_fixed.h"

#include "GlRenderer.h"
#include "GL3DCamera.h"
#include "MyGraphics.h"
#include "GLLiveGame.h"

const int kStatckAllocationmaxSize = 16 * 1024;

#ifdef WIN32

	class StackSizeMonitor
	{
	public:
		StackSizeMonitor():m_max(0){}
		~StackSizeMonitor()
		{
			//FILE* f = fopen("d:\\stackMax.txt","wt");			
			//if(f)
			//{
			//	fprintf(f,"max = %d\n",m_max);
			//	fclose(f);
			//}
		}

		void Set(int a)
		{
			if(a>m_max)
				m_max=a;
		}

	private:
		int m_max;
	};

	StackSizeMonitor	gStackSizemonitor;

	#define MONITOR_STACK_SIZE(x) gStackSizemonitor.Set(x)
#else
	#define MONITOR_STACK_SIZE(x) 
#endif


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CGLLiveGlRenderer::CGLLiveGlRenderer() :
	m_graphics(0),
	m_cancelatorMask(0)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CGLLiveGlRenderer::~CGLLiveGlRenderer()
{
	Free();
}

CGLLiveImage* CGLLiveGlRenderer::GetImage()
{
	return m_graphics->GetImage();
}

const CGLLiveImage* CGLLiveGlRenderer::GetImage() const
{
	return m_graphics->GetImage();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void* GetIDisplay();
void* GetScreenPtr();

bool CGLLiveGlRenderer::Init(CGLLive* pGame, CGLLiveMutableImage* pImage)
{
	m_pGame = pGame;

#if IPHONE > 0
	int viewportW = GL_VIEWPORT_WIDTH;
	int viewportH = GL_VIEWPORT_HEIGHT;
#else
	int viewportW = m_pGame->GetViewportWidth();
	int viewportH = m_pGame->GetViewportHeight();
#endif
	

#ifdef ___EGL_ENABLE___
	//EGL Init
	m_display = eglGetDisplay(EGL_DISPLAY_FROM_SIZE(viewportW, viewportH));

	GLL_ASSERT(m_display != EGL_NO_DISPLAY);

	EGLint major = 0;
	EGLint minor = 0;
	EGLBoolean result = eglInitialize(m_display, &major, &minor);

	GLL_ASSERT(result);
	
	const EGLint numConfigs = 10;
	EGLConfig configs[numConfigs];

	EGLint attribs[5] = {EGL_DEPTH_SIZE,16,0,0,0};

	EGLint nconfig;

	result = eglChooseConfig(m_display, attribs, (EGLConfig*)&configs, numConfigs, &nconfig);
	
	GLL_ASSERT(result);
	GLL_ASSERT(nconfig > 0);

	m_surface = eglCreateWindowSurface( m_display, configs[0], (NativeWindowType)(pImage->GetPixelData() + (m_pGame->GetViewportY()) * pImage->GetPitch()), 0);

	int error = eglGetError();

	GLL_ASSERT(m_surface != EGL_NO_SURFACE);

	m_context = eglCreateContext(m_display, m_config, 0, 0); 
	
	GLL_ASSERT(m_context != EGL_NO_CONTEXT);

	result = eglMakeCurrent(m_display, m_surface, m_surface, m_context);
	
	error = eglGetError();

	GLL_ASSERT(result);
#else //PLATFORM_MAC_OS_X

#endif //___EGL_ENABLE___
	// Smooth shading
	glShadeModel( GL_SMOOTH );

	// Depth Test
	glDisable( GL_DEPTH_TEST );

	BackFaceCulling(true);

	// Perspective Correction
	PerspectiveCorrection(true);

	// GL Initialization
	glViewport( 0, 0, viewportW, viewportH);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	
#if PLATFORM == PLATFORM_MAC_OS_X	
	glMatrixMode(GL_MODELVIEW);
	
	for(int i = 0; i < 100; i++)
	{
		glPushMatrix();
	}
	
	for(int i = 0; i < 100; i++)
	{
		glPopMatrix();
	}
#endif
	
	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CGLLiveGlRenderer::Free()
{
#ifdef ___EGL_ENABLE___
	EGLBoolean result;
	GLenum error;

	result = eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	error = eglGetError();
	GLL_ASSERT(result == EGL_TRUE);

	if (m_context)
	{
		result = eglDestroyContext(m_display, m_context);
		error = eglGetError();
		GLL_ASSERT(result == EGL_TRUE);
		m_context = 0;
	}
	
	if (m_surface)
	{
		result = eglDestroySurface(m_display, m_surface);
		error = eglGetError();
		GLL_ASSERT(result == EGL_TRUE);
		m_surface = 0;
	}
	
	if (m_display)
	{
		result = eglTerminate(m_display);
		error = eglGetError();
		GLL_ASSERT(result == EGL_TRUE);
		m_display = 0;
	}
#endif //___EGL_ENABLE___
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CGLLiveGlRenderer::SwapBuffers()
{
#ifdef ___EGL_ENABLE___
	eglWaitGL();
	eglSwapBuffers(m_display, m_surface);
#endif //___EGL_ENABLE___
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#if defined(_WINDOWS) && !defined(WIPI)
extern bool gDisablebackFaceCulling;
bool gDisablebackFaceCulling=false;
#endif

void CGLLiveGlRenderer::BackFaceCulling(bool c) const
{
#if defined(_WINDOWS) && !defined(WIPI)
	if(gDisablebackFaceCulling)
	{
		glDisable(GL_CULL_FACE);
		return;
	}
#endif
	if(c)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CGLLiveGlRenderer::PerspectiveCorrection(bool c) const
{
	if(c)
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	else
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
}

void CGLLiveGlRenderer::BeginRender()
{
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	m_camera->ApplyTransform();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
}

void CGLLiveGlRenderer::EndRender()
{
	glFlush();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CGLLiveGlRenderer::InvertAspect(CGLLive* pGame, CGLLiveMutableImage* pImage)
{
	Free();
	Init(pGame, pImage);
}
