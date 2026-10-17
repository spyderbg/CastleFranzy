#include "GLLiveStateTrophy.h"
//#include "GLLiveStateGLLiveMenu.h"
#include "MyGraphics.h"

#define SPACE_BETWEEN_COVERS	120

#define ICON_NUMBER_PER_LINE	4
#define TROPHY_OFF_X			8
#define TROPHY_OFF_Y			110

#define TROPHY_FRAME_RECT_W		74
#define TROPHY_FRAME_RECT_H		74
#define TROPHY_SELECT_RECT_W	62
#define TROPHY_SELECT_RECT_H	58
#define TROPHY_UNLOCK_RECT_W	54
#define TROPHY_UNLOCK_RECT_H	48
#define TROPHY_ICON_RECT_W		48
#define TROPHY_ICON_RECT_H		48

GLLiveStateTrophy::GLLiveStateTrophy(GLLiveStateHandler * handler) : GLLiveState( handler )
{
	EnableGo(false);
	m_commandBar->GetItem(0)->SetTitle("");

	m_offset_x = 0;
	m_moveDistance = 0;
	m_draggingX = false;

	m_holding = false;

	m_touchBeginY = 0;
	m_draggingY = false;

	m_draggingScroll = false;

	m_offset_y = XP_API_NEW int[m_gl_user->getNumberUserGameList()];
	XP_API_MEMSET(m_offset_y, 0, sizeof(int) * m_gl_user->getNumberUserGameList());

	m_page = 0;

	m_select = XP_API_NEW int[m_gl_user->getNumberUserGameList()];
	for(int i = 0; i < m_gl_user->getNumberUserGameList(); i++)
	{
		m_select[i] = 0;
	}
	m_pGameList = XP_API_NEW GLLiveGameInfo[m_gl_user->getNumberUserGameList()];
	m_pGameListFull = XP_API_NEW GLLiveGameInfo[m_gl_user->getNumberUserGameList()];

	if(m_pMsgBox == NULL)
	{
		m_pMsgBox = XP_API_NEW CGLLiveMessageBox();
	}

	//Begin to download trophy informations
	for(int i = 0; i < m_gl_user->getNumberUserGameList(); i++)
	{
		XP_API_STRCPY(m_pGameList[i].name, m_gl_user->getUserGameName(i));
		m_pGameList[i].pid = XP_API_ATOI(m_gl_user->getUserGamePID(i));
		m_pGameList[i].score = m_gl_user->getUserGameTrophy(i);
	}
	m_updatedTrophyGame = 0;
	m_updatedTrophyIconGame = 0;
	m_updatedIconGame = 0;
	m_updatedTrophyIcons = 0;
	m_inited = false;
}

GLLiveStateTrophy::~GLLiveStateTrophy(void)
{
	XP_API_DELETE_ARRAY(m_offset_y);
	XP_API_DELETE_ARRAY(m_select);
	XP_API_DELETE_ARRAY(m_pGameList);
	XP_API_DELETE_ARRAY(m_pGameListFull);
}

bool GLLiveStateTrophy::HandleEvent(_TOUCH_EVENT event)
{
	return HandleEventView(event);
}

