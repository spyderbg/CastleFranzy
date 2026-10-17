/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _ANIMATION_SELECTOR_H_
#define _ANIMATION_SELECTOR_H_

#include "resource.h"
#include <guplib.h>
#include "AnimationClipManager.h"

#define ANIMATION_SELECTOR_ID Class_ID(0x44305141, 0x52c37a89)

ClassDesc2* GetAnimationSelectorClassDesc();

namespace ICAnimationSelectorBackwardCompatibility
{
	enum AnimationSelectorVersion
	{
		ASV_CURRENT_VERSION = 0
	};

	enum AnimationClipsVersion
	{
		ACV_CURRENT_VERSION = 0
	};

	enum EventGroupVersion
	{
		EGV_CURRENT_VERSION = 0
	};
};

// Class Desc
class AnimationSelectorClassDesc : public ClassDesc2
{
public:
	int				IsPublic() {return TRUE; }
	void*			Create(BOOL isLoading);

	const TCHAR*	ClassName() {return GetString(IDS_AS_CLASS_NAME);}
	SClass_ID		SuperClassID() {return GUP_CLASS_ID;}
	Class_ID 		ClassID() {return ANIMATION_SELECTOR_ID;}

	const TCHAR* 	Category() {return _T("");}
	const TCHAR*	InternalName() { return GetString(IDS_AS_CLASS_NAME); }
	HINSTANCE		HInstance() { return hInstance; }

	// Load/Save
	BOOL NeedsToSave() { return FALSE; }
	IOResult Save(ISave *is){ return IO_OK; }
	IOResult Load(ILoad *il){ return IO_OK; }
};

class ICAnimationSelector : public GUP
{
public:
	// New data chunk ids must always be new enum values.
	// The enum values determine the association between chunks in the Max file and specific data.
	enum
	{
		CID_ANIMATION_CLIPS
		, CID_EVENT_GROUPS

		// Version number for all data saved by animation selector
		, CID_ANIMATION_SELECTOR_VERSION

		// Version number for animation clip data and event groups.  These are an exception.  
		// Other animation selector data will have their version number saved
		// in their respective chunk.
		, CID_ANIMATION_CLIPS_VERSION
		, CID_EVENT_GROUPS_VERSION
	};

private:
	fm::vector<AnimClip_Frames> m_animationClips;
	fm::vector<FCDEventGroup<int>> m_eventGroups;

private:

	void Save();
	void Load();

	void SaveAnimationSelectorVersion();
	void LoadAnimationSelectorVersion();

	void SaveAnimationClipsVersion();
	void LoadAnimationClipsVersion();

	void SaveAnimationClips();
	void LoadAnimationClips();

	void SaveEventGroupsVersion();
	void LoadEventGroupsVersion();

	void SaveEventGroups();
	void LoadEventGroups();

public:
	ICAnimationSelector(BOOL isLoading = FALSE);
	~ICAnimationSelector();

	void Reset();
	DWORD Start (){ return GUPRESULT_KEEP; }
	void Stop (){}

	int AddAnimationClip( fm::string name, int startTime, int endTime );
	int DeleteAnimationClip( size_t index );

	size_t GetNumAnimationClips() const;
	AnimClip_Frames &GetAnimationClip( size_t index );
	const AnimClip_Frames &GetAnimationClip( size_t index ) const;

	int GetAnimationClipStart( size_t index ) const;
	int GetAnimationClipEnd( size_t index ) const;
	const fm::string &GetAnimationClipName( size_t index ) const;

	int SetAnimationClipStart( size_t index, int start );
	int SetAnimationClipEnd( size_t index, int end );
	int SetAnimationClipName( size_t index, fm::string name );

	int GetAnimationClipIndexByName( const fm::string clipName ) const;

	inline public size_t GetNumEventGroups() const 
	{ 
		return m_eventGroups.size(); 
	}
	
	inline public FCDEventGroup<int> &GetEventGroup( size_t index )
	{ 
		return m_eventGroups[index]; 
	}
	
	inline public const FCDEventGroup<int> &GetEventGroup( size_t index ) const 
	{ 
		return m_eventGroups[index]; 
	}
	
	inline public void AddEventGroup( const FCDEventGroup<int> &eventGroup )
	{ 
		m_eventGroups.push_back( eventGroup );
	}
	
	inline public int DeleteEventGroup( size_t index )
	{ 
		FUAssert( index < m_eventGroups.size(), return IC_ERROR );
		m_eventGroups.erase( index );
		return IC_OK;
	}

	inline public fm::vector<FCDEventGroup<int>> *GetEventGroups()
	{ 
		return &m_eventGroups;
	}

	static void NotificationCallback( void *param, NotifyInfo *info );
};

#endif // _ANIMATION_SELECTOR_H_


