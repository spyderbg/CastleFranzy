#include "../../include/Game_defines.h"
#ifdef LOBBY_THREAD_TEST
#ifndef __QUEUE__
#define __QUEUE__
#include <deque>
#include <iostream>
#include <windows.h>
template <class T>
class Queue
{
public:
	Queue<T>()
	{
		InitializeCriticalSection( &g_cs );
	};
	~Queue<T>()
	{
		DeleteCriticalSection(&g_cs);
	};
protected:
	std::deque<T> c;
private :
	CRITICAL_SECTION g_cs;
public:
	typename std::deque<T>::size_type size() const
	{
		return c.size();
	}
	bool empty()
	{

		return c.empty();
	}
	void push(const T& elem)
	{
		
		EnterCriticalSection(&g_cs);
		c.push_back(elem);
		LeaveCriticalSection(&g_cs);
	}

	T pop()
	{
		EnterCriticalSection(&g_cs);
		if(c.empty())
		{
			LeaveCriticalSection(&g_cs);
			return NULL;
		}
		T ele(c.front());
		c.pop_front();
		LeaveCriticalSection(&g_cs);
		return ele;
		

	}

};


#endif
#endif