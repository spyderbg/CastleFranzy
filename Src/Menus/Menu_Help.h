#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IMenu.h"

///////////////////////////////////////////////////////////////////////

class MenuEngage;

class VisualControlButton;
class VisualControlSlider;
class VisualControlSwitch;
class VisualControlPane;
class VisualControlText;
class VisualControlMesh;
class VisualControlSprite;


class CSpriteInstance;
class CTouchElement;
class CUnit;


//forward declaration

enum MenuHelp_Types
{
	MenuHelp_NotSet = 0,
	MenuHelp_EnemiesBook, 
	MenuHelp_SpellsBook
};


struct Page
{
public:
	Page() : node3d( NULL ), anim_id( -1 ), page_type_3d ( false ), is_active( false ), 
		unit_animation_id( 1 )
	{
		meshControl = NULL; 			
		titleControl = NULL;			
		contentControl = NULL;
		spriteControl = NULL;	
		unit = NULL;
		unit_rot_pitch = 0;
	}

	~Page();

	bool LoadResurces( ASprite *default_sprite , ISceneNode *rootNode, MenuHelp_Types currentBookType,  bool cache_models = true );

	bool isActive()			    { return is_active; } 
	void setActive(bool active) { is_active = active; }

	void SetVisible( bool visible );
private:
	friend class PageManager;

	bool is_active;

	//string content
	stringc string_id;
	int	    page_string_id;

	//string title
	stringc string_title_id; 
	int     page_title_string_id;

	///string 3d model
	stringw	node3d_fileName;
	irr::scene::ISceneNode *node3d;

	//stringc node3d_animation_name;
	int unit_animation_id;
	int unit_rot_pitch;
	//string animation id from sprite
	int					    anim_id;


	VisualControlMesh	   *meshControl;
	VisualControlSprite	   *spriteControl;
	VisualControlText	   *titleControl;
	VisualControlText	   *contentControl;
	//or game unit :(
	CUnit *unit;

	bool page_type_3d;
	int  page_num;
};

class PageManager
{
public:
	PageManager();
	~PageManager();

	//initializes everything from a xml file
	bool LoadConfiguration( char *config_filename, MenuHelp_Types  ) ;
	bool LoadSprite();
	void CreateHelpers();
	void AddControls( IMenu *menu, VisualControlPane *enemies, VisualControlPane *spells );

	void SetMenuType( MenuHelp_Types type );
	void SetActiveCamera( bool activate ); 
	void SaveActiveCamera();

	//release everything
	void Release();

	//advances a page
	void Next();
	//dec a page
	void Prev();
	
	void Draw( ASprite *sprite );
	void Update( float dt);
	void Draw();

	void PrepareScene( bool hideAll ) ;
	
	int						 GetEnemiesPagesNum() { return m_enemiesBook.size(); }
	int						 GetSpellsPagesNum()  { return m_spellsBook.size(); }

	ISceneNode				 *GetRootNode(){ return m_rootNode; }

	void					  SaveVisibleStates();
	void					  RestoreVisibleStates();

private:
	ASprite*				m_images;
	ICameraSceneNode*		m_camera; 
	ILightSceneNode*		m_light;
	ICameraSceneNode*		m_oldCamera;

	VisualControlPane*		m_pane;
	
	MenuHelp_Types			m_currentBookType;

	inline array< Page * >& GetCurrentBook()
	{
		switch ( m_currentBookType )
		{
		case MenuHelp_EnemiesBook:
			return m_enemiesBook;
			break ; 
		case MenuHelp_SpellsBook:
			return m_spellsBook;
			break;
		default :
			//not allowed 
			GX_ASSERT ( false );
			return m_spellsBook;
			break;
		}
	}

	inline irr::core::array< Page * >&	GetBook( MenuHelp_Types type )
	{
		switch ( type )
		{
		case MenuHelp_EnemiesBook:
			return m_enemiesBook;
			break ; 
		case MenuHelp_SpellsBook:
			return m_spellsBook;
			break;
		default :
			//not allowed 
			GX_ASSERT ( false );
			return m_spellsBook;
			break;
		}
	}

	irr::core::array< Page * >	m_spellsBook;
	irr::core::array< Page * >	m_enemiesBook;

	VisualControlPane*			m_enemiesPane;
	VisualControlPane*			m_spellsPane;

	ISceneNode*					m_rootNode;

	int							m_oldPage;
	int							m_oldPosition;
	int							m_snapTimer;

	std::map< ISceneNode *, bool > m_visibleFlags;


};


///////////////////////////////////////////////////////////////////////
/// Menu_Help
///////////////////////////////////////////////////////////////////////

class Menu_Help : public IMenu
{
public:
	Menu_Help(IMenu* parent);
	virtual ~Menu_Help();

	void Initialize();
	void Destroy();

	void VisualControlDoAction( IVisualControl* visual_control, int action_id );

	void Draw();
	void Update( float dt );

	void ChangeCurrentMenu( IMenu* menu, bool gain_focus = true );

	// some initializations are made 
	void GainFocus();
	void LoseFocus();

	void SetHelpBookType( MenuHelp_Types type ) { m_helpBookType = type; }
	void SetIsMenuIngame( bool is_ingame ){ m_isIngame = is_ingame; }

	void ResetGraphics();

private:
	void						LoadConfigFile( );
	void						DrawBackground();

	MenuHelp_Types				m_helpBookType;

	bool						m_Finish;	
	bool						m_isIngame;

	VisualControlButton*		m_arrowLeft;
	VisualControlButton*		m_arrowRight;
	VisualControlButton*		m_buttonBack;

	PageManager*				m_pageManager;
	VisualControlPane*			m_enemiesPane;
	VisualControlPane*			m_spellsPane;
	VisualControlPane*			m_currentPane;

	CTouchElement*				m_TW_pages;
	bool						m_firstDraw;

};