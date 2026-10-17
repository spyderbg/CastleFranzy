#include "GLLiveStdafx.h"
#include "GLLiveStateProfile.h"
#include "GLLiveStateEdit.h"
#include "GLLiveMenu.h"
#include "GLLiveStateTrophy.h"

CGLImage * g_pAvatarImage = NULL;
int        g_iAvatarId;
//bool g_bShowTestImage = false;
unsigned char * pPicBuf = NULL;
AvatarInfo    * pSelImage = NULL;

int iPicLength = 0;
bool	isAvatarChanged = false;

#ifndef WIN32
extern void ShowPhotoLibrary(unsigned char ** ppPicBuf, int *piPicLength, void (* pCallBack)());

void CallBackEndPhotoLibrary(void)
{
	if(pPicBuf != NULL && iPicLength != 0)
	{
		isAvatarChanged = true;
	}
}
#endif

GLLiveStateProfile::GLLiveStateProfile(GLLiveStateHandler *handler) : GLLiveState(handler)
{
	//Copy old user infor
	XP_API_STRCPY(m_email, m_gl_user->getEmail());
	XP_API_STRCPY(m_username, m_gl_user->getUserName());
	XP_API_STRCPY(m_avatarId, m_gl_user->getAvatarId());
	m_avatar = -1;
	m_emailFlag = m_gl_user->getEmailFlag();
	m_country   = CountryToIndex(m_gl_user->getCountry());                                       //Init as not changed

	//m_trAvatar.Set(60, 30, 74, 74);

	m_trSelAvatar.Set(60, 33, 74, 74);

	m_trSelCountry = m_trSelAvatar;

	//m_trCountry.Set(330, 35, 74, 74);
	//m_trUsername.Set(106, 113, 266, 43);
	//m_trEmail.Set(106, 210, 266, 43);
	//m_trPassword.Set(106, 162, 266, 43);
	//m_trEmailFlag.Set(106, 256, 250, 35);

	_TouchRect rect;
	//Init menu item
	m_menu = XP_API_NEW GLLiveMenu(0, 0);

	rect.Set(150, 113, 79, 26);
	m_menu->AddMenuItem(this, IDC_PROFILE_SEL_AVATAR, GetString(STR_CHANGE), rect);
	m_menu->GetItem(0)->SetPic(1, Frame_menu_BUTTON_CHANGE, rect.left, rect.top);
	m_menu->GetItem(0)->SetSelPic(1, Frame_menu_BG_CHANGE, rect.left - 9, rect.top - 9);

	rect.Set(320, 113, 79, 26);
	m_menu->AddMenuItem(this, IDC_PROFILE_SEL_COUNTRY, GetString(STR_CHANGE), rect);
	m_menu->GetItem(1)->SetPic(1, Frame_menu_BUTTON_CHANGE, rect.left, rect.top);
	m_menu->GetItem(1)->SetSelPic(1, Frame_menu_BG_CHANGE, rect.left - 9, rect.top - 9);

	rect.Set(320, 158, 79, 26);
	m_menu->AddMenuItem(this, IDE_PROFILE_USERNAME, GetString(STR_CHANGE), rect);
	m_menu->GetItem(2)->SetPic(1, Frame_menu_BUTTON_CHANGE, rect.left, rect.top);
	m_menu->GetItem(2)->SetSelPic(1, Frame_menu_BG_CHANGE, rect.left - 9, rect.top - 9);

	rect.Set(320, 191, 79, 26);
	m_menu->AddMenuItem(this, IDE_PROFILE_PASSWORD, GetString(STR_CHANGE), rect);
	m_menu->GetItem(3)->SetPic(1, Frame_menu_BUTTON_CHANGE, rect.left, rect.top);
	m_menu->GetItem(3)->SetSelPic(1, Frame_menu_BG_CHANGE, rect.left - 9, rect.top - 9);

	rect.Set(320, 224, 79, 26);
	m_menu->AddMenuItem(this, IDE_PROFILE_EMAIL, GetString(STR_CHANGE), rect);
	m_menu->GetItem(4)->SetPic(1, Frame_menu_BUTTON_CHANGE, rect.left, rect.top);
	m_menu->GetItem(4)->SetSelPic(1, Frame_menu_BG_CHANGE, rect.left - 9, rect.top - 9);

	rect.Set(98, 263, 308, 32);
	m_menu->AddMenuItem(this, IDC_PROFILE_EMAIL_FLAG, NULL, rect);

	//Init menu for change password
	m_pwdMenu = XP_API_NEW GLLiveMenu(0, 0);
	
	rect.Set(105, 65, 266, 55);
	m_pwdMenu->AddMenuItem(this, IDE_PROFILE_CUR_PWD, NULL, rect);
	m_pwdMenu->GetItem(0)->SetPic(1, Frame_menu_EDIT_PROFILE, 105, 90);
	m_pwdMenu->GetItem(0)->SetSelPic(1, Frame_menu_BG_PROFILE_PWD, 105 - 6, 90 - 6);

	rect.Set(105, 125, 266, 55);
	m_pwdMenu->AddMenuItem(this, IDE_PROFILE_NEW_PWD, NULL, rect);
	m_pwdMenu->GetItem(1)->SetPic(1, Frame_menu_EDIT_PROFILE, 105, 150);
	m_pwdMenu->GetItem(1)->SetSelPic(1, Frame_menu_BG_PROFILE_PWD, 105 - 6, 150 - 6);

	rect.Set(105, 185, 266, 55);
	m_pwdMenu->AddMenuItem(this, IDE_PROFILE_CONFIRM_PWD, NULL, rect);
	m_pwdMenu->GetItem(2)->SetPic(1, Frame_menu_EDIT_PROFILE, 105, 210);
	m_pwdMenu->GetItem(2)->SetSelPic(1, Frame_menu_BG_PROFILE_PWD, 105 - 6, 210 - 6);

	rect.Set(105, 255, 266, 35);
	m_pwdMenu->AddMenuItem(this, IDC_PROFILE_CHANGE_PWD, GetString(STR_CHANGE), rect);
	m_pwdMenu->GetItem(3)->SetPic(1, Frame_menu_BUTTON_CONNECT, 105 - 107, 255 - 203);
	m_pwdMenu->GetItem(3)->SetSelPic(1, Frame_menu_BG_CONNECT, 105 - 107, 255 - 203);
	m_pwdMenu->GetItem(3)->SetFont(7);

	//Init confirm menu
	m_confirmMenu = XP_API_NEW GLLiveMenu(0, 0);
	rect.Set(136, 181 + 9, 79, 32 );
	m_confirmMenu->AddMenuItem(this, IDC_MSG_NO, GetString(STR_NO), rect);
	m_confirmMenu->GetItem(0)->SetPic(1, Frame_menu_BUTTON_YES, 0, 9);
	m_confirmMenu->GetItem(0)->SetSelPic(1, Frame_menu_BG_YES, 0, 9);

	rect.Set(263, 181 + 9, 79, 32 );
	m_confirmMenu->AddMenuItem(this, IDC_MSG_YES, GetString(STR_YES), rect);
	m_confirmMenu->GetItem(1)->SetPic(1, Frame_menu_BUTTON_NO, 0, 9);
	m_confirmMenu->GetItem(1)->SetSelPic(1, Frame_menu_BG_NO, 0, 9);


	m_commandBar->GetItem(0)->SetTitle(GetString(STR_CANCEL));

	if(m_pMsgBox == NULL)
	{
		m_pMsgBox = XP_API_NEW CGLLiveMessageBox();
	}

	m_last_state = PROFILE_EDIT;
	m_sub_state = PROFILE_EDIT;

	m_flying = false;
	m_scaling = false;
	m_selectTemp = -1;
	m_changedPwd = false;
	m_nameChanged = false;

	m_pressLeft = false;
	m_pressRight = false;
	m_isDeleting = false;
	m_rotateAngle = 0.0f;
	m_rotateLeft = true;
	m_cancelDeleting = false;
	m_pressAvatar = false;
	m_pressAvatarCount = 0;

	m_pAvatarList = NULL;
	m_avatarCount = 0;
	m_downloadAvatarIndex = -1;
	m_isUploadAvatar = false;
	
	XP_API_STRCPY(m_selectAvatarId, m_gl_user->getAvatarId());
}

GLLiveStateProfile::~GLLiveStateProfile(void)
{
	XP_API_DEL(m_pwdMenu);
	XP_API_DEL(m_menu);
	XP_API_DEL(pSelImage);
	XP_API_DEL(m_confirmMenu);
	for(int i = 0; i < m_avatarCount; i++)
	{
		XP_API_DEL(m_pAvatarList[i]);
	}
	XP_API_DELETE_ARRAY(m_pAvatarList);
}

void GLLiveStateProfile::SwitchState()
{
	CheckAvatarList();
}

