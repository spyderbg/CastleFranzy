#ifndef __GL_XPLAYER_MP_BASE_OBSERVER_H__
#define __GL_XPLAYER_MP_BASE_OBSERVER_H__

#include "DataPacket.h"

class GLXPlayerMPBaseObserver
{

public:	

//---------- common -----------
	virtual void OnMPNetworkError(int error) = 0;
	virtual void OnMPFunctionError(char* errMsg, int error_code) = 0;
	
	virtual void OnMPEstablishConnectionSuccess() = 0;
	virtual void OnMPDisconnectSuccess() = 0;
	virtual void OnMPLoginSuccess(DataPacket* currentPacket = NULL) = 0;	

	virtual void OnMPCreateSessionSuccess(DataPacket* currentPacket = NULL) = 0;	
	virtual void OnMPListSessionSuccess(DataPacket* currentPacket = NULL) = 0;	
	virtual void OnMPJoinSessionSuccess(DataPacket* currentPacket = NULL) = 0;	
	virtual void OnMPLeaveSessionSuccess() = 0;
	virtual void OnMPKickOutPlayerSuccess() = 0;

	virtual void OnMPStartGameSuccess(DataPacket* currentPacket = NULL) = 0;	

//---------- p2p -----------
	virtual void OnMPFinishGameSuccess(){}
	virtual void OnMPuPnPTestSuccess(){}
	virtual void OnMPLeaveGameSuccess(){}

//---------- lobby -----------
	enum PARAM_TYPE 
	{
		PARAM_NONE = 0,
		PARAM_GAME,
		PARAM_PLAYER,

		PARAM_TYPE_COUNT
	};
	virtual void OnMPSetParamSuccess(int result){}

	virtual void OnMPProcessPushMessages(DataPacket* currentPacket, int messageID){}
	virtual void OnMPRejoinLobbySuccess(DataPacket* currentPacket, bool bNeedChangeStatus){}
	virtual void OnMPGetLobbyInfo(DataPacket* currentPacket){}
	virtual void OnMPListSessionSuccessWithGameParam(DataPacket* currentPacket){}

	virtual void OnMPStartGameFailed(int result){}
	virtual void OnMPGetLobbyForName( DataPacket* currentPacket ){}
	virtual void OnMPGetLobbyForFriend( DataPacket* currentPacket ){}

	virtual void OnMPAutoMatchRequeset(int errType){}
	virtual void OnMPAutoMatchRequesetCancel(){}
};

#endif