#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "../../Porting.h"
#include "Memory.h"
#include "DefaultDataPacket.h"
#include "../../Common/GLXPlayerSocketFactory.h"

/**
 * The TCP connection class.
 * <p>
 * Handles all the physical communication with the server.
 * Opens, closes the sockets, sends and receives the data ...
 * <p>
 * To establish connection :
 * - call connect() to start process
 * - wait for a connection success message, then call set_connected ()
 * <p>
 * To terminate connection :
 * - call disconnect
 * <p>
 * To send requests to the server :
 * - either construct data packets and call addOutgoingPacket
 * - or use predefined primitives for standard messages ...
 * <p>
 * To get the packages received from the server :
 * - call getNextIncomingPacket
 *
 * @author World Online Team <<online-adm@gameloft.com>>
 * @author  Lu Tie Hua <tiehua.lu@gameloft.com> 
 * @version 0.0.1
 *
 */
class Connection
{
public:

	/**
	* Connection constructor.
	*/
	Connection(char* address,int server_port);

	/**
	* Connection destructor.
	*/
	virtual ~Connection();

//-----------------change the connect function to nonblock, add the connect state //by han yuan 2009.3.31
protected:
	int m_connectState;

public:
	enum NonBlockConnectState
	{
		CONNECT_STATE_START,
		CONNECT_STATE_WAITING,
		CONNECT_STATE_SUCCESS,
		CONNECT_STATE_FAILED
	};

	int GetConnectState()
	{ return m_connectState;}

	void SetConnceState(int state)
	{ m_connectState = state;}
//------------------------------------------

	/**
	 * Function connect().
	 * In order to establish an tcp connection with the server,
	 * we need to send two packets containing a connection identifier.
	 * If the server matches the packages, the connection is established.
	 *
	 */
	bool connect();
	
	/**
	 * Function disconnect().
	 * Close tcp connection with the server,	 
	 */
	void disconnect();

	void dnsResolved();
	virtual void sendData();
	virtual void receiveData(int expected);
	virtual void receiveDataLen();

	void addOutgoingPacket(DataPacket* packet);
	void addIncomingPacket(DataPacket* packet);

	DataPacket* getNextIncomingPacket();

	// utilities for sending different kind of packets according to the protocol ... 
	virtual void sendEstablishConnectionPackage();
	virtual void sendFinishConnectionPackage();
	void sendLoginPackage(char* nickname);
	void sendCreateSessionPackage(char* session_name,char* game_data,int length);
	void sendQuickGamePackage();


	void sendJoinSessionPackage(char* session_name);
	void sendLeaveSessionPackage();

	void sendListSessionsPackage(BYTE number_of_sessions,int session_index);

	void sendListSessionsByNamePackage(BYTE number_of_sessions,int session_index, char * name);
	void sendListSessionsByDataPackage(BYTE number_of_sessions,int session_index, char * data, int length);
	void sendListSessionsAllPackage(BYTE number_of_sessions,int session_index);

	void sendKickOutPlayerPackage(char* player_name);

	/**
	* Send the package to start game.
	*/
	void sendStartGamePackage();

	void sendFinishGamePackage();

	void sendUpdateSessionPackage(int player_number);

	void setName( char * name );

	bool isConnected();

	void set_connected(bool conn);

#ifdef MP_ENABLE_TIMEOUT 
	  // the time the request is started. Used to check
	  // for timeout. Only if MP_ENABLE_TIMEOUT is defined.
	  unsigned long last_activity_time;
#endif

	// call this from time to time to avoid closing connection by the server ... 
	bool keepConnectionAlive();

	// only wait for a request response a limited time ; 
	long getTimeOfLastReqest();

	// used for the retry operation in case of network failure ... 

	// call this when sending a packet that you want retry to work on ... 
	void saveRetryData(DataPacket* packet);
	// call this to send the data you saved 
	void sendRetryData();
	// call this when retry operation is no longer possible 
	void cleanRetryData();

	//expected length of next message
	BYTE rExpected;
	//progressive read, Eg: rExpected = 4: rProgressive = 2+1+1
	BYTE rProgressive;

	// Even though this is public it should only be used by the functions in
	// connection.cpp. All other classes must use the isConnected() function.
	bool connected;
   
	GLXPlayerSocket* GetSocket() { return m_socket; }

protected:

	char rbuf[(ONLINE_MAX_PACKET_SIZE + 1)]; //receive buf
	char sbuf[(ONLINE_MAX_PACKET_SIZE + 1)]; //send buf

	// communication objects ..
	//SOCKET	m_pISocket; // Pointer to socket

	GLXPlayerSocket * m_socket;

#ifdef MP_ENABLE_P2P

	GLXPlayerSocket * m_soc_udp;

#endif // MP_ENABLE_P2P

	// the inet adress of the server
	char* the_server_address;
	// the port to which will connect on the server
	int   the_server_port;

	// the data packets are linked in a list. once one is sent,
	// the next one will try to go

	// the outgoing packets ... 
	DataPacket* currentOutgoingPacket; // the current sending packet .. 
	DataPacket* lastOutgoingPacket;    // the last of the list .. 

	// the incoming packets ...
	DataPacket* currentIncomingPacket; // the current sending packet .. 
	DataPacket* lastIncomingPacket;    // the last of the list ..


	// keep alive connection ... 
	// this is the time of last sending
	// when the user last pressed a key (used in nick enter)
	long time_of_last_sending;

	// used for the retry in case of network fail. We must have a timeout. 
	long time_of_last_request;

	// if the request did not have a response, prompt for retry/cancel.
	// we need to keep the last sent package for this .. 
	DataPacket* retryPacket;

	char * m_name;

	//long total_number_of_received_packets;
	//long total_number_of_sent_packets;
};

#endif //__CONNECTION_H__