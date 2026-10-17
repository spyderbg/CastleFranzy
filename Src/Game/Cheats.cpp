///////////////////////////////////////////////////////////////////////
//
//	Cheats
//
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Game/Cheats.h"
#include "Game/Playfield.h"
#include "Game/Config/GameConfig.h"
#include "Game/Objects/Weapons/Fireball.h"
#include "Game/Objects/Weapons/Griffin.h"
#include "Game/Objects/Weapons/Iceball.h"
#include "Game/Objects/Weapons/Hydra.h"
#include "Game/Objects/Weapons/CastleGate.h"

#include "Core/irrlicht.h"

#include "Fonts/FontManager.h"

///////////////////////////////////////////////////////////////////////

#define CHEAT_START_POS				50
#define CHEAT_LINE_HEIGHT			40

///////////////////////////////////////////////////////////////////////


s32 CHEAT_BUFFER[] = { 1, 1, 1, 2, 2, 2, 1, 1, 1 }; // use only positive numbers 
//s32 CHEAT_BUFFER[] = { 1, 1 };
s32 CHEAT_BUFFER_LEN = ARRAY_SIZE( CHEAT_BUFFER );


///////////////////////////////////////////////////////////////////////
/// CCheatField
///////////////////////////////////////////////////////////////////////

CCheatField::CCheatField()
{
	this->type	= CT_Normal;
	this->text	= L"";
}

CCheatField::CCheatField( stringw text, TCheatType type )
{
	this->type	= type;
	this->text	= text;
}

CCheatField::CCheatField( const CCheatField & field )
{
	this->type	= field.type;
	this->text	= field.text;
	this->rect	= field.rect;
}

CCheatField::~CCheatField()
{
	this->type	= CT_Normal;
	this->text	= "";	
}


///////////////////////////////////////////////////////////////////////
/// CCheats
///////////////////////////////////////////////////////////////////////

CCheats::CCheats() :
	m_Enabled( false ),
	m_IsVisible( false )
{
	GX_ASSERT( CHEAT_BUFFER_LEN > 0 );
}

CCheats::~CCheats()
{
}

///////////////////////////////////////////////////////////////////////

void CCheats::AddNewCheat( stringc key, CCheatField field )
{
	s32 y_offset = CHEAT_START_POS + CHEAT_LINE_HEIGHT * m_CheatsList.size();

	field.rect = r2i( 0, y_offset, 480, y_offset + CHEAT_LINE_HEIGHT );

	m_CheatsList[key] = field;
}

///////////////////////////////////////////////////////////////////////

void CCheats::Init()
{
	m_CheatsList.clear();

	AddNewCheat( "Immortality", CCheatField( L"Immortality", CT_Switch ) );

	AddNewCheat( "Money", CCheatField( L"Money: +10000" ) );

	AddNewCheat( "Souls", CCheatField( L"Souls: +100" ) );

	AddNewCheat( "FPS", CCheatField( L"Show FPS", CT_Switch ) );

	AddNewCheat( "Weapons", CCheatField( L"Unlock all weapons", CT_Switch ) );

	AddNewCheat( "UnlockLevels", CCheatField( L"Unlock all levels", CT_Switch ) );

	Clear();

	m_Enabled = true;
}

///////////////////////////////////////////////////////////////////////

void CCheats::Clear()
{
	if( m_CheatBuffer.allocated_size() != CHEAT_BUFFER_LEN )
	{
		m_CheatBuffer.clear();
		m_CheatBuffer.reallocate( CHEAT_BUFFER_LEN );

		for( s32 i=0; i<m_CheatBuffer.allocated_size(); i++ )
			m_CheatBuffer.push_back( -CHEAT_BUFFER_LEN );
	}
	else
	{
		for( s32 i=0; i<m_CheatBuffer.allocated_size(); i++ )
			m_CheatBuffer[i] = -CHEAT_BUFFER_LEN;
	}
}

///////////////////////////////////////////////////////////////////////

