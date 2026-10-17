#ifndef __ONLINE_PLAYER_H__
#define __ONLINE_PLAYER_H__

#include "../Common/DefaultDataPacket.h"
#include "../Common/XIPAddress.h"
#include "../../Common/GLXPlayerSocketFactory.h"
#include "OnlinePlayerList.h"


enum OnlinePlayerType
{
	ONLINE_PLAYER_HOST,
	ONLINE_PLAYER_CLINET,
	ONLINE_PLAYER_LOCAL_HOST,
	ONLINE_PLAYER_LOCAL_CLINET
};

enum OnlineProtocol
{
	ONLINE_PROTOCOL_NIL = 0,
	ONLINE_PROTOCOL_TCP = 1,
	ONLINE_PROTOCOL_UDP = 2
};

class COnlinePlayerList;

/**
 * Represent one player in the game.
 * <p>
 * The player have some properties, such as name, nationality, IP, and so on.
 * <p>
 * 
 */
class COnlinePlayer
{

public:
	enum InitUpnpState
	{
		UPNP_INIT_TCPNONE=0,
		UPNP_INIT_TCPSEND,
		UPNP_INIT_TCPWAIT,
		UPNP_INIT_TCPEND,
		UPNP_INIT_UDPNONE,
		UPNP_INIT_UDPSEND,
		UPNP_INIT_UDPWAIT,
		UPNP_INIT_UDPEND,
		UPNP_INIT_ALLEND
	};

	COnlinePlayer(void);

	~COnlinePlayer(void);

	int GetIndex()
	{
		return m_index;
	}

	void SetIndex( int index )
	{
		m_index = index;
	}

	/**
	 * Return the nickname of the player.
	 *
	 */
	char * GetName();

	/**
	 * Reset the nickname of the player.
	 *
	 */
	void SetName( char * name );
	
	/**
	 * Return the nationality of the player.
	 *
	 */
	char * GetNationality();

	/**
	 * Reset the nationality of the player.
	 *
	 */
	void SetNationality( char * name );

	/**
	 * Add external ip and port.
	 * @param char *, for example: "219.142.114.185:56800"
	 */
	bool SetExternalTcpAddress( char * ip_data );

	bool SetExternalUdpAddress( char * ip_data );

	/**
	 * Return external IP address, such as "219.142.114.185".
	 *
	 */
	char * GetExternalIP();

	/**
	 * Retrun external port, such as 56800.
	 *
	 */
	int GetExternalTcpPort();

	/**
	 * Retrun external port, such as 56800.
	 *
	 */
	int GetExternalUdpPort();

	/**
	 * Add internal ip and port.
	 * @param char *, for example: "10.15.0.171:56800"
	 */
	bool SetInternalTcpAddress( char * ip_data );

	bool SetInternalUdpAddress( char * ip_data );	

	/**
	 * Return external IP address, such as "10.15.0.171".
	 *
	 */
	char * GetInternalIP();

	/**
	 * Retrun external port, such as 56800.
	 *
	 */
	int GetInternalTcpPort();
	
	/**
	 * Retrun external port, such as 56800.
	 *
	 */
	int GetInternalUdpPort();

	/**
	 * 
	 *
	 */
	bool IsAlive()
	{
		return m_bAlive;
	}

	/**
	 * The dead connection will be removed.
	 *
	 */
	void SetAlive( bool flag )
	{
		m_bAlive = flag;
	}

	/**
	 * 
	 *
	 */
	bool IsConnected()
	{
		return m_bConnected;
	}

	/**
	 * 
	 *
	 */
	void SetConnected( bool flag )
	{
		m_bConnected = flag;
	}

	/**
	 * 
	 *
	 */
	bool IsJoined()
	{
		return m_bJoined;
	}

	/**
	 * 
	 *
	 */
	void SetJoined( bool flag )
	{
		m_bJoined = flag;
	}

	/**
	 * 
	 *
	 */
	OnlineProtocol GetProtocol()
	{
		return m_protocol;
	}

	/**
	 * 
	 *
	 */
	void SetProtocol( OnlineProtocol protocol )
	{
		m_protocol = protocol;
	}

	DataPacket * GetData()
	{
		return m_data;
	}

	void SetData( DataPacket * packet )
	{
		XP_API_DEL( m_data );
		m_data = XP_API_NEW DefaultDataPacket( packet );
	}

	void SetTcpSocket( GLXPlayerSocket * socket );

	DataPacket * GetIncomingPacket()
	{
		return m_incoming_packet;
	}

	void SetIncomingPacket( DataPacket * packet )
	{
		m_incoming_packet = packet;
	}

	void RemoveIncomingPacket();

	void RemoveOutcomingPacket();

	bool KeepAlive();

    int GetUdpSocket()
    {
        return m_udp_socket->GetSocket();
    }

public:

	/**
	 * Try to receive data, including TCP and UDP data.
	 * <p>
	 * The data will be store in m_incoming_packet.
	 * <p>
	 * @return
	 *	true:	new data are saved in m_incoming_packet
	 *	false:	no data
	 *
	 */
	bool ReceiveData();

