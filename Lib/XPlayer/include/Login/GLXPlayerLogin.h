#ifndef __GL_XPLAYER_LOGIN_H__
#define __GL_XPLAYER_LOGIN_H__

#include "../Common/GLXPlayerWebComponent.h"

/**
* Class GLXPlayerLogin. 
* Class GLXPlayerLogin is designed for login, logout and keep the connection to web server. 
*
* @author  World Online Team <<online-adm@gameloft.com>>
* @author  Tran Thai Phi <<phi.tranthai@gameloft.com>>
 * @version 1.0.0 
 *  
 */
class GLXPlayerLogin : public GLXPlayerWebComponent
{

public:

	enum XPlayerLoginType
	{
		XPlayer_Login = 0,
		XPlayer_Login_Live = 1,
		XPlayer_Login_Game = 2
	};

	/**
	* GLXPlayerLogin constructor.
	* Create an instance of GLXPlayerLogin.
	*/
	GLXPlayerLogin(void);		
	
	/**
	* GLXPlayerLogin destructor.
	* Destroy an instance of GLXPlayerLogin.
	*/
	virtual ~GLXPlayerLogin(void);
	
	/**
	* Function Update.
	* Called each frame to Update state.	
	*/
	virtual void Update();	
	
	/**
	* Function SendLogin.
	* Login web server.
	* @param name User name.
	* @param password User password.
	* @param type prohibit multi-user login at the same time and set user state optional,default value is 0.
	* @param deviceToken Device token (Optional, iPhone only, need “o?parameter).
	* @param isReturnUserNumber Whether return user's user number from database.
	* @param isReturnName Whether return user's username from database.
	* @param language Language, must ISO 639-1 (2 chars, e.g. En, fr, jp ....)  optional, default is "en" (English).
	* @param bSendLastTimeStamp Optional, send last timestamp or not.
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool SendLogin( char * name, char * password, int type = 0, char* deviceToken = NULL, bool isReturnUserNumber = false, bool isReturnName = false, char* language = "en", bool bSendLastTimeStamp = false);
	
	
	/**
	* Function SendLogout.
	* Logout web server.	
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool SendLogout();	
	
	/**
	* Function SendGetAPNSInfo.
	* Send request to get apple push notification server informations.
	* @return Success: |r|s|server ip|server port|publicCertPemPath|privateKeyPemPath|privateKeyPassword 
	* @return Fail:  |r|e|error code  ERROR_NO_RESPONSE   = 27;	ERROR_INVALID_GGI    = 28
	*/	
	bool SendGetAPNSInfo(int tag = 0);	
	
	/**
	* Function SendGetProductInfo.
	* Get products' information by GGIs
	* @param ggiList Optional,split with??. If don't input, decided by GGI.
	* @return Success:f|95|r|s|product information list
	* @return Product information include product's GGI,product id,product name
	* @return Fail:  f|95|r|e|error code
	*/
	bool SendGetProductInfo(char * ggiList = NULL);
	
	/**
	* Function SendGetGeneralConditions.
	* Get general conditions from server 
	* @param lang Language e.g. EN, DE, FR ....(iso 639-1 ), default is EN
	* @return Success: |r|s
	* @return Fail:  |r|e|error code
	*/
	bool SendGetGeneralConditions(char * lang = "EN");
	
	/**
	* Function IsLoggedIn.
	* Check if logged in or not.
	* @return True / False.
	*/
	bool IsLoggedIn(){return m_isLoggedIn;}
	
	/**
	* Function KeepAlive.
	* Keep connection to web server alive.	
	*/
	void KeepAlive();
	
	/**
	* Function GLXPlayerLogin clearProductInfo().
	* Clear the list of product's information.
	*/
	void clearProductInfo();
	
	/**
	* Function getAPNSIP.
	* @see processApplePushServer()	
	* @return m_APNS_IP.
	*/
	char* getAPNSIP();
	
	/**
	* Function getAPNSPort.
	* @see processApplePushServer()	
	* @return m_APNS_Port.
	*/
	int getAPNSPort();
	
	/**
	* Function getAPNSFeedBackIP.
	* @see processApplePushServer()	
	* @return m_APNS_FeedBack_IP.
	*/
	char* getAPNSFeedBackIP();
	
	/**
	* Function getAPNSFeedBackPort.
	* @see processApplePushServer()	
	* @return m_APNS_FeedBack_Port.
	*/
	int getAPNSFeedBackPort();
	
	/**
	* Function getAPNSPublicPemPath.
	* @see processApplePushServer()	
	* @return m_publicPemPath.
	*/
	char* getAPNSPublicPemPath();
	
	/**
	* Function getAPNSPrivateKeyPemPath.
	* @see processApplePushServer()	
	* @return m_privateKeyPemPath.
	*/
	char* getAPNSPrivateKeyPemPath();
	
	/**
	* Function getAPNSPrivateKeyPemPath.
	* @see processApplePushServer()	
	* @return m_privateKeyPassword.
	*/
	char* getAPNSPrivateKeyPassword();
	
