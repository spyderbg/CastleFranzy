#include "../../../include/Buddy/GLXPlayerUserFriend.h"
//#include "memory.h"

/**
* GLXPlayerUserFriend constructor.
* Create an instance of GLXPlayerUserFriend.
*/
GLXPlayerUserFriend::GLXPlayerUserFriend(void) : GLXPlayerWebComponent()
{	
	LoadConfig();
	m_http_engine = XP_API_NEW GLXPlayerHttp( m_http_server_address, m_http_php_version, m_http_php_address );
	
	m_friendsCount = 0;
	m_usernumber = NULL;
	m_name = NULL;
	m_state = NULL;
	m_nationality = NULL;
	m_reputation = NULL;
	m_badEvaluationCount = NULL;
	m_goodEvaluationCount = NULL;
	m_current_game = NULL;
	m_trophy = NULL;
	m_totalTrophyScore = NULL;
	m_language = NULL;
	m_avatarLastUpdate = NULL;
	m_avatarKeys = NULL;
}

/**
* GLXPlayerUserFriend destructor.
* Destroy an instance of GLXPlayerUserFriend.
*/
GLXPlayerUserFriend::~GLXPlayerUserFriend(void)
{
	clearFriendsList();
}

/**
* Function GetFriendsCount.
* Return number of friend in current friends list.	
* @see sendGetUserFriendsCount()
* @see processFriendsList()
* @return m_friendsCount
*/
int GLXPlayerUserFriend::GetFriendsCount()
{
	return m_friendsCount;
}

/**
* Function GetUserNumber.
* Return name of friend in current friends list, indicated by position.	
* @param position Position of friend in the list.	
* @see sendGetUserFriends()	
* @see processFriendsList()	
* @return Friend name at the position indicated.
*/
char * GLXPlayerUserFriend::GetUserNumber(int position)
{
	if(m_usernumber == NULL || position < 0 || position > m_friendsCount)
	{
		return NULL;
	}
	return m_usernumber[position];
}

/**
* Function GetName.
* Return name of friend in current friends list, indicated by position.	
* @param position Position of friend in the list.	
* @see sendGetUserFriends()	
* @see processFriendsList()	
* @return Friend name at the position indicated.
*/
char * GLXPlayerUserFriend::GetName(int position)
{
	if(m_name == NULL || position < 0 || position > m_friendsCount-1)
	{
		return NULL;
	}
	return m_name[position];
}

/**
* Function GetCountry.
* Return country of friend in current friends list, indicated by position.	
* @param position Position of friend in the list.	
* @see sendGetUserFriends()	
* @see processFriendsList()	
* @return Friend country at the position indicated.
*/
char * GLXPlayerUserFriend::GetCountry(int position)
{
	if(m_nationality == NULL || position < 0 || position > m_friendsCount)
	{
		return NULL;
	}
	return m_nationality[position];
}

/**
* Function GetReputation.
* Return reputation of friend in current friends list, indicated by position.	
* @param position Position of friend in the list.	
* @see sendGetUserFriends()	
* @see processFriendsList()	
* @return Friend reputation at the position indicated.
*/
double GLXPlayerUserFriend::GetReputation(int position)
{
	if(m_reputation == NULL || position < 0 || position > m_friendsCount)
	{
		return -1;
	}

	return m_reputation[position];
}

int GLXPlayerUserFriend::GetBadReputationCount(int position)
{
	if(m_badEvaluationCount == NULL || position < 0 || position > m_friendsCount)
	{
		return -1;
	}

	return m_badEvaluationCount[position];
}

int GLXPlayerUserFriend::GetGoodReputationCount(int position)
{
	if(m_goodEvaluationCount == NULL || position < 0 || position > m_friendsCount)
	{
		return -1;
	}

	return m_goodEvaluationCount[position];
}

/**
* Function GetState.
* Return state of friend in current friends list, indicated by position.	
* @param position Position of friend in the list.	
* @see sendGetUserFriends()	
* @see processFriendsList()	
* @return Friend state at the position indicated.
*/
int GLXPlayerUserFriend::GetState(int position)
{
	if(m_state == NULL || position < 0 || position > m_friendsCount)
	{
		return -1;
	}
	return m_state[position];
}

/**
* Function GetCurrentGame.
* Return current game of friend in current friends list, indicated by position.	
* @param position Position of friend in the list.	
* @see sendGetUserFriends()	
* @see processFriendsList()	
* @return Friend current game at the position indicated.
*/
char * GLXPlayerUserFriend::GetCurrentGame(int position)
{
	if(m_current_game == NULL || position < 0 || position > m_friendsCount)
	{
		return NULL;
	}
	return m_current_game[position];
}