	/**
	 * Receive data from tcp connection, including session data and reliable game data.
	 *
	 */
	bool ReceiveTCPData();

	/**
	 * Receive data from udp socket, only the unreliable game data.
	 *
	 */
	bool ReceiveUDPData();

	bool ReceiveUDPData( COnlinePlayer * sender );

	bool SendAllData( COnlinePlayer * receiver );

	/**
	 * Send data by tcp connection, including session data and reliable game data.
	 *
	 */
	bool SendAllTCPData();

	/**
	 * Send data by udp socket, only the unreliable game data.
	 *
	 */
	bool SendAllUDPData( COnlinePlayer * receiver );

	/**
	 * Accept connection from client.
	 *
	 */
	GLXPlayerSocket * Accept();

	/**
	 * Add outcoming data.
	 *
	 */
	bool AddOutputData( DataPacket * packet );

	/**
	 * Time out or not.
	 *
	 */
	bool CheckAlive();

	/**
	 * Initialize the socket.
	 * <p>
	 * @param
	 *	bHost:	true - as host, using the tcp listening port
	 *			false - as client, using the tcp connecting port
	 *
	 */
	bool InitSocket( OnlinePlayerType type );

	bool StartListen( int count );

	bool StartLocalRoom( int count );

//-----------------change the connect function to nonblock, add the connect state //by han yuan 2009.3.31
	int GetConnectState()
	{ 
		if(m_tcp_socket == NULL)
			return GLXPlayerSocket::NONBLOCK_CONNECT_STATE_FAILED;
		else
			return m_tcp_socket->GetNonBlockConnectState();
	}

	void SetConnectState(int state)
	{ 
		if(m_tcp_socket == NULL)
			return;

		m_tcp_socket->SetNonBlockConnectState(state);
	}
//------------------------------------------

	bool Connect( COnlinePlayer * listener );

	bool CheckGameData( char * &name, DataPacket * &packet );

	void HandlePingData();
	void InitUpnpInitState()
	{
		m_upnpInitState=UPNP_INIT_TCPNONE;
		m_tcpIport=0;
		m_udpIport=0;

	}

private:

	/**
	 * Send a data packet with TCP connection.
	 *
	 */
	int SendTCPDataPacket( DataPacket * packet );

	/**
	 * Send a data packet with UDP socket.
	 *
	 */
	int SendUDPDataPacket( COnlinePlayer * receiver, DataPacket * packet );

	InitUpnpState InitTcpSocket( OnlinePlayerType type );

	InitUpnpState InitUdpSocket( OnlinePlayerType type );

	int AddIncomingData( char * buffer, int length, OnlineProtocol protocol, 
		char * ip = NULL, int port = 0 );

	int AddIncomingDataPacket( char * buffer, int length, OnlineProtocol protocol, 
		char * ip = NULL, int port = 0 );

private:

	int m_index;

	/**
	 * The nickname of the player.
	 *
	 */
	char * m_name;

	/**
	 * The nationality of the player.
	 *
	 */
	char * m_nationality;

	/**
	 * The ip address of external network.
	 *
	 */
	XIPAddress * m_external_tcp_address;

	/**
	 * The ip address of internal network.
	 *
	 */
	XIPAddress * m_internal_tcp_address;

	/**
	 * The ip address of external network.
	 *
	 */
	XIPAddress * m_external_udp_address;

	/**
	 * The ip address of internal network.
	 *
	 */
	XIPAddress * m_internal_udp_address;

	/**
	 * The last time to send data to it. 
	 *
	 */
	long m_last_send_time;

	/**
	 * The last time to receive data from it. 
	 *
	 */
	long m_last_recv_time;

	/**
	 * TCP Socket. 
	 *
	 */
	GLXPlayerSocket * m_tcp_socket;

	/**
	 * UDP Socket.
	 *
	 */
	GLXPlayerSocket * m_udp_socket;

	/**
	 * The packets to send to it.
	 *
	 */
	DataPacket * m_outcoming_packet;

	/**
	 * The packets received from it.
	 *
	 */
	DataPacket * m_incoming_packet;
	
	/**
	 * Is it still alive.
	 *
	 */
	bool m_bAlive;

	/**
	 * Is the direct connection has been established.
	 *
	 */
	bool m_bConnected;

	/**
	 * Is it has joined the game room.
	 *
	 */
	bool m_bJoined;

	/**
	 * Game data network protocol between players.
	 *
	 */
	OnlineProtocol m_protocol;

	/**
	 * Player information.
	 *
	 */
	DataPacket * m_data;

	char m_tcp_buffer[ONLINE_MAX_PACKET_SIZE];

	int m_tcp_position;

	char m_udp_buffer[ONLINE_MAX_PACKET_SIZE];

	int m_udp_position;
	/////////////////////////////////////////////////////////////////////////////////////


	InitUpnpState m_upnpInitState;

   int m_tcpIport;
   int m_udpIport;
public :
	void OnAddPortMappingSuccess(int type,int portMapped);
};

#endif