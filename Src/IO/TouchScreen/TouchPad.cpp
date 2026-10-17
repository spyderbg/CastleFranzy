////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IO/TouchScreen/TouchPad.h"

#include "stdlib.h"
#include <algorithm>


////////////////////////////////////////////////////////////////////////////////////////////////////
// Maths and debug functionality wrappers follow
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef DBG_SOUND
	#define DBG_SOUND (0)
#endif //DEBUG_OUT

#ifndef ANGLE_TO_POINT
	#define ANGLE_TO_POINT(dx, dy) AngleDiff(dx, dy, 0)
	#define ANGLE_TO_POINT_BASE (1024)
#endif //ANGLE_TO_POINT

#ifndef COMPONENT_X
	#define COMPONENT_X(angle, amplitude) ((amplitude * COSIN(angle - ANGLE_TO_POINT_BASE)) >> COS_SIN_SHIFT)
	#define COMPONENT_Y(angle, amplitude) ((amplitude * SINUS(angle - ANGLE_TO_POINT_BASE)) >> COS_SIN_SHIFT)
#endif //COMPONENT_X






//============================================================================
//============================================================================
//
// CTouchPad
//
//============================================================================
//============================================================================

E_ORIENTATION CTouchPad::s_currentDeviceOrientation = EOO_270;
int CTouchPad::s_currentTime = 0;

#if defined(OS_WIN32) || defined(OS_LINUX)
	int CTouchPad::s_currentTiltX = -60;
#else
	int CTouchPad::s_currentTiltX = 0;
#endif
int CTouchPad::s_currentTiltY = 0;
int CTouchPad::s_currentTiltZ = 0;

int CTouchPad::s_currentTiltAcclerationX = 0;
int CTouchPad::s_currentTiltAcclerationY = 0;
int CTouchPad::s_currentTiltAcclerationZ = 0;

int CTouchPad::s_touchStartX[k_max_touches];
int CTouchPad::s_touchStartY[k_max_touches];

int CTouchPad::s_touchCurrentX[k_max_touches];
int CTouchPad::s_touchCurrentY[k_max_touches];
int CTouchPad::s_touchCurrentState[k_max_touches] = {State_Off};

int CTouchPad::s_touchPrevX[k_max_touches];
int CTouchPad::s_touchPrevY[k_max_touches];
int CTouchPad::s_touchPrevState[k_max_touches] = {State_Off};

int CTouchPad::s_touchType[k_max_touches] = {Type_AnyTouch};
int CTouchPad::s_touchTimestamp[k_max_touches];
bool CTouchPad::s_touchHasNewEvent[k_max_touches] = {false};


//============================================================================
/// @brief Initializes the working variables
//============================================================================
void CTouchPad::InitTouchPad(bool resetTouches, bool resetTilt)
{
	if (resetTouches)
	{
		for(int i = 0; i < k_max_touches; i++)
		{
			s_touchStartX[i] = -1;
			s_touchStartY[i] = -1;

			s_touchCurrentX[i] = -1;
			s_touchCurrentY[i] = -1;
			s_touchCurrentState[i] = State_Off;

			s_touchPrevX[i] = -1;
			s_touchPrevY[i] = -1;
			s_touchPrevState[i] = State_Off;	

			s_touchType[i] = Type_AnyTouch;
			s_touchTimestamp[i] = -1;
			s_touchHasNewEvent[i] = false;
		}
	}

	if (resetTilt)
	{
		s_currentTiltX = 0;
		s_currentTiltY = 0;
		s_currentTiltZ = 0;

		s_currentTiltAcclerationX = 0;
		s_currentTiltAcclerationY = 0;
		s_currentTiltAcclerationZ = 0;
	}

	//s_currentDeviceOrientation = ORIENTATION_LANDSCAPE_270;
}



//============================================================================
/// @brief Transforms the supplied x and y screen coordinates based on the current device orientation
//============================================================================
bool CTouchPad::TransformCoordinatesByOrientation(int& x, int& y)
{
	bool coordinatesAreModified = false;
	
	if ( s_currentDeviceOrientation == EOO_90 )
	{
		int a = y;
		y = x;
		x = OS_SCREEN_W - a;

		coordinatesAreModified = true;
	}
	else if ( s_currentDeviceOrientation == EOO_270 )
	{
		int a = x;
		x = y;
		y = OS_SCREEN_H - a;

		coordinatesAreModified = true;
	}

	return coordinatesAreModified;
}

bool CTouchPad::InverseTransformCoordinatesByOrientation(int& x, int& y)
{
	bool coordinatesAreModified = false;
	
	if ( s_currentDeviceOrientation == EOO_90 )
	{
		int a = x;
		x = y;
		y = OS_SCREEN_W - a;

		coordinatesAreModified = true;
	}
	else if ( s_currentDeviceOrientation == EOO_270 )
	{
		int a = y;
		y = x;
		x = OS_SCREEN_H - a;

		coordinatesAreModified = true;
	}

	return coordinatesAreModified;
}



