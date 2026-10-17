#pragma once
#include "../../include/Login/GLXPlayerLogin.h"
#include "../../include/Buddy/GLXPlayerUserFriend.h"
#include "TestImpl.h"
#include "TestReporterStdout.h"
#include "Stream.h"

class MockGLXPlayerUserFriend	
{
public:
	MockGLXPlayerUserFriend(GLXPlayerLogin* pGLXPlayerLogin);
	virtual ~MockGLXPlayerUserFriend(void);
	
	void runAllTest();		
	void registerResult(TestResults *result);		
		
	Test* m_testAddUserFriend;
	Test* m_testConfirmUserFriend;
	Test* m_testDelUserFriend;
	Test* m_testGetUserFriendsCount;
	Test* m_testGetUserFriends;

	TestList *m_testList;	
	TestReporter *m_testReporter;
	TestRunner *m_testRunner;	

	GLXPlayerLogin * m_GLXPlayerLogin;
	Test* m_curTest;	
};

class TestGLXPlayerUserFriend : public TestImpl
{
public:
	TestGLXPlayerUserFriend(GLXPlayerLogin* pGLXPlayerLogin, char const* testName, char const* suiteName):TestImpl(pGLXPlayerLogin, testName, suiteName)
	{		
		m_GLXPlayerUserFriend = XP_API_NEW GLXPlayerUserFriend();
		m_GLXPlayerUserFriend->RegisterObserver(this);				
	}
	
	virtual ~TestGLXPlayerUserFriend()
	{
		XP_API_DEL(m_GLXPlayerUserFriend);		
	}
	
	virtual void RunImpl(){};
	
public:
	GLXPlayerUserFriend* m_GLXPlayerUserFriend;	
};


class TestAddUserFriend : public TestGLXPlayerUserFriend
{
public:
	TestAddUserFriend(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUserFriend(pGLXPlayerLogin, "TestAddUserFriend", "TestGLXPlayerUserFriend"){}

	virtual ~TestAddUserFriend(){}

	void RunImpl()
	{	
		
		runLoginProcess(TEST_USER_NAME, TEST_PASSWORD);
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerUserFriend->SetUID(m_GLXPlayerLogin->GetUID());
		}
					
	#ifdef USE_USER_NUMBER
		m_GLXPlayerUserFriend->sendAddUserFriend(TEST_OTHER_USER_NAME, "Hello, Add me as a friend!", true);
	#else
		m_GLXPlayerUserFriend->sendAddUserFriend(TEST_OTHER_USER_NAME, "Hello, Add me as a friend!");
	#endif
		while(!m_testFinished)
		{
			m_GLXPlayerUserFriend->Update();
		}	
		runLogoutProcess();		
	}		
};

class TestConfirmUserFriend : public TestGLXPlayerUserFriend
{
public:
	TestConfirmUserFriend(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUserFriend(pGLXPlayerLogin, "TestConfirmUserFriend", "TestGLXPlayerUserFriend"){}

	virtual ~TestConfirmUserFriend(){}

	void RunImpl()
	{	
		
		runLoginProcess(TEST_OTHER_USER, TEST_OTHER_USER_PASSWORD);
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerUserFriend->SetUID(m_GLXPlayerLogin->GetUID());
		}	
	#ifdef USE_USER_NUMBER
		m_GLXPlayerUserFriend->sendConfirmUserFriend(TEST_USER_NUMBER, true);
	#else
		m_GLXPlayerUserFriend->sendConfirmUserFriend(TEST_USER_NAME, true);
	#endif
		while(!m_testFinished)
		{
			m_GLXPlayerUserFriend->Update();
		}	
		runLogoutProcess();		
	}		
};

class TestDelUserFriend : public TestGLXPlayerUserFriend
{
public:
	TestDelUserFriend(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUserFriend(pGLXPlayerLogin, "TestDelUserFriend", "TestGLXPlayerUserFriend"){}

	virtual ~TestDelUserFriend(){}

	void RunImpl()
	{	
		
		runLoginProcess(TEST_OTHER_USER, TEST_OTHER_USER_PASSWORD);
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerUserFriend->SetUID(m_GLXPlayerLogin->GetUID());
		}	
	#ifdef USE_USER_NUMBER
		m_GLXPlayerUserFriend->sendDelUserFriend(TEST_USER_NUMBER);
	#else
		m_GLXPlayerUserFriend->sendDelUserFriend(TEST_USER_NAME);
	#endif
		while(!m_testFinished)
		{
			m_GLXPlayerUserFriend->Update();
		}	
		runLogoutProcess();		
	}		
};

class TestGetUserFriendsCount : public TestGLXPlayerUserFriend
{
public:
	TestGetUserFriendsCount(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUserFriend(pGLXPlayerLogin, "TestGetUserFriendsCount", "TestGLXPlayerUserFriend"){}

	virtual ~TestGetUserFriendsCount(){}

	void RunImpl()
	{	
		
		runLoginProcess(TEST_USER_NAME, TEST_PASSWORD);
		
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerUserFriend->SetUID(m_GLXPlayerLogin->GetUID());
		}
		
		m_GLXPlayerUserFriend->sendGetUserFriendsCount();
	
		while(!m_testFinished)
		{
			m_GLXPlayerUserFriend->Update();
		}	
		runLogoutProcess();		
	}		
};

class TestGetUserFriends : public TestGLXPlayerUserFriend
{
public:
	TestGetUserFriends(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUserFriend(pGLXPlayerLogin, "TestGetUserFriends", "TestGLXPlayerUserFriend"){}

	virtual ~TestGetUserFriends(){}

	void RunImpl()
	{	
		
		runLoginProcess(TEST_USER_NAME, TEST_PASSWORD);
		
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerUserFriend->SetUID(m_GLXPlayerLogin->GetUID());
		}
		
		m_GLXPlayerUserFriend->sendGetUserFriends();
	
		while(!m_testFinished)
		{
			m_GLXPlayerUserFriend->Update();
		}	
		runLogoutProcess();		
	}		
};
