///////////////////////////////////////////////////////////////////////
/// EventManager
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Common/EventManager.h"

#include "Entities/IEventReceiver.h"

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ctors / Dtor
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EventManager::EventManager()
{
}

EventManager::~EventManager()
{
	m_eventQueue.clear();
}


// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods (Privates/Protected)
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EventManager::_trace(const char* msg) const
{
	if(SHOW_TRACE)
	{
//		Logs::trace("EventManager", msg);
	}
}


// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EventManager::attach(IEvent::ID eventID, IEventReceiver2* eventReceiver)
{
	EventListIter eIter = m_events.find(eventID);

	if(eIter == m_events.end())
	{
		m_events[eventID].push_back(eventReceiver);
	}
	else
	{
		for(ReceiverListIter rIter = eIter->second.begin(); rIter != eIter->second.end(); ++rIter)
		{
			if((*rIter) == eventReceiver)
			{
				return false;
			}
		}

		eIter->second.push_back(eventReceiver);
	}

	return true;
}

bool EventManager::detach(IEvent::ID eventID, IEventReceiver2* eventReceiver)
{
	EventListIter eIter = m_events.find(eventID);

	if(eIter == m_events.end())
	{
		return false;
	}

	for(ReceiverListIter rIter = eIter->second.begin(); rIter != eIter->second.end(); ++rIter)
	{
		if((*rIter) == eventReceiver)
		{
			eIter->second.erase(rIter);
			return true;
		}
	}

	return false;
}


void EventManager::raise(const IEvent& ev) const
{
	EventListConstIter eIter = m_events.find(ev.getEventID());

	if(eIter != m_events.end())
	{
		const ReceiverList&		rList = eIter->second;
		ReceiverListConstIter	rIter = rList.begin();

		for(; rIter != rList.end(); ++rIter)
		{
			(*rIter)->onEvent(&ev, this);
		}
	}
}

void EventManager::raise(const IEvent* ev) const
{
	raise(*ev);
}


void EventManager::raiseAsync(const IEvent* ev) const
{
	m_eventQueue.push_back(ev);
}


// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods (IUpdatable impl.)
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EventManager::update(TimeStamp timestamp)
{
	while(m_eventQueue.size() > 0)
	{
		const IEvent* ev = m_eventQueue.front();
		raise(ev);
		delete ev;
		m_eventQueue.pop_front();
	}
}