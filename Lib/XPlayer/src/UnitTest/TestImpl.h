#ifndef __TESTIMPL_H__
#define __TESTIMPL_H__

#include "../../include/Login/GLXPlayerLogin.h"
#include "UnitTest++.h"
#include "memory.h"
#include "TestValue.h"
#include "DEF.h"

using namespace UnitTest;

class TestImpl : public GLXPlayerObserver, public Test
{
public:
	
	TestImpl(GLXPlayerLogin * pGLXPlayerLogin, char const* testName, char const* suiteName = "DefaultSuite"):Test(testName, suiteName)
	{	
		m_errorCode = ERROR_INIT;
		m_GLXPlayerLogin = pGLXPlayerLogin;
		m_GLXPlayerLogin->RegisterObserver(this);
		m_testFinished = false;
	}

	virtual ~TestImpl()
	{					
	}		
	
	void runLoginProcess(char* userName, char* password, int type = 1, char* deviceToken = NULL, bool isReturnNumber = false, bool isReturnName = false)
	{
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			OnRequestFailure( FUNCTION_LOGIN, ERROR_USER_HAS_LOGIN);
		}
		m_GLXPlayerLogin->SendLogin(userName, password, type, deviceToken,isReturnNumber, isReturnName);
		for(int i = 0; i < 1500; i++)
		{
			for(int j = 0; j < 1000; j++)
			{
				m_GLXPlayerLogin->Update();
			}
		}		
	}
	
	void runLogoutProcess()
	{
		if(!m_GLXPlayerLogin->IsLoggedIn())
		{
			OnRequestFailure( FUNCTION_LOGOUT, ERROR_HAS_NOT_LOGIN);
		}
		m_GLXPlayerLogin->SendLogout();
		for(int i = 0; i < 1500; i++)
		{
			for(int j = 0; j < 1000; j++)
			{
				m_GLXPlayerLogin->Update();
			}
		}		
	}
	
	void runKeepAliveProcess()
	{		
		m_GLXPlayerLogin->KeepAlive();
		
		for(int i = 0; i < 1500; i++)
		{
			for(int j = 0; j < 1000; j++)
			{
				m_GLXPlayerLogin->Update();
			}
		}
	}
	
	int checkResponseUID( char* data)
	{
		int pos =0;
		char token[ONLINE_MAX_PACKET_SIZE];
		XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
		getValue(data, token, pos++, '|');
		if(XP_API_STRCMP(token, "u") == 0)
		{					
			XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
			getValue(data, token, pos++, '|');
			if(XP_API_STRLEN(token) > 0)
			{
				return ERROR_NONE;
			}
			else
			{
				return ERROR_BAD_RESPONSE;
			}
		}
		else
		{
			return ERROR_BAD_RESPONSE;	
		}
	}
	
	int checkNumberOfToken(char* data, int number)
	{
		int pos =0;
		char token[ONLINE_MAX_PACKET_SIZE];			
		do
		{
			XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
			getValue(data, token, pos++, '|');
		}
		while(XP_API_STRLEN(token) > 0);
		
		if((pos - 1) == number)
		{					
			return ERROR_NONE;					
		}
		else
		{
			return ERROR_BAD_RESPONSE;
		}
	}
	
	void OnNetworkError()
	{
		m_errorCode = ERROR_CONNECTION;
		CHECK_EQUAL(ERROR_NONE, m_errorCode);
		m_testFinished = true;
	}

	void OnRequestTimeout( int func_id )
	{
		m_errorCode = ERROR_REQUEST_TIME_OUT;
		CHECK_EQUAL(ERROR_NONE, m_errorCode);
		m_testFinished = true;
	}

	void OnRequestFailure( int func_id, int error_code )
	{
		m_errorCode = error_code;
		CHECK_EQUAL(ERROR_NONE, m_errorCode);
		m_testFinished = true;
	}

	void OnRequestSuccess( int func_id, char * data, int data_len )
	{	
		char token[ONLINE_MAX_PACKET_SIZE] = {0};
		int pos = 0;

		switch(func_id)
		{			
			// For GLXPLayerLogin
			case FUNCTION_LOGIN:				
				m_errorCode = ERROR_NONE;				
				break;				
			case FUNCTION_LOGOUT:				
				m_errorCode = checkResponseUID(data);									
				break;				
			case FUNCTION_KEEP_ALIVE:				
				m_errorCode = ERROR_NONE;	
				break;				
			case FUNCTION_APPLE_PUSH_NOTIFICATION_SERVER:
				m_errorCode = checkNumberOfToken(data, 7); //These are 7 token returned in this function			
				break;				
			case FUNCTION_GET_PRODUCT:
				m_errorCode = ERROR_NONE;	
				break;
				
			// For GLXPlayerUser
			case FUNCTION_CHANGE_USERNAME:
				m_errorCode = checkResponseUID(data);
				break;				
			case FUNCTION_CHANGE_PASSWORD:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_CHANGE_EMAIL:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_CHANGE_COUNTRY:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_REGISTER:
				m_errorCode = checkResponseUID(data);
				break;
			case FUNCTION_GET_COUNTRY:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_USED_APPS:
				{
					pos = 0;
					XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
					getValue(data, token, pos++, '|');
					if(XP_API_STRLEN(token) > 0)
					{
						int totalApp = XP_API_ATOI(token);
						m_errorCode = checkNumberOfToken(data, totalApp*5 + 1);					
					}
					else
					{
						m_errorCode = ERROR_BAD_RESPONSE;
					}
				}
				break;
			case FUNCTION_RETRIEVAL_PASSWORD:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_USER_GAMELIST:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_USER_STATE:
				{
					pos = 0;
					XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
					getValue(data, token, pos++, '|');
					if(XP_API_STRCMP(token, "n") == 0 || XP_API_STRCMP(token, "un") == 0)
					{
						pos++;
						XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
						getValue(data, token, pos++, '|');
						if(XP_API_STRCMP(token, "t") == 0)
						{						
							pos++;
							XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
							getValue(data, token, pos++, '|');
							if(XP_API_STRCMP(token, "l") == 0)
							{
								pos++;
								XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
								getValue(data, token, pos++, '|');
								if(XP_API_STRCMP(token, "a") == 0)
								{
									m_errorCode = ERROR_NONE;
								}
								else
								{
									m_errorCode = ERROR_BAD_RESPONSE;
								}
							}
							else
							{
								m_errorCode = ERROR_BAD_RESPONSE;
							}							
						}
						else
						{
							m_errorCode = ERROR_BAD_RESPONSE;
						}						
					}
					else
					{
						m_errorCode = ERROR_BAD_RESPONSE;
					}
				}				
				break;
			case FUNCTION_SET_USER_STORE_DATA:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_USER_STORE_DATA:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_REPUTATION:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_POINT_USER_REPUTATION:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_USER_UID:
				{
					pos = 0;
					XP_API_MEMSET( token, 0, ONLINE_MAX_PACKET_SIZE );
					getValue(data, token, pos++, '|');
					if(XP_API_STRLEN(token) > 0)
					{						
						m_errorCode = ERROR_NONE;				
					}
					else
					{
						m_errorCode = ERROR_BAD_RESPONSE;
					}
				}
				break;
			case FUNCTION_GET_USER_BEST_SCORE:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_USER_AVATAR:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_DOWNLOAD_USER_AVATAR:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_UPLOAD_USER_AVATAR:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_DEL_USER_STORE_DATA:
				m_errorCode = ERROR_NONE;
				break;			
			case FUNCTION_GET_USER_INFO:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_UPDATE_USER_INFO:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_TROPHY_ICON:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_AWARD_TROPHY:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_GAME_TROPHYLIST:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_USER_GAME_TROPHYLIST:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_ADV_RSS:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_ADV_ATTACHMENT:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_GAME_ICON:
				m_errorCode = ERROR_NONE;
				break;
			
			//For testing GLXPlayerUserFriend
			case FUNCTION_ADD_USER_FRIEND:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_CONFIRM_USER_FRIEND:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_DEL_USER_FRIEND:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_USER_FRIENDS:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_USER_FRIENDS_COUNT:
				m_errorCode = ERROR_NONE;
				break;
				
			//For GLXPlayerLeaderboard
			case FUNCTION_UPLOAD_SCORE:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_RANKINGS:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_RANKINGS_AROUND_PLAYER:
				m_errorCode = ERROR_NONE;
				break;
				
			//For GLXPlayerMessage
			case FUNCTION_GET_MESSAGE_COUNT:				
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_MESSAGE_HEADER:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_READ_MESSAGE:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_SEND_MESSAGE:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_DEL_MESSAGE:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_MY_SENT_MESSAGE_COUNT:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_GET_MY_SENT_MESSAGE_HEADER:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_READ_MY_SENT_MESSAGE:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_DEL_MY_SENT_MESSAGE:
				m_errorCode = ERROR_NONE;
				break;
			
			//For GLXPlayerChat
			case FUNCTION_GET_CHAT_WARNING:
				m_errorCode = ERROR_NONE;
				break;
				
			//For GLXPlayerYoutube
			case FUNCTION_GET_VIDEO_INFO:
				m_errorCode = ERROR_NONE;
				break;
			case FUNCTION_SET_VIDEO_STATUS:
				m_errorCode = ERROR_NONE;
				break;
				
			default:
				m_errorCode = ERROR_NONE;
				break;
		}		

		CHECK_EQUAL(ERROR_NONE, m_errorCode);	
		
		if(func_id != FUNCTION_LOGIN && func_id != FUNCTION_LOGOUT && func_id != FUNCTION_KEEP_ALIVE)
		{
			m_testFinished = true;
		}		
	}	
		
protected:				
	int m_errorCode;
	bool m_testFinished;
	GLXPlayerLogin*  m_GLXPlayerLogin;
};
#endif //__TESTIMPL_H__