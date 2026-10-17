#ifndef __GL_XPLAYER_USER_H__
#define __GL_XPLAYER_USER_H__

#include "../Porting.h"
#include "../Common/GLXPlayerWebComponent.h"
#include "../3rdParty/XMLParser/SlimXml.h"
#include "../3rdParty/XMLParser/rss.h"

using namespace slim;
/**
* Class GLXPlayerUser. 
* Class GLXPlayerUser is designed for changing user's property, such as name, nickname, password, email address, nationaliy, etc. 
*
* @author  World Online Team <<online-adm@gameloft.com>>
* @author  Tran Thai Phi <<phi.tranthai@gameloft.com>>
 * @version 1.0.0 
 *  
 */
class GLXPlayerUser : public GLXPlayerWebComponent
{

public:

	/**
	* GLXPlayerUser constructor.
	* Create an instance of GLXPlayerUser.
	*/
	GLXPlayerUser();
	
	/**
	* GLXPlayerUser destructor.
	* Destroy an instance of GLXPlayerUser.
	*/
	virtual ~GLXPlayerUser(void);	
	
	/**
	* Function sendGetUserUID.
	* Get the UID of other player by his user name.
	* @param userName Other player's user name.	
	* @return Success: |r|s|uid / Fail:  |r|e|error code
	*/
	bool sendGetUserUID(char* userName);
	
	/**
	* Function sendRegister.
	* Register an UID in web server.
	* @param name User name.
	* @param password User password to login.
	* @param email Registration email, use to login to chat component and to receive new email from Gameloft.	
	* @param country User country.	
	* @param emailFlag User can consider to receive email from Gameloft or not.	
	* @param isRegisterWithGGLive Optional parameter, register with GLLive or not.
	* @return Success: |r|s / Fail:  |r|e|error code
	* @return Error code: ERROR_NO_USERNAME = 46; ERROR_NO_PASSWORD = 47; ERROR_NOT_REGISTERED = 4; ERROR_USERNAME_TAKEN = 48; ERROR_BAD_USER_NAME = 70 
	*/
	bool sendRegister( char * name, char * password, char * email, char *country, bool emailFlag, bool isRegisterWithGGLive = false, char* deviceToken = NULL);	
		
	/**
	* Function sendChangeUserName.
	* Change login username.
	* @param userName User name.
	* @param password User password to login.
	* @param newUserName New user name.
	* @param isRegisterWithGLLive optional, register with GLLive or not, default is false.
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool sendChangeUserName( char* userName, char* password, char * newUserName, bool isRegisterWithGLLive = false);	
	
	/**
	* Function sendChangePassword.
	* Change login password.
	* @param userName User name.
	* @param password User password to login.
	* @param newPassword New password.
	* @param confirmNewPassword Confirm new password.
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool sendChangePassword( char* userName, char* password, char * newPassword, char* confirmNewPassword );	
	
	/**
	* Function sendChangeEmail.
	* Change login password.
	* @param userName User name.
	* @param password User password to login.
	* @param newEmail New email.	
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool sendChangeEmail( char* userName, char* password, char * newEmail);	
	
	/**
	* Function sendRetrievalPassword.
	* If user lost password, user can use this function to get his password. This function tell the web server to send userName's  password by his email.
	* @param userName User name.	
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool sendRetrievalPassword( char* userName);	
	
	/**
	* Function sendGetCountry.
	* Get user country.
	* @param userName User name.	
	* @param nickName Other user name, optional parameter.	
	* @param nickNameFlag Nick name flag, optional parameter.	
	* @return Success: |r|s|country or |r|s|n|nick name|country / Fail:  |r|e|error code
	*/
	bool sendGetCountry(char * userName, char * nickName = NULL, bool nickNameFlag = false);	
	
	/**
	* Function sendChangeCountry.
	* Changecuser country.
	* @param userName User name.
	* @param password User password to login.
	* @param country New country.	
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool sendChangeCountry( char* userName, char* password, char * country );
	
	/**
	* Function sendGetUsedApp.
	* Get the list of user's played application..
	* @param userName User name, optional parameter, if don't set, decided by uid.		
	* @return Success: |r|s|app list  or |r|s|n|name|app list / Fail:  |r|e|error code
	*/
	bool sendGetUsedApp(char* userName = NULL);	
	
