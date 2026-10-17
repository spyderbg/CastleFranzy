// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_COMMON_GL_FEATURE_MAP_H_INCLUDED__
#define __C_COMMON_GL_FEATURE_MAP_H_INCLUDED__

#include "IrrCompileConfig.h"
#if defined(_IRR_COMPILE_WITH_OPENGL_) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)

#include "CCommonGLHeaders.h"
#include "CCommonGLExtensionList.h"
#include "EDriverFeatures.h"
#include "irros.h"

#pragma warning( disable : 4530)
#include <bitset>

namespace irr
{
namespace video
{

class CCommonGLExtensionHandler
{
public:
	// constructor
	CCommonGLExtensionHandler();

	// deferred initialization
	void initExtensions(bool stencilBuffer);

	//! queries the features of the driver, returns true if feature is available
	bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

	//! queries the features of the driver, returns true if feature is available
	bool queryOpenGLFeature(EOpenGLFeatures feature) const
	{
		return FeatureAvailable[feature];
	}

	// Some non-boolean properties
	u32 getMaxTextureUnits() {return MaxTextureUnits;}
	GLint getMaxLights() {return MaxLights;}
	GLint getMaxIndices() {return MaxIndices;}
	f32 getMaxAnisotropy() {return MaxAnisotropy;}
	u32 getMaxUserClipPlanes() {return MaxUserClipPlanes;}
	u32 getMaxPaletteMatrices() {return MaxPaletteMatrices;}
	u32 getMaxVertexUnits() {return MaxVertexUnits;}
	u32 getVersion() {return Version;}
	u32 getShaderLanguageVersion() {return ShaderLanguageVersion;}

	void setMaxTextureUnits(u32 value) {MaxTextureUnits = value;}
	void setMaxLights(GLint value) {MaxLights = value;}
	void setMaxIndices(GLint value) {MaxIndices = value;}
	void setMaxAnisotropy(f32 value) {MaxAnisotropy = value;}
	void setMaxUserClipPlanes(u32 value) {MaxUserClipPlanes = value;}
	void setMaxPaletteMatrices(u32 value) {MaxPaletteMatrices = value;}
	void setMaxVertexUnits(u32 value) {MaxVertexUnits = value;}
	void setVersion(u32 value) {Version = value;}
	void setShaderLanguageVersion(u32 value) {ShaderLanguageVersion = value;}

	//! show all features with availablity
	void dump() const;

	// public access to the (loaded) extensions.
	// general functions
	void activeTexture(GLenum texture);
	void clientActiveTexture(GLenum texture);
	GLenum getActiveTexture();
		
	void pointParameterf(GLint loc, GLfloat f);
	void pointParameterfv(GLint loc, const GLfloat *v);

	void compressedTexImage2D(GLenum target, GLint level,
		GLenum internalformat, GLsizei width, GLsizei height,
		GLint border, GLsizei imageSize, const void* data);

#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
	void stencilFuncSeparate (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
	void stencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass);

