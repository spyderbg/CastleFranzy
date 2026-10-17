///////////////////////////////////////////////////////////////////////
//
//	Cheats
//
///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#define __CHEATS()		( CCheats::GetInstance() )

///////////////////////////////////////////////////////////////////////

#include "Common/Singleton.h"

///////////////////////////////////////////////////////////////////////

class CCheats;

///////////////////////////////////////////////////////////////////////

typedef enum
{
	CT_Normal,
	CT_Switch
} TCheatType;

class CCheatField
{
	friend class CCheats;

public:
	CCheatField();
	CCheatField( const CCheatField & field );
	CCheatField( irr::core::stringw text, TCheatType type = CT_Normal );
	virtual ~CCheatField();

	TCheatType			type;
	irr::core::stringw	text;

private:
	irr::core::rect<irr::s32>	rect;
};

typedef irr::core::array<irr::s32>							TCheatArray;
typedef irr::core::map< irr::core::stringc, CCheatField >	TCheatList;

///////////////////////////////////////////////////////////////////////
/// CCheats
///////////////////////////////////////////////////////////////////////

class CCheats : public CSingleton<CCheats>
{
public:
	CCheats();
	virtual ~CCheats();

			void	Init();
			void	Draw();
			void	Update( irr::s32 sector );			// display sector 1 - left 2 - right
			void	Clear();

			bool	IsVisible()	{ return m_IsVisible; }
			void	Show()		{ m_IsVisible = true; }
			void	Hide()		{ m_IsVisible = false; }

			bool	IsActive( irr::core::stringc key );
			bool	Activate( irr::s32 x, irr::s32 y );	// if return 'true' close cheat menu

private:
			void	AddNewCheat( irr::core::stringc key, CCheatField field );

public:
			bool	m_Enabled;
			bool	m_IsVisible;

	TCheatArray		m_CheatBuffer;
	TCheatList		m_CheatsList;
};

