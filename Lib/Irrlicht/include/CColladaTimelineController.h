#ifndef __CCOLLADATIMELINECONTROLLER_H__
#define __CCOLLADATIMELINECONTROLLER_H__

#include "ITimelineController.h"
#include "SDatabaseCollada.h"

namespace irr
{
namespace collada
{

class CTimelineController : public irr::scene::ITimelineController
{
public:
	CTimelineController();
	~CTimelineController();

	void init(irr::s32 AnimStart, irr::s32 AnimEnd);
	
	inline float getLocalStartTime() const {
		return m_localStartTime;
	}
	inline float getLocalTimeRange() const {
		return m_localTimeRange;
	}
	virtual float getScale() const {
		return m_scale;
	}
	virtual void setScale(float scale) {
		m_scale = scale;
	}
	inline void setLocalTime(float _time) {
		m_localTime = _time;
	}
	inline void setLocalStartTime(float _time)	{
		m_localStartTime = _time;
	}
	inline void setLocalTimeRange(float _time)	{
		m_localTimeRange = _time;
	}
	virtual void setLoop(bool bLoop) {
		IsLooping = bLoop;
	}

	virtual bool getLoop() const {
		return IsLooping;
	}

	virtual void jumpTo(irr::s32 timeMs);
	virtual void update(irr::s32 timeMs);	

	void setAnimationClips(const SLibraryAnimationClips *library)
	{
		m_pAnimationClips = library;
		if (m_pAnimationClips && m_pAnimationClips->getClipCount()) {
			setClip(0);
		}
		else
		{
			//TODO-FH: set the entire clip lenght?
			setStart(0);
			setEnd(1);
		}
	}

	// ! This function should not be used when there is an animation clip
	virtual void setRange(irr::s32 start, irr::s32 end)
	{
		if(!m_pAnimationClips)
		{
			setStart(start);
			setEnd(end);

			//set the interval range
			setLocalStartTime(getStart() / 1000.0f);
			setLocalTimeRange((getEnd() - getStart()) / 1000.0f);
		}

		jumpTo(getStart());
	}

	inline const SLibraryAnimationClips * getAnimationClipsLibrary() const
	{
		return m_pAnimationClips;
	}

	const SLibraryAnimationClips * getAnimationClipsLibrary()
	{
		return m_pAnimationClips;
	}

	int			setClip(const char *pAnimId);
	void		setClip(int idx);
	int			getClipIndex(const char *pAnimId) const;

	irr::s32	getClipCount() const {
		if(!m_pAnimationClips)
		{
			return 0;
		}
		return m_pAnimationClips->getClipCount();
	}
	irr::s32	getClipStart(int idx) const {
		_IRR_DEBUG_BREAK_IF(!m_pAnimationClips);
		return m_pAnimationClips->getClipStart(idx);
	}
	irr::s32	getClipEnd(int idx) const {
		_IRR_DEBUG_BREAK_IF(!m_pAnimationClips);
		return  m_pAnimationClips->getClipEnd(idx);
	}
	irr::s32	getClipLength(int idx) const {
		return getClipEnd(idx) - getClipStart(idx);
	}
	irr::s32 getCurrentClipStart() const {
		return  getClipStart(m_iCurrentClip);
	}
	irr::s32 getCurrentClipEnd() const {
		return getClipEnd(m_iCurrentClip);
	}
	irr::s32 getCurrentClipLength() const {
		return getClipLength(m_iCurrentClip);
	}
	const char * getClipName(int idx) const {
		_IRR_DEBUG_BREAK_IF(!m_pAnimationClips);
		return m_pAnimationClips->getClipName(idx);
	}
	irr::s32 getCurrentClipIndex() const {
		return m_iCurrentClip;
	}
protected:
	//Interval Adjust
	bool  IsLooping;
	float m_localStartTime;
	float m_localTimeRange;
	float m_lastTime;
	float m_localTime;

	//Speed,Scale and Offset Adjust
	float m_scale;

	//Animation Clips
	const SLibraryAnimationClips *	m_pAnimationClips;
	int								m_iCurrentClip;

	bool m_bCallback;
	bool m_bLastTime;
};

}; // namespace collada
}; // namespace irr

#endif //__CCOLLADATIMELINECONTROLLER_H__
