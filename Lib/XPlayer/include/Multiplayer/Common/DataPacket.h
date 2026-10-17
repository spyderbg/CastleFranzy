#ifndef __DATAPACKET_H__
#define __DATAPACKET_H__

#include "../../Porting.h"
#include "Memory.h"  //byhy for debug memory leak
#include "../../Game_defines.h"

enum DataPacketType
{
	DATA_PACKET_TYPE_NONE = 0,
	DATA_PACKET_TYPE_SESSION = 1,
	DATA_PACKET_TYPE_GAME = 2,
	DATA_PACKET_TYPE_GAME_RELIABLE = 3
};

/**
 * DataPacket class.
 * <p>
 * The basic class that encapsulates packages sent on the network.
 * contains init and and/retrieve data primitives.
 * The packages are linked in a list, every package contains a link to the next
 * package that needs to be processed.
 * <p>
 *  To send a message :
 *  - construct object 
 *  - call initMessage
 *  - add some data
 *  - call packMessage
 *  - when sending , get the data using getMessageBody and getMessageLen
 * <p>
 *   When receiving a message :
 *  - construct object
 *  - call setMessageBody to store the data
 *  - get the data in the same order it was added 
 *
 *
 * @version 0.0.1
 *
 */
class DataPacket
{

public:

	virtual ~DataPacket( void )
	{
		;	// do nothing	
	}

	virtual bool addByte(BYTE data) = 0;
	virtual bool getByte(BYTE &data) = 0;

	virtual bool addInt(int data) = 0;
	virtual bool getInt(int& data) = 0;

	virtual bool addFloat(float data) = 0;
	virtual bool getFloat(float &data) = 0;

	virtual bool addShort(short data) = 0;
	virtual bool getShort(short& data) = 0;

	virtual bool addByteLenString(const char* data, BYTE len) = 0;
	virtual bool getByteLenString(char* &data, BYTE& len) = 0;

	virtual bool addByteLenWideString(const wchar_t* data, BYTE len) = 0;
	virtual bool getByteLenWideString(wchar_t* &data, BYTE& len) = 0;

	virtual bool addQueryString(const char* data, short len) = 0;
	virtual bool addShortLenString(const char* data, short len) = 0;
	virtual bool getShortLenString(char* &data, short& len) = 0;

	virtual bool addShortLenWideString(const wchar_t* data, BYTE len) = 0;
	virtual bool getShortLenWideString(wchar_t* &data, BYTE& len) = 0;

	virtual bool addBlob( char* data, short len ){ return false;}
	virtual bool getBlob( char*& data, short & len ){ return false;}

	virtual void addPacket( DataPacket * packet ) = 0;
	virtual void getPacket( DataPacket * &packet ) = 0;

    virtual void packMessage() = 0;

    virtual int getMessageLen() = 0;
    virtual BYTE * getMessageBody() = 0;

    virtual void setMessageBody( BYTE * body, int body_len ) = 0;

    virtual DataPacket* getNextPacket() = 0;
    virtual void setNextPacket(DataPacket* next) = 0;

	virtual int getDataType() = 0;
	virtual void setDataType( int type ) = 0;

	virtual int getPort() = 0;
	virtual void setPort( int port ) = 0;

	virtual char * getIP() = 0;
	virtual void setIP( char * ip ) = 0;

	virtual bool IsSendByTcp() = 0;
	virtual bool IsSendByUdp() = 0;

	virtual void Packet2String( char * buffer, int &len ) = 0;
	virtual int String2Packet( char * buffer, int len ) = 0;

	virtual void InitMessage( int type ){}
	virtual int GetMessageType(){ return -1;}

	virtual  void InitMMOMessage( short type ){}
	  virtual  short GetMMOMessageType(){return -1;}
};

#endif // __DATAPACKET_H__