bool GLLiveStateTrophy::HandleEventView(_TOUCH_EVENT event)
{
	if(event.eType == TET_Pressed)
	{
		_TouchRect rect1;
		rect1.left  = 0;
		rect1.top   = 0;
		rect1.width = SCREEN_WIDTH;
		rect1.height= SCREEN_HEIGHT - 70;
		_TouchRect rect2;
		rect2.left  = 70;
		rect2.top   = SCREEN_HEIGHT - 70;
		rect2.width = SCREEN_WIDTH - 70;
		rect2.height= 70;
		if(rect1.Contain(event.position) || rect2.Contain(event.position))
		{
			m_touchBeginX = event.position.x;
			m_touchBeginY = event.position.y;
			m_holding = true;
			
			return true;
		}
	}
	else if(event.eType == TET_Moving)
	{
		if(m_holding)
		{
			int offsetX = event.position.x - m_touchBeginX;
			int offsetY = event.position.y - m_touchBeginY;
			if(ABS(offsetX) >= 3 || ABS(offsetY) >= 3)
			{
				m_holding = false;
				//X
				if(ABS(offsetX) > ABS(offsetY))
				{
					m_draggingX = true;
				}
				//Y
				else if( GetPageLines(m_page) > 2)
				{
					_TouchPoint point;
					_TouchRect rectView, rectScroll;
					
					point.x = m_touchBeginX;
					point.y = m_touchBeginY;
					rectView.Set(80, 20 + TROPHY_OFF_Y, 300, 150);

					int barPos    = (float)(134 * (-m_offset_y[m_page])) / (float)((GetPageLines(m_page) - 2) * TROPHY_FRAME_RECT_H);				
					rectScroll.Set(380 - 5, 20 + TROPHY_OFF_Y + barPos - 4 - 5, 30, 30);
					if(rectScroll.Contain(point))
					{
						m_draggingScroll = true;
					}
					else if(rectView.Contain(point))
					{
						m_draggingY = true;
					}
				}
			}
		}

		if(m_draggingX)
		{
			m_draggingX = true;
			m_moveDistance = event.position.x - m_touchBeginX;
		}
		else if(m_draggingScroll)
		{
			m_offset_y[m_page] = -1 * (event.position.y + 4 - (20 + TROPHY_OFF_Y) - 10) * (float)((GetPageLines(m_page) - 2) * TROPHY_FRAME_RECT_H) / 134;

			if(m_offset_y[m_page] > 0)
			{
				m_offset_y[m_page] = 0;
			}
			else if(m_offset_y[m_page] < -TROPHY_FRAME_RECT_H * (GetPageLines(m_page) - 2))
			{
				m_offset_y[m_page] = -TROPHY_FRAME_RECT_H * (GetPageLines(m_page) - 2);
			}
		}
		else if(m_draggingY)
		{
			m_offset_y[m_page] += event.position.y - m_touchBeginY;
			m_touchBeginY = event.position.y;
			if(m_offset_y[m_page] > 0)
			{
				m_offset_y[m_page] = 0;
			}
			else if(m_offset_y[m_page] < -TROPHY_FRAME_RECT_H * (GetPageLines(m_page) - 2))
			{
				m_offset_y[m_page] = -TROPHY_FRAME_RECT_H * (GetPageLines(m_page) - 2);
			}
		}
	}
	else if(event.eType == TET_Released)
	{
		if(m_holding)
		{
			m_holding = false;

			if(event.position.x > 90 && event.position.y > 20 + TROPHY_OFF_Y
				&& event.position.x < 400 && event.position.y < 280)
			{
				int col = (event.position.x - 90) / TROPHY_FRAME_RECT_W;
				int row = (event.position.y - m_offset_y[m_page] - (20 + TROPHY_OFF_Y)) / TROPHY_FRAME_RECT_H;
				m_select[m_page] = row * ICON_NUMBER_PER_LINE + col;
			}
			else
			{
				m_select[m_page] = -1;
			}
		}

		else if(m_draggingY)
		{
			//...
			m_draggingY = false;
		}
		else if(m_draggingScroll)
		{
			m_draggingScroll = false;
		}
		else if(m_draggingX)
		{
			m_draggingX = false;
			m_offset_x += m_moveDistance;
			if(m_offset_x < 0)
				m_offset_x = (m_offset_x - SPACE_BETWEEN_COVERS / 2 ) / SPACE_BETWEEN_COVERS * SPACE_BETWEEN_COVERS;
			else
				m_offset_x = (m_offset_x + SPACE_BETWEEN_COVERS / 2 ) / SPACE_BETWEEN_COVERS * SPACE_BETWEEN_COVERS;
			
			//int max_off = (m_gl_user->getNumberUserGameList() - 1) / 2 < 3 ? (m_gl_user->getNumberUserGameList() - 1) / 2 : 3;
			int max_off_l = (m_gl_user->getNumberUserGameList()) / 2;
			int max_off_r = (m_gl_user->getNumberUserGameList() - 1) / 2;
			if(m_offset_x < -SPACE_BETWEEN_COVERS * max_off_l)
				m_offset_x = -SPACE_BETWEEN_COVERS * max_off_l;
			else if(m_offset_x > SPACE_BETWEEN_COVERS * max_off_r)
				m_offset_x = SPACE_BETWEEN_COVERS * max_off_r;
			m_moveDistance = 0;
			m_touchBeginX = event.position.x;
			
			int offsetPage = m_offset_x / SPACE_BETWEEN_COVERS;
			if(offsetPage >= 0)
			{
				m_page = 2 * offsetPage;
			}
			else
			{
				m_page = -2 * offsetPage - 1;
			}
			
			//if(m_page < 0 )
			//{
			//	m_page = 0;
			//	m_offset_x = 0;
			//}
			//if(m_page >= m_gl_user->getNumberUserGameList())
			//{
			//	m_page = m_gl_user->getNumberUserGameList() - 1;
			//	m_offset_x = (m_gl_user->getNumberUserGameList() / 2 - m_page) * SPACE_BETWEEN_COVERS;
			//}
			return true;
		}
	}
	return GLLiveState::HandleEvent(event);
}

