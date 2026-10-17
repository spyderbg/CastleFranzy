// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EventManager
//
//		Handles the reception of events, and dispatch it to the subscribed classes
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IEvent.h"
#include "Entities/IUpdatable.h"

#include <map>
#include <list>


// ** Forward Declarations
// ////////////////////////////////////////////////////////
class IEventReceiver2;



// ** Main Class
// ////////////////////////////////////////////////////////
class EventManager : public IUpdatable
{
// ** Constants
// ////////////////////////////////////////////////////////
private:
	static const bool			SHOW_TRACE = false;


// ** Typedefs
// ////////////////////////////////////////////////////////
private:
	typedef std::list<IEventReceiver2*>			ReceiverList;
	typedef ReceiverList::iterator				ReceiverListIter;
	typedef ReceiverList::const_iterator		ReceiverListConstIter;

	typedef std::map<IEvent::ID, ReceiverList>	EventList;		// event -> list of subscribers
	typedef EventList::iterator					EventListIter;
	typedef EventList::const_iterator			EventListConstIter;

	typedef std::list<const IEvent*>			DelayedEventQueue;
	typedef DelayedEventQueue::iterator			DelayedEventQueueIter;
	typedef DelayedEventQueue::const_iterator	DelayedEventQueueConstIter;


// ** Members
// ////////////////////////////////////////////////////////
private:
	EventList					m_events;
	mutable DelayedEventQueue	m_eventQueue;


// ** Ctors / Dtor
// ////////////////////////////////////////////////////////
public:
								EventManager();
	virtual						~EventManager();


// ** Methods
// ////////////////////////////////////////////////////////
private:
	void						_trace(const char* msg) const;

public:
	bool						attach(IEvent::ID eventID, IEventReceiver2* eventReceiver);
	bool						detach(IEvent::ID eventID, IEventReceiver2* eventReceiver);

	void						raise(const IEvent& ev)	const;
	void						raise(const IEvent* ev)	const;

	void						raiseAsync(const IEvent* ev)	const;

	// ** IUpdatable impl.
	void						update(TimeStamp timestamp);
};