	// shader programming
	void genPrograms(GLsizei n, GLuint *programs);
	void bindProgram(GLenum target, GLuint program);
	void programString(GLenum target, GLenum format, GLsizei len, const GLvoid *string);
	void deletePrograms(GLsizei n, const GLuint *programs);
	void programLocalParameter4fv(GLenum, GLuint, const GLfloat *);
	GLhandleARB createShaderObject(GLenum shaderType);
	void shaderSource(GLhandleARB shader, int numOfStrings, const char **strings, int *lenOfStrings);
	void compileShader(GLhandleARB shader);
	GLhandleARB createProgramObject(void);
	void attachObject(GLhandleARB program, GLhandleARB shader);
	void linkProgram(GLhandleARB program);
	void useProgramObject(GLhandleARB prog);
	void deleteObject(GLhandleARB object);
	void getInfoLog(GLhandleARB object, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
	void getObjectParameteriv(GLhandleARB object, GLenum type, int *param);
	GLint getUniformLocation(GLhandleARB program, const char *name);
	void uniform4fv(GLint location, GLsizei count, const GLfloat *v);
	void uniform1iv(GLint loc, GLsizei count, const GLint *v);
	void uniform1i(GLint loc, const GLint v);
	void uniform1fv(GLint loc, GLsizei count, const GLfloat *v);
	void uniform2fv(GLint loc, GLsizei count, const GLfloat *v);
	void uniform3fv(GLint loc, GLsizei count, const GLfloat *v);
	void uniformMatrix2fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
	void uniformMatrix3fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
	void uniformMatrix4fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v);
	void getActiveUniform(GLhandleARB program, GLuint index, GLsizei maxlength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
#endif

	// framebuffer objects
	void bindFramebuffer(GLenum target, GLuint framebuffer);
	void deleteFramebuffers(GLsizei n, const GLuint *framebuffers);
	void genFramebuffers(GLsizei n, GLuint *framebuffers);
	GLenum checkFramebufferStatus(GLenum target);
	void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void bindRenderbuffer(GLenum target, GLuint renderbuffer);
	void deleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
	void genRenderbuffers(GLsizei n, GLuint *renderbuffers);
	void renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	void activeStencilFace(GLenum face);

	// vertex buffer object
	void genBuffers(GLsizei n, GLuint *buffers);
	void bindBuffer(GLenum target, GLuint buffer);
	void bufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
	void deleteBuffers(GLsizei n, const GLuint *buffers);
	void bufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
	void getBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
#endif
	void *mapBuffer (GLenum target, GLenum access);
	GLboolean unmapBuffer (GLenum target);
	GLboolean isBuffer (GLuint buffer);
	void getBufferParameteriv (GLenum target, GLenum pname, GLint *params);
	void getBufferPointerv (GLenum target, GLenum pname, GLvoid **params);

protected:
	// Some variables for properties
	// Flags that cans be turned to constant on OpenGL ES
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	static const bool MultiTextureExtension;
	static const bool TextureCompressionExtension;
#else
	bool MultiTextureExtension;
	bool TextureCompressionExtension;
#endif
	bool StencilBuffer;
	bool MultiSamplingExtension;
	bool AnisotropyExtension;
	
	// Some non-boolean properties
	//! Maxmimum texture layers supported by the fixed pipeline
	u32 MaxTextureUnits;
	//! Maximum hardware lights supported
	GLint MaxLights;
	//! Optimal number of indices per meshbuffer
	GLint MaxIndices;
	//! Maximal Anisotropy
	f32 MaxAnisotropy;
	//! Number of user clipplanes
	u32 MaxUserClipPlanes;
	//! Maximum number of palette matrices
	u32 MaxPaletteMatrices;
	//! Maximum number of vertex units
	u32 MaxVertexUnits;


	//! OpenGL version as Integer: 100*Major+Minor, i.e. 2.1 becomes 201
	u32 Version;
	//! GLSL version as Integer: 100*Major+Minor
	u32 ShaderLanguageVersion;


#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	//! Implements a state shadow layer for caching some render states.
	struct SExtensionShadowState
	{
		// Add more as needed

		GLenum ActiveTexture;
		GLenum ClientActiveTexture;

		SExtensionShadowState();

	} ExtensionShadowState;
#endif

	// the global feature array
	std::bitset<IRR_OpenGL_Feature_Count> FeatureAvailable;

#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	PFNGLACTIVETEXTUREARBPROC pGlActiveTextureARB;
	PFNGLCLIENTACTIVETEXTUREARBPROC	pGlClientActiveTextureARB;
	PFNGLGENPROGRAMSARBPROC pGlGenProgramsARB;
	PFNGLBINDPROGRAMARBPROC pGlBindProgramARB;
	PFNGLPROGRAMSTRINGARBPROC pGlProgramStringARB;
	PFNGLDELETEPROGRAMSNVPROC pGlDeleteProgramsARB;
	PFNGLPROGRAMLOCALPARAMETER4FVARBPROC pGlProgramLocalParameter4fvARB;
	PFNGLCREATESHADEROBJECTARBPROC pGlCreateShaderObjectARB;
	PFNGLSHADERSOURCEARBPROC pGlShaderSourceARB;
	PFNGLCOMPILESHADERARBPROC pGlCompileShaderARB;
	PFNGLCREATEPROGRAMOBJECTARBPROC pGlCreateProgramObjectARB;
	PFNGLATTACHOBJECTARBPROC pGlAttachObjectARB;
	PFNGLLINKPROGRAMARBPROC pGlLinkProgramARB;
	PFNGLUSEPROGRAMOBJECTARBPROC pGlUseProgramObjectARB;
	PFNGLDELETEOBJECTARBPROC pGlDeleteObjectARB;
	PFNGLGETINFOLOGARBPROC pGlGetInfoLogARB;
	PFNGLGETOBJECTPARAMETERIVARBPROC pGlGetObjectParameterivARB;
	PFNGLGETUNIFORMLOCATIONARBPROC pGlGetUniformLocationARB;
	PFNGLUNIFORM1IVARBPROC pGlUniform1ivARB;
	PFNGLUNIFORM1IARBPROC pGlUniform1iARB;
	PFNGLUNIFORM1FVARBPROC pGlUniform1fvARB;
	PFNGLUNIFORM2FVARBPROC pGlUniform2fvARB;
	PFNGLUNIFORM3FVARBPROC pGlUniform3fvARB;
	PFNGLUNIFORM4FVARBPROC pGlUniform4fvARB;
	PFNGLUNIFORMMATRIX2FVARBPROC pGlUniformMatrix2fvARB;
	PFNGLUNIFORMMATRIX3FVARBPROC pGlUniformMatrix3fvARB;
	PFNGLUNIFORMMATRIX4FVARBPROC pGlUniformMatrix4fvARB;
	PFNGLGETACTIVEUNIFORMARBPROC pGlGetActiveUniformARB;
	PFNGLPOINTPARAMETERFARBPROC  pGlPointParameterfARB;
	PFNGLPOINTPARAMETERFVARBPROC pGlPointParameterfvARB;
	PFNGLSTENCILFUNCSEPARATEPROC pGlStencilFuncSeparate;
	PFNGLSTENCILOPSEPARATEPROC pGlStencilOpSeparate;
	PFNGLSTENCILFUNCSEPARATEATIPROC pGlStencilFuncSeparateATI;
	PFNGLSTENCILOPSEPARATEATIPROC pGlStencilOpSeparateATI;
	PFNGLCOMPRESSEDTEXIMAGE2DPROC pGlCompressedTexImage2D;
#    ifdef _IRR_WINDOWS_API_
	typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;
#    elif defined(_IRR_LINUX_PLATFORM_) && defined(GLX_SGI_swap_control)
	PFNGLXSWAPINTERVALSGIPROC glxSwapIntervalSGI;
#    endif
	PFNGLBINDFRAMEBUFFEREXTPROC pGlBindFramebufferEXT;
	PFNGLDELETEFRAMEBUFFERSEXTPROC pGlDeleteFramebuffersEXT;
	PFNGLGENFRAMEBUFFERSEXTPROC pGlGenFramebuffersEXT;
	PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC pGlCheckFramebufferStatusEXT;
	PFNGLFRAMEBUFFERTEXTURE2DEXTPROC pGlFramebufferTexture2DEXT;
	PFNGLBINDRENDERBUFFEREXTPROC pGlBindRenderbufferEXT;
	PFNGLDELETERENDERBUFFERSEXTPROC pGlDeleteRenderbuffersEXT;
	PFNGLGENRENDERBUFFERSEXTPROC pGlGenRenderbuffersEXT;
	PFNGLRENDERBUFFERSTORAGEEXTPROC pGlRenderbufferStorageEXT;
	PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC pGlFramebufferRenderbufferEXT;
	PFNGLACTIVESTENCILFACEEXTPROC pGlActiveStencilFaceEXT;
	PFNGLGENBUFFERSARBPROC pGlGenBuffersARB;
	PFNGLBINDBUFFERARBPROC pGlBindBufferARB;
	PFNGLBUFFERDATAARBPROC pGlBufferDataARB;
	PFNGLDELETEBUFFERSARBPROC pGlDeleteBuffersARB;
	PFNGLBUFFERSUBDATAARBPROC pGlBufferSubDataARB;
	PFNGLGETBUFFERSUBDATAARBPROC pGlGetBufferSubDataARB;
	PFNGLMAPBUFFERARBPROC pGlMapBufferARB;
	PFNGLUNMAPBUFFERARBPROC pGlUnmapBufferARB;
	PFNGLISBUFFERARBPROC pGlIsBufferARB;
	PFNGLGETBUFFERPARAMETERIVARBPROC pGlGetBufferParameterivARB;
	PFNGLGETBUFFERPOINTERVARBPROC pGlGetBufferPointervARB;
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	PFNGLBLENDEQUATIONSEPARATEOESPROC pGlBlendEquationSeparateOES;
	PFNGLBLENDFUNCSEPARATEOESPROC pGlBlendFuncSeparateOES;
	PFNGLISRENDERBUFFEROESPROC pGlIsRenderbufferOES;
	PFNGLBINDRENDERBUFFEROESPROC pGlBindRenderbufferOES;
	PFNGLDELETERENDERBUFFERSOESPROC pGlDeleteRenderbuffersOES;
	PFNGLGENRENDERBUFFERSOESPROC pGlGenRenderbuffersOES;
	PFNGLRENDERBUFFERSTORAGEOESPROC pGlRenderbufferStorageOES;
	PFNGLGETRENDERBUFFERPARAMETERIVOESPROC pGlGetRenderbufferParameterivOES;
	PFNGLISFRAMEBUFFEROESPROC pGlIsFramebufferOES;
	PFNGLBINDFRAMEBUFFEROESPROC pGlBindFramebufferOES;
	PFNGLDELETEFRAMEBUFFERSOESPROC pGlDeleteFramebuffersOES;
	PFNGLGENFRAMEBUFFERSOESPROC pGlGenFramebuffersOES;
	PFNGLCHECKFRAMEBUFFERSTATUSOESPROC pGlCheckFramebufferStatusOES;
	PFNGLFRAMEBUFFERRENDERBUFFEROESPROC pGlFramebufferRenderbufferOES;
	PFNGLFRAMEBUFFERTEXTURE2DOESPROC pGlFramebufferTexture2DOES;
	PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC pGlGetFramebufferAttachmentParameterivOES;
	PFNGLGENERATEMIPMAPOESPROC pGlGenerateMipmapOES;
	PFNGLMAPBUFFEROESPROC pGlMapBufferOES;
	PFNGLUNMAPBUFFEROESPROC pGlUnmapBufferOES;
	PFNGLGETBUFFERPOINTERVOESPROC pGlGetBufferPointervOES;
#endif
};

inline void CCommonGLExtensionHandler::activeTexture(GLenum texture)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (MultiTextureExtension && pGlActiveTextureARB
#    ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		&& texture != ExtensionShadowState.ActiveTexture
#    endif
	)
	{
		pGlActiveTextureARB(texture);
#    ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ExtensionShadowState.ActiveTexture = texture;
#    endif
	}
#else
	if (MultiTextureExtension
#    ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		&& texture != ExtensionShadowState.ActiveTexture
#    endif
	)
	{
		glActiveTexture(texture);
#    ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ExtensionShadowState.ActiveTexture = texture;
#    endif
	}
#endif
}

inline void CCommonGLExtensionHandler::clientActiveTexture(GLenum texture)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (MultiTextureExtension && pGlClientActiveTextureARB
#    ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		&& texture != ExtensionShadowState.ClientActiveTexture
#    endif
	)
	{
		pGlClientActiveTextureARB(texture);
#    ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ExtensionShadowState.ClientActiveTexture = texture;
#    endif
	}
#else
	if (MultiTextureExtension
#    ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		&& texture != ExtensionShadowState.ClientActiveTexture
#    endif
	)
	{
		glClientActiveTexture(texture);
#    ifdef _IRR_OPENGL_USE_SHADOW_STATE_
		ExtensionShadowState.ClientActiveTexture = texture;
#    endif
	}
#endif
}

inline GLenum CCommonGLExtensionHandler::getActiveTexture()
{
#ifdef _IRR_OPENGL_USE_SHADOW_STATE_
	return ExtensionShadowState.ClientActiveTexture;
#else
	return 0;
#endif
}

inline void CCommonGLExtensionHandler::pointParameterf(GLint loc, GLfloat f)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlPointParameterfARB)
		pGlPointParameterfARB(loc, f);
