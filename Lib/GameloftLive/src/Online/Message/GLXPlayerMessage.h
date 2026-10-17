#ifndef __GL_XPLAYER_MESSAGE_H__
#define __GL_XPLAYER_MESSAGE_H__

#include "../Common/GLXPlayerWebComponent.h"
#include "memory.h"

/**
* Class GLXPlayerMessage. 
* Class GLXPlayerMessage is designed for sending and receiving messages, including game message and user message.
*
* @author  World Online Team <<online-adm@gameloft.com>>
* @author  Tran Thai Phi <<phi.tranthai@gameloft.com>>
 * @version 1.0.0 
 *  
 */
namespace gllive
{
	class GLXPlayerMessage : public GLXPlayerWebComponent
	{

	public:

		/**
		* GLXPlayerMessage constructor.
		* Create an instance of GLXPlayerMessage.
		*/
		GLXPlayerMessage(void);
		
		/**
		* GLXPlayerMessage destructor.
		* Destroy an instance of GLXPlayerMessage.
		*/
		virtual ~GLXPlayerMessage(void);	
		
		/**
		* Function SendGetMessageCount.
		* Get user's  messages count.
		* @return Success: |r|s|messageType^totalCount^newCount^readedCount|... / Fail:  |r|e|error code
		*/
		bool SendGetMessageCount(int msgType = -1, int readType =  USER_MESSAGE_READTYPE_NEW);	
		
		/**
		* Function SendGetMessageHeader.
		* Get user's message headers list.
		* @param msgType Message type: Game message, User message.
		* @param readType Message read type: All, New, Read.
		* @param startIndex Start index to read.
		* @param count Read count, 0 is no limit.
		* @return Success: |r|s|c|total count|l|message headers list / Fail:  |r|e|error code
		*/
		bool SendGetMessageHeader(int msgType = -1, int readType =  USER_MESSAGE_READTYPE_NEW , int startIndex = 0, int count = 0);
		
		/**
		* Function SendReadMessage.
		* Read user's messages by messageId.
		* @param msgID Id of message to read.
		* @return Success: |r|s|message body / Fail:  |r|e|error code
		*/
		bool SendReadMessage( int msgID );	

		/**
		* Function SendReadMySentMessage.
		* Read user's sent messages by messageId.
		* @param msgID Id of message to read.
		* @return Success: |r|s|message body / Fail:  |r|e|error code
		*/
		bool SendReadMySentMessage( int msgID );
		
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
		bool SendOnlineMessage( char * receiver, int type, char * title, char * body, int body_length, bool is_userNumber = false );
	#else
		bool SendOnlineMessage( char * receiver, int type, char * title, char * body, int body_length);
	#endif
		/**
		* Function SendDeleteMessage.
		* Delete user's messag indicated by msgID. Can't delete the message which sended to ALL.
		* @param msgID Id of message to be deleted.
		* @return Success: |r|s / Fail:  |r|e|error code
		*/
		bool SendDeleteMessage( int msgID );

		/**
		* Function SendDeleteMySentMessage.
		* Delete user's sent messag indicated by msgID.
		* @param msgID Id of message to be deleted.
		* @return Success: |r|s / Fail:  |r|e|error code
		*/
		bool SendDeleteMySentMessage( int msgID );

		/**
		* Function SendGetMySentMessageCount
		* Get my sent messages' count
		*
		* @return Success: |r|s / Fail:  |r|e|error code
		*/
		bool SendGetMySentMessageCount();

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
		bool SendGetMySentMessageHeader(int msgType = -1, int startIndex = 0, int count = 0);


		/**
		* Function getMsgListCount.
		* Count messages in the message list.
		* @return Number of messages in the message list.
		*/
		int getMsgListCount();
		
		/**
		* Function getMsgCount.
		* Count messages in the message list by message type and read type.
		* @param msgType Message type: Game message, User message.
		* @param readType Message read type: All, New, Read..
		* @return Number of messages.
		*/
		int getMsgCount(int msgType, int readType);
		
		/**
		* Function getMsgID.
		* Return ID of message in the message list, indicated by position.
		* @param position Index of message in the list.	
		* @return Message ID.
		*/
		int getMsgID(int position);	
		
