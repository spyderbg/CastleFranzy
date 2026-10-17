#include "GLLiveStateTrophy.h"
//#include "GLLiveStateGLLiveMenu.h"
#include "MyGraphics.h"

#define SPACE_BETWEEN_COVERS	120

GLLiveStateTrophy::GLLiveStateTrophy(GLLiveStateHandler * handler) : GLLiveState( handler )
{
	m_sub_state = TROPHY_VIEW;
	EnableGo(false);

	m_offset_x = 0;
	m_moveDistance = 0;
	m_draggingX = false;

	m_holding = false;

	m_touchBeginY = 0;
	m_draggingY = false;

	m_offset_y = XP_API_NEW int[m_gl_user->getNumberUserGameList()];
	XP_API_MEMSET(m_offset_y, 0, sizeof(int) * m_gl_user->getNumberUserGameList());

	m_page = 0;

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
	XP_API_DELETE_ARRAY(m_pGameList);
	XP_API_DELETE_ARRAY(m_pGameListFull);
}

bool GLLiveStateTrophy::HandleEvent(_TOUCH_EVENT event)
{
	switch(m_sub_state)
	{
	case TROPHY_VIEW:
		return HandleEventView(event);
	case TROPHY_DETAIL:
		return HandleEventDetail(event);
	}
	return  false;
}

bool GLLiveStateTrophy::HandleEventDetail(_TOUCH_EVENT event)
{
	if(event.eType == TET_Pressed)
	{
		m_sub_state = TROPHY_VIEW;
	}
	return true;
}

bool GLLiveStateTrophy::HandleEventView(_TOUCH_EVENT event)
{
	if(event.eType == TET_Pressed)
	{
		_TouchRect rect;
		rect.left  = 0;
		rect.top   = 0;
		rect.width = SCREEN_WIDTH;
		rect.height= SCREEN_HEIGHT - 30;
		if(rect.Contain(event.position))
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

				_TouchRect rect;
				rect.Set(117, 99, 250, 190);

				if(ABS(offsetY) > ABS(offsetX) && rect.Contain(event.position) && GetPageLines(m_page) > 3)
				{
					m_draggingY = true;
				}
				else
				{
					m_draggingX = true;
				}
			}
		}

		if(m_draggingX)
		{
			m_draggingX = true;
			m_moveDistance = event.position.x - m_touchBeginX;
		}

		else if(m_draggingY)
		{
			m_offset_y[m_page] += event.position.y - m_touchBeginY;
			m_touchBeginY = event.position.y;
			if(m_offset_y[m_page] > 0)
			{
				m_offset_y[m_page] = 0;
			}
			else if(m_offset_y[m_page] < -60 * (GetPageLines(m_page) - 3))
			{
				m_offset_y[m_page] = -60 * (GetPageLines(m_page) - 3);
			}
		}
	}
	else if(event.eType == TET_Released)
	{
		if(m_holding)
		{
			m_holding = false;

			if(event.position.x > 123 && event.position.y > 100
				&& event.position.x < 350 && event.position.y < 290)
			{
				int col = (event.position.x - 123) / 75;
				int row = (event.position.y - m_offset_y[m_page] - 100) / 60;
				m_select = row * 3 + col;
			}
			else
			{
				m_select = -1;
			}

			if(m_select >= 0 && m_select < m_pGameListFull[m_page].trophyCount)
			{
				m_pMsgBox->SetTitle(m_pGameListFull[m_page].trophyList[m_select].name);
				m_sub_state = TROPHY_DETAIL;
			}
		}

		else if(m_draggingY)
		{
			//...
			m_draggingY = false;
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
	switch(m_sub_state)
	{
	case TROPHY_VIEW:
		PaintView();
		break;
	case TROPHY_DETAIL:
		PaintDetail();
		break;
	}
}

void GLLiveStateTrophy::PaintDetail()
{
	CGLLiveGraphics *g = m_handler->GetGraphics();
	CGLLiveFont     *f7 =m_handler->GetFont(6);
	CGLLiveFont     *f11=m_handler->GetFont(10);

	PaintView();
	
	m_pMsgBox->Repaint(m_handler);

	//Draw trophy icon
	//the trophy is got
	if(m_pGameList[m_page].trophyList != NULL 
		&& HasTheTrophy(m_pGameList[m_page].trophyList, m_pGameList[m_page].trophyCount, m_pGameListFull[m_page].trophyList[m_select].id))
	{
		// find the trophy index
		int trophyIndex = 0;
		for(; trophyIndex < m_pGameList[m_page].trophyCount; trophyIndex++)
		{
			if(m_pGameList[m_page].trophyList[trophyIndex].id == m_pGameListFull[m_page].trophyList[m_select].id)
			{
				//must enter here
				break;
			}
		}
		//if loaded
		if(m_pGameList[m_page].trophyList != NULL && m_pGameList[m_page].trophyList[trophyIndex].icon != NULL)
		{
#ifdef WIN32
#else
			g->DrawRegion(m_pGameList[m_page].trophyList[trophyIndex].icon, 0, 0, 48, 48, CGLLiveGraphics::TRANS_MIRROR_ROT180, 
						  240 - 48/2, 
						  130 - 9,
						  0);
#endif
		}
		else
		{
			m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TROPHY_LOADING0 + m_frameCount % 8, 
				240 - 30/2, 
				130, 
				0);
		}
	}
	else
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_Trophy, 240 - 54/2, 130 - 9, 0);
	}

	//Trophy score
	char pTemp[20];
	XP_API_MEMSET(pTemp, 0, sizeof(pTemp));
	XP_API_SPRINTF(pTemp, "%d", m_pGameListFull[m_page].trophyList[m_select].score);
	f11->DrawString(g, pTemp, 240, 170, CGLLiveGraphics::RIGHT | CGLLiveGraphics::TOP);
	f11->UpdateStringSizeEN(pTemp);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCORE_UINT,245, 170-2, 0);

	//Draw detail
	char *des = m_pGameListFull[m_page].trophyList[m_select].description;
	f7->InitPage(des, 190);
	f7->DrawPage(g, des, 240, 208, CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER, 3);
}

