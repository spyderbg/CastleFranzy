#include "MockGLXPlayerUserFriend.h"
#include "memory.h"

MockGLXPlayerUserFriend::MockGLXPlayerUserFriend(GLXPlayerLogin* pGLXPlayerLogin)
{
	m_GLXPlayerLogin = pGLXPlayerLogin;		
	m_testAddUserFriend = XP_API_NEW TestAddUserFriend(m_GLXPlayerLogin);	
	m_testConfirmUserFriend = XP_API_NEW TestConfirmUserFriend(m_GLXPlayerLogin);	
	m_testDelUserFriend = XP_API_NEW TestDelUserFriend(m_GLXPlayerLogin);	
	m_testGetUserFriendsCount = XP_API_NEW TestGetUserFriendsCount(m_GLXPlayerLogin);	
	m_testGetUserFriends = XP_API_NEW TestGetUserFriends(m_GLXPlayerLogin);	

	m_testList = XP_API_NEW TestList();
	m_testList->Add(m_testAddUserFriend);
	m_testList->Add(m_testConfirmUserFriend);
	m_testList->Add(m_testDelUserFriend);
	m_testList->Add(m_testGetUserFriendsCount);
	m_testList->Add(m_testGetUserFriends);
	
	m_testReporter = XP_API_NEW TestReporterStdout();
	m_testRunner = XP_API_NEW TestRunner(*m_testReporter);	
	m_curTest = m_testList->GetHead();
}

MockGLXPlayerUserFriend::~MockGLXPlayerUserFriend()
{		
	XP_API_DEL(m_testAddUserFriend);
	XP_API_DEL(m_testConfirmUserFriend);
	XP_API_DEL(m_testDelUserFriend);
	XP_API_DEL(m_testGetUserFriendsCount);
	XP_API_DEL(m_testGetUserFriends);
	
	XP_API_DEL(m_testList);
	XP_API_DEL(m_testRunner);
	XP_API_DEL(m_testReporter);		

	XP_API_DEL(m_curTest);
}

void MockGLXPlayerUserFriend::runAllTest()
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

void MockGLXPlayerUserFriend::registerResult(TestResults *result)
{
	m_testRunner->setResult(result);
}

