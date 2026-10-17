/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDAnimationClip.h
	This file contains the FCDAnimationClip class.
*/

#ifndef _FCD_EVENT_GROUP_H_
#define _FCD_EVENT_GROUP_H_

typedef fm::vector<fstring> StringList;

template <class T>
class FCDEventGroup
{
private:
	T m_time;
	StringList m_events;

public:
	FCDEventGroup( T time = 0 )
		: m_time( time )
	{}

	~FCDEventGroup(){}

	inline void SetTime( T time )
	{
		m_time = time;
	}

	inline T GetTime() const
	{
		return m_time;
	}

	inline size_t GetNumEvents() const
	{
		return m_events.size();
	}
	
	inline void AddEvent( const fstring &event )
	{
		m_events.push_back( event );
	}

	inline void DeleteAllEvents()
	{
		m_events.clear();
	}

	inline void DeleteEvent( size_t index )
	{
		FUAssert( index < m_events.size(), return );
		m_events.erase( index );
	}

	inline void SetEvent( size_t index, const fstring &event )
	{
		FUAssert( index < m_events.size(), return );
		m_events[index] = event;
	}
	
	inline const fstring &GetEvent( size_t index ) const
	{
		return m_events[index];
	}
};

typedef fm::vector<FCDEventGroup<int>> EventGroups_Ticks;
typedef fm::vector<FCDEventGroup<int>> EventGroups_Frames;
typedef fm::vector<FCDEventGroup<float>> EventGroups_Secs;

#endif // _FCD_EVENT_GROUP_H_

