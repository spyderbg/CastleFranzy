#include "StdAfx.h"
#include "CColladaTimelineController.h"

#include "IrrPlatform.h"	//for irr::stricmp

namespace irr
{
namespace collada
{

CTimelineController::CTimelineController()
: IsLooping(false)
, m_localStartTime(0.0f)
, m_localTimeRange(0.0f)
, m_localTime(0)
, m_scale(1.0f)
, m_pAnimationClips(0)
, m_iCurrentClip(0)
, m_bCallback(false)
, m_bLastTime(false)
{
}

CTimelineController::~CTimelineController()
{
}

void CTimelineController::init(irr::s32 AnimStart, irr::s32 AnimEnd)
{
	setStart(AnimStart);
	setEnd(AnimEnd);
}

void CTimelineController::jumpTo(irr::s32 timeMs)
{
	setCtrlTime(timeMs);
	setLocalTime(timeMs/1000.f);

	m_bCallback = false;
	m_bLastTime = false;
}

void CTimelineController::update(irr::s32 timeMs)
{
	float time = timeMs / 1000.0f;
	float dt = (time - m_lastTime)*m_scale;
	float endTime = getEnd() / 1000.0f;
	
	// Prevent large dt value to corrupt the animation sequence
	if(!m_bLastTime)
	{
		dt = 0;
		m_bLastTime = true;
	}

	m_localTime += dt;
	m_lastTime = time;

	if(m_localTime > endTime)
	{
		if(IsLooping)
		{
			float forwardTime = 0;
			if(m_localTimeRange != 0)
			{
				forwardTime = fmodf((m_localTime - endTime), m_localTimeRange);
			}
			m_localTime = m_localStartTime + forwardTime;
			invokeCallback();
		}
		else
		{
			m_localTime = endTime;
			if(!m_bCallback)
			{
				m_bCallback = true;
				invokeCallback();				
			}
		}		
	}	
	
	setCtrlTime(s32(m_localTime * 1000 + 0.5f));
}

int	CTimelineController::setClip(const char *pAnimId)
{
	_IRR_DEBUG_BREAK_IF(!m_pAnimationClips);

	int animId = getClipIndex(pAnimId);
	if(animId >= 0)
	{
		setClip(animId);
	}
	return animId;
}

void CTimelineController::setClip(int idx)
{
	_IRR_DEBUG_BREAK_IF(!m_pAnimationClips);
	_IRR_DEBUG_BREAK_IF(idx < 0 || idx > m_pAnimationClips->getClipCount());

	m_iCurrentClip = idx;
	m_bCallback = false;

	//set the clip range
	setStart(getCurrentClipStart());
	setEnd(getCurrentClipEnd());

	//set the interval range
	setLocalStartTime(getStart() / 1000.0f);
	setLocalTimeRange((getEnd() - getStart()) / 1000.0f);

	//set the local time
	setLocalTime(getLocalStartTime());

	//set the local time to the beginning of the clip
	setCtrlTime(getStart());

	m_bLastTime = false;
}

int CTimelineController::getClipIndex(const char *pAnimId) const
{
	_IRR_DEBUG_BREAK_IF(!m_pAnimationClips);
	for(int i = 0; i < m_pAnimationClips->getClipCount(); ++i)
	{
		const char* clipName = m_pAnimationClips->getClipName(i);
		if(irr::stricmp(clipName, pAnimId) == 0)
		{
			return i;
		}
	}
	return -1;
}


}; // namespace collada
}; // namespace irr