//============================================================================
/// @brief Duplicates the current touch coordinates into the "prev" variables
//============================================================================
void CTouchPad::SetPrevState(int touch_index)
{
	s_touchPrevX[touch_index] = s_touchCurrentX[touch_index];
	s_touchPrevY[touch_index] = s_touchCurrentY[touch_index];
	s_touchPrevState[touch_index] = s_touchCurrentState[touch_index];
}



//============================================================================
/// @brief Call this to notify for a touch event
//============================================================================
void CTouchPad::EventSetTilt(int tilt_x, int tilt_y, int tilt_z)
{
	s_currentTiltX = tilt_x;
	s_currentTiltY = tilt_y;
	s_currentTiltZ = tilt_z;
}



//============================================================================
/// @brief Call this to notify for a touch event
//============================================================================
void CTouchPad::EventSetTiltAcceleration(int acc_x, int acc_y, int acc_z)
{
	s_currentTiltAcclerationX = acc_x;
	s_currentTiltAcclerationY = acc_y;
	s_currentTiltAcclerationZ = acc_z;
}



//============================================================================
/// @brief Call this to notify for a touch event
//============================================================================
void CTouchPad::EventTouchDown(int touch_index, int x, int y)
{
	if (k_debug_output)  
	{
		DBG_OUT("touch down i=%d x=%d y=%d\n", touch_index, x, y);
	}

	#if !defined(WIN32) && !defined(OS_LINUX)
		TransformCoordinatesByOrientation(x, y);
	#endif //WIN32

	s_touchStartX[touch_index] = x;
	s_touchStartY[touch_index] = y;

	s_touchCurrentX[touch_index] = x;
	s_touchCurrentY[touch_index] = y;
	s_touchCurrentState[touch_index] = State_On;

	s_touchType[touch_index] = Type_AnyTouch;
	s_touchTimestamp[touch_index] = s_currentTime;

	s_touchPrevX[touch_index] = x;
	s_touchPrevY[touch_index] = y;
	s_touchPrevState[touch_index] = State_Off;

	s_touchHasNewEvent[touch_index] = true;

	#if DBG_SOUND
		cGame* g_pGame = CGame::getGame();
		g_pGame->DbgPlaySoundId(g_pGame->m_nSoundIdTest++); 
		if (g_pGame->m_nSoundIdTest == 213) g_pGame->m_nSoundIdTest = 0; 
	#endif// DBG_SOUND

	CTouchElement::UpdateElementsByTouch(touch_index);
}



//============================================================================
/// @brief Call this to notify for a touch event
//============================================================================
void CTouchPad::EventTouchMove(int touch_index, int x, int y)
{
	#if !defined(WIN32) && !defined(OS_LINUX)
		TransformCoordinatesByOrientation(x, y);
	#endif //WIN32

	if (k_debug_output)  
	{
		DBG_OUT("touch move i=%d x=%d y=%d\n", touch_index, x, y);
	}
	
	s_touchCurrentX[touch_index] = x;
	s_touchCurrentY[touch_index] = y;
	//s_touchCurrentState[touch_index] = State_On;

	int total_dx = ABS(s_touchCurrentX[touch_index] - s_touchStartX[touch_index]);
	int total_dy = ABS(s_touchCurrentY[touch_index] - s_touchStartY[touch_index]);
}



//============================================================================
/// @brief Call this to notify for a touch event
//============================================================================
void CTouchPad::EventTouchUp(int touch_index, int x, int y)
{
	if (k_debug_output)  
	{
		DBG_OUT("touch up i=%d x=%d y=%d\n", touch_index, x, y);
	}

	if (x == -1) x = s_touchCurrentX[touch_index];
	if (y == -1) y = s_touchCurrentY[touch_index];
	
	#if !defined(WIN32) && !defined(OS_LINUX)
		TransformCoordinatesByOrientation(x, y);
	#endif //WIN32

	//s_touchPrevX[touch_index] = s_touchCurrentX[touch_index];
	//s_touchPrevY[touch_index] = s_touchCurrentY[touch_index];
	//s_touchPrevState[touch_index] = s_touchCurrentState[touch_index];
	
	s_touchCurrentX[touch_index] = x;
	s_touchCurrentY[touch_index] = y;
	s_touchCurrentState[touch_index] = State_Off;

	int total_dx = Abs(s_touchCurrentX[touch_index] - s_touchStartX[touch_index]);
	int total_dy = Abs(s_touchCurrentY[touch_index] - s_touchStartY[touch_index]);

	if (s_currentTime - s_touchTimestamp[touch_index] <= k_tap_time
		&& total_dx < k_taps_max_displacement
		&& total_dy < k_taps_max_displacement)
	{
		s_touchType[touch_index] = Type_SingleTap;
	}

	s_touchHasNewEvent[touch_index] = true;

	CTouchElement::UpdateElementsByTouch(touch_index);
}



