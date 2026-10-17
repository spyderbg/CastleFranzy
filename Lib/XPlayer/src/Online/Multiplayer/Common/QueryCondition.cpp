#include "../../../../include/Multiplayer/Common/QueryCondition.h"
//#include "Memory.h"  //byhy for debug memory leak

CQueryCondition::CQueryCondition(void)
{
	m_condition = NULL;
}

CQueryCondition::~CQueryCondition(void)
{
	while( m_condition )
	{
		OnlineQueryField * temp = m_condition;
		m_condition = m_condition->next;
		XP_API_DEL( temp );
	}
}

bool CQueryCondition::ToPacket( DataPacket * packet )
{
	char buffer[ONLINE_MAX_PACKET_SIZE] = { 0 };
	int pos = 0;

	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	
	int count = GetFieldCount();
	for ( int i=0; i<count; i++ )
	{
		char alias[256] = { 0 };
		char value[256] = { 0 };
		int length = 0;

		XP_API_MEMSET( alias, 0, 256 );
		XP_API_MEMSET( value, 0, 256 );
		
		ONLINE_QUERY_DATA_TYPE type = GetFieldAlias( i, alias );

		if ( GetConditionString( alias, value, length ) )
		{
			XP_API_MEMCPY( buffer+pos, value, length );
			pos += length;
			buffer[pos++] = XPLAYER_DATA_SEPERATOR;
		}
		else
		{
			buffer[pos++] = Type2String( type );
			buffer[pos++] = XPLAYER_DATA_SEPERATOR;
		}
	}

	packet->setMessageBody( (BYTE *)buffer, pos );

	return true;
}

bool CQueryCondition::GetConditionString( char * alias, char * value, int &length )
{
	OnlineQueryField * head = m_condition;

	while( head )
	{
		if ( XP_API_STRCMP( head->alias, alias ) == 0 )
		{
			XP_API_MEMCPY( value, head->value, head->length );
			length = head->length;
			return true;
		}

		head = head->next;
	}

	return false;
}

BYTE CQueryCondition::Type2String( ONLINE_QUERY_DATA_TYPE type )
{
	BYTE ret = 0;

	switch( type )
	{
	case ONLINE_QUERY_DATA_TYPE_BYTE:
		ret = 'b';
		break;
	case ONLINE_QUERY_DATA_TYPE_SHORT:
		ret = 's';
		break;
	case ONLINE_QUERY_DATA_TYPE_INT:
		ret = 'i';
		break;
	case ONLINE_QUERY_DATA_TYPE_FLOAT:
		ret = 'f';
		break;
	case ONLINE_QUERY_DATA_TYPE_STRING:
		ret = 't';
		break;
	default:
		break;
	}

	return ret;
}

bool CQueryCondition::And( char * alias, ONLINE_QUERY_OPERATOR query_operator, void * value )
{
	ONLINE_QUERY_DATA_TYPE type = GetFieldType( alias );

	bool bSuccessful = false;

	switch( type )
	{
	case ONLINE_QUERY_DATA_TYPE_BYTE:
		bSuccessful = AddAndByte( alias, query_operator, (BYTE *)value );
		break;
	case ONLINE_QUERY_DATA_TYPE_SHORT:
		bSuccessful = AddAndShort( alias, query_operator, (short *)value );
		break;
	case ONLINE_QUERY_DATA_TYPE_INT:
		bSuccessful = AddAndInt( alias, query_operator, (int *)value );
		break;
	case ONLINE_QUERY_DATA_TYPE_FLOAT:
		bSuccessful = AddAndFloat( alias, query_operator, (float *)value );
		break;
	case ONLINE_QUERY_DATA_TYPE_STRING:
		bSuccessful = AddAndString( alias, query_operator, (char *)value );
		break;
	default:
		break;
	}

	return bSuccessful;
}

OnlineQueryField * CQueryCondition::GetField( char * alias )
{
	if ( NULL == m_condition )
	{
		return NULL;
	}

	OnlineQueryField * head = m_condition;

	while( head )
	{
		if ( XP_API_STRCMP( head->alias, alias ) == 0 )
		{
			return head;
		}

		head = head->next;
	}

	return NULL;
}

