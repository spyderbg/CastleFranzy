#ifndef __CTIMELINECONTROLLER_H__
#define __CTIMELINECONTROLLER_H__

#include <ITimelineController.h>
#include <irrString.h>
#include "SLibraryAnimationClips.h"

namespace irr
{
namespace scene
{
class CTimelineController
	: public ITimelineController
{
public:
	CTimelineController() 
		: m_LastUpdateTime(0)
		, m_bLoop(true)
		, m_pLibraryAnimationClips(0)
		, m_iCurrentClip(0)
	{
	}

	virtual ~CTimelineController()
	{
		if(m_pLibraryAnimationClips)
			m_pLibraryAnimationClips->drop();
	}
	
	virtual void setLoop(bool bLoop) { m_bLoop = bLoop; }

	//! Allow a jump of the controller relative time.
	virtual void jumpTo(s32 timeMS)
	{
		timeMS = core::clamp<s32>(timeMS, getCurrentClipStart(), getCurrentClipEnd());
		setCtrlTime(timeMS);

		if(timeMS >= getCurrentClipEnd())
		{
			if(m_bLoop)
			{
				setCtrlTime(getCurrentClipStart());
			}
			else
			{
				invokeCallback();
			}
		}
	};

	//! Get a transformated time based on the current time
	virtual void update(s32 timeMs) 
	{
		s32 dt = timeMs - m_LastUpdateTime;

		// TODO:
		// Temporary hack to fix the dt > time line
		dt = core::clamp<s32>(dt, 0, 34);
		
		m_LastUpdateTime = timeMs;
		setCtrlTime(getCtrlTime() + dt);
		if(getCtrlTime() >= getCurrentClipEnd())
		{
			if(m_bLoop)
			{
				setCtrlTime(getCurrentClipStart());
			}
			else
			{
				invokeCallback();
			}
		}
	}

	void
	setAnimationClipsLibrary(CLibraryAnimationClips *library)
	{
		if(m_pLibraryAnimationClips)
			m_pLibraryAnimationClips->drop();

		m_pLibraryAnimationClips = library;

		if(m_pLibraryAnimationClips)
		{
			m_pLibraryAnimationClips->grab();
			setClip(0);
		}
		else
		{
			setStart(0);
			setEnd(1);
		}
	}

	virtual void
	setClip(int idx)
	{
		_IRR_DEBUG_BREAK_IF(!m_pLibraryAnimationClips);
		_IRR_DEBUG_BREAK_IF(idx < 0 || idx > (int)m_pLibraryAnimationClips->clips.size());

		m_iCurrentClip = idx;

		setCtrlTime(getCurrentClipStart());
		setStart(getCurrentClipStart());
		setEnd(getCurrentClipEnd());
	}

	//! Set the current animation clip with a clip string id
	//! \return: Return the animation clip index for this string id.
	virtual int
	setClip(const char *pAnimId)
	{
		_IRR_DEBUG_BREAK_IF(!m_pLibraryAnimationClips);

		int animId = getClipIndex(pAnimId);
		if(animId >= 0)
		{
			setClip(animId);
		}
		return animId;
	}

	//! \return: Return the animation index for this string id.
	virtual int getClipIndex(const char *pAnimId) const
	{
		_IRR_DEBUG_BREAK_IF(!m_pLibraryAnimationClips);

		for(int i = 0, sz = m_pLibraryAnimationClips->clips.size(); i < sz; i++)
		{
			if(m_pLibraryAnimationClips->clips[i].id == pAnimId)
			{
				return i;
			}
		}

		return -1;
	}

	CLibraryAnimationClips *
	getAnimationClipsLibrary()
	{
		return m_pLibraryAnimationClips;
	}

	virtual const char *
	getClipName(int idx) const
	{
		_IRR_DEBUG_BREAK_IF(!m_pLibraryAnimationClips);
		return m_pLibraryAnimationClips->clips[idx].id.c_str();
	}	

	virtual s32
	getClipStart(int idx) const
	{
		_IRR_DEBUG_BREAK_IF(!m_pLibraryAnimationClips);
		return m_pLibraryAnimationClips->clips[idx].start;
	}

	virtual s32
	getClipEnd(int idx) const
	{
		_IRR_DEBUG_BREAK_IF(!m_pLibraryAnimationClips);
		return m_pLibraryAnimationClips->clips[idx].end;
	}

	virtual s32
	getClipLength(int idx) const
	{
		return getClipEnd(idx) - getClipStart(idx);
	}

	virtual s32
	getCurrentClipStart() const
	{
		return  getClipStart(m_iCurrentClip);
	}

	virtual s32
	getCurrentClipEnd() const
	{
		return getClipEnd(m_iCurrentClip);
	}

	virtual s32
	getCurrentClipLength() const
	{
		return getClipLength(m_iCurrentClip);
	}

	virtual s32 
	getClipCount() const
	{
		_IRR_DEBUG_BREAK_IF(!m_pLibraryAnimationClips);
		return m_pLibraryAnimationClips->clips.size();
	}

protected:
	int m_iCurrentClip;
	CLibraryAnimationClips *m_pLibraryAnimationClips;
	static const s32 m_MaxTimeDT = 1000;
	s32 m_LastUpdateTime;
	bool m_bLoop;
};
};
};

#endif
