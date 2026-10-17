#ifndef _EGL_TYPES_H_
#define _EGL_TYPES_H_


typedef int		NativeDisplayType;
typedef int		NativeWindowType;
typedef int		NativePixmapType;

#define EGL_DISPLAY_FROM_SIZE( w, h )		((h << 16) | (w))

#define EGL_DEFAULT_DISPLAY		EGL_DISPLAY_FROM_SIZE( 176, 208 )

typedef int		EGLConfig;
typedef int		EGLSurface;
typedef int		EGLContext;

typedef int		EGLBoolean;
typedef int		EGLint;
typedef int		EGLDisplay;

#define EGL_NO_CONTEXT ((EGLContext)0)
#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_SURFACE ((EGLSurface)0)


#endif