	/**
	* Function sendGetUserGameList.
	* Get the list of user's played games.
	* @param userName User name, optional parameter, if don't set, decided by uid.		
	* @param day Indicate how many days played, if don't restrict it, set it's  0.
	* @param limit How many results will be returned. If don't restrict it, set it's 0.
	* @see processUserGameList()
	* @return Success: |r|s|games list or |r|s|n|name|games list / Fail:  |r|e|error code
	*/
	bool sendGetUserGameList(char* userName = NULL, int day = 0, int limit = 0);	
	
	/**
	* Function sendGetUserState.
	* Get the user's state now.
	* @param userName User name, optional parameter, if don't set, decided by uid.			
	* @see processUserState()		
	* @return Success: |r|s|t|state|a|game info or |r|s|n|name|t|state|a|game info / Fail:  |r|e|error code
	*/
	bool sendGetUserState(char* userName = NULL);
	
	/**
	* Function sendGetUserAvatar.
	* Get user's current avatar key.
	* @param userName User name, optional parameter, if don't set, decided by uid.			
	* @see processUserAvatar()
	* @return Success: |r|s|DATA or |r|s|n|name|DATA / Fail:  |r|e|error code. Data returned as an blob string.
	*/
	bool sendGetUserAvatar(char* userName = NULL);	
	
	/**
	* Function sendDownloadUserAvatar.
	* Download user's avatar data(base64 encoded png data, start with 0x22)	
	* @param d ?avatar key (e.g. s10, d200)			
	* @see processDownloadAvatar()
	* @return Success: |r|s|DATA or |r|s|n|name|DATA / Fail:  |r|e|error code. Data returned as an blob string.
	*/
	bool sendDownloadUserAvatar(char* avatarKey);

	/**
	* Function sendUploadUserAvatar.
	* Upload user avatar, this avatar can use in chat component.
	* @param data Avatar data is a key (system avatar or uploaded custom avatar) Or base64 encoded binary data (Only support png, max size 65535 bytes and please attach 0x22 at the begin of data, e.g. ?ee3489973499dffe==).
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool sendUploadUserAvatar(char* data);				
	
	/**
	* Function sendDelUserAvatar.
	* Delete a user's avatar, Only the user's custom avatar could be deleted.
	* @param avatarKey  avatar key (e.g. c200).
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool sendDelUserAvatar(char* avatarKey);
	
	/**
	* Function sendGetUserAvatarList.
	* et user's avatar list (system avatar and user avatar).
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool sendGetUserAvatarList();
	
	/**
	* Function sendGetUserStoreData.
	* User can store data in the server. This function retun the data stored which key is storeName. Data returned as an blob value.
	* @param storeName Tag of the stored data.
	* @param userName Name of user that we want to get data, optional parameter.
	* @see processUserData()
	* @return Success: |r|s |DATA or |r|s|n|name|DATA/ Fail:  |r|e|error code. 
	*/
	bool sendGetUserStoreData(char* storeName, char* userName = NULL);
	
	/**
	* Function sendSetUserStoreData.
	* User can store data in the server. This function store data in the server which key is storeName.
	* @param storeName Tag of the stored data.
	* @param data Data to store in the server, encoded as an blob string.
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool sendSetUserStoreData(char* storeName, char* data);
	
	/**
	* Function sendDelUserStoreData.
	* User can store data in the server. This function delete data in the server which key is storeName. The key storeName will be deleted also.
	* @param storeName Tag of the stored data.	
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool sendDelUserStoreData(char* storeName);		
	
	/**
	* Function sendGetUserBestScore.
	* Get user's best score in all games.
	* @param userName Name of user. If userName is set to NULL, return current user's.
	* @see processUserBestScore()
	* @return Success: |r|s||user's best score or |r|s|n|name|user's best score / Fail:  |r|e|error code. 
	*/
	bool sendGetUserBestScore(char* userName = NULL);
	
