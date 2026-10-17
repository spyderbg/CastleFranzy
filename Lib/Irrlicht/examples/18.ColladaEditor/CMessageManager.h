#pragma once

#include <list>
class wxMutex;


class CMessageManager
{
public:
	static void LogMessage(const char *pFormat, ...);
	//static void LogMessage(const char *pMessage);
	static void OnSceneChanged();
	static void OnAddAnimation();
	static void OnDisplayCapture(void *pCapture);

	static void PopMessage();
	static int GetMessage(int &iMessageType, void *&pMessageData);

	enum MessageType
	{
		EMT_LOG,
		EMT_SCENE_GRAPH_HAS_CHANGED,
		EMT_ANIMATION_ADDED,
		EMT_DATABASE_ADDED,
		EMT_DISPLAY_CAPTURE,
	};
protected:
	

	// Message Utility beetween slaves and main thread
	struct Message
	{
		int iMessageType;
		void* pMessageData;
	};

	static std::list<Message> s_MessageQueue;
	static wxMutex s_MessageMutex;

	static void PushMessage(int iMessageType, void *pMessageData);
	
};
