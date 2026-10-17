#include "MockGLXPlayerLeaderboard.h"
#include "memory.h"

MockGLXPlayerLeaderboard::MockGLXPlayerLeaderboard(GLXPlayerLogin* pGLXPlayerLogin)
{
	m_GLXPlayerLogin = pGLXPlayerLogin;
	
	m_testSendHighScore = XP_API_NEW TestSendHighScore(m_GLXPlayerLogin);
	m_testSendMultipleHighScore = XP_API_NEW TestSendMultipleHighScore(m_GLXPlayerLogin);
	m_testSendHighScoreWithSupplementalData = XP_API_NEW TestSendHighScoreWithSupplementalData(m_GLXPlayerLogin);
	m_testSendRankGet = XP_API_NEW TestSendRankGet(m_GLXPlayerLogin);
	m_testSendRankGetAroundPlayer = XP_API_NEW TestSendRankGetAroundPlayer(m_GLXPlayerLogin);
	
	m_testList = XP_API_NEW TestList();
	m_testList->Add(m_testSendHighScore);
	m_testList->Add(m_testSendMultipleHighScore);
	m_testList->Add(m_testSendHighScoreWithSupplementalData);
	m_testList->Add(m_testSendRankGet);
	m_testList->Add(m_testSendRankGetAroundPlayer);
	
	m_testReporter = XP_API_NEW TestReporterStdout();
	m_testRunner = XP_API_NEW TestRunner(*m_testReporter);	
	m_curTest = m_testList->GetHead();
}

MockGLXPlayerLeaderboard::~MockGLXPlayerLeaderboard()
{
	XP_API_DEL(m_testSendHighScore);
	XP_API_DEL(m_testSendMultipleHighScore);
	XP_API_DEL(m_testSendHighScoreWithSupplementalData);
	XP_API_DEL(m_testSendRankGet);
	XP_API_DEL(m_testSendRankGetAroundPlayer);
	
	XP_API_DEL(m_testList);
	XP_API_DEL(m_testRunner);
	XP_API_DEL(m_testReporter);		

	XP_API_DEL(m_curTest);
}

void MockGLXPlayerLeaderboard::runAllTest()
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

void MockGLXPlayerLeaderboard::registerResult(TestResults *result)
{
	m_testRunner->setResult(result);
}
