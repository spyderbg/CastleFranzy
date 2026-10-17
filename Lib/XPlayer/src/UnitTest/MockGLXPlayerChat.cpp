#include "MockGLXPlayerChat.h"
#include "memory.h"

MockGLXPlayerChat::MockGLXPlayerChat(GLXPlayerLogin * pGLXPlayerLogin)
{	
	m_GLXPlayerLogin = pGLXPlayerLogin;	

	m_testSendGetChatWarning = XP_API_NEW TestSendGetChatWarning(m_GLXPlayerLogin);		

	m_testList = XP_API_NEW TestList();
	m_testList->Add(m_testSendGetChatWarning);		
	
	m_testReporter = XP_API_NEW TestReporterStdout();
	m_testRunner = XP_API_NEW TestRunner(*m_testReporter);		
	m_curTest = m_testList->GetHead();
}

MockGLXPlayerChat::~MockGLXPlayerChat()
{
	XP_API_DEL(m_testSendGetChatWarning);	
	
	XP_API_DEL(m_testList);

	XP_API_DEL(m_testRunner);
	XP_API_DEL(m_testReporter);			
	
	XP_API_DEL(m_curTest);	
}

void MockGLXPlayerChat::runAllTest()
{		
	if(m_curTest != NULL)
	{
		m_testRunner->RunTest(m_testRunner->getTestResult(), m_curTest, 0);
		m_curTest = m_curTest->next;
	}
	else
	{
		m_testRunner->Finish();
	}		
}

void MockGLXPlayerChat::registerResult(TestResults *result)
{	
	m_testRunner->setResult(result);
}