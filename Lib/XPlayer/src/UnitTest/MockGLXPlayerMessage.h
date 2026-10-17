#include "../../include/Login/GLXPlayerLogin.h"
#include "../../include/Message/GLXPlayerMessage.h"
#include "TestImpl.h"
#include "TestReporterStdout.h"

class MockGLXPlayerMessage
{
public:
	MockGLXPlayerMessage(GLXPlayerLogin* pGLXPlayerLogin);
	virtual ~MockGLXPlayerMessage(void);
	
	void runAllTest();
	void registerResult(TestResults *result);		
	
	Test* m_testSendOnlineMessage;
	Test* m_testSendGetMessageCount;
	Test* m_testSendGetMessageHeader;
	Test* m_testSendReadMessage;
	Test* m_testSendDeleteMessage;	
	Test* m_testSendGetMySentMessageCount;
	Test* m_testSendGetMySentMessageHeader;
	Test* m_testSendReadMySentMessage;
	Test* m_testSendDelMySentMessage;

	TestList *m_testList;	
	TestReporter *m_testReporter;
	TestRunner *m_testRunner;	

	GLXPlayerLogin * m_GLXPlayerLogin;
	Test* m_curTest;
};

class TestGLXPlayerMessage : public TestImpl
{
public:
	TestGLXPlayerMessage(GLXPlayerLogin* pGLXPlayerLogin, char const* testName):TestImpl(pGLXPlayerLogin, testName, "TestGLXPlayerMessage")
	{		
		m_GLXPlayerMessage = XP_API_NEW GLXPlayerMessage();
		m_GLXPlayerMessage->RegisterObserver(this);				
	}
	
	virtual ~TestGLXPlayerMessage()
	{
		XP_API_DEL(m_GLXPlayerMessage);		
	}
	
	virtual void RunImpl(){};
	
public:
	GLXPlayerMessage* m_GLXPlayerMessage;	
};


class TestSendOnlineMessage : public TestGLXPlayerMessage
{
public:
	TestSendOnlineMessage(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerMessage(pGLXPlayerLogin, "TestSendOnlineMessage"){}

	virtual ~TestSendOnlineMessage(){}

	void RunImpl()
	{		
		runLoginProcess(TEST_USER_NAME, TEST_PASSWORD);
		m_GLXPlayerMessage->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerMessage->SendOnlineMessage(TEST_USER_NAME, TEST_SEND_MSG_TYPE, TEST_SEND_MSG_TITLE, TEST_SEND_MSG_BODY, XP_API_STRLEN(TEST_SEND_MSG_BODY));
		while(!m_testFinished)
		{
			m_GLXPlayerMessage->Update();
		}		
	}	
};

class TestSendGetMessageCount : public TestGLXPlayerMessage
{
public:
	TestSendGetMessageCount(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerMessage(pGLXPlayerLogin, "TestSendGetMessageCount"){}

	virtual ~TestSendGetMessageCount(){}

	void RunImpl()
	{				
		m_GLXPlayerMessage->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerMessage->SendGetMessageCount();
		while(!m_testFinished)
		{
			m_GLXPlayerMessage->Update();
		}
		
		if ( ( TEST_MSG_TYPE & USER_MESSAGE_TYPE_GAMEMESSAGE ) != 0 )
		{
			TEST_MSG_COUNT = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_GAMEMESSAGE, TEST_MSG_READ_TYPE);
		}

		if ( ( TEST_MSG_TYPE & USER_MESSAGE_TYPE_USERMESSAGE ) != 0 )
		{
			TEST_MSG_COUNT += m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE, TEST_MSG_READ_TYPE);
		}

		if ( ( TEST_MSG_TYPE & USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION ) != 0 )
		{
			TEST_MSG_COUNT += m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION, TEST_MSG_READ_TYPE);
		}

		if ( ( TEST_MSG_TYPE & USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION ) != 0 )
		{
			TEST_MSG_COUNT += m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION, TEST_MSG_READ_TYPE);
		}

		if ( ( TEST_MSG_TYPE & USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION ) != 0 )
		{
			TEST_MSG_COUNT += m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION, TEST_MSG_READ_TYPE);
		}		
	}	
};

