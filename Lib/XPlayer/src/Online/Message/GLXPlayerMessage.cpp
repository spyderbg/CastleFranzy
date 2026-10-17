#include "../../../include/Message/GLXPlayerMessage.h"

/**
* GLXPlayerMessage constructor.
* Create an instance of GLXPlayerMessage.
*/
GLXPlayerMessage::GLXPlayerMessage() : GLXPlayerWebComponent()
{
	LoadConfig();
	m_http_engine = XP_API_NEW GLXPlayerHttp( m_http_server_address, m_http_php_version, m_http_php_address );
	
	m_msgId = NULL;
	m_msgType = NULL;
	m_msgTitle = NULL;
	m_msgSenderNumber = NULL;
	m_msgSender = NULL;
	m_msgDesUserNumber = NULL;
	m_msgDesUserName = NULL;
	m_msgCreateTime = NULL;
	m_msgReadState = NULL;
	m_msgBody = NULL;
	m_MsgListCount = 0;
	m_currentMsgID = -1;
	m_mySentMsgCount = 0;
	clearMsgCount();
}

/**
* GLXPlayerMessage destructor.
* Destroy an instance of GLXPlayerMessage.
*/

GLXPlayerMessage::~GLXPlayerMessage(void)
{
	clearMessageList();	
	XP_API_DEL(m_msgBody);
}

/**
* Private function OnUpdateSuccess.
* Handle success message returned from the Web server.	
* @param func_id Function ID.
* @return True / False.
*/
bool GLXPlayerMessage::OnUpdateSuccess( int func_id )
{
	switch(func_id)
	{
		case FUNCTION_GET_MESSAGE_COUNT:		
			processMsgCount(m_pResponsePtr);			
			break;
		case FUNCTION_GET_MY_SENT_MESSAGE_COUNT:
			{
				char* tmp = new char [16];
				XP_API_MEMSET(tmp, 0, 16);
				getValue(m_pResponsePtr, tmp, 0, '|');
				m_mySentMsgCount = XP_API_ATOI(tmp);
				XP_API_DEL(tmp);
			}			
			break;
		case FUNCTION_GET_MESSAGE_HEADER:
			processMsgHeaderList(m_pResponsePtr);
			break;
		case FUNCTION_GET_MY_SENT_MESSAGE_HEADER:
			processMySentMsgHeaderList(m_pResponsePtr);
			break;
		case FUNCTION_READ_MESSAGE:
		case FUNCTION_READ_MY_SENT_MESSAGE:
			XP_API_DEL(m_msgBody);
			m_msgBody = XP_API_STRNEW(m_pResponsePtr);
			break;
			
		default:			
			break;
	}
	return GLXPlayerWebComponent::OnUpdateSuccess( func_id );
}

/**
* Function getMsgListCount.
* Count messages in the message list.
* @return Number of messages in the message list.
*/
int GLXPlayerMessage::getMsgListCount()
{
	return m_MsgListCount;
}

/**
* Function getMsgCount.
* Count messages in the message list by message type and read type.
* @param msgType Message type: Game message, User message.
* @param readType Message read type: All, New, Read..
* @return Number of messages.
*/
int GLXPlayerMessage::getMsgCount(int msgType, int readType)
{	
	if(readType < 0 || readType >= 3)
	{
		return -1;
	}
	
	if(msgType == USER_MESSAGE_TYPE_GAMEMESSAGE)
	{
		return m_msgCount[0][readType];			
	}
	else if(msgType == USER_MESSAGE_TYPE_USERMESSAGE)
	{
		return m_msgCount[1][readType];
	}
	else if(msgType == USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION)
	{
		return m_msgCount[2][readType];
	}
	else if(msgType == USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION)
	{
		return m_msgCount[3][readType];
	}
	else if(msgType == USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION)
	{
		return m_msgCount[4][readType];
	}	
	
	return -1;	
}

/**
* Function getMySentMsgCount.
* Get number of my sent messages.
* @return m_mySentMsgCount.
*/
int GLXPlayerMessage::getMySentMsgCount()
{	
	return m_mySentMsgCount;
}