void GLLiveStateProfile::PaintMain()
{
	CGLLiveGraphics * g = m_handler->GetGraphics();
	CGLLiveFont     * f = m_handler->GetFont(3);
	CGLLiveFont     * f5= m_handler->GetFont(4);
	CGLLiveFont     * f2= m_handler->GetFont(1);
	CGLLiveFont     *f14= m_handler->GetFont(13);
	CGLLiveFont     * f3= m_handler->GetFont(2);

	//g->EndDraw2D();
	//g->BeginDraw2D(320, 480);
	//Draw bg
	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_PROFILE, 0, 0, 0, 0, 0);

	//Draw title
	f2->DrawString(g, GetString(STR_PROFILE_TITLE), 240, 33,CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);

	//Draw avatar
	if(m_avatar >= 0)
	{
		ASSERT(m_avatar < m_avatarCount);
		PaintAvatar(m_pAvatarList[m_avatar], 60, 65, 74, 74);
	}
	else
	{
		PaintAvatar(g_pAvatarImage, 60, 65, 74, 74);
	}

	//Draw username
	f5->DrawString(g, m_gl_user->getUserName(), 155, 83,CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER, 160);
	
	//Draw country
	g->BeginScale(0.8, 0.8, 338, 65);
	m_handler->GetASprite(SPRITE_FLAG)->PaintFrame(g, m_country, 338, 65, 0, 0, 0);
	g->EndScale();

	//Draw edit username
	f->DrawString(g, GetString(STR_USERNAME), 60, 167, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	f->DrawString(g, m_username, 150, 167, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP, 160);
	//Draw password
	f->DrawString(g, GetString(STR_PASSWORD), 60, 200, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	char pTemp[MAX_PASSWORD];
	XP_API_STRCPY(pTemp, m_gl_user->getPassword());
	XP_API_STRSET(pTemp, '*');
	f->DrawString(g, pTemp,150, 200, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP, 160);

	//Draw email
	f->DrawString(g, GetString(STR_EMAIL), 60, 233, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	f->DrawString(g, m_email, 150, 233, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP, 160);

	//Draw email flag
	if(XP_API_STRLEN(m_email) == 0)
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHECK_PROFILE_gray, 103, 266, 0, 0, 0);
	}
	else
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHECK_PROFILE, 103, 266, 0, 0, 0);
		if(m_menu->GetItemById(IDC_PROFILE_EMAIL_FLAG)->IsSelected())
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_CHECK_PROFILE, 103 - 7, 266 - 7, 0, 0, 0);
		}
		if(m_emailFlag)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHECK_ON_PROFILE, 103, 266, 0, 0, 0);
		}
	}
	f14->InitPage(GetString(STR_REG_EMAILFLAG), 260);
	f14->DrawPage(g, GetString(STR_REG_EMAILFLAG), 150,266 + 13, CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);

	m_menu->Repaint(m_handler);

	//Draw separator
	for(int i = 0; i < 3; i++)
	{
		m_handler->GetASprite(SPRITE_TEXTBOX)->PaintFrame(g, Frame_textbox_SEPARATOR, 42, 187 + i * 33, 0, 0, 0);
	}

	GLLiveState::Paint();
}

void GLLiveStateProfile::PaintReplace()
{
	CGLLiveGraphics * g = m_handler->GetGraphics();	

	PaintSelAvatar();
	g->EndDraw2D();
	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
	g->EnableTextureBlend(true);
	g->SetARGBColor(0x80000000);
	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);
	g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_update, 28, 58, 0);
	m_handler->GetFont(0)->DrawString(g, GetString(STR_AVATAR_REPLACE), 240, 61, CGLLiveGraphics::HCENTER | CGLLiveGraphics::TOP);

	int x = 54;
	int y = 100;
	int count = 0;
	for(int i = 0; i < m_gl_user->getNumberAvatarList(); i++)
	{
		if( i != m_select - 1 && m_gl_user->getUserAvatarId(i)[0] != 's' )//&& m_pAvatarList[i] != NULL)
		{
			PaintListAvatar( i+1, x + count / 2 * 80, y + count % 2 * 60, 52, 52);
			count++;
		}
	}
}

void GLLiveStateProfile::PaintDelConfirm()
{
	CGLLiveGraphics * g = m_handler->GetGraphics();	

	PaintSelAvatar();
	g->EndDraw2D();
	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
	g->EnableTextureBlend(true);
	g->SetARGBColor(0x80000000);
	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);
	g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	m_pMsgBox->Repaint(m_handler);
	m_confirmMenu->Repaint(m_handler);
}

void GLLiveStateProfile::PaintSelAvatar()
{
	CGLLiveGraphics * g = m_handler->GetGraphics();
	CGLLiveFont     * f = m_handler->GetFont(FONT_SMALL);

	//Draw main first as bg
	PaintMain();
	if(m_sub_state == PROFILE_SEL_AVATAR)
	{
		g->EndDraw2D();
		g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
		g->EnableTextureBlend(true);
		g->SetARGBColor(0x80000000);
		m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);
		g->EnableTextureBlend(false);
#else
		g->SetARGBColor(0x80888888);
		g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif
	}

	//Paint avatar bg
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_SELECTOR, 0, 0, 0, 0, 0);

	//Draw avatar
	if(m_select > 0)
	{
		ASSERT(m_select < m_avatarCount);
		PaintAvatar(m_pAvatarList[m_select-1], m_trSelAvatar.left, m_trSelAvatar.top, 74, 74);
	}
	else
	{
		PaintAvatar(g_pAvatarImage, m_trSelAvatar.left, m_trSelAvatar.top, 74, 74);
	}

	//Paint user name
	f->DrawString(g, m_gl_user->getUserName(), m_trSelAvatar.left + m_trSelAvatar.width + 15, 
		m_trSelAvatar.top + m_trSelAvatar.height / 2,
		CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);

	//Paint selecter
	//m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BUTTON_SELECTOR, 0, 0, 0, 0, 0);

	//Paint avatar icon
	int x = m_startX;
	int y = m_startY;

	
	//g->SetARGBColor(0x500000ff);
	//g->FillRect(m_startX, m_startY, m_width - m_separateWidth, m_elementWidth);
	g->SetClip(m_startX - 8, m_startY - 10, m_viewWidth + 10, 75 * 2 + 10 + 10);
	for(int i = 0; i < m_maxElement; i++)
	{
		x = m_startX + (i / 2) * (m_elementWidth + m_separateWidth) + m_offset;
		//add page separate
		x += (i / 8) * m_separateWidth;
		y = m_startY + (i % 2)*(m_elementWidth + 10);
		if(x + m_elementWidth >= m_startX && x < m_startX + m_viewWidth )
		{
			if(m_isDeleting)
			{
				g->BeginScale(0.8, 0.8, x, y);
				//if(i % 2 == 0)
				//{
				//	g->BeginRotate( m_rotateAngle * 4, x + 64 * 0.8 / 2, y + 64 * 0.8 / 2 );
				//}
				//else
				//{
				//	g->BeginRotate( -m_rotateAngle * 4, x + 64 * 0.8 / 2, y + 64 * 0.8 / 2 );
				//}

				PaintListAvatar(i, x + (m_frameCount + i) % 4 / 2, y +  (m_frameCount + i) % 2);
				//Add X icon
				if( i > 0 && m_gl_user->getUserAvatarId(i-1)[0] != 's' && XP_API_STRICMP(m_gl_user->getUserAvatarId(i-1), m_gl_user->getAvatarId()) != 0)
				{
					m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_DEL, x + (m_frameCount + i) % 4 / 2 - 8, y + (m_frameCount + i) % 2 - 8, 0, 0, 0);
				}
				//g->EndRotate();
				g->EndScale();
			}
			else if(m_scaling)
			{
				if( i == m_selectTemp)
				{
					g->BeginScale(m_scaleRate, m_scaleRate, x + m_elementWidth / 2, y + m_elementWidth / 2);
					PaintListAvatar(i, x - 8, y - 8);
					g->EndScale();
				}
				else
				{
					PaintListAvatar(i, x, y, 0.8 * 64, 0.8 * 64);
				}
			}
			else if(i != m_selectTemp)
			{
				PaintListAvatar(i, x, y, 0.8 * 64, 0.8 * 64);
			}
		}
	}
	g->SetClip(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Paint page count
	for(int j = 0; j < m_totalPage; j++)
	{
		if(j == m_curPage)
		{
			//g->BeginScale(1.5, 1.5, 200, 100);
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_PAGE_ON, m_pageX + j * (9 + 6), m_pageY, 0, 0, 0);
			//g->EndScale();
		}
		else
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_PAGE, m_pageX + j * (9 + 6), m_pageY, 0, 0, 0);
		}
	}

	//if on flying, paint flying avatar
	if(m_flying)
	{
		g->BeginScale(m_scaleRate, m_scaleRate, m_flyX, m_flyY);
		PaintListAvatar(m_selectTemp, m_flyX, m_flyY);
		//m_handler->GetASprite(SPRITE_AVATAR)->PaintFrame(g, m_selectTemp, m_flyX, m_flyY, 0, 0, 0);
		g->EndScale();
	}

	//Paint arrow
	if(m_curPage > 0)
	{
		if(m_pressLeft)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_LEFT_SHINE, 0, 0, 0);
		}
		else
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintAFrame(g, 0, m_frameCount % 6, 0, 0, 0, 0, 0);
		}
	}
	if(m_curPage < m_totalPage - 1)
	{
		if(m_pressRight)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_RIGHT_SHINE, 0, 0, 0);
		}
		else
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintAFrame(g, 1, m_frameCount % 6, 0, 0, 0, 0, 0);
		}
	}

	GLLiveState::Paint();

}

