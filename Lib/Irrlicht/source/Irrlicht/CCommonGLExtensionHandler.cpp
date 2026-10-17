#include "stdafx.h"
// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#if defined(_IRR_COMPILE_WITH_OPENGL_) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)

#include "CCommonGLExtensionHandler.h"
#include "irrString.h"
#include "SMaterial.h" // for MATERIAL_MAX_TEXTURES
#include "fast_atof.h"

namespace irr
{
namespace video
{

// Flags that cans be turned to constant on OpenGL ES (compilation optimization)
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
const bool CCommonGLExtensionHandler::MultiTextureExtension = true;
const bool CCommonGLExtensionHandler::TextureCompressionExtension = false;
#endif

#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
CCommonGLExtensionHandler::SExtensionShadowState::SExtensionShadowState()
	: ActiveTexture(GL_TEXTURE0),
	  ClientActiveTexture(GL_TEXTURE0)
{
}
#endif

CCommonGLExtensionHandler::CCommonGLExtensionHandler()
	: StencilBuffer(false)
#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	, MultiTextureExtension(false)
	, TextureCompressionExtension(false)
#endif
	, MultiSamplingExtension(false)
	, AnisotropyExtension(false)
	, MaxTextureUnits(1)
	, MaxLights(1)
	, MaxIndices(65535)
	, MaxAnisotropy(1.0f)
	, MaxUserClipPlanes(0)
	, Version(0)
	, ShaderLanguageVersion(0)
#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	, pGlActiveTextureARB(0)
	, pGlClientActiveTextureARB(0)
	, pGlGenProgramsARB(0)
	, pGlBindProgramARB(0)
	, pGlProgramStringARB(0)
	, pGlDeleteProgramsARB(0)
	, pGlProgramLocalParameter4fvARB(0)
	, pGlCreateShaderObjectARB(0)
	, pGlShaderSourceARB(0)
	, pGlCompileShaderARB(0)
	, pGlCreateProgramObjectARB(0)
	, pGlAttachObjectARB(0)
	, pGlLinkProgramARB(0)
	, pGlUseProgramObjectARB(0)
	, pGlDeleteObjectARB(0)
	, pGlGetObjectParameterivARB(0)
	, pGlGetUniformLocationARB(0)
	, pGlUniform1ivARB(0)
	, pGlUniform1iARB(0)
	, pGlUniform1fvARB(0)
	, pGlUniform2fvARB(0)
	, pGlUniform3fvARB(0)
	, pGlUniform4fvARB(0)
	, pGlUniformMatrix2fvARB(0)
	, pGlUniformMatrix3fvARB(0)
	, pGlUniformMatrix4fvARB(0)
	, pGlGetActiveUniformARB(0)
	, pGlPointParameterfARB(0)
	, pGlPointParameterfvARB(0)
	, pGlStencilFuncSeparate(0)
	, pGlStencilOpSeparate(0)
	, pGlStencilFuncSeparateATI(0)
	, pGlStencilOpSeparateATI(0)
	, pGlCompressedTexImage2D(0)
#    ifdef _IRR_USE_WINDOWS_DEVICE_
	, wglSwapIntervalEXT(0)
#    elif defined(GLX_SGI_swap_control)
	, glxSwapIntervalSGI(0)
#    endif
	, pGlGenBuffersARB(0)
	, pGlBindBufferARB(0)
	, pGlBufferDataARB(0)
	, pGlDeleteBuffersARB(0)
	, pGlBufferSubDataARB(0)
	, pGlGetBufferSubDataARB(0)
	, pGlBindFramebufferEXT(0)
	, pGlDeleteFramebuffersEXT(0)
	, pGlGenFramebuffersEXT(0)
	, pGlCheckFramebufferStatusEXT(0)
	, pGlFramebufferTexture2DEXT(0)
	, pGlBindRenderbufferEXT(0)
	, pGlDeleteRenderbuffersEXT(0)
	, pGlGenRenderbuffersEXT(0)
	, pGlRenderbufferStorageEXT(0)
	, pGlFramebufferRenderbufferEXT(0)
	, pGlMapBufferARB(0)
	, pGlUnmapBufferARB(0)
	, pGlIsBufferARB(0)
	, pGlGetBufferParameterivARB(0)
	, pGlGetBufferPointervARB(0)
#elif defined(_IRR_OPENGL_USE_EXTPOINTER_)
	, pGlBlendEquationSeparateOES(0)
	, pGlBlendFuncSeparateOES(0)
	, pGlIsRenderbufferOES(0)
	, glBindRenderbufferOES(0)
	, glDeleteRenderbuffersOES(0)
	, glGenRenderbuffersOES(0)
	, glRenderbufferStorageOES(0)
	, glGetRenderbufferParameterivOES(0)
	, glIsFramebufferOES(0)
	, glBindFramebufferOES(0)
	, glDeleteFramebuffersOES(0)
	, glGenFramebuffersOES(0)
	, glCheckFramebufferStatusOES(0)
	, glFramebufferRenderbufferOES(0)
	, glFramebufferTexture2DOES(0)
	, glGetFramebufferAttachmentParameterivOES(0)
	, glGenerateMipmapOES(0)
	, glMapBufferOES(0)
	, glUnmapBufferOES(0)
	, glGetBufferPointervOES(0)
#endif // _IRR_OPENGL_USE_EXTPOINTER_
{
	for (u32 i=0; i<IRR_OpenGL_Feature_Count; ++i)
		FeatureAvailable[i]=false;
}


void CCommonGLExtensionHandler::dump() const
{
	for (u32 i=0; i<IRR_OpenGL_Feature_Count; ++i)
		os::Printer::log(OpenGLFeatureStrings[i], FeatureAvailable[i]?" true":" false");
}

void CCommonGLExtensionHandler::initExtensions(bool stencilBuffer)
{
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	core::stringc ver = reinterpret_cast<const c8*>(glGetString(GL_VERSION));
	int dot = ver.findFirst('.');
	if(dot>0)
	{
		int end = ver.findNext(' ', dot);
		if(end>dot)
			ver = ver.subString(dot-1,end-dot+1);
		else
			ver = ver.subString(dot-1,ver.size()-dot+1);
	}
	const f32 ogl_ver = core::fast_atof(ver.c_str());
	Version = core::floor32(ogl_ver)*100+core::ceil32(core::fract(ogl_ver)*10.0f);
	
	if ( Version >= 101)
		os::Printer::log("OpenGL|ES driver version is 1.1 or better.", ELL_INFORMATION);
	else
		os::Printer::log("OpenGL|ES driver version is not 1.1 or better.", ELL_WARNING);
#else
	const f32 ogl_ver = core::fast_atof(reinterpret_cast<const c8*>(glGetString(GL_VERSION)));
	Version = core::floor32(ogl_ver)*100+core::ceil32(core::fract(ogl_ver)*10.0f);
	if ( Version >= 102)
		os::Printer::log("OpenGL driver version is 1.2 or better.", ELL_INFORMATION);
	else
		os::Printer::log("OpenGL driver version is not 1.2 or better.", ELL_WARNING);
#endif
	
	{
		const char* t = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
		const size_t len = strlen(t);
		c8 *str = irrnew c8[len+1];
		c8* p = str;

		for (size_t i=0; i<len; ++i)
		{
			str[i] = static_cast<char>(t[i]);

			if (str[i] == ' ')
			{
				str[i] = 0;
				for (u32 j=0; j<IRR_OpenGL_Feature_Count; ++j)
				{
					if (!strcmp(OpenGLFeatureStrings[j], p))
					{
						FeatureAvailable[j] = true;
						break;
					}
				}

				p = p + strlen(p) + 1;
			}
		}

		delete [] str;
	}

	
#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	MultiTextureExtension = FeatureAvailable[IRR_ARB_multitexture];
	TextureCompressionExtension = FeatureAvailable[IRR_ARB_texture_compression];
#endif
	MultiSamplingExtension = FeatureAvailable[IRR_ARB_multisample];
	AnisotropyExtension = FeatureAvailable[IRR_EXT_texture_filter_anisotropic];
	StencilBuffer=stencilBuffer;

#ifdef _IRR_OPENGL_USE_EXTPOINTER_
#    ifdef _IRR_WINDOWS_API_
	// vsync extension
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC) wglGetProcAddress("wglSwapIntervalEXT");
#        define IRR_OGL_LOAD_EXTENSION(x) wglGetProcAddress(x)
#        define GLubyte char
#    elif defined(_IRR_USE_LINUX_DEVICE_) || defined (_IRR_USE_SDL_DEVICE_)
#        ifdef _IRR_USE_SDL_DEVICE_
#            define IRR_OGL_LOAD_EXTENSION(x) SDL_GL_GetProcAddress(reinterpret_cast<const char*>(x))
#        else
	// Accessing the correct function is quite complex
	// All libraries should support the ARB version, however
	// since GLX 1.4 the non-ARB version is the official one
	// So we have to check the runtime environment and
	// choose the proper symbol
	// In case you still have problems please enable the
	// next line by uncommenting it
	// #define _IRR_GETPROCADDRESS_WORKAROUND_

#            ifndef _IRR_GETPROCADDRESS_WORKAROUND_
	__GLXextFuncPtr (*IRR_OGL_LOAD_EXTENSION)(const GLubyte*)=0;
#                ifdef GLX_VERSION_1_4
		int major,minor;
		glXQueryVersion(glXGetCurrentDisplay(), &major, &minor);
		if ((major>1) || (minor>3))
			IRR_OGL_LOAD_EXTENSION=glXGetProcAddress;
		else
#                endif
			IRR_OGL_LOAD_EXTENSION=glXGetProcAddressARB;
#            else
#                define IRR_OGL_LOAD_EXTENSION glXGetProcAddressARB
#            endif
#        endif
#    endif