	/**
	* Function sendGetReputation.
	* Get user's reputation.
	* @param userName Name of user. If userName is set to NULL, return current user's.
	* @see processUserReputation()
	* @return Success: |r|s|reputation or |r|s|n|name|reputation / Fail:  |r|e|error code. 
	*/
	bool sendGetReputation(char* userName = NULL);	
	
	/**
	* Function sendPointUserReputation.
	* Give other user reputation point.
	* @param userName Name of user.
	* @param reputation Allow only 1 or 5.
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
#ifdef USE_USER_NUMBER
	bool sendPointUserReputation(char* userName, int reputation, bool isUserMunber = false);	
#else
	bool sendPointUserReputation(char* userName, int reputation);	
#endif //USE_USER_NUMBER
	
	/**
	* Function sendGetUserInfo.
	* Get some user's information.
	* @param name User name. Optional parameters, if don't set, decided by uid.
	* @see processUserInformation()
	* @return Success: |r|s|userInfo1|value|userInfo2|value|...| userInfo N|value or  |r|s|n|name|userInfo1|value|userInfo2|value|...| userInfo N|value   / Fail:  |r|e|error code. 
	*/
	bool sendGetUserInfo(char* name = NULL);
	
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
	bool sendUpdateUserInfo(char* email = NULL, int email_flag = -1, char* country = NULL, char* firstName = NULL, char* lastName = NULL, int sex = -1, char* birthday = NULL);
		
	/**
	* Function sendGetTrophyIcon.
	* Get a trophy's icon data. The icon data is the binary data which encoded by base64.
	* 
	* @param trophyID Trophy ID.
	* @return Success: |r|s|icon data / Fail:  |r|e|error code. 
	* @return ERROR_NO_RESPONSE = 27
	* @return ERROR_NO_GAMEICON = 68
	*/
	bool sendGetTrophyIcon(int trophyID);
	
	/**
	* Function sendAwardTrophy.
	* Award a trophy to a user.
	* 
	* @param trophyID Trophies' id list.
	* @return Success: |r|s / Fail:  |r|e|error code. 
	*/
	bool sendAwardTrophy(int* trophyID, int numberOfID);
	
	/**
	* Function sendGetGameTrophyList.
	* Get all trophy list of a game
	* 	
	* @param ggi Product, Optional parameters, if don't set, decided by GGI.
	* @return Success: |r|s|trophy list / Fail:  |r|e|error code. 
	* @return Format of trophy list  is(Split with ^ and | ):	1st torphy id^1st trophy name^1st trophy description^1st trophy score|2nd torphy id^2nd trophy name^2nd trophy description^2nd trophy score|...
	*/
	bool sendGetGameTrophyList(int ggi = -1);
	
	/**
	* Function sendGetUserGameTrophyList.
	* Get user's all trophy list of a game. Look's like FUNCTION_GET_GAME_TROPHYLIST
	* 
	* @param ggi Product, Optional parameters, if don't set, decided by GGI.
	* @return Success: |r|s|trophy list / Fail:  |r|e|error code. 
	* @return Format of trophy list  is(Split with ^ and | ):	1st torphy id^1st trophy name^1st trophy description^1st trophy score|2nd torphy id^2nd trophy name^2nd trophy description^2nd trophy score|...
	*/
	bool sendGetUserGameTrophyList(int ggi = -1, char* name = NULL);
	
	/**
	* Function sendGetAdvRSS.
	* Get advertisements (RSS)
	* 
	* @param ggi Product, Optional parameters, if don't set, decided by GGI.
	* @return Success: |s|rss / Fail: |r|e|error code.
	* @return RSS example: 
		<br>
		<rss version="2.0">"
			 <channel>
				 <title>Gameloft iPhone RSS</title>  
				 <link>http://www.Gameloft.com</link> 
				 <description>Gameloft iPhone RSS</description> 				
				 <item> 
					<title>Wind 1</title> 
					<description>1st Background For Desctop</description> 
					<link>http://www.gameloft.com/1.html</link> 
					<attachment_id>1</attachment_id> 
					<fileType>jpg</fileType> 
					<fileSize>7246</fileSize> 
					<width>320</width>
					<height>140</height> 
					<times>0</times>
				</item><br>
				<item><br>
					<title>Wind 2</title>
					<description>2nd Background For Desctop</description>
					<link></link>
					<attachment_id>2</attachment_id>
					<fileType>gif</fileType> 
					<fileSize>6850</fileSize>
					<width>320</width>
					<height>140</height>
					<times>0</times>
				</item>
			</channel>
		</rss>
		<br>	
		times : How many seconds the advertisement will show (If times>0).
	*/
	bool sendGetAdvRSS(int ggi = -1);
	