void CCheats::Draw()
{
	if( !m_IsVisible || !m_Enabled )
		return;

	SColor bgr_color1	= SColor( 0xFFF0F0F0 );
	SColor bgr_color2	= SColor( 0xFF0F0F0F );
	SColor line_color	= SColor( 0xFFF0F0F0 );

	TCheatList::Iterator iter = m_CheatsList.getIterator();
	for( s32 i=0; !iter.atEnd(); iter++, i++ )
	{				
		CCheatField & field = iter->getValue();

		stringw str = field.text;

		if( field.type == CT_Switch )
		{
			if( IsActive( iter->getKey() ) )
				str = str + L": On";
			else
				str = str + L": Off";
		}
		
//		__IRR_DRIVER()->draw2DRectangle( (i & 1) ? bgr_color1 : bgr_color2, field.rect );
		__IRR_DRIVER()->draw2DLine( p2i( field.rect.UpperLeftCorner.X, field.rect.LowerRightCorner.Y ), 
									p2i( field.rect.LowerRightCorner.X, field.rect.LowerRightCorner.Y ),
									line_color );
		__IRR_DRIVER()->draw2DLine( p2i( field.rect.UpperLeftCorner.X, field.rect.LowerRightCorner.Y + 1 ), 
									p2i( field.rect.LowerRightCorner.X, field.rect.LowerRightCorner.Y + 1 ),
									line_color );

		__IRR_DRIVER()->draw2DLine( p2i( field.rect.UpperLeftCorner.X, field.rect.UpperLeftCorner.Y ), 
									p2i( field.rect.LowerRightCorner.X, field.rect.UpperLeftCorner.Y ),
									line_color );
		__IRR_DRIVER()->draw2DLine( p2i( field.rect.UpperLeftCorner.X, field.rect.UpperLeftCorner.Y - 1 ), 
									p2i( field.rect.LowerRightCorner.X, field.rect.UpperLeftCorner.Y - 1 ),
									line_color );

		__FONT("MS_Title")->DrawString( str.c_str(), 240, (field.rect.UpperLeftCorner.Y+field.rect.LowerRightCorner.Y) / 2, CFont::HVCENTER );
	}
}

void CCheats::Update( s32 sector )
{
	if( m_IsVisible || !m_Enabled )
	{
		Clear();
		return;
	}

	GX_ASSERT( m_CheatBuffer.size() == CHEAT_BUFFER_LEN );

	/// Add new number 
	if( m_CheatBuffer.getLast() < 0 )
	{
		s32 i = 0;
		while( m_CheatBuffer[i] > 0 ) { i++; }

		m_CheatBuffer[i] = sector;
	}
	else
	{
		for( s32 i = 0; i < CHEAT_BUFFER_LEN-1; i++ )
			m_CheatBuffer[i] = m_CheatBuffer[i+1];

		m_CheatBuffer[CHEAT_BUFFER_LEN-1] = sector;
	}

	/// Check 
	bool same = true;
	for( s32 i = 0; i < CHEAT_BUFFER_LEN; i++ )
	{
		if( CHEAT_BUFFER[i] != m_CheatBuffer[i] )
		{
			same = false;
			break;
		}
	}
	if( same == true )
	{
		m_IsVisible = true;

		Clear();
	}
}

///////////////////////////////////////////////////////////////////////

bool CCheats::IsActive( irr::core::stringc key )
{
	if( key == "Immortality" )
		return CGameConfig::Invulnerable;
	else if( key == "Money" )
		return false;
	else if( key == "Souls" )
		return false;
	else if( key == "FPS" )
		return CGameConfig::DrawFPS;
	else if( key == "Weapons" )
		return CGameConfig::AllWeaponsSold;
	else if( key == "UnlockLevels" )
		return CGameConfig::UnlockAllLevels;

	return false;
}

bool CCheats::Activate( s32 x, s32 y )
{
	stringc key = "";

	TCheatList::Iterator iter = m_CheatsList.getIterator();
	for( s32 i=0; !iter.atEnd(); iter++, i++ )
	{
		CCheatField & field = iter->getValue();

		if( field.rect.isPointInside( p2i( x, y ) ) )
		{
			key = iter->getKey();
			break;
		}
	}
	
	CPlayfield* palyfield = CPlayfield::GetInstance();

	if( key == "Immortality" )
		CGameConfig::Invulnerable = !CGameConfig::Invulnerable;
	else if( key == "Money" )
		palyfield->GetLevelStats().Gold += 10000;
	else if( key == "Souls" )
		palyfield->GetLevelStats().Souls += 100;
	else if( key == "FPS" )
		CGameConfig::DrawFPS = !CGameConfig::DrawFPS;
	else if( key == "Weapons" )
	{
		CGameConfig::AllWeaponsSold = !CGameConfig::AllWeaponsSold;

		if( palyfield->IsLevelLoaded() )
		{
			palyfield->getHydra()->setLevel( WL_3 );
			palyfield->getHydra()->reset();
			palyfield->getGriffin()->setLevel( WL_3 );
			palyfield->getGriffin()->reset();
			palyfield->getFireball()->setLevel( WL_3 );
			palyfield->getFireball()->reset();
			palyfield->getIceball()->setLevel( WL_3 );
			palyfield->getIceball()->reset();
			palyfield->getGate()->setLevel( WL_3 );
			palyfield->getGate()->reset();
		}
	}
	else if( key == "UnlockLevels" )
		CGameConfig::UnlockAllLevels = !CGameConfig::UnlockAllLevels;

	return true;
}