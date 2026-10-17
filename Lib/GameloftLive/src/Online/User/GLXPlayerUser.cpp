#include "GLXPlayerUser.h"

/**
* GLXPlayerUser constructor.
* Create an instance of GLXPlayerUser.
*/

namespace gllive
{
	GLXPlayerUser::GLXPlayerUser( ): GLXPlayerWebComponent()
	{
		m_userNumber = NULL;
		m_userName = NULL;
		m_recommendedUsername = NULL;
		m_password = NULL;
		m_email = NULL;
		m_country = NULL;
		m_firstName = NULL;
		m_lastName = NULL;
		m_birthday = NULL;	
		m_onlineTime = 0;	
		m_totalTrophy = 0;	
		m_emailFlag = false;
		m_sex = false;	
		
		m_userGameListPID = NULL;
		m_userGameListName = NULL;
		m_userGameListTrophy = NULL;
		m_numberUserGameList = 0;
		m_userAvatarIdList = NULL;
		m_userAvatarUpdateTimeList = NULL;
		m_userAvatarStateList = NULL;
		m_userAvatarListCount = 0;
		m_reputation = 0;
		m_badEvaluationCount = 0;
		m_goodEvaluationCount = 0;
		
		m_currentGamePID = NULL;
		m_currentGameName = NULL;	
		m_currentGameTrophy = -1;	
		m_userState = false;
		
		m_userBestScore = -1;	
		m_userAvatar = NULL;
		m_userAvatarUpdateTime = NULL;
		m_userData = NULL;
		m_otherUserName = NULL;

		m_gameIconData = NULL;
		m_gameAttention = NULL;
		XP_API_MEMSET(m_gameIconType, 0, 8);
		m_gameIconUpdateTime = 0;
		m_avatarId = NULL;
		m_avatarUpdateTime = NULL;
	}

	/**
	* GLXPlayerUser destructor.
	* Destroy an instance of GLXPlayerUser.
	*/
	GLXPlayerUser::~GLXPlayerUser(void)
	{
		clearUserGameList();
		clearUserState();
		clearUserInformation();
		clearUserAvatarList();
		XP_API_DELETE_ARRAY(m_password);
		XP_API_DELETE_ARRAY(m_userAvatar);
		XP_API_DELETE_ARRAY(m_userAvatarUpdateTime);
		XP_API_DELETE_ARRAY(m_userData);
		XP_API_DELETE_ARRAY(m_otherUserName);
		XP_API_DELETE_ARRAY(m_recommendedUsername);
		XP_API_DELETE_ARRAY(m_gameIconData);
		XP_API_DELETE_ARRAY(m_gameAttention);
	}

	/**
	* Function getUserName.
	* Return User name.	
	* @see processUserInformation()
	* @return User name
	*/
	char* GLXPlayerUser::getUserName()
	{
		return m_userName;
	}

	/**
	* Function getAvatarId.
	* Return User current avatarId.	
	* @see processUserInformation()
	* @return User current avatarId
	*/
	char* GLXPlayerUser::getAvatarId()
	{
		return m_avatarId;
	}

	/**
	* Function getAvatarUpdateTime.
	* Return User avatar's last update time.	
	* @see processUserInformation()
	* @return User avatar's last update time
	*/
	char* GLXPlayerUser::getAvatarUpdateTime()
	{
		return m_avatarUpdateTime;
	}

	/**
	* Function getPassword.
	* Return User password.		
	* @return User name
	*/
	char* GLXPlayerUser::getPassword()
	{
		return m_password;
	}

	/**
	* Function setPassword.
	* Return User name.	
	* @param  password User password.	
	*/
	void GLXPlayerUser::setPassword( char * password)
	{
		XP_API_DEL(m_password);
		m_password = XP_API_STRNEW(password);
	}

	/**
	* Function getEmail.
	* Return User email.	
	* @see processUserInformation()
	* @return User email.
	*/
	char* GLXPlayerUser::getEmail()
	{
		return m_email;
	}

	/**
	* Function getEmailFlag.
	* Return User email flag.	
	* @see processUserInformation()
	* @return User email flag.
	*/
	bool GLXPlayerUser::getEmailFlag()
	{
		return m_emailFlag;
	}

	/**
	* Function getCountry.
	* Return User country.	
	* @see processUserInformation()
	* @return User country.
	*/
	char* GLXPlayerUser::getCountry()
	{
		return m_country;
	}

	/**
	* Function getFirstName.
	* Return User first name.	
	* @see processUserInformation()
	* @return User first name.
	*/
	char* GLXPlayerUser::getFirstName()
	{
		return m_firstName;
	}

	/**
	* Function getLastName.
	* Return User last name.	
	* @see processUserInformation()
	* @return User last name.
	*/
	char* GLXPlayerUser::getLastName()
	{
		return m_lastName;
	}

	/**
	* Function getSex.
	* Return User sex.	
	* @see processUserInformation()
	* @return User sex.
	*/
	bool GLXPlayerUser::getSex()
	{
		return m_sex;
	}

	/**
	* Function getBirthday.
	* Return User birthday.	
	* @see processUserInformation()
	* @return User birthday.
	*/
	char* GLXPlayerUser::getBirthday()
	{
		return m_birthday;
	}

	/**
	* Function getOnlineTime.
	* Return online time.	
	* @see processUserInformation()
	* @return m_onlineTime.
	*/
	long GLXPlayerUser::getOnlineTime()
	{
		return m_onlineTime;
	}

	/**
	* Function getTotalTrophy.
	* Return total trophy in all games.	
	* @see processUserInformation()
	* @return m_totalTrophy.
	*/
	int GLXPlayerUser::getTotalTrophy()
	{
		return m_totalTrophy;
	}

	void GLXPlayerUser::setTotalTrophy(int score)
	{
		m_totalTrophy = score;
	}

	/**
	* Function getNumberUserGameList.
	* Count number of game in game list.	
	* @see processUserGameList()
	* @return Number of game in game list.
	*/
	int GLXPlayerUser::getNumberUserGameList()
	{
		return m_numberUserGameList;
	}

	/**
	* Function getNumberAvatarGameList.
	* Count number of game in game list.	
	* @see processUseAvatarList()
	* @return Number of avatar list.
	*/
	int GLXPlayerUser::getNumberAvatarList()
	{
		return m_userAvatarListCount;
	}

	/**
	* Function getUserGamePID.
	* Get the PID of game in the game list, indicated by index.	
	* @param index Index of game in game list.
	* @see processUserGameList()
	* @return Number of game in game list.
	*/
	char* GLXPlayerUser::getUserGamePID(int index)
	{
		if(index < 0 || m_userGameListPID == NULL || m_userGameListPID[index] == NULL)
		{
			return NULL;
		}
		return m_userGameListPID[index];
	}

	/**
	* Function getUserGameName.
	* Get the name of game in the game list, indicated by index.	
	* @param index Index of game in game list.
	* @see processUserGameList()
	* @return Name of game in game list.
	*/
	char* GLXPlayerUser::getUserGameName(int index)
	{
		if(index < 0 || m_userGameListName == NULL || m_userGameListName[index] == NULL)
		{
			return NULL;
		}
		return m_userGameListName[index];
	}

	/**
	* Function .
	* Get user score of game in the game list, indicated by index.	
	* @param index Index of game in game list.
	* @see processUserGameList()
	* @return User score.
	*/
	int GLXPlayerUser::getUserGameTrophy(int index)
	{
		if(index < 0 || m_userGameListTrophy == NULL)
		{
			return -1;
		}
		return m_userGameListTrophy[index];
	}

	/**
	* Function getUserAvatarState.
	* Get the avatarId in the game list, indicated by index.	
	* @param index Index of avatar list.
	* @see processUseAvatarList()
	* @return state.
	*/
	int GLXPlayerUser::getUserAvatarState(int index)
	{
		if(index < 0 || m_userAvatarListCount == NULL)
		{
			return -1;
		}
		return m_userAvatarStateList[index];
	}