//============================================================================
/// @brief Returns the X device tilt (transformed with respect to s_currentDeviceOrientation)
/// @note The current implementation handles only ORIENTATION_LANDSCAPE_90 and ORIENTATION_LANDSCAPE_270
//============================================================================
int CTouchPad::GetTiltX()
{
	if (CTouchPad::s_currentDeviceOrientation == EOO_90 )
	{
		return -s_currentTiltX;
	}
	else
	{
		return s_currentTiltX;
	}
}



//============================================================================
/// @brief Returns the Y device tilt (transformed with respect to s_currentDeviceOrientation)
/// @note The current implementation handles only ORIENTATION_LANDSCAPE_90 and ORIENTATION_LANDSCAPE_270
//============================================================================
int CTouchPad::GetTiltY()
{
	if ( CTouchPad::s_currentDeviceOrientation == EOO_270 )
	{
		return -s_currentTiltY;
	}
	else
	{
		return s_currentTiltY;
	}
}



//============================================================================
/// @brief Returns the Z device tilt (transformed with respect to s_currentDeviceOrientation)
/// @note The current implementation handles only ORIENTATION_LANDSCAPE_90 and ORIENTATION_LANDSCAPE_270
//============================================================================
int CTouchPad::GetTiltZ()
{
	if ( CTouchPad::s_currentDeviceOrientation == EOO_270 )
	{
		return -s_currentTiltZ;
	}
	else
	{
		return s_currentTiltZ;
	}
}



//============================================================================
/// @brief Call this every frame to update internal counters
/// @param dtime Time in milliseconds since last call to this method
//============================================================================
void CTouchPad::UpdateCounters(int dtime)
{
	s_currentTime += dtime;
	
	for(int i = 0; i < k_max_touches; i++)
	{
		if (s_touchType[i] == Type_AnyTouch)
		{
			int total_dx = ABS(s_touchCurrentX[i] - s_touchStartX[i]);
			int total_dy = ABS(s_touchCurrentY[i] - s_touchStartY[i]);

			if (total_dx + total_dy >= k_min_drag_displacement ||
				s_currentTime - s_touchTimestamp[i] > k_tap_time)
			{
				s_touchType[i] = Type_Drag;
			}
		}
	}
}





//============================================================================
//============================================================================
//
// CTouchElement
//
//============================================================================
//============================================================================

array<CTouchElement*> CTouchElement::s_elementArray;
CTouchElement* CTouchElement::s_singleEnabledElement = NULL;
CTouchElement* CTouchElement::s_elementByTouch[CTouchPad::k_max_touches] = {0};

//============================================================================
/// @brief Sets the supplied variables with the parameters of elements of type element_type
//============================================================================
void CTouchElement::GetElementsParamsByType(int element_type, int& touch_type, bool& acceptTwoTouches, bool& requireTwoTouches, bool& tapDetectionEnabled)
{
	switch(element_type)
	{
		case Type_AnyTouch:
			touch_type = CTouchPad::Type_AnyTouch;
			acceptTwoTouches = true;
			requireTwoTouches = false;
			tapDetectionEnabled = false;
			break;

		case Type_Button:
			touch_type = CTouchPad::Type_AnyTouch;
			acceptTwoTouches = false;
			requireTwoTouches = false;
			tapDetectionEnabled = false;
			break;
		
		case Type_SingleTap:
			touch_type = CTouchPad::Type_SingleTap;
			acceptTwoTouches = false;
			requireTwoTouches = false;
			tapDetectionEnabled = true;
			break;

		case Type_Drag:
			touch_type = CTouchPad::Type_Drag;
			acceptTwoTouches = false;
			requireTwoTouches = false;
			tapDetectionEnabled = false;
			break;

		case Type_DragAndZoom:
			touch_type = CTouchPad::Type_Drag;
			acceptTwoTouches = true;
			requireTwoTouches = false;
			tapDetectionEnabled = false;
			break;

		case Type_Joystick:
			touch_type = CTouchPad::Type_AnyTouch;
			acceptTwoTouches = false;
			requireTwoTouches = false;
			tapDetectionEnabled = false;
			break;

		default:
			//Unhandled element type
			touch_type = -1;
			acceptTwoTouches = false;
			requireTwoTouches = false;
			tapDetectionEnabled = false;
	}
}


//============================================================================
/// @brief Constructs a touch element of type elementType
//============================================================================
CTouchElement::CTouchElement(int elementType, int zLevel)
{
	m_elementType = elementType;
	m_zLevel = zLevel;

	GetElementsParamsByType(
			m_elementType,
			m_requiredTouchType,
			m_acceptTwoSimultaneousTouches,
			m_requireTwoSimultaneousTouches,
			m_tapDetectionEnabled
		);

	m_assignedTouchIndex = -1;
	m_assignedTouchIndex2 = -1;
	m_useFirstTouchForDefault = true;
	
	m_isEnabled = true;
	m_isActivated = false;
	m_bJustActivated = false;
	m_justReleasedCounter = 0;
	
	ResetTapDetection();
	
	// Add the element to the elements list
	s_elementArray.push_front(this);
}