void GLLiveStateProfile::PaintSelCountry()
{
	CGLLiveGraphics * g = m_handler->GetGraphics();
	CGLLiveFont     * f = m_handler->GetFont(FONT_SMALL);

	//Draw main first as bg
	PaintMain();
	g->EndDraw2D();
	g->BeginDraw2D(SCREEN_HEIGHT,  SCREEN_WIDTH);
#ifdef USE_NEW_STYLE_INTERFACE
	g->EnableTextureBlend(true);
	g->SetARGBColor(0x80000000);
	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0);
	g->EnableTextureBlend(false);
#else
	g->SetARGBColor(0x80888888);
	g->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

	//Paint bg
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_SELECTOR, 0, 0, 0, 0, 0);
	//Paint country
	m_handler->GetASprite(SPRITE_FLAG)->PaintFrame(g, m_select, m_trSelCountry.left, m_trSelCountry.top, 0, 0, 0);

	//Paint user name
	if(m_select > 0)
	{
		f->DrawString(g, GetString(STR_FLAG_1 + m_select - 1), m_trSelCountry.left + m_trSelCountry.width + 15, 
			m_trSelCountry.top + m_trSelCountry.height / 2,
			CGLLiveGraphics::LEFT | CGLLiveGraphics::VCENTER);
	}

	//Paint country icon
	int x = m_startX;
	int y = m_startY;
	g->SetClip(m_startX, m_startY, m_viewWidth, 2 * m_elementWidth);
	for(int i = 0; i < m_maxElement; i++)
	{
		x = m_startX + (i / 2)* (m_elementWidth + m_separateWidth) + m_offset;
		//add page separateWidth
		x += (i / 8) * 3 * m_separateWidth;
		y = m_startY + (i % 2)*m_elementWidth;
		if(x + m_elementWidth >= m_startX && x <= m_startX + m_viewWidth)
		{
			if(m_scaling)
			{
				if( i == m_selectTemp)
				{
					g->BeginScale(m_scaleRate, m_scaleRate, x + m_elementWidth / 2, y + m_elementWidth / 2);
					m_handler->GetASprite(SPRITE_FLAG)->PaintFrame(g, i + 1, x, y, 0, 0, 0);
					g->EndScale();
				}
				else
				{
					m_handler->GetASprite(SPRITE_FLAG)->PaintFrame(g, i + 1, x, y, 0, 0, 0);
				}
			}
			else if(i != m_selectTemp)
			{
				m_handler->GetASprite(SPRITE_FLAG)->PaintFrame(g, i + 1, x, y, 0, 0, 0);
			}
		}
	}
	g->SetClip(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Paint page count
	for(int j = 0; j < m_totalPage; j++)
	{
		if(j == m_curPage)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_PAGE_ON, m_pageX + j * (9 + 6), m_pageY, 0, 0, 0);
		}
		else
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_PAGE, m_pageX + j * (9 + 6), m_pageY, 0, 0, 0);
		}
	}

	//if on flying, paint flying country
	if(m_flying)
	{
		m_handler->GetASprite(SPRITE_FLAG)->PaintFrame(g, m_selectTemp + 1, m_flyX, m_flyY, 0, 0, 0);
	}

	//Paint arrow
	if(m_curPage > 0)
	{
		if(m_pressLeft)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_LEFT_SHINE, -5, 0, 0);
		}
		else
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintAFrame(g, 0, m_frameCount % 6, -5, 0, 0, 0, 0);
		}
	}
	if(m_curPage < m_totalPage - 1)
	{
		if(m_pressRight)
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_RIGHT_SHINE, 5, 0, 0);
		}
		else
		{
			m_handler->GetASprite(SPRITE_MENU)->PaintAFrame(g, 1, m_frameCount % 6, 5, 0, 0, 0, 0);
		}
	}

	GLLiveState::Paint();

}