	/**
	* Function getUserAvatarUpdateTime.
	* Get the avatarId in the game list, indicated by index.	
	* @param index Index of avatar list.
	* @see processUseAvatarList()
	* @return last update time.
	*/
	char* GLXPlayerUser::getUserAvatarUpdateTime(int index)
	{
		if(index < 0 || m_userAvatarListCount == NULL)
		{
			return NULL;
		}
		return m_userAvatarUpdateTimeList[index];
	}

	/**
	* Function getUserAvatar.
	* Get the avatarId in the game list, indicated by index.	
	* @param index Index of avatar list.
	* @see processUseAvatarList()
	* @return id.
	*/
	char* GLXPlayerUser::getUserAvatarId(int index)
	{
		if(index < 0 || m_userAvatarListCount == NULL)
		{
			return NULL;
		}
		return m_userAvatarIdList[index];
	}

	/**
	* Function getCurrentGamePID.
	* Get current game PID in the user state returned by server.
	* @see processUserState()
	* @return Game PID.
	*/
	char* GLXPlayerUser::getCurrentGamePID()
	{
		return m_currentGamePID;
	}

	/**
	* Function getCurrentGameName.
	* Get current game name in the user state returned by server.
	* @see processUserState()
	* @return Game name.
	*/
	char* GLXPlayerUser::getCurrentGameName()
	{
		return m_currentGameName;
	}

	/**
	* Function getCurrentGameTrophy.
	* Get current user score in the user state returned by server.
	* @see processUserState()
	* @return User score.
	*/
	int GLXPlayerUser::getCurrentGameTrophy()
	{
		return m_currentGameTrophy;
	}

	/**
	* Function getCurrentUserState.
	* Get current user state in the user state returned by server.
	* @see processUserState()
	* @return User state.
	*/
	bool GLXPlayerUser::getCurrentUserState()
	{
		return m_userState;
	}

	/**
	* Function getUserReputation.
	* Get user reputation.
	* @see processUserReputation()
	* @return User reputation.
	*/
	double GLXPlayerUser::getUserReputation()
	{
		return m_reputation;
	}

	int GLXPlayerUser::getUserBadReputationCount()
	{
		return m_badEvaluationCount;
	}

	int GLXPlayerUser::getUserGoodReputationCount()
	{
		return m_goodEvaluationCount;
	}

	/**
	* Function getUserTrophy.
	* Get user trophy.
	* @see processUserTrophy()
	* @return User trophy.
	*/
	int GLXPlayerUser::getUserTrophy()
	{
		return m_trophy;
	}

	/**
	* Function getUserBestScore.
	* Get user best score.
	* @see processUserBestScore()
	* @return User best score.
	*/	
	int GLXPlayerUser::getUserBestScore()
	{
		return m_userBestScore;
	}

	/**
	* Function getOtherUserAvatar.
	* Get user avatar.
	* @see processUserAvatar()
	* @return User avatar.
	*/
	char* GLXPlayerUser::getOtherUserAvatar()
	{
		return m_userAvatar;
	}

	/**
	* Function getOtherUserAvatarTime.
	* Get user avatar last update time.
	* @see processUserAvatar()
	* @return User avatar.
	*/
	char* GLXPlayerUser::getOtherUserAvatarTime()
	{
		return m_userAvatarUpdateTime;
	}

	/**
	* Function getUserAvatar.
	* Get user data.
	* @see processUserData()
	* @return User data.
	*/
	char* GLXPlayerUser::getUserData()
	{
		return m_userData;
	}


	/**
	* Function getOtherUserName.
	* Get other user name in some function to get other user properties. If this function return NULL, then the properties are owned by current user.	
	* @return Other user name.
	*/
	char* GLXPlayerUser::getOtherUserName()
	{
		return m_otherUserName;
	}

