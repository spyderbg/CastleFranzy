#ifndef __C_COMMON_INCLUDES_GL_H_INCLUDED__
#define __C_COMMON_INCLUDES_GL_H_INCLUDED__

#if defined(_IRR_COMPILE_WITH_OPENGL_) || defined(_IRR_COMPILE_WITH_OPENGL_ES_) || defined(_IRR_COMPILE_WITH_PS3_)

#include "IrrCompileConfig.h"
#include "irrTypes.h"

// ----------------------------------------------------------------------------
// Select the right headers (specific to the active OpenGL platform)
// ----------------------------------------------------------------------------
#if defined(_IRR_WINDOWS_API_)
	// include windows headers for HWND
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

	#if defined(_IRR_COMPILE_WITH_OPENGL_ES_)
		#define _IRR_OPENGLES_USE_EXTPOINTER_
		#include <GLES/egl.h>
		#include <GLES/gl.h>
		#include <GLES/glext.h>
	#else //_IRR_IPHONE_EMULATION_
		#include <GL/gl.h>
		#include "glext.h"
		#ifdef _MSC_VER
			#pragma comment(lib, "OpenGL32.lib")
			#pragma comment(lib, "GLu32.lib")
		#endif
	#endif //_IRR_IPHONE_EMULATION_

// ----------------------------------------------------------------------------
#elif defined(_IRR_USE_IPHONEOS_DEVICE_)
	#include "CIrrDeviceIPhoneOS.h"

// 	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
// 		#define GL_GLEXT_LEGACY 1
// 	#endif
	#include <OpenGLES/ES1/gl.h>
	#include <OpenGLES/ES1/glext.h>
// 	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
// 		#include "glext.h"
// 	#endif

// ----------------------------------------------------------------------------
#elif defined(_IRR_USE_OSX_DEVICE_)
	#include "CIrrDeviceMacOSX.h"
	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
		#define GL_GLEXT_LEGACY 1
	#endif
	#include <OpenGL/gl.h>
	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
		#include "glext.h"
	#endif

	// Add missing calls
	#define GL_COMBINE_EXT                    0x8570
	#define GL_COMBINE_RGB_EXT                0x8571
	#define GL_COMBINE_ALPHA_EXT              0x8572
	#define GL_RGB_SCALE_EXT                  0x8573
	#define GL_ADD_SIGNED_EXT                 0x8574
	#define GL_INTERPOLATE_EXT                0x8575
	#define GL_CONSTANT_EXT                   0x8576
	#define GL_PRIMARY_COLOR_EXT              0x8577
	#define GL_PREVIOUS_EXT                   0x8578
	#define GL_SOURCE0_RGB_EXT                0x8580
	#define GL_SOURCE1_RGB_EXT                0x8581
	#define GL_SOURCE2_RGB_EXT                0x8582
	#define GL_SOURCE3_RGB_EXT                0x8583
	#define GL_SOURCE4_RGB_EXT                0x8584
	#define GL_SOURCE5_RGB_EXT                0x8585
	#define GL_SOURCE6_RGB_EXT                0x8586
	#define GL_SOURCE7_RGB_EXT                0x8587
	#define GL_SOURCE0_ALPHA_EXT              0x8588
	#define GL_SOURCE1_ALPHA_EXT              0x8589
	#define GL_SOURCE2_ALPHA_EXT              0x858A
	#define GL_SOURCE3_ALPHA_EXT              0x858B
	#define GL_SOURCE4_ALPHA_EXT              0x858C
	#define GL_SOURCE5_ALPHA_EXT              0x858D
	#define GL_SOURCE6_ALPHA_EXT              0x858E
	#define GL_SOURCE7_ALPHA_EXT              0x858F
	#define GL_OPERAND0_RGB_EXT               0x8590
	#define GL_OPERAND1_RGB_EXT               0x8591
	#define GL_OPERAND2_RGB_EXT               0x8592
	#define GL_OPERAND3_RGB_EXT               0x8593
	#define GL_OPERAND4_RGB_EXT               0x8594
	#define GL_OPERAND5_RGB_EXT               0x8595
	#define GL_OPERAND6_RGB_EXT               0x8596
	#define GL_OPERAND7_RGB_EXT               0x8597
	#define GL_OPERAND0_ALPHA_EXT             0x8598
	#define GL_OPERAND1_ALPHA_EXT             0x8599
	#define GL_OPERAND2_ALPHA_EXT             0x859A
	#define GL_OPERAND3_ALPHA_EXT             0x859B
	#define GL_OPERAND4_ALPHA_EXT             0x859C
	#define GL_OPERAND5_ALPHA_EXT             0x859D
	#define GL_OPERAND6_ALPHA_EXT             0x859E
	#define GL_OPERAND7_ALPHA_EXT             0x859F

