#include "upnpThread.h"


Cthread::Cthread():
#ifndef OS_IPHONE
m_handle(INVALID_HANDLE_VALUE),
m_threadId(-1)
#else
m_handle(-1),
m_threadId(NULL)
#endif
{
#ifndef OS_IPHONE
	InitializeCriticalSection( &m_mutex );
#else
	pthread_mutex_init( &m_mutex, 0 );
#endif
}

Cthread::~Cthread()
{
#ifndef OS_IPHONE
	DeleteCriticalSection(&m_mutex);
	if(m_handle!=INVALID_HANDLE_VALUE)
	{

		CloseHandle(m_handle);
		m_handle=INVALID_HANDLE_VALUE;
	}

#else

	pthread_mutex_destroy( &m_mutex );
#endif
}

#ifndef OS_IPHONE
unsigned long  Cthread::run0(void* param)
{
	Cthread* ct=(Cthread*)param;
	ct->run();

	return (0);
}
#else
void Cthread::run0(void* param)
{
	Cthread* ct=(Cthread*)param;
	ct->run();
}
#endif

//avoid call this method
void Cthread::Terminate()
{
#ifndef OS_IPHONE
	TerminateThread(m_handle,0);

#else
	pthread_cancel(m_threadId); 
#endif
}

bool Cthread::isCurThreadRunning()
{
#ifndef OS_IPHONE
	unsigned long  p;
	GetExitCodeThread(m_handle,&p);
	return (p==STILL_ACTIVE);
#else
	int ret = pthread_kill(m_threadId,0);
return (ret!=ESRCH&&ret!=EINVAL);

#endif
}

void Cthread::start()
{
#ifndef OS_IPHONE

	m_handle=chBEGINTHREADEX(NULL,0,run0,(void*)this,0,&m_threadId);
#else
	m_handle=pthread_create(&m_threadId,NULL,(void *(*)(void *))run0,(void*)this);
   pthread_detach(m_threadId);//can free res by self,Otherwise call pthread_join in isCurThreadRunning(by def a global  var)!

#endif
}