	/**
	* Function sendRegister.
	* Register an UID in web server.
	* @param name User name.
	* @param password User password to login.
	* @param email Registration email, use to login to chat component and to receive new email from Gameloft.	
	* @param country User country.	
	* @param emailFlag User can consider to receive email from Gameloft or not.	
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendRegister( char * name, char * password, char * email, char *country, bool emailFlag, char* deviceToken, char* lang, char* UDID)
	{
		if(name == NULL || password == NULL || email == NULL || country == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_REGISTER, ERROR_INIT );
			return false;
		}
		
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE);
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%s|e|%s|c|%s|ef|%d|o|1|l|%s|", FUNCTION_REGISTER, m_ggi, name, password, email, country, emailFlag ? 1 : 0, lang);	
		
		if(deviceToken != NULL)
		{
			char subMsg[128] = {0};
			XP_API_MEMSET(subMsg, 0, 128);
			XP_API_SPRINTF(subMsg, "d|%s|", deviceToken); 	
			XP_API_STRCAT(buffer, subMsg);
		}
		if(UDID)
		{
			char temp[128] = {0};
			XP_API_SPRINTF(temp, "udid|%s|", UDID);
			XP_API_STRCAT(buffer, temp);
		}

		XP_DEBUG_OUT("GLXPlayerUser::sendRegister before String2Blob -> buffer = %s\n", buffer);

		return SendByGet( buffer );
	}

	/**
	* Function sendGetCountry.
	* Get user country.
	* @param userName User name.	
	* @param nickName Other user name, optional parameter.	
	* @param nickNameFlag Nick name flag, optional parameter.	
	* @return Success: |r|s|country or |r|s|n|nick name|country / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendGetCountry(char * userName, char * nickName, bool nickNameFlag)
	{
		if(userName == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_CHANGE_COUNTRY, ERROR_INIT );
			return false;
		}
		
		char buffer[ONLINE_MAX_PACKET_SIZE];

		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		if(nickName != NULL)
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|n|%s|nf|%d|", FUNCTION_GET_COUNTRY, m_ggi, userName, nickName, nickNameFlag ? 1 : 0);
		}
		else
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|nf|%d|", FUNCTION_GET_COUNTRY, m_ggi, userName, nickNameFlag ? 1 : 0);
		}
		XP_DEBUG_OUT("GLXPlayerUser::sendGetCountry before String2Blob -> buffer = %s\n", buffer);
		
		return SendByGet( buffer );
	}

	/**
	* Function sendChangeCountry.
	* Changecuser country.
	* @param userName User name.
	* @param password User password to login.
	* @param country New country.	
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendChangeCountry( char* userName, char* password, char * country )
	{
		if(userName == NULL || password == NULL || country == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_CHANGE_COUNTRY, ERROR_INIT );
			return false;
		}
		
		char buffer[ONLINE_MAX_PACKET_SIZE];

		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%s|nc|%s|", FUNCTION_CHANGE_COUNTRY, m_ggi, userName, password, country);
		XP_DEBUG_OUT("GLXPlayerUser::sendChangeCountry before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendGetUsedApp.
	* Get the list of user's played application..
	* @param userName User name, optional parameter, if don't set, decided by uid.		
	* @return Success: |r|s|app list  or |r|s|n|name|app list / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendGetUsedApp(char* userName)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		
		if(userName == NULL)
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_USED_APPS, m_ggi, m_uid);
		}
		else
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_USED_APPS, m_ggi, userName);
		}
			
		XP_DEBUG_OUT("GLXPlayerUser::sendGetUsedApp before String2Blob -> buffer = %s\n", buffer);
		
		return SendByGet( buffer );
	}

	/**
	* Function sendChangeUserName.
	* Change login username.
	* @param userName User name.
	* @param password User password to login.
	* @param newUserName New user name.
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendChangeUserName( char* userName, char* password, char * newUserName )
	{
		if(userName == NULL || password == NULL || newUserName == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_CHANGE_USERNAME, ERROR_INIT );
			return false;
		}
		
		char buffer[ONLINE_MAX_PACKET_SIZE];

		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%s|nu|%s|o|1", FUNCTION_CHANGE_USERNAME, m_ggi, userName, password, newUserName);
		XP_DEBUG_OUT("GLXPlayerUser::sendChangeUserName before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendChangePassword.
	* Change login password.
	* @param userName User name.
	* @param password User password to login.
	* @param newPassword New password.
	* @param confirmNewPassword Confirm new password.
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendChangePassword( char* userName, char* password, char * newPassword, char* confirmNewPassword )
	{
		if(userName == NULL || password == NULL || newPassword == NULL || confirmNewPassword == NULL || (XP_API_STRCMP(newPassword, confirmNewPassword) != 0))
		{
			m_observer->OnRequestFailure( FUNCTION_CHANGE_PASSWORD, ERROR_INIT );
			return false;
		}

		char buffer[ONLINE_MAX_PACKET_SIZE];

		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%s|np|%s|vp|%s|", FUNCTION_CHANGE_PASSWORD, m_ggi, userName, password, newPassword, confirmNewPassword);
		XP_DEBUG_OUT("GLXPlayerUser::sendChangePassword before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendChangeEmail.
	* Change login password.
	* @param userName User name.
	* @param password User password to login.
	* @param newEmail New email.	
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendChangeEmail( char* userName, char* password, char * newEmail)
	{
		if(userName == NULL || password == NULL || newEmail == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_CHANGE_EMAIL, ERROR_INIT );
			return false;
		}
		
		char buffer[ONLINE_MAX_PACKET_SIZE];

		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%s|ne|%s|", FUNCTION_CHANGE_EMAIL, m_ggi, userName, password, newEmail);
		XP_DEBUG_OUT("GLXPlayerUser::sendChangeEmail before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendRetrievalPassword.
	* If user lost password, user can use this function to get his password. This function tell the web server to send userName's  password by his email.
	* @param userName User name.	
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendRetrievalPassword( char* userName)
	{
		if(userName == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_RETRIEVAL_PASSWORD, ERROR_INIT );
			return false;
		}
		
		char buffer[ONLINE_MAX_PACKET_SIZE];

		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_RETRIEVAL_PASSWORD, m_ggi, userName);
		XP_DEBUG_OUT("GLXPlayerUser::sendRetrievalPassword before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendGetUserGameList.
	* Get the list of user's played games.
	* @param userName User name, optional parameter, if don't set, decided by uid.		
	* @param day Indicate how many days played, if don't restrict it, set it's  0.
	* @param limit How many results will be returned. If don't restrict it, set it's 0.
	* @see processUserGameList()
	* @return Success: |r|s|games list or |r|s|n|name|games list / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendGetUserGameList( char* userName, int day, int limit)
	{
		if(day < 0 || limit < 0)
		{
			m_observer->OnRequestFailure( FUNCTION_GET_USER_GAMELIST, ERROR_INIT );
			return false;
		}
		
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );

		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|d|%d|l|%d", FUNCTION_GET_USER_GAMELIST, m_ggi, m_uid, day, limit);
		
		if(userName != NULL)
		{	
			char subMsg[128] = {0};
		#ifdef USE_USER_NUMBER
			XP_API_SPRINTF(subMsg, "|un|%s|", userName); 
		#else
			XP_API_SPRINTF(subMsg, "|n|%s|", userName); 
		#endif // USE_USER_NUMBER
			XP_API_STRCAT(buffer, subMsg);
		}
		
		XP_DEBUG_OUT("GLXPlayerUser::sendGetUserGameList before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendGetUserState.
	* Get the user's state now.
	* @param userName User name, optional parameter, if don't set, decided by uid.			
	* @see processUserState()		
	* @return Success: |r|s|t|state|a|game info or |r|s|n|name|t|state|a|game info / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendGetUserState(char* userName)
	{	
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_GET_USER_STATE, m_ggi, m_uid);
		
		if(userName != NULL)
		{
			char subMsg[128] = {0};
		#ifdef USE_USER_NUMBER
			XP_API_SPRINTF(subMsg, "|un|%s|", userName);
		#else
			XP_API_SPRINTF(subMsg, "|n|%s|", userName);
		#endif
			XP_API_STRCAT(buffer, subMsg);
		}	
		XP_DEBUG_OUT("GLXPlayerUser::sendGetUserState before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendSetUserStoreData.
	* User can store data in the server. This function store data in the server which key is storeName.
	* @param storeName Tag of the stored data.
	* @param data Data to store in the server, encoded as an blob string.
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool GLXPlayerUser::sendSetUserStoreData(char* storeName, char* data)
	{	
		if(storeName == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_SET_USER_STORE_DATA, ERROR_INIT );
			return false;
		}
		
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|k|%s|d|%s|", FUNCTION_SET_USER_STORE_DATA, m_ggi, m_uid, storeName, data);
		
		XP_DEBUG_OUT("GLXPlayerUser::sendSetUserStoreData before String2Blob -> buffer length= %d\n", XP_API_STRLEN(buffer));	
		// Alway use POST method to send data to server
		return SendByPost( buffer );	
	}

	/**
	* Function sendGetUserStoreData.
	* User can store data in the server. This function retun the data stored which key is storeName. Data returned as an blob value.
	* @param storeName Tag of the stored data.
	* @param userName Name of user that we want to get data, optional parameter.
	* @see processUserData()
	* @return Success: |r|s |DATA or |r|s|n|name|DATA/ Fail:  |r|e|error code. 
	*/
	bool GLXPlayerUser::sendGetUserStoreData(char* storeName, char* userName)
	{
		if(storeName == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_GET_USER_STATE, ERROR_INIT );
			return false;
		}
		
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|k|%s", FUNCTION_GET_USER_STORE_DATA, m_ggi, m_uid, storeName);
		
		if(userName != NULL)
		{
			char subMsg[128] = {0};
		#ifdef USE_USER_NUMBER
			XP_API_SPRINTF(subMsg, "|un|%s|", userName);
		#else
			XP_API_SPRINTF(subMsg, "|n|%s|", userName);
		#endif //USE_USER_NUMBER
			XP_API_STRCAT(buffer, subMsg);
		}
		XP_DEBUG_OUT("GLXPlayerUser::sendGetUserStoreData before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendGetReputation.
	* Get user's reputation.
	* @param userName Name of user. If userName is set to NULL, return current user's.
	* @see processUserReputation()
	* @return Success: |r|s|reputation or |r|s|n|name|reputation / Fail:  |r|e|error code. 
	*/
	bool GLXPlayerUser::sendGetReputation(char* userName)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_GET_REPUTATION, m_ggi, m_uid);
		
		if(userName != NULL)
		{
			char subMsg[128] = {0};
		#ifdef USE_USER_NUMBER
			XP_API_SPRINTF(subMsg, "|un|%s|", userName);
		#else
			XP_API_SPRINTF(subMsg, "|n|%s|", userName);
		#endif // USE_USER_NUMBER
			XP_API_STRCAT(buffer, subMsg);
		}
		XP_DEBUG_OUT("GLXPlayerUser::sendGetReputation before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendPointUserReputation.
	* Give other user reputation point.
	* @param userName Name of user.
	* @param reputation Allow only 1 or 5.
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	#ifdef USE_USER_NUMBER
	bool GLXPlayerUser::sendPointUserReputation(char* userName, int reputation, bool isUserMunber)
	{
		if(userName == NULL || (reputation != 1 && reputation != 5))
		{
			m_observer->OnRequestFailure( FUNCTION_POINT_USER_REPUTATION, ERROR_INIT );
			return false;
		}	
		
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		
		if(isUserMunber)
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|un|%s|r|%d|", FUNCTION_POINT_USER_REPUTATION, m_ggi, m_uid, userName, reputation);
		}
		else
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|n|%s|r|%d|", FUNCTION_POINT_USER_REPUTATION, m_ggi, m_uid, userName, reputation);
		}
		