// ----------------------------------------------------------------------------
#elif defined(_IRR_USE_PS3_DEVICE_)
	#include "CIrrDevicePS3.h"

	#include <PSGL/psgl.h>
	#include <PSGL/psglu.h>
	#include <Cg/cg.h>

// ----------------------------------------------------------------------------
#elif defined(_IRR_USE_SDL_DEVICE_)
	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
		#define GL_GLEXT_LEGACY 1
		#define GLX_GLXEXT_LEGACY 1
	#else
		#define GL_GLEXT_PROTOTYPES 1
		#define GLX_GLXEXT_PROTOTYPES 1
	#endif
	#define NO_SDL_GLEXT
	#include <SDL/SDL_opengl.h>
	#include "glext.h"

// ----------------------------------------------------------------------------
#else
	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
		#define GL_GLEXT_LEGACY 1
		#define GLX_GLXEXT_LEGACY 1
	#else
		#define GL_GLEXT_PROTOTYPES 1
		#define GLX_GLXEXT_PROTOTYPES 1
	#endif
	#include <GL/gl.h>
	#include <GL/glx.h>
	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#include "glext.h"
	#undef GLX_ARB_get_proc_address // avoid problems with local glxext.h
	#include "glxext.h"
	#endif

#endif

// ----------------------------------------------------------------------------
// Remove extension suffix to OpenGL functions for each implementation 
// and make sure to map to the right function
// ----------------------------------------------------------------------------
#if defined(_IRR_COMPILE_WITH_OPENGL_ES_)
/// Mapping to OpenGL OES Functions
	#define glMapBuffer					glMapBufferOES
	#define glUnmapBuffer				glUnmapBufferOES
	#define glGetBufferPointerv			glGetBufferPointervOES
	#define glDeleteFramebuffers		glDeleteFramebuffersOES
	#define glBindFramebuffer			glBindFramebufferOES
	#define glGenFramebuffers			glGenFramebuffersOES
	#define glCheckFramebufferStatus	glCheckFramebufferStatusOES
	#define glFramebufferRenderbuffer	glFramebufferRenderbufferOES
	#define glFramebufferTexture2D		glFramebufferTexture2DOES
	#define glBindRenderbuffer			glBindRenderbufferOES
	#define glDeleteRenderbuffers		glDeleteRenderbuffersOES
	#define glGenRenderbuffers			glGenRenderbuffersOES
	#define glRenderbufferStorage		glRenderbufferStorageOES

	#define GL_SOURCE0_ALPHA			GL_SRC0_ALPHA
	#define GL_SOURCE0_RGB				GL_SRC0_RGB
	#define GL_RENDERBUFFER				GL_RENDERBUFFER_OES
	#define GL_FRAMEBUFFER				GL_FRAMEBUFFER_OES
	#define GL_COLOR_ATTACHMENT0		GL_COLOR_ATTACHMENT0_OES
	#define GL_STENCIL_ATTACHMENT		GL_STENCIL_ATTACHMENT_OES
	#define GL_DEPTH_ATTACHMENT			GL_DEPTH_ATTACHMENT_OES
	#define GL_POINT_SPRITE				GL_POINT_SPRITE_OES
	#define GL_COORD_REPLACE			GL_COORD_REPLACE_OES
	#define GL_DEPTH_STENCIL			GL_DEPTH_STENCIL_OES
	#define GL_DEPTH_COMPONENT24		GL_DEPTH_COMPONENT24_OES
	#define GL_UNSIGNED_INT_24_8		GL_UNSIGNED_INT_24_8_OES
	#define GL_MAX_PALETTE_MATRICES		GL_MAX_PALETTE_MATRICES_OES

	#define GL_FRAMEBUFFER_COMPLETE							GL_FRAMEBUFFER_COMPLETE_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT			GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS				GL_FRAMEBUFFER_INCOMPLETE_FORMATS_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS			GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT	GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_UNSUPPORTED						GL_FRAMEBUFFER_UNSUPPORTED_OES

