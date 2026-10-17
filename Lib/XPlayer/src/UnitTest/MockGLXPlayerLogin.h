#ifndef __MOCKGL_XPLAYER_LOGIN_H__
#define __MOCKGL_XPLAYER_LOGIN_H__

#include "../../include/Login/GLXPlayerLogin.h"
#include "TestImpl.h"
#include "TestReporterStdout.h"


using namespace UnitTest;
/**
* Class MockGLXPlayerLogin. 
* Class MockGLXPlayerLogin is designed for testing login, logout and keep the connection to web server. 
*
* @author  World Online Team <<online-adm@gameloft.com>>
* @author  Tran Thai Phi <<phi.tranthai@gameloft.com>>
* @version 1.0.0 
*  
*/

class MockGLXPlayerLogin
{
public:
	
	MockGLXPlayerLogin(GLXPlayerLogin * pGLXPlayerLogin);	
	virtual ~MockGLXPlayerLogin();	
	void runAllTest();		

	void registerResult(TestResults *result);

	Test * m_testLogin;
	Test * m_testLogout;
	Test * m_testKeepAlive;
	Test * m_testGetAPNSInfo;
	Test * m_testSendGetProductInfo;
	Test * m_testSendGetGeneralConditions;

	TestList *m_testList;	
	TestReporter *m_testReporter;
	TestRunner *m_testRunner;	
	
	GLXPlayerLogin * m_GLXPlayerLogin;	
	Test* m_curTest;
	
};

class TestLogin : public TestImpl
{
public:
	TestLogin(GLXPlayerLogin *p, char const* testName = "TestLogin", char const* suiteName = "MockGLXPlayerLogin"):TestImpl(p, testName, suiteName)	{}

	virtual ~TestLogin(){}		

	void RunImpl()
	{			
		runLoginProcess(TEST_USER_NAME, TEST_PASSWORD,TEST_LOGIN_TYPE, TEST_DEV_TOKEN,TEST_RETURN_USER_NUMBER, TEST_RETURN_USER_NAME);
	}		

public:			
};

class TestLogout : public TestImpl
{
public:
	TestLogout(GLXPlayerLogin *p, char const* testName = "TestLogout", char const* suiteName = "MockGLXPlayerLogin"):TestImpl(p, testName, suiteName)
	{		
	}

	virtual ~TestLogout(){}

	void RunImpl()
	{		
		runLogoutProcess();
	}	
};


class TestKeepAlive : public TestImpl
{
public:
	TestKeepAlive(GLXPlayerLogin *p, char const* testName = "TestKeepAlive", char const* suiteName = "MockGLXPlayerLogin"):TestImpl(p, testName, suiteName)
	{		
	}

	virtual ~TestKeepAlive(){}

	void RunImpl()
	{	
		if(m_GLXPlayerLogin->IsLoggedIn())
		{	
			runKeepAliveProcess();			
		}
		else
		{
			OnRequestFailure( FUNCTION_KEEP_ALIVE, ERROR_INIT);
		}
	}	
};


class TestGetAPNSInfo : public TestImpl
{
public:
	TestGetAPNSInfo(GLXPlayerLogin *p, char const* testName = "TestGetAPNSInfo", char const* suiteName = "MockGLXPlayerLogin"):TestImpl(p, testName, suiteName)
	{		
	}

	virtual ~TestGetAPNSInfo(){}

	void RunImpl()
	{	
		if(m_GLXPlayerLogin->IsLoggedIn())
		{	
			m_GLXPlayerLogin->SendGetAPNSInfo();
			for(int i = 0; i < 1000; i++)
					for(int j = 0; j < 1000; j++)
						m_GLXPlayerLogin->Update();				
		}
		else
		{
			OnRequestFailure( FUNCTION_APPLE_PUSH_NOTIFICATION_SERVER, ERROR_INIT);
		}
	}	
};

class TestSendGetProductInfo : public TestImpl
{
public:
	TestSendGetProductInfo(GLXPlayerLogin *p, char const* testName = "TestSendGetProductInfo", char const* suiteName = "MockGLXPlayerLogin"):TestImpl(p, testName, suiteName)
	{		
	}

	virtual ~TestSendGetProductInfo(){}

	void RunImpl()
	{	
		if(m_GLXPlayerLogin->IsLoggedIn())
		{	
			m_GLXPlayerLogin->SendGetProductInfo(TEST_PRODUCT_LIST);
			for(int i = 0; i < 1000; i++)
					for(int j = 0; j < 1000; j++)
						m_GLXPlayerLogin->Update();				
		}
		else
		{
			OnRequestFailure( FUNCTION_GET_PRODUCT, ERROR_INIT);
		}
	}	
};

class TestSendGetGeneralConditions : public TestImpl
{
public:
	TestSendGetGeneralConditions(GLXPlayerLogin *p, char const* testName = "TestSendGetGeneralConditions", char const* suiteName = "MockGLXPlayerLogin"):TestImpl(p, testName, suiteName)
	{		
	}

	virtual ~TestSendGetGeneralConditions(){}

	void RunImpl()
	{	
		if(m_GLXPlayerLogin->IsLoggedIn())
		{	
			m_GLXPlayerLogin->SendGetGeneralConditions(TEST_PRODUCT_LANGUAGE);
			for(int i = 0; i < 1000; i++)
					for(int j = 0; j < 1000; j++)
						m_GLXPlayerLogin->Update();				
		}
		else
		{
			OnRequestFailure( FUNCTION_GET_GENERAL_CONDITIONS, ERROR_INIT);
		}
	}	
};
#endif // __MOCKGL_XPLAYER_MESSAGE_H__