/**
* Function getMsgID.
* Return ID of message in the message list, indicated by position.
* @param position Index of message in the list.	
* @return Message ID.
*/
int GLXPlayerMessage::getMsgID(int position)
{
	if(m_msgId == NULL)
	{
		return -1;
	}
	return m_msgId[position];
}

/**
* Function getMsgTitle.
* Return subject of message in the message list, indicated by position.
* @param position Index of message in the list.	
* @return Message title.
*/
char* GLXPlayerMessage::getMsgTitle(int position)
{
	if(m_msgTitle == NULL)
	{
		return NULL;
	}
	return m_msgTitle[position];
}

/**
* Function getMsgSenderNumber.
* Return the sender of message in the message list, indicated by position.
* @param position Index of message in the list.	
* @return Message sender.
*/
char* GLXPlayerMessage::getMsgSenderNumber(int position)
{
	if(m_msgSenderNumber == NULL)
	{
		return NULL;
	}
	return m_msgSenderNumber[position];
}

/**
* Function getMsgSender.
* Return the sender of message in the message list, indicated by position.
* @param position Index of message in the list.	
* @return Message sender.
*/
char* GLXPlayerMessage::getMsgSender(int position)
{
	if(m_msgSender == NULL)
	{
		return NULL;
	}
	return m_msgSender[position];
}

/**
* Function getMsgDesUserNumberList.
* Return the list of receiver's number, indicated by position.
* @param position Index of message in the list.	
* @return m_msgDesUserNumber[position];
*/
char* GLXPlayerMessage::getMsgDesUserNumberList(int position)
{
	if(m_msgDesUserNumber == NULL)
	{
		return NULL;
	}
	return m_msgDesUserNumber[position];
}

/**
* Function getMsgDesUserNameList.
* Return the list of receiver's name, indicated by position.
* @param position Index of message in the list.	
* @return m_msgDesUserName[position];
*/
char* GLXPlayerMessage::getMsgDesUserNameList(int position)
{
	if(m_msgDesUserName == NULL || position < 0 || position > m_MsgListCount)
	{
		return NULL;
	}
	return m_msgDesUserName[position];
}

/**
* Function getMsgDesUserNumber
* Return the receiver's number in the list, indicated by listPosition and position.
* @param listPosition Position of user number list in the header list.	
* @param position Position of user number.	
* @return  Des user number ;
*/
char* GLXPlayerMessage::getMsgDesUserNumber(int listPosition, int position)
{
	if(m_msgDesUserNumber == NULL)
	{
		return NULL;
	}
	char* usernumber;
	usernumber = XP_API_NEW char[16];
	XP_API_MEMSET(usernumber, 0, 16);
	getValue(m_msgDesUserNumber[listPosition], usernumber, position, ',');
	return usernumber;	
}

/**
* Function getMsgDesUserName.
* Return the receiver's name in the list, indicated by listPosition and position.
* @param listPosition Position of user name list in the header list.	
* @param position Position of user name.	
* @return Des user name.
*/
char* GLXPlayerMessage::getMsgDesUserName(int listPosition, int position)
{
	if(m_msgDesUserName == NULL)
	{
		return NULL;
	}
	char* username;
	username = XP_API_NEW char[16];
	XP_API_MEMSET(username, 0, 16);
	getValue(m_msgDesUserName[listPosition], username, position, ',');
	return username;
}

/**
* Function getMsgCreatedTime.
* Return the time created of message in the message list, indicated by position.
* @param position Index of message in the list.	
* @return Message's created time.
*/
char* GLXPlayerMessage::getMsgCreatedTime(int position)
{
	if(m_msgCreateTime == NULL)
	{
		return NULL;
	}
	return m_msgCreateTime[position];
}

/**
* Function getMsgReadState.
* Return state (readed or not) of message in the message list, indicated by position.
* @param position Index of message in the list.	
* @return Message read state.
*/
bool GLXPlayerMessage::getMsgReadState(int position)
{
	if(m_msgReadState == NULL)
	{
		return NULL;
	}
	return m_msgReadState[position];
}

/**
* Function getMsgBody.
* Return current message body.	
* @return current message body
*/
char* GLXPlayerMessage::getMsgBody()
{
	return m_msgBody;
}