// ----------------------------------------------------------------------------
#elif defined(_IRR_COMPILE_WITH_PS3_)
	#define glDeleteFramebuffers		glDeleteFramebuffersOES
	#define glBindFramebuffer			glBindFramebufferOES
	#define glGenFramebuffers			glGenFramebuffersOES
	#define glCheckFramebufferStatus	glCheckFramebufferStatusOES
	#define glFramebufferRenderbuffer	glFramebufferRenderbufferOES
	#define glFramebufferTexture2D		glFramebufferTexture2DOES
	#define glBindRenderbuffer			glBindRenderbufferOES
	#define glDeleteRenderbuffers		glDeleteRenderbuffersOES
	#define glGenRenderbuffers			glGenRenderbuffersOES
	#define glRenderbufferStorage		glRenderbufferStorageOES

	#define GL_RENDERBUFFER				GL_RENDERBUFFER_OES
	#define GL_FRAMEBUFFER				GL_FRAMEBUFFER_OES
	#define GL_POINT_SPRITE				GL_POINT_SPRITE_OES
	#define GL_COORD_REPLACE			GL_COORD_REPLACE_OES
	#define GL_COLOR_ATTACHMENT0		GL_COLOR_ATTACHMENT0_EXT
	#define GL_STENCIL_ATTACHMENT		GL_STENCIL_ATTACHMENT_OES
	#define GL_DEPTH_ATTACHMENT			GL_DEPTH_ATTACHMENT_OES
	#define GL_COMPARE_R_TO_TEXTURE		GL_COMPARE_R_TO_TEXTURE_ARB

	#define GL_FRAMEBUFFER_COMPLETE							GL_FRAMEBUFFER_COMPLETE_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT			GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS				GL_FRAMEBUFFER_INCOMPLETE_FORMATS_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS			GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES
	#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT	GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES
	#define GL_FRAMEBUFFER_UNSUPPORTED						GL_FRAMEBUFFER_UNSUPPORTED_OES