//============================================================================
/// @brief Constructs a touch element with the required touch type
//============================================================================
CTouchElement::~CTouchElement(void)
{
	ResetInput();
	DisableAllOtherElements(false);
	
	// Remove the element from the elements list
	s_elementArray.erase( s_elementArray.linear_search(this) );
}



//============================================================================
/// @brief Sets the screen coordinates of the touch element
//============================================================================
void CTouchElement::SetScreenCoordinates(int x, int y, int w, int h)
{
	m_screenX = x;
	m_screenY = y;
	m_screenW = w;
	m_screenH = h;
}



//============================================================================
/// @brief Sets the screen coordinates of the touch element
//============================================================================
bool CTouchElement::IsTouchInsideElement(bool second_touch)
{
	int& assigned_touch = (second_touch || !m_useFirstTouchForDefault) ? m_assignedTouchIndex2 : m_assignedTouchIndex;

	int touch_x, touch_y;
	CTouchPad::GetTouchCurrentCoordinates(assigned_touch, touch_x, touch_y);

	bool touch_is_inside_element = (
			touch_x >= m_screenX &&
			touch_x < m_screenX + m_screenW &&
			touch_y >= m_screenY &&
			touch_y < m_screenY + m_screenH
			);

	return touch_is_inside_element;
}



//============================================================================
/// @brief Sets the element as enabled / disabled
//============================================================================
void CTouchElement::SetEnabled(bool is_enabled)
{	
	m_isEnabled = is_enabled;

	if (!m_isEnabled)
	{
		ResetInput();
	}
}



//============================================================================
/// @brief Disables/enables all other elements' input
//============================================================================
void CTouchElement::DisableAllOtherElements(bool disable)
{
	if (disable)
	{
		// Mark this element as the only enabled one
		s_singleEnabledElement = this;

		// Clear current input from the other touches
		for (int i = 0; i < s_elementArray.size(); i++)
		{
			if (s_elementArray[i] != this)
			{
				s_elementArray[i]->ResetInput();
			}
		}
	}
	else
	{
		// Enable the other elements
		if (this == s_singleEnabledElement)
		{
			s_singleEnabledElement = NULL;
		}
	}
}



//============================================================================
/// @brief Removes all associations with touches (i.e. makes the element NOT touched so that it can accept other touches)
//============================================================================
void CTouchElement::ResetInput()
{
	if (m_assignedTouchIndex != -1)
	{
		s_elementByTouch[m_assignedTouchIndex] = NULL;
	}

	if (m_assignedTouchIndex2 != -1)
	{
		s_elementByTouch[m_assignedTouchIndex2] = NULL;
	}
	
	m_assignedTouchIndex = -1;
	m_assignedTouchIndex2 = -1;
	m_isActivated = false;
	m_bJustActivated = false;
	m_justReleasedCounter = 0;
	
	ResetTapDetection();

	DisableAllOtherElements(false);
}



//============================================================================
/// @brief Assign the first or second assigned touch (depending on second_touch) to another touch element
//============================================================================
void CTouchElement::ReassignTouchTo(CTouchElement* anotherTE, bool second_touch)
{
	if (s_singleEnabledElement == this)
	{
		s_singleEnabledElement = anotherTE;
	}

	int& assigned_touch = (second_touch || !m_useFirstTouchForDefault) ? m_assignedTouchIndex2 : m_assignedTouchIndex;

	if (assigned_touch == -1) return;

	s_elementByTouch[assigned_touch] = anotherTE;

	bool wasActivated = anotherTE->m_isActivated;
	anotherTE->m_assignedTouchIndex = assigned_touch;
	anotherTE->m_isActivated = anotherTE->CheckForActivation();
	anotherTE->m_bJustActivated = !wasActivated && anotherTE->m_isActivated;
	CTouchPad::SetPrevState( anotherTE->m_assignedTouchIndex );
	
	//CTouchPad::ClearNewEventFlag(assigned_touch);
	assigned_touch = -1;
	ResetTapDetection();
}



//============================================================================
/// @brief Enables tapping (single and double) detection
/// @note If detection is enabled, IsActive() will return TRUE until ResetTapDetection() is called!
//============================================================================
void CTouchElement::SetTapDetectionEnabled(bool is_enabled)
{
	if (m_tapDetectionEnabled != is_enabled)
	{
		ResetTapDetection();
	}
	
	m_tapDetectionEnabled = is_enabled;
}


