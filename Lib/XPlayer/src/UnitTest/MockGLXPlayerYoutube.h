#pragma once
#include "../../include/Login/GLXPlayerLogin.h"
#include "../../include/YouTube/GLXPlayerYoutube.h"
#include "TestImpl.h"
#include "TestReporterStdout.h"
#include "Stream.h"

class MockGLXPlayerYoutube	
{
public:
	MockGLXPlayerYoutube(GLXPlayerLogin* pGLXPlayerLogin);
	virtual ~MockGLXPlayerYoutube(void);
	
	void runAllTest();		
	void registerResult(TestResults *result);	
	
	//Test GLXPlayerYoutube functions
	Test* m_testCheckReplayVideoAvailable;	
	Test* m_testAddVideoInfo;	
	Test* m_testAddVideoData;	

	TestList *m_testList;	
	TestReporter *m_testReporter;
	TestRunner *m_testRunner;	

	GLXPlayerLogin * m_GLXPlayerLogin;
	Test* m_curTest;	
};


class TestGLXPlayerYoutube : public TestImpl
{
public:
	TestGLXPlayerYoutube(GLXPlayerLogin* pGLXPlayerLogin, char const* testName, char const* suiteName = "TestGLXPlayerYoutube"):TestImpl(pGLXPlayerLogin, testName, suiteName)
	{		
		m_GLXPlayerYoutube = XP_API_NEW GLXPlayerYoutube();
		m_GLXPlayerYoutube->RegisterObserver(this);		
	}
	
	virtual ~TestGLXPlayerYoutube()
	{
		XP_API_DEL(m_GLXPlayerYoutube);		
	}
	
	virtual void RunImpl(){};
	
public:
	GLXPlayerYoutube* m_GLXPlayerYoutube;	
};

class TestCheckReplayVideoAvailable : public TestGLXPlayerYoutube
{
public:
	TestCheckReplayVideoAvailable(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerYoutube(pGLXPlayerLogin, "TestCheckReplayVideoAvailable"){}

	virtual ~TestCheckReplayVideoAvailable(){}

	void RunImpl()
	{		
		runLoginProcess(USER_NAME, TEST_PASSWORD);
		m_GLXPlayerYoutube->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerYoutube->sendCheckReplayVideoAvailable();
		while(!m_testFinished)
		{
			m_GLXPlayerYoutube->Update();
		}	
	}	
};

class TestAddVideoInfo : public TestGLXPlayerYoutube
{
public:
	TestAddVideoInfo(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerYoutube(pGLXPlayerLogin, "TestAddVideoInfo"){}

	virtual ~TestAddVideoInfo(){}

	void RunImpl()
	{		
		m_GLXPlayerYoutube->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerYoutube->sendAddVideoInfo(TEST_YOUTUBE_NAME, TEST_YOUTUBE_PASSWORD, TEST_VIDEO_TITLE, TEST_VIDEO_COMMENTS);
		while(!m_testFinished)
		{
			m_GLXPlayerYoutube->Update();
		}
		XP_API_MEMSET(TEST_VIDEO_ID, 0, 32);
		XP_API_SPRINTF(TEST_VIDEO_ID, m_GLXPlayerYoutube->getVideoID() );
	}	
};

class TestAddVideoData : public TestGLXPlayerYoutube
{
public:
	TestAddVideoData(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerYoutube(pGLXPlayerLogin, "TestAddVideoData"){}

	virtual ~TestAddVideoData(){}

	void RunImpl()
	{			
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			CResourceStream *stream = XP_API_NEW CResourceStream("test.avi");
			if(stream != NULL)
			{
				BYTE* pData = (BYTE*)(stream->GetBuffer());
				int size = stream->GetLength();			

				m_GLXPlayerYoutube->SetUID(m_GLXPlayerLogin->GetUID());
				m_GLXPlayerYoutube->sendAddVideoData((char *)pData, size);
				while(!m_testFinished)
				{
					m_GLXPlayerYoutube->Update();
				}			

				XP_API_DEL(stream);							
			}
			runLogoutProcess();
		}
	}	
};