void GLLiveStateTrophy::Paint()
{
	m_handler->ClearScreen();

	CGLLiveGraphics *g = m_handler->GetGraphics();

	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG, 0, 0, 0);

	for(int i = 3; i > 0; i--)
	{
		PaintCover(-i);
		PaintCover(i);
	}
	PaintCover(0);

	g->SetClip(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	GLLiveState::Paint();
}

void GLLiveStateTrophy::PaintCover(int index)
{
	int d		= m_offset_x + m_moveDistance;
	int page	= d / (SPACE_BETWEEN_COVERS);
	int off		= d % (SPACE_BETWEEN_COVERS);
	if(off < -SPACE_BETWEEN_COVERS/2)
	{
		page = page - 1;
		off += SPACE_BETWEEN_COVERS;
	}
	else if(off > SPACE_BETWEEN_COVERS/2)
	{
		page = page + 1;
		off -= SPACE_BETWEEN_COVERS;
	}

	int drawPage;
	if(page + index >= 0)
	{
		drawPage = 2 * (page + index);
	}
	else
	{
		drawPage = -2 * (page + index) - 1;
	}

	if(drawPage < 0 || drawPage > m_gl_user->getNumberUserGameList() - 1)
		return;

	CGLLiveGraphics *g				= m_handler->GetGraphics();
	CGLLiveFont     *f_name			= m_handler->GetFont(FONT_WHITE_MID);
	CGLLiveFont     *f_title		= m_handler->GetFont(FONT_RED_BIG);
	CGLLiveFont     *f_score		= m_handler->GetFont(13);
	CGLLiveFont     *f_description	= m_handler->GetFont(FONT_RED_SMALL);
	const int p_w = 326;//252;//480;

	int pos = SPACE_BETWEEN_COVERS * (-index);
	pos += off;
	g->BeginDraw3D();
#ifdef USE_NEW_STYLE_INTERFACE
	float posx = -p_w/2 - p_w/2 * pos / 400.f;
	glTranslatef(pos, -SCREEN_HEIGHT/2 - abs(pos)/20, 240.f * 1.732 + abs(pos)/1.5f);
#else
	float angle = pos / 5.0f;
	float posx = -p_w/2 - p_w/2 * pos / 400.f;
	glTranslatef(pos, -SCREEN_HEIGHT/2, 400.f + abs(pos)/4);
	glRotatef(angle, 0, 1.0, 0);
#endif
	g->SetClip(posx, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


	//Draw page
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_TROPHY_BG, posx, 20, 0);
	//g->SetClip(posx + 10, 30, 57, 57);
	if(m_pGameList[drawPage].icon)
	{
#ifdef WIN32
		g->BeginScaleIn3D(57.0 / 64, 57.0 / 64, posx + 8, 30);
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_AVATAR_LOADING0, 
				posx + 8, 30, 0);
		g->EndScaleIn3D();
#else
		//g->BeginScale(74.0 / 57, 74.0 / 57, posx + 10, 24);
		g->DrawRegion(m_pGameList[drawPage].icon, 0, 0, 57, 57, CGLLiveGraphics::TRANS_MIRROR_ROT180, 
				posx + 8,
				30,//24,
				0);
		//g->EndScale();
#endif
	}
	else
	{
		g->BeginScaleIn3D(57.0 / 64, 57.0 / 64, posx + 8, 30);
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_AVATAR_LOADING0 + m_frameCount % 8, 
				posx + 8, 30, 0);
		g->EndScaleIn3D();
	}
	//g->SetClip(posx, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//Game name
	f_name->DrawString(g, m_gl_user->getUserGameName(drawPage), posx + 95, 35, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP, 125);
	//Game score
	f_score->DrawNumber(g, 2000, posx + p_w - 28, 37, CGLLiveGraphics::RIGHT | CGLLiveGraphics::TOP);
	int _pos = f_score->GetStringSize();
	f_score->DrawString(g, "/", posx + p_w - 28 - _pos, 37, CGLLiveGraphics::RIGHT | CGLLiveGraphics::TOP);
	_pos += f_score->GetStringSize();
	f_score->DrawNumber(g, m_gl_user->getUserGameTrophy(drawPage), posx + p_w - 28 - _pos, 37, CGLLiveGraphics::RIGHT | CGLLiveGraphics::TOP);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCORE_UNIT_SMALL, posx + p_w - 25, 29, 0, 0, 0);

	if(m_pGameListFull[drawPage].trophyList && m_select[drawPage] >= 0 && m_select[drawPage] < m_pGameListFull[drawPage].trophyCount)
	{
		//trophy name
		f_title->UpdateStringSize(m_pGameListFull[drawPage].trophyList[m_select[drawPage]].name);
		if(f_title->GetStringSize() > 190)
		{
			f_title->DrawScrollString(g, m_pGameListFull[drawPage].trophyList[m_select[drawPage]].name, posx + 72, 70, 190);
		}
		else
		{
			f_title->DrawString(g, m_pGameListFull[drawPage].trophyList[m_select[drawPage]].name, posx + 72, 70, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
		}

		//Trophy score
		f_score->DrawNumber(g, m_pGameListFull[drawPage].trophyList[m_select[drawPage]].score, 
							posx + p_w - 28, 72, CGLLiveGraphics::RIGHT | CGLLiveGraphics::TOP);
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCORE_UNIT_SMALL, posx + p_w - 25, 64, 0, 0, 0);

		//trophy description
		char *des = m_pGameListFull[drawPage].trophyList[m_select[drawPage]].description;
		f_description->InitPage(des, 315);
		f_description->DrawPage(g, des, posx + 10, 106, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER, 2);

		//trophy SELECT RECT
		g->SetClip(posx, 20 + TROPHY_OFF_Y, SCREEN_WIDTH, 148);
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_TROPHY_SELECTED, 
						posx + TROPHY_OFF_X + (m_select[drawPage]%ICON_NUMBER_PER_LINE) * TROPHY_FRAME_RECT_W + (TROPHY_FRAME_RECT_W - TROPHY_SELECT_RECT_W) / 2, 
						20 + TROPHY_OFF_Y + (m_select[drawPage]/ICON_NUMBER_PER_LINE) * TROPHY_FRAME_RECT_H + m_offset_y[drawPage] + (TROPHY_FRAME_RECT_H - TROPHY_SELECT_RECT_H) / 2,
						0);
	}

	if(index == 0 || drawPage < m_updatedTrophyIconGame)
	{
		g->SetClip(posx, 20 + TROPHY_OFF_Y, SCREEN_WIDTH, 148);

		for(int i = 0; i < m_pGameListFull[drawPage].trophyCount; i++)
		{
			//the trophy is got
			if(m_pGameList[drawPage].trophyList != NULL 
				&& HasTheTrophy(m_pGameList[drawPage].trophyList, m_pGameList[drawPage].trophyCount, m_pGameListFull[drawPage].trophyList[i].id))
			{
				// find the trophy index
				int trophyIndex = 0;
				for(; trophyIndex < m_pGameList[drawPage].trophyCount; trophyIndex++)
				{
					if(m_pGameList[drawPage].trophyList[trophyIndex].id == m_pGameListFull[drawPage].trophyList[i].id)
					{
						//must enter here
						break;
					}
				}
				//if loaded
				if(m_pGameList[drawPage].trophyList[trophyIndex].icon != NULL)
				{
#ifdef WIN32
#else
					g->DrawRegion(m_pGameList[drawPage].trophyList[trophyIndex].icon, 0, 0, TROPHY_ICON_RECT_W, TROPHY_ICON_RECT_H, CGLLiveGraphics::TRANS_MIRROR_ROT180, 
						posx + TROPHY_OFF_X + (i%ICON_NUMBER_PER_LINE) * TROPHY_FRAME_RECT_W + (TROPHY_FRAME_RECT_W - TROPHY_ICON_RECT_W) / 2, 
						20 + TROPHY_OFF_Y + (i/ICON_NUMBER_PER_LINE) * TROPHY_FRAME_RECT_H + m_offset_y[drawPage] + (TROPHY_FRAME_RECT_H - TROPHY_ICON_RECT_H) / 2,
						0);
#endif
				}
				else
				{
					g->BeginScaleIn3D(TROPHY_ICON_RECT_W / 64.0, TROPHY_ICON_RECT_H / 64.0, 
									  posx + TROPHY_OFF_X + (i%ICON_NUMBER_PER_LINE) * TROPHY_FRAME_RECT_W + (TROPHY_FRAME_RECT_W - TROPHY_ICON_RECT_W) / 2, 
									  20 + TROPHY_OFF_Y + (i/ICON_NUMBER_PER_LINE) * TROPHY_FRAME_RECT_H + m_offset_y[drawPage] + (TROPHY_FRAME_RECT_H - TROPHY_ICON_RECT_H) / 2);
					m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_AVATAR_LOADING0 + m_frameCount % 8, 
						posx + TROPHY_OFF_X + (i%ICON_NUMBER_PER_LINE) * TROPHY_FRAME_RECT_W + (TROPHY_FRAME_RECT_W - TROPHY_ICON_RECT_W) / 2, 
						20 + TROPHY_OFF_Y + (i/ICON_NUMBER_PER_LINE) * TROPHY_FRAME_RECT_H + m_offset_y[drawPage] + (TROPHY_FRAME_RECT_H - TROPHY_ICON_RECT_H) / 2, 
						0);
					g->EndScaleIn3D();
				}
			}
			else
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_TROPHY_UNLOCK, 
						posx + TROPHY_OFF_X + (i%ICON_NUMBER_PER_LINE) * TROPHY_FRAME_RECT_W + (TROPHY_FRAME_RECT_W - TROPHY_UNLOCK_RECT_W) / 2, 
						20 + TROPHY_OFF_Y + (i/ICON_NUMBER_PER_LINE) * TROPHY_FRAME_RECT_H + m_offset_y[drawPage] + (TROPHY_FRAME_RECT_H - TROPHY_UNLOCK_RECT_H) / 2,
						0);
			}
		}
		//Paint scroll bg
		if(m_pGameListFull[drawPage].trophyCount > 8)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_TROPHY_SCROLL_BAR, posx + 308, 20 + TROPHY_OFF_Y + 2, 0);
			int barPos    = (float)(134 * (-m_offset_y[drawPage])) / (float)((GetPageLines(drawPage) - 2) * TROPHY_FRAME_RECT_H);
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR1, posx + 308 - 6, 20 + TROPHY_OFF_Y + barPos - 4, 0);//the point
		}
		g->ResetClip();
		//End draw page
	}

	g->EndDraw3D();
}

