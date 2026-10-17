///////////////////////////////////////////////////////////////////////
//
//	Touch pad
//
///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"

#ifndef MAX_TOUCHES
#define MAX_TOUCHES (5)
#endif

//============================================================================
/// @brief CTouchPad handles the touchpad input
/// @note Using CTouchPad directly is discouraged - use CTouchElement functionaly instead
//============================================================================
class CTouchPad
{
public:
	// ---------- constants -------------------------------------

	enum TouchState {
		State_Off,
		State_On
	};

	enum TouchType {
		Type_AnyTouch,
		Type_SingleTap,
		Type_Drag
	};

	static const int k_max_touches = MAX_TOUCHES;

	static const int k_tap_time = 400; // max duration (in milliseconds) of a tap
	static const int k_tap_max_interval = 500; // max interval (in milliseconds) between taps
	static const int k_taps_max_displacement = 30; // max movement (in pixels) for a touch to be considered a tap (not a drag)
	static const int k_min_drag_displacement = 1; // distance in pixels for a touch to move in order to be considered Type_Drag

	static const bool k_debug_output = false;


	// ---------- static members --------------------------------

	static E_ORIENTATION s_currentDeviceOrientation;

	static int s_currentTime; // internal timer


	// ---------- touches parameters ----------------------------
protected:
	static int s_touchStartX[k_max_touches];
	static int s_touchStartY[k_max_touches];

	static int s_touchCurrentX[k_max_touches];
	static int s_touchCurrentY[k_max_touches];
	static int s_touchCurrentState[k_max_touches];

	static int s_touchPrevX[k_max_touches];
	static int s_touchPrevY[k_max_touches];
	static int s_touchPrevState[k_max_touches];

	static int s_touchType[k_max_touches];
	static int s_touchTimestamp[k_max_touches];
	static bool s_touchHasNewEvent[k_max_touches];

	// ---------- device tilt parameters ------------------------
public:
	static int s_currentTiltX;
	static int s_currentTiltY;
	static int s_currentTiltZ;

	static int s_currentTiltAcclerationX;
	static int s_currentTiltAcclerationY;
	static int s_currentTiltAcclerationZ;


	// ---------- methods ---------------------------------------
public:
	CTouchPad(void);
	~CTouchPad(void);

	static void InitTouchPad(bool resetTouches = true, bool resetTilt = true);
	inline static void ResetTouches() { InitTouchPad(true, false); }

	static void UpdateCounters(int dtime);

	static void EventTouchDown(int touch_index, int x, int y);
	static void EventTouchUp(int touch_index, int x, int y);
	static void EventTouchMove(int touch_index, int x, int y);

	static void EventSetTilt(int tilt_x, int tilt_y, int tilt_z);
	static void EventSetTiltAcceleration(int acc_x, int acc_y, int acc_z);

	static int GetTiltX();
	static int GetTiltY();
	static int GetTiltZ();

	inline static int GetTouchType(int touch_index) { return s_touchType[touch_index]; }
	inline static int GetTouchTimestamp(int touch_index) { return s_touchTimestamp[touch_index]; }
	inline static int GetTouchCurrentState(int touch_index) { return s_touchCurrentState[touch_index]; }

	inline static void GetTouchStartCoordinates(int touch_index, int& x, int& y) { x = s_touchStartX[touch_index]; y = s_touchStartY[touch_index]; }
	inline static void GetTouchCurrentCoordinates(int touch_index, int& x, int& y) { x = s_touchCurrentX[touch_index]; y = s_touchCurrentY[touch_index]; }
	inline static void GetTouchPrevCoordinates(int touch_index, int& x, int& y) { x = s_touchPrevX[touch_index]; y = s_touchPrevY[touch_index]; }

	inline static bool GetNewEventFlag(int touch_index) {  return s_touchHasNewEvent[touch_index]; }
	inline static void ClearNewEventFlag(int touch_index) { s_touchHasNewEvent[touch_index] = false; }

	static void SetPrevState(int touch_index);

	static bool TransformCoordinatesByOrientation(int& x, int& y);
	static bool InverseTransformCoordinatesByOrientation(int& x, int& y);
};







//============================================================================
/// @brief CTouchElement represents a touchable area on the screen
//============================================================================

