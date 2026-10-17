#ifndef __ITIMELINECONTROLLER_H__
#define __ITIMELINECONTROLLER_H__

#include <ITimeController.h>
#include <irrString.h>

namespace irr
{
namespace scene 
{
class ITimelineController
	: public ITimeController
{
public:
	ITimelineController() : m_FxCallback(0) {}

	//! Set the callback that will be invoked at the end of the current timeline
	void setCallback(void (*callback)(ITimelineController*, void *), void *param)
	{
		m_FxCallback = callback;
		m_pCallbackData = param;
	}

	//! Allow a jump of the controller relative time.
	virtual void jumpTo(s32 timeMs) 
	{ 
		setCtrlTime(timeMs);
	}

	//! Get the Length in Ms of the current time line.
	inline s32 getLength() const 
	{ 
		return m_EndMs - m_StartMs;
	}

	//! Get the start time in Ms of the current time line.
	inline s32 getStart() const 
	{ 
		return m_StartMs;
	}

	//! Get the end time in Ms of the current time line.
	inline s32 getEnd() const 
	{ 
		return m_EndMs;
	}

	//! Set the current animation clip with a clip index
	virtual void setClip(int idx) = 0;

	//! Set the current animation clip with a clip string id
	virtual int setClip(const char *pAnimId) = 0;

	//! Get the index mapped to a given animation string id.
	virtual int getClipIndex(const char *pAnimId) const = 0;
	virtual s32 getClipCount() const = 0;
	virtual s32 getClipStart(int idx) const = 0;
	virtual s32 getClipEnd(int idx) const = 0;
	virtual s32 getClipLength(int idx) const = 0;
	virtual s32 getCurrentClipStart() const = 0;
	virtual s32 getCurrentClipEnd() const = 0;
	virtual s32 getCurrentClipLength() const = 0;
	virtual int getCurrentClipIndex() const = 0;
	virtual const char * getClipName(int idx) const = 0;
	virtual void setLoop(bool bLoop) = 0;
	
	virtual bool getLoop() const {
		_IRR_DEBUG_BREAK_IF(true);
		return false;
	}

	virtual void setScale(float scale) {
		_IRR_DEBUG_BREAK_IF(true);
	}
	virtual float getScale() const {
		_IRR_DEBUG_BREAK_IF(true);
		return 1.0f;
	}

	virtual void setRange(irr::s32 start, irr::s32 end) = 0;

protected:
	//! Get the start time in Ms of the current time line.
	inline void 
	setStart(s32 startMs) 
	{ 
		m_StartMs = startMs;
	}

	//! Get the end time in Ms of the current time line.
	inline void
	setEnd(s32 endMs) 
	{ 
		m_EndMs = endMs;
	}

	void invokeCallback()
	{
		if(m_FxCallback)
		{
			m_FxCallback(this, m_pCallbackData);
		}
	}

private:
	void (*m_FxCallback)(ITimelineController*, void *);
	void *m_pCallbackData;
	s32 m_StartMs;
	s32 m_EndMs;
};
};
};

#endif