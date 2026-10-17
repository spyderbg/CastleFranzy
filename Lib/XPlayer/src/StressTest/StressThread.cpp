#include "../../include/Game_defines.h"
#ifdef LOBBY_THREAD_TEST
#include "StressThread.h"



Queue<StressLog*> queue;
DWORD CALLBACK  StressThread::run0(PVOID param)
{
	StressThread * st=(StressThread*)param;
	st->run();
	return (0);
}
int  StressThread::run()
{


	int threadCount=pa;
	if(threadCount==0)
		return 0;
	XP_FILE_HANDLE pFile;
	pFile = XP_API_FILE_OPEN( STRESS_LOG_FILE_NAME, "at+" );

	if(!pFile)
	{
		XP_DEBUG_OUT("SaveProfile(), Error creating file %s \n", PROFILE_FILE_NAME);
		return false;
	}
	char* s = NULL;
	GetCurrentSystemTime( &s );

	char title[1024];
	memset(title,0,1024);


	XP_API_SPRINTF(title,
		"\r\nTime:%s\r\n"
		"****************************************************************************\r\n"\
		"****************************Begin Log***************************************\r\n"\
		"****************************************************************************\r\n"\
		,s);
	XP_API_FILE_WRITE(title,	1, strlen(title),pFile);	

	XP_API_DEL( s );
	long totalWebTime=0;
	long maxWebTime=0;
	int failWebLoginCount=0;

	long totalConLobbyTime=0;
	long maxConLobbyTime=0;
	int failLobbyConnect=0;

	long totalLogLobbyTime=0;
	long maxLogLobbyTime=0;
	int failLobbyLogin=0;

	long totalCreateLobbyTime=0;
	long maxCreateLobbyTime=0;
	int failLobbyCreate=0;
	int totalLobbyCreateThread=0;
	//long totalLogLobbyTime=0;
	//long maxLogLobbyTime=0;
	//int failLobbyLogin=0;
	long totalFilterLobbyTime=0;
	long maxFilterLobbyTime=0;
	int failLobbyFilter=0;
	int totalLobbyFilterThread=0;

	long totalJoinLobbyTime=0;
	long maxJoinLobbyTime=0;
	int failLobbyJoin=0;
	int totalLobbyJoinThread=0;
	while(threadCount>0)
	{
		bool empty=queue.empty();
		if(!empty)
		{

			StressLog* p;
			p=queue.pop();

			char* data=NULL;
			//web
			long curTime=p->getWebTime();
			if(maxWebTime<curTime)
				maxWebTime=curTime;
			p->exportLog(&data);
			totalWebTime+=curTime;
			failWebLoginCount=p->getb_loginWebFailed()==true?failWebLoginCount+1:failWebLoginCount;
			//lobby
			curTime=p->getConnectLobbyTime();
			if(maxConLobbyTime<curTime)
				maxConLobbyTime=curTime;
			totalConLobbyTime+=curTime;
			failLobbyConnect=p->getConnectLobbyFailed()==true?failLobbyConnect+1:failLobbyConnect;

			curTime=p->getlobbyLoginTime();
			if(maxLogLobbyTime<curTime)
				maxLogLobbyTime=curTime;
			totalLogLobbyTime+=curTime;
			failLobbyLogin=p->getlobbyLoginFailed()==true?failLobbyLogin+1:failLobbyLogin;


			if(p->getSendLobbyCreate())
			{
				totalLobbyCreateThread++;
				curTime=p->getLobbyCreateTime();
				if(maxCreateLobbyTime<curTime)
					maxCreateLobbyTime=curTime;
				totalCreateLobbyTime+=curTime;
				failLobbyCreate=p->getlobbyCreateFail()==true?failLobbyCreate+1:failLobbyCreate;


			}

			if(p->getSendLobbyFilter())
			{
				totalLobbyFilterThread++;
				curTime=p->getLobbyFilterTime();
				if(maxFilterLobbyTime<curTime)
					maxFilterLobbyTime=curTime;
				totalFilterLobbyTime+=curTime;
				failLobbyFilter=p->getlobbyFilterFail()==true?failLobbyFilter+1:failLobbyFilter;


			}

			if(p->getSendLobbyJoin())
			{
				totalLobbyJoinThread++;
				curTime=p->getLobbyJoinTime();
				if(maxJoinLobbyTime<curTime)
					maxJoinLobbyTime=curTime;
				totalJoinLobbyTime+=curTime;
				failLobbyJoin=p->getlobbyJoinFail()==true?failLobbyJoin+1:failLobbyJoin;


			}

			//XP_API_FILE_WRITE(data,	1, strlen(data),pFile);	
			delete data;
			threadCount--;
			delete p;


		}
		/*	else
		printf("empty\n");*/
		Sleep(10);

	}
	char result[2048];
	memset(result,0,2048);
	threadCount=pa;
	XP_API_SPRINTF(result,
		"Result:\r\nTotal Thread:%d \r\n"\
		"<-----------------------------Web------------------------------->\r\n"\
		"Login Web Failed:(%d/%d)\r\n"\
		"Login Web Success:(%d/%d)\r\n"\
		"Total   WebLogin Time:%d\r\n"\
		"Average WebLogin Time:%d\r\n"\
		"Max     WebLogin Time:%d\r\n\r\n"\
		"<----------------------------Lobby Connect---------------------->\r\n"\
		"Connect Lobby Failed:(%d/%d)                               (%d/%d)\r\n"\
		"Connect Lobby Success:(%d/%d)\r\n"\
		"Total   ConnectTime:%d\r\n"\
		"Average ConnectTime:%d\r\n"\
		"Max     ConnectTime:%d\r\n\r\n"\
		"<-------------------------Lobby Login---------------------------->\r\n"\
		"Lobby Login Failed:(%d/%d)                                  (%d/%d)\r\n"\
		"Lobby Login Success:(%d/%d)\r\n"\
		"Total   LoginLobbyTime:%d\r\n"\
		"Average LoginLobbyTime:%d\r\n"\
		"Max     LoginLobbyTime:%d\r\n\r\n"\
		"<-------------------------Lobby Create---------------------------->\r\n"\
		"Lobby Create Failed:(%d/%d) \r\n"\
		"Lobby Create Success:(%d/%d) \r\n"\
		"Total   CreateLobbyTime:%d\r\n"\
		"Average CreateLobbyTime:%d\r\n"\
		"Max     CreateLobbyTime:%d\r\n\r\n"\
		"<-------------------------Lobby Filter---------------------------->\r\n"\
		"Lobby Filter Failed:(%d/%d) \r\n"\
		"Lobby Filter Success:(%d/%d) \r\n"\
		"Total   FilterLobbyTime:%d\r\n"\
		"Average FilternLobbyTime:%d\r\n"\
		"Max     FilterLobbyTime:%d\r\n\r\n"\
		"<-------------------------Lobby Join---------------------------->\r\n"\
		"Lobby Join Failed:(%d/%d) \r\n"\
		"Lobby Join Success:(%d/%d) \r\n"\
		"Total   JoinLobbyTime:%d\r\n"\
		"Average JoinnLobbyTime:%d\r\n"\
		"Max     JoinLobbyTime:%d\r\n\r\n\r\n",
		threadCount,
		failWebLoginCount,
		threadCount,
		threadCount-failWebLoginCount,
		threadCount,
		totalWebTime,
		totalWebTime/threadCount,
		maxWebTime,
		failLobbyConnect-failWebLoginCount,
		threadCount-failWebLoginCount,
		failLobbyConnect,
		threadCount,
		threadCount-failLobbyConnect,
		threadCount-failWebLoginCount,
		totalConLobbyTime,
		totalConLobbyTime/((threadCount-failLobbyConnect)==0?1:(threadCount-failLobbyConnect)),
		maxConLobbyTime,
		failLobbyLogin-failLobbyConnect,
		threadCount-failLobbyConnect,
		failLobbyLogin,
		threadCount ,
		threadCount-failLobbyLogin,
		threadCount-failLobbyConnect, 
		totalLogLobbyTime,
		totalLogLobbyTime/((threadCount-failLobbyLogin)==0?1:(threadCount-failLobbyLogin)),
		maxLogLobbyTime,
		failLobbyCreate,
		totalLobbyCreateThread,
		totalLobbyCreateThread-failLobbyCreate,
		totalLobbyCreateThread,
		totalCreateLobbyTime,
		totalCreateLobbyTime/((totalLobbyCreateThread-failLobbyCreate)==0?1:(totalLobbyCreateThread-failLobbyCreate)) ,
		maxCreateLobbyTime,
		failLobbyFilter,
		totalLobbyFilterThread,
		totalLobbyFilterThread-failLobbyFilter,
		totalLobbyFilterThread,
		totalFilterLobbyTime,
		totalFilterLobbyTime/((totalLobbyFilterThread-failLobbyFilter)==0?1:(totalLobbyFilterThread-failLobbyFilter)) ,
		maxFilterLobbyTime,
		failLobbyJoin,
		totalLobbyJoinThread,
		totalLobbyJoinThread-failLobbyJoin,
		totalLobbyJoinThread,
		totalJoinLobbyTime,
		totalJoinLobbyTime/((totalLobbyJoinThread-failLobbyJoin)==0?1:(totalLobbyJoinThread-failLobbyJoin)) ,
		maxJoinLobbyTime
		);
	XP_API_FILE_WRITE(result,	1, strlen(result),pFile);	
	XP_API_FILE_CLOSE(pFile);

	return 1;
}

StressThread::StressThread()
{

}

void StressThread::start(int threadCount)
{
	DWORD id=0;
	pa=threadCount;
	t=chBEGINTHREADEX(NULL,0,run0,(PVOID)this,0,&id);

	/*StressLog* a=new StressLog;
	a->setId(0);
	a->setData("Begin Log");
	queue.push(a);*/
}
#endif