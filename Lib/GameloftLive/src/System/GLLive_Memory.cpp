#include "GLLiveStdafx.h"
#include "GLLive_Memory.h"

//namespace gllive
//{
	#if PLATFORM == PLATFORM_WINDOWS_MOBILE
	#elif (PLATFORM == PLATFORM_KDDI)

	void* operator new(unsigned int size)
	{
		if(size == 0)
			return NULL;

		void* p = MALLOC(size);
		memset(p, 0, size);
		return p;
	}

	void* operator new[](unsigned int size)
	{
		if(size == 0)
			return NULL;

		return operator new(size);
	}

	void* operator new(unsigned int size, MemoryType memType)
	{
		if(size == 0)
			return NULL;

		if(memType == DYNAMIC)
			return operator new(size);
		else
			return operator new(size);
	}

	//
	//void* operator new(unsigned int size,char* f,int l)
	//{ 
	//	if(size == 0)
	//		return 0;
	//	void* ptr = MALLOC(size);
	//	ASSERT(ptr);
	//	if(!ptr)
	//		return 0;
	//	memset(ptr, 0, size);
	//	return (char*)ptr;
	//}

	//void* operator new[](unsigned int size,char *f,int l)
	//{
	//	if(size == 0)
	//		return 0;
	//	void* ptr = MALLOC(size);
	//	ASSERT(ptr);
	//	if(!ptr)
	//		return 0;
	//	memset(ptr, 0, size);
	//	return (char*)ptr;
	//}

	void operator delete(void *p)
	{
		if(!p)
			return;
		FREE(p);
	}

	#elif (PLATFORM == PLATFORM_WIN32) || (defined(WIN32))

	#if MEMORY_DEBUG_LEVEL == 1

		void* operator new(size_t nSize, const char * lpszFileName, int nLine)
		{
			void* p = ::operator new(nSize, 1, lpszFileName, nLine);
			return p;
		}

	#elif MEMORY_DEBUG_LEVEL == 2

		typedef struct 
		{
			VOID*	ptr;
			DWORD	size;
		}MemoryBlock;

		#define MAX_MEMORY_BLOCK_COUNT	1024*64

		static MemoryBlock	g_MemoryBlocks[MAX_MEMORY_BLOCK_COUNT];
		static bool			g_bMemoryInitialized = false;	
		unsigned int		g_nMemoryUsed;	
		unsigned int		g_nMaxMemoryUsed;	

		void* operator new(size_t nSize, const char * lpszFileName, int nLine)
		{
			if(!g_bMemoryInitialized)
			{
				memset(g_MemoryBlocks, 0, sizeof(g_MemoryBlocks));
				g_bMemoryInitialized = true;
				g_nMemoryUsed = 0;
				g_nMaxMemoryUsed = 0;
			}

			void* p = ::operator new(nSize);
			
			int i;

			for(i = 0; i < MAX_MEMORY_BLOCK_COUNT; i++)
			{
				if(g_MemoryBlocks[i].ptr == NULL)
				{
					g_MemoryBlocks[i].ptr = p;
					g_MemoryBlocks[i].size = nSize;
					break;
				}
			}

			ASSERT(i < MAX_MEMORY_BLOCK_COUNT);
			
			g_nMemoryUsed += nSize;

			if(g_nMaxMemoryUsed < g_nMemoryUsed)
				g_nMaxMemoryUsed = g_nMemoryUsed;

			return p;
		}


		void operator delete(void* p)
		{
			int i;

			for(i = 0; i < MAX_MEMORY_BLOCK_COUNT; i++)
			{
				if(g_MemoryBlocks[i].ptr == p)
				{
					g_nMemoryUsed -= g_MemoryBlocks[i].size;
					g_MemoryBlocks[i].ptr = NULL;
					g_MemoryBlocks[i].size = 0;
					break;
				}
			}

			ASSERT(i < MAX_MEMORY_BLOCK_COUNT);
			
			free(p);
		}

		void operator delete[](void* p)
		{
			int i;

			for(i = 0; i < MAX_MEMORY_BLOCK_COUNT; i++)
			{
				if(g_MemoryBlocks[i].ptr == p)
				{
					g_nMemoryUsed -= g_MemoryBlocks[i].size;
					g_MemoryBlocks[i].ptr = NULL;
					g_MemoryBlocks[i].size = 0;
					break;
				}
			}

			ASSERT(i < MAX_MEMORY_BLOCK_COUNT);

			free(p);
		}

		int GetCurrentUsedMemory()
		{
			return g_nMemoryUsed;
		}

		int GetMaxUsedMemory()
		{
			return g_nMaxMemoryUsed;
		}

		void DisplayMemoryInfo()
		{

		}

		unsigned int GetMaxMemory()
		{
			return 0xA00000;
		};
		
		unsigned int GetFreeMemory()
		{
			return GetMaxMemory() - g_nMemoryUsed;
		};


	#endif

	#elif PLATFORM == PLATFORM_SYMBIAN
		
	unsigned int GetMaxMemory()
	{
		return User::Heap().Size();
	}

	unsigned int GetFreeMemory()
	{
		TInt aBiggestBlock;
		return User::Heap().Available(aBiggestBlock);
	}


	void* operator new(unsigned int nSize, int x) 
	{
		void* ptr = 0;
		int nError = KErrNone;
		TRAP(nError,ptr = User::AllocL(nSize));
		if(nError != KErrNone)
		{
			User::Exit(KErrNoMemory);
		}
		memset(ptr,0,nSize);
		return ptr;
	}

	void* operator new[](unsigned int nSize, int x) 
	{
		void * p = 0;
		int nError = KErrNone;
		TRAP(nError,p = User::AllocL(nSize));
		if(nError != KErrNone)
		{
			User::Exit(KErrNoMemory);
		}
		memset(p,0,nSize);
		return p;
	}
			
	#elif PLATFORM == PLATFORM_NGAGE

	#include <memorymanagement.h> 

	using namespace ngi;

	extern IMemoryManager* 	g_pMemoryManager;

	unsigned int GetMaxMemory()
	{
		return g_pMemoryManager->GetCurrentHeap().GetMaxSize();
	}

	unsigned int GetFreeMemory()
	{
		return g_pMemoryManager->GetCurrentHeap().GetSize();
	}

	#endif
//}