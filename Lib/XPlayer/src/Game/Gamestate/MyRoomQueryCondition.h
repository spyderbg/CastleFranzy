#pragma once
#include "../../include/Multiplayer/Common/querycondition.h"

class CMyRoomQueryCondition :
	public CQueryCondition
{

public:

	int GetFieldCount();

	ONLINE_QUERY_DATA_TYPE GetFieldType( char * alias );

	ONLINE_QUERY_DATA_TYPE GetFieldAlias( int index, char * alias );

};
