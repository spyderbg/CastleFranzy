#ifndef __MENU_ITEM_H__
#define __MENU_ITEM_H__

#include "GLLiveMenuItemObserver.h"
#include "GLLiveStateHandler.h"
#include "GLXPlayerObserver.h"

/**
 * The item in menu.
 * <p>
 *
 * @version 0.0.1
 *
 */
class GLLiveMenuItem
{
public:
	GLLiveMenuItem(GLXPlayerObserver * observer, int id, const _TouchRect &rect, const char * title = NULL);
	~GLLiveMenuItem(void);

	void Repaint(GLLiveStateHandler * handler);
	inline void SetSelected(bool selected)
	{
		m_bSelected = selected;	
	};

	void OnClicked();

	void SetEnable(bool bEnable);
	void SetVisible(bool bVisible);
	void SetRect(const _TouchRect& rect);
	void SetPic(int asprite, int frame, int x = 0, int y = 0);
	void SetSelPic(int asprite, int frame, int x = 0, int y = 0);
	void SetTitle(const char *title);
	void SetFont(int font, int lineHeight = -1);
	void SetTitlePos(int x, int y, int anchor, int width = 0);
	bool IsEnable() const;
	bool IsVisible() const;
	bool IsSelected() const;
	const _TouchRect& GetRect() const;
	int GetId();

private:
	class ButtonPic
	{
	public:
		ButtonPic()
		{
			asprite = -1;
			frame   = -1;
			x = 0;
			y = 0;
		}
		
		bool CanShow()
		{
			return asprite > -1 && frame > -1;
		}
		int asprite;
		int frame;
		int x;
		int y;
	};
	//int m_x;
	//int m_y;
	//int m_w;
	//int m_h;
	_TouchRect m_rect; 
	GLXPlayerObserver *m_observer;
	char *m_title;
	int  m_titlePosX;
	int  m_titlePosY;
	int  m_titleAnchor;
	int  m_titleWidth;
	bool m_bSelected;
	bool m_bEnable;
	bool m_bVisible;
	int  m_id;
	int  m_font;
	int  m_fontLineHeight;
	ButtonPic * m_pic;
	ButtonPic * m_selPic;
};

#endif // __MENU_ITEM_H__