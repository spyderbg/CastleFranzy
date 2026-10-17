// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// StateBase
//
//		State to be used with the StateMachine
//
//	Notes:
//		- Methods are virtual, but not pure. That way, a child class can decide to not implement some methods.
//		- Also, only update() receive a non-const StateMachine, which means States should not be calling switch/push/pop
//		  methods while in another callback.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

class StateMachine;

///////////////////////////////////////////////////////////////////////
/// IStateBase
///////////////////////////////////////////////////////////////////////

class IStateBase
{
	friend			class CStateMachine;

protected:
					IStateBase()					{};
public:
	virtual			~IStateBase()					{};


public:
	// Internal callbacks (done by the StateMachine itself)
	virtual void	ctor() {};		// when added to the StateMachine stack
	virtual void	dtor() {};		// when removed from the StateMachine stack
	virtual void	pause() {};		// when a new state is added atop
	virtual void	resume() {};	// when the state above is removed

	// External callbacks
	virtual void	update( float dt ) {};	// game update: update
	virtual void	draw()	{};				// game update: draw
	virtual void	sleep()	{};				// application is interrupted
	virtual void	wakeup()	{};			// application is resumed

};