	// get multitexturing function pointers
	pGlActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glActiveTextureARB"));
	pGlClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glClientActiveTextureARB"));

	// get fragment and vertex program function pointers
	pGlGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenProgramsARB"));
	pGlBindProgramARB = (PFNGLBINDPROGRAMARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindProgramARB"));
	pGlProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProgramStringARB"));
	pGlDeleteProgramsARB = (PFNGLDELETEPROGRAMSNVPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteProgramsARB"));
	pGlProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glProgramLocalParameter4fvARB"));
	pGlCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCreateShaderObjectARB"));
	pGlShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glShaderSourceARB"));
	pGlCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCompileShaderARB"));
	pGlCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCreateProgramObjectARB"));
	pGlAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glAttachObjectARB"));
	pGlLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glLinkProgramARB"));
	pGlUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUseProgramObjectARB"));
	pGlDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteObjectARB"));
	pGlGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetInfoLogARB"));
	pGlGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetObjectParameterivARB"));
	pGlGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetUniformLocationARB"));
	pGlUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform4fvARB"));
	pGlUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform1ivARB"));
	pGlUniform1iARB = (PFNGLUNIFORM1IARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform1iARB"));
	pGlUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform1fvARB"));
	pGlUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform2fvARB"));
	pGlUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform3fvARB"));
	pGlUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniform4fvARB"));
	pGlUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniformMatrix2fvARB"));
	pGlUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniformMatrix3fvARB"));
	pGlUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUniformMatrix4fvARB"));
	pGlGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetActiveUniformARB"));

	// get point parameter extension
	pGlPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glPointParameterfARB"));
	pGlPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glPointParameterfvARB"));

	// get stencil extension
	pGlStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glStencilFuncSeparate"));
	pGlStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glStencilOpSeparate"));
	pGlStencilFuncSeparateATI = (PFNGLSTENCILFUNCSEPARATEATIPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glStencilFuncSeparateATI"));
	pGlStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glStencilOpSeparateATI"));

	// compressed textures
	pGlCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCompressedTexImage2D"));

#    if defined(GLX_SGI_swap_control) && !defined(_IRR_USE_SDL_DEVICE_)
	// get vsync extension
	glxSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glXSwapIntervalSGI"));
#    endif

	// FrameBufferObjects
	pGlBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindFramebufferEXT"));
	pGlDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteFramebuffersEXT"));
	pGlGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenFramebuffersEXT"));
	pGlCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glCheckFramebufferStatusEXT"));
	pGlFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glFramebufferTexture2DEXT"));
	pGlBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindRenderbufferEXT"));
	pGlDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteRenderbuffersEXT"));
	pGlGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenRenderbuffersEXT"));
	pGlRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glRenderbufferStorageEXT"));
	pGlFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glFramebufferRenderbufferEXT"));
	pGlGenBuffersARB = (PFNGLGENBUFFERSARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGenBuffersARB"));
	pGlBindBufferARB = (PFNGLBINDBUFFERARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBindBufferARB"));
	pGlBufferDataARB = (PFNGLBUFFERDATAARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBufferDataARB"));
	pGlDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glDeleteBuffersARB"));
	pGlBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glBufferSubDataARB"));
	pGlGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetBufferSubDataARB"));
	pGlMapBufferARB = (PFNGLMAPBUFFERARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glMapBufferARB"));
	pGlUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glUnmapBufferARB"));
	pGlIsBufferARB = (PFNGLISBUFFERARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glIsBufferARB"));
	pGlGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetBufferParameterivARB"));
	pGlGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)
		IRR_OGL_LOAD_EXTENSION(reinterpret_cast<const GLubyte*>("glGetBufferPointervARB"));

#    ifdef _IRR_WINDOWS_API_
#        undef GLubyte
#    endif
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)

	pGlBlendEquationSeparateOES = (PFNGLBLENDEQUATIONSEPARATEOESPROC)
		eglGetProcAddress("glBlendEquationSeparateOES");
	pGlBlendFuncSeparateOES = (PFNGLBLENDFUNCSEPARATEOESPROC)
		eglGetProcAddress("glBlendFuncSeparateOES");
	pGlIsRenderbufferOES = (PFNGLISRENDERBUFFEROESPROC)
		eglGetProcAddress("glIsRenderbufferOES");
	pGlBindRenderbufferOES = (PFNGLBINDRENDERBUFFEROESPROC)
		eglGetProcAddress("glBindRenderbufferOES");
	pGlDeleteRenderbuffersOES = (PFNGLDELETERENDERBUFFERSOESPROC)
		eglGetProcAddress("glDeleteRenderbuffersOES");
	pGlGenRenderbuffersOES = (PFNGLGENRENDERBUFFERSOESPROC)
		eglGetProcAddress("glGenRenderbuffersOES");
	pGlRenderbufferStorageOES = (PFNGLRENDERBUFFERSTORAGEOESPROC)
		eglGetProcAddress("glRenderbufferStorageOES");
	pGlGetRenderbufferParameterivOES = (PFNGLGETRENDERBUFFERPARAMETERIVOESPROC)
		eglGetProcAddress("glGetRenderbufferParameterivOES");
	pGlIsFramebufferOES = (PFNGLISFRAMEBUFFEROESPROC)
		eglGetProcAddress("glIsFramebufferOES");
	pGlBindFramebufferOES = (PFNGLBINDFRAMEBUFFEROESPROC)
		eglGetProcAddress("glBindFramebufferOES");
	pGlDeleteFramebuffersOES = (PFNGLDELETEFRAMEBUFFERSOESPROC)
		eglGetProcAddress("glDeleteFramebuffersOES");
	pGlGenFramebuffersOES = (PFNGLGENFRAMEBUFFERSOESPROC)
		eglGetProcAddress("glGenFramebuffersOES");
	pGlCheckFramebufferStatusOES = (PFNGLCHECKFRAMEBUFFERSTATUSOESPROC)
		eglGetProcAddress("glCheckFramebufferStatusOES");
	pGlFramebufferRenderbufferOES = (PFNGLFRAMEBUFFERRENDERBUFFEROESPROC)
		eglGetProcAddress("glFramebufferRenderbufferOES");
	pGlFramebufferTexture2DOES = (PFNGLFRAMEBUFFERTEXTURE2DOESPROC)
		eglGetProcAddress("glFramebufferTexture2DOES");
	pGlGetFramebufferAttachmentParameterivOES = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC)
		eglGetProcAddress("glGetFramebufferAttachmentParameterivOES");
	pGlGenerateMipmapOES = (PFNGLGENERATEMIPMAPOESPROC)
		eglGetProcAddress("glGenerateMipmapOES");
	pGlMapBufferOES = (PFNGLMAPBUFFEROESPROC)
		eglGetProcAddress("glMapBufferOES");
	pGlUnmapBufferOES = (PFNGLUNMAPBUFFEROESPROC)
		eglGetProcAddress("glUnmapBufferOES");
	pGlGetBufferPointervOES = (PFNGLGETBUFFERPOINTERVOESPROC)
		eglGetProcAddress("glGetBufferPointervOES");
#endif

	GLint num;
	// set some properties
#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	#if defined(GL_ARB_multitexture) || defined(GL_VERSION_1_3)
	if (Version>102 || FeatureAvailable[IRR_ARB_multitexture])
	{
		glGetIntegerv(GL_MAX_TEXTURE_UNITS, &num);
		MaxTextureUnits=num;
	}
	#endif
#else
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &num);
	MaxTextureUnits=core::min_((u32)num, MATERIAL_MAX_TEXTURES);
#endif

	glGetIntegerv(GL_MAX_LIGHTS, &MaxLights);
	
#ifdef GL_EXT_texture_filter_anisotropic
	if (FeatureAvailable[IRR_EXT_texture_filter_anisotropic])
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAnisotropy);
#endif
		
#ifdef GL_OES_matrix_palette
	if (FeatureAvailable[IRR_OES_matrix_palette])
	{
		glGetIntegerv(GL_MAX_PALETTE_MATRICES, &num);
		MaxPaletteMatrices = num;
		glGetIntegerv(GL_MAX_VERTEX_UNITS_OES, &num);
		MaxVertexUnits = num;
	}
	else
	{
		MaxPaletteMatrices = 0;
		MaxVertexUnits = 0;
	}
#elif defined(GL_ARB_matrix_palette) && defined(GL_ARB_vertex_blend)
	if (FeatureAvailable[IRR_ARB_matrix_palette])
	{
		glGetIntegerv(GL_MAX_PALETTE_MATRICES_ARB, &num);
		MaxPaletteMatrices = num;
	}
	else
	{
		MaxPaletteMatrices = 0;
	}
	if (FeatureAvailable[IRR_ARB_vertex_blend])
	{
		glGetIntegerv(GL_MAX_VERTEX_UNITS_ARB, &num);
		MaxVertexUnits = num;
	}
	else
	{
		MaxVertexUnits = 0;
	}
#endif	

#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	#ifdef GL_VERSION_1_2
	if (Version>101)
		glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &MaxIndices);
	#endif
	glGetIntegerv(GL_MAX_CLIP_PLANES, reinterpret_cast<GLint*>(&MaxUserClipPlanes));

	#if defined(GL_ARB_shading_language_100) || defined (GL_VERSION_2_0)
	if (FeatureAvailable[IRR_ARB_shading_language_100] || Version>=200)
	{
		glGetError(); // clean error buffer
	#ifdef GL_SHADING_LANGUAGE_VERSION
		const GLubyte* shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	#else
		const GLubyte* shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION_ARB);
	#endif
		if (glGetError() == GL_INVALID_ENUM)
			ShaderLanguageVersion = 100;
		else
		{
			const f32 sl_ver = core::fast_atof(reinterpret_cast<const c8*>(shaderVersion));
			ShaderLanguageVersion = core::floor32(sl_ver)*100+core::ceil32(core::fract(sl_ver)*10.0f);
		}
	}
	#endif

	#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (!pGlActiveTextureARB || !pGlClientActiveTextureARB)
	{
		MultiTextureExtension = false;
		os::Printer::log("Failed to load OpenGL's multitexture extension, proceeding without.", ELL_WARNING);
	}
	else
	#endif

	#if !defined(_IRR_COMPILE_WITH_PS3_)
	if (MaxTextureUnits < 2)
	{
		MultiTextureExtension = false;
		os::Printer::log("Warning: OpenGL device only has one texture unit. Disabling multitexturing.", ELL_WARNING);
	}
	MaxTextureUnits = core::min_(MaxTextureUnits,MATERIAL_MAX_TEXTURES);
	#endif
#endif
}

bool CCommonGLExtensionHandler::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
{
	switch (feature)
	{
#if defined(_IRR_USE_PS3_DEVICE_)
	case EVDF_MULTITEXTURE:
		return MultiTextureExtension;
	case EVDF_BILINEAR_FILTER:
	case EVDF_MIP_MAP:
	case EVDF_TEXTURE_NPOT:
	case EVDF_TEXTURE_NSQUARE:
	case EVDF_FRAMEBUFFER_OBJECT:
	case EVDF_VERTEX_BUFFER_OBJECT:
	case EVDF_STENCIL_BUFFER:
		return true;
#else

	case EVDF_MULTITEXTURE:
		return MultiTextureExtension;
	case EVDF_BILINEAR_FILTER:
	case EVDF_MIP_MAP:
	case EVDF_RENDER_TO_TARGET:
		return true;
	case EVDF_MIP_MAP_AUTO_UPDATE:
#    ifdef _IRR_COMPILE_WITH_OPENGL_ES_
		return true;
#    else
		return FeatureAvailable[IRR_SGIS_generate_mipmap];
#    endif
#    ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	case EVDF_STENCIL_BUFFER:
		return StencilBuffer;
	case EVDF_ARB_VERTEX_PROGRAM_1:
		return FeatureAvailable[IRR_ARB_vertex_program];
	case EVDF_ARB_FRAGMENT_PROGRAM_1:
		return FeatureAvailable[IRR_ARB_fragment_program];
	case EVDF_ARB_GLSL:
		return (FeatureAvailable[IRR_ARB_shading_language_100]||Version>=200);
	case EVDF_TEXTURE_NSQUARE:
		return true; // non-square is always supported
#    endif
	case EVDF_TEXTURE_NPOT:
		// Some ATI cards seem to have only SW support in OpenGL 2.0
		// drivers if the extension is not exposed, so we skip this
		// extra test for now!
		// return (FeatureAvailable[IRR_ARB_texture_non_power_of_two]||Version>=200);
		return FeatureAvailable[IRR_ARB_texture_non_power_of_two] || FeatureAvailable[IRR_OES_texture_npot];
	case EVDF_FRAMEBUFFER_OBJECT:
		return FeatureAvailable[IRR_EXT_framebuffer_object] || FeatureAvailable[IRR_OES_framebuffer_object];
	case EVDF_VERTEX_BUFFER_OBJECT:
#    ifdef _IRR_COMPILE_WITH_OPENGL_ES_
		return true;
#    else
		return FeatureAvailable[IRR_ARB_vertex_buffer_object];
#    endif
#endif
	default:
		return false;
	};
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_ || _IRR_COMPILE_WITH_OPENGL_ES_

