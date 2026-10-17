////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PORT_MAPPER_H__
#define __PORT_MAPPER_H__

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "miniupnpc.h"
#include "../../../../../include/Common/GLXPlayerSocketFactory.h"
#include "../../../../../include/Multiplayer/P2P/GLXPlayerUpnpObserver.h"
#include "upnpThread.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_PORT_MAPPING_COUNT	10

typedef struct _UPnPPort
{
	int type;
	int port;
	struct _UPnPPort * next;
}UPnPPort;

class PortMapper: public Cthread
{
public:

	static PortMapper*	m_instance;
	static PortMapper*	GetInstance();
	static void	ForceDestroy();

	enum ProtocolType
	{
		PORT_MAPPING_TCP = 0,
		PORT_MAPPING_UDP = 1
	};
	/**
	*
	*thread run excuteFunType
	*/
	enum ExcuteFunType
	{
		FunNone=0,
		FunInit,
		//FunGetExternIP,
		FunAddPortMapping,
		FunRemovePortMapping,
		FunRemoveAllPortMapping

	};

	/**
	*
	*Game thread state
	*/
	enum ExcuteStat
	{
		ExcutePrepare=0,
		ExcuteStart,
		ExcuteWaiting,
		ExcuteIdle

	};
	/**
	*
	*request state
	*-1:upnp function failed
	*0:upnp do not run
	*1:upnp function success
	*/
	enum UpnpStateCode
	{
		StateFailed=-1,
		StateNone=0,
		StateSuccess=1
	};
public:


	/**
	* GGI and user name are used for identifing different port mapping.
	* "GL-XPlayer-GGI-UserName" will be added into the description of the mapping.
	* According to the description, xplayer could know whether the port has been mapped by itself.
	*
	*/
	long    m_ggi;
	char *  m_username;

	/**
	*
	*
	*/
	int     m_try_times;


	char m_externIp[128];

	GLXPlayerSocket * m_socket;

	struct UPNPUrls urls;
	struct IGDdatas data;

public:

	bool	IsInited()		{	return (m_bInit==StateSuccess);		}
	bool	IsIGDFound()	{	return (m_bIGDFound==StateSuccess);		}
	bool IsGetExternIp()    {   return (m_bGetExternIp==StateSuccess);  }
public:


	~PortMapper();

	void RegistUpnuObserver(GLXPlayerUpnpObserver *upnpObserver)
	{
		m_upnpObserver = upnpObserver;
	}

	void Init( long ggi, char * username, int try_times );
	bool GetHostName(char ** addr);
	bool GetLocalIP(char* addr);
	bool GetExternIP(char* addr);
	int AddPortMapping( int type, char * ip, int port );
	bool RemovePortMapping( int type, int port );
	void RemoveAllPortMapping();

	void Terminate();
	void update();
private:
	PortMapper();
	UPnPPort * m_list;

	GLXPlayerUpnpObserver *m_upnpObserver;

	void ThreadInit();

	void ThreadAddPortMapping();
	void ThreadRemovePortMapping();
	void ThreadRemoveAllPortMapping();
protected:
	void run();
private:
	/**
	*
	*local store var
	*/
	char* m_externIP;
	int m_mappedType;
	char * m_mappedIp;
	int m_mappedPort;
	int m_Port;

	ExcuteFunType m_excuteFunType;
	ExcuteStat m_excuteState;

	UpnpStateCode	m_bInit;
	UpnpStateCode	m_bIGDFound;
	UpnpStateCode    m_bGetExternIp;
	UpnpStateCode    m_bAddTcpMapping;
	UpnpStateCode    m_bAddUdpMapping;





};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //__PORT_MAPPER_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
