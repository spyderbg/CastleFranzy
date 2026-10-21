#ifndef IO_AUDIO_AUDIOFILES_H
#define IO_AUDIO_AUDIOFILES_H

//#define DEBUG_AUDIO_SYSTEM


//#define FORCE_STEREO_AUDIO
#define USE_SOUND_LOCALIZATION

#define USE_DECODER_ADPCM			(0)
#define USE_DECODER_MPC				(0)
#define USE_DECODER_MSADPCM			(1)
#define USE_DECODER_PCM				(1)

#if USE_DECODER_MSADPCM || USE_DECODER_PCM
#	define USE_DECODER_WAV			(1)
#else
#	define USE_DECODER_WAV			(0)
#endif

#ifndef gll_delete
#define gll_delete delete
#endif

#ifndef gll_new
#define gll_new new
#endif

#ifdef DEBUG_AUDIO_SYSTEM
#	if WIN32
#		include <assert.h>
#		define GLL_ASSERT(arg1)			assert(arg1)
#	else
#		include <stdio.h>
#		define GLL_ASSERT(arg1)			if (!(arg1)) { printf("assert %s failed in %s(%d) %p\n", #arg1, __FILE__, __LINE__, this); }
#	endif
#else
#	include <stdio.h>
#	define GLL_ASSERT(arg1)
#endif 

//#ifndef SAFE_DEL
//#define SAFE_DEL gll_delete
//#endif
//
//#ifndef SAFE_DEL_ARRAY
//#define SAFE_DEL_ARRAY gll_delete []
//#endif

namespace gameloft
{
	namespace audio
	{
		typedef unsigned char u8;
		typedef unsigned short u16;
		typedef unsigned int u32;

		typedef unsigned long long u64;

		typedef signed char s8;
		typedef signed short int s16;
		typedef signed int s32;

		static_assert(sizeof(u32) == 4 && sizeof(s32) == 4, "Audio word sizes changed");
		typedef u64 OSTick;



		enum ESoundType
		{
			// type
			k_typeMemory       = 0x00000001,
			k_typeStreamedFile = 0x00000002,

			// file format
		#if USE_DECODER_MPC
			k_typeMPC		= 0x00010000,
		#endif //USE_DECODER_MPC
		#if USE_DECODER_ADPCM
			k_typeADPCM		= 0x00020000,
		#endif //USE_DECODER_ADPCM
		#if USE_DECODER_WAV
			k_typeWAV		= 0x00040000,
		#endif //USE_DECODER_WAV

			// flags
			k_typePreloadData        = 0x00100000,
			k_typeKeepDataLoaded     = 0x00200000,
			k_typeKeepInstanceLoaded = 0x00400000,
			k_typeMusic              = 0x00800000,
			k_typeSecondMusic        = 0x01000000,
		};	
	}
}

#ifdef DEBUG_AUDIO_SYSTEM
#	include <stdio.h>
#	define LOG_OPENAL_ERROR				DebugLogAlError(alGetError(), __FILE__, __LINE__, this)
#	define LOG_OPENAL_ERROR_STATIC		DebugLogAlError(alGetError(), __FILE__, __LINE__, 0)

	inline void DebugLogAlError(int error, const char* file, int line, void* ptr)
	{
		static int g_lastError = 0;
		if (error && error != g_lastError)
		{
			printf("ERROR: OpenAL %d %s(%d) %p\n", error, file, line, ptr);
		}
		g_lastError = error;
	}
#else
	#define LOG_OPENAL_ERROR			alGetError()
	#define LOG_OPENAL_ERROR_STATIC		alGetError()
#endif


#endif

