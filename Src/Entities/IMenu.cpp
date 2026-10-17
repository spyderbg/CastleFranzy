//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "stdafx.h"

#include "Entities/IMenu.h"
#include "VisualControls/IVisualControl.h"
//#include "IO/Audio/SoundManager.h"

///////////////////////////////////////////////////////////////////////

using namespace irr;
using namespace core;

///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
IMenu::IMenu(IMenu* parent) :
	m_transition_control( NULL )
{
	SetParent(parent);

	m_isInitialized			= false;
	m_isOnFocus				= false;
	m_is_music_enable		= true;
	m_music_id				= -1;

	m_transition_alpha		= 255;
	m_visual_controls_num	= 0;

	m_transition_action		= -1;

	m_pCurrentSubMenu		= NULL;

	m_subMenus.clear();

	m_sprite			 = NULL;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
IMenu::~IMenu()
{
	VisualControls_Free();
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::LoadResources()
{
	m_sprite = GetParent()->GetSprite();
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::FreeResources()
{
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::GainFocus()
{
	if( !m_isInitialized || m_isOnFocus ) 
		return;

	if( m_pCurrentSubMenu != NULL )
		m_pCurrentSubMenu->GainFocus();
	
	VisualControls_SetEnableTE( true );

	m_isOnFocus = true;

	Transition_Start( transition_show );
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::LoseFocus()
{
	if( !m_isInitialized ) 
		return;

	SubMenus_LoseFocus();
	
	VisualControls_SetEnableTE(false);

	m_isOnFocus = false;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::Draw()
{
	if( !m_isInitialized )
		return;

	/// Draw controls
	VisualControls_Draw();

	/// Draw submenu
	if( m_pCurrentSubMenu != NULL )
		m_pCurrentSubMenu->Draw();
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::Update( float dt )
{
	m_frame_counter++;

	if( !m_isInitialized )
		return;

	/// TRansition
	Transition_Update();

	/// Update controls
	VisualControls_Update( dt );

	/// Update submenu
	if( m_pCurrentSubMenu != NULL )
		m_pCurrentSubMenu->Update( dt );
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::VisualControls_Draw()
{
	for (int i = 0; i < m_visual_controls_num; i++)
	{
		m_visual_controls[i]->Draw();
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::VisualControls_Update( float dt )
{
	for (int i = 0; i < m_visual_controls_num; i++)
	{
		int action = m_visual_controls[i]->Update( dt );
		if (action != -1 && !Transition_IsGoing())
		{
			VisualControlDoAction(m_visual_controls[i], action);
		}
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::VisualControlAdd( IVisualControl* visual_control ) 
{ 
	if (m_visual_controls_num <= MAX_VISUAL_CONTROLS)
	{
		m_visual_controls[m_visual_controls_num++] = visual_control;
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void IMenu::SetVisibleFlagToControls( bool flag ) 
{
	for (int i = 0; i < m_visual_controls_num; i++)
	{
		m_visual_controls[i]->SetVisible( flag );		
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void IMenu::VisualControls_SetEnable( bool is_enable )
{
	for (int i = 0; i < m_visual_controls_num; i++)
	{
		m_visual_controls[i]->SetEnabled(is_enable);
	}
}

void IMenu::VisualControls_SetEnableTE( bool is_enable )
{
	for (int i = 0; i < m_visual_controls_num; i++)
	{
		m_visual_controls[i]->SetEnabledTE(is_enable);
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::VisualControls_SetSelected( bool is_selected )
{
	for (int i = 0; i < m_visual_controls_num; i++)
	{
		m_visual_controls[i]->SetSelected(is_selected);
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::VisualControls_Free()
{
	for (int i = 0; i < m_visual_controls_num; i++)
	{
		//m_visual_controls[i]->SetEnabled(false);
		SAFE_DEL(m_visual_controls[i]);
	}

	m_visual_controls_num = 0;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::VisualControls_StartTransition( eTransitionType type, int param1, int param2 )
{
	for (int i = 0; i < m_visual_controls_num; i++)
	{
		if ( m_visual_controls[i]->GetValue() != -2 )
			m_visual_controls[i]->Transition_Start( type, param1, param2 );
	}
}


//---------------------------------------------------------------------------------------------------------
// Recursively for all sub menus
//---------------------------------------------------------------------------------------------------------
void IMenu::VisualControls_Reset( )
{
	for( TSubMenus::Iterator iter = m_subMenus.getIterator(); !iter.atEnd(); iter++ )
	{
		IMenu* sub_menu = iter->getValue();

		sub_menu->VisualControls_Reset( );
	}

	for (int i = 0; i < m_visual_controls_num; i++)
	{
		m_visual_controls[i]->Reset();
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
IVisualControl* IMenu::VisualControlGetByActionId(int action_id)
{
	for (int i = 0; i < m_visual_controls_num; i++)
	{
		if (m_visual_controls[i]->GetActionId() == action_id )
			return m_visual_controls[i];
	}

	return NULL;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
IVisualControl* IMenu::VisualControlGetById(int control_id)
{
	for (int i = 0; i < m_visual_controls_num; i++)
	{
		if (m_visual_controls[i]->GetControlId() == control_id )
			return m_visual_controls[i];
	}

	return NULL;
}


//---------------------------------------------------------------------------------------------------------
// Notify with action_id all sub menus of this_menu.
//---------------------------------------------------------------------------------------------------------
void IMenu::VisualControlAction_NotifyMenus( IMenu* this_menu, IVisualControl* visual_control, int action_id )
{
	for( TSubMenus::Iterator iter = this_menu->m_subMenus.getIterator(); !iter.atEnd(); iter++ )
	{
		IMenu* sub_menu = iter->getValue();

		sub_menu->VisualControlAction_NotifyMenus( sub_menu, visual_control, action_id);
	}

	this_menu->VisualControlDoAction(visual_control, action_id);
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
IMenu* IMenu::GetRootMenu()
{
	IMenu* curr_menu = this;
	IMenu* parent = this->GetParent();
	while (parent != 0)
	{
		curr_menu = parent;
		parent = parent->GetParent();
	}

	return curr_menu;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenu_Register( stringc submenuKey, IMenu* sub_menu )
{
	GX_ASSERT( SubMenus_GetCount() < MAX_SUB_MENUS );

	TSubMenus::Node* node = m_subMenus.find( submenuKey );
	if( node != NULL )
	{
		DBG_WARNING( "Try to register already registred submenu %s !!!\n", + submenuKey.c_str() );

		return;
	}

	m_subMenus[submenuKey] = sub_menu;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenu_GainFocus(IMenu* sub_menu, bool trans_parent, bool trans_this)
{
	m_pCurrentSubMenu = sub_menu;
	
	LoseFocus();

	m_pCurrentSubMenu->GainFocus();
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenu_LoseFocus()
{
	if( m_pCurrentSubMenu != NULL )
	{
		m_pCurrentSubMenu->LoseFocus();
		m_pCurrentSubMenu = NULL;
	}

	if( !IsOnFocus() ) 
		GainFocus();
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenu_Change( stringc submenuKey, bool gain_focus)
{	
	TSubMenus::Node* node = m_subMenus.find( submenuKey );
	if( node == NULL )
		return;

	IMenu* menu = node->getValue();

	if( menu == NULL || m_pCurrentSubMenu == menu )
		return;

	if( m_pCurrentSubMenu != NULL )
		m_pCurrentSubMenu->LoseFocus();
	
	m_pCurrentSubMenu = menu;

	if( gain_focus )
		m_pCurrentSubMenu->GainFocus();
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenu_Remove( )
{	
	if( m_pCurrentSubMenu != NULL )
		m_pCurrentSubMenu->LoseFocus();
	
	m_pCurrentSubMenu = NULL;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenu_Draw()
{
	if (m_pCurrentSubMenu == NULL) return;
	if (!m_pCurrentSubMenu->IsInitialized()) return;
	//if (!m_pCurrentSubMenu->IsOnFocus()) return;

	m_pCurrentSubMenu->Draw();
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenu_Update( float dt )
{
	if (m_pCurrentSubMenu == NULL) return;
	if (!m_pCurrentSubMenu->IsInitialized()) return;
	//if (!m_pCurrentSubMenu->IsOnFocus()) return;

	m_pCurrentSubMenu->Update( dt );
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
bool IMenu::SubMenu_Exists ()
{
	return m_pCurrentSubMenu != NULL;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
bool IMenu::SubMenu_Is (stringc key)
{
	TSubMenus::Node* node = m_subMenus.find( key );
	return node && node->getValue() == m_pCurrentSubMenu;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenus_Initialize()
{
	s32 count = SubMenus_GetCount();
	if( count <= 0 )
		return;

	for( TSubMenus::Iterator iter = m_subMenus.getIterator(); !iter.atEnd(); iter++ )
	{
		IMenu* sub_menu = iter->getValue();

		sub_menu->Initialize();
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenus_LoseFocus()
{
	s32 count = SubMenus_GetCount();
	if( count <= 0 )
		return;

	// Recursively for all children
	for( TSubMenus::Iterator iter = m_subMenus.getIterator(); !iter.atEnd(); iter++ )
	{
		IMenu* sub_menu = iter->getValue();

		sub_menu->SubMenus_LoseFocus();
	}

	m_pCurrentSubMenu = NULL;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenus_Destroy()
{
	s32 count = SubMenus_GetCount();
	if( count <= 0 )
		return;

	for( TSubMenus::Iterator iter = m_subMenus.getIterator(); !iter.atEnd(); iter++ )
	{
		IMenu* sub_menu = iter->getValue();

		sub_menu->Destroy();
	}

	for( TSubMenus::Iterator iter = m_subMenus.getIterator(); !iter.atEnd(); iter++ )
	{
		IMenu* sub_menu = iter->getValue();

		SAFE_DEL( sub_menu );
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenus_LoadResources()
{
	s32 count = SubMenus_GetCount();
	if( count <= 0 )
		return;

	// Recursively for all children
	for( TSubMenus::Iterator iter = m_subMenus.getIterator(); !iter.atEnd(); iter++ )
	{
		IMenu* sub_menu = iter->getValue();

		sub_menu->SubMenus_LoadResources();
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SubMenus_FreeResources()
{
	s32 count = SubMenus_GetCount();
	if( count <= 0 )
		return;

	// Recursively for all children
	for( TSubMenus::Iterator iter = m_subMenus.getIterator(); !iter.atEnd(); iter++ )
	{
		IMenu* sub_menu = iter->getValue();

		sub_menu->SubMenus_FreeResources();
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::SetMusicId( int music_id, bool bLoop )
{
	m_music_id = music_id;
	m_is_music_loop = bLoop;

	PlayMusic();
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::PlayMusic()
{
	if( (m_is_music_enable) && (m_music_id != -1) )
	{		
		__MGR_SOUNDS()->stopAllMusics();
		__MGR_SOUNDS()->play( m_music_id, true );
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::Transition_Start( eTransitionType type, int action_id, int param1, int param2 )
{
	m_transition_type   = type;
	m_transition_action = action_id;
	m_frame_counter     = 0;

	VisualControls_StartTransition( type, param1, param2 );

	switch(type)
	{
		case transition_show:
			m_transition_alpha = 80;//0;
			m_transition_is_finished = false;
			break;

		case transition_hide:
			m_transition_alpha = 255;
			m_transition_is_finished = false;
			break;

		case transition_showLeft:
		case transition_showRight:
			m_transition_step        = (float)param2;
			m_transition_offset_x    = (float)param1;
			m_transition_offset_max  = (float)param1;
			m_transition_is_finished = false;
			break;

		case transition_hideLeft:
		case transition_hideRight:
			m_transition_step        = (float)param2;
			m_transition_offset_x    = 0;
			m_transition_offset_max  = (float)param1;
			m_transition_is_finished = false;
			break;

		case transition_showWithZoom:
			m_transition_scale = 0;
			break;

		case transition_hideWithZoom:
			m_transition_scale = 100;
			break;

		case transition_stretch_show:
			// TODO: Spyder
			//SoundManager::GetInstance()->PlaySfxIfNotPlaying( SFX_MENU_CHAIN_BROWSE );
		case transition_stretch_hide:
			m_transition_FramesCount = param1;
			break;
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::Transition_Update()
{
	if (m_transition_type == transition_finishing)
		m_transition_type = transition_none;
		
	if (m_transition_is_finished)
	{
		Transition_Finish();
		m_transition_is_finished = false;
	}

	switch (m_transition_type)
	{
		case transition_show:
			m_transition_alpha += ((255 - m_transition_alpha) / 3);
			if (m_transition_alpha >= (255 - 3)) m_transition_alpha = 255;
			if (m_transition_alpha == 255) m_transition_is_finished = true;
			break;

		case transition_hide:
			m_transition_alpha -= (m_transition_alpha / 3);
			if (m_transition_alpha <= 55) m_transition_alpha = 0;
			if (m_transition_alpha == 0) m_transition_is_finished = true;
			//if (m_transition_alpha <= 120) m_transition_is_finished = true;
			break;

		case transition_showRight:
			m_transition_offset_x -= (m_transition_offset_x / 2);
			if (m_transition_offset_x <= 2) m_transition_offset_x = 0;
			if (m_transition_offset_x == 0) m_transition_is_finished = true;
			break;

		case transition_showWithZoom:
			m_transition_scale += ((100 - m_transition_scale) / 2);
			if (m_transition_scale >= 99) m_transition_scale = 100;
			//m_transition_alpha = (int)(m_transition_scale * 255) / 100;
			if (m_transition_scale == 100) m_transition_is_finished = true;
			break;

		case transition_hideWithZoom:
			m_transition_scale -= (m_transition_scale / 2);
			if (m_transition_scale <= 1) m_transition_scale = 0;
			//m_transition_alpha = (int)(m_transition_scale * 255) / 100;
			if (m_transition_scale == 0) m_transition_is_finished = true;
			break;

		case transition_stretch_hide:
		{
			if (m_frame_counter == 1)
				m_transition_val = 1;
			else if ( (m_frame_counter == 2) || (m_frame_counter == 3) || (m_frame_counter == 4) )
				m_transition_val = 2;

			if (m_frame_counter == 4)
				m_transition_is_finished = true;

			m_transition_alpha = (int)((4 - m_frame_counter) * 255) / 4 + (100 /* minimum alpha */);
			CLAMP_VAL(m_transition_alpha, 0, 255);
			break;
		}

		case transition_stretch_show:
		{
			if ( (m_frame_counter == 2) || (m_frame_counter == 3) || (m_frame_counter == 6) || (m_frame_counter == 7) )
				m_transition_val = 1;
			else if ( (m_frame_counter == 1) || (m_frame_counter == 3) )
				m_transition_val = 2;
			else if ( (m_frame_counter == 4) || (m_frame_counter == 5) || (m_frame_counter == 8) )
				m_transition_val = 0;

			if (m_frame_counter == 8)
			{
				m_transition_val = 0;
				m_transition_is_finished = true;
			}

			m_transition_alpha = (int)(m_frame_counter * 255) / 8 + (100 /* minimum alpha */);
			CLAMP_VAL(m_transition_alpha, 0, 255);
			break;
		}
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::Transition_Finish()
{
	switch (m_transition_type)
	{
		case transition_show:
			break;

		case transition_hide:
			//LoseFocus();
			break;
	}

	m_transition_type = transition_finishing;

	if (m_transition_action != -1)
	{
		VisualControlDoAction(m_transition_control, m_transition_action);
	}
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
int IMenu::Transition_GetPercent()
{
	int percent;
	switch (m_transition_type)
	{
		case transition_show:
		case transition_hide:
			percent = (m_transition_alpha * 100) / 255;
			break;

		case transition_showWithZoom:
		case transition_hideWithZoom:
			percent = (int)m_transition_scale;
			break;

		default:
			percent = 100;
			break;
	}

	return percent;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
void IMenu::UpdateMenuMusic()
{
	if( (m_is_music_enable) && (m_music_id != -1) )
	{
		/*m_music_id = */__MGR_SOUNDS()->playIfNotPlaying( m_music_id );
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
