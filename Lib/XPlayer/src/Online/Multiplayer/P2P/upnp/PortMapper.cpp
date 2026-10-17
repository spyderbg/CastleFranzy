////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PortMapper.h"

#define XP_PORT_MAP_HEAD	"GL-XPlayer"

#include "miniwget.h"
#include "upnpcommands.h"

#include "../../../../../include/Porting.h"
//#include "Memory.h"  //byhy for debug memory leak

////////////////////////////////////////////////////////////////////////////////////////////////////

// get local ip address
bool PortMapper::GetLocalIP(char * szLocalIP)
{
	return m_socket->GetLocalIP( szLocalIP );
}

bool PortMapper::GetHostName(char ** addr)
{
	return ( m_socket->GetHostName( addr ) != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PortMapper* PortMapper::m_instance		=	0;

////////////////////////////////////////////////////////////////////////////////////////////////////

PortMapper* PortMapper::GetInstance()
{
	if(!m_instance)
	{
		m_instance = XP_API_NEW PortMapper();
	}

	return m_instance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void PortMapper::ForceDestroy()
{
	if( !m_instance )
		return;	

	delete m_instance;
	m_instance = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PortMapper::PortMapper():m_excuteFunType(FunNone),m_excuteState(ExcuteIdle)
{
	m_bInit =StateNone;
	m_bIGDFound = StateNone;
	m_bGetExternIp=StateNone;
	m_bAddTcpMapping=StateNone;
	m_bAddUdpMapping=StateNone;
	m_username = NULL;	
	m_socket = GLXPlayerSocketFactory::GetSocket();
	XP_API_MEMSET(&urls, 0, sizeof(struct UPNPUrls));
	m_list = NULL;
	memset(m_externIp,0,128);
}
PortMapper::~PortMapper()
{
	FreeUPNPUrls(&urls);
	XP_API_DEL( m_username );
	XP_API_DEL( m_socket );

	while( NULL != m_list )
	{
		UPnPPort * temp = m_list;
		m_list = m_list->next;
		XP_API_DEL( temp );
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void PortMapper::Init( long ggi, char * username, int try_times )
{

	//if(m_excuteStat==ExcuteWaiting)
	//return;


	m_ggi = ggi;
	m_username = XP_API_STRNEW( username );
	m_try_times = try_times;
	m_excuteFunType=FunInit;
	m_excuteState=ExcutePrepare;
	//start();


}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool PortMapper::GetExternIP(char* addr)
{
	if(!urls.controlURL || urls.controlURL[0] == '\0')
	{
		XP_DEBUG_OUT("TB : GetExternIP the init was not done !\n");
		return false;;
	}

	int r = UPNP_GetExternalIPAddress(urls.controlURL, data.servicetype, addr);

	if(r != UPNPCOMMAND_SUCCESS)
	{
		XP_DEBUG_OUT("GetExternalIPAddress() failed\n");
		return false;
	}

	return true;

}

/**
* Add port mapping into router.
* <p>
* @param:
*     type: TCP or UDP 
*	   ip:   ip address to be mapped
*     port: port to be mapped
* @return:
*     >0:   the port has been mapped
*     =0:   failed
*/
int PortMapper::AddPortMapping( int type, char * ip, int port )
{
	// Check parameter
	if ( type != PORT_MAPPING_TCP && type != PORT_MAPPING_UDP )
	{
		m_upnpObserver->OnUpnpRequestFailed("mapping port type error!", ERROR_MP_UPNP_MAPPING);
		return 0;
	}

	m_mappedType=type;
	m_mappedIp=XP_API_STRNEW(ip);
	m_mappedPort=port;

	m_excuteFunType=FunAddPortMapping;
	m_excuteState=ExcutePrepare;

	return 0;

}

////////////////////////////////////////////////////////////////////////////////////////////////////

void PortMapper::RemoveAllPortMapping()
{
	UPnPPort * header = m_list;
	while( NULL != header )
	{
		RemovePortMapping( header->type, header->port );
		header = header->next;
	}
}

bool PortMapper::RemovePortMapping( int type, int port )
{
	char szLocalPort[16];

	if(!urls.controlURL || urls.controlURL[0] == '\0')
	{
		XP_DEBUG_OUT("TB : RemovePortMapping the init was not done !\n");
		return false;
	}

	XP_API_MEMSET(szLocalPort, 0, 16);
	XP_API_SPRINTF(szLocalPort, "%d", port);

	//XP_DEBUG_OUT("TB : RemovePortMapping (%s)\n", szLocalPort);

	if ( type == PORT_MAPPING_TCP )
	{
		UPNP_DeletePortMapping(urls.controlURL, data.servicetype, szLocalPort, "TCP");
		XP_DEBUG_OUT("TB : RemovePortMapping TCP (%s)\n", szLocalPort);
	}
	else if ( type == PORT_MAPPING_UDP )
	{
		UPNP_DeletePortMapping(urls.controlURL, data.servicetype, szLocalPort, "UDP");
		XP_DEBUG_OUT("TB : RemovePortMapping UDP (%s)\n", szLocalPort);
	}
	else
	{
		return false;
	}	

	return true;
}
///////////////////////////////////////////////////////////////////
void PortMapper::ThreadInit()
{
	if(m_bInit==StateSuccess)
	{

		memset(m_externIp,0,128);

		if( !GetExternIP( m_externIp ) )
		{
			m_bGetExternIp=StateFailed;

			return;
		}
		m_bGetExternIp = StateSuccess;

		return;
	}

	m_bIGDFound=StateNone;
	m_bInit=StateNone;
	m_bGetExternIp=StateNone;

	{
		struct UPNPDev * devlist;
		struct UPNPDev * dev;
		char * descXML;
		int descXMLsize = 0;

		XP_DEBUG_OUT("TB : init_upnp()\n");

		XP_API_MEMSET(&urls, 0, sizeof(struct UPNPUrls));
		XP_API_MEMSET(&data, 0, sizeof(struct IGDdatas));

		devlist = upnpDiscover(2000, NULL, NULL, false);

		if (devlist)
		{
			dev = devlist;

			while (dev)
			{
				if (XP_API_STRSTR (dev->st, "InternetGatewayDevice"))
				{
					m_bIGDFound = StateSuccess;
					break;
				}
				dev = dev->pNext;
			}

			if (!dev)
			{
				dev = devlist; // defaulting to first device 
			}

			XP_DEBUG_OUT("UPnP device :\n" " desc: %s\n st: %s\n", dev->descURL, dev->st);

			descXML = (char*)miniwget(dev->descURL, &descXMLsize);

			if (descXML)
			{
				parserootdesc (descXML, descXMLsize, &data);
				XP_API_DEL (descXML);
				descXML = 0;
				GetUPNPUrls(&urls, &data, dev->descURL);
			}


			freeUPNPDevlist(devlist);
		}

		m_bInit = StateSuccess;

		if(m_bIGDFound==StateNone)
		{
			m_bIGDFound=StateFailed;

		}
		else
		{

			memset(m_externIp,0,128);

			if( !GetExternIP( m_externIp ) )
			{

				m_bGetExternIp=StateFailed;

				return;
			}
			m_bGetExternIp = StateSuccess;


		}
	}
}

void PortMapper::ThreadAddPortMapping()
{
	if((m_mappedType==PORT_MAPPING_TCP&&m_bAddTcpMapping==StateSuccess)||
		(m_mappedType==PORT_MAPPING_UDP&&m_bAddUdpMapping==StateSuccess))
	{

		return;
	}
	m_bAddTcpMapping=StateNone;
	m_bAddUdpMapping=StateNone;
	char szLocalPort[8] = { 0 };
	char szRouterPort[8] = { 0 };
	char szLocalAddress[64] = { 0 };

	if( !urls.controlURL || urls.controlURL[0] == '\0' )
	{

		XP_DEBUG_OUT("TB : AddPortMapping the init was not done !\n");
		if(m_mappedType==PORT_MAPPING_TCP)
		{
			m_bAddTcpMapping=StateFailed;
		}else
		{
			m_bAddUdpMapping=StateFailed;
		}

		return ;
	}

	char infor[64] = { 0 };
	XP_API_MEMSET( infor, 0 ,64 );
	XP_API_SPRINTF( infor, "%s-%d-%s", XP_PORT_MAP_HEAD, m_ggi, m_username );

	char proto[8] = { 0 };
	XP_API_MEMSET( proto, 0 ,8 );
	if ( m_mappedType == PORT_MAPPING_TCP )
	{
		XP_API_SPRINTF( proto, "%s", "TCP");
	}
	else if ( m_mappedType == PORT_MAPPING_UDP )
	{
		XP_API_SPRINTF( proto, "%s", "UDP");
	}

	int try_times = 0;
	int map_result = -1;
	char desc[64] = { 0 };
	int port_mapped = m_mappedPort;

	// Try to map the port once adding 1
	while( map_result != UPNPCOMMAND_SUCCESS && try_times < m_try_times )
	{
		// Initialize address and port
		XP_API_MEMSET( desc, 0 ,64 );
		XP_API_MEMSET( szLocalPort, 0 ,8 );	
		XP_API_MEMSET( szLocalAddress, 0 ,64 );
		XP_API_MEMSET( szRouterPort, 0 ,8 );
		XP_API_SPRINTF( szRouterPort, "%d", port_mapped );		

		// Check whether the port has been mapped
		int port_status = UPNP_GetSpecificPortMappingEntry(
			urls.controlURL, data.servicetype, szRouterPort, proto, szLocalAddress, szLocalPort, desc );

		if ( port_status == UPNPCOMMAND_SUCCESS )
		{
			XP_DEBUG_OUT("AddPortMapping already mapped [%s:%s] => [router:%s]\n", szLocalAddress, szLocalPort, szRouterPort );

			// The port has been mapped by myself
			if ( XP_API_STRCMP( desc, infor ) == 0 )	
			{
				map_result = UPNPCOMMAND_SUCCESS;

				if(m_mappedType==PORT_MAPPING_TCP)
				{
					m_bAddTcpMapping=StateSuccess;
				}else
				{
					m_bAddUdpMapping=StateSuccess;
				}
				m_Port=port_mapped;

				return ;
			}
			// The port has been mapped by others
			else
			{
				port_mapped++;
			}
		}
		else
		{
			// The port has not been mapped
			try_times++;

			// Initialize address and port
			XP_API_MEMSET( szLocalPort, 0 ,8 );	
			XP_API_MEMSET( szLocalAddress, 0 ,64 );
			XP_API_SPRINTF( szLocalAddress, "%s", m_mappedIp);
			//XP_API_SPRINTF( szLocalPort, "%d", port_mapped );
			XP_API_SPRINTF( szLocalPort, "%d", m_mappedPort );
			XP_API_MEMSET( szRouterPort, 0 ,8 );
			XP_API_SPRINTF( szRouterPort, "%d", port_mapped );

			map_result = UPNP_AddPortMapping( urls.controlURL, data.servicetype, szRouterPort, 
				szLocalPort, szLocalAddress, infor, proto );
		}		
	}

	if( map_result != UPNPCOMMAND_SUCCESS )
	{
		XP_DEBUG_OUT("AddPortMapping(%s, %s, %s) failed\n", szLocalPort, szLocalPort, szLocalAddress);
		if(m_mappedType==PORT_MAPPING_TCP)
		{
			m_bAddTcpMapping=StateFailed;
		}else
		{
			m_bAddUdpMapping=StateFailed;
		}
		return ;
	}

	XP_DEBUG_OUT("AddPortMapping [%s:%s] => [router:%s %s]\n", szLocalAddress, szLocalPort, szRouterPort, proto );

	UPnPPort * upnp = XP_API_NEW UPnPPort;
	upnp->type = m_mappedType;
	//upnp->port = port;
	upnp->port = port_mapped;
	upnp->next = NULL;

	if ( NULL == m_list )
	{
		m_list = upnp;
	}
	else
	{
		UPnPPort * prev = m_list;
		while( NULL != prev->next )
		{
			prev = prev->next;
		}
		prev->next = upnp;
	}
	if(m_mappedType==PORT_MAPPING_TCP)
	{
		m_bAddTcpMapping=StateSuccess;
	}else
	{
		m_bAddUdpMapping=StateSuccess;
	}
	m_Port=port_mapped;

	return ;


}

void PortMapper::Terminate()
{

	m_excuteState=ExcuteIdle;
}
/**
*
* call in upnp thread
*/
void PortMapper::run()
{
	//lock();
	// better define a temp var to store m_excuteFunType ,than 2 methods can sync run
	switch(m_excuteFunType)
	{
	case FunNone:
		return;
	case FunInit:
		ThreadInit();
		break;;
	case FunAddPortMapping:
		ThreadAddPortMapping();
		break;
	}

	//unlock();
}
/**
*
*update the request call in Game thread
*/
void PortMapper::update()
{

	switch(m_excuteState)
	{
	case ExcutePrepare:
		{
			switch(m_excuteFunType)
			{
			case FunInit://D-C
				{
					if(IsGetExternIp())
					{
						m_upnpObserver->OnUpnpInitSuccess(m_externIp);
						m_excuteState=ExcuteIdle;
					}
					else
					{
						if(isCurThreadRunning())
						{
							m_excuteState=ExcuteWaiting;
						}
						else
						{
							if(IsGetExternIp())
							{
								m_upnpObserver->OnUpnpInitSuccess(m_externIp);
								m_excuteState=ExcuteIdle;
							}
							else
							{	
								m_excuteState=ExcuteStart;
							}
						}
					}
				}
				break;
			case FunAddPortMapping:
				{
					if(isCurThreadRunning())
					{
						m_excuteState=ExcuteWaiting;
					}
					else
					{
						m_excuteState=ExcuteStart;
					}
				}
				break;
			}
		}
		break;
	case ExcuteStart:
		{
			m_excuteState=ExcuteWaiting;
			start();
		}
		break;
	case ExcuteWaiting:
		{
			switch(m_excuteFunType)
			{
			case FunInit:
				{
					if(IsGetExternIp())
					{
						m_upnpObserver->OnUpnpInitSuccess(m_externIp);
						m_excuteState=ExcuteIdle;
					}
					else
					{
						if(m_bIGDFound==StateFailed)
						{
							m_upnpObserver->OnUpnpRequestFailed("uPnP disabled!", ERROR_MP_UPNP_DISABLE);
							m_excuteState=ExcuteIdle;
						}
						else if(m_bGetExternIp==StateFailed)
						{
							m_upnpObserver->OnUpnpRequestFailed("uPnP disabled!", ERROR_MP_UPNP_GET_EXTERNAL_IP);
							m_excuteState=ExcuteIdle;
						}
					}
				}
				break;
			case FunAddPortMapping:
				if((m_mappedType==PORT_MAPPING_TCP&&m_bAddTcpMapping==StateSuccess)||
					(m_mappedType==PORT_MAPPING_UDP&&m_bAddUdpMapping==StateSuccess))
				{
					m_upnpObserver->OnAddPortMappingSuccess(m_mappedType,m_Port);
					m_excuteState=ExcuteIdle;
				}
				else if((m_mappedType==PORT_MAPPING_TCP&&m_bAddTcpMapping==StateFailed)||
					(m_mappedType==PORT_MAPPING_UDP&&m_bAddUdpMapping==StateFailed))
				{
					m_upnpObserver->OnUpnpRequestFailed("mapping err!", ERROR_MP_UPNP_MAPPING);
					m_excuteState=ExcuteIdle;
				}
				break;
			}

		}
		break;
	case ExcuteIdle:
		break;


	}






}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