void GLLiveStateProfile::PaintChangePwd()
{
	CGLLiveGraphics * g  = m_handler->GetGraphics();
	CGLLiveFont     * f2 = m_handler->GetFont(1);
	CGLLiveFont     * f  = m_handler->GetFont(3);
	char        pTemp[MAX_PASSWORD];

	//Draw bg
	m_handler->GetASprite(SPRITE_BG)->PaintFrame(g, 0, 0, 0, 0, 0, 0);
	m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_BG_PROFILE, 0, 0, 0, 0, 0);

	//Draw title
	f2->DrawString(g, GetString(STR_CHANGE_PWD), 240, 33,CGLLiveGraphics::HCENTER | CGLLiveGraphics::VCENTER);

	m_pwdMenu->Repaint(m_handler);

	//Draw current password
	f->DrawString(g, GetString(STR_CURRENT_PWD), 105, 70, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	MEMSET(pTemp, 0, sizeof(pTemp));
	XP_API_STRCPY(pTemp, m_currentPwd);
	XP_API_STRSET(pTemp, '*');
	f->DrawString(g, pTemp, 105 + 5, 97, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);

	//Draw current password
	f->DrawString(g, GetString(STR_NEW_PWD), 105, 130, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	MEMSET(pTemp, 0, sizeof(pTemp));
	XP_API_STRCPY(pTemp, m_newPwd);
	XP_API_STRSET(pTemp, '*');
	f->DrawString(g, pTemp, 105 + 5, 157, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);

	//Draw current password
	f->DrawString(g, GetString(STR_CONFIRM_PWD), 105, 190, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);
	MEMSET(pTemp, 0, sizeof(pTemp));
	XP_API_STRCPY(pTemp, m_confirmPwd);
	XP_API_STRSET(pTemp, '*');
	f->DrawString(g, pTemp, 105 + 5, 217, CGLLiveGraphics::LEFT | CGLLiveGraphics::TOP);

	GLLiveState::Paint();
}

void GLLiveStateProfile::Paint()
{
	m_handler->ClearScreen();
	switch(m_sub_state)
	{
	case PROFILE_EDIT:
		PaintMain();
		break;
	case PROFILE_PROCESS:
		switch(m_last_state)
		{
		case PROFILE_EDIT:
			PaintMain();
			break;
		case PROFILE_CHANGE_PWD:
			PaintChangePwd();
			break;
		case PROFILE_SEL_AVATAR:
			PaintSelAvatar();
			break;
		case PROFILE_SEL_COUNTRY:
			PaintSelCountry();
			break;
		}
		PaintLoading();
		break;
	case PROFILE_ERROR:
		switch(m_last_state)
		{
		case PROFILE_EDIT:
			PaintMain();
			break;
		case PROFILE_CHANGE_PWD:
			PaintChangePwd();
			break;
		case PROFILE_SEL_AVATAR:
			PaintSelAvatar();
			break;
		case PROFILE_SEL_COUNTRY:
			PaintSelCountry();
			break;
		}
		m_pMsgBox->Repaint(m_handler);
		break;
	case PROFILE_SEL_AVATAR:
		PaintSelAvatar();
		break;
	case PROFILE_SEL_COUNTRY:
		PaintSelCountry();
		break;
	case PROFILE_CHANGE_PWD:
		PaintChangePwd();
		break;
	case PROFILE_DEL_CONFIRM:
		PaintDelConfirm();
		break;
	case PROFILE_REPLACE:
		PaintReplace();
		break;
	default:
		break;
	}
}

void GLLiveStateProfile::UpdateAnimation()
{
	//Selecting Avatar
	if(m_sub_state == PROFILE_SEL_AVATAR)
	{
		if(m_moving)
		{
			m_speed = m_offset - m_oldOffset;/* / 1 */
			m_oldOffset = m_offset;

			if(ABS(m_speed) < 3)
			{
				m_holdCount++;
			}
			else
			{
				m_holdCount = 0;
			}

			if(m_pressAvatar && !m_moved)
			{
				m_pressAvatarCount++;
				//if hold 20 frames and no moved, show delete screen
				if(!m_isDeleting && m_pressAvatarCount > 20)
				{
					m_moving = false;
					m_isDeleting = true;
					m_cancelDeleting = false;
					m_rotateAngle = 0.0f;
					m_pressAvatarCount = 0;
				}
			}
		}
		else
		{
			int temp = (-1 * m_curPage * m_pageWidth) - m_offset;
			int speed = m_speed;

			if(ABS(speed) < 3)
			{
				speed = temp > 0 ? 20 : -20;
			}

			if(ABS(temp) < ABS(speed))
			{
				m_offset += temp;
				m_speed = 0;
			}
			else
			{
				m_offset += speed;
				if(m_offset > 0)
				{
					m_offset = 0;
					m_speed = 0;
				}
				else if(m_offset < (-1 * (m_totalWidth - m_pageWidth)))
				{
					m_offset = -1 * (m_totalWidth - m_pageWidth);
					m_speed = 0;
				}
			}
		}

		if(m_flying)
		{
			if( ABS(m_flyX - (m_trSelAvatar.left)) < m_distanceX / FLY_SPEED )
			{
				m_flying = false;
				m_select = m_selectTemp;
				XP_API_STRCPY(m_selectAvatarId, m_gl_user->getUserAvatarId(m_select - 1));
				//Begin to show avatar flied away
				m_scaling = true;
				m_scaleRate = 0;
			}
			else
			{
				//Change fly postion
				if(m_flyX < m_trSelAvatar.left)
				{
					m_flyX += m_distanceX / FLY_SPEED;
				}
				else
				{
					m_flyX -= m_distanceX / FLY_SPEED;
				}
				m_flyY -= (m_distanceY / FLY_SPEED);
				//Change scale rate
				m_scaleRate += (1 - 0.8) / FLY_SPEED;
			}
		}

		if(m_scaling)
		{
			m_scaleRate += 0.1;
			if(ABS(m_scaleRate - 0.8) < 0.001 )
			{
				m_scaling = false;
				m_selectTemp = -1;
			}
		}

		if(m_isDeleting)
		{
			//Update rotate angle
			if(m_rotateLeft)
			{
				if(m_rotateAngle > -1)
				{
					m_rotateAngle--;
				}
				else
				{
					m_rotateLeft = false;
					m_rotateAngle++;
				}
			}
			else
			{
				if(m_rotateAngle < 1)
				{
					m_rotateAngle++;
				}
				else
				{
					m_rotateLeft = true;;
					m_rotateAngle--;
				}
			}
		}
	}
	//Selecting Country
	else if(m_sub_state == PROFILE_SEL_COUNTRY)
	{
		if(m_moving)
		{
			m_speed = m_offset - m_oldOffset;/* / 1 */
			m_oldOffset = m_offset;

			if(ABS(m_speed) < 3)
			{
				m_holdCount++;
			}
			else
			{
				m_holdCount = 0;
			}
		}
		else
		{
			int temp = (-1 * m_curPage * m_pageWidth) - m_offset;
			int speed = m_speed;

			if(ABS(speed) < 3)
			{
				speed = temp > 0 ? 20 : -20;
			}

			if(ABS(temp) < ABS(speed))
			{
				m_offset += temp;
				m_speed = 0;
			}
			else
			{
				m_offset += speed;
				if(m_offset > 0)
				{
					m_offset = 0;
					m_speed = 0;
				}
				else if(m_offset < (-1 * (m_totalWidth - m_pageWidth)))
				{
					m_offset = -1 * (m_totalWidth - m_pageWidth);
					m_speed = 0;
				}
			}
		}

		if(m_flying)
		{
			if( ABS(m_flyX - (m_trSelCountry.left)) < m_distanceX / FLY_SPEED )
			{
					m_flying = false;
					m_select = m_selectTemp + 1;
					//Begin to show country flied away
					m_scaling = true;
					m_scaleRate = 0;
			}
			else
			{
				//Change fly postion
				if(m_flyX < m_trSelCountry.left)
				{
					m_flyX += m_distanceX / FLY_SPEED;
				}
				else
				{
					m_flyX -= m_distanceX / FLY_SPEED;
				}
				m_flyY -= (m_distanceY / FLY_SPEED);
			}
		}

		if(m_scaling)
		{
			m_scaleRate += 0.1;
			if(ABS(m_scaleRate - 1) < 0.001 )
			{
				m_scaling = false;
				m_selectTemp = -1;
			}
		}
	}
	else if(m_sub_state == PROFILE_EDIT)
	{
		//if mail address is null, email flag must be false
		if(XP_API_STRLEN(m_email) == 0)
		{
			m_emailFlag = false;
		}
	}
	else if(m_sub_state == PROFILE_DEL_CONFIRM)
	{
		m_confirmMenu->Update();
	}

	if(m_sub_state == PROFILE_SEL_AVATAR && isAvatarChanged)
	{
		int customCount = 0;
		for(int i = 0; i < m_gl_user->getNumberAvatarList(); i++)
		{
			if(m_gl_user->getUserAvatarId(i)[0] != 's')
			{
				customCount++;
			}
		}
		
		if(customCount < 10)
		{
			m_sub_state = PROFILE_ERROR;
			m_last_state = PROFILE_SEL_AVATAR;
			char pTemp[512];
			XP_API_SPRINTF(pTemp, "\n%s", GetString(STR_CHECK_AVATAR));
			m_pMsgBox->SetText(pTemp);
			m_pMsgBox->SetTitle(GetString(STR_WARNING));
		}
		else
		{
			m_sub_state = PROFILE_DEL_CONFIRM;
			m_last_state = PROFILE_SEL_AVATAR;
			m_pMsgBox->SetText(GetString(STR_AVATAR_FULL));
			m_pMsgBox->SetTitle(GetString(STR_WARNING));				
		}
	}

	m_menu->Update();
	m_pwdMenu->Update();

	GLLiveState::UpdateAnimation();
}

bool GLLiveStateProfile::SendChangeUsername()
{
	if(XP_API_STRCMP(m_username, m_gl_user->getUserName()) != 0)
	{
		//Change password first
		m_gl_user->sendChangeUserName(m_gl_user->getUserName(), m_gl_user->getPassword(), m_username);
		return true;
	}
	return false;
}

bool GLLiveStateProfile::CheckProfile()
{
	int errorCode = 0;
	if(XP_API_STRLEN(m_username) <= 0)
	{
		errorCode = ERROR_NO_USERNAME;
	}
	else if(XP_API_STRLEN(m_email) <= 0)
	{
		errorCode = ERROR_NO_EMAIL;
	}
	else if(!CheckEmail(m_email))
	{
		errorCode = ERROR_WRONG_EMAIL;
	}
	else if(!CheckInput(m_username))
	{
		errorCode = ERROR_WRONG_INPUT;
	}
	else
	{
		return true;
	}
	m_pMsgBox->SetTitle(GetString(STR_ERROR));
	m_pMsgBox->SetText(GetErrorString(errorCode));
	m_last_state = m_sub_state;
	m_sub_state = PROFILE_ERROR;
	return false;
}

bool GLLiveStateProfile::SendChangeUserInfo()
{
	if(XP_API_STRCMP(m_email, m_gl_user->getEmail()) != 0
		|| m_emailFlag != m_gl_user->getEmailFlag()
		|| m_country != CountryToIndex(m_gl_user->getCountry()))
	{
		//Change user information
		m_gl_user->sendUpdateUserInfo(m_email, m_emailFlag, (char*)IndexToCountry(m_country));
		return true;
	}
	return false;
}

bool GLLiveStateProfile::SendChangeAvatar()
{
	if(m_avatar >= 0 && XP_API_STRICMP(m_gl_user->getAvatarId(), m_gl_user->getUserAvatarId(m_avatar)) != 0)
	{
		//char * tempAvatar = NULL;
		//if(pSelImage->id == CUSTOMIZE_AVATAR_ID)
		//{
		//	if(pPicBuf == NULL)
		//		return false;
		//	char *	base64_buf = NULL;
		//	int		base64_size;
		//	base64_encode((char *)pPicBuf, iPicLength, base64_buf, base64_size);
		//	tempAvatar = XP_API_NEW char[base64_size + 2];
		//	XP_API_MEMSET(tempAvatar, 0, base64_size + 2);
		//	tempAvatar[0] = 0x22;//as the png header
		//	XP_API_MEMCPY(tempAvatar + 1, base64_buf, base64_size);
		//	XP_API_DELETE_ARRAY(base64_buf);
		//}
		//else
		//{
		//	tempAvatar = XP_API_NEW char[32];
		//	tempAvatar[0] = 's';
		//	tempAvatar[1] = '0' + pSelImage->id;
		//	tempAvatar[2] = '\0';
		//}
		m_gl_user->sendUploadUserPhoto(m_gl_user->getUserAvatarId(m_avatar));
		return true;
	}
	return false;
}

void GLLiveStateProfile::OnMenuItemClicked(int id)
{
	//Handle go and back button
	switch(m_sub_state)
	{
	case PROFILE_EDIT:
		if(id == IDC_GO)
		{
			if(CheckProfile())
			{
				m_last_state = m_sub_state;
				m_sub_state = PROFILE_PROCESS;
				if(!SendChangeUsername())
				{
					if(!SendChangeUserInfo())
					{
						if(!SendChangeAvatar())
						{
							//No change, back
							//m_handler->PopGLLiveState();
							m_gl_user->sendGetUserInfo();
							return;
						}
					}
				}
			}
		}
		else if(id == IDC_BACK)
		{
			m_handler->PopGLLiveState();
			return;
		}
		break;
	case PROFILE_SEL_AVATAR:
		if(id == IDC_BACK)
		{
			m_sub_state = PROFILE_EDIT;
		}
		else if(id == IDC_GO)
		{
			m_avatar = m_select - 1;
			XP_API_STRCPY(m_avatarId, m_selectAvatarId);
			m_sub_state = PROFILE_EDIT;
		}
		break;
	case PROFILE_SEL_COUNTRY:
		if(id == IDC_BACK)
		{
			m_sub_state = PROFILE_EDIT;
		}
		else if(id == IDC_GO)
		{
			m_country = m_select;
			m_sub_state = PROFILE_EDIT;
		}
		break;
	case PROFILE_CHANGE_PWD:
		if(id == IDC_BACK)
		{
			EnableGo(true);
			m_sub_state = PROFILE_EDIT;
		}
		break;
	default:
		break;
	}

	//handle other menu item
	switch(id)
	{
	case IDC_PROFILE_SEL_AVATAR:
		m_gl_user->sendGetAvatarList();
		m_sub_state = PROFILE_PROCESS;
		m_last_state = PROFILE_EDIT;
	    return;
	case IDC_PROFILE_SEL_COUNTRY:
	    //Init country icons data
	    m_select     = m_country;        
	    m_maxElement = 83;
	    m_elementWidth = 63;
	    m_separateWidth = 18;
	    m_totalPage  = (m_maxElement + 7) / 8;
	    m_viewWidth  = 4 * (m_elementWidth + m_separateWidth) - m_separateWidth;
	    m_pageWidth  = 4 * (m_elementWidth + m_separateWidth) + 3 * m_separateWidth;
	    m_totalWidth = m_totalPage * m_pageWidth;
	    m_startX     = (SCREEN_WIDTH - m_viewWidth) / 2;
	    m_startY     = 125;
	    m_touchPos   = 0;
	    m_moving     = false;
	    m_moved      = false;
	    m_curPage    = 0;
	    m_offset     = - 1 * m_curPage * m_pageWidth;
	    m_pageX      = (SCREEN_WIDTH - (m_totalPage * 9 + (m_totalPage - 1) * 6)) / 2;
	    m_pageY      = m_startY + 2 * m_elementWidth;
	    m_flying     = false;
	    m_holdCount  = 0;
	    m_speed      = 0;
		m_selectTemp = -1;
		m_scaling    = false;
		m_pressLeft  = false;
		m_pressRight = false;
	    //Change state
	    m_sub_state = PROFILE_SEL_COUNTRY;
	    return;
	case IDE_PROFILE_EMAIL:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_EMAIL), m_email, false, MAX_EMAIL - 1));
		return;
	case IDE_PROFILE_PASSWORD:
		MEMSET(m_currentPwd, 0, sizeof(m_currentPwd));
		MEMSET(m_newPwd, 0, sizeof(m_newPwd));
		MEMSET(m_confirmPwd, 0, sizeof(m_confirmPwd));
		EnableGo(false);
		m_sub_state = PROFILE_CHANGE_PWD;
		return;
	case IDE_PROFILE_USERNAME:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_USERNAME), m_username, false, MAX_USERNAME - 1));
		return;
	case IDC_PROFILE_EMAIL_FLAG:
		m_emailFlag = !m_emailFlag;
		//if mail address is null, email flag must be false
		if(XP_API_STRLEN(m_email) == 0)
		{
			m_emailFlag = false;
		}
		return;
	case IDE_PROFILE_CUR_PWD:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_CURRENT_PWD), m_currentPwd, true, MAX_PASSWORD - 1));
		return;
	case IDE_PROFILE_NEW_PWD:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_NEW_PWD), m_newPwd, true, MAX_PASSWORD - 1));
		return;
	case IDE_PROFILE_CONFIRM_PWD:
		m_handler->PushGLLiveState(XP_API_NEW GLLiveStateEdit(m_handler, GetString(STR_CONFIRM_PWD), m_confirmPwd, true, MAX_PASSWORD - 1));
		return;
	case IDC_PROFILE_CHANGE_PWD:
		if(XP_API_STRLEN(m_currentPwd) == 0)
		{
			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetErrorString(ERROR_NO_CURRENT_PWD));
			m_last_state = m_sub_state;
			m_sub_state = PROFILE_ERROR;
			break;
		}
		else if(XP_API_STRCMP(m_currentPwd, m_gl_user->getPassword()) != 0)
		{
			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetString(STR_INVALID_USERNAMEORPASSWORD));
			m_last_state = m_sub_state;
			m_sub_state = PROFILE_ERROR;
			break;
		}
		else if(XP_API_STRLEN(m_newPwd) == 0)
		{
			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetErrorString(ERROR_NO_NEW_PWD));
			m_last_state = m_sub_state;
			m_sub_state = PROFILE_ERROR;
			break;
		}
		else if(!CheckInput(m_newPwd) || !CheckInput(m_currentPwd) || !CheckInput(m_confirmPwd) )
		{
			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetErrorString(ERROR_WRONG_INPUT));
			m_last_state = m_sub_state;
			m_sub_state = PROFILE_ERROR;
			break;
		}
		else if(XP_API_STRCMP(m_newPwd, m_confirmPwd) != 0 )
		{
			m_pMsgBox->SetTitle(GetString(STR_ERROR));
			m_pMsgBox->SetText(GetErrorString(ERROR_PASSWORD_CONFIRM_FAILED));
			m_last_state = m_sub_state;
			m_sub_state = PROFILE_ERROR;
			break;
		}
		else if(XP_API_STRCMP(m_currentPwd, m_newPwd) == 0)
		{
			m_pMsgBox->SetTitle(GetString(STR_SUCCEED));
			m_pMsgBox->SetText(GetString(STR_PASSWORD_SUCCEED));
			m_sub_state = PROFILE_ERROR;
			m_changedPwd = true;
			break;
		}
		m_last_state = m_sub_state;
		m_sub_state = PROFILE_PROCESS;
		m_gl_user->sendChangePassword(m_gl_user->getUserName(), m_currentPwd, m_newPwd, m_confirmPwd);
		break;
	case IDC_MSG_YES:
		if(isAvatarChanged)
		{
			m_sub_state = PROFILE_REPLACE;
			isAvatarChanged = false;
		}
		else
		{
			m_gl_user->sendDelCustomAvatar(m_gl_user->getUserAvatarId(m_selectTemp - 1));
			m_selectTemp = -1;
			m_last_state = PROFILE_SEL_AVATAR;
			m_sub_state = PROFILE_PROCESS;
		}
		break;
	case IDC_MSG_NO:
		isAvatarChanged = false;			
		m_selectTemp = -1;
		m_sub_state = PROFILE_SEL_AVATAR;
		break;
	default:
		break;
	}
}