class TestSendGetMessageHeader : public TestGLXPlayerMessage
{
public:
	TestSendGetMessageHeader(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerMessage(pGLXPlayerLogin, "TestSendGetMessageHeader"){}

	virtual ~TestSendGetMessageHeader(){}

	void RunImpl()
	{		
		m_GLXPlayerMessage->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerMessage->SendGetMessageHeader(TEST_MSG_TYPE, TEST_MSG_READ_TYPE, TEST_MSG_INDEX, TEST_MSG_COUNT);
		while(!m_testFinished)
		{
			m_GLXPlayerMessage->Update();
		}			
		TEST_MSG_ID = m_GLXPlayerMessage->getMsgID(TEST_MSG_POSITION);
	}	
};

class TestSendReadMessage : public TestGLXPlayerMessage
{
public:
	TestSendReadMessage(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerMessage(pGLXPlayerLogin, "TestSendReadMessage"){}

	virtual ~TestSendReadMessage(){}

	void RunImpl()
	{		
		m_GLXPlayerMessage->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerMessage->SendReadMessage(TEST_MSG_ID);
		while(!m_testFinished)
		{
			m_GLXPlayerMessage->Update();
		}		
	}	
};

class TestSendDeleteMessage : public TestGLXPlayerMessage
{
public:
	TestSendDeleteMessage(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerMessage(pGLXPlayerLogin, "TestSendDeleteMessage"){}

	virtual ~TestSendDeleteMessage(){}

	void RunImpl()
	{		
		m_GLXPlayerMessage->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerMessage->SendDeleteMessage(TEST_MSG_ID);
		while(!m_testFinished)
		{
			m_GLXPlayerMessage->Update();
		}		
	}	
};

class TestSendGetMySentMessageCount : public TestGLXPlayerMessage
{
public:
	TestSendGetMySentMessageCount(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerMessage(pGLXPlayerLogin, "TestSendGetMySentMessageCount"){}

	virtual ~TestSendGetMySentMessageCount(){}

	void RunImpl()
	{		
		m_GLXPlayerMessage->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerMessage->SendGetMySentMessageCount();
		while(!m_testFinished)
		{
			m_GLXPlayerMessage->Update();
		}
		
		if ( ( TEST_MSG_TYPE & USER_MESSAGE_TYPE_GAMEMESSAGE ) != 0 )
		{
			TEST_MSG_COUNT = m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_GAMEMESSAGE, TEST_MSG_READ_TYPE);
		}

		if ( ( TEST_MSG_TYPE & USER_MESSAGE_TYPE_USERMESSAGE ) != 0 )
		{
			TEST_MSG_COUNT += m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE, TEST_MSG_READ_TYPE);
		}

		if ( ( TEST_MSG_TYPE & USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION ) != 0 )
		{
			TEST_MSG_COUNT += m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION, TEST_MSG_READ_TYPE);
		}

		if ( ( TEST_MSG_TYPE & USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION ) != 0 )
		{
			TEST_MSG_COUNT += m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION, TEST_MSG_READ_TYPE);
		}

		if ( ( TEST_MSG_TYPE & USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION ) != 0 )
		{
			TEST_MSG_COUNT += m_GLXPlayerMessage->getMsgCount(USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION, TEST_MSG_READ_TYPE);
		}
	}	
};

class TestSendGetMySentMessageHeader : public TestGLXPlayerMessage
{
public:
	TestSendGetMySentMessageHeader(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerMessage(pGLXPlayerLogin, "TestSendGetMySentMessageHeader"){}

	virtual ~TestSendGetMySentMessageHeader(){}

	void RunImpl()
	{		
		m_GLXPlayerMessage->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerMessage->SendGetMySentMessageHeader(TEST_MSG_INDEX, TEST_MSG_COUNT);
		while(!m_testFinished)
		{
			m_GLXPlayerMessage->Update();
		}
		TEST_MSG_ID = m_GLXPlayerMessage->getMsgID(TEST_MSG_POSITION);			
		char* userNumber = m_GLXPlayerMessage->getMsgDesUserNumber(0, 0);	
		char* userName = m_GLXPlayerMessage->getMsgDesUserName(0, 0);	
		XP_API_DEL(userNumber);
		XP_API_DEL(userName);		
	}	
};

class TestSendReadMySentMessage : public TestGLXPlayerMessage
{
public:
	TestSendReadMySentMessage(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerMessage(pGLXPlayerLogin, "TestSendReadMySentMessage"){}

	virtual ~TestSendReadMySentMessage(){}

	void RunImpl()
	{		
		m_GLXPlayerMessage->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerMessage->SendReadMySentMessage(TEST_MSG_ID);
		while(!m_testFinished)
		{
			m_GLXPlayerMessage->Update();
		}
	}	
};

class TestSendDelMySentMessage : public TestGLXPlayerMessage
{
public:
	TestSendDelMySentMessage(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerMessage(pGLXPlayerLogin, "TestSendDelMySentMessage"){}

	virtual ~TestSendDelMySentMessage(){}

	void RunImpl()
	{		
		m_GLXPlayerMessage->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerMessage->SendDelMySentMessage(TEST_MSG_ID);
		while(!m_testFinished)
		{
			m_GLXPlayerMessage->Update();
		}				
		runLogoutProcess();
	}	
};
