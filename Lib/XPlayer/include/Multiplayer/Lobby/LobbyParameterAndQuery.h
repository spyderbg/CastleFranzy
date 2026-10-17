#pragma once

#include "../../Porting.h"
#include "DataPacketLobby.h"

//>>if you wana change this enum, must be notice the new type to service, 
//or not the discode mode in DB will have some error
enum dataTypeT
{
	DT_INT = 0,
	DT_FLOAT,
	DT_STRING
};

enum compareOptT
{
	CMP_CONTAIN = 0X01,					// SUB string
	CMP_EQUAL,							// =
	CMP_NOT_EQUAL,						// !=
	CMP_LESS_THAN,						// <
	CMP_NO_MORE_THAN,					// <=
	CMP_MORE_THAN,						// >
	CMP_NO_LESS_THAN,					// >=
};
//<<end

//>>init your lobby parameter array. max parameter conut 10
const int GAME_PARAMETER_MAX_COUNT = 10;
//<<end

//modify for your game
class CLobbyParameterAndQuery
{
public:

	CLobbyParameterAndQuery();
	
	virtual ~CLobbyParameterAndQuery();

	virtual void SetParameterValue( int index, char* parameterString );

	virtual void AddQueryCondition( int index, compareOptT cmpType, char* conditionString );

	virtual char* PackParameter( int& clength );
	
private:
	int  MY_LOBBY_PARAMETER_DATA_COUNT;
	char MY_LOBBY_PARAMETER_DATA[GAME_PARAMETER_MAX_COUNT];

	char** stringParameter; 
	int    stringParameter_length_1[GAME_PARAMETER_MAX_COUNT];

};
//<<end