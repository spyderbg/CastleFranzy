#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

#include "GLLiveStateHandler.h"
#include "TouchEvent.h"
#include "GLLiveMenuItemObserver.h"
#include "StringManager.h"
#include "GLLive_ASprite.h"
#include "GLLive_Font.h"
#include "GLLiveMessageBox.h"
#include "Porting.h"
#include "GLXPlayerSereverConfig.h"
#include "GLXPlayerUser.h"
#include "GLXPlayerLogin.h"
#include "GLXPlayerMessage.h"
#include "GLXPlayerUserFriend.h"
#include "GLXPlayerChat.h"

class GLLiveMenu;

#define MAX_USERNAME       16
#define MAX_PASSWORD       16
#define MAX_EMAIL          128
#define MAX_FILENAME       64
//The user information
/*
struct UserInfo
{
	//General user info
	char username[MAX_USERNAME];
	char password[MAX_PASSWORD];
	char email[MAX_EMAIL];
	int  avatarId;
	int  country;
	int  totalScore;
	int reputation;
	bool emailFlag;
	//Login user info
	//int friendCount;
	//int msgNewCount;
	char *news;
	//more - ad. pic
	//...
	UserInfo()
	{
		news = NULL;
		Clear();
	}
	~UserInfo()
	{
		Clear();
	}
	void Clear()
	{
		XP_API_MEMSET(username, 0, sizeof(username));
		XP_API_MEMSET(password, 0, sizeof(password));
		XP_API_MEMSET(email, 0, sizeof(email));
		//XP_API_MEMSET(avatarFile, 0, sizeof(avatarFile));
		avatarId    = -1;   // -1  Has not set
		country     = 0;   // -1  Has not set
		totalScore  = 0;
		reputation  = 0;
		//friendCount = 0;
		//msgNewCount = 0;
		emailFlag   = true;
		
		XP_API_DEL(news);
	}
};
*/