	/**
	* Function sendGetAdvAttachment.
	* Get a advertisement attachment data. The data is the binary data which encoded by base64.
	* 
	* @param attachmentID Attachment id
	* @return  Success: |r|s|data / Fail: |r|e|error code.
	* 
	*/
	bool sendGetAdvAttachment(int attachmentID);
	
	
	/**
	* Function sendGetGameIcon.
	* Get a advertisement attachment data. The data is the binary data which encoded by base64.
	*
	* @param ggi Product, Optional parameters, if don't set, decided by GGI.
	* @param productName Product name, Optional parameters, if don't set, decided by GGI.
	* @return Success: |s|data / Fail: |r|e|error code.
	*/
	bool sendGetGameIcon(int ggi = -1, char* productName = NULL);

	/**
	* Function sendSetUserState.
	* Set user's state : Online(in GL Live) or InGame
	* @param state 2 : In GL Live, 3 : In Game
	* @return Success:f|97|r|s / fail: f|97|r|e|error code
	* @return  ERROR_NO_RESPONSE = 27, ERROR_DATA_INVALID = 66
	*/
	bool sendSetUserState(int state = XPLAYER_USER_STATE_ONLINE);	
	
	/**
	* Function getUserName.
	* Return User name.	
	* @see processUserInformation()
	* @return User name
	*/
	char* getUserName();

	/**
	* Function getRecommandUsername.
	* Return recommand user name.	
	* @return recommand User name
	*/
	char *getRecommandUsername()
	{
		return m_recommendedUsername;
	}
	
	/**
	* Function getPassword.
	* Return User password.		
	* @return User name
	*/
	char* getPassword();
	
	/**
	* Function setPassword.
	* Return User name.	
	* @param  password User password.	
	*/
	void setPassword(char * password);
	
	/**
	* Function getEmail.
	* Return User email.	
	* @see processUserInformation()
	* @return User email.
	*/
	char* getEmail();
	
	/**
	* Function getEmailFlag.
	* Return User email flag.	
	* @see processUserInformation()
	* @return User email flag.
	*/
	bool getEmailFlag();
	
	/**
	* Function getCountry.
	* Return User country.	
	* @see processUserInformation()
	* @return User country.
	*/
	char* getCountry();
	
	/**
	* Function getFirstName.
	* Return User first name.	
	* @see processUserInformation()
	* @return User first name.
	*/
	char* getFirstName();
	
	/**
	* Function getLastName.
	* Return User last name.	
	* @see processUserInformation()
	* @return User last name.
	*/
	char* getLastName();
	
	/**
	* Function getSex.
	* Return User sex.	
	* @see processUserInformation()
	* @return User sex.
	*/
	bool getSex();
	
	/**
	* Function getBirthday.
	* Return User birthday.	
	* @see processUserInformation()
	* @return User birthday.
	*/
	char* getBirthday();
	
	/**
	* Function getOnlineTime.
	* Return user online time.	
	* @see processUserInformation()
	* @return m_onlineTime
	*/
	long getOnlineTime();
	
	/**
	* Function getTotalTrophy.
	* Return user total trophy in all games.	
	* @see processUserInformation()
	* @return m_totalTrophy
	*/
	int getTotalTrophy();
	
	
	/**
	* Function getNumberUserGameList.
	* Count number of game in game list.	
	* @see processUserGameList()
	* @return Number of game in game list.
	*/
	int getNumberUserGameList();
	
	/**
	* Function getUserGamePID.
	* Get the PID of game in the game list, indicated by index.	
	* @param index Index of game in game list.
	* @see processUserGameList()
	* @return Number of game in game list.
	*/
	char* getUserGamePID(int index);
	
