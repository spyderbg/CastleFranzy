#pragma once
#include "../../include/Login/GLXPlayerLogin.h"
#include "../../include/User/GLXPlayerUser.h"
#include "TestImpl.h"
#include "TestReporterStdout.h"
#include "Stream.h"

class MockGLXPlayerUser	
{
public:
	MockGLXPlayerUser(GLXPlayerLogin* pGLXPlayerLogin);
	virtual ~MockGLXPlayerUser(void);
	
	void runAllTest();		
	void registerResult(TestResults *result);	
	
	Test* m_testRegister;
	Test* m_testRetrievalPassword;
	Test* m_testChangeUserName;
	Test* m_testChangePassword;
	Test* m_testChangeEmail;
	Test* m_testChangeCountry;	
	Test* m_testGetCountry;
	Test* m_testGetUsedApp;	
	Test* m_testGetUserGameList;
	Test* m_testGetUserState;
	Test* m_testSetUserStoreData;
	Test* m_testGetUserStoreData;
	Test* m_testGetReputation;
	Test* m_testPointUserReputation;
	Test* m_testGetUserUID;
	Test* m_testGetUserBestScore;
	Test* m_testGetUserPhoto;
	Test* m_testDownloadUserAvatar;
	Test* m_testDelUserAvatar;
	Test* m_testGetUserAvatarList;
	Test* m_testUploadUserPhoto;
	Test* m_testDelUserStoreData;
	Test* m_testGetUserInfo;
	Test* m_testUpdateUserInfo;
	Test* m_testGetTrophyIcon;
	Test* m_testAwardTrophy;
	Test* m_testGetGameTrophyList;
	Test* m_testGetUserGameTrophyList;
	Test* m_testGetAdvRSS;
	Test* m_testGetAdvAttachment;
	Test* m_testGetGameIcon;
	Test* m_testSetUserState;		

	TestList *m_testList;	
	TestReporter *m_testReporter;
	TestRunner *m_testRunner;	

	GLXPlayerLogin * m_GLXPlayerLogin;
	Test* m_curTest;	
};

class TestGLXPlayerUser : public TestImpl
{
public:
	TestGLXPlayerUser(GLXPlayerLogin* pGLXPlayerLogin, char const* testName, char const* suiteName):TestImpl(pGLXPlayerLogin, testName, suiteName)
	{		
		m_GLXPlayerUser = XP_API_NEW GLXPlayerUser();
		m_GLXPlayerUser->RegisterObserver(this);		
	}
	
	virtual ~TestGLXPlayerUser()
	{
		XP_API_DEL(m_GLXPlayerUser);		
	}
	
	virtual void RunImpl(){};
	
public:
	GLXPlayerUser* m_GLXPlayerUser;	
};

class TestRegister : public TestGLXPlayerUser
{
public:
	TestRegister(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestRegister", "TestGLXPlayerUser"){}

	virtual ~TestRegister(){}

	void RunImpl()
	{				
		int random = rand();		
		XP_API_MEMSET(USER_NAME, 0, 32);
		XP_API_SPRINTF(USER_NAME, "%s%d", TEST_REGISTER_NAME, random);
		
		m_GLXPlayerUser->sendRegister(USER_NAME, TEST_PASSWORD, TEST_REGISTER_EMAIL, TEST_REGISTER_COUNTRY, TEST_REGISTER_EMAIL_FLAG, TEST_REGISTER_GLLIVE, TEST_DEV_TOKEN);

		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}			
	}		
};