static const char BASE64_STRING[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static void base64_encode(const char * inBuffer, const int iInSize, char * & outBuffer, int & iOutSize)
{
	if(inBuffer == NULL)
	{
		outBuffer = NULL;
		iOutSize = -1;
		return;
	}
	if(iInSize == 0)
	{
		outBuffer = new char[1];
		outBuffer[0] = '\0';
		iOutSize = 0;
		return;
	}
	if(outBuffer != NULL)
	{
		delete(outBuffer);
	}
	int iLength = iInSize;
	int iFixedLength = iLength % 3;
	
	iOutSize = (iLength + (3 - iFixedLength) % 3) * 4 / 3;
	
	outBuffer = new char[iOutSize + 1];
	memset(outBuffer, 0, sizeof(char) * iOutSize);

	iLength -= iFixedLength;
	for(int i = 0,  j = 0; i < iLength && j < iOutSize; i += 3, j += 4)
	{
		outBuffer[j] = (inBuffer[i] >> 2) & 0x3F;
		outBuffer[j + 1] = ((inBuffer[i] << 4) & 0x30) | ((inBuffer[i + 1] >> 4) & 0x0F);
		outBuffer[j + 2] = ((inBuffer[i + 1] << 2) & 0x3c) | ((inBuffer[i + 2] >> 6) & 0x03);
		outBuffer[j + 3] = inBuffer[i + 2] & 0x3F;
	}
	if(iFixedLength != 0)
	{
		char cTempBuffer[3] = {0};
		memcpy(cTempBuffer, &inBuffer[iLength], iFixedLength);
		outBuffer[iOutSize - 4] = (cTempBuffer[0] >> 2) & 0x3F;
		outBuffer[iOutSize - 3] = ((cTempBuffer[0] << 4) & 0x30) | ((cTempBuffer[1] >> 4) & 0x0F);
		outBuffer[iOutSize - 2] = ((cTempBuffer[1] << 2) & 0x3c) | ((cTempBuffer[2] >> 6) & 0x03);
		outBuffer[iOutSize - 1] = cTempBuffer[2] & 0x3F;
	}
	for(int i = 0; i < iOutSize; i++)
	{
		outBuffer[i] = BASE64_STRING[outBuffer[i]];
	}
	if(iFixedLength != 0)
	{
		for(int i = 0; i < 3 - iFixedLength; i++)
		{
			outBuffer[iOutSize - 1 - i] = '='; 
		}
	}
	outBuffer[iOutSize] = 0;
}

static void base64_decode(const char * inBuffer, const int iInSize, char * & outBuffer, int & iOutSize)
{
	if(inBuffer == NULL)
	{
		outBuffer = NULL;
		iOutSize = -1;
		return;
	}
	if(iInSize == 0)
	{
		outBuffer = new char[1];
		outBuffer[0] = '\0';
		iOutSize = 0;
		return;
	}
	if(outBuffer != NULL)
	{
		delete(outBuffer);
	}
	char cBase64DecodeMap[256] = {-1};
	
	//create decode char map
	for(int i = 0; i < 64; i++)
	{
		cBase64DecodeMap[BASE64_STRING[i]] = i;
	}

	int iLength = iInSize;
	int iFixedLength = 0;
	for(int i = 1; i < 3; i++)
	{
		if(inBuffer[iInSize - i] == '=')
		{
			iFixedLength++;
		}
	}
	iLength -= iFixedLength;
	char * pEncodeBuffer = new char[iLength];
	memcpy(pEncodeBuffer, inBuffer, sizeof(char) * iLength);

	for(int i = 0; i < iLength; i++)
	{
		pEncodeBuffer[i] = cBase64DecodeMap[inBuffer[i]];
	}

	iOutSize = iInSize * 3 / 4 - iFixedLength;
	outBuffer = new char[iOutSize];
	for(int i = 0, j = 0; i < iOutSize - 2 && j < iLength - 3; i += 3, j += 4)
	{
		outBuffer[i] = ((pEncodeBuffer[j] << 2) & 0xfc) | ((pEncodeBuffer[j + 1] >> 4) & 0x03);
		outBuffer[i + 1] = ((pEncodeBuffer[j + 1] << 4) & 0xf0) | ((pEncodeBuffer[j + 2] >> 2) & 0x0F);
		outBuffer[i + 2] = ((pEncodeBuffer[j + 2] << 6) & 0xc0) | (pEncodeBuffer[j + 3] & 0x3f);
	}
	if(iFixedLength > 0)
	{
		if(iFixedLength <= 2)
		{
			outBuffer[iOutSize - (3 - iFixedLength)] = ((pEncodeBuffer[iLength - 2 - (2 - iFixedLength)] << 2) & 0xfc) | ((pEncodeBuffer[iLength - (3 - iFixedLength)] >> 4) & 0x03);
		}
		if(iFixedLength <= 1)
		{
			outBuffer[iOutSize - 1] = ((pEncodeBuffer[iLength - 2] << 4) & 0xf0) | ((pEncodeBuffer[iLength - 1] >> 2) & 0x0F);
		}
	}
	delete pEncodeBuffer;
}

/**
 * The abstract basic class of game state.
 * <p>
 *
 * @version 0.0.1
 *
 */
//class GLLiveState : public MenuItemObserver, public GLXPlayerObserver, public GLXPlayerChatObserver, public GLXPlayerChatRoomObserver
class GLLiveState : public GLXPlayerObserver
{

public:
	GLLiveState( GLLiveStateHandler * handler, bool cancel  = true);

	/**
	 * Event handler.
     *
	 */
	virtual bool HandleEvent( _TOUCH_EVENT event );

	/**
	 * Every frame, you have a chance to switch to another game state.
	 *
	 */
    virtual void SwitchState() = 0;

	/**
	 * Drawing.
	 *
	 */
    virtual void Paint();

	/**
	 * Do your logic process at this time.
     *
	 */
	virtual void UpdateAnimation();

	/**
	 * Get the state type.
     *
	 */
	virtual DEF::GAME_STATE_ENUM State() = 0;

	virtual bool IsProcess() = 0;

	virtual void OnMenuItemClicked( int id );

// GLXPlayerObserver interface implement
	virtual void OnNetworkError();
	virtual void OnRequestTimeout( int func_id );
	virtual void OnRequestSuccess( int func_id, char * data, int data_len );
	virtual void OnRequestFailure( int func_id, int error_code );

//  Chat Observer interface

	virtual void OnChatConnected(){}

	virtual void OnChatDisconnect( int e );

	virtual void OnChatRoster( const char * jid, const char * name, const char * group, int state ){}

	virtual void OnChatRosterPresence( const char * jid, const char * name, const char * group, int state ){}

	virtual void OnChatSession( char * from ){}

	virtual void OnChatMessage( char * from, char * message ){}

	virtual void OnChatAddBuddy( const char * jid ){}

	virtual void OnChatRemoveBuddy( const char * jid ){}

	virtual void OnChatBuddyRequest( const char * jid, const char * message ){}

// Chat room observer interface
	virtual void OnChatRoomInvitation( const char * room, const char * invitor, const char * reason,
		const char * password );

	virtual void OnChatRoomMessage( const char * room, const char * nickname, const char * message );

	virtual void OnChatRoomFound( const char * room, const char * nickname ){}

	virtual void OnChatRoomPresence( const char * room_jid, const char * user_name, int state );

	virtual void OnChatRoomInvitationDecline( const char * room, const char * invitee, const char * reason ){}
	
	virtual void OnChatRoomConfigResult( const char * room, bool success ){}

// Other

	virtual void PaintLoading();

	virtual void PaintAvatar(CGLImage * img, int x, int y, int scalex = -1, int scaley = -1);

	virtual ~GLLiveState(void);

	void EnableGo(bool enable);

	void RegisterOnlineObject();

	char *GetErrorString(int errorCode);

	int CountryToIndex(char* country);

	const char* IndexToCountry(int index);

	virtual void CancelRequest();

protected:

	GLLiveStateHandler * m_handler;

	static bool CheckInput(const char *input);
	static bool CheckEmail(const char *email);

	static bool SaveFileToLocal(char *file, const char *data, int dataLen);
	static bool LoadFileFromLocal(char *file, char *& data, int &dataLen);
	static CGLImage *LoadPicFromLocal(const char *picName);
	static bool SavePicToLocal(const char *picName, const char *data, int dataLen);

	//Ptr to online object 
	static GLXPlayerSereverConfig* m_gl_serverConfig;
	static GLXPlayerLogin        * m_gl_login;
	static GLXPlayerUser         * m_gl_user;
	static GLXPlayerMessage      * m_gl_message;
	static GLXPlayerUserFriend   * m_gl_userFriend;
	static GLXPlayerChat		 * m_gl_chat;

	static int m_instanceCount;
	//static UserInfo              m_loginUser;
	static int                   m_frameCount;
	static bool                  m_bAutoLogin;
	static int                   m_iKeepFailedCount;

	GLLiveMenu                      * m_commandBar;               //CommandBar object as a menu

	CGLLiveMessageBox               * m_pMsgBox;
};

#endif // __GAME_STATE_H__