bool GLLiveStateProfile::HandleEventMain(_TOUCH_EVENT event)
{
	if(m_menu->HandleEvent(event))
	{
		return true;
	}
	return GLLiveState::HandleEvent(event);
}

bool GLLiveStateProfile::HandleEventSelAvatar(_TOUCH_EVENT event)
{
	if(event.eType == TET_Pressed)
	{
		_TouchRect rect;
		rect.left  = m_startX;
		rect.top   = m_startY - 10;
		rect.width = m_viewWidth;
		rect.height= m_elementWidth * 2 + 20;
		if(rect.Contain(event.position))
		{
			m_touchPos = event.position.x;
			m_moving   = true;

			int tmpY= (event.position.y - m_startY) % (m_elementWidth + 5); 
			int tmpX= (event.position.x - m_startX) % (m_elementWidth + m_separateWidth);

			//Check whether is in separator
			if(tmpX <= 64 * 0.8 && tmpY <= 64 * 0.8)
			{
				m_pressAvatar = true;
				m_pressAvatarCount = 0;
			}
			return true;
		}
		else
		{
			rect.Set(53, 150, 39, 60);
			if(m_offset % m_pageWidth == 0 && m_curPage > 0 && rect.Contain(event.position))
			{
				m_pressLeft = true;
				return true;
			}
			rect.left = 389;
			if(m_offset % m_pageWidth == 0 && !m_pressLeft && m_curPage < m_totalPage - 1 && rect.Contain(event.position))
			{
				m_pressRight = true;
				return true;
			}
		}
		
		if(m_isDeleting)
		{
			rect.Set(47, 23, 386, 254);
			if(!rect.Contain(event.position))
			{
				m_cancelDeleting = true;
				return true;
			}
		}
	}
	else if(event.eType == TET_Moving)
	{
		_TouchRect rect;
		if(m_moving)
		{
			m_oldOffset = m_offset;
			m_offset += event.position.x - m_touchPos;
			if(m_offset > 0)
			{
				m_offset = 0;
			}
			if(m_offset < -1 * (m_totalWidth - m_pageWidth))
			{
				m_offset = -1 * (m_totalWidth - m_pageWidth);
			}
			if(ABS(event.position.x - m_touchPos) > 1)
			{
				m_moved = true;
				m_flying = false;
				m_scaling = false;
				m_holdCount   = 0;
				m_selectTemp = -1;
				m_pressAvatarCount = 0;
			}
			m_touchPos = event.position.x;
			//m_curPage = -1 * m_offset / m_width;
			return true;
		}
		else if(m_pressLeft)
		{
			rect.Set(53, 150, 39, 60);
			if(!rect.Contain(event.position))
			{
				m_pressLeft = false;
				return true;
			}
		}
		else if(m_pressRight)
		{
			rect.Set(389, 150, 39, 60);
			if(!rect.Contain(event.position))
			{
				m_pressRight = false;
				return true;
			}
		}
		else if(m_isDeleting && m_cancelDeleting)
		{
			rect.Set(47, 23, 386, 254);
			if(rect.Contain(event.position))
			{
				m_cancelDeleting = false;
				return true;
			}
		}
	}
	else if(event.eType == TET_Released)
	{
		if(m_moving)
		{
			m_moving = false;
			//Auto turn page
			//There is HOLD!
			int temp = (-1 * m_offset) - (m_curPage * m_pageWidth);
			if(m_holdCount > 0)
			{
				if(temp > m_viewWidth / 2)
				{
					PageDown();
				}
				else if (temp < -1 * m_viewWidth / 2 )
				{
					PageUp();
				}
			}
			//There is NO HOLE! Turn page!
			else
			{
				if(temp > 0 && m_speed < 0)
				{
					PageDown();
				}
				else if (temp < 0  && m_speed > 0)
				{
					PageUp();
				}
			}

			if(m_moved)
			{
				m_moved = false;
				m_pressAvatar = false;
				return false;
			}
			else
			{
				if(m_pressAvatar)
				{
					m_pressAvatar = false;

					int row = (event.position.y - m_startY) / (m_elementWidth + 5);
					int col = (event.position.x - m_startX) / (m_elementWidth + m_separateWidth);

					int select = 8 * m_curPage + row + col * 2;
					if (select < m_maxElement && select != m_selectTemp)
					{
						if(m_isDeleting)
						{
							if(select <= 0 || m_gl_user->getUserAvatarId(select-1)[0] == 's' 
								|| XP_API_STRICMP(m_gl_user->getUserAvatarId(select-1), m_gl_user->getAvatarId()) == 0
								|| m_pAvatarList[select-1] == NULL)
							{
								return true;
							}
							m_pMsgBox->SetTitle(GetString(STR_WARNING));
							m_pMsgBox->SetText(GetString(STR_AVATAR_DEL));
							m_sub_state = PROFILE_DEL_CONFIRM;
							m_selectTemp = select;
							return true;
						}
						else
						{
							if(select == 0)
							{
#ifndef WIN32
								ShowPhotoLibrary(&pPicBuf, &iPicLength, CallBackEndPhotoLibrary);
#endif
								return true;
							}
							
							if( m_pAvatarList[select-1] == NULL )
							{
								return true;
							}
							
							if(m_gl_user->getUserAvatarState(select - 1) == AVATAR_STATE_CHECKING)
							{
								m_sub_state = PROFILE_ERROR;
								m_last_state = PROFILE_SEL_AVATAR;
								
								m_pMsgBox->SetText(GetString(STR_AVATAR_CHECKING));
								m_pMsgBox->SetTitle(GetString(STR_ERROR));
								return true;
							}
							
							if(m_gl_user->getUserAvatarState(select - 1) == AVATAR_STATE_CHECKE_FAILED)
							{
								m_sub_state = PROFILE_ERROR;
								m_last_state = PROFILE_SEL_AVATAR;
								
								m_pMsgBox->SetText(GetString(STR_AVATAR_FAILED));
								m_pMsgBox->SetTitle(GetString(STR_ERROR));
								return true;
							}

							m_selectTemp = select;

							//Begin to fly
							m_flyX = m_startX + col * (m_elementWidth + m_separateWidth);
							m_flyY = m_startY + (m_selectTemp % 2) * m_elementWidth;;
							m_distanceX = ABS(m_trSelAvatar.left - m_flyX);
							m_distanceY = ABS(m_trSelAvatar.top - m_flyY);
							m_scaleRate = 0.8;
							m_flying = true;
							m_scaling = false;
						}
					}
					return true;
				}
			}
			return true;
		}
		else if(m_pressLeft)
		{
			PageUp();
			m_pressLeft = false;
		}
		else if(m_pressRight)
		{
			PageDown();
			m_pressRight = false;
		}
		else if(m_isDeleting && m_cancelDeleting)
		{
			m_isDeleting = false;
			m_cancelDeleting = false;
		}
	}
	return GLLiveState::HandleEvent(event);
}