#elif defined(GL_ARB_point_parameters) || defined(_IRR_COMPILE_WITH_OPENGL_ES_)
	glPointParameterf(loc, f);
#elif defined(_DEBUG)
	os::Printer::log("glPointParameterf not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::pointParameterfv(GLint loc, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlPointParameterfvARB)
		pGlPointParameterfvARB(loc, v);
#elif defined(GL_ARB_point_parameters)
	glPointParameterfv(loc, v);
#elif defined(_DEBUG)
	os::Printer::log("glPointParameterfv not supported", ELL_ERROR);
#endif
}


inline void CCommonGLExtensionHandler::compressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width,
		GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlCompressedTexImage2D)
		pGlCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
#elif defined(GL_ARB_texture_compression) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
#elif defined(_DEBUG)
	os::Printer::log("glCompressedTexImage2D not supported", ELL_ERROR);
#endif
}


#if !defined(_IRR_COMPILE_WITH_OPENGL_ES_) && !defined(_IRR_COMPILE_WITH_PS3_)
inline void CCommonGLExtensionHandler::genPrograms(GLsizei n, GLuint *programs)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGenProgramsARB)
		pGlGenProgramsARB(n, programs);
#elif defined(GL_ARB_vertex_program)
	glGenProgramsARB(n,programs);