/**
* Function GetTrophy.
*  Return trophy of friend in current friends list, indicated by position.	
* @param position Position of friend in the list.	
* @see sendGetUserFriends()	
* @see processFriendsList()	
* @return Friend trophy game at the position indicated.
*/
int GLXPlayerUserFriend::GetTrophy(int position)
{
	if(m_trophy == NULL || position < 0 || position > m_friendsCount)
	{
		return -1;
	}
	return m_trophy[position];
}

/**
* Function GetTotalTrophyScore.
*  Return total trophy score of friend in current friends list, indicated by position.	
* @param position Position of friend in the list.	
* @see sendGetUserFriends()	
* @see processFriendsList()	
* @return Friend trophy game at the position indicated.
*/
int GLXPlayerUserFriend::GetTotalTrophyScore(int position)
{
	if(m_totalTrophyScore == NULL || position < 0 || position > m_friendsCount)
	{
		return -1;
	}
	return m_totalTrophyScore[position];
}

/**
* Function GetLanguage.
*  Return last language of friend in current friends list, indicated by position.	
*
*/
char * GLXPlayerUserFriend::GetLanguage(int position)
{
	if(m_language == NULL || position < 0 || position > m_friendsCount)
	{
		return NULL;
	}
	return m_language[position];
}

/**
* Function GetAvatarLastUpdate().
*  Return friend's avatar last update.
*
*/
char* GLXPlayerUserFriend::GetAvatarLastUpdate(int position)
{
	if(m_avatarLastUpdate == NULL || position < 0 || position > m_friendsCount)
	{
		return NULL;
	}
	return m_avatarLastUpdate[position];
}

/**
* Function GetAvatar().
*  Return friend's avatar.
*
*/
char* GLXPlayerUserFriend::GetAvatarKey(int position)
{
	if(m_avatarKeys == NULL || position < 0 || position > m_friendsCount)
	{
		return NULL;
	}
	return m_avatarKeys[position];
}

///////////////////////////////////
///@ Private functions - OnUpdateSuccess
///////////////////////////////////

/**
* Private function OnUpdateSuccess.
* Handle success message returned from the Web server.	
* @param func_id Function ID.
* @return True / False.
*/
bool GLXPlayerUserFriend::OnUpdateSuccess( int func_id )
{
	switch(func_id)
	{
	case FUNCTION_GET_USER_FRIENDS_COUNT:
		{
			clearFriendsList();			
			char tmp[8] = {0};
			getValue(m_pResponsePtr, tmp, 0, '|');
			m_friendsCount = XP_API_ATOI(tmp);					
		}
		break;
	case FUNCTION_GET_USER_FRIENDS:			
		processFriendsList(m_pResponsePtr);
		break;

	default:			
		break;
	}
	return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
}

///////////////////////////////////
///@ Public functions - Transport functions
///////////////////////////////////

/**
* Function sendAddUserFriend.
* Add a friend and wait for friend confirmation.
* @param userName Name of user to add as a friend.
* @param msg Option parameter. Invitation message.
* @return Success: |r|s / Fail:  |r|e|error code
*/
#ifdef USE_USER_NUMBER
bool GLXPlayerUserFriend::sendAddUserFriend(char* userName, char * msg, bool isUserNumber)
{
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	
	if(isUserNumber)
	{
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|un|%s|", FUNCTION_ADD_USER_FRIEND, m_ggi, m_uid, userName);
	}
	else
	{
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|n|%s|", FUNCTION_ADD_USER_FRIEND, m_ggi, m_uid, userName);
	}
	
	if(msg != NULL)
	{
		char subMsg[256] = {0};
		XP_API_SPRINTF(subMsg, "m|%s|", msg);
		XP_API_STRCAT(buffer, subMsg);		
	}
	XP_DEBUG_OUT("GLXPlayerUser::sendAddUserFriend before String2Blob -> buffer = %s\n", buffer);	

	return SendByGet( buffer );	
}
#else
bool GLXPlayerUserFriend::sendAddUserFriend(char* userName, char * msg)
{
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
	
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|n|%s|", FUNCTION_ADD_USER_FRIEND, m_ggi, m_uid, userName);	
	
	if(msg != NULL)
	{
		char subMsg[256] = {0};
		XP_API_SPRINTF(subMsg, "m|%s|", msg);
		XP_API_STRCAT(buffer, subMsg);		
	}
	XP_DEBUG_OUT("GLXPlayerUser::sendAddUserFriend before String2Blob -> buffer = %s\n", buffer);	

	return SendByGet( buffer );	
}
#endif //USE_USER_NUMBER
/**
* Function sendConfirmUserFriend.
* Allow or refuse to add current user as a friend.
* @param userName Name of user to add as a friend.
* @param confirmValue 1: Allow, 0: Refuse. 
* @see sendAddUserFriend() 
* @return Success: |r|s / Fail: |r|e|error code
*/
bool GLXPlayerUserFriend::sendConfirmUserFriend(char* userName ,int confirmValue)
{
	char buffer[ONLINE_MAX_PACKET_SIZE];
	if(confirmValue != 0)
	{
		confirmValue = 1;
	}
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
#ifdef	USE_USER_NUMBER
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|un|%s|c|%d|", FUNCTION_CONFIRM_USER_FRIEND, m_ggi, m_uid, userName, confirmValue);
#else
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|n|%s|c|%d|", FUNCTION_CONFIRM_USER_FRIEND, m_ggi, m_uid, userName, confirmValue);
#endif //USE_USER_NUMBER
	XP_DEBUG_OUT("GLXPlayerUser::sendConfirmUserFriend before String2Blob -> buffer = %s\n", buffer);	

	return SendByGet( buffer );	
}