		/**
		* Function getMsgType.
		* Return type of message in the message list, indicated by position.
		* @param position Index of message in the list.	
		* @return Message type.
		*/
		int getMsgType(int position);
		
		/**
		* Function getMsgTitle.
		* Return subject of message in the message list, indicated by position.
		* @param position Index of message in the list.	
		* @return Message title.
		*/
		char* getMsgTitle(int position);
		
		/**
		* Function getMsgSenderNumber.
		* Return the sender of message in the message list, indicated by position.
		* @param position Index of message in the list.	
		* @return Message sender.
		*/
		char* getMsgSenderNumber(int position);	
		
		/**
		* Function getMsgSender.
		* Return the sender of message in the message list, indicated by position.
		* @param position Index of message in the list.	
		* @return Message sender.
		*/
		char* getMsgSender(int position);
		
		/**
		* Function getMsgCreatedTime.
		* Return the time created of message in the message list, indicated by position.
		* @param position Index of message in the list.	
		* @return Message's created time.
		*/
		char* getMsgCreatedTime(int position);	
		
		/**
		* Function getMsgReadState.
		* Return state (readed or not) of message in the message list, indicated by position.
		* @param position Index of message in the list.	
		* @return Message read state.
		*/
		bool getMsgReadState(int position);
		
		/**
		* Function getCurrentMsgID.
		* Return current message ID.
		* @return current message ID.
		*/
		int getCurrentMsgID();
		
		/**
		* Function getCurrentMsgPosition.
		* Return current message position in the list.	
		* @return current message position.
		*/
		int getCurrentMsgPosition();
		
		/**
		* Function getMsgBody.
		* Return current message body.	
		* @return current message body
		*/
		char* getMsgBody();
		
	protected:
		/**
		* Private function OnUpdateSuccess.
		* Handle success message returned from the Web server.	
		* @param func_id Function ID.
		* @return True / False.
		*/
		bool OnUpdateSuccess( int func_id );

		/**
		* Private function processMsgHeaderList.
		* Process message header list if success message returned from the server.
		* @see SendGetMessageHeader()	
		* @param msgString Data returned from the server.
		*/
		void processMsgHeaderList(char* msgString);

		/**
		* Private function processSentMsgHeaderList.
		* Process message header list if success message returned from the server.
		* @see SendGetMessageHeader()	
		* @param msgString Data returned from the server.
		*/
		void processSentMsgHeaderList(char* msgString);

		/**
		* Private function processMsgCount.
		* Process message count if success message returned from the server.
		* @see SendGetMessageCount()	
		* @param msgString Data returned from the server.
		*/
		void processMsgCount(char* msgString);

	private:	
		/**
		* Private function clearMessageList.
		* Clear message list.	
		*/
		void clearMessageList();
		
		/**
		* Private function clearMsgCount.
		* Reset message count.	
		*/
		void clearMsgCount();
		
		/**
		* Variable m_msgId.
		* List of message ID. 
		*/
		int * m_msgId;
		
		/**
		* Variable m_msgTitle.
		* List of message title. 
		*/
		char** m_msgTitle;
		
		/**
		* Variable m_msgSenderNumber.
		* List of message sender number. 
		*/
		char** m_msgSenderNumber;
		
		/**
		* Variable m_msgSender.
		* List of message sender. 
		*/
		char** m_msgSender;
		
		/**
		* Variable m_msgCreateTime.
		* List of message created time. 
		*/
		char** m_msgCreateTime;
		
		/**
		* Variable m_msgReadState.
		* List of message read state. 
		*/
		bool * m_msgReadState;
		
		/**
		* Variable m_msgType.
		* List of message type.
		*/
		int* m_msgType;
		
		/**
		* Variable m_MsgListCount.
		* Count number of message in the list. 
		*/
		int m_MsgListCount;
		
		/**
		* Variable m_currentMsgID.
		* Current message ID to read.
		*/
		int m_currentMsgID;
		
		/**
		* Variable m_msgBody.
		* Current message body.
		*/
		char* m_msgBody;	
		
		/**
		* Variable m_msgCount.
		* Array to store message count. Currently, we have 5 type and 3 read type.
		* @see SendGetMessageCount()
		* @see processMsgCount()
		* @see getMsgCount()
		*/
		int m_msgCount[6][3];
	};
}
#endif // __GL_XPLAYER_MESSAGE_H__