void GLLiveStateTrophy::UpdateAnimation()
{
	if(m_gl_user->getNumberUserGameList() > 0 && !m_inited)
	{
		m_inited = true;
		LoadGameIcon();
		//m_gl_user->sendGetUserGameTrophyList(XP_API_ATOI(m_gl_user->getUserGamePID(m_updatedTrophyGame)), m_gl_user->getUserName());
	}
	GLLiveState::UpdateAnimation();
}

TrophyInfo *GLLiveStateTrophy::processTrophyList(char *data, int dataLen, int &trophyCount)
{
	TrophyInfo *list = NULL;
	trophyCount = 0;
	if(data == NULL || XP_API_STRLEN(data) < 1)
	{
		return NULL;
	}

	char tmpString[256] = {0};
	char tmp[128] = {0};

	//Get the total count
	trophyCount++;
	for(int i=0; i < dataLen ;i++)
	{
		if(data[i] == '|')
		{			
			trophyCount ++;
		}
	}

	list = XP_API_NEW TrophyInfo[trophyCount];
	
	for(int i = 0; i < trophyCount; i++)
	{		
		XP_API_MEMSET(tmpString, 0, 256);
		getValue(data, tmpString, i, '|');			
		
		XP_API_MEMSET(tmp, 0, sizeof(tmp));
		getValue(tmpString, tmp, 0, '^');
		list[i].id = XP_API_ATOI(tmp);
		
		XP_API_MEMSET(tmp, 0, sizeof(tmp));
		getValue(tmpString, tmp, 1, '^');
		list[i].name = XP_API_STRNEW(tmp);
		
		XP_API_MEMSET(tmp, 0, sizeof(tmp));
		getValue(tmpString, tmp, 2, '^');
		list[i].description = XP_API_STRNEW(tmp);

		XP_API_MEMSET(tmp, 0, sizeof(tmp));
		getValue(tmpString, tmp, 3, '^');
		list[i].score = XP_API_ATOI(tmp);
		
		XP_API_MEMSET(tmp, 0, sizeof(tmp));
		getValue(tmpString, tmp, 4, '^');
		list[i].time = XP_API_STRNEW(tmp);
	}

	return list;
}

