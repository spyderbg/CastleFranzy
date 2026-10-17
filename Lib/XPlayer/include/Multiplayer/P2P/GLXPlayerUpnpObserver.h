#ifndef __GL_XPLAYER_UPNPOBSERVER_H__
#define __GL_XPLAYER_UPNPOBSERVER_H__


class GLXPlayerUpnpObserver
{

public:


	virtual void OnNetworkError() = 0;

	virtual void OnRequestTimeout() = 0;

	virtual void OnUpnpRequestFailed(char *errMsg, int errCode)=0;

	virtual void OnUpnpInitSuccess(const char* externIp)=0;

	virtual  void OnAddPortMappingSuccess(int type,int portMapped)=0;

	virtual  void OnRemovePortMappingSuccess()=0;

	virtual void OnRemoveAllPortMappingSuccess()=0;

	//virtual void OnGetExternIPSuccess() = 0;

};

#endif