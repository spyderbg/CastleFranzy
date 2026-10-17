/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _ANIMATION_CLIP_MANAGER_H_
#define _ANIMATION_CLIP_MANAGER_H_

#include <vector>
#include "maxscrpt/MAXScrpt.h"
#include "Interval.h"
#include "FMath\FMSort.h"
#include "FCDocument\FCDEventGroup.h"

#define ANIMCLIP_END_NOTEKEY_PREFIX "end_"
#define ANIMCLIP_STD_PREFIX "AUTO_"
#define ANIMCLIP_STD_SUFFIX "_Range"

#define SIMPLE_GETTER( retType, funcName, retVal ) \
inline retType funcName() \
{ \
	return retVal; \
}

#define SIMPLE_SETTER( argType, funcName, varName ) \
inline void funcName( argType val ) \
{ \
	varName = val; \
}

class DefNoteTrack;
class INode;
class ExportNode;
typedef fm::map<INode*, ExportNode*> ExportNodeMap;
typedef fm::map<TimeValue,float> TicksToSecsMap; // maps tick to it's corresponding sec value
typedef fm::map<Animatable *, TicksToSecsMap > AnimatableKeyTimeMap;

template <class T>
class AnimClip
{
public:
	enum
	{
		STANDARD_ANIM_CLIP = 0x00000001,
		IS_SELECTED        = 0x00000002 // UI specific
	};

private:
	T m_start;
	T m_end;
	fm::string m_name;
	int m_flags;
	
public:

	AnimClip( T begin, T end, fm::string name, int flags = 0 ) 
		:m_start( begin )
		, m_end( end )
		, m_name( name )
		, m_flags( flags )
		{}

	class NameComparator : public fm::comparator<AnimClip<T>>
	{
		/** Compares two elements in ascending order, using operator<.
			@param a The first element.
			@param b The second element.
			@return True if a is smaller than b, false otherwise.*/
		virtual bool compare(const AnimClip<T>& a, const AnimClip<T>& b)
		{
			return (a.m_name < b.m_name);
		}
	};

	inline public void SetStart( T time )
	{ 
		m_start = time; 
	}
	
	inline public T GetStart() const 
	{ 
		return m_start; 
	}

	inline public void SetEnd( T time )
	{ 
		m_end = time; 
	}
	
	inline public T GetEnd() const 
	{ 
		return m_end; 
	}

	inline public void SetFlags( int flags )
	{ 
		m_flags = flags; 
	}
	
	inline public int GetFlags() const 
	{ 
		return m_flags; 
	}

	inline public void SetName( const fstring &name )
	{ 
		m_name = name; 
	}
	
	inline public const fstring &GetName() const 
	{ 
		return m_name; 
	}
};

typedef AnimClip<int> AnimClip_Ticks;
typedef AnimClip<int> AnimClip_Frames;
typedef AnimClip<float> AnimClip_Secs;

// provided to avoir compile errors related to implementation of fm::comparator<T>::compare func
template <class T>
bool operator<( const AnimClip<T> &clipA, const AnimClip<T> &clipB );

// returns true only if every attribute of the clip is equivalent
template<class T>
bool IsEquivalent( const AnimClip<T> &clipA, const AnimClip<T> &clipB );

#define RESET_ANIM_RANGE( animRange ){ animRange.SetStart( INT_MAX ); animRange.SetEnd( INT_MIN );  }

class AnimationClipManager
{
public:
	enum GetAnimatableKeyTimesFlags
	{
		GAKTF_NONE                          = 0x00000000
		, GAKTF_IGNORE_BILLBOARD_CONTROLLER = 0x00000001
		, GAKTF_CONVERT_KEYTIMES_TOSECS     = 0x00000002
		, GAKTF_INCLUDE_MIXER_KEY_TIMES		= 0x00000004

