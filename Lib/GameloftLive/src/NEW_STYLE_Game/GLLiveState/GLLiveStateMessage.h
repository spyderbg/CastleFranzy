#pragma once

#include "GLLiveState.h"
#include "GLLiveMenu.h"

#define WIRTE_MSG_MAX_LEN    180


class GLLiveStateMessage : public GLLiveState
{
public:
	enum MessageSubState
	{	
		MESSAGE_BOX,
		MESSAGE_READ,
		MESSAGE_WRITE,
		MESSAGE_SENDBOX,
		MESSAGE_PROCESS,	
		MESSAGE_ERROR		
	};

	GLLiveStateMessage(GLLiveStateHandler * handler, char * buddy_name = NULL);
	~GLLiveStateMessage(void);

	void OnRequestSuccess( int func_id, char * data, int data_len );
	//void OnNetworkError();
	//void OnRequestTimeout( int func_id );
	void OnRequestFailure( int func_id, int error_code );
	bool HandleEvent( _TOUCH_EVENT event );
	void OnMenuItemClicked( int id );
	void SwitchState(){};
	void UpdateAnimation();
	DEF::GAME_STATE_ENUM State(){return DEF::GAME_STATE_MESSAGE;}
	bool IsProcess(){return m_sub_state == MESSAGE_PROCESS;}
	void Paint();
	void PaintMessageBox();
	void PaintMessageRead();
	void PaintMessageWrite();
	void PaintMessageSendBox();
	void PaintHScoreBar();
	static void checkHttpLink(const char* src, int& index1, int& index2);
	bool IsTouchInLinkArea(int x, int y);
	void GetDateTimeStrByLang(const char *in, char *out, int outLen, int lang = -1);
private:
	void ChangeOffset(int newOffset);
	int GetNewMsgCount();

	void ChangeMsgBoxOffset(int newOffset,int curLines,int maxLines,CGLLiveFont* f2,int BarMaxHeight);

	MessageSubState	m_sub_state;
	MessageSubState m_last_state;
	int				m_selected;
	char			m_receiver[MAX_USERNAME];
	char *			m_buddy_name;
	char			m_title[32];
	char			m_body[WIRTE_MSG_MAX_LEN];
	char            m_showBodyBuffer[WIRTE_MSG_MAX_LEN + 1];

	int    m_lineHeight;
	//visible area
	int    m_startY;
	int    m_height;

	//Scroll Bar
	int    m_scrollBarH;
	int    m_scrollBarPos;
	//Animation data
	int        m_offset;
	int        m_oldOffset;
	int        m_touchPos;         //Y-
	bool       m_moving;
	bool       m_moved;
	int        m_speed;
	bool       m_scrollMoving;
	bool	m_isMsgActived;


	GLLiveMenu  *m_listMenu;
	GLLiveMenu  *m_readMenu;
	GLLiveMenu  *m_writeMenu;
	GLLiveMenu  *m_boxMenu;
	GLLiveMenu  *m_sendBoxMenu;

	bool      m_deleteConfirm;
	bool      m_delAllConfirm;
	bool      m_sendingMsg;
	bool      m_readingMsg;
	bool      m_readingSentMsg;

	int		  m_nLines;
	int			m_linkIndex1;
	int			m_linkIndex2;
	char*		m_link;
	int			m_linkPosition[4];//start x,y, end x,y
	bool		m_willGoToLink;
};