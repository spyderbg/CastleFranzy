#ifndef __XIPADDRESS_H__
#define __XIPADDRESS_H__

#include "../../Porting.h"
#include "Memory.h"  //byhy for debug memory leak

class XIPAddress
{

public:

	XIPAddress( char * data = NULL );

	XIPAddress( XIPAddress * addr );
	
	~XIPAddress( void );

	char * getIP();	
	void setIP(const char * ip);

	int  getTcpPort();
	void setTcpPort(int port);
	
	int  getUdpPort();
	void setUdpPort(int port);

	int  getBroadcastPort();
	void setBroadcastPort(int port);

	bool IsPrivateAddress();

private:
	char *	m_ip;
	int		m_port_tcp;
	int		m_port_udp;
	int     m_port_broadcast;
};
#endif // __XIPADDRESS_H__