/**
* Function getCurrentMsgID.
* Return current message ID.
* @return current message ID.
*/
int GLXPlayerMessage::getCurrentMsgID()
{
	return m_currentMsgID;
}

/**
* Function getCurrentMsgPosition.
* Return current message position in the list.	
* @return current message position.
*/
int GLXPlayerMessage::getCurrentMsgPosition()
{
	for(int i=0; i < getMsgListCount(); i++) 
	{
		if(getMsgID(i) == getCurrentMsgID())
		{
			return i;
		}
	}
	return -1;
}

/**
* Function getMsgType.
* Return type of message in the message list, indicated by position.
* @param position Index of message in the list.	
* @return Message type.
*/
int GLXPlayerMessage::getMsgType(int position)
{
	if(m_msgType == NULL)
	{
		return -1;
	}
	return m_msgType[position];
}

/**
* Function SendGetMessageCount.
* Get user's  messages count.
* @return Success: |r|s|messageType^totalCount^newCount^readedCount|... / Fail:  |r|e|error code
*/
bool GLXPlayerMessage::SendGetMessageCount(int msgType, int readType)
{
	char buffer[ONLINE_MAX_PACKET_SIZE];
	char type[64];
	int index = 0;

	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_MEMSET( type, 0, 64 );
	
	if(msgType != -1)
	{
		if ( ( msgType & USER_MESSAGE_TYPE_GAMEMESSAGE ) != 0 )
		{
			int len_old = 0;
			int len_new = 0;

			len_old = XP_API_STRLEN( type );
			XP_API_SPRINTF( type+index, "%d,", USER_MESSAGE_TYPE_GAMEMESSAGE );
			len_new = XP_API_STRLEN( type );

			index += ( len_new - len_old );
		}

		if ( ( msgType & USER_MESSAGE_TYPE_USERMESSAGE ) != 0 )
		{
			int len_old = 0;
			int len_new = 0;

			len_old = XP_API_STRLEN( type );
			XP_API_SPRINTF( type+index, "%d,", USER_MESSAGE_TYPE_USERMESSAGE );
			len_new = XP_API_STRLEN( type );

			index += ( len_new - len_old );
		}

		if ( ( msgType & USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION ) != 0 )
		{
			int len_old = 0;
			int len_new = 0;

			len_old = XP_API_STRLEN( type );
			XP_API_SPRINTF( type+index, "%d,", USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION );
			len_new = XP_API_STRLEN( type );

			index += ( len_new - len_old );
		}

		if ( ( msgType & USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION ) != 0 )
		{
			int len_old = 0;
			int len_new = 0;

			len_old = XP_API_STRLEN( type );
			XP_API_SPRINTF( type+index, "%d,", USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION );
			len_new = XP_API_STRLEN( type );

			index += ( len_new - len_old );
		}

		if ( ( msgType & USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION ) != 0 )
		{
			int len_old = 0;
			int len_new = 0;

			len_old = XP_API_STRLEN( type );
			XP_API_SPRINTF( type+index, "%d,", USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION );
			len_new = XP_API_STRLEN( type );

			index += ( len_new - len_old );
		}

		if ( XP_API_STRLEN( type ) > 0 )
		{
			type[ XP_API_STRLEN( type ) - 1 ] = '\0';
		}	
		XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|t|%s|r|%d|", FUNCTION_GET_MESSAGE_COUNT, m_ggi, m_uid, type, readType);
	}
	else
	{
		XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_MESSAGE_COUNT, m_ggi, m_uid);
	}
	XP_DEBUG_OUT("[SendGetMessageCount] %s\n", buffer);

	return SendByGet( buffer );
}

