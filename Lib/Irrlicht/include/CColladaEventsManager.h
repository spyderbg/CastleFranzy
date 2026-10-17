#ifndef __CCOLLADAEVENTSTRACK_H__
#define __CCOLLADAEVENTSTRACK_H__

#include "SDatabaseCollada.h"
#include "IReferenceCounted.h"

namespace irr
{
namespace collada
{


struct STriggeredEvent
{
	s32				elapsedTime;
	const char *	name;
};

typedef void (*FxEventTrackCallback)(const STriggeredEvent &event, void *userData);

static void FxEchoNotHandledEvents(const STriggeredEvent &event, void *userData)
{
	os::Printer::log("Warning - Event not handled : See collada::CSceneNodeAnimator::setEventsCallback");
	os::Printer::log((char*)event.name);
}

#ifdef DEBUG
#define DEFAULT_HANDLER 0
#else
#define DEFAULT_HANDLER &FxEchoNotHandledEvents
#endif

class CEventsManager
	: public IReferenceCounted
{
public:
	CEventsManager(const SEventsTrack &track)
		: Track(track)
		, DispatchFunc(DEFAULT_HANDLER)
		, UserData(0)
	{

	}

	int findEntry(s32 time)
	{
		// Dummy seach ... We will take the one from AnimationTrack
		for(int i = 0; i < Track.times.iData.size(); i++)
		{
			if(time < Track.times.iData[i])
				return i - 1;
		}
		return Track.times.iData.size() - 1;
	}

	void dispatchEvents(s32 entryFrom, s32 entryTo, s32 currentTime)
	{
		_IRR_DEBUG_BREAK_IF(!DispatchFunc);
		for(int i = entryFrom; i <= entryTo; i++)
		{
			for(int j = 0; j < Track.events[i].size(); j++)
			{
				STriggeredEvent event;
				event.elapsedTime = currentTime - Track.times.iData[i];
				event.name = (const char *)Track.events[i][j];
				DispatchFunc(event, UserData);
			}
		}
	}

	void onUpdate(s32 timeFrom, s32 timeTo)
	{
		if(!DispatchFunc)
		{
			return;
		}

		dispatchEvents(findEntry(timeFrom) + 1, findEntry(timeTo), timeTo);
	}

	void onUpdate(s32 timeFrom, s32 timeTo, s32 timeBegin, s32 timeEnd)
	{
		if(timeFrom == timeTo || !DispatchFunc)
		{
			return;
		}

		if(timeFrom <= timeTo)
		{
			dispatchEvents(findEntry(timeFrom - 1) + 1, findEntry(timeTo), timeTo);
		}
		else
		{
			dispatchEvents(findEntry(timeFrom - 1) + 1, findEntry(timeEnd), timeEnd + (timeTo - timeBegin));
			dispatchEvents(findEntry(timeBegin - 1) + 1, findEntry(timeTo), timeTo);
		}
	}

	void setCallback(FxEventTrackCallback callback, void *userData = 0) 
	{ 
		DispatchFunc = callback; 
		UserData = userData;
	}

protected:
	const SEventsTrack &Track;
	FxEventTrackCallback DispatchFunc;
	void *UserData;
};

};
};

#endif // __CCOLLADAEVENTSTRACK_H__