	/**
	* Function getProductListCount.
	* @see processGetProductInfo()	
	* @return m_productListCount.
	*/
	int getProductListCount();
	
	/**
	* Function getProductListCount.
	* @see processGetProductInfo()	
	* @param index index of product.	
	* @return m_productListGGI[index].
	*/
	int getProductGGI(int index);
	
	/**
	* Function getProductID.
	* @see processGetProductInfo()	
	* @param index index of product.	
	* @return m_productListID[index].
	*/
	int getProductID(int index);
	
	/**
	* Function getProductName.
	* @see processGetProductInfo()	
	* @param index index of product.	
	* @return m_productListName[index].
	*/
	char* getProductName(int index);
	
	/**
	* Function getUserName.		
	* @return m_userName.
	*/
	char* getUserName(){return m_userName;}
	
	/**
	* Function getUserNumber.		
	* @return m_userNumber.
	*/
	char* getUserNumber(){return m_userNumber;}
	
	/**
	* Function getUserLanguage.		
	* @return m_userLanguage.
	*/
	char* getUserLanguage(){ return m_userLanguage;}
	
	/**
	* Function getGeneralCondition.		
	* @return m_generalCondition.
	*/
	char* getGeneralCondition(){ return m_generalCondition;}
	
	/**
	* Function getLastUpdateTime.		
	* @return m_lastUpdateTime.
	*/
	char* getLastUpdateTime(){ return m_lastUpdateTime;}
	
	bool SendGetLobbyServer();
	bool SendGameInvitationGetLobbyServer(char* name);

protected:
	/**
	* Protected function OnUpdateSuccess.
	* Handle success message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	virtual bool OnUpdateSuccess( int func_id );
	
	/**
	* Private function processApplePushServer.
	* Process product information from function SendGetAPNSInfo().
	* @see SendGetAPNSInfo()	
	* @param msgString Data returned from the server.
	*/
	void processApplePushServer(char* msgString);
	
	/**
	* Private function processGetProductInfo.
	* Process product information from function SendGetProductInfo().
	* @see SendGetProductInfo()	
	* @param msgString Data returned from the server.
	*/
	void processGetProductInfo(char* msgString);
	
	/**
	* Protected function SendKeepAlive.
	* Keep connection to web server alive.	
	* @return Success: |r|s / Fail:  |r|e|error code
	*/
	bool SendKeepAlive();
	
	/**
	* Protected function ShouldSendKeepAlive.
	* Calculate should call send keep alive or not	
	* @return True / False
	*/
	bool ShouldSendKeepAlive();		
	
	/**
	* Variable m_keepAliveReturns.
	* Time value response from server form last called sendKeepAlive()	
	*/
	long m_keepAliveReturns;
	
	/**
	* Variable m_isLoggedIn.
	* Is logged in?
	*/
	bool m_isLoggedIn;
	
	/**
	* Variable m_lastSendKeepAliveTime.
	* The time of last called sendKeepAlive().
	*/
	long m_lastSendKeepAliveTime;	
	
	/**
	* Variable m_APNS_IP.
	* Apple Push server IP.
	*/
	char* m_APNS_IP;	
	
	/**
	* Variable m_APNS_Port.
	* Apple push server port.
	*/
	int m_APNS_Port;	
	
	/**
	* Variable m_APNS_FeedBack_IP.
	* Apple Push feed back server IP.
	*/
	char* m_APNS_FeedBack_IP;	
	
	/**
	* Variable m_APNS_Port.
	* Apple push feed back server port.
	*/
	int m_APNS_FeedBack_Port;	
	
	/**
	* Variable m_publicPemPath.
	* Public Certification Pem Path.
	*/
	char* m_publicPemPath;	
	
	/**
	* Variable m_privateKeyPemPath.
	* Private key pem path.
	*/
	char* m_privateKeyPemPath;	
		
	/**
	* Variable m_privateKeyPassword.
	* Private key password.
	*/
	char* m_privateKeyPassword;	
	
	
	/**
	* Variable m_productListCount.
	* Count the number of product in the list.
	*/
	int m_productListCount;	
	
	/**
	* Variable m_productListGGI.
	* Store the list of GGI .
	*/
	int* m_productListGGI;
	
	/**
	* Variable m_productListID.
	* Store the list of product ID.
	*/
	int* m_productListID;	
	
	/**
	* Variable m_productListName.
	* Store the list of product name.
	*/
	char** m_productListName;	
	
	/**
	* Variable m_userName.
	* Store user name.
	*/
	char* m_userName;	
	
	/**
	* Variable m_userLanguage.
	* Store user number.
	*/
	char* m_userNumber;	
	
	/**
	* Variable m_userLanguage.
	* Store current user language.
	*/
	char* m_userLanguage;	
	
	/**
	* Variable m_generalCondition.
	* Store general condition after login or function get general condition.
	*/
	char* m_generalCondition;
	
	/**
	* Variable m_lastUpdateTime.
	* Store the time when call get general condition.
	*/
	char* m_lastUpdateTime;	
	
};

#endif // __GL_XPLAYER_MESSAGE_H__