#elif defined(_DEBUG)
	os::Printer::log("glGenPrograms not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::bindProgram(GLenum target, GLuint program)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBindProgramARB)
		pGlBindProgramARB(target, program);
#elif defined(GL_ARB_vertex_program)
	glBindProgramARB(target, program);
#elif defined(_DEBUG)
	os::Printer::log("glBindProgram not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::programString(GLenum target, GLenum format, GLsizei len, const GLvoid *string)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlProgramStringARB)
		pGlProgramStringARB(target, format, len, string);
#elif defined(GL_ARB_vertex_program)
	glProgramStringARB(target,format,len,string);
#elif defined(_DEBUG)
	os::Printer::log("glProgramString not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::deletePrograms(GLsizei n, const GLuint *programs)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteProgramsARB)
		pGlDeleteProgramsARB(n, programs);
#elif defined(GL_ARB_vertex_program)
	glDeleteProgramsARB(n,programs);
#elif defined(_DEBUG)
	os::Printer::log("glDeletePrograms not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::programLocalParameter4fv(GLenum n, GLuint i, const GLfloat * f)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlProgramLocalParameter4fvARB)
		pGlProgramLocalParameter4fvARB(n,i,f);
#elif defined(GL_ARB_vertex_program)
	glProgramLocalParameter4fvARB(n,i,f);
#elif defined(_DEBUG)
	os::Printer::log("glProgramLocalParameter4fv not supported", ELL_ERROR);
#endif
}

inline GLhandleARB CCommonGLExtensionHandler::createShaderObject(GLenum shaderType)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlCreateShaderObjectARB)
		return pGlCreateShaderObjectARB(shaderType);
#elif defined(GL_ARB_shader_objects)
	return glCreateShaderObjectARB(shaderType);
#elif defined(_DEBUG)
	os::Printer::log("glCreateShaderObject not supported", ELL_ERROR);
#endif
	return 0;
}

inline void CCommonGLExtensionHandler::shaderSource(GLhandleARB shader, int numOfStrings, const char **strings, int *lenOfStrings)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlShaderSourceARB)
		pGlShaderSourceARB(shader, numOfStrings, strings, lenOfStrings);