		, GAKTF_DEFAULT = GAKTF_CONVERT_KEYTIMES_TOSECS | GAKTF_INCLUDE_MIXER_KEY_TIMES
	};

private:
	std::vector<AnimClip_Ticks> m_animClips;
	std::vector<std::string> m_animClipListBoxNames;
	std::vector<int> m_animClipsToExport;
	Interval m_sceneAnimRange;
	Interval m_userDefinedAnimClipsRange;
	Interval m_exportedAnimClipsRange;
	Interval m_totalAnimRange;
	TicksToSecsMap m_sceneKeyTimesMap;
	AnimatableKeyTimeMap m_mixerAnimatableKeyTimes;

	static AnimationClipManager *s_pAnimationClipManager;

private:
	AnimationClipManager()
	  {
	    RESET_ANIM_RANGE( m_sceneAnimRange );
	    RESET_ANIM_RANGE( m_userDefinedAnimClipsRange );
	    RESET_ANIM_RANGE( m_exportedAnimClipsRange );
	    RESET_ANIM_RANGE( m_totalAnimRange );
	  }
	~AnimationClipManager();

private:
	static DefNoteTrack *GetNoteTrack( INode *pNode );
	static int GetNoteKeyIndexByName( INode *pNode, const char *name );
	static int GetAnimClipStartNoteKeyIndex( INode *pNode, int clipId );

	Interval GetMixerKeyTimes( Animatable *pAnimatable, TicksToSecsMap &keyTimesMap );
	Interval GetSubAnimatableKeyTimes( Animatable *pAnimatable, TicksToSecsMap &keyTimesMap, int flags ); // returns sub-animatable anim key range
	void GetBipedNodeKeyTimes( INode *pNode, AnimatableKeyTimeMap &animatableKeyTimes );


public:
	void AddValidAnimClips( INode *pNode );

	// animation clips
	int GetNumAnimClips();

	const char *GetAnimClipListBoxName( int clipId );
	const char *GetAnimClipName( int clipId );
	int GetAnimClipStart( int clipId );
	int GetAnimClipEnd( int clipId );
	int GetAnimClipFlags( int clipId );
	void SetAnimClipFlags( int clipId, int flags );

	void PrepareAnimClipListBoxNames();
	Interval GetUserDefinedAnimClipsRange();
	Interval GetExportedAnimClipsRange();
	Interval GetSceneAnimRange();
	Interval GetTotalAnimRange();
	bool ValidateAnimRange( Interval animRange );

	void InitAnimClipsToExport();

	SIMPLE_GETTER( std::vector<int> &, GetAnimClipsToExport, m_animClipsToExport );
	SIMPLE_GETTER( int, GetNumAnimClipsToExport, m_animClipsToExport.size() );

	// also calculates the scene animation range
	Interval CalculateSceneAnimationKeyTimes( ExportNodeMap &exportNodes );
	void CalculateSceneMixerKeyTimes( ExportNodeMap &exportNodes );
	void GetSceneAnimationClips( ExportNodeMap &exportNodes );
	void CalculateExportedAnimClipsRange();

	int GetAnimClipId( const char *name );

	inline const TicksToSecsMap &GetSceneAnimationKeyTimesMap()
	{
		return m_sceneKeyTimesMap;
	}

public:
	static AnimationClipManager *GetInst();
	static void Release();

	Interval GetAnimatableKeyTimes( Animatable *pAnimatable, TicksToSecsMap &keyTimesMap, int flags = GAKTF_DEFAULT ); // returns animatable anim key range
	static fm::string GenerateStandardAnimClipName( fm::string name );
};

inline
AnimationClipManager *AnimationClipManager::GetInst()
{
	return s_pAnimationClipManager ? s_pAnimationClipManager : ( s_pAnimationClipManager = new AnimationClipManager() );
}

inline
void AnimationClipManager::Release()
{
	SAFE_DELETE( s_pAnimationClipManager );
}

#endif // _ANIMATION_CLIP_MANAGER_H_