//============================================
//  Example usage of CTouchElement:
//
//  CTouchElement* joystickTE = new CTouchElement(CTouchElement::Type_Joystick, 0); //zLevel is 0, i.e. on top
//  joystickTE->SetScreenCoordinates(100, 100, 50, 50);
//  joystickTE->InitJoystick(125, 125, 25); // Joystick specific
//  
//  joystickTE->SetEnabled(bIsIngame);
//
//  // You must call CTouchElement::UpdateElements() every frame before checking the input
//  // This will make the elements check for new touches and update their state according to the state of their currently assigned touches
//  CTouchElement::UpdateElements()
//  
//  if (joystickTE->IsActive())
//  {
//  	// If we want to terminate the touch (usefull for buttons that need their action executed only once)
//  	// A new tuch must be pressed in order to activate the element again
//  	//joystickTE->ResetInput();
//  
//  	int joystick_touch_x, joystick_touch_y;
//  	joystickTE->GetTouchPos_Current(joystick_touch_x, joystick_touch_y);
//		
//  	// Joystick specific
//  	int joystick_angle, joystick_amplitude;
//  	int joystick_draw_x, joystick_draw_y;
//  
//  	joystick_angle = joystickTE->GetJoystickAngle();
//  	joystick_amplitude = joystickTE->GetJoystickAmplitude();
//  	joystickTE->GetJoystickPos(joystick_draw_x, joystick_draw_y);
//  }
//
//==== End of CTouchElement example ==========

class CTouchElement
{
public:

	enum ElementType {
		Type_AnyTouch,
		Type_Button,
		Type_SingleTap,
		Type_Drag,
		Type_DragAndZoom,
		Type_Joystick,
	};

	static const bool k_debug_output = false;

	static const int k_joystick_return_speed = 3; // the bigger the faster the joystick will return to center position when not pressed


	// ---------- touch element parameters ----------------------
	bool m_isEnabled;
	bool m_isActivated;
	bool m_bJustActivated;
	int m_justReleasedCounter;

	int m_elementType;


	// ---------- touch elements list ---------------------------
	static array<CTouchElement*> s_elementArray;
	static CTouchElement* s_singleEnabledElement; // only this element will receive input

	// ---------- screen coordinates ----------------------------
	int m_zLevel; // 0 = on top, 99 = on bottom
	int m_screenX;
	int m_screenY;
	int m_screenW;
	int m_screenH;


	// ---------- assigned touches ------------------------------
	int m_requiredTouchType; // the type of touch (CTouchPad::TouchType) required for the element to be activated
	bool m_acceptTwoSimultaneousTouches;
	bool m_requireTwoSimultaneousTouches;

	int m_assignedTouchIndex; // index of the touch linked to this touch element
	int m_assignedTouchIndex2; // used if m_acceptTwoSimultaneousTouches is true
	bool m_useFirstTouchForDefault;

	static CTouchElement* s_elementByTouch[CTouchPad::k_max_touches];


	// ---------- tap detection  --------------------------------
	bool m_tapDetectionEnabled;
	int m_tapDetectionCounter; // number of taps detected
	int m_tapsFinalCount; // will be set to m_tapDetectionCounter when no more taps are made (timeout)
	int m_lastDetectedTapTimestamp;

	// Remember tap position (to read when the touch is gone)
	int m_lastTapX;
	int m_lastTapY;

	// Remeber touch position (to read when the touch is gone)
	int m_lastTouchX;
	int m_lastTouchY;
	int m_lastTouchPrevX;
	int m_lastTouchPrevY;


	// ---------- joystick parameters ---------------------------
	int m_joystickCenterX;
	int m_joystickCenterY;
	int m_joystickCurrentX;
	int m_joystickCurrentY;
	int m_joystickAngle;
	int m_joystickAmplitude;
	int m_joystickMinAmplitude;
	int m_joystickMaxAmplitude;


	// ========== methods ===========================================================
	CTouchElement(int elementType = Type_AnyTouch, int zLevel = 50);
	~CTouchElement();

	static void		UpdateElements();
	static void		UpdateElementsByTouch(int touchIndex);

	void			SetScreenCoordinates(int x, int y, int w, int h);
	inline r2i		GetArea() { return r2i(m_screenX, m_screenY, m_screenX + m_screenW, m_screenY + m_screenH); }

	void SetEnabled(bool is_enabled);
	inline bool IsEnabled() { return m_isEnabled; }
	void DisableAllOtherElements(bool disable);

	bool IsPointInside (position2di pt);

	bool IsActive();
	bool IsActiveWithTwoTouches(int& dx, int& dy);
	inline bool IsNowActivated() { return m_bJustActivated; }
	inline bool IsNowReleased() { return (m_justReleasedCounter > 0); }

	static void ResetElementsInput(); // reset the input (as if all touches are unpressed) of all elements
	void ResetInput();

