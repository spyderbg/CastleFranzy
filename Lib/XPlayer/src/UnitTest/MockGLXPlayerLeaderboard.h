#include "../../include/Login/GLXPlayerLogin.h"
#include "../../include/Leaderboard/GLXPlayerLeaderboard.h"
#include "TestImpl.h"
#include "TestReporterStdout.h"

class MockGLXPlayerLeaderboard
{
public:
	MockGLXPlayerLeaderboard(GLXPlayerLogin* pGLXPlayerLogin);
	virtual ~MockGLXPlayerLeaderboard(void);
	
	void runAllTest();
	void registerResult(TestResults *result);
		
	Test* m_testSendHighScore;
	Test* m_testSendMultipleHighScore;
	Test* m_testSendHighScoreWithSupplementalData;
	Test* m_testSendRankGet;
	Test* m_testSendRankGetAroundPlayer;

	TestList *m_testList;	
	TestReporter *m_testReporter;
	TestRunner *m_testRunner;	

	GLXPlayerLogin * m_GLXPlayerLogin;
	Test* m_curTest;
};

class TestGLXPlayerLeaderboard : public TestImpl
{
public:
	TestGLXPlayerLeaderboard(GLXPlayerLogin* pGLXPlayerLogin, char const* testName):TestImpl(pGLXPlayerLogin, testName, "TestGLXPlayerLeaderboard")
	{		
		m_GLXPlayerLeaderboard = XP_API_NEW GLXPlayerLeaderboard();
		m_GLXPlayerLeaderboard->RegisterObserver(this);				
	}
	
	virtual ~TestGLXPlayerLeaderboard()
	{
		XP_API_DEL(m_GLXPlayerLeaderboard);		
	}
	
	virtual void RunImpl(){};
	
public:
	GLXPlayerLeaderboard* m_GLXPlayerLeaderboard;	
};

class TestSendHighScore : public TestGLXPlayerLeaderboard
{
public:
	TestSendHighScore(GLXPlayerLogin* pGLXPlayerLogin): TestGLXPlayerLeaderboard(pGLXPlayerLogin, "TestSendHighScore")
	{
	}
	
	virtual ~TestSendHighScore()
	{		
	}
	
	void RunImpl()
	{	
		runLoginProcess(TEST_USER_NAME, TEST_PASSWORD);
		
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerLeaderboard->SetUID(m_GLXPlayerLogin->GetUID());
			m_GLXPlayerLeaderboard->sendHighScore(TEST_SCORE_VALUE, TEST_SCORE_LEVEL, TEST_SCORE_TYPE);
		
			while(!m_testFinished)
			{
				m_GLXPlayerLeaderboard->Update();
			}
		}
		else		
		{
			OnRequestFailure( FUNCTION_UPLOAD_SCORE, ERROR_INIT);
		}
	}
};

class TestSendMultipleHighScore : public TestGLXPlayerLeaderboard
{
public:
	TestSendMultipleHighScore(GLXPlayerLogin* pGLXPlayerLogin): TestGLXPlayerLeaderboard(pGLXPlayerLogin, "TestSendMultipleHighScore")
	{
		m_scoreArray = NULL;
	}
	
	virtual ~TestSendMultipleHighScore()
	{		
		XP_API_DEL(m_scoreArray);
	}
	
	void RunImpl()
	{			
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerLeaderboard->SetUID(m_GLXPlayerLogin->GetUID());
			
			m_scoreArray = XP_API_NEW char[1024];
			XP_API_MEMSET( m_scoreArray, 0, 1024 );
			
			m_GLXPlayerLeaderboard->addScoreEntry(m_scoreArray, TEST_SCORE_VALUE, TEST_SCORE_LEVEL, TEST_SCORE_TYPE);
			m_GLXPlayerLeaderboard->addScoreEntry(m_scoreArray, TEST_SCORE_VALUE + 100, TEST_SCORE_LEVEL + 1, TEST_SCORE_TYPE);
			m_GLXPlayerLeaderboard->addScoreEntry(m_scoreArray, TEST_SCORE_VALUE + 200, TEST_SCORE_LEVEL + 2, TEST_SCORE_TYPE);
			
			m_GLXPlayerLeaderboard->sendHighScore(m_scoreArray);
		
			while(!m_testFinished)
			{
				m_GLXPlayerLeaderboard->Update();
			}
		}
		else
		{
			OnRequestFailure( FUNCTION_UPLOAD_SCORE, ERROR_INIT);
		}		
	}
	
	char* m_scoreArray;
};