// ----------------------------------------------------------------------------
#elif defined(_IRR_COMPILE_WITH_OPENGL_)
/// Mapping to OpenGL ARB Functions
//	typedef GLsizeiptrARB				GLsizeiptr;
//	typedef GLintptrARB					GLintptr;

	#define glActiveTexture				glActiveTextureARB
	#define glClientActiveTexture		glClientActiveTextureARB
	#define glGetBufferPointerv			glGetBufferPointervEXT
	#define glDeleteFramebuffers		glDeleteFramebuffersEXT
	#define glBindFramebuffer			glBindFramebufferEXT
	#define glGenFramebuffers			glGenFramebuffersEXT
	#define glCheckFramebufferStatus	glCheckFramebufferStatusEXT
	#define glFramebufferTexture2D		glFramebufferTexture2DEXT
	#define glBindRenderbuffer			glBindRenderbufferEXT
	#define glDeleteRenderbuffers		glDeleteRenderbuffersEXT
	#define glGenRenderbuffers			glGenRenderbuffersEXT
	#define glRenderbufferStorage		glRenderbufferStorageEXT

	// Map to EXT for OpenGL before 1.3 and 1.4
	// The define must be checked before setting it as it's presence depend on the
	// glext.h version included ( newer glext already define some of these )
	#ifndef GL_TEXTURE0
	#define GL_TEXTURE0				GL_TEXTURE0_ARB
	#endif
	#ifndef GL_TEXTURE1
	#define GL_TEXTURE1				GL_TEXTURE1_ARB
	#endif
	#ifndef GL_TEXTURE2
	#define GL_TEXTURE2				GL_TEXTURE2_ARB
	#endif
	#ifndef GL_TEXTURE3
	#define GL_TEXTURE3				GL_TEXTURE3_ARB
	#endif
	#ifndef GL_TEXTURE4
	#define GL_TEXTURE4				GL_TEXTURE4_ARB
	#endif
	#ifndef GL_COMBINE
	#define GL_COMBINE				GL_COMBINE_EXT
	#endif
	#ifndef GL_PRIMARY_COLOR
	#define GL_PRIMARY_COLOR		GL_PRIMARY_COLOR_EXT
	#endif
	#ifndef GL_PREVIOUS
		#ifdef GL_PREVIOUS_ARB
		#define GL_PREVIOUS			GL_PREVIOUS_ARB
		#else
		#define GL_PREVIOUS			GL_PREVIOUS_EXT
		#endif
	#endif
	#ifndef GL_INTERPOLATE
		#ifdef GL_INTERPOLATE_ARB
		#define GL_INTERPOLATE		GL_INTERPOLATE_ARB
		#else
		#define GL_INTERPOLATE		GL_INTERPOLATE_EXT
		#endif
	#endif
	#ifndef GL_ADD_SIGNED
		#ifdef GL_ADD_SIGNED_ARB
		#define GL_ADD_SIGNED		GL_ADD_SIGNED_ARB
		#else
		#define GL_ADD_SIGNED		GL_ADD_SIGNED_EXT
		#endif
	#endif
	#ifndef GL_BGRA_EXT
	#define GL_BGRA_EXT				GL_BGRA
	#endif
	#ifndef GL_RENDERBUFFER
	#define GL_RENDERBUFFER			GL_RENDERBUFFER_EXT
	#endif
	#ifndef GL_COLOR_ATTACHMENT0
	#define GL_COLOR_ATTACHMENT0	GL_COLOR_ATTACHMENT0_EXT
	#endif
	#ifndef GL_STENCIL_ATTACHMENT
	#define GL_STENCIL_ATTACHMENT	GL_STENCIL_ATTACHMENT_EXT
	#endif
	#ifndef GL_DEPTH_ATTACHMENT
	#define GL_DEPTH_ATTACHMENT		GL_DEPTH_ATTACHMENT_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_COMPLETE
	#define GL_FRAMEBUFFER_COMPLETE							GL_FRAMEBUFFER_COMPLETE_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
	#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT			GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_INCOMPLETE_FORMATS
	#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS				GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
	#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS			GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
	#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT	GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT
	#endif
	#ifndef GL_FRAMEBUFFER_UNSUPPORTED
	#define GL_FRAMEBUFFER_UNSUPPORTED						GL_FRAMEBUFFER_UNSUPPORTED_EXT
	#endif
	#ifndef GL_POINT_SPRITE
	#define GL_POINT_SPRITE									GL_POINT_SPRITE_ARB
	#endif
	#ifndef GL_POINT_DISTANCE_ATTENUATION
	#define GL_POINT_DISTANCE_ATTENUATION					GL_POINT_DISTANCE_ATTENUATION_ARB
	#endif
	#ifndef GL_POINT_SIZE_MAX
	#define GL_POINT_SIZE_MAX								GL_POINT_SIZE_MAX_ARB
	#endif
	#ifndef GL_POINT_FADE_THRESHOLD_SIZE
	#define GL_POINT_FADE_THRESHOLD_SIZE					GL_POINT_FADE_THRESHOLD_SIZE_ARB
	#endif
	#ifndef GL_DEPTH_STENCIL
	#define GL_DEPTH_STENCIL								GL_DEPTH_STENCIL_EXT
	#endif
	#ifndef GL_UNSIGNED_INT_24_8
	#define GL_UNSIGNED_INT_24_8_EXT						GL_UNSIGNED_INT_24_8_EXT
	#endif
	#ifndef GL_MAX_PALETTE_MATRICES
	#define GL_MAX_PALETTE_MATRICES							GL_MAX_PALETTE_MATRICES_ARB
	#endif

// ----------------------------------------------------------------------------
#endif // _IRR_COMPILE_WITH_OPENGL_ / _IRR_COMPILE_WITH_OPENGL_ES_

#endif // _IRR_COMPILE_WITH_OPENGL_ || _IRR_COMPILE_WITH_OPENGL_ES_
#endif // __C_COMMON_INCLUDES_GL_H_INCLUDED__