	int GetCurrentTouchType(bool second_touch = false); // what type is the touch in this moment (it can change from CTouchPas::Type_AnyTouch to CTouchPas::Type_SingleTap or CTouchPas::Type_Drag, etc.)
	int GetCurrentTouchDuration(bool second_touch = false); // what time has passed since the start of the touch
	void GetTouchPos_Start(int& x, int&y, bool second_touch = false); // start position of the touch
	void GetTouchPos_Current(int& x, int&y, bool second_touch = false); // current position of the touch
	void GetTouchPos_Prev(int& x, int&y, bool second_touch = false); // prev position of the touch
	void GetTouchPos_DT(int& x, int&y, bool second_touch = false); // displacement since last call to this method
	void GetTouchPos_DTotal(int& x, int&y, bool second_touch = false); // displacement since the beginning of the touch
	bool IsTouchInsideElement(bool second_touch = false);

	void ReassignTouchTo(CTouchElement* anotherTE, bool second_touch = false);


	// ---------- joystick methods ------------------------------
	void InitJoystick(int centerX = -1, int centerY = -1, int maxAmplitude = -1, int minAmplitude = -1);
	void UpdateJoystick();
	void ResetJoystickInput();
	void GetJoystickPos(int& x, int& y, bool relativeToCenter = false);
	inline int GetJoystickAngle() { return m_joystickAngle; }  // angle formed by the lines from the joystick center to the current touch position and the axis line (y = 0)
	inline int GetJoystickAmplitude() { return m_joystickAmplitude; } // distance from the joystick center to the current touch position

	// ---------- tapping detection methods ---------------------
	// If detection is enabled, GetDetectedTaps() will return the number
	// of taps made until ResetTapDetection() is called
	// Note that after the taps are made (when GetDetectedTaps() returns their number) IsActive() returns FALSE
	void SetTapDetectionEnabled(bool is_enabled);
	void ResetTapDetection();
	inline int GetDetectedTaps(bool intermediate_counter = false) { return (intermediate_counter) ? m_tapDetectionCounter : m_tapsFinalCount; }
	inline void GetTapPos(int& x, int& y) { x = m_lastTapX; y = m_lastTapY; }

	// ---------- internal methods ------------------------------
protected:

	static void GetElementsParamsByType(int element_type, int& touch_type, bool& acceptTwoTouches, bool& requireTwoTouches, bool& tapDetectionEnabled);

	void Update(bool only_validate_assigned_touches = false); // Is called by UpdateElements()

	// Activation checking methods
	bool CheckForFirstTouch();
	bool CheckForSecondTouch();
	bool CheckForActivation(bool force_two_touches = false);
};




//============================================================================
/// @brief OrientationSensorHistoryQueue
//============================================================================
class OrientationSensorHistoryQueue
{
private:
	enum{ QUEUE_LENGTH=20 }; 
	int m_xTilt[QUEUE_LENGTH];
	int m_yTilt[QUEUE_LENGTH];
	int m_zTilt[QUEUE_LENGTH];		
	int m_Time[QUEUE_LENGTH];		
	int m_firstIndex;
	int m_lastIndex;
public:
	OrientationSensorHistoryQueue() {
		m_firstIndex=0;
		m_lastIndex=0;
	};
	bool isEmpty() {	
		return m_firstIndex==m_lastIndex;
	}
	int getNextElementIndex(int index){			
		return (index+1)%QUEUE_LENGTH;
	};
	void clear() {
		m_firstIndex=0;
		m_lastIndex=0;
	}

	void addValues(int xTilt,int yTilt,int zTilt,int time)
	{
		m_xTilt[m_firstIndex] = xTilt;
		m_yTilt[m_firstIndex] = yTilt;
		m_zTilt[m_firstIndex] = zTilt;
		m_Time[m_firstIndex]  = time;
		m_firstIndex = getNextElementIndex(m_firstIndex);
		if (m_firstIndex==m_lastIndex)
		{
			m_lastIndex = getNextElementIndex(m_lastIndex);
		}						
	}
	bool getValues(int framesBeforeCurrentCount,int *xTilt,int *yTilt,int *zTilt,int *time)
	{
		if (isEmpty()) return false;
		if (framesBeforeCurrentCount>=QUEUE_LENGTH) return false;

		int index=-1;
		if (m_firstIndex>m_lastIndex)
		{
			int elementsCount = m_firstIndex - m_lastIndex;				
			if (elementsCount>framesBeforeCurrentCount)
			{	
				index=m_firstIndex-framesBeforeCurrentCount-1;
				if (index<0) index=index+QUEUE_LENGTH;					
			}
		}
		else
		{
			int elementsCount = m_firstIndex - (m_lastIndex-QUEUE_LENGTH);				
			if (elementsCount>framesBeforeCurrentCount)
			{	
				index=m_firstIndex-framesBeforeCurrentCount-1;
				if (index<0) index=index+QUEUE_LENGTH;
			}
		}

		if (index>=0)
		{
			*xTilt= m_xTilt[index];
			*yTilt= m_yTilt[index];
			*zTilt= m_zTilt[index];
			*time= m_Time[index];
			return true;
		}
		return false;
	}

};