/**
* Function SendGetMessageHeader.
* Get user's message headers list.
* @param msgType Message type: Game message, User message.
* @param readType Message read type: All, New, Read.
* @param startIndex Start index to read.
* @param count Read count, 0 is no limit.
* @return Success: |r|s|c|total count|l|message headers list / Fail:  |r|e|error code
*/
bool GLXPlayerMessage::SendGetMessageHeader(int msgType, int readType, int startIndex, int count )
{
	char buffer[ONLINE_MAX_PACKET_SIZE];
	char type[64];
	int index = 0;

	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_MEMSET( type, 0, 64 );
	
	if(msgType != -1)
	{
		if ( ( msgType & USER_MESSAGE_TYPE_GAMEMESSAGE ) != 0 )
		{
			int len_old = 0;
			int len_new = 0;

			len_old = XP_API_STRLEN( type );
			XP_API_SPRINTF( type+index, "%d,", USER_MESSAGE_TYPE_GAMEMESSAGE );
			len_new = XP_API_STRLEN( type );

			index += ( len_new - len_old );
		}

		if ( ( msgType & USER_MESSAGE_TYPE_USERMESSAGE ) != 0 )
		{
			int len_old = 0;
			int len_new = 0;

			len_old = XP_API_STRLEN( type );
			XP_API_SPRINTF( type+index, "%d,", USER_MESSAGE_TYPE_USERMESSAGE );
			len_new = XP_API_STRLEN( type );

			index += ( len_new - len_old );
		}

		if ( ( msgType & USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION ) != 0 )
		{
			int len_old = 0;
			int len_new = 0;

			len_old = XP_API_STRLEN( type );
			XP_API_SPRINTF( type+index, "%d,", USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION );
			len_new = XP_API_STRLEN( type );

			index += ( len_new - len_old );
		}

		if ( ( msgType & USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION ) != 0 )
		{
			int len_old = 0;
			int len_new = 0;

			len_old = XP_API_STRLEN( type );
			XP_API_SPRINTF( type+index, "%d,", USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION );
			len_new = XP_API_STRLEN( type );

			index += ( len_new - len_old );
		}

		if ( ( msgType & USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION ) != 0 )
		{
			int len_old = 0;
			int len_new = 0;

			len_old = XP_API_STRLEN( type );
			XP_API_SPRINTF( type+index, "%d,", USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION );
			len_new = XP_API_STRLEN( type );

			index += ( len_new - len_old );
		}

		if ( XP_API_STRLEN( type ) > 0 )
		{
			type[ XP_API_STRLEN( type ) - 1 ] = '\0';
		}

		XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|t|%s|r|%d|a|%d|c|%d|", FUNCTION_GET_MESSAGE_HEADER, m_ggi, m_uid, type, readType, startIndex, count);
	}
	else
	{
		XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|r|%d|a|%d|c|%d|", FUNCTION_GET_MESSAGE_HEADER, m_ggi, m_uid, readType, startIndex, count);
	}
	XP_DEBUG_OUT("[SendGetMessageHeader] %s\n", buffer);

	return SendByGet( buffer );
}

/**
* Function SendReadMessage.
* Read user's messages by messageId.
* @param msgID Id of message to read.
* @return Success: |r|s|message body / Fail:  |r|e|error code
*/
bool GLXPlayerMessage::SendReadMessage( int msgID )
{
	m_currentMsgID = msgID;
	
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|m|%d|", FUNCTION_READ_MESSAGE, m_ggi, m_uid, m_currentMsgID);
	XP_DEBUG_OUT("[SendReadMessage] %s\n", buffer);

	return SendByGet( buffer );
}

/**
* Function SendReadMySentMessage.
* Read my sent message by messageId.
* @param msgID Id of message to read.
* @return Success: |r|s|message body / Fail:  |r|e|error code
*/
bool GLXPlayerMessage::SendReadMySentMessage( int msgID )
{
	m_currentMsgID = msgID;
	
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|m|%d|", FUNCTION_READ_MY_SENT_MESSAGE, m_ggi, m_uid, m_currentMsgID);
	XP_DEBUG_OUT("[SendReadMySentMessage] %s\n", buffer);

	return SendByGet( buffer );
}

