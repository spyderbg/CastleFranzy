#include "stdafx.h"
#include "GameState.h"


GameState::GameState( GameStateHandler * handler ) : m_handler( handler )
{
	m_UpKey.left = DEF::BUTTON_UP_X;
	m_UpKey.top = DEF::BUTTON_UP_Y;
	m_UpKey.width = DEF::BUTTON_UP_W;
	m_UpKey.height = DEF::BUTTON_UP_H;

	m_DownKey.left = DEF::BUTTON_DOWN_X;
	m_DownKey.top = DEF::BUTTON_DOWN_Y;
	m_DownKey.width = DEF::BUTTON_DOWN_W;
	m_DownKey.height = DEF::BUTTON_DOWN_H;	
	
	m_SoftLeft.left = DEF::BUTTON_LEFT_X;
	m_SoftLeft.top = DEF::BUTTON_LEFT_Y;
	m_SoftLeft.width = DEF::BUTTON_LEFT_W;
	m_SoftLeft.height = DEF::BUTTON_LEFT_H;

	m_SoftRight.left = DEF::BUTTON_RIGHT_X;
	m_SoftRight.top = DEF::BUTTON_RIGHT_Y;
	m_SoftRight.width = DEF::BUTTON_RIGHT_W;
	m_SoftRight.height = DEF::BUTTON_RIGHT_H;
	
	m_SoftMid.left = DEF::BUTTON_MID_X;
	m_SoftMid.top = DEF::BUTTON_MID_Y;
	m_SoftMid.width = DEF::BUTTON_MID_W;
	m_SoftMid.height = DEF::BUTTON_MID_H;
}

GameState::~GameState(void)
{
}

void GameState::SetGameState( DEF::GAME_STATE_ENUM state )
{
	m_handler->SetGameState( state );
}

/**
 * Draw command bar.
 *
 */
void GameState::DrawCommandBar( GameStateHandler * handler, CommandBarState state, bool drawUpDown )
{
	char f1[] = "Go";
	char f2[] = "Back";
	char f3[] = "Cancel";
	char f4[] = "Join";
	char f5[] = "Leave";
	char f6[] = "Start";
	char f7[] = "Kick";
	char f8[] = "Finish";
	char f9[] = "Leave";
	char f10[] = "Start Kick Leave";
	char f11[] = "Continue";
	char f12[] = "Chat";
	char f13[] = "Send";
	char up[] = "Up";
	char down[] = "Down";
	int y1 = DEF::BUTTON_LEFT_Y + DEF::BUTTON_LEFT_H/4;
	int y2 = y1;
	int y3 = y1;
	
	int x1 = DEF::BUTTON_LEFT_X + DEF::BUTTON_LEFT_W/8;
	int x2 = DEF::BUTTON_RIGHT_X + DEF::BUTTON_RIGHT_W/8;
	int x3 = DEF::BUTTON_MID_X + DEF::BUTTON_MID_W/8;;	
	
	int x_up = DEF::BUTTON_UP_X + DEF::BUTTON_UP_W/8;
	int y_up = DEF::BUTTON_UP_Y + DEF::BUTTON_UP_H/4;
	
	int x_down = DEF::BUTTON_DOWN_X + DEF::BUTTON_DOWN_W/8;
	int y_down = DEF::BUTTON_DOWN_Y + DEF::BUTTON_DOWN_H/4;

	if(drawUpDown)
	{
		handler->DrawString(x_up,y_up,up);
		handler->DrawString(x_down,y_down,down);
	}

	switch( state )
	{
	case CB_GO_NULL:
		handler->DrawString(x1,y1,f1);
		break;
	case CB_NULL_BACK:
		handler->DrawString(x2,y2,f2);
		break;
	case CB_GO_BACK:
		handler->DrawString(x1,y1,f1);
		handler->DrawString(x2,y2,f2);
		break;
	case CB_REMOVE_BACK:
		handler->DrawString(x1,y1,f1);
		handler->DrawString(x3,y3,"Del");
		handler->DrawString(x2,y2,f2);
		break;
	case CB_CANCEL:
		handler->DrawString(x3,y3,f3);
		break;
	case CB_JOIN_BACK:
		handler->DrawString(x1,y1,f4);
		handler->DrawString(x2,y2,f2);
		break;
	case CB_LEAVE_NULL:
		handler->DrawString(x1,y1,f5);
		break;
	case CB_START_KICKOUT:
		handler->DrawString(x1,y1,f6);
		handler->DrawString(x3,y3,f7);
		handler->DrawString(x2,y2,f9);
		break;
	case CB_FINISH_LEAVE:
		handler->DrawString(x1,y1,f8);
		handler->DrawString(x2,y2,f9);
		break;
	case CB_CONTINUE_BACK:
		handler->DrawString(x1,y1,f11);
		handler->DrawString(x2,y2,f2);
		break;
	case CB_ACCEPT_IGNORE:
		handler->DrawString(x1,y1,"Accept");
		handler->DrawString(x2,y2,"Decline");
		break;
	default:
		break;
	}
}