#elif defined(GL_ARB_shader_objects)
	glShaderSourceARB(shader, numOfStrings, strings, (GLint *)lenOfStrings);
#elif defined(_DEBUG)
	os::Printer::log("glShaderSource not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::compileShader(GLhandleARB shader)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlCompileShaderARB)
		pGlCompileShaderARB(shader);
#elif defined(GL_ARB_shader_objects)
	glCompileShaderARB(shader);
#elif defined(_DEBUG)
	os::Printer::log("glCompileShader not supported", ELL_ERROR);
#endif
}

inline GLhandleARB CCommonGLExtensionHandler::createProgramObject(void)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlCreateProgramObjectARB)
		return pGlCreateProgramObjectARB();
#elif defined(GL_ARB_shader_objects)
	return glCreateProgramObjectARB();
#elif defined(_DEBUG)
	os::Printer::log("glCreateProgramObject not supported", ELL_ERROR);
#endif
	return 0;
}

inline void CCommonGLExtensionHandler::attachObject(GLhandleARB program, GLhandleARB shader)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlAttachObjectARB)
		pGlAttachObjectARB(program, shader);
#elif defined(GL_ARB_shader_objects)
	glAttachObjectARB(program, shader);
#elif defined(_DEBUG)
	os::Printer::log("glAttachObject not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::linkProgram(GLhandleARB program)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlLinkProgramARB)
		pGlLinkProgramARB(program);
#elif defined(GL_ARB_shader_objects)
	glLinkProgramARB(program);
#elif defined(_DEBUG)
	os::Printer::log("glLinkProgram not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::useProgramObject(GLhandleARB prog)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUseProgramObjectARB)
		pGlUseProgramObjectARB(prog);
#elif defined(GL_ARB_shader_objects)
	glUseProgramObjectARB(prog);
#elif defined(_DEBUG)
	os::Printer::log("glUseProgramObject not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::deleteObject(GLhandleARB object)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteObjectARB)
		pGlDeleteObjectARB(object);
#elif defined(GL_ARB_shader_objects)
	glDeleteObjectARB(object);
#elif defined(_DEBUG)
	os::Printer::log("gldeleteObject not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::getInfoLog(GLhandleARB object, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetInfoLogARB)
		pGlGetInfoLogARB(object, maxLength, length, infoLog);
#elif defined(GL_ARB_shader_objects)
	glGetInfoLogARB(object, maxLength, length, infoLog);
#elif defined(_DEBUG)
	os::Printer::log("glGetInfoLog not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::getObjectParameteriv(GLhandleARB object, GLenum type, int *param)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetObjectParameterivARB)
		pGlGetObjectParameterivARB(object, type, param);
#elif defined(GL_ARB_shader_objects)
	glGetObjectParameterivARB(object, type, (GLint *)param);
#elif defined(_DEBUG)
	os::Printer::log("glGetObjectParameteriv not supported", ELL_ERROR);
#endif
}

inline GLint CCommonGLExtensionHandler::getUniformLocation(GLhandleARB program, const char *name)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetUniformLocationARB)
		return pGlGetUniformLocationARB(program, name);
#elif defined(GL_ARB_shader_objects)
	return glGetUniformLocationARB(program, name);
#elif defined(_DEBUG)
	os::Printer::log("glGetUniformLocation not supported", ELL_ERROR);
#endif
	return 0;
}

inline void CCommonGLExtensionHandler::uniform4fv(GLint location, GLsizei count, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform4fvARB)
		pGlUniform4fvARB(location, count, v);
#elif defined(GL_ARB_shader_objects)
	glUniform4fvARB(location, count, v);
#elif defined(_DEBUG)
	os::Printer::log("glUniform4fv not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::uniform1iv(GLint loc, GLsizei count, const GLint *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform1ivARB)
		pGlUniform1ivARB(loc, count, v);
#elif defined(GL_ARB_shader_objects)
	glUniform1ivARB(loc, count, v);
#elif defined(_DEBUG)
	os::Printer::log("glUniform1iv not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::uniform1i(GLint loc, const GLint v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform1iARB)
		pGlUniform1iARB(loc, v);
#elif defined(GL_ARB_shader_objects)
	glUniform1ivARB(loc, count, v);
#elif defined(_DEBUG)
	os::Printer::log("glUniform1i not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::uniform1fv(GLint loc, GLsizei count, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform1fvARB)
		pGlUniform1fvARB(loc, count, v);
#elif defined(GL_ARB_shader_objects)
	glUniform1fvARB(loc, count, v);
#elif defined(_DEBUG)
	os::Printer::log("glUniform1fv not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::uniform2fv(GLint loc, GLsizei count, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform2fvARB)
		pGlUniform2fvARB(loc, count, v);
#elif defined(GL_ARB_shader_objects)
	glUniform2fvARB(loc, count, v);
#elif defined(_DEBUG)
	os::Printer::log("glUniform2fv not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::uniform3fv(GLint loc, GLsizei count, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniform3fvARB)
		pGlUniform3fvARB(loc, count, v);
#elif defined(GL_ARB_shader_objects)
	glUniform3fvARB(loc, count, v);
#elif defined(_DEBUG)
	os::Printer::log("glUniform3fv not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::uniformMatrix2fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniformMatrix2fvARB)
		pGlUniformMatrix2fvARB(loc, count, transpose, v);
#elif defined(GL_ARB_shader_objects)
	glUniformMatrix2fvARB(loc, count, transpose, v);
#elif defined(_DEBUG)
	os::Printer::log("glUniformMatrix2fv not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::uniformMatrix3fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniformMatrix3fvARB)
		pGlUniformMatrix3fvARB(loc, count, transpose, v);
#elif defined(GL_ARB_shader_objects)
	glUniformMatrix3fvARB(loc, count, transpose, v);
#elif defined(_DEBUG)
	os::Printer::log("glUniformMatrix3fv not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::uniformMatrix4fv(GLint loc, GLsizei count, GLboolean transpose, const GLfloat *v)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUniformMatrix4fvARB)
		pGlUniformMatrix4fvARB(loc, count, transpose, v);
#elif defined(GL_ARB_shader_objects)
	glUniformMatrix4fvARB(loc, count, transpose, v);
#elif defined(_DEBUG)
	os::Printer::log("glUniformMatrix4fv not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::getActiveUniform(GLhandleARB program,
		GLuint index, GLsizei maxlength, GLsizei *length,
		GLint *size, GLenum *type, GLcharARB *name)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetActiveUniformARB)
		pGlGetActiveUniformARB(program, index, maxlength, length, size, type, name);
#elif defined(GL_ARB_shader_objects)
	glGetActiveUniformARB(program, index, maxlength, length, size, type, name);
#elif defined(_DEBUG)
	os::Printer::log("glGetActiveUniform not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::stencilFuncSeparate (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlStencilFuncSeparate)
		pGlStencilFuncSeparate(frontfunc, backfunc, ref, mask);
	else if (pGlStencilFuncSeparateATI)
		pGlStencilFuncSeparateATI(frontfunc, backfunc, ref, mask);
#elif defined(GL_VERSION_2_0)
	glStencilFuncSeparate(frontfunc, backfunc, ref, mask);
#elif defined(GL_ATI_separate_stencil)
	glStencilFuncSeparateATI(frontfunc, backfunc, ref, mask);
#elif defined(_DEBUG)
	os::Printer::log("glStencilFuncSeparate not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::stencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlStencilOpSeparate)
		pGlStencilOpSeparate(face, fail, zfail, zpass);
	else if (pGlStencilOpSeparateATI)
		pGlStencilOpSeparateATI(face, fail, zfail, zpass);
#elif defined(GL_VERSION_2_0)
	glStencilOpSeparate(face, fail, zfail, zpass);
#elif defined(GL_ATI_separate_stencil)
	glStencilOpSeparateATI(face, fail, zfail, zpass);
#elif defined(_DEBUG)
	os::Printer::log("glStencilOpSeparate not supported", ELL_ERROR);
#endif
}
#endif

inline void CCommonGLExtensionHandler::bindFramebuffer(GLenum target, GLuint framebuffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBindFramebufferEXT)
		pGlBindFramebufferEXT(target, framebuffer);
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlBindFramebufferOES)
		pGlBindFramebufferOES(target, framebuffer);
#elif defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object)
	glBindFramebuffer(target, framebuffer);
#elif defined(_DEBUG)
	os::Printer::log("glBindFramebuffer not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::deleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteFramebuffersEXT)
		pGlDeleteFramebuffersEXT(n, framebuffers);
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlDeleteFramebuffersOES)
		pGlDeleteFramebuffersOES(n, framebuffers);
#elif defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object)
	glDeleteFramebuffers(n, framebuffers);
#elif defined(_DEBUG)
	os::Printer::log("glDeleteFramebuffers not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::genFramebuffers(GLsizei n, GLuint *framebuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGenFramebuffersEXT)
		pGlGenFramebuffersEXT(n, framebuffers);
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlGenFramebuffersOES)
		pGlGenFramebuffersOES(n, framebuffers);
#elif defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object)
	glGenFramebuffers(n, framebuffers);
#elif defined(_DEBUG)
	os::Printer::log("glGenFramebuffers not supported", ELL_ERROR);
#endif
}

inline GLenum CCommonGLExtensionHandler::checkFramebufferStatus(GLenum target)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlCheckFramebufferStatusEXT)
		return pGlCheckFramebufferStatusEXT(target);
	else
		return 0;
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlCheckFramebufferStatusOES)
		return pGlCheckFramebufferStatusOES(target);
	else
		return 0;
#elif defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object)
	return glCheckFramebufferStatus(target);
#elif defined(_DEBUG)
	os::Printer::log("glCheckFramebufferStatus not supported", ELL_ERROR);
#endif
	return 0;
}

inline void CCommonGLExtensionHandler::framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlFramebufferTexture2DEXT)
		pGlFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlFramebufferTexture2DOES)
		pGlFramebufferTexture2DOES(target, attachment, textarget, texture, level);
#elif defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object)
	glFramebufferTexture2D(target, attachment, textarget, texture, level);
#elif defined(_DEBUG)
	os::Printer::log("glFramebufferTexture2D not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::bindRenderbuffer(GLenum target, GLuint renderbuffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBindRenderbufferEXT)
		pGlBindRenderbufferEXT(target, renderbuffer);
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlBindRenderbufferOES)
		pGlBindRenderbufferOES(target, renderbuffer);
#elif defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object)
	glBindRenderbuffer(target, renderbuffer);
#elif defined(_DEBUG)
	os::Printer::log("glBindRenderbuffer not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::deleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteRenderbuffersEXT)
		pGlDeleteRenderbuffersEXT(n, renderbuffers);
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlDeleteRenderbuffersOES)
		pGlDeleteRenderbuffersOES(n, renderbuffers);
#elif defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object)
	glDeleteRenderbuffers(n, renderbuffers);
#elif defined(_DEBUG)
	os::Printer::log("glDeleteRenderbuffers not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::genRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGenRenderbuffersEXT)
		pGlGenRenderbuffersEXT(n, renderbuffers);
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlGenRenderbuffersOES)
		pGlGenRenderbuffersOES(n, renderbuffers);
#elif defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object)
	glGenRenderbuffers(n, renderbuffers);
#elif defined(_DEBUG)
	os::Printer::log("glGenRenderbuffers not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlRenderbufferStorageEXT)
		pGlRenderbufferStorageEXT(target, internalformat, width, height);
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlRenderbufferStorageOES)
		pGlRenderbufferStorageOES(target, internalformat, width, height);
#elif defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object)
	glRenderbufferStorage(target, internalformat, width, height);
#elif defined(_DEBUG)
	os::Printer::log("glRenderbufferStorage not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlFramebufferRenderbufferEXT)
		pGlFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlFramebufferRenderbufferOES)
		pGlFramebufferRenderbufferOES(target, attachment, renderbuffertarget, renderbuffer);
#elif defined(GL_EXT_framebuffer_object) || defined(GL_OES_framebuffer_object)
	glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
#elif defined(_DEBUG)
	os::Printer::log("glFramebufferRenderbuffer not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::activeStencilFace(GLenum face)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlActiveStencilFaceEXT)
		pGlActiveStencilFaceEXT(face);
#elif defined(GL_EXT_stencil_two_side)
	glActiveStencilFaceEXT(face);
#elif defined(_DEBUG)
	os::Printer::log("glActiveStencilFace not supported", ELL_ERROR);
#endif
}


inline void CCommonGLExtensionHandler::genBuffers(GLsizei n, GLuint *buffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGenBuffersARB)
		pGlGenBuffersARB(n, buffers);
#elif defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	glGenBuffers(n, buffers);
#elif defined(_DEBUG)
	os::Printer::log("glGenBuffers not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::bindBuffer(GLenum target, GLuint buffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBindBufferARB)
		pGlBindBufferARB(target, buffer);
#elif defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	glBindBuffer(target, buffer);
#elif defined(_DEBUG)
	os::Printer::log("glBindBuffer not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::bufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBufferDataARB)
		pGlBufferDataARB(target, size, data, usage);
#elif defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	glBufferData(target, size, data, usage);
#elif defined(_DEBUG)
	os::Printer::log("glBufferData not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::deleteBuffers(GLsizei n, const GLuint *buffers)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlDeleteBuffersARB)
		pGlDeleteBuffersARB(n, buffers);
#elif defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	glDeleteBuffers(n, buffers);
#elif defined(_DEBUG)
	os::Printer::log("glDeleteBuffers not supported", ELL_ERROR);
#endif
}

inline void CCommonGLExtensionHandler::bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlBufferSubDataARB)
		pGlBufferSubDataARB(target, offset, size, data);
#elif defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	glBufferSubData(target, offset, size, data);
#elif defined(_DEBUG)
	os::Printer::log("glBufferSubData not supported", ELL_ERROR);
#endif
}

#ifndef _IRR_COMPILE_WITH_OPENGL_ES_
inline void CCommonGLExtensionHandler::getBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetBufferSubDataARB)
		pGlGetBufferSubDataARB(target, offset, size, data);
#elif defined(GL_ARB_vertex_buffer_object)
	glGetBufferSubData(target, offset, size, data);
#elif defined(_DEBUG)
	os::Printer::log("glGetBufferSubData not supported", ELL_ERROR);
#endif
}
#endif

#if !defined(_IRR_IPHONE_EMULATION_)
inline void *CCommonGLExtensionHandler::mapBuffer(GLenum target, GLenum access)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlMapBufferARB)
		return pGlMapBufferARB(target, access);
	return 0;
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlMapBufferOES)
		return pGlMapBufferOES(target, access);
	return 0;
