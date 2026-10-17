#include "MockGLXPlayerUser.h"
#include "memory.h"

MockGLXPlayerUser::MockGLXPlayerUser(GLXPlayerLogin* pGLXPlayerLogin)
{
	m_GLXPlayerLogin = pGLXPlayerLogin;	
	
	m_testRegister = XP_API_NEW TestRegister(m_GLXPlayerLogin);
	m_testRetrievalPassword = XP_API_NEW TestRetrievalPassword(m_GLXPlayerLogin);	
	m_testChangeUserName = XP_API_NEW TestChangeUserName(m_GLXPlayerLogin);
	m_testChangePassword = XP_API_NEW TestChangePassword(m_GLXPlayerLogin);
	m_testChangeEmail = XP_API_NEW TestChangeEmail(m_GLXPlayerLogin);
	m_testChangeCountry = XP_API_NEW TestChangeCountry(m_GLXPlayerLogin);	
	m_testGetCountry = XP_API_NEW TestGetCountry(m_GLXPlayerLogin);
	m_testGetUsedApp = XP_API_NEW  TestGetUsedApp(m_GLXPlayerLogin);	
	m_testGetUserGameList = XP_API_NEW TestGetUserGameList(m_GLXPlayerLogin);
	m_testGetUserState = XP_API_NEW TestGetUserState(m_GLXPlayerLogin);
	m_testSetUserStoreData = XP_API_NEW TestSetUserStoreData(m_GLXPlayerLogin);
	m_testGetUserStoreData = XP_API_NEW TestGetUserStoreData(m_GLXPlayerLogin);
	m_testGetReputation = XP_API_NEW TestGetReputation(m_GLXPlayerLogin);
	m_testPointUserReputation = XP_API_NEW TestPointUserReputation(m_GLXPlayerLogin);
	m_testGetUserUID = XP_API_NEW TestGetUserUID(m_GLXPlayerLogin);
	m_testGetUserBestScore = XP_API_NEW TestGetUserBestScore(m_GLXPlayerLogin);
	m_testGetUserPhoto = XP_API_NEW TestGetUserPhoto(m_GLXPlayerLogin);
	m_testDownloadUserAvatar = XP_API_NEW TestDownloadUserAvatar(m_GLXPlayerLogin);
	m_testDelUserAvatar = XP_API_NEW TestDelUserAvatar(m_GLXPlayerLogin);
	m_testGetUserAvatarList = XP_API_NEW TestGetUserAvatarList(m_GLXPlayerLogin);
	m_testUploadUserPhoto = XP_API_NEW TestUploadUserPhoto(m_GLXPlayerLogin);
	m_testDelUserStoreData = XP_API_NEW TestDelUserStoreData(m_GLXPlayerLogin);	
	m_testGetUserInfo = XP_API_NEW TestGetUserInfo(m_GLXPlayerLogin);
	m_testUpdateUserInfo = XP_API_NEW TestUpdateUserInfo(m_GLXPlayerLogin);
	m_testGetTrophyIcon = XP_API_NEW TestGetTrophyIcon(m_GLXPlayerLogin);
	m_testAwardTrophy = XP_API_NEW TestAwardTrophy(m_GLXPlayerLogin);
	m_testGetGameTrophyList = XP_API_NEW TestGetGameTrophyList(m_GLXPlayerLogin);
	m_testGetUserGameTrophyList = XP_API_NEW TestGetUserGameTrophyList(m_GLXPlayerLogin);
	m_testGetAdvRSS = XP_API_NEW TestGetAdvRSS(m_GLXPlayerLogin);
	m_testGetAdvAttachment = XP_API_NEW TestGetAdvAttachment(m_GLXPlayerLogin);
	m_testGetGameIcon = XP_API_NEW TestGetGameIcon(m_GLXPlayerLogin);
	m_testSetUserState = XP_API_NEW TestSetUserState(m_GLXPlayerLogin);

	m_testList = XP_API_NEW TestList();
	m_testList->Add(m_testRegister);
	m_testList->Add(m_testRetrievalPassword);
	m_testList->Add(m_testChangeUserName);
	m_testList->Add(m_testChangePassword);
	m_testList->Add(m_testChangeEmail);
	m_testList->Add(m_testChangeCountry);	
	m_testList->Add(m_testGetCountry);	
	m_testList->Add(m_testGetUsedApp);		
	m_testList->Add(m_testGetUserGameList);	
	m_testList->Add(m_testGetUserState);	
	m_testList->Add(m_testSetUserStoreData);	
	m_testList->Add(m_testGetUserStoreData);	
	m_testList->Add(m_testGetReputation);	
	m_testList->Add(m_testPointUserReputation);	
	m_testList->Add(m_testGetUserUID);	
	m_testList->Add(m_testGetUserBestScore);	
	m_testList->Add(m_testGetUserPhoto);	
	m_testList->Add(m_testDownloadUserAvatar);	
	m_testList->Add(m_testDelUserAvatar);	
	m_testList->Add(m_testGetUserAvatarList);	
	m_testList->Add(m_testUploadUserPhoto);
	m_testList->Add(m_testDelUserStoreData);	
	m_testList->Add(m_testGetUserInfo);
	m_testList->Add(m_testUpdateUserInfo);
	m_testList->Add(m_testGetTrophyIcon);
	m_testList->Add(m_testAwardTrophy);
	m_testList->Add(m_testGetGameTrophyList);
	m_testList->Add(m_testGetUserGameTrophyList);
	m_testList->Add(m_testGetAdvRSS);
	m_testList->Add(m_testGetAdvAttachment);
	m_testList->Add(m_testGetGameIcon);
	m_testList->Add(m_testSetUserState);
	
	m_testReporter = XP_API_NEW TestReporterStdout();
	m_testRunner = XP_API_NEW TestRunner(*m_testReporter);	
	m_curTest = m_testList->GetHead();
}