	/**
	* Function getUserGameName.
	* Get the name of game in the game list, indicated by index.	
	* @param index Index of game in game list.
	* @see processUserGameList()
	* @return Name of game in game list.
	*/
	char* getUserGameName(int index);
	
	/**
	* Function getUserGameTrophy.
	* Get user score of game in the game list, indicated by index.	
	* @param index Index of game in game list.
	* @see processUserGameList()
	* @return User score.
	*/
	int getUserGameTrophy(int index);
	
	/**
	* Function getCurrentGamePID.
	* Get current game PID in the user state returned by server.
	* @see processUserState()
	* @return Game PID.
	*/
	char* getCurrentGamePID();
	
	/**
	* Function getCurrentGameName.
	* Get current game name in the user state returned by server.
	* @see processUserState()
	* @return Game name.
	*/
	char * getCurrentGameName();
	
	/**
	* Function getCurrentGameTrophy.
	* Get current user score in the user state returned by server.
	* @see processUserState()
	* @return User score.
	*/
	int getCurrentGameTrophy();
	
	/**
	* Function getCurrentUserState.
	* Get current user state in the user state returned by server.
	* @see processUserState()
	* @return User state.
	*/
	bool getCurrentUserState();
	
	/**
	* Function getUserReputation.
	* Get user reputation.
	* @see processUserReputation()
	* @return User reputation.
	*/
	double getUserReputation();
	
	/**
	* Function getUserBadReputationCount.
	* Get user bad reputation count.
	* @see processUserReputation()
	* @return User reputation.
	*/
	int getUserBadReputationCount();
	
	/**
	* Function getUserGoodReputationCount.
	* Get user good reputation count.
	* @see processUserReputation()
	* @return User reputation.
	*/
	int getUserGoodReputationCount();
	
	/**
	* Function getUserTrophy.
	* Get user trophy.
	* @see processUserTrophy()
	* @return User trophy.
	*/
	int getUserTrophy();
	
	/**
	* Function getUserBestScore.
	* Get user best score.
	* @see processUserBestScore()
	* @return User best score.
	*/
	int getUserBestScore();
	
	/**
	* Function getUserAvatarKey.
	* Get user avatar.
	* @see processUserAvatar()
	* @return User avatar.
	*/
	char* getUserAvatarKey();
	
	/**
	* Function getUserAvatarLastUpdate.
	* Get user avatar.
	* @see processUserAvatar()
	* @return m_userAvatarLastUpdate.
	*/
	char* getUserAvatarLastUpdate();

	/**
	* Function getUserAvatarData.
	* Get user avatar.
	* @see processUserAvatarData()
	* @return User avatar.
	*/
	char* getUserAvatarData();

	/**
	* Function getUserData.
	* Get user data.
	* @see processUserData()
	* @return User data.
	*/
	char* getUserData();
	
	/**
	* Function getOtherUserName.
	* Get other user name in some function to get other user properties. If this function return NULL, then the properties are owned by current user.	
	* @return Other user name.
	*/
	char* getOtherUserName();

	/**
	* Function getUserUID()
	* Get user UID
	* @see sendGetUserUID()
	* @return m_UserUID
	*/
	char* getUserUID(){ return m_UserUID;}

	/**
	* Function getGameIconData()
	* Get game icon data
	* @see processUserGameIcon()
	* @return game icon data
	*/
	char *getGameIconData(){ return m_gameIconData;}

	/**
	* Function getGameIconType()
	* Get game icon type
	* @see processUserGameIcon()
	* @return game icon type
	*/
	char *getGameIconType(){ return m_gameIconType;}
	
	/**
	* Function getGameIconUpdateTime()
	* Get game icon update time
	* @see processUserGameIcon()
	* @return m_gameIconUpdateTime
	*/
	long getGameIconUpdateTime(){ return m_gameIconUpdateTime;}	
	
	/**
	* Function getTrophyIconData()
	* Get trophy icon data
	* @see processUserTrophyIcon()
	* @return m_trophyIconData
	*/
	char *getTrophyIconData(){ return m_trophyIconData;}
	