/**
* Function sendDelUserFriend.
* Delete one of my friend.
* @param userName Name of user to delete.	
* @return Success: |r|s / Fail: |r|e|error code
*/
bool GLXPlayerUserFriend::sendDelUserFriend(char* userName)
{
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
#ifdef	USE_USER_NUMBER
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|un|%s|", FUNCTION_DEL_USER_FRIEND, m_ggi, m_uid, userName);
#else
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|n|%s|", FUNCTION_DEL_USER_FRIEND, m_ggi, m_uid, userName);
#endif //USE_USER_NUMBER
	XP_DEBUG_OUT("GLXPlayerUser::sendDelUserFriend before String2Blob -> buffer = %s\n", buffer);	

	return SendByGet( buffer );	
}

/**
* Function sendGetUserFriends.
* Get user's friend list.
* @param startIndex Start index to read..	
* @param count Read count.	
* @see processFriendsList()	
* @return Success: |r|s|c|total friends count|l|friend list / Fail: |r|e|error code
*/
bool GLXPlayerUserFriend::sendGetUserFriends(int startIndex, int count)
{
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|a|%d|c|%d|", FUNCTION_GET_USER_FRIENDS, m_ggi, m_uid, startIndex, count);
	XP_DEBUG_OUT("GLXPlayerUser::sendGetUserFriends before String2Blob -> buffer = %s\n", buffer);	

	return SendByGet( buffer );	
}


/**
* Function sendGetUserFriendsCount.
* Get user's friends count.	
* @return Success: |r|s|friends count / Fail: |r|e|error code
*/
bool GLXPlayerUserFriend::sendGetUserFriendsCount()
{
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_USER_FRIENDS_COUNT, m_ggi, m_uid);
	XP_DEBUG_OUT("GLXPlayerUser::sendGetUserFriendsCount before String2Blob -> buffer = %s\n", buffer);	

	return SendByGet( buffer );	
}

///////////////////////////////////
///@ Private functions
///////////////////////////////////

/**
* Private function clearFriendsList.
* Clear user's friend list.	
*/
void GLXPlayerUserFriend::clearFriendsList()
{
	XP_API_DEL(m_reputation);
	XP_API_DEL(m_badEvaluationCount);
	XP_API_DEL(m_goodEvaluationCount);
	XP_API_DEL(m_state);
	XP_API_DEL(m_trophy);
	XP_API_DEL(m_totalTrophyScore);

	XP_API_DEL_ARRAY(m_usernumber, m_friendsCount);
	XP_API_DEL_ARRAY(m_name, m_friendsCount);
	XP_API_DEL_ARRAY(m_nationality, m_friendsCount);
	XP_API_DEL_ARRAY(m_current_game, m_friendsCount);
	XP_API_DEL_ARRAY(m_language, m_friendsCount);
	XP_API_DEL_ARRAY(m_avatarLastUpdate, m_friendsCount);
	XP_API_DEL_ARRAY(m_avatarKeys, m_friendsCount);
	m_friendsCount = 0;
}

