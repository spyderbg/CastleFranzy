////////////////////////////////////////////////////////////////////////////////////////////////////
/// StateMachine.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Common/StateMachine.h"

#include "Entities/IStateBase.h"

#include "Core/Logs.h"

#include "Game/Misc.h"

///////////////////////////////////////////////////////////////////////
/// CStateMachine
///////////////////////////////////////////////////////////////////////

CStateMachine::CStateMachine() :
	m_stackIdx(INVALID_IDX),
	m_isInDrawCallback(false),
	m_switchState(NULL),
	m_pushState(NULL),
	m_popState(0)
{
}

CStateMachine::~CStateMachine()
{
	/*
	for(StateListRevIter iter = m_stateList.rbegin(); iter != m_stateList.rend(); ++iter)
	{
		(*iter)->dtor();
		delete *iter;
	}

	if (m_switchState)
	{
		m_switchState->dtor();
		delete m_switchState;
	}

	if (m_pushState)
	{
		m_switchState->dtor();
		delete m_switchState;
	}
	*/
}


// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods (Privates/Protected)
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CStateMachine::_trace(const char* msg) const
{
	if(SHOW_TRACE)
	{
		Logs::trace("CStateMachine", msg);
	}
}


// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CStateMachine::_switchState(IStateBase* state)
{
	GX_ASSERT(!m_switchState && !m_pushState);
	GX_ASSERT(state);
	m_switchState = state;
}

void CStateMachine::_pushState(IStateBase* state)
{
	GX_ASSERT(!m_switchState && !m_pushState);
	GX_ASSERT(state);
	m_pushState = state;
}

void CStateMachine::popState()
{
	GX_ASSERT(!m_switchState && !m_pushState);
	GX_ASSERT(m_popState < m_stateList.size());
	++m_popState;
}


void CStateMachine::recurseDraw() const
{
	// Must be in the draw() callback
	if(m_isInDrawCallback)
	{
		// Backup of the stack index
		irr::u32 previousIdx = m_stackIdx;

		// If the stack index is -1, it means we are pointing
		// on the last element (the one on top of the stack)
		if(m_stackIdx == INVALID_IDX)
		{
			m_stackIdx = m_stateList.size() - 1;
		}

		// If the index isn't 0, it means there is a state
		// underneath the current one: we can recurse
		if(m_stackIdx > 0)
		{
			m_stateList[--m_stackIdx]->draw();
		}

		// Restoration of the stack index
		m_stackIdx = previousIdx;
	}
}


// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods (IUpdatable, IDrawable, ISleepable impl.)
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CStateMachine::update(TimeStamp timestamp)
{
	/// Switch State
	if (m_switchState)
	{
		m_popState = m_stateList.size();
		m_pushState = m_switchState;
		m_switchState = 0;
	}

	bool pauseBeforePush = (m_popState == 0);
	bool resumeAfterPop = !m_pushState;

	/// Pop State
	if (m_popState)
	{
		while (m_popState > 0)
		{
			m_stateList.back()->dtor();
			delete m_stateList.back();
			m_stateList.pop_back();
			--m_popState;
		}

		if (resumeAfterPop && m_stateList.size() > 0)
		{
			m_stateList.back()->resume();
		}
	}

	/// Push State
	if (m_pushState)
	{
		if (pauseBeforePush && m_stateList.size() > 0)
		{
			m_stateList.back()->pause();
		}

		RemoveUnusedTextures();
		
		m_stateList.push_back(m_pushState);

		m_stateList.back()->ctor();

		m_pushState = 0;
	}

	// Update States
	if (m_stateList.size() > 0)
	{
		m_stateList.back()->update( timestamp );
	}
}


void CStateMachine::draw() const
{
	if(m_stateList.size() > 0)
	{
		m_isInDrawCallback = true;
		m_stateList.back()->draw();
		m_isInDrawCallback = false;
	}
}

int CStateMachine::getDrawingLayer() const
{
	return IDrawable::LAYER_NOT_APPLICABLE;
}


void CStateMachine::sleep()
{
	__MGR_SOUNDS()->stopAllSounds();
	// We start with the Last In
	for(StateListRevIter iter = m_stateList.rbegin(); iter != m_stateList.rend(); ++iter)
	{
		(*iter)->sleep();
	}
}

void CStateMachine::wakeup()
{
	// We start with the First In
	for(StateListIter iter = m_stateList.begin(); iter != m_stateList.end(); ++iter)
	{
		(*iter)->wakeup();
	}
}