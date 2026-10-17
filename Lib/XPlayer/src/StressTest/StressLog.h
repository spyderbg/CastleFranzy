#include "../../include/Game_defines.h"
#ifdef LOBBY_THREAD_TEST
#ifndef __STRESS_LOG__
#define __STRESS_LOG__
#define STRESS_LOG_FILE_NAME ".\\lobbytest\\StressLog.txt"
#include "../../include/Porting.h"
class StressLog
{
public :
	StressLog();
	~StressLog();
	/**
	* set userId
	*/
	void setId(int id)
	{
		m_id=id;
	}
	/**
	* get userId
	* @return m_id
	*/
	int getId() const
	{
		return m_id;
	}
	/**
	* set userName
	*/
	void setUserName(const char *data)
	{
		m_userName=XP_API_STRNEW(data);

	}
	/**
	* get userName
	* @return m_userName
	*/
	const char* getUserName() const
	{
		return m_userName;
	}
	/**
	* set login web server time and set b_loginWebFail to false
	*/
	void setWebTime(long time)
	{
		b_loginWebFail=false;
		m_webTime=time;
	}
	/**
	* get login web server time
	* @return m_webTime
	*/
	long getWebTime()const
	{
		return m_webTime;
	}

	/**
	* get is login web failed
	* @return b_loginWebFail
	*/
	bool getb_loginWebFailed() const
	{
		return b_loginWebFail;
	}
	/**
	* set lobbyConnectTime and set b_lobbyConnectFail to false
	*/
	void setlobbyConnectTime(long time)
	{
		b_lobbyConnectFail=false;
		m_lobbyConnectTime=time;
	}
	/**
	* get lobbyConnectTime
	* @return m_lobbyConnectTime
	*/
	long getConnectLobbyTime()const
	{
		return m_lobbyConnectTime;
	}

	/**
	* get lobbyConnectFail
	* @return b_lobbyConnectFail
	*/
	bool getConnectLobbyFailed()
	{
		return b_lobbyConnectFail;
	}
	/**
	* set lobbyLoginTime and set b_lobbyLoginFail to false
	*/
	void setlobbyLoginTime(long time)
	{
		b_lobbyLoginFail=false;
		m_lobbyLoginTime=time;
	}
	/**
	* get lobbyLoginTime
	* @return lobbyLoginTime
	*/
	long getlobbyLoginTime()const
	{

		return m_lobbyLoginTime;
	}
	/**
	* get lobbyLoginFail
	* @return b_lobbyLoginFail
	*/
	bool getlobbyLoginFailed()
	{
		return b_lobbyLoginFail;
	}



	/**
	*set lobbyCreateTime and set b_lobbyCreateFail to false
	*/
	void setLobbyCreateTime(long time)
	{
		b_lobbyCreateFail=false;
		m_lobbyCreateTime=time;
	}
	long getLobbyCreateTime()
	{
		return m_lobbyCreateTime;
	}
	void setSendLobbyCreate()
	{
		b_sendLobbyCreate=true;
	}
	bool getSendLobbyCreate()
	{
		return b_sendLobbyCreate;
	}
	bool getlobbyCreateFail()
	{
	return b_lobbyCreateFail;
	}

	/**
	*set lobbyJoinTime and set b_lobbyJoinFail to false
	*/
	void setLobbyJoinTime(long time)
	{
		b_lobbyJoinFail=false;
		m_lobbyJoinTime=time;
	}
	long getLobbyJoinTime()
	{
		return m_lobbyJoinTime;
	}
	void setSendLobbyJoin()
	{
		b_sendLobbyJoin=true;
	}
	bool getSendLobbyJoin()
	{
		return b_sendLobbyJoin;
	}
	bool getlobbyJoinFail()
	{
	return b_lobbyJoinFail;
	}



	/**
	*set lobbyFilterTime and set b_lobbyFilterFail to false
	*/
	void setLobbyFilterTime(long time)
	{
		b_lobbyFilterFail=false;
		m_lobbyFilterTime=time;
	}
	long getLobbyFilterTime()
	{
		return m_lobbyFilterTime;
	}
	void setSendLobbyFilter()
	{
		b_sendLobbyFilter=true;
	}
	bool getSendLobbyFilter()
	{
		return b_sendLobbyFilter;
	}
	bool getlobbyFilterFail()
	{
	return b_lobbyFilterFail;
	}


	/**
	* export log
	* @param data return log
	*/
	void exportLog(char** data)const;
private :
	StressLog(const StressLog&);

	StressLog& operator=(const StressLog&);
	int m_id;
	char* m_userName;

	long m_webTime;
	bool b_loginWebFail;

	long m_lobbyConnectTime;
	bool b_lobbyConnectFail;

	long m_lobbyLoginTime;
	bool b_lobbyLoginFail;

	long m_lobbyCreateTime;
	bool b_sendLobbyCreate;
	bool b_lobbyCreateFail;

	long m_lobbyJoinTime;
	bool b_sendLobbyJoin;
	bool b_lobbyJoinFail;

	long m_lobbyFilterTime;
	bool b_sendLobbyFilter;
	bool b_lobbyFilterFail;


};




#endif
#endif