bool GLLiveStateProfile::HandleEventSelCountry(_TOUCH_EVENT event)
{
	if(event.eType == TET_Pressed)
	{
		_TouchRect rect;
		rect.left  = m_startX;
		rect.top   = m_startY;
		rect.width = m_viewWidth;
		rect.height= 2 * m_elementWidth;
		if(rect.Contain(event.position))
		{
			m_touchPos = event.position.x;
			m_moving   = true;
			return true;
		}
		else
		{
			rect.Set(53, 150, 39, 60);
			if(m_offset % m_pageWidth == 0 && m_curPage > 0 && rect.Contain(event.position))
			{
				m_pressLeft = true;
			}
			rect.left = 389;
			if(m_offset % m_pageWidth == 0 && !m_pressLeft && m_curPage < m_totalPage - 1 && rect.Contain(event.position))
			{
				m_pressRight = true;
			}
		}
	}
	else if(event.eType == TET_Moving)
	{
		if(m_moving)
		{
			m_offset += event.position.x - m_touchPos;
			if(m_offset > 0)
			{
				m_offset = 0;
			}
			if(m_offset < -1 * (m_totalWidth - m_pageWidth))
			{
				m_offset = -1 * (m_totalWidth - m_pageWidth);
			}
			if(ABS(event.position.x - m_touchPos) > 1)
			{
				m_moved = true;
				m_flying = false;
				m_scaling = false;
				m_holdCount   = 0;
				m_selectTemp = -1;
			}
			m_touchPos = event.position.x;
			//m_curPage = -1 * m_offset / m_width;
			return true;
		}
		else if(m_pressLeft)
		{
			_TouchRect rectLeft;
			rectLeft.Set(53, 150, 39, 60);
			if(!rectLeft.Contain(event.position))
			{
				m_pressLeft = false;
			}
		}
		else if(m_pressRight)
		{
			_TouchRect rectRight;
			rectRight.Set(389, 150, 39, 60);
			if(!rectRight.Contain(event.position))
			{
				m_pressRight = false;
			}
		}
	}
	else if(event.eType == TET_Released)
	{
		if(m_moving)
		{
			m_moving = false;

			//Auto turn page
			//There is HOLD!
			int temp = (-1 * m_offset) - (m_curPage * m_pageWidth);
			if(m_holdCount > 0)
			{
				if(temp > (m_viewWidth / 2))
				{
					PageDown();
				}
				else if (temp < -1 * (m_viewWidth / 2) )
				{
					PageUp();
				}
			}
			//There is NO HOLE! Turn page!
			else
			{
				if(temp > 0 && m_speed < 0)
				{
					PageDown();
				}
				else if (temp < 0 && m_speed > 0)
				{
					PageUp();
				}
			}

			if(m_moved)
			{
				m_moved = false;
				return false;
			}
			else
			{
				_TouchRect rect;
				rect.left  = m_startX;
				rect.top   = m_startY;
				rect.width = m_viewWidth;
				rect.height= 2 * m_elementWidth;
				if(rect.Contain(event.position))
				{
					int row = (event.position.y - m_startY) / m_elementWidth;
					int col = (event.position.x - m_startX) / (m_elementWidth + m_separateWidth);
					int select = 8 * m_curPage + row + col * 2;
					if(select < m_maxElement && select != m_selectTemp )
					{
						m_selectTemp = select;

						//Begin to fly
						m_flyX = m_startX + col * (m_elementWidth + m_separateWidth);
						m_flyY = m_startY + (m_selectTemp % 2) * m_elementWidth;
						m_distanceX = ABS(m_trSelAvatar.left - m_flyX);
						m_distanceY = ABS(m_trSelAvatar.top - m_flyY);
						m_scaleRate = 0.8;
						m_flying = true;
						m_scaling = false;
					}
					return true;
				}
			}
			return true;
		}
		else if(m_pressLeft)
		{
			PageUp();
			m_pressLeft = false;
		}
		else if(m_pressRight)
		{
			PageDown();
			m_pressRight = false;
		}
	}
	return GLLiveState::HandleEvent(event);
}

