#include "../../../../include/Multiplayer/Lobby/LobbyParameterAndQuery.h"


#ifdef OS_IPHONE
#import <stdio.h>
extern void GetConfigFilePath(char* filePath, const char* filename);
#endif

CLobbyParameterAndQuery::CLobbyParameterAndQuery()
{
#ifdef LOBBY_THREAD_TEST
	MY_LOBBY_PARAMETER_DATA_COUNT = 1;
	for(int i = 0; i < GAME_PARAMETER_MAX_COUNT; i++)
	{
		MY_LOBBY_PARAMETER_DATA[i] = 0;
	}
#else
	XP_FILE_HANDLE pFile;
	
#ifdef OS_IPHONE
    char tmp[FILENAME_MAX];
	XP_API_MEMSET(tmp,0,FILENAME_MAX);
	GetConfigFilePath(tmp, XPLAYER_FILE_CONF);
	pFile = XP_API_FILE_OPEN( tmp, "r" );
#else
	pFile = XP_API_FILE_OPEN( XPLAYER_FILE_CONF, "r" );
#endif

	if( !pFile )
	{
		XP_DEBUG_OUT("[CLobbyParameterAndQuery] not found \n", XPLAYER_FILE_CONF );
		return;
	}

	int length = XP_API_FILE_GET_LENGTH( pFile );
	char * buffer = XP_API_NEW char[length+1];
	
	XP_API_MEMSET( buffer, 0, length+1 );
	XP_API_FILE_READ( buffer, length, 1, pFile );

	int line = 0;
	char * data = XP_API_NEW char[length+1];

	XP_API_MEMSET( data, 0, length+1 );
	XP_API_PARSE_DATA( buffer, data, line++, '\n' );
	if (data[XP_API_STRLEN(data)-1] == '\r')
	{
		data[XP_API_STRLEN(data)-1] = 0;
	}
	int len = XP_API_STRLEN( data );
	
	while( len > 0 )
	{
		char name[32] = { 0 };
		char value[256] = { 0 };
		int pos = 0;

		pos = XP_API_PARSE_DATA( data, name, 0, ':' );
		pos = XP_API_PARSE_DATA( data, value, 1, ':' );
		XP_API_MEMSET( value, 0, 256 );
		XP_API_MEMCPY( value, data+pos, XP_API_STRLEN(data)-pos );
		
		XP_API_STRTRIM( (char *) name );
		XP_API_STRTRIM( (char *) value );

		if ( XP_API_STRCMP( name, GAME_PARAMETER_COUNT ) == 0 )
		{
			MY_LOBBY_PARAMETER_DATA_COUNT = XP_API_ATOI(value);

			if(MY_LOBBY_PARAMETER_DATA_COUNT > GAME_PARAMETER_MAX_COUNT)
				MY_LOBBY_PARAMETER_DATA_COUNT = GAME_PARAMETER_MAX_COUNT;
		}

		if ( XP_API_STRCMP( name, GAME_PARAMETER_TYPE ) == 0 )
		{
			// type1|type2|type3|....

			int lenValue = XP_API_STRLEN(value);

			int startIndex = 0;
			int paramIndex = 0, paramStartIndex = 0;
			while(startIndex < lenValue)
			{
				startIndex++;
				
				if((startIndex == lenValue || value[startIndex] == '|') && paramIndex < MY_LOBBY_PARAMETER_DATA_COUNT)
				{
					char valueParam[10] = { 0 };
					XP_API_STRNCPY(valueParam, &value[paramStartIndex], startIndex - paramStartIndex);
					MY_LOBBY_PARAMETER_DATA[paramIndex] = XP_API_ATOI(valueParam);
					paramStartIndex = startIndex + 1;
					paramIndex++;
				}
			}
		}

		XP_API_MEMSET( data, 0, length+1 );
		XP_API_PARSE_DATA( buffer, data, line++, '\n' );
		if (data[XP_API_STRLEN(data)-1] == '\r')
		{
			data[XP_API_STRLEN(data)-1] = 0;
		}		
		len = XP_API_STRLEN( data );
	}

	XP_API_DEL( data );
	XP_API_DEL( buffer );
	XP_API_FILE_CLOSE( pFile );
#endif //LOBBY_THREAD_TEST

	stringParameter = NULL;
	if(MY_LOBBY_PARAMETER_DATA_COUNT != 0)
	{
		stringParameter = XP_API_NEW char*[MY_LOBBY_PARAMETER_DATA_COUNT];

		for( int i = 0; i < MY_LOBBY_PARAMETER_DATA_COUNT; i++ )
		{
			stringParameter[i] = NULL;
			stringParameter_length_1[i] = 0;
		}
	}
}