/**
* Function SendOnlineMessage.
* Send message to another user. Can not send GameMessage.
* @param receiver Name of receiver.
* @param type User message, Invite message.
* @param title Subject of the message.
* @param body Content of the message.
* @param body_length length of message body.
* @return Success: |r|s / Fail:  |r|e|error code
*/
#ifdef USE_USER_NUMBER
bool GLXPlayerMessage::SendOnlineMessage( char * receiver, int type, char * title, char * body, int body_length , bool isUserNumber)
{
	if ( NULL == receiver || NULL == title || NULL == body || 0 == body_length || type == USER_MESSAGE_TYPE_GAMEMESSAGE)
	{
		m_observer->OnRequestFailure( FUNCTION_SEND_MESSAGE, ERROR_INIT );
		return false;
	}

	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	
	if(isUserNumber)
	{
		XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|t|%d|l|%s|o|%s|un|%s|", FUNCTION_SEND_MESSAGE, m_ggi, m_uid, type, title, body, receiver );
	}
	else
	{
		XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|t|%d|l|%s|o|%s|n|%s|", FUNCTION_SEND_MESSAGE, m_ggi, m_uid, type, title, body, receiver );
	}
	XP_DEBUG_OUT("[SendOnlineMessage] %s\n", buffer);
	return SendByGet( buffer );	
}
#else
bool GLXPlayerMessage::SendOnlineMessage( char * receiver, int type, char * title, char * body, int body_length)
{
	if ( NULL == receiver || NULL == title || NULL == body || 0 == body_length || type == USER_MESSAGE_TYPE_GAMEMESSAGE)
	{
		m_observer->OnRequestFailure( FUNCTION_SEND_MESSAGE, ERROR_INIT );
		return false;
	}

	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|t|%d|l|%s|o|%s|n|%s|", FUNCTION_SEND_MESSAGE, m_ggi, m_uid, type, title, body, receiver );

	XP_DEBUG_OUT("[SendOnlineMessage] %s\n", buffer);
	return SendByGet( buffer );	
}
#endif

/**
* Function SendDeleteMessage.
* Delete user's messag indicated by msgID. Can't delete the message which sended to ALL.
* @param msgID Id of message to be deleted.
* @return Success: |r|s / Fail:  |r|e|error code
*/
bool GLXPlayerMessage::SendDeleteMessage( int msgID )
{	
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|m|%d|", FUNCTION_DEL_MESSAGE, m_ggi, m_uid, msgID);
	XP_DEBUG_OUT("[SendDeleteMessage] %s\n", buffer);

	return SendByGet( buffer );
}

/**
* Function SendDelMySentMessage.
* Delete sent message indicated by msgID. Can't delete the message which sended to ALL.
* @param msgID Id of message to be deleted. If msgID < 0, delete all messages.
* @return Success: |r|s / Fail:  |r|e|error code
*/
bool GLXPlayerMessage::SendDelMySentMessage( int msgID )
{	
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|m|%d|", FUNCTION_DEL_MY_SENT_MESSAGE, m_ggi, m_uid, msgID);
	XP_DEBUG_OUT("[SendDelMySentMessage] %s\n", buffer);

	return SendByGet( buffer );
}


/**
* Function SendGetMySentMessageCount
* Get my sent messages' count
*
* @return Success: |r|s / Fail:  |r|e|error code
*/
bool GLXPlayerMessage::SendGetMySentMessageCount()
{	
	char buffer[ONLINE_MAX_PACKET_SIZE];
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );
	XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|", FUNCTION_GET_MY_SENT_MESSAGE_COUNT, m_ggi, m_uid);
	XP_DEBUG_OUT("[GLXPlayerMessage::SendGetMySentMessageCount] %s\n", buffer);

	return SendByGet( buffer );
}

/**
* Function SendGetMySentMessageHeader.
* Get my sent messages' headers
* 
* @param msgType Message type: Game message, User message.
* @param readType Message read type: All, New, Read.
* @param startIndex Start index to read.
* @param count Read count, 0 is no limit.
* @return Success: |r|s|c|total count|l|message headers list / Fail:  |r|e|error code
*/
bool GLXPlayerMessage::SendGetMySentMessageHeader(int startIndex, int count )
{
	char buffer[ONLINE_MAX_PACKET_SIZE];	
	XP_API_MEMSET( buffer, 0, ONLINE_MAX_PACKET_SIZE );	
	XP_API_SPRINTF( buffer, "f|%d|i|%d|u|%s|a|%d|c|%d|", FUNCTION_GET_MY_SENT_MESSAGE_HEADER, m_ggi, m_uid, startIndex, count);	
	XP_DEBUG_OUT("[SendGetMessageHeader] %s\n", buffer);

	return SendByGet( buffer );
}