void GLLiveStateTrophy::OnRequestFailure(int func_id, int error_code)
{
	switch(func_id)
	{
	case FUNCTION_GET_USER_GAME_TROPHYLIST:
		XP_DEBUG_OUT( "[GLLiveStateTrophy] Get user game trophy list fail!\n" );
		break;
	case FUNCTION_GET_GAME_ICON:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Get game icon fail!\n" );
		//Next game icon
		m_updatedTrophyIconGame++;
		LoadGameIcon();
		break;
	case FUNCTION_GET_TROPHY_ICON:
		XP_DEBUG_OUT( "[GLLiveStateTrophy] Get trophy icon fail!\n" );
		//Next trophy icon
		m_updatedTrophyIcons++;
		LoadCurTrophyIcon();
		break;
	default:
		GLLiveState::OnRequestFailure(func_id, error_code);
		break;
	}
}

void GLLiveStateTrophy::OnRequestSuccess(int func_id, char *data, int data_len)
{
	switch(func_id)
	{
	case FUNCTION_GET_USER_GAME_TROPHYLIST:
		XP_DEBUG_OUT( "[GLLiveStateTrophy] Get user game trophy list OK!\n" );
		m_pGameList[m_updatedTrophyGame].trophyList = processTrophyList(data, data_len, m_pGameList[m_updatedTrophyGame].trophyCount);
		//download next game
		m_updatedTrophyGame++;
		if(m_updatedTrophyGame < m_gl_user->getNumberUserGameList())
		{
			m_gl_user->sendGetUserGameTrophyList(XP_API_ATOI(m_gl_user->getUserGamePID(m_updatedTrophyGame)), m_gl_user->getUserName());
		}
		else
		{
			m_updatedTrophyGame = 0;
			m_gl_user->sendGetGameTrophyList(XP_API_ATOI(m_gl_user->getUserGamePID(m_updatedTrophyGame)));
		}
		break;
	case FUNCTION_GET_GAME_TROPHYLIST:
		XP_DEBUG_OUT( "[GLLiveStateTrophy] Get game trophy list OK!\n" );
		m_pGameListFull[m_updatedTrophyGame].trophyList = processTrophyList(data, data_len, m_pGameListFull[m_updatedTrophyGame].trophyCount);
		//download next game
		m_updatedTrophyGame++;
		if(m_updatedTrophyGame < m_gl_user->getNumberUserGameList())
		{
			m_gl_user->sendGetGameTrophyList(XP_API_ATOI(m_gl_user->getUserGamePID(m_updatedTrophyGame)));
		}
		else
		{
			//load trophy icons
			LoadCurTrophyIcon();
		}
		break;
	case FUNCTION_GET_GAME_ICON:
		XP_DEBUG_OUT( "[GLLiveStateFriends] Get game icon OK!\n" );
		//Parse data
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			//nothing to do
		}
		else
		{
			// d|data|t|file type
			char	fileType[10] = {0};
			getValue(data, fileType, 3, '|');
			char *	base64_buf = NULL;
			int		base64_size;
			
			base64_decode(data + 2, data_len - 2 - (XP_API_STRLEN(fileType) + 3), base64_buf, base64_size);
			m_pGameList[m_updatedTrophyIconGame].icon = NEW CGLImage(base64_buf, base64_size);
			//Save to local
			char fileName[20];
			XP_API_MEMSET(fileName, 0, sizeof(fileName));
			XP_API_SPRINTF(fileName, "Game-%d.png", m_pGameList[m_updatedTrophyIconGame].pid);
			SaveFileToLocal(fileName, base64_buf, base64_size);
	
			//Free base64 buf
			XP_API_DELETE_ARRAY(base64_buf);
		}

		//Next game icon
		m_updatedTrophyIconGame++;
		LoadGameIcon();
		break;
	case FUNCTION_GET_TROPHY_ICON:
		XP_DEBUG_OUT( "[GLLiveStateTrophy] Get trophy icon OK!\n" );
		//Parse data
		if(data == NULL || XP_API_STRLEN(data) <= 0)
		{
			//nothing to do
		}
		else
		{
			char *	base64_buf = NULL;
			int		base64_size;
			base64_decode(data, data_len, base64_buf, base64_size);
			m_pGameList[m_updatedTrophyIconGame].trophyList[m_updatedTrophyIcons].icon = NEW CGLImage(base64_buf, base64_size);

			//Save to local
			char fileName[20];
			XP_API_MEMSET(fileName, 0, sizeof(fileName));
			XP_API_SPRINTF(fileName, "Trophy-%d.png", m_pGameList[m_updatedTrophyIconGame].trophyList[m_updatedTrophyIcons].id);
			SaveFileToLocal(fileName, base64_buf, base64_size);
			
			//Free base64 buf
			XP_API_DELETE_ARRAY(base64_buf);
		}

		//Next trophy icon
		m_updatedTrophyIcons++;
		LoadCurTrophyIcon();
		break;
	default:
		GLLiveState::OnRequestSuccess(func_id, data, data_len);
		break;
	}
}

