#include "StdAfx.h"
#include "MyRoomQueryCondition.h"

char m_field_name[4][10] = { "pitch","substitue","halftime","sameteam" } ;
ONLINE_QUERY_DATA_TYPE m_field_type[4] = 
{
	ONLINE_QUERY_DATA_TYPE_STRING,
	ONLINE_QUERY_DATA_TYPE_INT,
	ONLINE_QUERY_DATA_TYPE_FLOAT,
	ONLINE_QUERY_DATA_TYPE_BYTE
};	

int CMyRoomQueryCondition::GetFieldCount()
{
	return 4;
}

ONLINE_QUERY_DATA_TYPE CMyRoomQueryCondition::GetFieldAlias( int index, char * alias )
{
	XP_API_STRCPY( alias, m_field_name[index] );
	return m_field_type[index];
}

ONLINE_QUERY_DATA_TYPE CMyRoomQueryCondition::GetFieldType( char * alias )
{
	for (int i=0; i<4; i++)
	{
		if ( XP_API_STRCMP( alias, m_field_name[i] ) == 0 )
		{
			return m_field_type[i];
		}
	}

	return ONLINE_QUERY_DATA_TYPE_NONE;
}