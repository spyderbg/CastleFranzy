#ifndef __QUERY_CONDITION_H__
#define __QUERY_CONDITION_H__

#include "../../Porting.h"
#include "DataPacket.h"

enum ONLINE_QUERY_OPERATOR
{
	ONLINE_QUERY_EQUAL,			// =
	ONLINE_QUERY_MORE,			// >
	ONLINE_QUERY_LESS,			// <
	ONLINE_QUERY_MORE_OR_EQUAL,	// >=
	ONLINE_QUERY_LESS_OR_EQUAL,	// <=
	ONLINE_QUERY_LIKE,			// %
	ONLINE_QUERY_COUNT,
};

enum ONLINE_QUERY_DATA_TYPE
{
	ONLINE_QUERY_DATA_TYPE_NONE,
	ONLINE_QUERY_DATA_TYPE_BYTE,
	ONLINE_QUERY_DATA_TYPE_SHORT,
	ONLINE_QUERY_DATA_TYPE_INT,
	ONLINE_QUERY_DATA_TYPE_FLOAT,
	ONLINE_QUERY_DATA_TYPE_STRING
};

class OnlineQueryField
{

public:

	OnlineQueryField()
	{
		alias = NULL;
		value = NULL;
		next = NULL;
		length = 0;
	}

	~OnlineQueryField()
	{
		XP_API_DEL ( alias );
		XP_API_DEL ( value );
	}

	char * alias;

	char * value;

	int length;

	OnlineQueryField * next;

};

class CQueryCondition
{

public:

	CQueryCondition(void);

	~CQueryCondition(void);

	bool And( char * alias, ONLINE_QUERY_OPERATOR query_operator, void * value );

	virtual int GetFieldCount() = 0;

	virtual ONLINE_QUERY_DATA_TYPE GetFieldType( char * alias ) = 0;

	virtual ONLINE_QUERY_DATA_TYPE GetFieldAlias( int index, char * alias ) = 0;

	bool ToPacket( DataPacket * packet );

	bool GetConditionString( char * alias, char * value, int &length );

protected:

	bool AddAndByte( char * alias, ONLINE_QUERY_OPERATOR query_operator, BYTE * value );

	bool AddAndShort( char * alias, ONLINE_QUERY_OPERATOR query_operator, short * value );

	bool AddAndInt( char * alias, ONLINE_QUERY_OPERATOR query_operator, int * value );

	bool AddAndFloat( char * alias, ONLINE_QUERY_OPERATOR query_operator, float * value );

	bool AddAndString( char * alias, ONLINE_QUERY_OPERATOR query_operator, char * value );

	OnlineQueryField * GetField( char * alias );

	OnlineQueryField * AddField( char * alias );

	OnlineQueryField * m_condition;

	BYTE Type2String( ONLINE_QUERY_DATA_TYPE type );

};

#endif