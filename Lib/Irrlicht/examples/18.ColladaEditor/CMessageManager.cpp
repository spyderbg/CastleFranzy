#include "CMessageManager.h"
#include <stdarg.h>
#include <wx/thread.h>

std::list<CMessageManager::Message> CMessageManager::s_MessageQueue;
wxMutex CMessageManager::s_MessageMutex;

void 
CMessageManager::LogMessage(const char *pFormat, ...)
{
	char pMessage[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(pMessage, pFormat, args);
	va_end(args);

	
	
	int iSize = strlen(pMessage);
	if(iSize)
	{
		char *pNewMessage = new char[iSize+1];
		strcpy(pNewMessage, pMessage);
		PushMessage(EMT_LOG, (void *)pNewMessage);
	}
}

void 
CMessageManager::OnSceneChanged()
{
	PushMessage(EMT_SCENE_GRAPH_HAS_CHANGED, 0);
}

void 
CMessageManager::OnAddAnimation()
{
	PushMessage(EMT_ANIMATION_ADDED, 0);
}

void 
CMessageManager::OnDisplayCapture(void *pCapture)
{
	PushMessage(EMT_DISPLAY_CAPTURE, pCapture);
}

void 
CMessageManager::PushMessage(int iMessageType, void *pMessageData)
{
	// before using the list we must acquire the mutex
    wxMutexLocker lock(s_MessageMutex);

	Message msg;
	msg.iMessageType = iMessageType;
	msg.pMessageData = pMessageData;
	s_MessageQueue.push_back(msg);
}

void 
CMessageManager::PopMessage()
{
	// before using the list we must acquire the mutex
    wxMutexLocker lock(s_MessageMutex);

	if(s_MessageQueue.size() != 0)
	{
		int iMessageType = s_MessageQueue.front().iMessageType;
		void *pMessageData = s_MessageQueue.front().pMessageData;

		switch(iMessageType)
		{
		case EMT_LOG:
			delete [] (char *)pMessageData;
			break;
		default:
			//Do Nothing
			break;
		};
		s_MessageQueue.pop_front();
	}
}

int 
CMessageManager::GetMessage(int &iMessageType, void *&pMessageData)
{
	// before using the list we must acquire the mutex
    wxMutexLocker lock(s_MessageMutex);

	if(s_MessageQueue.size() == 0)
	{
		return -1; // Failed
	}

	iMessageType = s_MessageQueue.begin()->iMessageType;
	pMessageData = s_MessageQueue.begin()->pMessageData;
	
	return 0; // Succeed
}