		XP_DEBUG_OUT("GLXPlayerUser::sendUpdateReputation before String2Blob -> buffer = %s\n", buffer);
		return SendByGet( buffer );			
	}
	#else
	bool GLXPlayerUser::sendPointUserReputation(char* userName, int reputation)
	{
		if(userName == NULL || (reputation != 1 && reputation != 5))
		{
			m_observer->OnRequestFailure( FUNCTION_POINT_USER_REPUTATION, ERROR_INIT );
			return false;
		}	
		
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|n|%s|r|%d|", FUNCTION_POINT_USER_REPUTATION, m_ggi, m_uid, userName, reputation);

		XP_DEBUG_OUT("GLXPlayerUser::sendUpdateReputation before String2Blob -> buffer = %s\n", buffer);
		return SendByGet( buffer );			
	}
	#endif //USE_USER_NUMBER

	/**
	* Function sendGetUserUID.
	* Get the UID of other player by his user name.
	* @param userName Other player's user name.	
	* @return Success: |r|s|uid / Fail:  |r|e|error code
	*/
	bool GLXPlayerUser::sendGetUserUID(char* userName)
	{	
		if(userName == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_GET_USER_UID, ERROR_INIT );
			return false;
		}	
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	#ifdef	USE_USER_NUMBER
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|un|%s|", FUNCTION_GET_USER_UID, m_ggi, m_uid, userName);
	#else
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|n|%s|", FUNCTION_GET_USER_UID, m_ggi, m_uid, userName);
	#endif //USE_USER_NUMBER
		XP_DEBUG_OUT("GLXPlayerUser::sendGetUserUID before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendGetUserBestScore.
	* Get user's best score in all games.
	* @param userName Name of user. If userName is set to NULL, return current user's.
	* @see processUserBestScore()
	* @return Success: |r|s||user's best score or |r|s|n|name|user's best score / Fail:  |r|e|error code. 
	*/
	bool GLXPlayerUser::sendGetUserBestScore(char* userName)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_GET_USER_BEST_SCORE, m_ggi, m_uid);
		
		if(userName != NULL)
		{
			char subMsg[128] = {0};
		#ifdef	USE_USER_NUMBER
			XP_API_SPRINTF(subMsg, "|un|%s|", userName);
		#else
			XP_API_SPRINTF(subMsg, "|n|%s|", userName);
		#endif //USE_USER_NUMBER
			XP_API_STRCAT(buffer, subMsg);
		}
		XP_DEBUG_OUT("GLXPlayerUser::sendGetUserBestScore before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendGetUserPhoto.
	* Download user avatar, this avatar can use in chat component. Can download avatar of another user.
	* @param userName User name, optional parameter, if don't set, decided by uid.			
	* @see processUserAvatar()
	* @return Success: |r|s|DATA or |r|s|n|name|DATA / Fail:  |r|e|error code. Data returned as an blob string.
	*/
	bool GLXPlayerUser::sendGetUserPhoto(char* userName)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_GET_USER_AVATAR, m_ggi, m_uid);
		if(userName != NULL)
		{
			char subMsg[128] = {0};
		#ifdef	USE_USER_NUMBER
			XP_API_SPRINTF(subMsg, "|un|%s|", userName);
		#else
			XP_API_SPRINTF(subMsg, "|n|%s|", userName);
		#endif //USE_USER_NUMBER
			XP_API_STRCAT(buffer, subMsg);
		}
		XP_DEBUG_OUT("GLXPlayerUser::sendGetUserPhoto before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );	
	}

	/**
	* Function sendUploadUserPhoto.
	* Upload user avatar, this avatar can use in chat component.
	* @param data Photo's data, encoded as an blob string, limited by 64kb.
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool GLXPlayerUser::sendUploadUserPhoto(char* data)
	{		
		if(data == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_UPLOAD_USER_AVATAR, ERROR_INIT );
			return false;
		}

		//if system avatar, remove the update time
		char pTmp[64];
		char *pData = data;
		if(data[0] == 's')
		{
			XP_API_STRCPY(pTmp, data);
			int i = 0;
			while(pTmp[i] != '-' && pTmp[i] != '\0')
			{
				i++;
			}
			pTmp[i] = '\0';
			pData = pTmp;
		}
		
		int buffLen = XP_API_STRLEN(data) + 128;
		char* buffer = XP_API_NEW char[buffLen];
		XP_API_MEMSET( buffer, 0, buffLen );	
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|d|%s|", FUNCTION_UPLOAD_USER_AVATAR, m_ggi, m_uid, pData);
		XP_DEBUG_OUT("GLXPlayerUser::sendSetUserStoreData before String2Blob -> buffer length = %d\n", XP_API_STRLEN(buffer));	
		// Alway use POST method to send data to server
		bool bResult = SendByPost( buffer );
		XP_API_DEL( buffer );
		return bResult;
	}

	/**
	* Function sendDelUserStoreData.
	* User can store data in the server. This function delete data in the server which key is storeName. The key storeName will be deleted also.
	* @param storeName Tag of the stored data.	
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool GLXPlayerUser::sendDelUserStoreData(char* storeName)
	{
		if(storeName == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_DEL_USER_STORE_DATA, ERROR_INIT );
			return false;
		}
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );

		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|k|%s|", FUNCTION_DEL_USER_STORE_DATA, m_ggi, m_uid, storeName);
		
		XP_DEBUG_OUT("GLXPlayerUser::sendDelUserStoreData before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}

	/**
	* Function sendGetUserTrophy.
	* Get  user's trophy.
	* @param name Name of user.	
	* @see processUserTrophy()
	* @return Success: |r|s|trophy or |r|s|n|name|trophy / Fail:  |r|e|error code. 
	*/
	bool GLXPlayerUser::sendGetUserTrophy(char* name)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_GET_USER_TROPHY, m_ggi, m_uid);
		
		if(name != NULL)
		{
			char subMsg[128] = {0};
		#ifdef USE_USER_NUMBER
			XP_API_SPRINTF(subMsg, "|un|%s|", name);
		#else
			XP_API_SPRINTF(subMsg, "|n|%s|", name);
		#endif //USE_USER_NUMBER
			XP_API_STRCAT(buffer, subMsg);
		}
		
		XP_DEBUG_OUT("GLXPlayerUser::sendGetUserTrophy before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}

	/**
	* Function sendSetUserTrophy.
	* Set user's trophy if the new value is bigger than old value.
	* @param data Trophy value.
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool GLXPlayerUser::sendSetUserTrophy(int data)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|d|%d|", FUNCTION_SET_USER_TROPHY, m_ggi, m_uid, data);	
		XP_DEBUG_OUT("GLXPlayerUser::sendSetUserTrophy before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );
	}

	/**
	* Function sendGetUserInfo.
	* Get some user's information.
	* @param name User name. Optional parameters, if don't set, decided by uid.
	* @see processUserInformation()
	* @return Success: |r|s|userInfo1|value|userInfo2|value|...| userInfo N|value or  |r|s|n|name|userInfo1|value|userInfo2|value|...| userInfo N|value   / Fail:  |r|e|error code. 
	*/
	bool GLXPlayerUser::sendGetUserInfo(char* name)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_GET_USER_INFO, m_ggi, m_uid);
		
		if(name != NULL)
		{	
			char subMsg[128] = {0};
		#ifdef USE_USER_NUMBER
			XP_API_SPRINTF(subMsg, "|un|%s|", name);
		#else
			XP_API_SPRINTF(subMsg, "|n|%s|", name);
		#endif //USE_USER_NUMBER
			XP_API_STRCAT(buffer, subMsg);	
		}
		XP_DEBUG_OUT("GLXPlayerUser::sendGetUserInfo before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );
	}

	/**
	* Function sendUpdateUserInfo.
	* Set user's trophy if the new value is bigger than old value.
	* @param email User email. Optional parameter.
	* @param email_flag User want to get mail? Optional parameter, allowed 0 OR 1 only.
	* @param country User country.
	* @param firstName First name..
	* @param lastName Last name.
	* @param sex User sex.
	* @param birthday User birthday.
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool GLXPlayerUser::sendUpdateUserInfo(char* email, int email_flag, char* country, char* firstName, char* lastName, int sex, char* birthday)
	{
		if(email == NULL && email_flag == -1 && country == NULL && 
			firstName == NULL && lastName == NULL && sex == -1 && birthday == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_UPDATE_USER_INFO, ERROR_INIT );
			return false;
		}
		
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_UPDATE_USER_INFO, m_ggi, m_uid);	
		
		char subMsg[128] = {0};	
		
		if(email != NULL)
		{
			XP_API_MEMSET(subMsg, 0, 128);
			XP_API_SPRINTF(subMsg, "|email|%s", email);
			XP_API_STRCAT(buffer, subMsg);	
		}
		if(email_flag != -1)
		{
			XP_API_MEMSET(subMsg, 0, 128);
			XP_API_SPRINTF(subMsg,"|email_flag|%d", email_flag);
			XP_API_STRCAT(buffer, subMsg);		
		}	
		if(country != NULL)
		{
			XP_API_MEMSET(subMsg, 0, 128);
			XP_API_SPRINTF(subMsg, "|country|%s", country);
			XP_API_STRCAT(buffer, subMsg);
		}
		if(firstName != NULL)
		{
			XP_API_MEMSET(subMsg, 0, 128);
			XP_API_SPRINTF(subMsg, "|firstname|%s", firstName);
			XP_API_STRCAT(buffer, subMsg);		
		}
		if(lastName != NULL)
		{
			XP_API_MEMSET(subMsg, 0, 128);
			XP_API_SPRINTF(subMsg, "|lastname|%s", lastName);
			XP_API_STRCAT(buffer, subMsg);			
		}
		if(sex != -1)
		{
			XP_API_MEMSET(subMsg, 0, 128);
			XP_API_SPRINTF(subMsg, "|sex|%d", sex);
			XP_API_STRCAT(buffer, subMsg);			
		}
		if(birthday != NULL)
		{
			XP_API_MEMSET(subMsg, 0, 128);
			XP_API_SPRINTF(subMsg, "|birthday|%s|", birthday);
			XP_API_STRCAT(buffer, subMsg);			
		}
		
		XP_DEBUG_OUT("GLXPlayerUser::sendUpdateUserInfo before String2Blob -> buffer = %s\n", buffer);	

		return SendByGet( buffer );
	}

	/**
	* Function sendGetTrophyIcon.
	* Get a trophy's icon data. The icon data is the binary data which encoded by base64.
	* 
	*/
	bool GLXPlayerUser::sendGetTrophyIcon(int trophyID)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|t|%d|", FUNCTION_GET_TROPHY_ICON, m_ggi, m_uid, trophyID);
		
		XP_DEBUG_OUT("GLXPlayerUser::sendGetTrophyIcon before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}

	/**
	* Function sendAwardTrophy.
	* Award a trophy to a user.
	* 
	*/
	bool GLXPlayerUser::sendAwardTrophy(int* trophyID, int numberOfID)
	{
		if(numberOfID <=0 || trophyID == NULL)
		{
			m_observer->OnRequestFailure( FUNCTION_AWARD_TROPHY, ERROR_INIT );
			return false;
		}	
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		
		char tmp[256] = {0};	
		char t[16] = {0};

		if(numberOfID == 1)
		{
			XP_API_MEMSET(t, 0, 16);		
			XP_API_SPRINTF(t, "%d", trophyID[0]);
			XP_API_STRCAT(tmp, t);
		}
		else
		{
			for(int i = 0; i < numberOfID - 1; i++)
			{		
				XP_API_MEMSET(t, 0, 16);		
				XP_API_SPRINTF(t, "%d,", trophyID[i]);
				XP_API_STRCAT(tmp, t);
				
				if(i == numberOfID - 2)
				{
					XP_API_MEMSET(t, 0, 16);		
					XP_API_SPRINTF(t, "%d", trophyID[i + 1]);
					XP_API_STRCAT(tmp, t);
				}
			}
		}
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|t|%s|", FUNCTION_AWARD_TROPHY, m_ggi, m_uid, tmp);
		
		XP_DEBUG_OUT("GLXPlayerUser::sendAwardTrophy before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}

	/**
	* Function sendGetGameTrophyList.
	* Get all trophy list of a game
	* 
	*/
	bool GLXPlayerUser::sendGetGameTrophyList(int ggi)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		
		if(ggi == -1)
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_GAME_TROPHYLIST, m_ggi, m_uid);
		}
		else
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%d|", FUNCTION_GET_GAME_TROPHYLIST, m_ggi, m_uid, ggi);
		}
		
		XP_DEBUG_OUT("GLXPlayerUser::sendGetGameTrophyList before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}

	/**
	* Function sendGetUserGameTrophyList.
	* Get user's all trophy list of a game. Look's like FUNCTION_GET_GAME_TROPHYLIST
	* 
	*/
	bool GLXPlayerUser::sendGetUserGameTrophyList(int ggi, char* name)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		char subMsg[128] = {0};
		
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_GET_USER_GAME_TROPHYLIST, m_ggi, m_uid);
		
		if(ggi != -1)
		{
			XP_API_SPRINTF(subMsg, "|p|%d", ggi);
			XP_API_STRCAT(buffer, subMsg);
		}
		
		if(name != NULL)
		{			
		#ifdef USE_USER_NUMBER
			XP_API_SPRINTF(subMsg, "|un|%s|", name);
		#else
			XP_API_SPRINTF(subMsg, "|n|%s|", name);
		#endif //USE_USER_NUMBER
			XP_API_STRCAT(buffer, subMsg);	
		}
		XP_DEBUG_OUT("GLXPlayerUser::sendGetUserGameTrophyList before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}

	/**
	* Function sendGetAdvRSS.
	* Get advertisements (RSS)
	* 
	*/
	bool GLXPlayerUser::sendGetAdvRSS(int ggi)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
		if(ggi == -1)
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_ADV_RSS, m_ggi, m_uid);
		}
		else
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%d|", FUNCTION_GET_ADV_RSS, m_ggi, m_uid, ggi);
		}
		
		XP_DEBUG_OUT("GLXPlayerUser::sendGetAdvRSS before String2Blob -> buffer = %s\n", buffer);	
		return SendByPost( buffer );
	}

	/**
	* Function sendGetAdvAttachment.
	* Get a advertisement attachment data. The data is the binary data which encoded by base64.
	* 
	*/
	bool GLXPlayerUser::sendGetAdvAttachment(int attachmentID)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|a|%d|", FUNCTION_GET_ADV_ATTACHMENT, m_ggi, m_uid, attachmentID);	
		
		XP_DEBUG_OUT("GLXPlayerUser::sendGetAdvAttachment before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}
	/**
	* Function sendGetAvatarList.
	* Get avatar list.
	* 
	*/
	bool GLXPlayerUser::sendGetAvatarList()
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s", FUNCTION_GET_USER_AVATAR_LIST, m_ggi, m_uid);	
		
		XP_DEBUG_OUT("GLXPlayerUser::sendGetAvatarList before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );	
	}


	/**
	* Function sendGetGeneralCondition.
	* Get general condition.
	* 
	*/
	bool GLXPlayerUser::sendGetGeneralCondition(const char *pLang)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|l|%s|", FUNCTION_GET_GENERAL_CONDITIONS, m_ggi, "12345", pLang);	
		
		XP_DEBUG_OUT("GLXPlayerUser::sendGetGeneralCondition before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}

	bool GLXPlayerUser::sendDelCustomAvatar(const char *avatarId)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|d|%s", FUNCTION_DEL_USER_AVATAR, m_ggi, m_uid, avatarId);	
		
		XP_DEBUG_OUT("GLXPlayerUser::sendDelCustomAvatar before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}

	bool GLXPlayerUser::sendDownloadAvatar(const char *avatarId)
	{
		//if system avatar, remove the update time
		char pTmp[64];
		XP_API_STRCPY(pTmp, avatarId);
		if(pTmp[0] == 's')
		{
			int i = 0;
			while(pTmp[i] != '-' && pTmp[i] != '\0')
			{
				i++;
			}
			pTmp[i] = '\0';
		}

		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|d|%s", FUNCTION_DOWNLOAD_USER_AVATAR, m_ggi, m_uid, pTmp);	
		
		XP_DEBUG_OUT("GLXPlayerUser::sendDownloadAvatar before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}

	int GLXPlayerUser::getUserNumber()
	{
		return XP_API_ATOI(m_userNumber);
	}

	void GLXPlayerUser::setUserNumber(const char *un)
	{
		XP_API_DEL(m_userNumber);
		if(un == NULL)
		{
			return;
		}
		m_userNumber = XP_API_STRNEW(un);
	}

	/**
	* Function sendGetAttention.
	* Get attention.
	* 
	*/
	bool GLXPlayerUser::sendGetAttention()
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );		
		XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_CHAT_WARNING, m_ggi, m_uid);	
		
		XP_DEBUG_OUT("GLXPlayerUser::sendGetAttention before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}

	/**
	* Function sendGetGameIcon.
	* Get a advertisement attachment data. The data is the binary data which encoded by base64.
	* 
	*/
	bool GLXPlayerUser::sendGetGameIcon(int ggi)
	{
		char buffer[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );		

		if(ggi == -1)
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_GAME_ICON, m_ggi, m_uid);
		}
		else
		{
			XP_API_SPRINTF(buffer, "f|%d|i|%d|u|%s|p|%d|", FUNCTION_GET_GAME_ICON, m_ggi, m_uid, ggi);
		}
		
		XP_DEBUG_OUT("GLXPlayerUser::sendGetAdvAttachment before String2Blob -> buffer = %s\n", buffer);	
		return SendByGet( buffer );
	}


	/**
	* Private function OnUpdateSuccess.
	* Handle success message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	bool GLXPlayerUser::OnUpdateSuccess( int func_id )
	{
		switch(func_id)
		{		
			case FUNCTION_GET_USER_GAMELIST:
				XP_API_DEL(m_otherUserName);
				processUserGameList(m_pResponsePtr);
				break;
			case FUNCTION_GET_USER_STATE:
				XP_API_DEL(m_otherUserName);
				processUserState(m_pResponsePtr);
				break;
			case FUNCTION_GET_REPUTATION:	
				XP_API_DEL(m_otherUserName);
				processUserReputation(m_pResponsePtr);			
				break;
			case FUNCTION_GET_USER_TROPHY:
				XP_API_DEL(m_otherUserName);
				processUserTrophy(m_pResponsePtr);			
				break;
			case FUNCTION_GET_USER_INFO:	
				XP_API_DEL(m_otherUserName);
				processUserInformation(m_pResponsePtr);			
				break;
			case FUNCTION_GET_USER_BEST_SCORE:				
				XP_API_DEL(m_otherUserName);
				processUserBestScore(m_pResponsePtr);			
				break;
			case FUNCTION_GET_USER_AVATAR:				
				XP_API_DEL(m_otherUserName);
				processUserAvatar(m_pResponsePtr);			
				break;
			case FUNCTION_GET_USER_STORE_DATA:				
				XP_API_DEL(m_otherUserName);
				processUserData(m_pResponsePtr);			
				break;
			case FUNCTION_GET_COUNTRY:
				m_country = XP_API_NEW char[128];
				XP_API_MEMSET(m_country, 0, 128);
				getValue(m_pResponsePtr, m_country, 1, '|');
				break;
			case FUNCTION_GET_GAME_ICON:
				processUserGameIcon(m_pResponsePtr);
				break;
			case FUNCTION_GET_ADV_RSS:
				//processGetADV_RSS(m_pResponsePtr);
				break;
			case FUNCTION_GET_CHAT_WARNING:
				processUserAttention(m_pResponsePtr);
				break;
			case FUNCTION_GET_USER_AVATAR_LIST:
				processUserAvatarList(m_pResponsePtr);
				break;
			default:			
				break;
		}
		return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
	}

	/**
	* Private function OnUpdateFailure.
	* Handle failure message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	bool GLXPlayerUser::OnUpdateFailure( int func_id )
	{
		char token[ONLINE_MAX_PACKET_SIZE];

		int err_code = GetNextResponseIntToken();

		if(func_id == FUNCTION_REGISTER)
		{
			if(err_code == ERROR_USERNAME_TAKEN)
			{
				if(IsNextResponseStringToken("u"))
				{
					// get the recommended username from server response
					XP_API_DELETE_ARRAY(m_recommendedUsername);
					m_recommendedUsername = XP_API_NEW char[17];
					XP_API_MEMSET(m_recommendedUsername, 0, 17);
					XP_API_STRCPY(m_recommendedUsername, GetNextResponseToken(token));
				}
			}
		}

		m_observer->OnRequestFailure( func_id, err_code );
		return true;
	}

	/**
	* Private function processUserGameList.
	* Process list of user played game if success message returned from the server.
	* @see sendGetUserGameList()	
	* @param data Data returned from the server.
	*/
	void GLXPlayerUser::processUserGameList(char* data)
	{
		clearUserGameList();
		
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}

		char tmpString[256] = {0};
		char tmp[8] = {0};
		int index = 0;
		
		getValue(data, tmpString, index++, '|');
		if( XP_API_STRCMP( tmpString, "n" ) == 0)
		{
			m_otherUserName = XP_API_NEW char[16];
			getValue(data, m_otherUserName, index++, '|');
		}

		m_numberUserGameList = 1;
		
		for(int i=0; i < XP_API_STRLEN(data) ;i++)
		{
			if(data[i] == '|')
			{			
				m_numberUserGameList ++;
			}
		}
		
		m_numberUserGameList -= (index - 1);
		if(index < 2)//if current user, reset index
		{
			index = 0;
		}
		m_userGameListPID = XP_API_NEW char* [m_numberUserGameList];
		m_userGameListName = XP_API_NEW char* [m_numberUserGameList];
		m_userGameListTrophy = XP_API_NEW int [m_numberUserGameList];
		
		
		for(int i = 0; i < m_numberUserGameList; i++)
		{		
			XP_API_MEMSET(tmpString, 0, 256);
			getValue(data, tmpString, i + index, '|');
			
			m_userGameListPID[i] = XP_API_NEW char[128];
			m_userGameListName[i] = XP_API_NEW char[128];				
			
			XP_API_MEMSET(m_userGameListPID[i], 0, 128);
			getValue(tmpString, m_userGameListPID[i], 0, '^');
			
			XP_API_MEMSET(m_userGameListName[i], 0, 128);
			getValue(tmpString, m_userGameListName[i], 1, '^');
			
			XP_API_MEMSET(tmp, 0, 8);
			getValue(tmpString,tmp, 2, '^');	
			 m_userGameListTrophy[i] = XP_API_ATOI(tmp);
		}	
	}

	/**
	* Private function processUserState.
	* Process user state and related information if success message returned from the server.
	* @see sendGetUserState()	
	* @param data Data returned from the server.
	*/
	void GLXPlayerUser::processUserState(char* data)
	{
		clearUserState();
		
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}
		char tmpHR[256] = {0};
		char tmp[8] = {0};
		int index = 0;
		
		getValue(data, tmpHR, index++, '|');
		if( XP_API_STRCMP( tmpHR, "n" ) == 0)
		{
			m_otherUserName = XP_API_NEW char[16];
			getValue(data, m_otherUserName, index++, '|');
		}
		
		if(index < 2)//if current user, reset index
		{
			index = 0;
		}
		
		getValue(data, tmpHR, index++, '|');
		
		if (XP_API_STRCMP(tmpHR, "t") == 0)
		{
			getValue(data, tmpHR, index++, '|');
			m_userState =  (XP_API_ATOI(tmpHR) == 1 ? false : true);		
			getValue(data, tmpHR, index++, '|');
			
			if(m_userState == false) return; //if user is offline,  don't get game information
			
			if (XP_API_STRCMP(tmpHR, "a") == 0)
			{
				getValue(data, tmpHR, index++, '|');		
				
				m_currentGamePID = XP_API_NEW char[128];
				m_currentGameName = XP_API_NEW char[128];
				m_currentGameTrophy = -1;					
				
				getValue(tmpHR, m_currentGamePID, 0, '^');			
				getValue(tmpHR, m_currentGameName, 1, '^');			
				getValue(tmpHR, tmp, 2, '^');			
				
				m_currentGameTrophy = XP_API_ATOI(tmp);
			}		
		}	
	}

	/**
	* Private function processUserReputation.
	* Process user reputation if success message returned from the server.
	* @see sendGetReputation()	
	* @param data Data returned from the server.
	*/
	void GLXPlayerUser::processUserReputation(char* data)
	{
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}
		
		char tmpHR[512] = {0};	
		int index = 0;
		
		getValue(data, tmpHR, index++, '|');
		if( XP_API_STRCMP( tmpHR, "n" ) == 0)
		{
			m_otherUserName = XP_API_NEW char[16];
			getValue(data, m_otherUserName, index++, '|');
		}	
		if(index < 2)//if current user, reset index
		{
			index = 0;
		}
		//Reputation format: reputation value,bad evaluation count,good evaluation count.
		XP_API_MEMSET(tmpHR, 0, 512);
		getValue(data, tmpHR, index++, '|');
		
		char tmpReputationStr[128] = {0};
		
		XP_API_MEMSET(tmpReputationStr, 0, 128);	
		getValue(tmpHR, tmpReputationStr, 0, ',');
		m_reputation = XP_API_ATOF(tmpReputationStr);

		XP_API_MEMSET(tmpReputationStr, 0, 128);	
		getValue(tmpHR, tmpReputationStr, 1, ',');
		m_badEvaluationCount = XP_API_ATOI(tmpReputationStr);

		XP_API_MEMSET(tmpReputationStr, 0, 128);	
		getValue(tmpHR, tmpReputationStr, 2, ',');
		m_goodEvaluationCount = XP_API_ATOI(tmpReputationStr);
	}

	/**
	* Private function processUserTrophy.
	* Process user trophy if success message returned from the server.
	* @see sendGetUserTrophy()	
	* @param data Data returned from the server.
	*/
	void GLXPlayerUser::processUserTrophy(char* data)
	{
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}
		
		char tmpHR[256] = {0};	
		int index = 0;
		
		getValue(data, tmpHR, index++, '|');
		if( XP_API_STRCMP( tmpHR, "n" ) == 0)
		{
			m_otherUserName = XP_API_NEW char[16];
			getValue(data, m_otherUserName, index++, '|');
		}	
		if(index < 2)//if current user, reset index
		{
			index = 0;
		}		
		getValue(data, tmpHR, index++, '|');
		m_trophy = XP_API_ATOI(tmpHR);
	}

	/**
	* Private function processUserInformation.
	* Process user information if success message returned from the server.
	* @see sendGetUserInfo()	
	* @param data Data returned from the server.
	*/
	void GLXPlayerUser::processUserInformation(char* data)
	{
		//f|81|r|s|n|xxxx|username|Tom|email|tom@gameloft.com|email_flag|0|country|CN|reputation|1.3|firstname|Tom|lastname|Jackson|sex|1|birthday|1970-1-1
		clearUserInformation();
		
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}
		
		char tmpHR[256] = {0};	
		int index = 0;
		
		getValue(data, tmpHR, index++, '|');
		if( XP_API_STRCMP( tmpHR, "n" ) == 0)
		{
			m_otherUserName = XP_API_NEW char[16];
			getValue(data, m_otherUserName, index++, '|');
		}	
		if(index < 2)//if current user, reset index
		{
			index = 0;
		}
		
		index ++;
		char tmp[32] = {0};	
		
		m_userNumber = XP_API_NEW char[16];
		getValue(data, m_userNumber, index, '|');
		index += 2;	
		
		m_userName = XP_API_NEW char[16];
		getValue(data, m_userName, index, '|');
		index += 2;	
		
		m_email = XP_API_NEW char[128];
		getValue(data, m_email, index, '|');
		index += 2;
		
		XP_API_MEMSET(tmpHR, 0, 256);
		getValue(data, tmpHR, index, '|');
		m_emailFlag = XP_API_ATOI(tmpHR) !=0 ? 1 : 0;
		index += 2;
		
		m_country = XP_API_NEW char[128];
		getValue(data, m_country, index, '|');
		index += 2;

		m_avatarId = XP_API_NEW char[128];
		getValue(data, m_avatarId, index, '|');
		index += 2;

		m_avatarUpdateTime = XP_API_NEW char[128];
		getValue(data, m_avatarUpdateTime, index, '|');
		index += 2;
		
		XP_API_MEMSET(tmp, 0, 32);
		getValue(data, tmp, index, '|');	
		m_reputation = XP_API_ATOI(tmp);
		index += 2;
		
		m_firstName = XP_API_NEW char[128];
		getValue(data, m_firstName, index, '|');
		index += 2;
			
		m_lastName = XP_API_NEW char[128];
		getValue(data, m_lastName, index, '|');	
		index += 2;
		
		XP_API_MEMSET(tmpHR, 0, 256);
		getValue(data, tmpHR, index, '|');
		m_sex = XP_API_ATOI(tmpHR) !=0 ? 1 : 0;	
		index += 2;
			
		m_birthday = XP_API_NEW char[128];
		getValue(data, m_birthday, index, '|');		
		index += 2;	
		
		XP_API_MEMSET(tmp, 0, 32);
		getValue(data, tmp, index, '|');	
		m_onlineTime = XP_API_ATOI(tmp);
		index += 2;
		
		XP_API_MEMSET(tmp, 0, 32);
		getValue(data, tmp, index, '|');	
		m_totalTrophy = XP_API_ATOI(tmp);
		index += 2;

		//if system avatar, append update time as id
		if(m_avatarId[0] == 's')
		{
			XP_API_STRCAT(m_avatarId, "-");
			XP_API_STRCAT(m_avatarId, m_avatarUpdateTime);
			for(int a = 0; m_avatarId[a] != 0; a++)
			{
				if(m_avatarId[a] == ':')
				{
					m_avatarId[a] = '@';
				}
			}
		}
	}

	/**
	* Private function processUserBestScore.
	* Process user best score if success message returned from the server.
	* @see sendGetUserBestScore()	
	* @param data Data returned from the server.
	*/
	void GLXPlayerUser::processUserBestScore(char* data)
	{
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}
		
		char tmpHR[256] = {0};	
		int index = 0;
		
		getValue(data, tmpHR, index++, '|');
		if( XP_API_STRCMP( tmpHR, "n" ) == 0)
		{
			m_otherUserName = XP_API_NEW char[16];
			getValue(data, m_otherUserName, index++, '|');
		}	
		if(index < 2)//if current user, reset index
		{
			index = 0;
		}		
		getValue(data, tmpHR, index++, '|');
		m_userBestScore = XP_API_ATOI(tmpHR);
	}

	/**
	* Private function processUserAvatar.
	* Process user avatar if success message returned from the server.
	* @see sendGetUserPhoto()	
	* @param data Data returned from the server.
	*/
	void GLXPlayerUser::processUserAvatar(char* data)
	{
		XP_API_DEL(m_userAvatar);
		XP_API_DEL(m_userAvatarUpdateTime);	
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}
		
		int length = XP_API_STRLEN(data) + 1;
		char* tmpHR = XP_API_NEW char[length];	
		XP_API_MEMSET(tmpHR, 0, length);	
		int index = 0;
		
		getValue(data, tmpHR, index++, '|');
		if( XP_API_STRCMP( tmpHR, "n" ) == 0)
		{
			m_otherUserName = XP_API_NEW char[16];
			getValue(data, m_otherUserName, index, '|');
			index += 2;

			m_userAvatar = XP_API_NEW char[64];
			XP_API_MEMSET(m_userAvatar, 0, 64);
			getValue(data, m_userAvatar, index, '|');
			index += 2;

			m_userAvatarUpdateTime = XP_API_NEW char[64];
			XP_API_MEMSET(m_userAvatarUpdateTime, 0, 64);
			getValue(data, m_userAvatarUpdateTime, index, '|');

			//if system avatar, append update time as id
			if(m_userAvatar[0] == 's')
			{
				XP_API_STRCAT(m_userAvatar, "-");
				XP_API_STRCAT(m_userAvatar, m_userAvatarUpdateTime);
				for(int a = 0; m_userAvatar[a] != 0; a++)
				{
					if(m_userAvatar[a] == ':')
					{
						m_userAvatar[a] = '@';
					}
				}
			}
		}
		else
		{
			XP_API_DEL(m_avatarId);
			m_avatarId = XP_API_NEW char[64];
			XP_API_MEMSET(m_avatarId, 0, 64);
			getValue(data, m_avatarId, index, '|');
			index += 2;

			XP_API_DEL(m_avatarUpdateTime);
			m_avatarUpdateTime = XP_API_NEW char[64];
			XP_API_MEMSET(m_avatarUpdateTime, 0, 64);
			getValue(data, m_avatarUpdateTime, index, '|');

			//if system avatar, append update time as id
			if(m_avatarId[0] == 's')
			{
				XP_API_STRCAT(m_avatarId, "-");
				XP_API_STRCAT(m_avatarId, m_avatarUpdateTime);
				for(int a = 0; m_avatarId[a] != 0; a++)
				{
					if(m_avatarId[a] == ':')
					{
						m_avatarId[a] = '@';
					}
				}
			}
		}
		XP_API_DEL( tmpHR );
	}

	/**
	* Private function processUserAvatar.
	* Process user data if success message returned from the server.
	* @see sendGetUserStoreData()	
	* @param data Data returned from the server.
	*/
	void GLXPlayerUser::processUserData(char* data)
	{
		XP_API_DEL(m_userData);
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}
		
		int length = XP_API_STRLEN(data) + 1;
		char* tmpHR = XP_API_NEW char[length];	
		XP_API_MEMSET(tmpHR, 0, length);	
		int index = 0;
		
		getValue(data, tmpHR, index++, '|');
		if( XP_API_STRCMP( tmpHR, "n" ) == 0)
		{
			m_otherUserName = XP_API_NEW char[16];
			getValue(data, m_otherUserName, index++, '|');
		}	
		if(index < 2)//if current user, reset index
		{
			index = 0;
		}	
		
		m_userData = XP_API_NEW char[length];
		XP_API_MEMSET(m_userData, 0, length);
		getValue(data, m_userData, index++, '|');	

		XP_API_DEL(tmpHR);
	}

	/**
	* Private function processUserGameIcon.
	* Process game icon data and icon type if success message returned from the server.
	* @see sendGetGameIcon(int ggi)	
	* @param data Data returned from the server.
	*/
	void GLXPlayerUser::processUserGameIcon(char* data)
	{
		//d|data|t|fileType

		XP_API_DELETE_ARRAY(m_gameIconData);
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}
		
		int index = 1; //skip 'd'

		XP_API_PARSE_DATA_NEW( data, &m_gameIconData, index, '|' );

		index += 2; //skip 't'

		XP_API_PARSE_DATA( data, m_gameIconType, index, '|' );

		index += 2; //skip 't'
		char tmp[32] = {0};
		XP_API_MEMSET(tmp, 0, 32);
		XP_API_PARSE_DATA( data, tmp, index, '|' );
		m_gameIconUpdateTime = XP_API_ATOI(tmp);
	}

	/**
	* Private function processGetADV_RSS.
	* Process game icon data and icon type if success message returned from the server.
	* @see sendGetGameIcon(int ggi)	
	* @param data Data returned from the server.
	*/
	//void GLXPlayerUser::processGetADV_RSS(char* data)
	//{
	//	XmlDocument readingDoc;
	//	readingDoc.loadFromMemory(data, XP_API_STRLEN(data));	
	//	
	//	XmlNode* readingNode = readingDoc.findChild( T("rss") );
	//	if ( readingNode != NULL )
	//	{
	//		m_rss.read( readingNode );
	//	}
	//}

	void GLXPlayerUser::processUserAttention(char* data)
	{
		XP_API_DEL(m_gameAttention);
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}

		int length = XP_API_STRLEN(data);
		m_gameAttention = XP_API_NEW char[length + 1];
		XP_API_MEMCPY(m_gameAttention, data, length);
		m_gameAttention[length] = 0;
	}

	/**
	* Private function processUserAvatarList.
	* Process user avatar info list
	* @see sendGetAvatarList()	
	* @param data Data returned from the server.
	*/
	void GLXPlayerUser::processUserAvatarList(char *data)
	{
		clearUserAvatarList();
		
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			return;
		}

		char tmpString[256] = {0};
		char tmp[8] = {0};
		int index = 0;

		m_userAvatarListCount = 1;
		
		for(int i=0; i < XP_API_STRLEN(data) ;i++)
		{
			if(data[i] == '|')
			{
				m_userAvatarListCount++;
			}
		}
		
		m_userAvatarIdList = XP_API_NEW char* [m_userAvatarListCount];
		m_userAvatarUpdateTimeList = XP_API_NEW char* [m_userAvatarListCount];
		m_userAvatarStateList = XP_API_NEW int [m_userAvatarListCount];
		
		
		for(int i = 0; i < m_userAvatarListCount; i++)
		{		
			XP_API_MEMSET(tmpString, 0, 256);
			getValue(data, tmpString, i + index, '|');
			
			m_userAvatarIdList[i] = XP_API_NEW char[128];
			m_userAvatarUpdateTimeList[i] = XP_API_NEW char[128];				
			
			XP_API_MEMSET(m_userAvatarIdList[i], 0, 128);
			getValue(tmpString, m_userAvatarIdList[i], 0, '^');
			
			XP_API_MEMSET(m_userAvatarUpdateTimeList[i], 0, 128);
			getValue(tmpString, m_userAvatarUpdateTimeList[i], 1, '^');
			
			XP_API_MEMSET(tmp, 0, 8);
			getValue(tmpString,tmp, 2, '^');	
			 m_userAvatarStateList[i] = XP_API_ATOI(tmp);

 			//if system avatar, append update time as id
			if(m_userAvatarIdList[i][0] == 's')
			{
				XP_API_STRCAT(m_userAvatarIdList[i], "-");
				XP_API_STRCAT(m_userAvatarIdList[i], m_userAvatarUpdateTimeList[i]);
				for(int a = 0; m_userAvatarIdList[i][a] != 0; a++)
				{
					if(m_userAvatarIdList[i][a] == ':')
					{
						m_userAvatarIdList[i][a] = '@';
					}
				}
			}
		}
	}

	/**
	* Private function clearUserGameList.
	* Clear user's played game list.	
	*/
	void GLXPlayerUser::clearUserGameList()
	{
		XP_API_DEL_ARRAY(m_userGameListPID, m_numberUserGameList);
		XP_API_DEL_ARRAY(m_userGameListName, m_numberUserGameList);
		XP_API_DEL(m_userGameListTrophy);	
		
		m_numberUserGameList = 0;	
	}

	void GLXPlayerUser::clearUserAvatarList()
	{
		XP_API_DEL_ARRAY(m_userAvatarIdList, m_userAvatarListCount);
		XP_API_DEL_ARRAY(m_userAvatarUpdateTimeList, m_userAvatarListCount);
		XP_API_DEL(m_userAvatarStateList);

		m_userAvatarListCount = 0;
	}

	/**
	* Private function clearUserState.
	* Clear user's state related variable.	
	*/
	void GLXPlayerUser::clearUserState()
	{
		XP_API_DEL(m_currentGamePID);
		XP_API_DEL(m_currentGameName);
		
		m_currentGameTrophy = -1;	
		m_userState = false;	
	}

	/**
	* Private function clearUserInformation.
	* @see processUserInformation()
	* Clear user's information.	
	*/
	void GLXPlayerUser::clearUserInformation()
	{
		XP_API_DEL(m_userNumber);	
		XP_API_DEL(m_userName);	
		XP_API_DEL(m_email);
		XP_API_DEL(m_country);
		XP_API_DEL(m_firstName);
		XP_API_DEL(m_lastName);
		XP_API_DEL(m_birthday);
		XP_API_DEL(m_avatarId);
		XP_API_DEL(m_avatarUpdateTime);
		m_onlineTime = 0;
		m_totalTrophy = 0;
			
		m_emailFlag = false;
		m_sex = false;	
	}

}