OnlineQueryField * CQueryCondition::AddField( char * alias )
{
	OnlineQueryField * field = NULL;

	field = XP_API_NEW OnlineQueryField();
	field->alias = XP_API_STRNEW( alias );

	if ( NULL == m_condition )
	{
		m_condition = field;
	}
	else
	{
		OnlineQueryField * last = m_condition;
		while( last->next )
		{
			last = last->next;
		}
		last->next = field;
	}

	return field;
}

bool CQueryCondition::AddAndByte( char * alias, ONLINE_QUERY_OPERATOR query_operator, BYTE * value )
{
	if ( query_operator == ONLINE_QUERY_MORE )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[7];
			XP_API_MEMSET( field->value, 0, 7 );
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_BYTE );
			field->value[1] = '(';
			field->value[2] = *value;
			field->value[3] = ',';
			field->value[4] = (BYTE)0xFF;
			field->value[5] = ']';
			field->length = 6;
		}
		else
		{
			field->value[1] = '(';
			field->value[2] = *value;
		}
	}
	else if ( query_operator == ONLINE_QUERY_MORE_OR_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[7];
			XP_API_MEMSET( field->value, 0, 7 );
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_BYTE );
			field->value[1] = '[';
			field->value[2] = *value;
			field->value[3] = ',';
			field->value[4] = (BYTE)0xFF;
			field->value[5] = ']';
			field->length = 6;
		}
		else
		{
			field->value[1] = '[';
			field->value[2] = *value;
		}
	}
	else if ( query_operator == ONLINE_QUERY_LESS )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[7];
			XP_API_MEMSET( field->value, 0, 7 );
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_BYTE );
			field->value[1] = '[';
			field->value[2] = 0;
			field->value[3] = ',';
			field->value[4] = *value;
			field->value[5] = ')';
			field->length = 6;
		}
		else
		{
			field->value[4] = *value;
			field->value[5] = ')';
		}
	}
	else if ( query_operator == ONLINE_QUERY_LESS_OR_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[7];
			XP_API_MEMSET( field->value, 0, 7 );
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_BYTE );
			field->value[1] = '[';
			field->value[2] = 0;
			field->value[3] = ',';
			field->value[4] = *value;
			field->value[5] = ']';
			field->length = 6;
		}
		else
		{
			field->value[4] = *value;
			field->value[5] = ']';
		}
	}
	else if ( query_operator == ONLINE_QUERY_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[7];
			XP_API_MEMSET( field->value, 0, 7 );
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_BYTE );
			field->value[1] = '[';
			field->value[2] = *value;
			field->value[3] = ',';
			field->value[4] = *value;
			field->value[5] = ']';
			field->length = 6;
		}
		else
		{
			field->value[1] = '[';
			field->value[2] = *value;
			field->value[4] = *value;
			field->value[5] = ']';
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool CQueryCondition::AddAndShort( char * alias, ONLINE_QUERY_OPERATOR query_operator, short * value )
{
	if ( query_operator == ONLINE_QUERY_MORE )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[9];
			XP_API_MEMSET( field->value, 0, 9 );
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_SHORT );
			field->value[1] = '(';
			field->value[2] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[3] = (BYTE)((*value & 0x000000FF));
			field->value[4] = ',';
			field->value[5] = (BYTE)0x7F;
			field->value[6] = (BYTE)0xFF;
			field->value[7] = ']';
			field->length = 8;
		}
		else
		{
			field->value[1] = '(';
			field->value[2] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[3] = (BYTE)((*value & 0x000000FF));
		}
	}
	else if ( query_operator == ONLINE_QUERY_MORE_OR_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[9];
			XP_API_MEMSET( field->value, 0, 9 );
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_SHORT );
			field->value[1] = '[';
			field->value[2] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[3] = (BYTE)((*value & 0x000000FF));
			field->value[4] = ',';
			field->value[5] = (BYTE)0x7F;
			field->value[6] = (BYTE)0xFF;
			field->value[7] = ']';
			field->length = 8;
		}
		else
		{
			field->value[1] = '[';
			field->value[2] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[3] = (BYTE)((*value & 0x000000FF));
		}
	}
	else if ( query_operator == ONLINE_QUERY_LESS )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[9];
			XP_API_MEMSET( field->value, 0, 9 );
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_SHORT );
			field->value[1] = '[';
			field->value[2] = (BYTE)0x80;
			field->value[3] = (BYTE)0x00;
			field->value[4] = ',';
			field->value[5] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[6] = (BYTE)((*value & 0x000000FF));
			field->value[7] = ')';
			field->length = 8;
		}
		else
		{
			field->value[5] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[6] = (BYTE)((*value & 0x000000FF));
			field->value[7] = ')';
		}
	}
	else if ( query_operator == ONLINE_QUERY_LESS_OR_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[9];
			XP_API_MEMSET( field->value, 0, 9 );
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_SHORT );
			field->value[1] = '[';
			field->value[2] = (BYTE)0x80;
			field->value[3] = (BYTE)0x00;
			field->value[4] = ',';
			field->value[5] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[6] = (BYTE)((*value & 0x000000FF));
			field->value[7] = ']';
			field->length = 8;
		}
		else
		{
			field->value[5] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[6] = (BYTE)((*value & 0x000000FF));
			field->value[7] = ']';
		}
	}
	else if ( query_operator == ONLINE_QUERY_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[9];
			XP_API_MEMSET( field->value, 0, 9 );
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_SHORT );
			field->value[1] = '[';
			field->value[2] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[3] = (BYTE)((*value & 0x000000FF));
			field->value[4] = ',';
			field->value[5] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[6] = (BYTE)((*value & 0x000000FF));
			field->value[7] = ']';
			field->length = 8;
		}
		else
		{
			field->value[1] = '[';
			field->value[2] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[3] = (BYTE)((*value & 0x000000FF));
			field->value[5] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[6] = (BYTE)((*value & 0x000000FF));
			field->value[7] = ']';
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool CQueryCondition::AddAndInt( char * alias, ONLINE_QUERY_OPERATOR query_operator, int * value )
{
	if ( query_operator == ONLINE_QUERY_MORE )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[13];
			XP_API_MEMSET( field->value, 0, 13);
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_INT );
			field->value[1] = '(';
			field->value[2] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[3] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[4] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[5] = (BYTE)((*value & 0x000000FF));
			field->value[6] = ',';
			field->value[7] = (BYTE)0x7F;
			field->value[8] = (BYTE)0xFF;
			field->value[9] = (BYTE)0xFF;
			field->value[10] = (BYTE)0xFF;
			field->value[11] = ']';
			field->length = 12;
		}
		else
		{
			field->value[1] = '(';
			field->value[2] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[3] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[4] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[5] = (BYTE)((*value & 0x000000FF));
		}
	}
	else if ( query_operator == ONLINE_QUERY_MORE_OR_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[13];
			XP_API_MEMSET( field->value, 0, 13);
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_INT );
			field->value[1] = '[';
			field->value[2] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[3] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[4] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[5] = (BYTE)((*value & 0x000000FF));
			field->value[6] = ',';
			field->value[7] = (BYTE)0x7F;
			field->value[8] = (BYTE)0xFF;
			field->value[9] = (BYTE)0xFF;
			field->value[10] = (BYTE)0xFF;
			field->value[11] = ']';
			field->length = 12;
		}
		else
		{
			field->value[1] = '[';
			field->value[2] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[3] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[4] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[5] = (BYTE)((*value & 0x000000FF));
		}
	}
	else if ( query_operator == ONLINE_QUERY_LESS )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[13];
			XP_API_MEMSET( field->value, 0, 13);
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_INT );
			field->value[1] = '[';
			field->value[2] = (BYTE)0x80;
			field->value[3] = (BYTE)0x00;
			field->value[4] = (BYTE)0x00;
			field->value[5] = (BYTE)0x00;
			field->value[6] = ',';
			field->value[7] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[8] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[9] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[10] = (BYTE)((*value & 0x000000FF));
			field->value[11] = ')';
			field->length = 12;
		}
		else
		{
			field->value[7] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[8] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[9] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[10] = (BYTE)((*value & 0x000000FF));
			field->value[11] = ')';
		}
	}
	else if ( query_operator == ONLINE_QUERY_LESS_OR_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[13];
			XP_API_MEMSET( field->value, 0, 13);
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_INT );
			field->value[1] = '[';
			field->value[2] = (BYTE)0x80;
			field->value[3] = (BYTE)0x00;
			field->value[4] = (BYTE)0x00;
			field->value[5] = (BYTE)0x00;
			field->value[6] = ',';
			field->value[7] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[8] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[9] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[10] = (BYTE)((*value & 0x000000FF));
			field->value[11] = ']';
			field->length = 12;
		}
		else
		{
			field->value[7] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[8] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[9] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[10] = (BYTE)((*value & 0x000000FF));
			field->value[11] = ']';
		}
	}
	else if ( query_operator == ONLINE_QUERY_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[13];
			XP_API_MEMSET( field->value, 0, 13);
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_INT );
			field->value[1] = '[';
			field->value[2] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[3] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[4] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[5] = (BYTE)((*value & 0x000000FF));
			field->value[6] = ',';
			field->value[7] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[8] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[9] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[10] = (BYTE)((*value & 0x000000FF));
			field->value[11] = ']';
			field->length = 12;
		}
		else
		{
			field->value[1] = '[';
			field->value[2] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[3] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[4] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[5] = (BYTE)((*value & 0x000000FF));
			field->value[7] = (BYTE)((*value & 0xFF000000)>>24);
			field->value[8] = (BYTE)((*value & 0x00FF0000)>>16);
			field->value[9] = (BYTE)((*value & 0x0000FF00)>>8);
			field->value[10] = (BYTE)((*value & 0x000000FF));
			field->value[11] = ']';
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool CQueryCondition::AddAndFloat( char * alias, ONLINE_QUERY_OPERATOR query_operator, float * value )
{
	if ( query_operator == ONLINE_QUERY_MORE )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[13];
			XP_API_MEMSET( field->value, 0, 13);
			char *p = (char *)value;
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_FLOAT );
			field->value[1] = '(';
			field->value[2] = *(p);
			field->value[3] = *(p+1);
			field->value[4] = *(p+2);
			field->value[5] = *(p+3);
			field->value[6] = ',';
			field->value[7] = (BYTE)0xFF; 
			field->value[8] = (BYTE)0xFF; 
			field->value[9] = (BYTE)0x7F; 
			field->value[10] = (BYTE)0x7F;
			field->value[11] = ']';
			field->length = 12;
		}
		else
		{
			char *p = (char *)value;
			field->value[1] = '(';
			field->value[2] = *(p);
			field->value[3] = *(p+1);
			field->value[4] = *(p+2);
			field->value[5] = *(p+3);
		}
	}
	else if ( query_operator == ONLINE_QUERY_MORE_OR_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[13];
			XP_API_MEMSET( field->value, 0, 13);
			char *p = (char *)value;
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_FLOAT );
			field->value[1] = '[';
			field->value[2] = *(p);
			field->value[3] = *(p+1);
			field->value[4] = *(p+2);
			field->value[5] = *(p+3);
			field->value[6] = ',';
			field->value[7] = (BYTE)0xFF;
			field->value[8] = (BYTE)0xFF;
			field->value[9] = (BYTE)0x7F;
			field->value[10] = (BYTE)0x7F;
			field->value[11] = ']';
			field->length = 12;
		}
		else
		{
			char *p = (char *)value;
			field->value[1] = '[';
			field->value[2] = *(p);
			field->value[3] = *(p+1);
			field->value[4] = *(p+2);
			field->value[5] = *(p+3);
		}
	}
	else if ( query_operator == ONLINE_QUERY_LESS )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[13];
			XP_API_MEMSET( field->value, 0, 13);
			char *p = (char *)value;
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_FLOAT );
			field->value[1] = '[';
			field->value[2] = (BYTE)0x00;
			field->value[3] = (BYTE)0x00;
			field->value[4] = (BYTE)0x00;
			field->value[5] = (BYTE)0x80;
			field->value[6] = ',';
			field->value[7] = *(p);
			field->value[8] = *(p+1);
			field->value[9] = *(p+2);
			field->value[10] = *(p+3);
			field->value[11] = ')';
			field->length = 12;
		}
		else
		{
			char *p = (char *)value;
			field->value[7] = *(p);
			field->value[8] = *(p+1);
			field->value[9] = *(p+2);
			field->value[10] = *(p+3);
			field->value[11] = ')';
		}
	}
	else if ( query_operator == ONLINE_QUERY_LESS_OR_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[13];
			XP_API_MEMSET( field->value, 0, 13);
			char *p = (char *)value;
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_FLOAT );
			field->value[1] = '[';
			field->value[2] = (BYTE)0x00;
			field->value[3] = (BYTE)0x00;
			field->value[4] = (BYTE)0x00;
			field->value[5] = (BYTE)0x80;
			field->value[6] = ',';
			field->value[7] = *(p);
			field->value[8] = *(p+1);
			field->value[9] = *(p+2);
			field->value[10] = *(p+3);
			field->value[11] = ']';
			field->length = 12;
		}
		else
		{
			char *p = (char *)value;
			field->value[7] = *(p);
			field->value[8] = *(p+1);
			field->value[9] = *(p+2);
			field->value[10] = *(p+3);
			field->value[11] = ']';
		}
	}
	else if ( query_operator == ONLINE_QUERY_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
			field->value = XP_API_NEW char[13];
			XP_API_MEMSET( field->value, 0, 13);
			char *p = (char *)value;
			field->value[0] = Type2String( ONLINE_QUERY_DATA_TYPE_FLOAT );
			field->value[1] = '[';
			field->value[2] = *(p);
			field->value[3] = *(p+1);
			field->value[4] = *(p+2);
			field->value[5] = *(p+3);
			field->value[6] = ',';
			field->value[7] = *(p);
			field->value[8] = *(p+1);
			field->value[9] = *(p+2);
			field->value[10] = *(p+3);
			field->value[11] = ']';
			field->length = 12;
		}
		else
		{
			char *p = (char *)value;
			field->value[1] = '[';
			field->value[2] = *(p);
			field->value[3] = *(p+1);
			field->value[4] = *(p+2);
			field->value[5] = *(p+3);
			field->value[7] = *(p);
			field->value[8] = *(p+1);
			field->value[9] = *(p+2);
			field->value[10] = *(p+3);
			field->value[11] = ']';
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool CQueryCondition::AddAndString( char * alias, ONLINE_QUERY_OPERATOR query_operator, char * value )
{
	if ( query_operator == ONLINE_QUERY_EQUAL )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
		}
		else
		{
			XP_API_DEL( field->value );
			field->length = 0;
		}
		int len = XP_API_STRLEN( value );
		int length = len + 3;
		field->value = XP_API_NEW char[length+1];
		XP_API_MEMSET( field->value, 0, length+1);
		XP_API_SPRINTF( field->value, "%c=%c%s", Type2String( ONLINE_QUERY_DATA_TYPE_STRING ), (BYTE)len, value );
		field->length = length;
	}
	else if ( query_operator == ONLINE_QUERY_LIKE )
	{
		OnlineQueryField * field = GetField( alias );
		if ( NULL == field )
		{
			field = AddField( alias );
		}
		else
		{
			XP_API_DEL( field->value );
			field->length = 0;			
		}
		int len = XP_API_STRLEN( value );
		int length = len + 3;
		field->value = XP_API_NEW char[length+1];
		XP_API_MEMSET( field->value, 0, length+1);
		XP_API_SPRINTF( field->value, "%c?%c%s", Type2String( ONLINE_QUERY_DATA_TYPE_STRING ), (BYTE)len, value );
		field->length = length;
	}
	else
	{
		return false;
	}

	return true;
}