//============================================================================
/// @brief Resets (sets to 0) the taps counter
//============================================================================
void CTouchElement::ResetTapDetection()
{
	m_tapDetectionCounter = 0;
	m_tapsFinalCount = 0;
	m_lastDetectedTapTimestamp = 0;
}



//============================================================================
/// @brief Returns true if the element is now activated by touching it
//============================================================================
bool CTouchElement::CheckForActivation(bool force_two_touches)
{
	if (!m_isEnabled) return false;
	if (s_singleEnabledElement != NULL && this != s_singleEnabledElement) return false;
	if (m_assignedTouchIndex == -1) return false;
	if ((m_requireTwoSimultaneousTouches || force_two_touches) && m_assignedTouchIndex2 == -1) return false;

	if (m_requiredTouchType == CTouchPad::Type_AnyTouch) return true;

	bool touches_type_is_ok = true;

	if (CTouchPad::GetTouchType(m_assignedTouchIndex) != m_requiredTouchType) touches_type_is_ok = false;
	if ((m_acceptTwoSimultaneousTouches || force_two_touches) && m_assignedTouchIndex2 != -1)
	{
		if (CTouchPad::GetTouchType(m_assignedTouchIndex2) != m_requiredTouchType) touches_type_is_ok = false;
	}

	return touches_type_is_ok;
}



//============================================================================
/// @brief Checks all new touches to see if the element is touched by any of them
/// @return True if a touch has been assigned to the element
//============================================================================
bool CTouchElement::CheckForFirstTouch()
{
	if (m_assignedTouchIndex != -1) return false; // touch is already assigned
	
	for(int i = 0; i < CTouchPad::k_max_touches; i++)
	{
		// if a new touch has just been pressed
		if (CTouchPad::GetTouchCurrentState(i) == CTouchPad::State_On && CTouchPad::GetNewEventFlag(i))
		{
			int touch_x, touch_y;

			CTouchPad::GetTouchStartCoordinates(i, touch_x, touch_y);

			bool touch_is_inside_element = (
					touch_x >= m_screenX &&
					touch_x < m_screenX + m_screenW &&
					touch_y >= m_screenY &&
					touch_y < m_screenY + m_screenH
					);

			if (!touch_is_inside_element) 
				continue;

			if (k_debug_output)
			{
				DBG_OUT("%d A ON (%d)\n", (int)this, m_assignedTouchIndex);
			}
			
			m_assignedTouchIndex = i;
			s_elementByTouch[m_assignedTouchIndex] = this;
			
			CTouchPad::ClearNewEventFlag(i);

			return true;
		}
	}
	
	return false;
}



//============================================================================
/// @brief Checks all new touches to see if the element is touched by any of them
/// @return True if a second touch has been assigned to the element
//============================================================================
bool CTouchElement::CheckForSecondTouch()
{
	if (m_assignedTouchIndex == -1) return false; // first touch must be already assigned
	if (m_assignedTouchIndex2 != -1) return false; // second touch is already assigned
	
	for(int i = 0; i < CTouchPad::k_max_touches; i++)
	{
		if (i != m_assignedTouchIndex)
		// if a new touch has just been pressed
		if (CTouchPad::GetTouchCurrentState(i) == CTouchPad::State_On && CTouchPad::GetNewEventFlag(i))
		{
			int touch_x, touch_y;

			CTouchPad::GetTouchStartCoordinates(i, touch_x, touch_y);

			bool touch_is_inside_element = (
					touch_x >= m_screenX &&
					touch_x < m_screenX + m_screenW &&
					touch_y >= m_screenY &&
					touch_y < m_screenY + m_screenH
					);

			if (!touch_is_inside_element) continue;
			
			if (k_debug_output)
			{
				DBG_OUT("%d B ON (%d)\n", (int)this, m_assignedTouchIndex2);
			}
			
			m_assignedTouchIndex2 = i;
			s_elementByTouch[m_assignedTouchIndex2] = this;
			
			CTouchPad::ClearNewEventFlag(i);

			return true;
		}
	}
	
	return false;
}