	/**
	* Function getRSS()
	* Get RSS
	* @see processGetADV_RSS()
	* @return m_rss
	*/
	rss getRSS(){ return m_rss;}
	
	/**
	* Function getRssAttachment()
	* Get RSS attachment.
	* @see processRssAttachment()
	* @return m_rssAttachment.
	*/
	char* getRssAttachment(){ return m_rssAttachment;}
	
	/**
	* Function getUserLanguage.
	* Get current user language in the user state returned by server.
	* @see processUserState()
	* @return m_userLanguage.
	*/
	bool getUserLanguage(){ return m_userLanguage;}

	/**
	* Function getAvatarKey.
	* Get avatar's key indecated by position.
	* @see processUserAvatarList()
	* @return m_avatarKeyList[position].
	*/
	char* getAvatarKey(int position);
	
	/**
	* Function getAvatarLastUpdate.
	* Get avatar's lastupdate indecated by position.
	* @see processUserAvatarList()
	* @return m_avatarLastUpdateList[position].
	*/
	char* getAvatarLastUpdate(int position);
		
	/**
	* Function getAvatarStatus.
	* Get avatar's status indecated by position.
	* @see processUserAvatarList()
	* @return m_avatarStatusList[position].
	*/
	int getAvatarStatus(int position);
	
	/**
	* Function getAvatarListCount.
	* Return the number of avatar in the list.
	* @see processUserAvatarList()
	* @return m_avatarListCount.
	*/
	int getAvatarListCount(){ return m_avatarListCount;}	

protected:
	/**
	* Private function OnUpdateSuccess.
	* Handle success message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	bool OnUpdateSuccess( int func_id );

	/**
	* Private function OnUpdateFailure.
	* Handle failure message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	bool OnUpdateFailure( int func_id );
	
	/**
	* Private function processUserGameList.
	* Process list of user played game if success message returned from the server.
	* @see sendGetUserGameList()	
	* @param data Data returned from the server.
	*/
	void processUserGameList(char* data);
	
	/**
	* Private function processUserState.
	* Process user state and related information if success message returned from the server.
	* @see sendGetUserState()	
	* @param data Data returned from the server.
	*/
	void processUserState(char* data);
	
	/**
	* Private function processUserReputation.
	* Process user reputation if success message returned from the server.
	* @see sendGetReputation()	
	* @param data Data returned from the server.
	*/
	void processUserReputation(char* data);
	
	/**
	* Private function processUserTrophy.
	* Process user trophy if success message returned from the server.
	* @see sendGetUserTrophy()	
	* @param data Data returned from the server.
	*/
	void processUserTrophy(char* data);
	
	/**
	* Private function processUserInformation.
	* Process user information if success message returned from the server.
	* @see sendGetUserInfo()	
	* @param data Data returned from the server.
	*/
	void processUserInformation(char* data);
	
	/**
	* Private function processUserBestScore.
	* Process user best score if success message returned from the server.
	* @see sendGetUserBestScore()	
	* @param data Data returned from the server.
	*/
	void processUserBestScore(char* data);
	
	/**
	* Private function processUserAvatar.
	* Process user avatar if success message returned from the server.
	* @see sendGetUserAvatar()	
	* @param data Data returned from the server.
	*/
	void processUserAvatar(char* data);
	
	/**
	* Private function processUserAvatarList.
	* Parsing list of avata's information.
	* @see sendGetUserAvatarList()	
	* @param data Data returned from the server.
	*/	
	void processUserAvatarList(char* data);
	
	/**
	* Private function processDownloadAvatar.
	* Download user's avatar from the server.
	* @see sendDownloadUserAvatar()	
	* @param data Data returned from the server.
	*/	
	void processDownloadAvatar(char* data);
	
	/**
	* Private function processUploadAvatar.
	* After uploaded, server return avatar key and last update time.
	* @see sendUploadUserAvatar()	
	* @param data Data returned from the server.
	*/	
	void processUploadAvatar(char* data);

	/**
	* Private function processUserData.
	* Process user data if success message returned from the server.
	* @see sendGetUserStoreData()	
	* @param data Data returned from the server.
	*/
	void processUserData(char* data);

