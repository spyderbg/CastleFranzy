#ifndef __TOUCHRECT_H__
#define __TOUCHRECT_H__

#pragma once

enum TouchAction
{
	TouchActionSelectContinue = 0,
	TouchActionMoveLeft,
	TouchActionMoveRight,
	TouchActionMoveUp,
	TouchActionMoveDown,
	TouchActionNext,
	TouchActionBack,
	TouchActionPageUp,
	TouchActionPageDown,
	TouchActionSelectOnly,
	TouchActionCount,
};




typedef struct
{
	int x;
	int y;
}_TouchPoint;


typedef struct
{
	short left;
	short top;
	short width;
	short height;
	short action;
	short param;

	bool Contain(_TouchPoint pt) const
	{
		return pt.x >= left && pt.y >= top && pt.x < left + width && pt.y < top + height;
	}

}_TouchRect;


typedef struct
{
	int			count;

	_TouchRect	rects[80];

	void Append(_TouchRect& rect){rects[count++] = rect;};
}_TouchRectList;


#if (PLATFORM == PLATFORM_WIN32) || (PLATFORM == PLATFORM_WINDOWS_MOBILE) || (PLATFORM == PLATFORM_MAC_OS_X)


enum IconState
{
	IconNone = -1,
	IconMove,
	IconResizeWL,
	IconResizeWR,
	IconResizeHT,
	IconResizeHB,
	IconResizeWHRB,
};


class CTouchRect
{
public:
	CTouchRect(int left = 0, int top = 0, int width = 0, int height = 0):m_left(left), m_top(top), m_width(width), m_height(height){};
	~CTouchRect(void){};

	IconState GetIconState(int x, int y);
	
	inline int Left() const {return m_left;};
	inline int Top() const{return m_top;};
	inline int Width() const{return m_width;};
	inline int Height() const{return m_height;};


private:
	int m_left;
	int m_top;
	int m_width;
	int m_height;
};

#endif

#endif