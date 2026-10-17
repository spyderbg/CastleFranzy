#ifndef _GLLIVE_MEMORY_H_
#define _GLLIVE_MEMORY_H_

#include "GLLivePredefine.h"

#define MEMORY_DEBUG_LEVEL 1

#if MEMORY_DEBUG_LEVEL == 1
#ifdef WIN32
	#include <stdlib.h>
	#include <crtdbg.h>
#endif
#endif

//namespace gllive
//{
	#if PLATFORM == PLATFORM_NGAGE

		#define NEW new
		#define DEL delete

		#define SAFE_RELEASE_NGI(x) if(x){x->Release();x=NULL;}
		#define NGI_CREATE_REG(x) 
		#define NGI_CREATEINSTANCE_REG(x) CRuntime::CreateInstance(x)

		unsigned int GetMaxMemory();
		unsigned int GetFreeMemory();
		
		#define SAFE_DELETE(p) {if(p) DEL(p);p = NULL;}
		#define SAFE_DELETE_ARRAY(p) {if(p) DEL[](p);p = NULL;}
		
	#elif PLATFORM == PLATFORM_SYMBIAN

		unsigned int GetMaxMemory();
		unsigned int GetFreeMemory();
		
		void* operator new(unsigned int nSize, int x);
		void* operator new[](unsigned int nSize, int x);

		#define NEW new(1)
		#define DEL delete

		#define SAFE_DELETE(p) {if(p) DEL(p);p = NULL;}
		#define SAFE_DELETE_ARRAY(p) {if(p) DEL[](p);p = NULL;}

	#elif PLATFORM == PLATFORM_WINDOWS_MOBILE

		#ifdef NEW
		#undef NEW
		#endif

		#ifdef DEL
		#undef DEL
		#endif

		#define memcpy8 memcpy

		#define NEW new
		#define DEL delete

		inline unsigned int GetMaxMemory(){return 0xFFFFFFFF;};
		inline unsigned int GetFreeMemory(){return 0xFFFFFFFF;};

		void* operator new(size_t nSize, const char * lpszFileName, int nLine);
		void operator delete(void* p);

		void DisplayMemoryInfo();
		int GetCurrentUsedMemory();
		int GetMaxUsedMemory();

		#define SAFE_DELETE(p) {if(p) DEL(p);p = NULL;}
		#define SAFE_DELETE_ARRAY(p) {if(p) DEL[](p);p = NULL;}
	#elif (PLATFORM == PLATFORM_KDDI) 

	typedef enum ENUM_MEMORY_TYPE{
		DYNAMIC = -1,
		POOL_BOTTOM,
		POOL_TOP,
		GLOBAL_BOTTOM,
		GLOBAL_TOP,
	}MemoryType;

		void* operator new(unsigned int size);
		void* operator new[](unsigned int size);
		void* operator new(unsigned int size, MemoryType memType);
		//void* operator new(unsigned int size,char* f,int l);
		//void* operator new[](unsigned int size,char* f,int l);
		void operator delete(void* p);

		#ifdef NEW
		#undef NEW
		#endif
		//#define NEW new(__FILE__, __LINE__)
		#define NEW new
		#define NEW_X new

		#ifdef DEL
		#undef DEL
		#endif
		#define DEL delete

		#define SAFE_DELETE(p) {if(p) DEL(p);p = NULL;}
		#define SAFE_DELETE_ARRAY(p) {if(p) DEL[](p);p = NULL;}

	#elif (PLATFORM == PLATFORM_WIN32) || (defined(WIN32))
	#if KDDI
		typedef enum ENUM_MEMORY_TYPE{
			DYNAMIC = -1,
			POOL_BOTTOM,
			POOL_TOP,
			GLOBAL_BOTTOM,
			GLOBAL_TOP,
		}MemoryType;
	#endif

		#ifdef NEW
		#undef NEW
		#endif

		#ifdef DEL
		#undef DEL
		#endif

		#define memcpy8 memcpy


		#if MEMORY_DEBUG_LEVEL == 0
			#define NEW new
			#define DEL delete

			inline unsigned int GetMaxMemory(){return 0xA00000;};
			inline unsigned int GetFreeMemory(){return 0xA00000;};


		#elif MEMORY_DEBUG_LEVEL == 1
			#define _CRTDBG_MAP_ALLOC
			
			void* operator new(size_t nSize, const char * lpszFileName, int nLine);
			void operator delete(void* p);
			void operator delete[](void* p);
			
			#define NEW new(__FILE__, __LINE__)
			#define DEL delete

			inline unsigned int GetMaxMemory(){return 0xA00000;};
			inline unsigned int GetFreeMemory(){return 0xA00000;};

		#elif MEMORY_DEBUG_LEVEL == 2

			void* operator new(size_t nSize, const char * lpszFileName, int nLine);
			void operator delete(void* p);

			#define NEW new(__FILE__, __LINE__)
			#define DEL delete

			void DisplayMemoryInfo();
			int GetCurrentUsedMemory();
			int GetMaxUsedMemory();
			unsigned int GetMaxMemory();
			unsigned int GetFreeMemory();
		#endif


		#define SAFE_DELETE(p) {if(p) DEL(p);p = NULL;}
		#define SAFE_DELETE_ARRAY(p) {if(p) DEL[](p);p = NULL;}
	#elif (PLATFORM == PLATFORM_MAC_OS_X)
		#ifdef NEW
			#undef NEW
		#endif
		#define NEW new

		#ifdef DEL
			#undef DEL
		#endif
		#define DEL delete

	//	void* operator new(size_t nSize, const char * lpszFileName, int nLine);
	//	void operator delete(void* p);
	#include <string.h>
		#define SAFE_DELETE(p) {if(p) DEL(p);p = NULL;}
		#define SAFE_DELETE_ARRAY(p) {if(p) DEL[](p);p = NULL;}
	#endif
//}
		
#endif
