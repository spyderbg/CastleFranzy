#include "stdafx.h"
#include "GLReplayObserver.h"

GLReplayObserver::GLReplayObserver(void)
{
	m_GLXPlayerYoutube = XP_API_NEW GLXPlayerYoutube();	
	m_GLXPlayerYoutube->RegisterObserver( this );

	bSend = false ;

	bCheckReplayVideoAvailable  = false ;
	
}

GLReplayObserver::~GLReplayObserver(void)
{
	XP_API_DEL(m_GLXPlayerYoutube) ;
}


void GLReplayObserver::OnNetworkError()
{
	printf("======star========\r\nGLReplayObserver::OnNetworkError\r\n=======end=========\r\n") ;

	printf(TIPS_INFO) ;
}

void GLReplayObserver::OnRequestFailure(int func_id, int error_code)
{
	printf("=======star===========\r\nGLReplayObserver::OnRequestFailure function id=%d error code %d\r\n========end========\r\n", func_id, error_code) ;

	printf(TIPS_INFO) ;
}

void GLReplayObserver::OnRequestSuccess(int func_id, char *data, int data_len)
{
	printf("========star==========\r\nGLReplayObserver::OnRequestSuccess function id=%d\r\n=======end=========\r\n", func_id) ;
	switch(func_id)
	{
		case FUNCTION_CHECK_REPLAYVIDEO_AVAILABLE:
			{

			}
		case FUNCTION_ADD_VIDEO_INFO:
			{
				char moudlePath[MAX_PATH] = {0};
				GetModuleFileNameA(NULL, (char*)&moudlePath, sizeof(moudlePath));
				size_t pos = XP_API_STRLEN(moudlePath);
				while(pos > 0)
				{
					if(moudlePath[pos--] == '\\')
					{
						break;
					}
				}
				moudlePath[pos + 2] = 0;
				XP_API_STRCAT(moudlePath, "\\data\\test.sav") ;
				
				XP_FILE_HANDLE fp = XP_API_FILE_OPEN(moudlePath, "rb+") ;
				if (fp > 0)
				{
					int fileSize = XP_API_FILE_GET_LENGTH(fp) ;
					BYTE *pData =  XP_API_NEW BYTE[fileSize] ;
					XP_API_MEMSET(pData, 0, fileSize) ;
					int readSize = XP_API_FILE_READ(pData,sizeof(BYTE), fileSize, fp) ;
					XP_API_FILE_CLOSE(fp) ;

					if (readSize == fileSize)
					{
						m_GLXPlayerYoutube->sendAddVideoData((char *)pData, readSize);
					}
					XP_API_DEL(pData) ;
				}
			}	
			break;
		case FUNCTION_UPLOAD_VIDEO_DATA:
			{
				//for test more than video
				static int sendCount = 1 ;
				if (sendCount < 1)
				{
					Sleep(1000) ;
					sendCount ++ ;
					bSend = true ;
				}
				else
				{
					sendCount = 1 ;
				}

			}
			break;
	}

	printf(TIPS_INFO) ;
}

void GLReplayObserver::OnRequestTimeout(int func_id)
{
	printf("========star==========\r\nGLReplayObserver::OnRequestTimeout function id=%d\r\n=======end=========\r\n", func_id) ;

	printf(TIPS_INFO) ;
}


void GLReplayObserver::Update()
{
	m_GLXPlayerYoutube->Update() ;

	if (!bCheckReplayVideoAvailable )
	{
		m_GLXPlayerYoutube->SetUID("0") ;
		m_GLXPlayerYoutube->sendCheckReplayVideoAvailable()  ;
		bCheckReplayVideoAvailable = true ;
	}
	

	if (bSend)
	{
		bSend = false ;
		//for test 
		char title[50] = {0} ;
		struct tm t;
		time_t sysTime;
		time(&sysTime);
		gmtime_s(&t, &sysTime) ;

		sprintf( title, "%04d-%02d-%02d %02d:%02d:%02d",1900 + t.tm_year, 1 + t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec );

		static char users[][2][100] = {
			{"gameloft.beijing@gmail.com","beijing.online"}
		} ;
		static int userIndex = 0 ;

		m_GLXPlayerYoutube->SetUID("0") ;
		if (userIndex < 0 || userIndex >= sizeof(users) / 200)
		{
			userIndex = 0 ;
		}

		
		printf("\nSending.....\n") ;

		//if use wchar ,use TranslateWChar2Utf8 
		wchar_t *src = L"test chinese comment ²âÊÔÖÐÎÄ ×¢ÊÍ" ;
		char *blob = NULL;
		wchar_t *dest = NULL;
		char *utf8_src = NULL ;
		char *utf8_dest = NULL ;
		int ret = TranslateWChar2Utf8(&utf8_src, src, wcsnlen(src,1024), 1) ;
		blob = SSEncDec_String2Blob( utf8_src );
		utf8_dest = SSEncDec_Blob2String(blob) ;
		ret = TranslateUtf82WChar(&dest, utf8_dest, strlen(utf8_dest),1) ;

		//send title and comment to server
		m_GLXPlayerYoutube->sendAddVideoInfo(users[userIndex][0], users[userIndex][1], title, utf8_src,false);	

		userIndex ++ ;	
	}
}

void GLReplayObserver::Send()
{
	bSend = true ;
}