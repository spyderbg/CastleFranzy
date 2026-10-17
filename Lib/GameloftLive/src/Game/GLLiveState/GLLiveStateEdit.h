#pragma once
#include "GLLiveState.h"

#ifdef WIN32
#include "GLLiveInputBox.h"
#endif

class GLLiveStateEdit :
	public GLLiveState
{
public:
	GLLiveStateEdit(GLLiveStateHandler * handler, char *pTitle, char *pText, bool isPwd = false, unsigned int maxLen = 64, bool isView = false);

	GLLiveStateEdit(GLLiveStateHandler *handler, char *pText, GLLiveState *pLastState);

	virtual ~GLLiveStateEdit(void);

	bool HandleEvent( _TOUCH_EVENT event );

    void SwitchState();

	DEF::GAME_STATE_ENUM State(){return DEF::GAME_STATE_EDIT;}
	bool IsProcess(){return false;}
 
    void Paint();

	void UpdateAnimation();

	void OnMenuItemClicked( int id );
private:
#ifdef WIN32
	GLLiveInputBox *m_inputBox;
#else
	void *m_inputBox;
#endif	
	char     *m_text;
	char     *m_title;
	char     *m_targetText;

	unsigned int m_maxText;
	bool     m_bIsPwd;
	
	GLLiveMenu    *m_menu;
	bool	m_orientation;
	
	GLLiveState *m_pLastState;
};