bool GLLiveStateProfile::HandleEventChangePwd(_TOUCH_EVENT event)
{
	if( m_pwdMenu->HandleEvent(event) )
	{
		return true;
	}

	return GLLiveState::HandleEvent(event);
}

bool GLLiveStateProfile::HandleEventReplace(_TOUCH_EVENT event)
{
	_TouchRect rect;
	rect.Set(55, 100, 370, 115);
	switch(event.eType)
	{
		case TET_Pressed:
			if(rect.Contain(event.position))
			{
				m_pressAvatar = true;
				m_replaceX = event.position.x;
				m_replaceY = event.position.y;
			}
			break;
		case TET_Moving:
			if(m_pressAvatar && ( ABS(m_replaceX - event.position.x) > 2 || ABS(m_replaceY - event.position.y) > 2 ))
			{
				m_pressAvatar = false;
			}
			break;
		case TET_Released:
			if(m_pressAvatar)
			{
				m_pressAvatar = false;
				int tmpY= (event.position.y - 100) % (52 + 8); 
				int tmpX= (event.position.x - 55) % (52 + 28);
				//Check whether is in separator
				if(tmpX <= 52 && tmpY <= 52)
				{
					int row = (event.position.y - 100) / (52 + 8); 
					int col = (event.position.x - 55) / (52 + 28);
					int select =  row + col * 2;
					
					if(select >= m_select - 1)
					{
						select++;
					}

					if(m_pAvatarList[select + 1] == NULL)
					{
						return true;
					}

					m_gl_user->sendDelCustomAvatar(m_gl_user->getUserAvatarId(select));
					m_sub_state = PROFILE_PROCESS;
					m_last_state = PROFILE_SEL_AVATAR;

					m_isUploadAvatar = true;
				}
			}
		default:
			break;
	}
	return true;
}

bool GLLiveStateProfile::HandleEvent(_TOUCH_EVENT event)
{
	switch(m_sub_state)
	{
	case PROFILE_EDIT:
		return HandleEventMain(event);
	case PROFILE_SEL_AVATAR:
		return HandleEventSelAvatar(event);
	case PROFILE_SEL_COUNTRY:
		return HandleEventSelCountry(event);
	case PROFILE_ERROR:
		if(event.eType == TET_Pressed)
		{
			if(m_changedPwd)
			{
				m_changedPwd = false;
				EnableGo(true);
				m_sub_state = PROFILE_EDIT;
			}
			else if(isAvatarChanged)
			{
				isAvatarChanged = false;
				char *	base64_buf = NULL;
				int		base64_size;
				base64_encode((char *)pPicBuf, iPicLength, base64_buf, base64_size);
				char *tempAvatar = XP_API_NEW char[base64_size + 2];
				XP_API_MEMSET(tempAvatar, 0, base64_size + 2);
				tempAvatar[0] = 0x22;//as the png header
				XP_API_MEMCPY(tempAvatar + 1, base64_buf, base64_size);
				XP_API_DELETE_ARRAY(base64_buf);
				
				m_gl_user->sendUploadUserPhoto(tempAvatar);
				m_isUploadAvatar = true;
				m_sub_state = PROFILE_PROCESS;
			}
			else
			{
				m_sub_state = m_last_state;
			}
		}
		return true;
	case PROFILE_PROCESS:
		return true;
	case PROFILE_CHANGE_PWD:
		return HandleEventChangePwd(event);
	case PROFILE_DEL_CONFIRM:
		return m_confirmMenu->HandleEvent(event);
	case PROFILE_REPLACE:
		return HandleEventReplace(event);
	default:
		break;
	}
	return false;
}

//void GLLiveStateProfile::OnNetworkError()
//{
//	m_pMsgBox->SetTitle(GetString(STR_ERROR));
//	m_pMsgBox->SetText(GetErrorString(ERROR_CONNECTION));
//	m_sub_state = PROFILE_ERROR;;
//	XP_DEBUG_OUT( "[GLLiveStateProfile] OnNetworkError.\n");
//}
//void GLLiveStateProfile::OnRequestTimeout( int func_id )
//{
//	m_pMsgBox->SetTitle(GetString(STR_ERROR));
//	m_pMsgBox->SetText(GetErrorString(ERROR_REQUEST_TIME_OUT));
//	m_sub_state = PROFILE_ERROR;;
//	XP_DEBUG_OUT( "[GLLiveStateProfile] OnRequestTimeout.\n");
//}