//============================================================================
/// @brief The main method for updating a touch element
//============================================================================
void CTouchElement::Update(bool only_validate_assigned_touches)
{
	if (!m_isEnabled) return;
	if (s_singleEnabledElement != NULL && this != s_singleEnabledElement) return;

	if (!only_validate_assigned_touches)
	{
		m_bJustActivated = false;

		if (m_justReleasedCounter > 0)
		{
			m_justReleasedCounter--;
		}
		
		if (m_assignedTouchIndex == -1)
		{
			m_bJustActivated = CheckForFirstTouch();
		}

		if (m_acceptTwoSimultaneousTouches && m_assignedTouchIndex2 == -1)
		{
			CheckForSecondTouch();
		}

		bool activated = CheckForActivation();
		m_bJustActivated = !m_isActivated && activated;
		m_isActivated = activated;

		
		if (m_elementType == Type_Joystick)
		{
			UpdateJoystick();
		}
	}

	bool touchIndexReset = false;
	bool tapDetected = false;
	int tap_x, tap_y;

	if (m_assignedTouchIndex2 != -1)
	{
		bool new_event_touch_off = (CTouchPad::GetTouchCurrentState(m_assignedTouchIndex2) == CTouchPad::State_Off && CTouchPad::GetNewEventFlag(m_assignedTouchIndex2));
		
		if (new_event_touch_off)
		{
			if (k_debug_output)
			{
				DBG_OUT("%d B OFF (%d)\n", (int)this, m_assignedTouchIndex);
			}
			
			CTouchPad::ClearNewEventFlag(m_assignedTouchIndex2);

			if (CTouchPad::GetTouchType(m_assignedTouchIndex2) == CTouchPad::Type_SingleTap)
			{
				GetTouchPos_Start(tap_x, tap_y, true);
				tapDetected = true;
			}
			
			s_elementByTouch[m_assignedTouchIndex2] = NULL;
			m_assignedTouchIndex2 = -1;
			touchIndexReset = true;
		}
	}

	if (m_assignedTouchIndex != -1)
	{
		bool new_event_touch_off = (CTouchPad::GetTouchCurrentState(m_assignedTouchIndex) == CTouchPad::State_Off && CTouchPad::GetNewEventFlag(m_assignedTouchIndex));
		
		if (new_event_touch_off)
		{
			if (k_debug_output)
			{
				DBG_OUT("%d A OFF (%d)\n", (int)this, m_assignedTouchIndex);
			}

			CTouchPad::ClearNewEventFlag(m_assignedTouchIndex);
			
			if (CTouchPad::GetTouchType(m_assignedTouchIndex) == CTouchPad::Type_SingleTap)
			{
				GetTouchPos_Start(tap_x, tap_y, false);
				tapDetected = true;
			}

			// Remeber last touch coordinates
			CTouchPad::GetTouchCurrentCoordinates(m_assignedTouchIndex, m_lastTouchX, m_lastTouchY);
			CTouchPad::GetTouchPrevCoordinates(m_assignedTouchIndex, m_lastTouchPrevX, m_lastTouchPrevY);			
			
			// Free touch
			s_elementByTouch[m_assignedTouchIndex] = NULL;
			m_assignedTouchIndex = -1;
			touchIndexReset = true;

			m_justReleasedCounter = 2;
		}
	}

	if (m_tapDetectionEnabled)
	{
		bool too_delayed_tap = false;
		bool too_displaced_tap = false;

		if (m_tapDetectionCounter > 0)
		{
			too_delayed_tap = CTouchPad::s_currentTime - m_lastDetectedTapTimestamp > CTouchPad::k_tap_max_interval;
			
			if (tapDetected)
			{
				too_displaced_tap = ABS(tap_x - m_lastTapX) + ABS(tap_y - m_lastTapY) > CTouchPad::k_taps_max_displacement;
			}
		}
		
		if (too_delayed_tap || too_displaced_tap)
		{
			m_tapsFinalCount = m_tapDetectionCounter;

			m_tapDetectionCounter = 0;
		}
		else if (tapDetected)
		{
			m_tapDetectionCounter++;
			m_lastDetectedTapTimestamp = CTouchPad::s_currentTime;
			m_lastTapX = tap_x;
			m_lastTapY = tap_y;
		}

	}

	if (touchIndexReset)
	{
		m_isActivated = CheckForActivation();
	}
}



//============================================================================
/// @brief Returns true if the element is active
//============================================================================
bool CTouchElement::IsActive()
{
	if (!m_isEnabled) return false;
	if (s_singleEnabledElement != NULL && this != s_singleEnabledElement) return false;
	
	switch(m_elementType)
	{
		case Type_AnyTouch:
		case Type_Button:
		case Type_Joystick:
			return m_isActivated;
		
		case Type_Drag:
		case Type_DragAndZoom:
			return m_isActivated && CTouchPad::GetTouchType(m_assignedTouchIndex) == CTouchPad::Type_Drag;
		
		case Type_SingleTap:
			return (GetDetectedTaps() == 1);

		default: return false;
	}
}



//============================================================================
/// @brief Returns true if the element has 2 touches assigned
/// @note The method returns the DX and DY between the current positions of the 2 touches
//============================================================================
bool CTouchElement::IsActiveWithTwoTouches(int& dx, int& dy)
{
	if (m_assignedTouchIndex == -1 || m_assignedTouchIndex2 == -1) return false;
	
	int current_x1, current_y1;
	int current_x2, current_y2;

	CTouchPad::GetTouchCurrentCoordinates(m_assignedTouchIndex, current_x1, current_y1);
	CTouchPad::GetTouchCurrentCoordinates(m_assignedTouchIndex2, current_x2, current_y2);
	
	dx = current_x2 - current_x1;
	dy = current_y2 - current_y1;
	
	return true;
}



