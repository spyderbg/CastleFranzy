#include "../../../include/Common/GLXPlayerSocketFactory.h"
#include "../../../include/Game_defines.h"
#include "../../../include/Porting.h"

#ifdef OS_WIN32
#include "../../../include/Win32Socket.h"
#endif // OS_WIN32

#ifdef OS_IPHONE
#include "../../../include/IPhoneSocket.h"
#endif // OS_IPHONE

GLXPlayerSocket * GLXPlayerSocketFactory::GetSocket( char * server_address, 
	int server_port, GLXPlayerSocketObserver * observer )
{
#ifdef OS_WIN32
	return XP_API_NEW CWin32Socket( server_address, server_port, observer );
#endif // OS_WIN32
	
#ifdef OS_IPHONE
	return XP_API_NEW CIPhoneSocket( server_address, server_port, observer );
#endif // OS_IPHONE
	
	return 0;
}