int GLLiveStateTrophy::GetPageLines(int page)
{
	int lines = m_pGameListFull[page].trophyCount / ICON_NUMBER_PER_LINE;
	int off   = m_pGameListFull[page].trophyCount % ICON_NUMBER_PER_LINE;

	if(off > 0)
	{
		lines ++;
	}

	return lines;
}
void GLLiveStateTrophy::LoadGameIcon()
{
	if(m_updatedTrophyIconGame < m_gl_user->getNumberUserGameList())
	{
		//Load from local first
		char  fileName[20];
		char *data = NULL;
		int   dataLen = 0;

		XP_API_MEMSET(fileName, 0, sizeof(fileName));
		XP_API_SPRINTF(fileName, "Game-%d.png", m_pGameList[m_updatedTrophyIconGame].pid);
		
		if(GLLiveStateTrophy::LoadFileFromLocal(fileName, data, dataLen))
		{
			m_pGameList[m_updatedTrophyIconGame].icon = NEW CGLImage(data, dataLen);
			XP_API_DELETE_ARRAY(data);
			m_updatedTrophyIconGame++;
			LoadGameIcon();
		}
		else
		{
			m_gl_user->sendGetGameIcon(m_pGameList[m_updatedTrophyIconGame].pid);
		}
	}
	else
	{
		m_updatedTrophyIconGame = 0;
		m_gl_user->sendGetUserGameTrophyList(XP_API_ATOI(m_gl_user->getUserGamePID(m_updatedTrophyGame)), m_gl_user->getUserName());
	}
}
void GLLiveStateTrophy::LoadCurTrophyIcon()
{
	//Turn current Game-Thophy
	while(m_updatedTrophyIcons > m_pGameList[m_updatedTrophyIconGame].trophyCount - 1)
	{
		m_updatedTrophyIconGame++;
		m_updatedTrophyIcons = 0;
		if(m_updatedTrophyIconGame > m_gl_user->getNumberUserGameList() - 1)
		{
			return;
		}
	}

	//Load from local first
	char  fileName[20];
	char *data = NULL;
	int   dataLen = 0;

	XP_API_MEMSET(fileName, 0, sizeof(fileName));
	XP_API_SPRINTF(fileName, "Trophy-%d.png", m_pGameList[m_updatedTrophyIconGame].trophyList[m_updatedTrophyIcons].id);
	
	if(LoadFileFromLocal(fileName, data, dataLen))
	{
		m_pGameList[m_updatedTrophyIconGame].trophyList[m_updatedTrophyIcons].icon = NEW CGLImage(data, dataLen);
		XP_API_DELETE_ARRAY(data);
		m_updatedTrophyIcons++;
		LoadCurTrophyIcon();
	}
	else
	{
		m_gl_user->sendGetTrophyIcon(m_pGameList[m_updatedTrophyIconGame].trophyList[m_updatedTrophyIcons].id);
	}
}

bool GLLiveStateTrophy::HasTheTrophy(TrophyInfo* trophylist, int length, int trophyId)
{
	for(int i = 0; i < length; i++)
	{
		if(trophylist[i].id == trophyId)
		{
			return true;
		}
	}
	return false;
}