//============================================================================
/// @brief The main method for updating all the enabled elements (check if touched, etc.)
//============================================================================

bool compareTE(const CTouchElement* x, const CTouchElement* y) 
{
	return x->m_zLevel < y->m_zLevel; 
}

void CTouchElement::UpdateElements()
{
	if (!s_elementArray.empty())
	{
		/// Sort by z-level
		std::stable_sort(&s_elementArray[0], &s_elementArray[0] + s_elementArray.size(), &compareTE);

		for (int i = 0; i < s_elementArray.size(); i++)
		{
			if (s_elementArray[i]->IsEnabled())
			{
				s_elementArray[i]->Update();
			}
		}
	}
}



//============================================================================
/// @brief Updates the element that has the touch given by touchIndex assigned
//============================================================================
void CTouchElement::UpdateElementsByTouch(int touchIndex)
{
	if (s_elementByTouch[touchIndex] == NULL) return; // no element has this touch assigned

	CTouchElement* element = s_elementByTouch[touchIndex];

	element->Update(true);
}



//============================================================================
/// @brief Resets all elements' input
//============================================================================
void CTouchElement::ResetElementsInput()
{
	s_singleEnabledElement = NULL;
	
	for (int i = 0; i < s_elementArray.size(); i++)
	{
		s_elementArray[i]->ResetInput();
	}

	CTouchPad::ResetTouches();
}





//============================================================================
/// @brief Returns the touch's start position in the supplied coordinates variables
//============================================================================
void CTouchElement::GetTouchPos_Start(int& x, int&y, bool second_touch)
{
	int touch_index = (second_touch || !m_useFirstTouchForDefault) ? m_assignedTouchIndex2 : m_assignedTouchIndex;
	
	if (touch_index == -1) return;
	
	CTouchPad::GetTouchStartCoordinates(touch_index, x, y);
}



//============================================================================
/// @brief Returns the touch's current type
//============================================================================
int CTouchElement::GetCurrentTouchType(bool second_touch)
{
	int touch_index = (second_touch && m_useFirstTouchForDefault) ? m_assignedTouchIndex2 : m_assignedTouchIndex;
	
	if (touch_index == -1)
	{
		return CTouchPad::Type_AnyTouch;
	}
	
	return CTouchPad::GetTouchType(touch_index);
}



//============================================================================
/// @brief Returns the touch's duration
//============================================================================
int CTouchElement::GetCurrentTouchDuration(bool second_touch)
{
	int touch_index = (second_touch || !m_useFirstTouchForDefault) ? m_assignedTouchIndex2 : m_assignedTouchIndex;
	
	if (touch_index == -1)
	{
		return -1;
	}
	
	return CTouchPad::s_currentTime - CTouchPad::GetTouchTimestamp(touch_index);
}



//============================================================================
/// @brief Returns the touch's current position in the supplied coordinates variables
//============================================================================
void CTouchElement::GetTouchPos_Current(int& x, int&y, bool second_touch)
{
	int touch_index = (second_touch || !m_useFirstTouchForDefault) ? m_assignedTouchIndex2 : m_assignedTouchIndex;

	if (touch_index == -1)
	{
		if (second_touch || !m_useFirstTouchForDefault) return;

		x = m_lastTouchX;
		y = m_lastTouchY;
	}
	else
	{	
		CTouchPad::GetTouchCurrentCoordinates(touch_index, x, y);
	}
}

//============================================================================
/// @brief Returns the touch's previous position in the supplied coordinates variables
//============================================================================
void CTouchElement::GetTouchPos_Prev(int& x, int&y, bool second_touch)
{
	int touch_index = (second_touch || !m_useFirstTouchForDefault) ? m_assignedTouchIndex2 : m_assignedTouchIndex;

	if (touch_index == -1)
	{
		if (second_touch || !m_useFirstTouchForDefault) return;

		x = m_lastTouchPrevX;
		y = m_lastTouchPrevY;
	}
	else
	{
		CTouchPad::GetTouchPrevCoordinates(touch_index, x, y);
	}
}



//============================================================================
/// @brief Returns the touch's total displacement (from start to current) in the supplied coordinates variables
//============================================================================
void CTouchElement::GetTouchPos_DTotal(int& x, int&y, bool second_touch)
{
	int touch_index = (second_touch || !m_useFirstTouchForDefault) ? m_assignedTouchIndex2 : m_assignedTouchIndex;
	
	if (touch_index == -1)
	{
		x = 0;
		y = 0;
		
		return;
	}
	
	int cx, cy, sx, sy;
	
	GetTouchPos_Start(sx, sy, second_touch);
	GetTouchPos_Current(cx, cy, second_touch);

	x = cx - sx;
	y = cy - sy;
}