void GLLiveStateProfile::OnRequestSuccess( int func_id, char * data, int data_len )
{
	switch(func_id)
	{
	case FUNCTION_CHANGE_PASSWORD:
		XP_DEBUG_OUT( "[GLLiveStateProfile] Change password OK!\n" );
		m_gl_user->setPassword(m_newPwd);
		CGLLive::SaveUserInfo(m_bAutoLogin, m_gl_user->getUserName(), m_gl_user->getPassword());

		m_pMsgBox->SetTitle(GetString(STR_SUCCEED));
		m_pMsgBox->SetText(GetString(STR_PASSWORD_SUCCEED));
		m_sub_state = PROFILE_ERROR;
		m_changedPwd = true;
		break;
	case FUNCTION_UPDATE_USER_INFO:
		XP_DEBUG_OUT( "[GLLiveStateProfile] Change user info OK!\n" );
		//XP_API_STRCPY(m_loginUser.email, m_email);
		//m_loginUser.country = m_country;
		if(!SendChangeAvatar())
		{
			m_gl_user->sendGetUserInfo();
			return;
		}		
		break;
	case FUNCTION_UPLOAD_USER_AVATAR:
		if(m_isUploadAvatar)
		{
			XP_DEBUG_OUT( "[GLLiveStateProfile] Upload avatar OK!\n" );
			m_gl_user->sendGetAvatarList();
			m_isUploadAvatar = false;
		}
		else
		{
			XP_DEBUG_OUT( "[GLLiveStateProfile] Change avatar OK!\n" );
			SAFE_DELETE(g_pAvatarImage);
			g_pAvatarImage = LoadPicFromLocal(m_gl_user->getUserAvatarId(m_avatar));
			XP_API_STRCPY(m_gl_user->getAvatarId(), m_gl_user->getUserAvatarId(m_avatar));
			
			m_gl_user->sendGetUserInfo();
		}
		return;
	case FUNCTION_CHANGE_USERNAME:
		XP_DEBUG_OUT( "[GLLiveStateProfile] Change username OK!\n" );
		CGLLive::SaveUserInfo(m_bAutoLogin, m_username, m_gl_user->getPassword());
		//XP_API_STRCPY(m_loginUser.username, m_username);
		{
			//load save time stamp
			char* temp = NULL;
			int len = 0;
			char saveName1[32] = {0};
			char saveName2[32] = {0};
			XP_API_SPRINTF(saveName1, "gcTime_%s", m_gl_user->getUserName());
			XP_API_SPRINTF(saveName2, "gcTime_%s", m_username);
			GLLiveStateTrophy::LoadFileFromLocal(saveName1, temp, len);
			GLLiveStateTrophy::SaveFileToLocal(saveName2, temp, len);
			GLLiveStateTrophy::SaveFileToLocal(saveName1, temp, 1);
			XP_API_DELETE_ARRAY(temp);
		}
		m_nameChanged = true;
		if(!SendChangeUserInfo())
		{
			if(!SendChangeAvatar())
			{
				m_gl_user->sendGetUserInfo();
				return;
			}
		}
		break;
	case FUNCTION_GET_USER_INFO:
		XP_DEBUG_OUT( "[GLLiveStateProfile] Reget user info OK!\n" );
		//m_handler->PopGLLiveState();
		if(m_nameChanged)
		{
			m_gl_chat->Logout();
			m_gl_chat->Login(m_gl_user->getUserName(), m_gl_user->getPassword());
		}
		else
		{
			m_handler->PopGLLiveState();
		}
		break;
	case FUNCTION_GET_USER_AVATAR_LIST:
	{
		XP_DEBUG_OUT( "[GLLiveStateProfile] Load avatar info list OK!\n" );
		bool selectExist = false;
		bool avatarExist = false;
		//Update the select index
		for(int i = 0; i < m_gl_user->getNumberAvatarList(); i++)
		{
		  if(XP_API_STRICMP(m_gl_user->getUserAvatarId(i), m_selectAvatarId) == 0)
		  {
			  m_select = i + 1;
			  selectExist = true;
		  }
		  if(XP_API_STRICMP(m_gl_user->getUserAvatarId(i), m_avatarId) == 0)
		  {
			  m_avatar = i;
			  avatarExist = true;
		  }			
		}

		if(!avatarExist)
		{
			XP_API_STRCPY(m_avatarId, m_gl_user->getAvatarId());
			for(int i = 0; i < m_gl_user->getNumberAvatarList(); i++)
			{
			  if(XP_API_STRICMP(m_gl_user->getUserAvatarId(i), m_avatarId) == 0)
			  {
				  m_avatar = i;
				  avatarExist = true;
			  }			
			}
			//ASSERT(avatarExist);
		}

		if(!selectExist)
		{
			m_select = m_avatar + 1;
			XP_API_STRCPY(m_selectAvatarId, m_avatarId);
			selectExist = true;
		}

		LoadAvatarListFromLocal();
		InitSelAvatarData();
		m_maxElement = m_avatarCount + 1;
		m_sub_state = PROFILE_SEL_AVATAR;
		CheckAvatarList();
		break;
	}
	case FUNCTION_DOWNLOAD_USER_AVATAR:
		XP_DEBUG_OUT( "[GLLiveStateProfile] Download avatar data OK!\n" );
		ASSERT(m_downloadAvatarIndex >= 0);
		if(data == NULL || data_len <= 0)
		{
			m_pAvatarList[m_downloadAvatarIndex] = LoadPicFromLocal(NULL);
		}
		else
		{
			//Remove the 0x22 first
			data++;
			data_len--;

			char *	base64_buf = NULL;
			int		base64_size;
			base64_decode(data, data_len, base64_buf, base64_size);
			m_pAvatarList[m_downloadAvatarIndex] = NEW CGLImage(base64_buf, base64_size);
			SavePicToLocal(m_gl_user->getUserAvatarId(m_downloadAvatarIndex), base64_buf, base64_size);
			XP_API_DELETE_ARRAY(base64_buf);
		}
		CheckAvatarList();
		break;
	case FUNCTION_DEL_USER_AVATAR:
		XP_DEBUG_OUT( "[GLLiveStateProfile] Delete avatar data OK!\n" );
		if(m_isUploadAvatar)
		{
			isAvatarChanged = false;
			char *	base64_buf = NULL;
			int		base64_size;
			base64_encode((char *)pPicBuf, iPicLength, base64_buf, base64_size);
			char *tempAvatar = XP_API_NEW char[base64_size + 2];
			XP_API_MEMSET(tempAvatar, 0, base64_size + 2);
			tempAvatar[0] = 0x22;//as the png header
			XP_API_MEMCPY(tempAvatar + 1, base64_buf, base64_size);
			XP_API_DELETE_ARRAY(base64_buf);
			
			m_gl_user->sendUploadUserPhoto(tempAvatar);
			m_isUploadAvatar = true;
			m_sub_state = PROFILE_PROCESS;
		}
		else
		{
			m_gl_user->sendGetAvatarList();
		}
		break;
	default:
		GLLiveState::OnRequestSuccess(func_id, data, data_len);
		break;
	}
}
void GLLiveStateProfile::OnRequestFailure( int func_id, int error_code )
{
	if(func_id != FUNCTION_KEEP_ALIVE && func_id != FUNCTION_DOWNLOAD_USER_AVATAR)
	{
		m_pMsgBox->SetTitle(GetString(STR_ERROR));
		m_pMsgBox->SetText(GetErrorString(error_code));
		m_sub_state = PROFILE_ERROR;
		XP_DEBUG_OUT( "[GLLiveStateProfile] OnRequestFailure.\n");	
	}
	
	CheckAvatarList();
	m_isUploadAvatar = false;

	switch(func_id)
	{
	case FUNCTION_CHANGE_PASSWORD:
		XP_DEBUG_OUT( "[GLLiveStateProfile] Change password failed!\n" );
		break;
	case FUNCTION_UPDATE_USER_INFO:
		XP_DEBUG_OUT( "[GLLiveStateProfile] Change user info failed!\n" );		
		break;
	case FUNCTION_UPLOAD_USER_AVATAR:
		XP_DEBUG_OUT( "[GLLiveStateProfile] Change avatar failed!\n" );
		break;
	case FUNCTION_CHANGE_USERNAME:
		XP_DEBUG_OUT( "[GLLiveStateProfile] Change username failed!\n" );
		break;
	case FUNCTION_DOWNLOAD_USER_AVATAR:
		break;
	default:
		GLLiveState::OnRequestFailure(func_id, error_code);
		break;
	}
}

void GLLiveStateProfile::PageDown()
{
	m_curPage++;
	if(m_curPage >= m_totalPage)
	{
		m_curPage = m_totalPage - 1;
	}
}

void GLLiveStateProfile::PageUp()
{
	m_curPage--;
	if(m_curPage < 0)
	{
		m_curPage = 0;
	}
}

void GLLiveStateProfile::OnChatConnected()
{
	XP_DEBUG_OUT( "[GLLiveStateProfile] Relogin chat server OK!\n" );
	m_handler->PopGLLiveState();
}

void GLLiveStateProfile::LoadAvatarListFromLocal()
{
	for(int i = 0; i < m_avatarCount; i++)
	{
		XP_API_DEL(m_pAvatarList[i]);
	}
	XP_API_DELETE_ARRAY(m_pAvatarList);

	m_avatarCount = m_gl_user->getNumberAvatarList();
	m_pAvatarList = XP_API_NEW CGLImage*[m_avatarCount];
	for(int i = 0; i < m_avatarCount; i++)
	{
		m_pAvatarList[i] = LoadPicFromLocal(m_gl_user->getUserAvatarId(i));
	}
}

bool GLLiveStateProfile::CheckAvatarList()
{
	for(int i = 0; i < m_avatarCount; i++)
	{
		if( m_pAvatarList[i] == NULL )
		{
			m_gl_user->sendDownloadAvatar(m_gl_user->getUserAvatarId(i));
			m_downloadAvatarIndex = i;
			return false;
		}
	}
	return true;
}

void GLLiveStateProfile::InitSelAvatarData()
{	
	//Init avatar icons data
	if(m_last_state != PROFILE_SEL_AVATAR && m_sub_state != PROFILE_SEL_AVATAR)
	{
		m_select = m_avatar + 1;
		XP_API_STRCPY(m_selectAvatarId, m_avatarId);
	}
    m_maxElement = m_avatarCount + 1;
	m_elementWidth = 64 * 0.8;
    m_separateWidth = 29;
    m_totalPage  = (m_maxElement + 7) / 8;
    m_pageWidth  = 4 * (m_elementWidth + m_separateWidth) + m_separateWidth;
    m_viewWidth  = 4 * (m_elementWidth + m_separateWidth) - m_separateWidth;
    m_totalWidth = m_totalPage * m_pageWidth;
    m_startX     = (SCREEN_WIDTH - m_viewWidth) / 2;
    m_startY     = 125;
    m_touchPos   = 0;
    m_moving     = false;
    m_moved      = false;
    m_curPage    = 0;
    m_offset     = - 1 * m_curPage * m_pageWidth;
    m_pageX      = (SCREEN_WIDTH - (m_totalPage * 9 + (m_totalPage - 1) * 6)) / 2;
    m_pageY      = 245;
    m_flying     = false;
    m_holdCount  = 0;
    m_speed      = 0;
	m_scaling    = false;
	m_selectTemp = -1;
	m_pressLeft  = false;
	m_pressRight = false;
	m_isDeleting = false;
	m_rotateAngle = 0.0f;
    //Change state
    m_sub_state = PROFILE_SEL_AVATAR;
}

void GLLiveStateProfile::PaintListAvatar(int index, int x, int y, int scalex, int scaley)
{
	if(index < 0)
	{
		return;
	}

	CGLLiveGraphics	*g = m_handler->GetGraphics();
	bool			needScale = (scalex > 0 && scaley > 0);;

	if(needScale)
		g->BeginScale(scalex / 64.0, scaley / 64.0, x, y);

	if(index == 0)
	{
		m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_AVATAR, x, y, 0, 0, 0);
	}
	else
	{
		PaintAvatar(m_pAvatarList[index-1], x, y);
		switch( m_gl_user->getUserAvatarState(index-1) )
		{
			case AVATAR_STATE_CHECKING:
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_CHECK, x, y, 0, 0, 0);
				break;
			case AVATAR_STATE_CHECKE_FAILED:
				m_handler->GetASprite(SPRITE_MENU)->PaintFrame(g, Frame_menu_FORBID, x, y, 0, 0, 0);
				break;
			default:
				break;
		}
	}

	if(needScale)
		g->EndScale();
}