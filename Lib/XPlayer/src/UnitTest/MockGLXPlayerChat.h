#ifndef __MOCKGL_XPLAYER_CHAT_H__
#define __MOCKGL_XPLAYER_CHAT_H__

#include "../../include/Chat/GLXPlayerChat.h"
#include "TestImpl.h"
#include "TestReporterStdout.h"


using namespace UnitTest;
/**
* Class MockGLXPlayerChat. 
* Class MockGLXPlayerChat is designed for testing login, logout and keep the connection to web server. 
*
* @author  World Online Team <<online-adm@gameloft.com>>
* @author  Tran Thai Phi <<phi.tranthai@gameloft.com>>
* @version 1.0.0 
*  
*/

class MockGLXPlayerChat
{
public:
	
	MockGLXPlayerChat(GLXPlayerLogin * pGLXPlayerLogin);	
	virtual ~MockGLXPlayerChat();	
	void runAllTest();		

	void registerResult(TestResults *result);

	Test * m_testSendGetChatWarning;	

	TestList *m_testList;	
	TestReporter *m_testReporter;
	TestRunner *m_testRunner;	
	
	GLXPlayerLogin * m_GLXPlayerLogin;	
	Test* m_curTest;
	
};

class TestSendGetChatWarning: public TestImpl
{
public:
	TestSendGetChatWarning(GLXPlayerLogin *p, char const* testName = "TestSendGetChatWarning", char const* suiteName = "MockGLXPlayerChat"):TestImpl(p, testName, suiteName)
	{
		m_GLXPlayerChat = XP_API_NEW GLXPlayerChat();
		m_GLXPlayerChat->RegisterObserver(this);	
	}

	virtual ~TestSendGetChatWarning()
	{
		XP_API_DEL(m_GLXPlayerChat);
	}		

	void RunImpl()
	{		
		m_testFinished = false;
		runLoginProcess(TEST_USER_NAME, TEST_PASSWORD);
		
		if(m_GLXPlayerLogin->IsLoggedIn())
		{
			m_GLXPlayerChat->SetUID(m_GLXPlayerLogin->GetUID());
			m_GLXPlayerChat->SendGetChatWarning();
		
			while(!m_testFinished)
			{
				m_GLXPlayerChat->Update();
			}
		}
		else		
		{
			OnRequestFailure( FUNCTION_GET_CHAT_WARNING, ERROR_INIT);
		}
		
		runLogoutProcess();
	}		

public:	
	GLXPlayerChat* m_GLXPlayerChat;		
};
#endif // __MOCKGL_XPLAYER_MESSAGE_H__
