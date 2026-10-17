///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "VisualControls/IVisualControl.h"

///////////////////////////////////////////////////////////////////////

#define MAX_VISUAL_CONTROLS        ( 1000 )
#define MAX_SUB_MENUS              ( 30 )

///////////////////////////////////////////////////////////////////////

class IMenu;

///////////////////////////////////////////////////////////////////////

typedef irr::core::map< irr::core::stringc, IMenu * > TSubMenus;

///////////////////////////////////////////////////////////////////////

typedef enum 
{
	button_action_gain_focus = -123456, 
	button_action_lose_focus,

} GlobalMenuActions;

//---------------------------------------------------------------------------------------------------------
// Basic menu interface
//---------------------------------------------------------------------------------------------------------
class IMenu
{
public:
	IMenu(IMenu* parent);
	virtual ~IMenu();

protected:
	virtual void    LoadResources();
	virtual void    FreeResources();

public:
	virtual void    Initialize() = 0;
	virtual void    Destroy() = 0;
	inline  bool    IsInitialized()                                { return m_isInitialized; }

	virtual void    GainFocus();
	virtual void    LoseFocus();
	virtual bool    IsOnFocus()                                    { return m_isOnFocus; }

	inline  void    SetMusicEnable( bool enable )                  { m_is_music_enable = enable; }
			void    SetMusicId( int music_id, bool bLoop = true );
	inline  int     GetMusicId()                                   { return m_music_id; }
	        void    PlayMusic();
			void	UpdateMenuMusic();

	inline  void    SetSprite(ASprite* sprite)                      { m_sprite = sprite; } 
	inline  ASprite* GetSprite()                                    { return m_sprite; }

	virtual void    Draw();
	virtual void    Update( float dt );

	//-------------------- Visual controls functionality -----------------------
	virtual void    VisualControlDoAction( IVisualControl* visual_control, int action_id ) = 0;
	        void    VisualControlAdd( IVisualControl* visual_control );
			void    VisualControlAction_NotifyMenus( IMenu* this_menu, IVisualControl* visual_control, int action_id );
	IVisualControl* VisualControlGetByActionId(int action_id);
	IVisualControl* VisualControlGetById(int control_id);

			void    VisualControls_SetEnable( bool is_enable );
			void    VisualControls_SetEnableTE( bool is_enable );
			void    VisualControls_SetSelected( bool is_selected );
	        void    VisualControls_Draw();
	        void    VisualControls_Update( float dt );
	        void    VisualControls_Free();
			void    VisualControls_Reset();
			void    VisualControls_StartTransition( eTransitionType type, int param1 = 0, int param2 = 0 );


	//-------------------------------- Transitions ---------------------------------
	virtual void    Transition_Start( eTransitionType type, int action_id = -1, int param1 = 0, int param2 = 0 );
	virtual void    Transition_Update();
	virtual void    Transition_Finish();
	inline  bool    Transition_IsGoing()              { return (m_transition_type != transition_none); };
	        int     Transition_GetPercent();


	//-------------------- Parent and children functionality -----------------------
	inline IMenu*   GetParent()                    { return m_pParentMenu; }
	inline  void    SetParent(IMenu* parent)       { m_pParentMenu = parent; }
	       IMenu*   GetRootMenu();

			void    SubMenu_Register( irr::core::stringc submenuKey, IMenu* sub_menu );
			void    SubMenu_GainFocus(IMenu* sub_menu, bool trans_parent = false, bool trans_this = false);
			void    SubMenu_LoseFocus();
			void	SubMenu_Change( irr::core::stringc submenuKey, bool gain_focus = true );
			void	SubMenu_Remove();
			void    SubMenu_Draw();
			void    SubMenu_Update( float dt );
			bool	SubMenu_Exists ();
			bool	SubMenu_Is (stringc key);
			
	inline	s32		SubMenus_GetCount() { return m_subMenus.size(); }
			void    SubMenus_LoadResources(); 
			void    SubMenus_FreeResources(); 
			void    SubMenus_LoseFocus(); 
			void    SubMenus_Initialize();
			void    SubMenus_Destroy();
	
			void	SetMenuName( int text_id ) { m_menu_name = text_id; }			
			int 	GetMenuName() { return m_menu_name; }
			void	SetVisibleFlagToControls( bool flag ) ;

private:
	IVisualControl*  m_visual_controls[MAX_VISUAL_CONTROLS];
	
	int				 m_menu_name;

protected:
	IMenu*			m_pParentMenu;
	IMenu*			m_pCurrentSubMenu;

	TSubMenus		m_subMenus;

protected:
	ASprite*         m_sprite;

	int              m_frame_counter;
	bool             m_isInitialized;
	bool             m_isOnFocus;
	int              m_visual_controls_num;

	int              m_music_id;
	bool             m_is_music_enable;
	bool             m_is_music_loop;

	// transitions
	//------------------------------------------
	eTransitionType  m_transition_type;
	int              m_transition_action;
	IVisualControl*  m_transition_control;
	int              m_transition_alpha;
	bool             m_transition_is_finished;
	float            m_transition_scale;
	float            m_transition_offset_y;
	float            m_transition_offset_x;
	float            m_transition_offset_max;
	float            m_transition_step;

	int              m_transition_FramesCount;
	int              m_transition_val;
};
//---------------------------------------------------------------------------------------------------------