	/**
	* Private function processUserGameIcon.
	* Process game icon data and icon type if success message returned from the server.
	* @see sendGetGameIcon(int ggi)	
	* @param data Data returned from the server.
	*/
	void processUserGameIcon(char* data);

	/**
	* Private function processUserTrophyIcon.
	* Process trophy icon .
	* @see sendGetTrophyIcon()	
	* @param data Data returned from the server.
	*/
	void processUserTrophyIcon(char* data);	

	/**
	* Private function processGetADV_RSS.
	* Parse RSS data returned from server
	* @see sendGetAdvRSS(int ggi)	
	* @param data Data returned from the server.
	*/
	void processGetADV_RSS(char* data);

	/**
	* Function processRssAttachment.
	* Get the attachment data from webserver
	* @see sendGetAdvAttachment()	
	* @param data Data returned from the server.
	*/
	void processRssAttachment(char* data);	

private:		
	/**
	* Private function clearUserGameList.
	* Clear user's played game list.	
	*/
	void clearUserGameList();

	/**
	* Private function clearUserState.
	* Clear user's state related variable.	
	*/
	void clearUserState();	
	
	/**
	* Private function clearUserInformation.
	* @see processUserInformation()
	* Clear user's information.	
	*/
	void clearUserInformation();
	
	/**
	* Private function clearAvatarList.
	* @see processUserAvatarList()
	* Clear avatar list.
	*/
	void clearAvatarList();					
	
	/**
	* Variable m_UserUID
	* Store the user UID. 
	* @see sendGetUserUID()
	*/
	char* m_UserUID;
	
	/**
	* Variable m_userNumber
	* Store the user number. 
	* @see processUserInformation()
	*/
	char* m_userNumber;
	
	/**
	* Variable m_userName.
	* Store the user name. 
	* @see processUserInformation()
	*/
	char* m_userName;
	
	/**
	* Variable m_password.
	* Store the user password. 
	* @see processUserInformation()
	*/
	char* m_password;
	
	/**
	* Variable m_recommendedUsername.
	* The name returned by server when register name is occupied.
	*/
	char *m_recommendedUsername;

	/**
	* Variable m_email.
	* Store the user email. 
	* @see processUserInformation()
	*/
	char* m_email;
	
	/**
	* Variable m_emailFlag.
	* Store the user email flag. 
	* @see processUserInformation()
	*/
	bool m_emailFlag;
	
	/**
	* Variable m_country.
	* Store the user country. 
	* @see processUserInformation()
	*/
	char* m_country;
	
	/**
	* Variable m_firstName.
	* Store the user first name. 
	* @see processUserInformation()
	*/
	char* m_firstName;
	
	/**
	* Variable m_lastName.
	* Store the user last name. 
	* @see processUserInformation()
	*/
	char* m_lastName ;
	
	/**
	* Variable m_sex.
	* Store the user sex. 
	* @see processUserInformation()
	*/
	bool m_sex ;
	
	/**
	* Variable m_birthday.
	* Store the user birthday. 
	* @see processUserInformation()
	*/
	char* m_birthday;	
	
	/**
	* Variable m_onlineTime.
	* Store online time. 
	* @see processUserInformation()
	*/
	long m_onlineTime;	
	
	/**
	* Variable m_totalTrophy.
	* Store the trophy in all games. 
	* @see processUserInformation()
	*/
	int m_totalTrophy;	
	
	/**
	* Variable m_userGameListPID.
	* Store the list of user played game PID.
	* @see processUserGameList()
	*/
	char** m_userGameListPID;
	
	/**
	* Variable m_userGameListName.
	* Store the list of user played game name.
	* @see processUserGameList()
	*/
	char** m_userGameListName;
	
	/**
	* Variable m_userGameListTrophy.
	* Store the list of user played game score.
	* @see processUserGameList()
	*/
	int* m_userGameListTrophy;
	
	/**
	* Variable m_numberUserGameList.
	* Store the number of user played game.
	* @see processUserGameList()
	*/
	int m_numberUserGameList;
	
	/**
	* Variable m_currentGamePID.
	* Store the game PID in the state return from the server.
	* @see processUserState()
	*/
	char* m_currentGamePID;
	