CLobbyParameterAndQuery::~CLobbyParameterAndQuery()
{
	for( int i = 0; i < MY_LOBBY_PARAMETER_DATA_COUNT; i++ )
	{
		XP_API_DEL( stringParameter[i] );
	}

	XP_API_DEL( stringParameter );
}

void CLobbyParameterAndQuery::SetParameterValue( int index, char* parameterString )
{
	if( index > MY_LOBBY_PARAMETER_DATA_COUNT - 1 || index < 0 )
		return;

	if( parameterString != NULL)
	{
		int len = XP_API_STRLEN( parameterString );
		
		XP_API_DEL( stringParameter[index] );
		stringParameter[index] = XP_API_NEW char[len + 4 + 1];
		XP_API_MEMSET( stringParameter[index], 0, len + 4 + 1 );
		stringParameter_length_1[index] = len + 4;

		int curPos = 0;
		stringParameter[index][curPos] = (BYTE)(index & 0x000000FF);							curPos++;
		stringParameter[index][curPos] = (BYTE)(MY_LOBBY_PARAMETER_DATA[index] & 0x000000FF);   curPos++;

		stringParameter[index][curPos] = (BYTE)((len & 0x0000FF00)>>8);   curPos++;
		stringParameter[index][curPos] = (BYTE)((len & 0x000000FF));	  curPos++;
		XP_API_MEMCPY( &stringParameter[index][curPos], parameterString, len );
	}
}

void CLobbyParameterAndQuery::AddQueryCondition( int index, compareOptT cmpType, char* conditionString )
{
	if( index > MY_LOBBY_PARAMETER_DATA_COUNT - 1 || index < 0 )
		return;

	if( conditionString != NULL)
	{
		int len = XP_API_STRLEN( conditionString );
		
		XP_API_DEL( stringParameter[index] );
		stringParameter[index] = XP_API_NEW char[len + 4 + 1];
		XP_API_MEMSET( stringParameter[index], 0, len + 4 + 1 );
		stringParameter_length_1[index] = len + 4;

		int curPos = 0;
		stringParameter[index][curPos] = (BYTE)(index & 0x000000FF);      curPos++;
		stringParameter[index][curPos] = (BYTE)(cmpType & 0x000000FF);    curPos++;

		stringParameter[index][curPos] = (BYTE)((len & 0x0000FF00)>>8);   curPos++;
		stringParameter[index][curPos] = (BYTE)((len & 0x000000FF));	  curPos++;
		XP_API_MEMCPY( &stringParameter[index][curPos], conditionString, len );
	}
}

char* CLobbyParameterAndQuery::PackParameter( int& clength )
{
	clength = 0;
	int length = 0;
	for( int i = 0; i < MY_LOBBY_PARAMETER_DATA_COUNT; i++ )
	{
		length += stringParameter_length_1[i];
	}

	if( length == 0 )
		return NULL;

	char* parameter = XP_API_NEW char[length + 1 + 1];// one byte for count, another for null
	XP_API_MEMSET( parameter, 0, length + 1 + 1 );

	int curPos = 1, parameterNum = MY_LOBBY_PARAMETER_DATA_COUNT;
	for( int i = 0; i < MY_LOBBY_PARAMETER_DATA_COUNT; i++ )
	{
		int len = stringParameter_length_1[i];

		if( len == 0)
		{
			parameterNum--;
			continue;
		}	

		XP_API_MEMCPY( &parameter[curPos], stringParameter[i], len );
		curPos += len;
	}

	parameter[0] = (BYTE)(parameterNum & 0x000000FF);
	clength = curPos;

	return parameter;
}