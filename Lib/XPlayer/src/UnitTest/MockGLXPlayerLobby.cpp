#include "MockGLXPlayerLobby.h"
#include "memory.h"

MockGLXPlayerLobby::MockGLXPlayerLobby(GLXPlayerLogin* pGLXPlayerLogin)
{
	m_GLXPlayerLogin = pGLXPlayerLogin;
	
	m_TestGetLobbyServer = XP_API_NEW TestGetLobbyServer(m_GLXPlayerLogin);		
	m_TestGameInvitationGetLobby = XP_API_NEW TestGameInvitationGetLobby(m_GLXPlayerLogin);	
	
	m_testList = XP_API_NEW TestList();		
	
	m_testList->Add(m_TestGetLobbyServer);	
	m_testList->Add(m_TestGameInvitationGetLobby);
	
	m_testReporter = XP_API_NEW TestReporterStdout();
	m_testRunner = XP_API_NEW TestRunner(*m_testReporter);	
	m_curTest = m_testList->GetHead();
}

MockGLXPlayerLobby::~MockGLXPlayerLobby()
{			
	XP_API_DEL(m_TestGetLobbyServer);		
	XP_API_DEL(m_TestGameInvitationGetLobby);		
	
	XP_API_DEL(m_testList);
	XP_API_DEL(m_testRunner);
	XP_API_DEL(m_testReporter);
	XP_API_DEL(m_curTest);
}

void MockGLXPlayerLobby::runAllTest()
{		
	if(m_curTest != NULL)
	{
		m_testRunner->RunTest(m_testRunner->getTestResult(), m_curTest, 0);
		((TestImpl*)m_curTest)->runKeepAliveProcess();
		m_curTest = m_curTest->next;
	}
	else
	{
		m_testRunner->Finish();
	}		
}

void MockGLXPlayerLobby::registerResult(TestResults *result)
{
	m_testRunner->setResult(result);
}

