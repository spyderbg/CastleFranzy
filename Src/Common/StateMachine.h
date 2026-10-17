// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StateMachine
//
//		State machine to help with the handling of the game logics
//
//	Notes
//		- sleep()/wakeup() warn _every_ States on the stack, not just the top-most one.
//		  (sleep() starts with the Last In, while wakeup() starts with the First In)
//		- When the StateMachine is deallocated, the remaining states have their dtor() called, followed by the real dtor
//
//	Warnings:
//		- StateMachine handles the NEW and the DELETE on the states. switchState and pushState returns the created
//		- Your state's dtor() or pause() are called as soon as you switch or push a state, even if your update() isn't done yet
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Common/Singleton.h"

#include "Entities/IUpdatable.h"
#include "Entities/IDrawable.h"
#include "Entities/ISleepable.h"

#include <vector>

///////////////////////////////////////////////////////////////////////

class IStateBase;

///////////////////////////////////////////////////////////////////////
/// CStateMachine
///////////////////////////////////////////////////////////////////////

class CStateMachine : public CSingleton<CStateMachine>, public IUpdatable, public IDrawable, public ISleepable
{
// ** Constants
// ////////////////////////////////////////////////////////
private:
	static const bool		SHOW_TRACE = false;
	static const irr::u32	INVALID_IDX = (irr::u32)-1;


// ** Typedefs
// ////////////////////////////////////////////////////////
private:
	typedef std::vector<IStateBase*>	StateList;
	typedef StateList::iterator			StateListIter;
	typedef StateList::const_iterator	StateListConstIter;
	typedef StateList::reverse_iterator	StateListRevIter;


// ** Members
// ////////////////////////////////////////////////////////
private:
	StateList			m_stateList;
	mutable irr::u32	m_stackIdx;
	mutable bool		m_isInDrawCallback;

	IStateBase*			m_switchState;
	IStateBase*			m_pushState;
	int					m_popState;

// ** Ctors / Dtor
// ////////////////////////////////////////////////////////
public:
						CStateMachine();
	virtual				~CStateMachine();


// ** Methods
// ////////////////////////////////////////////////////////
protected:
	void				_trace(const char* msg) const;
	void				_switchState(IStateBase* state);
	void				_pushState(IStateBase* state);

public:
	// Info
	int					stackSize()			const	{return m_stateList.size();};


	// API
	template <typename T>
	T*					switchState()				{T* t = new T(); _switchState(t); return t;};
	//switch to initialized state
	void				switchState( IStateBase *t ){ _switchState( t ); }
	template <typename T>
	T*					pushState()					{T* t = new T(); _pushState(t); return t;};
	void				popState();


	// Special - To call the state beneath, so it can draw itself as well
	void				recurseDraw()		const;


	// ** IUpdatable impl.
	void				update(TimeStamp timestamp);

	// ** IDrawable impl.
	void				draw()				const;
	int					getDrawingLayer()	const;

	// ** ISleepable impl.
	void				sleep();
	void				wakeup();
};