//============================================================================
/// @brief Returns the touch's displacement since the last call to this method in the supplied coordinates variables
//============================================================================
void CTouchElement::GetTouchPos_DT(int& x, int&y, bool second_touch)
{
	int touch_index = (second_touch || !m_useFirstTouchForDefault) ? m_assignedTouchIndex2 : m_assignedTouchIndex;
	
	if (touch_index == -1)
	{
		if (second_touch || !m_useFirstTouchForDefault)
		{
			x = 0;
			y = 0;
			return;
		}

		x = m_lastTouchX - m_lastTouchPrevX;
		y = m_lastTouchY - m_lastTouchPrevY;

		m_lastTouchPrevX = m_lastTouchX;
		m_lastTouchPrevY = m_lastTouchY;
	}
	else
	{	
		int cx, cy, px, py;
		
		CTouchPad::GetTouchPrevCoordinates(touch_index, px, py);
		CTouchPad::GetTouchCurrentCoordinates(touch_index, cx, cy);

		x = cx - px;
		y = cy - py;

		CTouchPad::SetPrevState(touch_index);
	}
}



//============================================================================
/// @brief Sets the parameters of a joystick element and initializes its state
//============================================================================
void CTouchElement::InitJoystick(int centerX, int centerY, int maxAmplitude, int minAmplitude)
{
	if (centerX == -1 && centerY == -1)
	{
		centerX = m_screenX + (m_screenW / 2);
		centerY = m_screenY + (m_screenH / 2);
	}

	m_joystickCenterX = centerX;
	m_joystickCenterY = centerY;

	m_joystickCurrentX = m_joystickCenterX;
	m_joystickCurrentY = m_joystickCenterY;

	m_joystickAngle = 0;
	m_joystickAmplitude = 0;
	m_joystickMinAmplitude = (minAmplitude == -1) ? 4 : minAmplitude;
	m_joystickMaxAmplitude = (maxAmplitude == -1) ? (m_screenW / 2) : maxAmplitude;
}



//============================================================================
/// @brief Updates the joystick behaviour of the element
//============================================================================
void CTouchElement::UpdateJoystick()
{
	if (IsActive())
	{
		GetTouchPos_Current(m_joystickCurrentX, m_joystickCurrentY);

		int dx = m_joystickCurrentX - m_joystickCenterX;
		int dy = m_joystickCurrentY - m_joystickCenterY;

		m_joystickAngle = ANGLE_TO_POINT(dx, dy);
		m_joystickAmplitude = SQRT((dx * dx) + (dy * dy));

		if (m_joystickAmplitude < m_joystickMinAmplitude)
		{
			m_joystickCurrentX = m_joystickCenterX;
			m_joystickCurrentY = m_joystickCenterY;
			
			m_joystickAngle = 0;
			m_joystickAmplitude = 0;
		}
		else if (m_joystickAmplitude > m_joystickMaxAmplitude)
		{
			m_joystickAmplitude = m_joystickMaxAmplitude;
			m_joystickCurrentX = m_joystickCenterX + COMPONENT_X(m_joystickAngle, m_joystickAmplitude);
			m_joystickCurrentY = m_joystickCenterY + COMPONENT_Y(m_joystickAngle, m_joystickAmplitude);
		}		
	}
	else
	{
		m_joystickCurrentX -= (m_joystickCurrentX - m_joystickCenterX) / k_joystick_return_speed;
		m_joystickCurrentY -= (m_joystickCurrentY - m_joystickCenterY) / k_joystick_return_speed;

		if (ABS(m_joystickCurrentX - m_joystickCenterX) < k_joystick_return_speed && ABS(m_joystickCurrentY - m_joystickCenterY) < k_joystick_return_speed)
		{
			m_joystickCurrentX = m_joystickCenterX;
			m_joystickCurrentY = m_joystickCenterY;
		}

		m_joystickAngle = 0;
		m_joystickAmplitude = 0;
	}
}



//============================================================================
/// @brief Returns the position of the joystick as it is to be drawn or relative to the joystick center
//============================================================================
void CTouchElement::GetJoystickPos(int& x, int& y, bool relativeToCenter)
{
	x = m_joystickCurrentX;
	y = m_joystickCurrentY;

	if (relativeToCenter)
	{
		x -= m_joystickCenterX;
		y -= m_joystickCenterY;
	}
}



//============================================================================
/// @brief Resets the input of the element and sets the joystick in initial position
//============================================================================
void CTouchElement::ResetJoystickInput()
{
	ResetInput();

	m_joystickCurrentX = m_joystickCenterX;
	m_joystickCurrentY = m_joystickCenterY;

	m_joystickAngle = 0;
	m_joystickAmplitude = 0;
}


bool CTouchElement::IsPointInside (position2di pt)
{
	r2i myRect(p2i(m_screenX, m_screenY), d2i(m_screenW, m_screenH));
	return myRect.isPointInside(pt);
}