#elif defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	return glMapBuffer(target, access);
#elif defined(_DEBUG)
	os::Printer::log("glMapBuffer not supported", ELL_ERROR);
#endif
	return 0;
}

inline GLboolean CCommonGLExtensionHandler::unmapBuffer(GLenum target)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlUnmapBufferARB)
		return pGlUnmapBufferARB(target);
	return false;
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlUnmapBufferOES)
		return pGlUnmapBufferOES(target);
	return false;
#elif defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	return glUnmapBuffer(target);
#elif defined(_DEBUG)
	os::Printer::log("glUnmapBuffer not supported", ELL_ERROR);
#endif
	return false;
}
#endif //!_IRR_IPHONE_EMULATION_

inline GLboolean CCommonGLExtensionHandler::isBuffer (GLuint buffer)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlIsBufferARB)
		return pGlIsBufferARB(buffer);
	return false;
#elif defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) 
	return glIsBuffer(buffer);
#elif defined(_DEBUG)
	os::Printer::log("glDeleteBuffers not supported", ELL_ERROR);
#endif
	return false;
}

inline void CCommonGLExtensionHandler::getBufferParameteriv (GLenum target, GLenum pname, GLint *params)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetBufferParameterivARB)
		pGlGetBufferParameterivARB(target, pname, params);
