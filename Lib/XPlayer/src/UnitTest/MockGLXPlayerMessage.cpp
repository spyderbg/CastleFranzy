#include "MockGLXPlayerMessage.h"
#include "memory.h"

MockGLXPlayerMessage::MockGLXPlayerMessage(GLXPlayerLogin* pGLXPlayerLogin)
{
	m_GLXPlayerLogin = pGLXPlayerLogin;	
	
	m_testSendOnlineMessage = XP_API_NEW TestSendOnlineMessage(m_GLXPlayerLogin);
	m_testSendGetMessageCount = XP_API_NEW TestSendGetMessageCount(m_GLXPlayerLogin);
	m_testSendGetMessageHeader = XP_API_NEW TestSendGetMessageHeader(m_GLXPlayerLogin);
	m_testSendReadMessage = XP_API_NEW TestSendReadMessage(m_GLXPlayerLogin);
	m_testSendDeleteMessage = XP_API_NEW TestSendDeleteMessage(m_GLXPlayerLogin);	
	m_testSendGetMySentMessageCount = XP_API_NEW TestSendGetMySentMessageCount(m_GLXPlayerLogin);
	m_testSendGetMySentMessageHeader = XP_API_NEW TestSendGetMySentMessageHeader(m_GLXPlayerLogin);
	m_testSendReadMySentMessage = XP_API_NEW TestSendReadMySentMessage(m_GLXPlayerLogin);
	m_testSendDelMySentMessage = XP_API_NEW TestSendDelMySentMessage(m_GLXPlayerLogin);
	
	m_testList = XP_API_NEW TestList();	
	
	m_testList->Add(m_testSendOnlineMessage);
	m_testList->Add(m_testSendGetMessageCount);
	m_testList->Add(m_testSendGetMessageHeader);
	m_testList->Add(m_testSendReadMessage);
	m_testList->Add(m_testSendDeleteMessage);	
	m_testList->Add(m_testSendGetMySentMessageCount);
	m_testList->Add(m_testSendGetMySentMessageHeader);
	m_testList->Add(m_testSendReadMySentMessage);
	m_testList->Add(m_testSendDelMySentMessage);
	
	m_testReporter = XP_API_NEW TestReporterStdout();
	m_testRunner = XP_API_NEW TestRunner(*m_testReporter);	
	m_curTest = m_testList->GetHead();
}

MockGLXPlayerMessage::~MockGLXPlayerMessage()
{	
	XP_API_DEL(m_testSendOnlineMessage);
	XP_API_DEL(m_testSendGetMessageCount);
	XP_API_DEL(m_testSendGetMessageHeader);
	XP_API_DEL(m_testSendReadMessage);
	XP_API_DEL(m_testSendDeleteMessage);	
	XP_API_DEL(m_testSendGetMySentMessageCount);
	XP_API_DEL(m_testSendGetMySentMessageHeader);
	XP_API_DEL(m_testSendReadMySentMessage);
	XP_API_DEL(m_testSendDelMySentMessage);
	
	XP_API_DEL(m_testList);
	XP_API_DEL(m_testRunner);
	XP_API_DEL(m_testReporter);		

	XP_API_DEL(m_curTest);
}

void MockGLXPlayerMessage::runAllTest()
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

void MockGLXPlayerMessage::registerResult(TestResults *result)
{
	m_testRunner->setResult(result);
}