class TestSendHighScoreWithSupplementalData : public TestGLXPlayerLeaderboard
{
public:
	TestSendHighScoreWithSupplementalData(GLXPlayerLogin* pGLXPlayerLogin): TestGLXPlayerLeaderboard(pGLXPlayerLogin, "TestSendHighScoreWithSupplementalData")
	{
		m_numberOfSupplement = 0;
		m_supplemenData = NULL;
		m_scoreArray = NULL;
	}
	
	virtual ~TestSendHighScoreWithSupplementalData()
	{		
		XP_API_DEL(m_supplemenData);
		XP_API_DEL(m_scoreArray);
	}
	
	void RunImpl()
	{			
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerLeaderboard->SetUID(m_GLXPlayerLogin->GetUID());
			
			m_scoreArray = XP_API_NEW char[2048];
			XP_API_MEMSET( m_scoreArray, 0, 2048 );
			
			m_numberOfSupplement = 5;
			m_supplemenData = XP_API_NEW int[m_numberOfSupplement];
			
			for(int i = 0; i < m_numberOfSupplement; i++)
			{
				m_supplemenData[i] = rand()%9;
			}
			
			m_GLXPlayerLeaderboard->addScoreEntry(m_scoreArray, TEST_SCORE_VALUE, TEST_SCORE_LEVEL, TEST_SCORE_TYPE, m_supplemenData, m_numberOfSupplement);
			
			m_GLXPlayerLeaderboard->sendHighScore(m_scoreArray);
		
			while(!m_testFinished)
			{
				m_GLXPlayerLeaderboard->Update();
			}
		}
		else
		{
			OnRequestFailure( FUNCTION_UPLOAD_SCORE, ERROR_INIT);
		}		
	}
	
	int m_numberOfSupplement;
	int* m_supplemenData;
	char* m_scoreArray;
};

class TestSendRankGet : public TestGLXPlayerLeaderboard
{
public:
	TestSendRankGet(GLXPlayerLogin* pGLXPlayerLogin): TestGLXPlayerLeaderboard(pGLXPlayerLogin, "TestSendRankGet")
	{
	}
	
	virtual ~TestSendRankGet()
	{		
	}
	
	void RunImpl()
	{			
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerLeaderboard->SetUID(m_GLXPlayerLogin->GetUID());
			m_GLXPlayerLeaderboard->sendRankGet(TEST_SCORE_LEVEL, TEST_NUMBER_OF_PLAYER, TEST_SCORE_TYPE);
		
			while(!m_testFinished)
			{
				m_GLXPlayerLeaderboard->Update();
			}			
		}
		else		
		{
			OnRequestFailure( FUNCTION_UPLOAD_SCORE, ERROR_INIT);
		}		
	}
};

class TestSendRankGetAroundPlayer : public TestGLXPlayerLeaderboard
{
public:
	TestSendRankGetAroundPlayer(GLXPlayerLogin* pGLXPlayerLogin): TestGLXPlayerLeaderboard(pGLXPlayerLogin, "TestSendRankGetAroundPlayer")
	{
	}
	
	virtual ~TestSendRankGetAroundPlayer()
	{		
	}
	
	void RunImpl()
	{			
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerLeaderboard->SetUID(m_GLXPlayerLogin->GetUID());
			m_GLXPlayerLeaderboard->sendRankGetAroundPlayer(TEST_SCORE_LEVEL, TEST_NUMBER_OF_PLAYER_AROUND, TEST_SCORE_TYPE);
		
			while(!m_testFinished)
			{
				m_GLXPlayerLeaderboard->Update();
			}			
			runLogoutProcess();
		}
		else		
		{
			OnRequestFailure( FUNCTION_UPLOAD_SCORE, ERROR_INIT);
		}		
	}
};