/**
* Private function clearMessageList.
* Clear message list.	
*/
void GLXPlayerMessage::clearMessageList()
{
	XP_API_DEL(m_msgId);
	XP_API_DEL(m_msgType);
	XP_API_DEL(m_msgReadState);
	XP_API_DEL_ARRAY(m_msgTitle, m_MsgListCount);
	XP_API_DEL_ARRAY(m_msgSenderNumber, m_MsgListCount);	
	XP_API_DEL_ARRAY(m_msgSender, m_MsgListCount);
	XP_API_DEL_ARRAY(m_msgDesUserNumber, m_MsgListCount);	
	XP_API_DEL_ARRAY(m_msgDesUserName, m_MsgListCount);
	XP_API_DEL_ARRAY(m_msgCreateTime, m_MsgListCount);
	m_MsgListCount = 0;
}

/**
* Private function clearMsgCount.
* Reset message count.	
*/
void GLXPlayerMessage::clearMsgCount(void)
{	
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			m_msgCount[i][j] = 0;
		}
	}
}

/**
* Private function processMsgCount.
* Process message count if success message returned from the server.
* @see SendGetMessageCount()	
* @param msgString Data returned from the server.
*/
void GLXPlayerMessage::processMsgCount(char* msgString)
{
	clearMsgCount();
	if(msgString == NULL || XP_API_STRLEN(msgString) <= 0)
	{
		return;
	}
	///@Return: f|74|r|s|0^2^0^0|1^0^0^0|2^0^0^0|3^0^0^0|4^0^0^0
	
	char tmpStr[32] = {0};
	char tmp[8] = {0};
	for(int i = 0; i < 5; i++)
	{
		XP_API_MEMSET(tmpStr, 0, 32);
		getValue(msgString, tmpStr, i, '|');
		for(int j=0; j< 3; j++)
		{
			XP_API_MEMSET(tmp, 0, 8);
			getValue(tmpStr, tmp, j + 1, '^');
			m_msgCount[i][j] = XP_API_ATOI(tmp);
		}
	}
}

/**
* Private function processMsgHeaderList.
* Process message header list if success message returned from the server.
* @see SendGetMessageHeader()	
* @param msgString Data returned from the server.
*/
void GLXPlayerMessage::processMsgHeaderList(char* msgString)
{
	clearMessageList();
	
	if(msgString == NULL || XP_API_STRLEN(msgString) <= 0)
	{
		return;
	}
	
	char tmpString[256] = {0};	
	char tmp[128] = {0};
	int count = 0;
	
	getValue(msgString, tmp, count++, '|'); //'c'

	XP_API_MEMSET(tmp, 0, 128);	
	getValue(msgString, tmp, count++, '|');//'count'	
	
	m_MsgListCount = XP_API_ATOI(tmp);
	XP_API_MEMSET(tmp, 0, 128);	
	
	int y = 0, n = 0, j = 0;
	
	
	m_msgId = XP_API_NEW int[m_MsgListCount];
	m_msgType = XP_API_NEW int[m_MsgListCount];	
	m_msgReadState = XP_API_NEW bool [m_MsgListCount];
	
	m_msgTitle = XP_API_NEW char* [m_MsgListCount];	
	m_msgSender = XP_API_NEW char* [m_MsgListCount];
	m_msgSenderNumber = XP_API_NEW char* [m_MsgListCount];
	m_msgCreateTime = XP_API_NEW char* [m_MsgListCount];
	
	XP_API_MEMSET(tmp, 0, 128);
	getValue(msgString, tmp, count++, '|');//'l'
	
	count = 0;
	
	for(int i = 0; i < m_MsgListCount; i++)
	{
		XP_API_MEMSET(tmpString, 0, 256);
		getValue(msgString, tmpString, i + 3, '|');
		
		m_msgTitle[i] = XP_API_NEW char[128];
		m_msgSender[i] = XP_API_NEW char[128];
		m_msgSenderNumber[i] = XP_API_NEW char[128];
		m_msgCreateTime[i] = XP_API_NEW char[128];		
		
		XP_API_MEMSET(m_msgTitle[i], 0, 128);		
		XP_API_MEMSET(m_msgSender[i], 0, 128);
		XP_API_MEMSET(m_msgSenderNumber[i], 0, 128);
		XP_API_MEMSET(m_msgCreateTime[i], 0, 128);

		XP_API_MEMSET(tmp, 0, 128);
		getValue(tmpString, tmp, count++, '^');
		m_msgId[i] = XP_API_ATOI(tmp);

		XP_API_MEMSET(tmp, 0, 128);		
		getValue(tmpString, tmp, count++, '^');
		m_msgType[i] = XP_API_ATOI(tmp);
		
		getValue(tmpString, m_msgTitle[i], count++, '^');
		getValue(tmpString, m_msgSender[i], count++, '^');			
		getValue(tmpString, m_msgSenderNumber[i], count++, '^');							
		getValue(tmpString, m_msgCreateTime[i], count++, '^');

		XP_API_MEMSET(tmp, 0, 128);
		getValue(tmpString, tmp, count++, '^');
		int rtmp = XP_API_ATOI(tmp);
		XP_API_MEMSET(tmp, 0, 128);

		if(rtmp != 0)
		{
			m_msgReadState[i] = true;
		}
		else
		{
			m_msgReadState[i] = false;
		}
		count = 0;
	}
}

