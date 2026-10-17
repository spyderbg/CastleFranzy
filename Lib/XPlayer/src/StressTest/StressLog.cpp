#include "../../include/Game_defines.h"
#ifdef LOBBY_THREAD_TEST
#include "StressLog.h"

StressLog::StressLog():m_id(0),m_webTime(0),b_loginWebFail(true),m_lobbyConnectTime(0),b_lobbyConnectFail(true),
m_lobbyLoginTime(0),
b_lobbyLoginFail(true),
m_lobbyCreateTime(0),
b_sendLobbyCreate(false),
b_lobbyCreateFail(true),
m_lobbyJoinTime(0),
b_sendLobbyJoin(false),
b_lobbyJoinFail(true),
m_lobbyFilterTime(0),
b_sendLobbyFilter(false),
b_lobbyFilterFail(true)
{
	m_userName=NULL;
}
StressLog::~StressLog()
{
	XP_API_DEL(m_userName);

}


void StressLog::exportLog(char** data)const
{
	*data=new char[255];
	int l=0;
	if(!b_loginWebFail)

	{
		XP_API_MEMSET(*data,0,255);

		XP_API_SPRINTF(*data,"--%s  webTime:%d ",m_userName,m_webTime);
		l=XP_API_STRLEN(*data);
		if(!b_lobbyConnectFail)
		{
			XP_API_SPRINTF((*data)+l," connectLobbyTime:%d",m_lobbyConnectTime);
			l=XP_API_STRLEN(*data);
			if(!b_lobbyLoginFail)
			{

				XP_API_SPRINTF((*data)+l," loginLobbyTime:%d",m_lobbyLoginTime);
	            l=XP_API_STRLEN(*data);
              if(b_sendLobbyCreate)
			  {
                XP_API_SPRINTF((*data)+l," <LobbyCreate> ");
				   l=XP_API_STRLEN(*data);
				if(!b_lobbyCreateFail)
				{
					XP_API_SPRINTF((*data)+l," LobbyCreateTime:%d",m_lobbyCreateTime);
				}
				else
				{
                  XP_API_SPRINTF((*data)+l," failed!");
				}

			  }
			  if(b_sendLobbyFilter)
			  {
				   XP_API_SPRINTF((*data)+l," <LobbyFilter> ");
				   l=XP_API_STRLEN(*data);
				if(!b_lobbyFilterFail)
				{
					XP_API_SPRINTF((*data)+l," LobbyFilterTime:%d",m_lobbyFilterTime);
				}
				else
				{
                  XP_API_SPRINTF((*data)+l," failed!");
				}

			
			  }
			  if(b_sendLobbyJoin)
			  {
				  XP_API_SPRINTF((*data)+l," <LobbyJoin> ");
				   l=XP_API_STRLEN(*data);
				if(!b_lobbyJoinFail)
				{
					XP_API_SPRINTF((*data)+l," LobbyJoinTime:%d",m_lobbyJoinTime);
				}
				else
				{
                  XP_API_SPRINTF((*data)+l," failed!");
				}
			  }









			}else
			{
				XP_API_SPRINTF((*data)+l," loginLobby failed!");
			}


		}
		else
		{
			XP_API_SPRINTF((*data)+l," connectLobby failed!");
		}

	}else
	{
		XP_API_MEMSET(*data,0,100);

		XP_API_SPRINTF(*data,"--%s  Login Web failed",m_userName);


	}
	l=XP_API_STRLEN(*data);
	XP_API_SPRINTF(*data+l,"\r\n");

}
#endif