MockGLXPlayerUser::~MockGLXPlayerUser()
{		
	XP_API_DEL(m_testRegister);
	XP_API_DEL(m_testRetrievalPassword);
	XP_API_DEL(m_testChangeUserName);
	XP_API_DEL(m_testChangePassword);
	XP_API_DEL(m_testChangeEmail);
	XP_API_DEL(m_testChangeCountry);
	XP_API_DEL(m_testGetCountry);	
	XP_API_DEL(m_testGetUsedApp);		
	XP_API_DEL(m_testGetUserGameList);	
	XP_API_DEL(m_testGetUserState);	
	XP_API_DEL(m_testSetUserStoreData);	
	XP_API_DEL(m_testGetUserStoreData);	
	XP_API_DEL(m_testGetReputation);	
	XP_API_DEL(m_testPointUserReputation);	
	XP_API_DEL(m_testGetUserUID);	
	XP_API_DEL(m_testGetUserBestScore);	
	XP_API_DEL(m_testGetUserPhoto);	
	XP_API_DEL(m_testDownloadUserAvatar);	
	XP_API_DEL(m_testDelUserAvatar);	
	XP_API_DEL(m_testGetUserAvatarList);	
	XP_API_DEL(m_testUploadUserPhoto);
	XP_API_DEL(m_testDelUserStoreData);	
	XP_API_DEL(m_testGetUserInfo);
	XP_API_DEL(m_testUpdateUserInfo);
	XP_API_DEL(m_testGetTrophyIcon);
	XP_API_DEL(m_testAwardTrophy);
	XP_API_DEL(m_testGetGameTrophyList);
	XP_API_DEL(m_testGetUserGameTrophyList);
	XP_API_DEL(m_testGetAdvRSS);
	XP_API_DEL(m_testGetAdvAttachment);
	XP_API_DEL(m_testGetGameIcon);
	XP_API_DEL(m_testSetUserState);		
	
	XP_API_DEL(m_testList);
	XP_API_DEL(m_testRunner);
	XP_API_DEL(m_testReporter);
	XP_API_DEL(m_curTest);
}

void MockGLXPlayerUser::runAllTest()
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

void MockGLXPlayerUser::registerResult(TestResults *result)
{
	m_testRunner->setResult(result);
}

