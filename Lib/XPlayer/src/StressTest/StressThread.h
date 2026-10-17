#include "../../include/Game_defines.h"
#ifdef LOBBY_THREAD_TEST
#ifndef __EXTRA_THREAD__
#define __EXTRA_THREAD__
#include "StressQueue.h"
#include <windows.h>
#include <process.h>       // For _beginthreadex
#include "../../include/Porting.h"
#include "StressLog.h"
typedef unsigned (__stdcall *PTHREAD_START) (void *);
#define chBEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
	pvParam, dwCreateFlags, pdwThreadId)                 \
	((HANDLE)_beginthreadex(                          \
	(void *)        (psa),                         \
	(unsigned)      (cbStackSize),                 \
	(PTHREAD_START) (pfnStartAddr),                \
	(void *)        (pvParam),                     \
	(unsigned)      (dwCreateFlags),               \
	(unsigned *)    (pdwThreadId)))
//typedef struct {
//int id;
//char* data;
//
//}log;
extern Queue<StressLog*> queue;

class StressThread
{
public:
	StressThread();
	~StressThread(){
		CloseHandle(t);
	}

	int run();
	void start(int threadCount);

private:

	StressThread(const StressThread&);
	StressThread& operator=(const StressThread&);
	static DWORD WINAPI run0(PVOID param);
	HANDLE t;
	int pa;



};



#endif
#endif