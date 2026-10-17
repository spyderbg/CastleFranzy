#include "MockGLXPlayerYoutube.h"
#include "memory.h"

MockGLXPlayerYoutube::MockGLXPlayerYoutube(GLXPlayerLogin* pGLXPlayerLogin)
{
	m_GLXPlayerLogin = pGLXPlayerLogin;
	
	//Test GLXPlayerYoutube functions
	m_testCheckReplayVideoAvailable = XP_API_NEW TestCheckReplayVideoAvailable(m_GLXPlayerLogin);
	m_testAddVideoInfo = XP_API_NEW TestAddVideoInfo(m_GLXPlayerLogin);
	m_testAddVideoData = XP_API_NEW TestAddVideoData(m_GLXPlayerLogin);	
	m_testList = XP_API_NEW TestList();	
	
	//Test GLXPlayerYoutube functions
	m_testList->Add(m_testCheckReplayVideoAvailable);	
	m_testList->Add(m_testAddVideoInfo);	
	m_testList->Add(m_testAddVideoData);
	
	m_testReporter = XP_API_NEW TestReporterStdout();
	m_testRunner = XP_API_NEW TestRunner(*m_testReporter);	
	m_curTest = m_testList->GetHead();
}

MockGLXPlayerYoutube::~MockGLXPlayerYoutube()
{	
	//Test GLXPlayerYoutubeEX functions
	XP_API_DEL(m_testCheckReplayVideoAvailable);	
	XP_API_DEL(m_testAddVideoInfo);	
	XP_API_DEL(m_testAddVideoData);	
	
	XP_API_DEL(m_testList);
	XP_API_DEL(m_testRunner);
	XP_API_DEL(m_testReporter);
	XP_API_DEL(m_curTest);
}

void MockGLXPlayerYoutube::runAllTest()
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

void MockGLXPlayerYoutube::registerResult(TestResults *result)
{
	m_testRunner->setResult(result);
}