/**
* Private function processFriendsList.
* Process friend list if success message returned from the server.
* @see sendGetUserFriends()	
* @param data Data returned from the server.
*/
void GLXPlayerUserFriend::processFriendsList(char* data)
{
	clearFriendsList();

	if(data == NULL || XP_API_STRLEN(data) <= 0)
	{
		return;
	}
	char tmpString[512] = {0};	
	char tmp[256] = {0};
	int count = 0;

	//|c|total friends count|l|friend list
	getValue(data, tmp, count++, '|'); //'c'

	XP_API_MEMSET(tmp, 0, 256);	
	getValue(data, tmp, count++, '|');//'count'	

	m_friendsCount = XP_API_ATOI(tmp);
	XP_API_MEMSET(tmp, 0, 256);	

	if( m_friendsCount == 0 ) return;

	int y = 0, n = 0, j = 0;

	//For reputation value:
	//Reputation format: reputation value,bad evaluation count,good evaluation count.
	m_reputation = XP_API_NEW double[m_friendsCount];
	m_badEvaluationCount = XP_API_NEW int[m_friendsCount];
	m_goodEvaluationCount = XP_API_NEW int[m_friendsCount];
	
	m_state = XP_API_NEW int[m_friendsCount];	
	m_trophy = XP_API_NEW int [m_friendsCount];
	m_totalTrophyScore = XP_API_NEW int [m_friendsCount];

	m_usernumber = XP_API_NEW char* [m_friendsCount];
	m_name = XP_API_NEW char* [m_friendsCount];	
	m_nationality = XP_API_NEW char* [m_friendsCount];
	m_current_game = XP_API_NEW char* [m_friendsCount];
	m_language = XP_API_NEW char* [m_friendsCount];
	m_avatarLastUpdate = XP_API_NEW char* [m_friendsCount];
	m_avatarKeys = XP_API_NEW char* [m_friendsCount];

	XP_API_MEMSET(tmp, 0, 256);
	getValue(data, tmp, count++, '|');//'l'

	count = 0;

	//|1st friend's username^1st friend's country^1st friend's reputation^ 1st friend's state^ 1st friend's currentGame^ 1st friend's trophy|

	for(int i = 0; i < m_friendsCount; i++)
	{
		XP_API_MEMSET(tmpString, 0, 512);
		getValue(data, tmpString, i + 3, '|');

		m_usernumber[i] = XP_API_NEW char[128];
		m_name[i] = XP_API_NEW char[128];
		m_nationality[i] = XP_API_NEW char[128];
		m_current_game[i] = XP_API_NEW char[128];		
		m_language[i] = XP_API_NEW char[3];		
		m_avatarLastUpdate[i] = XP_API_NEW char[32];		
		m_avatarKeys[i] = XP_API_NEW char[32];		

		XP_API_MEMSET(m_usernumber[i], 0, 128);
		XP_API_MEMSET(m_name[i], 0, 128);
		XP_API_MEMSET(m_nationality[i], 0, 128);
		XP_API_MEMSET(m_current_game[i], 0, 128);		
		XP_API_MEMSET(m_language[i], 0, 3);		
		XP_API_MEMSET(m_avatarLastUpdate[i], 0, 32);		
		XP_API_MEMSET(m_avatarKeys[i], 0, 32);		

		getValue(tmpString, m_usernumber[i], count++, '^');		
		getValue(tmpString, m_name[i], count++, '^');		
		getValue(tmpString, m_nationality[i], count++, '^');

		XP_API_MEMSET(tmp, 0, 256);
		getValue(tmpString, tmp, count++, '^');
		
		//Reputation
		char tmpReputationStr[128] = {0};
		XP_API_MEMSET(tmpReputationStr, 0, 128);
		getValue(tmp, tmpReputationStr, 0, ',');
		m_reputation[i] = XP_API_ATOF(tmpReputationStr);
		
		XP_API_MEMSET(tmpReputationStr, 0, 128);
		getValue(tmp, tmpReputationStr, 1, ',');
		m_badEvaluationCount[i] = XP_API_ATOI(tmpReputationStr);
		
		XP_API_MEMSET(tmpReputationStr, 0, 128);
		getValue(tmp, tmpReputationStr, 2, ',');
		m_goodEvaluationCount[i] = XP_API_ATOI(tmpReputationStr);

		XP_API_MEMSET(tmp, 0, 256);		
		getValue(tmpString, tmp, count++, '^');
		m_totalTrophyScore[i] = XP_API_ATOI(tmp);
		
		XP_API_MEMSET(tmp, 0, 256);
		getValue(tmpString, tmp, count++, '^');
		m_state[i] = XP_API_ATOI(tmp);

		getValue(tmpString, m_current_game[i], count++, '^');

		XP_API_MEMSET(tmp, 0, 256);		
		getValue(tmpString, tmp, count++, '^');
		m_trophy[i] = XP_API_ATOI(tmp);		

		getValue(tmpString, m_language[i], count++, '^');
		
		getValue(tmpString, m_avatarKeys[i], count++, '^');
		
		getValue(tmpString, m_avatarLastUpdate[i], count++, '^');
		
		count = 0;
	}
}