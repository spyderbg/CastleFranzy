/*
 *  EvVirtualButton.h
 *  Gangstar2
 *
 *  Created by covey on 11/11/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Events/Events.h"
#include "Entities/IEvent.h"

///////////////////////////////////////////////////////////////////////

namespace EvVButton
{
	namespace ButtonState
	{
		enum State
		{
			PRESSED = 0,
			HELD,
			RELEASED,
		};
	}
	
	namespace ButtonType
	{
		enum Type
		{
			ATTACK = 0,
			LOCK,
			ENTER_CAR,
		};
	}
}

	// ** Main Class - EvVirtualButton
	// ////////////////////////////////////////////////////////
class EvVirtualButton : public IEvent
{
	// ** Structs
	// ////////////////////////////////////////////////////////
	
	// ** Typedefs
	// ////////////////////////////////////////////////////////
private:
	typedef EvVButton::ButtonState::State VBState;
	typedef EvVButton::ButtonType::Type   VBType;

	// ** Members
	// ////////////////////////////////////////////////////////
public:
	VBState state;
	VBType  type;
	
	// ** Ctors / Dtor
	// ////////////////////////////////////////////////////////
public:
	EvVirtualButton(VBState _state, VBType _type) : 
	IEvent(Events::VIRTUAL_BUTTON),
	state(_state),
	type(_type)
	{};
	
	virtual	~EvVirtualButton() {};
};
