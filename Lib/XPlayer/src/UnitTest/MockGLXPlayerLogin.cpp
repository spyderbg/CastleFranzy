#include "MockGLXPlayerLogin.h"
#include "memory.h"

MockGLXPlayerLogin::MockGLXPlayerLogin(GLXPlayerLogin * pGLXPlayerLogin)
{	
	m_GLXPlayerLogin = pGLXPlayerLogin;	

	m_testLogin = XP_API_NEW TestLogin(m_GLXPlayerLogin);	
	m_testLogout = XP_API_NEW TestLogout(m_GLXPlayerLogin);
	m_testKeepAlive = XP_API_NEW TestKeepAlive(m_GLXPlayerLogin);
	m_testGetAPNSInfo = XP_API_NEW TestGetAPNSInfo(m_GLXPlayerLogin);
	m_testSendGetProductInfo = XP_API_NEW TestSendGetProductInfo(m_GLXPlayerLogin);
	m_testSendGetGeneralConditions = XP_API_NEW TestSendGetGeneralConditions(m_GLXPlayerLogin);

	m_testList = XP_API_NEW TestList();
	m_testList->Add(m_testLogin);	
	m_testList->Add(m_testGetAPNSInfo);
	m_testList->Add(m_testSendGetProductInfo);
	m_testList->Add(m_testSendGetGeneralConditions);
	m_testList->Add(m_testKeepAlive);
	m_testList->Add(m_testLogout);
	
	m_testReporter = XP_API_NEW TestReporterStdout();
	m_testRunner = XP_API_NEW TestRunner(*m_testReporter);		
	m_curTest = m_testList->GetHead();
}

MockGLXPlayerLogin::~MockGLXPlayerLogin()
{
	XP_API_DEL(m_testLogout);
	XP_API_DEL(m_testLogin);
	XP_API_DEL(m_testKeepAlive);
	XP_API_DEL(m_testGetAPNSInfo);
	XP_API_DEL(m_testSendGetProductInfo);
	XP_API_DEL(m_testSendGetGeneralConditions);
	
	XP_API_DEL(m_testList);

	XP_API_DEL(m_testRunner);
	XP_API_DEL(m_testReporter);			
	
	XP_API_DEL(m_curTest);	
}

void MockGLXPlayerLogin::runAllTest()
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

void MockGLXPlayerLogin::registerResult(TestResults *result)
{	
	m_testRunner->setResult(result);
}