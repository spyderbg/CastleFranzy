#ifndef __CTHREAD__
#define __CTHREAD__


#include "../../../../../include/Game_defines.h"

#ifndef OS_IPHONE
#include <windows.h>
#include <process.h>       // For _beginthreadex
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
#else

#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#endif
class Cthread
{
public :
	Cthread();
	virtual ~Cthread();
	void start();
	virtual void Terminate();
    bool isCurThreadRunning();
protected:

	virtual void run(void)=0;

	void lock()
	{
#ifndef OS_IPHONE
		EnterCriticalSection(&m_mutex);
#else
		pthread_mutex_lock( &m_mutex );
#endif

	}

	void unlock()
	{
#ifndef OS_IPHONE
		LeaveCriticalSection(&m_mutex);
#else
		pthread_mutex_unlock( &m_mutex );
#endif
	}

private :
#ifndef OS_IPHONE
	CRITICAL_SECTION m_mutex;
	unsigned long  static run0(void* param);
#else
	pthread_mutex_t m_mutex;
	void static run0(void* param);
#endif

	Cthread(const Cthread&);
	Cthread& operator=(const Cthread&);

#ifndef OS_IPHONE
	void* m_handle;
	unsigned long m_threadId;
#else
	int m_handle;
	pthread_t m_threadId;
#endif
};
#endif
