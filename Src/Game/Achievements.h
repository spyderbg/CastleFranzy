///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Common/Singleton.h"

#include "Entities/IDrawable.h"
#include "Entities/IUpdatable.h"

///////////////////////////////////////////////////////////////////////

#define __ACHIEVEMENTS()		( CAchievements::GetInstance() )

///////////////////////////////////////////////////////////////////////

class ASprite;
class CAchievements;

///////////////////////////////////////////////////////////////////////

typedef	enum {
		EMS_Wait,
		EMS_MoveRight,
		EMS_Visible,
		EMS_MoveLeft
} EventMessageState;

typedef struct 
{
	const char* name;

	const irr::u32	achieve_value;

	irr::u32	anim;
	irr::u32	name_id;
	irr::u32	dcrp_id;

} ACHIEVEMENT;

extern const ACHIEVEMENT	g_AchievementsList[];
extern const irr::s32		g_AchievementsListSize;

typedef irr::core::map<irr::core::stringc, irr::s32>		TAchievements;


///////////////////////////////////////////////////////////////////////
/// CAchievements
///////////////////////////////////////////////////////////////////////

class CAchievements : public CSingleton<CAchievements>, public IDrawable, public IUpdatable
{
public:
	CAchievements();
	~CAchievements();

	void		draw()				const;
	int			getDrawingLayer()	const;
	void		update( TimeStamp timestamp );

	void		reset();	// set to default values

	bool		resetAchievementsByProfile( const char* profile_name );	// Reset ahievements for a given profile name
	bool		loadAchievementsByProfile( const char* profile_name );	// Load ahievements for a given profile name
	bool		saveAchievementsByProfile( const char* profile_name );	// Save ahievements for a given profile name

	bool		isAchieved( const char* name ) const;		

	irr::u32	getValue( const char* name ) const;				// return current value for an achievement
	void		setValue( const char* name, irr::u32 value );	// set current value for an achievement and test is it achieve or not
	void		addValue( const char* name, irr::u32 value );	// increase an achievement value and check is it achieve
	void		setAchieved( const char* name );				// dircetly set value to its achieve value 

private:
	irr::u32	getAchieveValue( const char* name ) const;
	bool		isNameExists( const char* name ) const;

	void		unlockAchievement( const char* name );

private:
	irr::core::stringc	m_LastLoadedProfileName;
	TAchievements		m_Achievements;

///////////////////////////////////////////////////////////////////////
/// Event message
///////////////////////////////////////////////////////////////////////

public:
	void		init();
	void		release();

	void		eventMsgPush( irr::core::stringw text );

protected:
	irr::s32	eventMsgSize()		const					{ return m_eventMsgList.size(); }
	bool		hasEventMsgInList() const					{ return eventMsgSize() > 0; }

	void		eventMsgErase();

	void		eventMsgUpdatePos( irr::f32 percent );
	void		eventMsgUpdate( irr::f32 dt );
	void		eventMsgDraw()	const;

private:
	ASprite*					m_sprite;

	irr::core::array<irr::core::stringw>	m_eventMsgList;
	irr::core::stringw			m_eventMsg;
	EventMessageState			m_eventMsgState;
	irr::f32					m_eventMsgViewTime;

	irr::s32					m_eventMsgBgOffset;
};