/**
* Private function processMySentMsgHeaderList.
* Process my sent messages header list..
* @see SendGetMySentMessageHeader()	
* @param msgString Data returned from the server.
*/
void GLXPlayerMessage::processMySentMsgHeaderList(char* msgString)
{
	clearMessageList();
	
	if(msgString == NULL || XP_API_STRLEN(msgString) <= 0)
	{
		return;
	}
	
	char tmpString[256] = {0};	
	char tmp[128] = {0};
	int count = 0;
	
	getValue(msgString, tmp, count++, '|'); //'c'

	XP_API_MEMSET(tmp, 0, 128);	
	getValue(msgString, tmp, count++, '|');//'count'		
	m_MsgListCount = XP_API_ATOI(tmp);
	XP_API_MEMSET(tmp, 0, 128);	
	
	int y = 0, n = 0, j = 0;	
	
	m_msgId = XP_API_NEW int[m_MsgListCount];	
	m_msgTitle = XP_API_NEW char* [m_MsgListCount];	
	m_msgDesUserNumber = XP_API_NEW char* [m_MsgListCount];
	m_msgDesUserName = XP_API_NEW char* [m_MsgListCount];
	m_msgCreateTime = XP_API_NEW char* [m_MsgListCount];	
	XP_API_MEMSET(tmp, 0, 128);
	
	getValue(msgString, tmp, count++, '|');//'l'	
	count = 0;
	
	for(int i = 0; i < m_MsgListCount; i++)
	{
		XP_API_MEMSET(tmpString, 0, 256);
		getValue(msgString, tmpString, i + 3, '|');
		
		m_msgTitle[i] = XP_API_NEW char[128];
		m_msgDesUserNumber[i] = XP_API_NEW char[128];
		m_msgDesUserName[i] = XP_API_NEW char[128];
		m_msgCreateTime[i] = XP_API_NEW char[128];		
		
		XP_API_MEMSET(m_msgTitle[i], 0, 128);		
		XP_API_MEMSET(m_msgDesUserNumber[i], 0, 128);
		XP_API_MEMSET(m_msgDesUserName[i], 0, 128);
		XP_API_MEMSET(m_msgCreateTime[i], 0, 128);

		XP_API_MEMSET(tmp, 0, 128);
		getValue(tmpString, tmp, count++, '^');
		m_msgId[i] = XP_API_ATOI(tmp);
		
		getValue(tmpString, m_msgTitle[i], count++, '^');
		getValue(tmpString, m_msgDesUserNumber[i], count++, '^');			
		getValue(tmpString, m_msgDesUserName[i], count++, '^');							
		getValue(tmpString, m_msgCreateTime[i], count++, '^');

		count = 0;
	}
}