void GLLiveStateTrophy::PaintView()
{
	CGLLiveGraphics *g = m_handler->GetGraphics();

	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);

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

	CGLLiveGraphics *g = m_handler->GetGraphics();
	CGLLiveFont     *f5 = m_handler->GetFont(4);
	CGLLiveFont     *f11= m_handler->GetFont(10);
	const int p_w = 252;//480;

	int pos = SPACE_BETWEEN_COVERS * (-index);
	pos += off;
	g->BeginDraw3D();
#ifdef USE_NEW_STYLE_INTERFACE
	float posx = -p_w/2 - p_w/2 * pos / 400.f;
	glTranslatef(pos, -SCREEN_HEIGHT/2 - abs(pos)/20, 400.f + abs(pos)/1.5f);
#else
	float angle = pos / 5.0f;
	float posx = -p_w/2 - p_w/2 * pos / 400.f;
	glTranslatef(pos, -SCREEN_HEIGHT/2, 400.f + abs(pos)/4);
	glRotatef(angle, 0, 1.0, 0);
#endif
	g->SetClip(posx, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


	//Draw page
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_OF_TROPHY, posx, 20, 0);
	if(m_pGameList[drawPage].icon)
	{
#ifdef WIN32
		m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TROPHY_LOADING0, 
				posx + 30, 44, 0);
#else
		//g->BeginScale(74.0 / 57, 74.0 / 57, posx + 10, 24);
		g->DrawRegion(m_pGameList[drawPage].icon, 0, 0, 57, 57, CGLLiveGraphics::TRANS_MIRROR_ROT180, 
				posx + 10,
				30,//24,
				0);
		//g->EndScale();
#endif
	}
	else
	{
		m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TROPHY_LOADING0 + m_frameCount % 8, 
				posx + 30, 44, 0);
	}
	//Game name
	f5->DrawString(g, m_gl_user->getUserGameName(drawPage), posx + 90, 35, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP, 150);
	//Game score
	char pTemp[20];
	XP_API_MEMSET(pTemp, 0, sizeof(pTemp));
	XP_API_SPRINTF(pTemp, "%d", m_gl_user->getUserGameTrophy(drawPage));
	f11->DrawString(g, pTemp, posx + 90, 70, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	f11->UpdateStringSizeEN(pTemp);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCORE_UINT,posx + 90 + f11->GetStringSize() + 8, 68, 0, 0, 0);

	if(index == 0 || drawPage < m_updatedTrophyIconGame)
	{
		g->SetClip(posx, 100, SCREEN_WIDTH, 185);

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
					g->DrawRegion(m_pGameList[drawPage].trophyList[trophyIndex].icon, 0, 0, 48, 48, CGLLiveGraphics::TRANS_MIRROR_ROT180, 
						posx + 35 + (i%3) * 75 - 9, 
						118 + (i/3) * 60 + m_offset_y[drawPage] - 9,
						0);
#endif
				}
				else
				{
					m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_TROPHY_LOADING0 + m_frameCount % 8, 
						posx + 35 + (i%3) * 75, 
						118 + (i/3) * 60 + m_offset_y[drawPage], 
						0);
				}
			}
			else
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_Trophy, 
																	posx + 35 + (i%3) * 75 - 12, 
																	118 + (i/3) * 60 + m_offset_y[drawPage] - 9,
																	0);
			}
		}
		//Paint scroll bg
		if(m_pGameListFull[drawPage].trophyCount > 9)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_TROPHY, posx + 237, 100, 0, 0, 0);
			int barHeight = 190 * 3 / GetPageLines(drawPage);
			int barPos    = (float)(190 - barHeight) * (-m_offset_y[drawPage]) / (float)((GetPageLines(drawPage) - 3) * 60);
			for(int j = 0; j < barHeight / 10; j++)
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR, posx + 237, 100 + barPos + j * 10, 0, 0, 0);
			}
			if(barHeight % 10 > 0)
			{
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_SCROLL_BAR, posx + 237, 100 + barPos + barHeight - 10, 0, 0, 0);
			}
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
	int lines = m_pGameListFull[page].trophyCount / 3;
	int off   = m_pGameListFull[page].trophyCount % 3;

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