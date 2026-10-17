#pragma once
#include "../../include/Login/GLXPlayerLogin.h"
#include "../../include/Lobby/GLXPlayerLobby.h"
#include "TestImpl.h"
#include "TestReporterStdout.h"
#include "Stream.h"

class MockGLXPlayerLobby	
{
public:
	MockGLXPlayerLobby(GLXPlayerLogin* pGLXPlayerLogin);
	virtual ~MockGLXPlayerLobby(void);
	
	void runAllTest();		
	void registerResult(TestResults *result);	
	
	Test* m_TestGetLobbyServer;			
	Test* m_TestGameInvitationGetLobby;	

	TestList *m_testList;	
	TestReporter *m_testReporter;
	TestRunner *m_testRunner;	

	GLXPlayerLogin * m_GLXPlayerLogin;
	Test* m_curTest;	
};


class TestGLXPlayerLobby : public TestImpl
{
public:
	TestGLXPlayerLobby(GLXPlayerLogin* pGLXPlayerLogin, char const* testName, char const* suiteName = "TestGLXPlayerLobby"):TestImpl(pGLXPlayerLogin, testName, suiteName)
	{		
		m_GLXPlayerLobby = XP_API_NEW GLXPlayerLobby();
		m_GLXPlayerLobby->RegisterObserver(this);		
	}
	
	virtual ~TestGLXPlayerLobby()
	{
		XP_API_DEL(m_GLXPlayerLobby);		
	}
	
	virtual void RunImpl(){};
	
public:
	GLXPlayerLobby* m_GLXPlayerLobby;	
};


class TestGetLobbyServer : public TestGLXPlayerLobby
{
public:
	TestGetLobbyServer(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerLobby(pGLXPlayerLogin, "TestGetLobbyServer"){}

	virtual ~TestGetLobbyServer(){}

	void RunImpl()
	{		
		runLoginProcess(USER_NAME, TEST_PASSWORD);
		m_GLXPlayerLobby->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerLobby->sendGetLobbyServer();
		while(!m_testFinished)
		{
			m_GLXPlayerLobby->Update();
		}			
	}	
};

class TestGameInvitationGetLobby : public TestGLXPlayerLobby
{
public:
	TestGameInvitationGetLobby(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerLobby(pGLXPlayerLogin, "TestGameInvitationGetLobby"){}

	virtual ~TestGameInvitationGetLobby(){}

	void RunImpl()
	{				
		m_GLXPlayerLobby->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerLobby->sendGameInvitationGetLobby(USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerLobby->Update();
		}	
		runLogoutProcess();
	}	
};
