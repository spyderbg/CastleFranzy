#ifndef __GL_XPLAYER_SERVER_CONFIG_H__
#define __GL_XPLAYER_SERVER_CONFIG_H__
#include "GLXPlayerWebComponent.h"
#include <time.h>
namespace gllive {
	struct GameDate
	{
		int year;
		int yday;
	};

	class GLXPlayerSereverConfig : public GLXPlayerWebComponent
	{
	public:
		GLXPlayerSereverConfig(void);

		virtual ~GLXPlayerSereverConfig(void);

		/**
		*Function SendGetSreverConfig.
		*getAllServerURL from web
		*
		*f|1|i|ggi|v|0.01|
		*/
		bool SendGetSreverConfig(int& retCode);

		/*static char* m_webConfig_XPlayerURL;
		static char* m_webConfig_PhpVer;
		static char* m_webConfig_ChatServer;
		static char* m_webConfig_ChatDomain;
		static char* m_webConfig_ChatRoomDomain;*/
		static bool s_isLoadConfig;
		//static int s_runCount;


		static char m_webConfig_XPlayerURL[];
		static char m_webConfig_XPlayerDWURL[];
		static char m_webConfig_PhpVer[];
		static char m_webURLType[];
		static char m_webConfig_ChatServer[];
		static char m_webConfig_ChatDomain[];
		static char m_webConfig_ChatRoomDomain[];
		static int m_webConfig_ggi;

	private:
		GameDate* m_lastTime;
		GameDate* m_curTime;

	private:
		/**
		* Protected function OnUpdateSuccess.
		* Handle success message returned from the Web server.	
		* @param func_id Function ID.
		* @return True / False.
		*/
		virtual bool OnUpdateSuccess( int func_id );
		bool LoadConfig();
		char* m_gameVersion;
		char* m_configGameVersion;
		bool SaveServerConfig();
		bool loadServerConfig();
		bool isOutOfDate(int days);
		void GetCurTimeDate(GameDate* date);
	};
}


#endif