	/**
	* Variable m_currentGameName.
	* Store the game name in the state return from the server.
	* @see processUserState()
	*/
	char* m_currentGameName;
	
	/**
	* Variable m_currentGameName.
	* Store the game score in the state return from the server.
	* @see processUserState()
	*/
	int m_currentGameTrophy;		
	
	/**
	* Variable m_currentGameName.
	* Store user state in the state return from the server.
	* @see processUserState()
	*/
	bool m_userState;
	
	/**
	* Variable m_userLanguage.
	* Store user's language in the state return from the server.
	* @see processUserState()
	*/
	char* m_userLanguage;
	
	/**
	* Variable m_reputation.
	* Store user reputation.
	* @see processUserReputation()
	*/
	double m_reputation;
	
	/**
	* Variable m_badEvaluationCount.
	* Store the reputation list .
	*/
	int m_badEvaluationCount;
	
	/**
	* Variable m_goodEvaluationCount.
	* Store the reputation list .
	*/
	int m_goodEvaluationCount;
	/**
	* Variable m_trophy.
	* Store user trophy.
	* @see processUserTrophy()
	*/
	int m_trophy;
	
	/**
	* Variable m_userBestScore.
	* Store user best score.
	* @see processUserBestScore()
	*/
	int m_userBestScore;
	
	/**
	* Variable m_userAvatarKey.
	* Store user avatar.
	* @see processUserAvatar()
	*/
	char* m_userAvatarKey;
	
	/**
	* Variable m_userAvatarLastUpdate.
	* Store user avatar.
	* @see processUserAvatar()
	*/
	char* m_userAvatarLastUpdate;
	
	/**
	* Variablem_userAvatarData .
	* Store user avatar.
	* @see processUserAvatar()
	*/
	char* m_userAvatarData;		
	
	/**
	* Variable m_userData.
	* Store user data.
	* @see processUserData()
	*/
	char* m_userData;		
	
	/**
	* Variable m_otherUserName.
	* Store other user name in some function to get other user properties.
	* @see getOtherUserName()
	*/
	char* m_otherUserName;

	/**
	* Variable m_gameIconData.
	* Store the game icon data.
	* @see processUserGameIcon(char* data)
	*/
	char *m_gameIconData;
	
	/**
	* Variable m_trophyIconData.
	* Store the trophy icon data.
	* @see processUserTrophyIcon(char* data)
	*/
	char *m_trophyIconData;
	
	/**
	* Variable m_userPhotoData.
	* Store the user photo data.
	* @see processUserPhoto(char* data)
	*/
	char *m_userPhotoData;
	
	/**
	* Variable m_rssAttachment.
	* Store rss attachment.
	* @see processRssAttachment(char* data)
	*/
	char *m_rssAttachment;
	
	/**
	* Variable m_gameIconType.
	* Store the game icon type.
	* @see processUserGameIcon(char* data)
	*/
	char m_gameIconType[8];
	
	/**
	* Variable m_gameIconUpdateTime.
	* Store the game icon type.
	* @see processUserGameIcon(char* data)
	*/
	long m_gameIconUpdateTime;
	
	/**
	* Variable m_rss.
	* Store RSS information.
	* @see processGetADV_RSS()
	*/
	rss m_rss;

	/**
	* Variable m_avatarKeyList.
	* Store the list of avatar keys.
	* @see processUserAvatarList()
	*/
	char** m_avatarKeyList;

	/**
	* Variable m_avatarLastUpdateList.
	* Store the list of avatar last update.
	* @see processUserAvatarList()
	*/
	char** m_avatarLastUpdateList;

	/**
	* Variable m_avatarStatusList.
	* Store the list of avatar checking status.
	* Checking status:
	* 	0: nothing
	* 	1: waiting for checking
	* 	2: checking false
	* 	3: checking pass	
	* @see processUserAvatarList()
	*/
	int* m_avatarStatusList;
	
	/**
	* Variable m_avatarListCount.
	* Count the number os item in avatar list.
	* @see processUserAvatarList()
	*/
	int m_avatarListCount;	
};

#endif
