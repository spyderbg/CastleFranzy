#include "stdafx.h"
#include <conio.h>

#include "GLReplayObserver.h"
#ifdef OS_WIN32
#include "../../../include/Win32Socket.h"
#else
#include "../../../include/IPhoneSocket.h"
#endif





DWORD WINAPI ReplayThread(LPVOID lp) ;

int main(int argn, char **argv)
{
#ifdef OS_WIN32
	CWin32Socket::Startup();
#else
	CIPhoneSocket::Startup();
#endif

	GLReplayObserver glReplay  ;
	DWORD dwThreadID ;
	HANDLE hThread = CreateThread(NULL, 0, ReplayThread, &glReplay, 0, &dwThreadID) ;


	
	//for command
	char input ;
	while(true)
	{
		printf(TIPS_INFO) ;
		//gets_s(input, 128) ;
		input = _getch() ;
		if (input == 'e' || input == 'E')
		{
			break ;
		}
		else if (input == 's' || input == 'S')
		{
			glReplay.Send() ;
		}
	
	}

	TerminateThread(hThread, 0) ;


#ifdef OS_WIN32
	CWin32Socket::Clearup();
#else
	CIPhoneSocket::Clearup();
#endif
	return 0 ;
}


DWORD WINAPI ReplayThread(LPVOID lp)
{

	GLReplayObserver *glReplay = (GLReplayObserver*)lp;
	while (true)
	{
		glReplay->Update() ;
	}

	return 0 ;
}


