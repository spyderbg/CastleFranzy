#ifndef __DEFAULT_DATAPACKET_H__
#define __DEFAULT_DATAPACKET_H__

#include "DataPacket.h"

/**
 * DefaultDataPacket class.
 * <p>
 * The default implementation of DataPacket.
 * <p>
 * If it is not enough, please inherit it to implement your DataPacket.
 *
 *
 * @version 0.0.1
 *
 */
class DefaultDataPacket : public DataPacket
{

public:

    DefaultDataPacket( void );

	DefaultDataPacket( DataPacket * packet );

    ~DefaultDataPacket( void );

public:

	bool addByte(BYTE data);
	bool getByte(BYTE &data);

	bool addInt(int data);
	bool getInt(int& data);

	bool addFloat(float data);
	bool getFloat(float &data);

	bool addShort(short data);
	bool getShort(short& data);

	bool addByteLenString(const char* data, BYTE len);
	bool getByteLenString(char* &data, BYTE& len);

	bool addByteLenWideString(const wchar_t* data, BYTE len);
	bool getByteLenWideString(wchar_t* &data, BYTE &len);

	bool addQueryString(const char* data, short len){return false;}
	bool addShortLenString(const char* data, short len){return false;}
	bool getShortLenString(char* &data, short& len){return false;}

	bool addShortLenWideString(const wchar_t* data, BYTE len){return false;}
	bool getShortLenWideString(wchar_t* &data, BYTE &len){return false;}

	void addPacket( DataPacket * packet );
	void getPacket( DataPacket * &packet );

    void packMessage();

    int getMessageLen();
    BYTE * getMessageBody();

    void setMessageBody( BYTE * body, int body_len );

    DataPacket* getNextPacket();
    void setNextPacket(DataPacket* next);

	int getDataType();
	void setDataType( int type );

	bool IsSendByTcp();
	bool IsSendByUdp();

	int getPort();
	void setPort( int port );

	char * getIP();
	void setIP( char * ip );

	/**
	* Function Packet2String.
	* Generate string by the data, it'll remove the first two bytes which is used to represent length.
	*/
	void Packet2String( char * buffer, int &len );

	/**
	* Function String2Packet.
	* Generate packet by the data, it'll insert two bytes into the header of the data to represent length.
	* @return the total length.
	*/
	int String2Packet( char * buffer, int len );

private:

    BYTE messagebody[ONLINE_MAX_PACKET_SIZE];
    int  currentpos;
    int  messagelen;
    DataPacket * nextPacket;
	int m_data_type;

	char * m_ip;
	int m_port;
};

#endif