#elif defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)
	glGetBufferParameteriv(target, pname, params);
#elif defined(_DEBUG)
	os::Printer::log("glGetBufferParameteriv not supported", ELL_ERROR);
#endif
}

#if !defined(_IRR_IPHONE_EMULATION_)
inline void CCommonGLExtensionHandler::getBufferPointerv (GLenum target, GLenum pname, GLvoid **params)
{
#ifdef _IRR_OPENGL_USE_EXTPOINTER_
	if (pGlGetBufferPointervARB)
		pGlGetBufferPointervARB(target, pname, params);
#elif defined(_IRR_OPENGLES_USE_EXTPOINTER_)
	if (pGlGetBufferPointervOES)
		pGlGetBufferPointervOES(target, pname, params);
#elif defined(GL_ARB_vertex_buffer_object) || defined(_IRR_COMPILE_WITH_OPENGL_ES_)
	glGetBufferPointerv(target, pname, params);
#elif defined(_DEBUG)
	os::Printer::log("glGetBufferPointerv not supported", ELL_ERROR);
#endif
}
#endif //!_IRR_IPHONE_EMULATION_

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_ || _IRR_COMPILE_WITH_OPENGL_ES_
#endif // __C_COMMON_GL_FEATURE_MAP_H_INCLUDED__