class TestRetrievalPassword : public TestGLXPlayerUser
{
public:
	TestRetrievalPassword(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestRetrievalPassword", "TestGLXPlayerUser"){}

	virtual ~TestRetrievalPassword(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->sendRetrievalPassword(USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();		}
		
		runLoginProcess(USER_NAME, TEST_PASSWORD);		
	}	
};


class TestChangeUserName : public TestGLXPlayerUser
{
public:
	TestChangeUserName(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestChangeUserName", "TestGLXPlayerUser"){}

	virtual ~TestChangeUserName(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		
		m_GLXPlayerUser->sendChangeUserName(USER_NAME, TEST_PASSWORD, TEST_NEW_USER_NAME);
		for(int i = 0; i < 1000; i++)
		{
			for(int j = 0; j < 1000; j++)
			{
				m_GLXPlayerUser->Update();
			}
		}
		
		m_GLXPlayerUser->sendChangeUserName(TEST_NEW_USER_NAME, TEST_PASSWORD, USER_NAME);
		for(int i = 0; i < 1000; i++)
		{
			for(int j = 0; j < 1000; j++)
			{
				m_GLXPlayerUser->Update();
			}
		}
	}	
};

class TestChangePassword : public TestGLXPlayerUser
{
public:
	TestChangePassword(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestChangePassword", "TestGLXPlayerUser"){}

	virtual ~TestChangePassword(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		
		m_GLXPlayerUser->sendChangePassword(USER_NAME, TEST_PASSWORD, TEST_NEW_PASSWORD, TEST_NEW_PASSWORD);
		
		for(int i = 0; i < 1500; i++)
		{
			for(int j = 0; j < 1000; j++)
			{
				m_GLXPlayerUser->Update();
			}
		}
		
		m_GLXPlayerUser->sendChangePassword(USER_NAME, TEST_NEW_PASSWORD, TEST_PASSWORD, TEST_PASSWORD);
		for(int i = 0; i < 15000; i++)
		{
			for(int j = 0; j < 1000; j++)
			{
				m_GLXPlayerUser->Update();
			}
		}
	}
};

class TestChangeEmail : public TestGLXPlayerUser
{
public:
	TestChangeEmail(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestChangeEmail", "TestGLXPlayerUser"){}

	virtual ~TestChangeEmail(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		
		m_GLXPlayerUser->sendChangeEmail(USER_NAME, TEST_PASSWORD, TEST_NEW_EMAIL);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}		
	}
};

class TestChangeCountry : public TestGLXPlayerUser
{
public:
	TestChangeCountry(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestChangeCountry", "TestGLXPlayerUser"){}

	virtual ~TestChangeCountry(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		
		m_GLXPlayerUser->sendChangeCountry(USER_NAME, TEST_PASSWORD, TEST_NEW_COUNTRY);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestGetCountry : public TestGLXPlayerUser
{
public:
	TestGetCountry(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetCountry", "TestGLXPlayerUser"){}

	virtual ~TestGetCountry(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		
		m_GLXPlayerUser->sendGetCountry(TEST_OTHER_USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestGetUsedApp : public TestGLXPlayerUser
{
public:
	TestGetUsedApp(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetUsedApp", "TestGLXPlayerUser"){}

	virtual ~TestGetUsedApp(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		
		m_GLXPlayerUser->sendGetUsedApp();
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestGetUserGameList : public TestGLXPlayerUser
{
public:
	TestGetUserGameList(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetUserGameList", "TestGLXPlayerUser"){}

	virtual ~TestGetUserGameList(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetUserGameList(TEST_OTHER_USER_NAME, TEST_DAY, TEST_LIMIT);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestGetUserState : public TestGLXPlayerUser
{
public:
	TestGetUserState(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetUserState", "TestGLXPlayerUser"){}

	virtual ~TestGetUserState(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetUserState(TEST_OTHER_USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestSetUserStoreData : public TestGLXPlayerUser
{
public:
	TestSetUserStoreData(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestSetUserStoreData", "TestGLXPlayerUser"){}

	virtual ~TestSetUserStoreData(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendSetUserStoreData(TEST_STORE_NAME, TEST_STORE_VALUE);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestGetUserStoreData : public TestGLXPlayerUser
{
public:
	TestGetUserStoreData(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetUserStoreData", "TestGLXPlayerUser"){}

	virtual ~TestGetUserStoreData(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetUserStoreData(TEST_STORE_NAME, TEST_OTHER_USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestGetReputation : public TestGLXPlayerUser
{
public:
	TestGetReputation(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetReputation", "TestGLXPlayerUser"){}

	virtual ~TestGetReputation(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetReputation(TEST_OTHER_USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestPointUserReputation : public TestGLXPlayerUser
{
public:
	TestPointUserReputation(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestPointUserReputation", "TestGLXPlayerUser"){}

	virtual ~TestPointUserReputation(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendPointUserReputation(TEST_USER_NAME, TEST_POINT_REPUTATION);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestGetUserUID : public TestGLXPlayerUser
{
public:
	TestGetUserUID(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetUserUID", "TestGLXPlayerUser"){}

	virtual ~TestGetUserUID(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetUserUID(TEST_OTHER_USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestGetUserBestScore : public TestGLXPlayerUser
{
public:
	TestGetUserBestScore(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetUserBestScore", "TestGLXPlayerUser"){}

	virtual ~TestGetUserBestScore(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetUserBestScore(TEST_OTHER_USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestGetUserPhoto : public TestGLXPlayerUser
{
public:
	TestGetUserPhoto(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetUserPhoto", "TestGLXPlayerUser"){}

	virtual ~TestGetUserPhoto(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetUserAvatar(TEST_OTHER_USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
		XP_API_MEMSET(TEST_AVATAR_KEY, 0, 32);
		XP_API_SPRINTF(TEST_AVATAR_KEY, m_GLXPlayerUser->getUserAvatarKey());
	}	
};

class TestDownloadUserAvatar : public TestGLXPlayerUser
{
public:
	TestDownloadUserAvatar(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestDownloadUserAvatar", "TestGLXPlayerUser"){}

	virtual ~TestDownloadUserAvatar(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendDownloadUserAvatar(TEST_AVATAR_KEY);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestDelUserAvatar : public TestGLXPlayerUser
{
public:
	TestDelUserAvatar(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestDelUserAvatar", "TestGLXPlayerUser"){}

	virtual ~TestDelUserAvatar(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendDelUserAvatar(TEST_AVATAR_KEY);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestGetUserAvatarList : public TestGLXPlayerUser
{
public:
	TestGetUserAvatarList(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetUserAvatarList", "TestGLXPlayerUser"){}

	virtual ~TestGetUserAvatarList(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetUserAvatarList();
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}	
	}	
};

class TestUploadUserPhoto : public TestGLXPlayerUser
{
public:
	TestUploadUserPhoto(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestUploadUserPhoto", "TestGLXPlayerUser"){}

	virtual ~TestUploadUserPhoto(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		
		CResourceStream *stream = XP_API_NEW CResourceStream(TEST_AVATAR_PATH);
		if(stream != NULL)
		{
			BYTE* buffer = (BYTE*)(stream->GetBuffer());
			char* blob = SSEncDec_ByteArray2Blob(buffer, stream->GetLength());
			
			m_GLXPlayerUser->sendUploadUserAvatar(blob);
			while(!m_testFinished)
			{
				m_GLXPlayerUser->Update();
			}		
			
			XP_API_DEL(blob);
			XP_API_DEL(stream);							
		}
		else
		{
			OnRequestFailure( FUNCTION_UPLOAD_USER_AVATAR, ERROR_INIT);
		}
			
	}	
};

class TestDelUserStoreData : public TestGLXPlayerUser
{
public:
	TestDelUserStoreData(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestDelUserStoreData", "TestGLXPlayerUser"){}

	virtual ~TestDelUserStoreData(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());		
		m_GLXPlayerUser->sendDelUserStoreData(TEST_STORE_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};

class TestGetUserInfo : public TestGLXPlayerUser
{
public:
	TestGetUserInfo(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetUserInfo", "TestGLXPlayerUser"){}

	virtual ~TestGetUserInfo(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());		
		m_GLXPlayerUser->sendGetUserInfo(TEST_OTHER_USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};

class TestUpdateUserInfo : public TestGLXPlayerUser
{
public:
	TestUpdateUserInfo(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestUpdateUserInfo", "TestGLXPlayerUser"){}

	virtual ~TestUpdateUserInfo(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendUpdateUserInfo(TEST_USER_EMAIL, TEST_USER_EMAIL_FLAG, TEST_USER_COUNTRY, TEST_USER_FIRST_NAME, TEST_USER_LAST_NAME, TEST_USER_SEX, TEST_USER_BIRTHDAY);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};

class TestGetTrophyIcon : public TestGLXPlayerUser
{
public:
	TestGetTrophyIcon(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetTrophyIcon", "TestGLXPlayerUser"){}

	virtual ~TestGetTrophyIcon(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetTrophyIcon(TEST_TROPHY_ID);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};

class TestAwardTrophy : public TestGLXPlayerUser
{
public:
	TestAwardTrophy(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestAwardTrophy", "TestGLXPlayerUser"){}

	virtual ~TestAwardTrophy(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		int trophyID[5] = {{1}, {2}, {3}, {4}, {5}};
		m_GLXPlayerUser->sendAwardTrophy(trophyID, 5);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};

class TestGetGameTrophyList : public TestGLXPlayerUser
{
public:
	TestGetGameTrophyList(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetGameTrophyList", "TestGLXPlayerUser"){}

	virtual ~TestGetGameTrophyList(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetGameTrophyList(TEST_PRODUCT_ID);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};

class TestGetUserGameTrophyList : public TestGLXPlayerUser
{
public:
	TestGetUserGameTrophyList(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetUserGameTrophyList", "TestGLXPlayerUser"){}

	virtual ~TestGetUserGameTrophyList(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetUserGameTrophyList(TEST_PRODUCT_ID, TEST_OTHER_USER_NAME);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};

class TestGetAdvRSS : public TestGLXPlayerUser
{
public:
	TestGetAdvRSS(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetAdvRSS", "TestGLXPlayerUser"){}

	virtual ~TestGetAdvRSS(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetAdvRSS(TEST_PRODUCT_ID);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};

class TestGetAdvAttachment : public TestGLXPlayerUser
{
public:
	TestGetAdvAttachment(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetAdvAttachment", "TestGLXPlayerUser"){}

	virtual ~TestGetAdvAttachment(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetAdvAttachment(TEST_ATTACHMENT_ID);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};

class TestGetGameIcon : public TestGLXPlayerUser
{
public:
	TestGetGameIcon(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestGetGameIcon", "TestGLXPlayerUser"){}

	virtual ~TestGetGameIcon(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendGetGameIcon(TEST_PRODUCT_ID);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};

class TestSetUserState : public TestGLXPlayerUser
{
public:
	TestSetUserState(GLXPlayerLogin* pGLXPlayerLogin):TestGLXPlayerUser(pGLXPlayerLogin, "TestSetUserState", "TestGLXPlayerUser"){}

	virtual ~TestSetUserState(){}

	void RunImpl()
	{		
		m_GLXPlayerUser->SetUID(m_GLXPlayerLogin->GetUID());
		m_GLXPlayerUser->sendSetUserState(TEST_USER_STATE);
		while(!m_testFinished)
		{
			m_GLXPlayerUser->Update();
		}				
	}	
};
