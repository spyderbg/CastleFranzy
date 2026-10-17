#include "stdafx.h"
#include "EditorUI.h"
#include "Game/LevelInfo.h"
#include "Game/Misc.h"
#include <map>
#include <set>
#include <stack>

UIID::UIID (void* id, int64_t index, int64_t item)
{
	this->id = id;
	this->index = index;
	this->item = item;
}

UIID& UIID::setItem (int64_t item)
{
	this->item = item;
	return *this;
}

bool operator< (UIID a, UIID b)
{
	return a.id < b.id || a.id == b.id && a.index < b.index || a.id == b.id && a.index == b.index && a.item < b.item;
}
bool operator== (UIID a, UIID b)
{
	return a.id == b.id && a.index == b.index && a.item == b.item;
}

// ----------------------------------------------------------------------------

class CFilledRect : public IGUIElement
{
public:
	CFilledRect (r2i rc, SColor color, IGUIElement* parent, int id = -1);
	~CFilledRect ();

	void draw ();

	SColor color;
};

CFilledRect::CFilledRect (r2i rc, SColor color, IGUIElement* parent, int id)
:	IGUIElement(EGUIET_ELEMENT, __IRR_GUI(), parent, id, rc)
{
	this->color = color;
}

CFilledRect::~CFilledRect ()
{
}

void CFilledRect::draw ()
{
	__IRR_DRIVER()->draw2DRectangle(color, AbsoluteRect, &AbsoluteClippingRect);
	IGUIElement::draw();
}

class CDrawRect : public IGUIElement
{
public:
	CDrawRect (r2i rc, SColor color, IGUIElement* parent, int id = -1);
	~CDrawRect ();

	void draw ();

	SColor color;
};

CDrawRect::CDrawRect (r2i rc, SColor color, IGUIElement* parent, int id)
:	IGUIElement(EGUIET_ELEMENT, __IRR_GUI(), parent, id, rc)
{
	this->color = color;
}

CDrawRect::~CDrawRect ()
{
}

void CDrawRect::draw ()
{
	// top
	r2i rc = AbsoluteRect;
	rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + 1;
	__IRR_DRIVER()->draw2DRectangle(color, rc, &AbsoluteClippingRect);

	// bottom
	rc = AbsoluteRect;
	rc.UpperLeftCorner.Y = rc.LowerRightCorner.Y - 1;
	__IRR_DRIVER()->draw2DRectangle(color, rc, &AbsoluteClippingRect);

	// left
	rc = AbsoluteRect;
	rc.LowerRightCorner.X = rc.UpperLeftCorner.X + 1;
	++rc.UpperLeftCorner.Y;
	--rc.LowerRightCorner.Y;
	__IRR_DRIVER()->draw2DRectangle(color, rc, &AbsoluteClippingRect);

	// right
	rc = AbsoluteRect;
	rc.UpperLeftCorner.X = rc.LowerRightCorner.X - 1;
	++rc.UpperLeftCorner.Y;
	--rc.LowerRightCorner.Y;
	__IRR_DRIVER()->draw2DRectangle(color, rc, &AbsoluteClippingRect);

	IGUIElement::draw();
}

struct TimeUnitRowState
{
	float time;
	int type;
	int row;
	UnitIconState state;
};

bool operator< (const TimeUnitRowState& a, const TimeUnitRowState& b)
{
	return a.time < b.time;
}

class CUnitIconTimeline : public IGUIElement
{
public:
	CUnitIconTimeline (r2i rc, IGUIElement* parent, int id = -1);
	~CUnitIconTimeline ();

	void draw ();
	void setLevelEndTime (int levelEndTime);
	void clearItems ();
	void addItem (float time, int unitType, UnitIconState state);
	void sortItems ();
	void setTime (float time);

private:
	int m_levelEndTime;
	array<TimeUnitRowState> m_items;
	float m_time;
	int m_maxRow;
};

CUnitIconTimeline::CUnitIconTimeline (r2i rc, IGUIElement* parent, int id)
:	IGUIElement(EGUIET_ELEMENT, __IRR_GUI(), parent, id, rc)
{
	m_levelEndTime = 100;
	m_time = 0;
}

CUnitIconTimeline::~CUnitIconTimeline ()
{
}



void CUnitIconTimeline::setLevelEndTime (int levelEndTime)
{
	m_levelEndTime = levelEndTime;
}

void CUnitIconTimeline::clearItems ()
{
	m_items.set_used(0);
}

void CUnitIconTimeline::addItem (float time, int unitType, UnitIconState state)
{
	TimeUnitRowState turs = { time, unitType, -1, state };
	m_items.push_back(turs);
}

void CUnitIconTimeline::sortItems ()
{
	m_items.sort();
	m_maxRow = 0;
	std::map<int, float> sweep;
	float timePerIcon = 20.0f * float(m_levelEndTime) / AbsoluteRect.getWidth();
	for (int i = 0; i < m_items.size(); ++i)
	{
		// find an empty row
		int row = 0;
		for (; sweep.find(row) != sweep.end() && sweep[row] > m_items[i].time; ++row)
		{
		}
		sweep[row] = m_items[i].time + timePerIcon;
		m_items[i].row = row;
		if (row > m_maxRow)
			m_maxRow = row;
	}
}

void CUnitIconTimeline::setTime (float time)
{
	m_time = time;
}

// ----------------------------------------------------------------------------

class CTimelineRow;

class CTimeline : public IGUIElement
{
public:
	CTimeline (r2i rc, IGUIElement* parent, int id = -1);
	~CTimeline ();

	void draw ();

	array<CTimelineRow*> m_rows;
	int m_levelEndTime;
};

CTimeline::CTimeline (r2i rc, IGUIElement* parent, int id)
:	IGUIElement(EGUIET_ELEMENT, __IRR_GUI(), parent, id, rc)
{
}

CTimeline::~CTimeline ()
{
}

void CTimeline::draw ()
{
	if (!IsVisible)
		return;

	// draw background
	SColor col = __IRR_GUI()->getSkin()->getColor(EGDC_3D_FACE);
	__IRR_DRIVER()->draw2DRectangle(col, AbsoluteClippingRect);

	// draw major line every 10 seconds
	int y0 = AbsoluteClippingRect.UpperLeftCorner.Y;
	int y1 = AbsoluteClippingRect.LowerRightCorner.Y;
	for (int sec = 0; sec <= m_levelEndTime; sec += 10)
	{
		int x = IntRemap1(sec, 0, m_levelEndTime,
			AbsoluteRect.UpperLeftCorner.X, AbsoluteRect.LowerRightCorner.X);
		__IRR_DRIVER()->draw2DLine(p2i(x, y0), p2i(x, y1), 0x80000000);
	}

	IGUIElement::draw();
}

class CTimelineRow : public IGUIElement
{
public:
	CTimelineRow (r2i rc, IGUIElement* parent, int id = -1);
	~CTimelineRow ();

	void draw ();

	bool OnEvent (const SEvent& ev);

	int m_levelEndTime;
	int m_timeStart;
	int m_timeInterval;
	int m_repeatInterval;
	int m_repeatCount;

private:
	int m_start;
	p2i m_drag;
	bool m_dragging;
	int* m_dragValue;
};

CTimelineRow::CTimelineRow (r2i rc, IGUIElement* parent, int id)
:	IGUIElement(EGUIET_ELEMENT, __IRR_GUI(), parent, id, rc)
{
	m_levelEndTime = 10;
	m_dragging = false;
}

CTimelineRow::~CTimelineRow ()
{
}

void CTimelineRow::draw ()
{
	if (!IsVisible)
		return;

	p2i p0 = AbsoluteRect.UpperLeftCorner;
	int w = AbsoluteClippingRect.getWidth();
	for (int i = 0; i < m_repeatCount; ++i)
	{
		int sec = m_timeStart + m_repeatInterval * i;
		p2i p = p2i(IntRemap1(sec, 0, m_levelEndTime, p0.X, p0.X + w), p0.Y);
		d2i sz = d2i(IntRemap1(m_timeInterval, 0, m_levelEndTime, 0, w), AbsoluteRect.getHeight() - 1);
		if (sz.Width < 3)
			sz.Width = 3;
		__IRR_DRIVER()->draw2DRectangle(0xff000000, r2i(p, sz), &AbsoluteClippingRect);
		p += p2i(1,1);
		sz.Width -= 2;
		sz.Height -= 2;
		__IRR_DRIVER()->draw2DRectangle(m_dragging ? 0xff80ff80 : 0xff60bf60, r2i(p, sz), &AbsoluteClippingRect);
	}

	IGUIElement::draw();
}

bool CTimelineRow::OnEvent (const SEvent& ev)
{
	if (ev.EventType == EET_MOUSE_INPUT_EVENT)
	{
		if (!m_dragging &&
			(ev.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN ||
			 ev.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN ||
			 ev.MouseInput.Event == EMIE_MMOUSE_PRESSED_DOWN))
		{
			m_drag = p2i(ev.MouseInput.X, ev.MouseInput.Y);
			m_dragValue =
				(ev.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) ?
					&m_timeStart :
				(ev.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN) ?
					&m_timeInterval :
					&m_repeatInterval;
			m_start = *m_dragValue;
			m_dragging = true;
			__IRR_GUI()->setFocus(this);
			return true;
		}

		if (ev.MouseInput.Event == EMIE_LMOUSE_LEFT_UP ||
			ev.MouseInput.Event == EMIE_RMOUSE_LEFT_UP ||
			ev.MouseInput.Event == EMIE_MMOUSE_LEFT_UP)
		{
			if (m_dragging)
			{
				m_dragging = false;
				__IRR_GUI()->removeFocus(this);
				return true;
			}
		}

		if (ev.MouseInput.Event == EMIE_MOUSE_MOVED)
		{
			if (m_dragging)
			{
				int dx = ev.MouseInput.X - m_drag.X;
				int dsec = IntRemap1(dx, 0, AbsoluteClippingRect.getWidth(), 0, m_levelEndTime);
				*m_dragValue = m_start + dsec;
				if (*m_dragValue < 0)
					*m_dragValue = 0;
				if (*m_dragValue > m_levelEndTime)
					*m_dragValue = m_levelEndTime;
				return true;
			}
		}

		if (ev.MouseInput.Event == EMIE_MOUSE_WHEEL)
		{
			m_repeatCount += ev.MouseInput.Wheel;
			if (m_repeatCount < 1)
				m_repeatCount = 1;
			if (m_repeatCount > 99)
				m_repeatCount = 99;
		}
	}

	return IGUIElement::OnEvent(ev);
}


// ----------------------------------------------------------------------------

struct IDData
{
	IGUIElement* element;
	bool touched;
	bool clicked;
	int lastListItem;
	UIID up, down;
};

struct UIState
{
	UIState ()
	{
		sprites = 0;
		mgr = 0;
		rotatingCamera = false;
	}

	IGUISpriteBank* sprites;
	ISceneManager* mgr;
	ISceneNode* ground;

	std::map<UIID, IDData> idToData;
	std::map<IGUIElement*, UIID> elemToId;
	std::set<UIID> trackedIds;
	std::stack<IGUIElement*> parent;
	int64_t lastId;

	p2i layoutStart;
	p2i layoutCurrent;
	int layoutRowH;
	d2i lastSize;
	bool disableAutoNext;

	bool rotatingCamera;
	p2i mouseStart;
};

static UIState s_ui;

class CIcon : public IGUIElement
{
public:
	CIcon (int icon, r2i rc, SColor color, IGUIElement* parent, int id = -1);
	~CIcon ();

	void draw ();

	int icon;
	SColor color;
};

CIcon::CIcon (int icon, r2i rc, SColor color, IGUIElement* parent, int id)
:	IGUIElement(EGUIET_ELEMENT, __IRR_GUI(), parent, id, rc)
{
	this->icon = icon;
	this->color = color;
}

CIcon::~CIcon ()
{
}

void CIcon::draw ()
{
	SColor colors[] = { color, color, color, color };
	__IRR_DRIVER()->draw2DImage(
		s_ui.sprites->getTexture(0),
		AbsoluteRect,
		s_ui.sprites->getPositions()[icon],
		&AbsoluteClippingRect,
		colors,
		true);
	IGUIElement::draw();
}

void CUnitIconTimeline::draw ()
{
	// draw background
	SColor col = __IRR_GUI()->getSkin()->getColor(EGDC_3D_FACE);
	__IRR_DRIVER()->draw2DRectangle(col, AbsoluteClippingRect);

	// draw major line every 10 seconds
	int y0 = AbsoluteClippingRect.UpperLeftCorner.Y;
	int y1 = AbsoluteClippingRect.LowerRightCorner.Y;
	for (int sec = 0; sec <= m_levelEndTime; sec += 10)
	{
		int x = IntRemap1(sec, 0, m_levelEndTime,
			AbsoluteRect.UpperLeftCorner.X, AbsoluteRect.LowerRightCorner.X);
		__IRR_DRIVER()->draw2DLine(p2i(x, y0), p2i(x, y1), 0x80000000);
	}

	// draw icons
	float pixelsPerRow = (AbsoluteClippingRect.getHeight() - 20) / float(m_maxRow + 1);
	if (pixelsPerRow > 20.0f)
		pixelsPerRow = 20.0f;
	for (int i = 0; i < m_items.size(); ++i)
	{
		r2i rc = r2i(
			p2i(FloatRemap(m_items[i].time, 0.0f, m_levelEndTime, AbsoluteRect.UpperLeftCorner.X, AbsoluteRect.LowerRightCorner.X), AbsoluteRect.UpperLeftCorner.Y + m_items[i].row * pixelsPerRow),
			d2i(20, 20));
		if (m_items[i].state == UIS_NextToSpawn)
			__IRR_DRIVER()->draw2DRectangle(0x80ff0000, rc, &AbsoluteClippingRect);
		if (m_items[i].state == UIS_Alive)
			__IRR_DRIVER()->draw2DRectangle(0x80ffffff, rc, &AbsoluteClippingRect);

		SColor color = m_items[i].state == UIS_Future ? 0x80808080 : 0xffffffff;
		SColor colors[] = { color, color, color, color };
		__IRR_DRIVER()->draw2DImage(
			s_ui.sprites->getTexture(0),
			rc,
			s_ui.sprites->getPositions()[m_items[i].type],
			&AbsoluteClippingRect,
			colors,
			true);

		if (m_items[i].state == UIS_Dead)
		{
			int x = rc.UpperLeftCorner.X;
			int y = rc.UpperLeftCorner.Y;
			__IRR_DRIVER()->draw2DRectangle(0xffff0000, r2i(p2i(x + 2, y + 6), d2i(16, 2)), &AbsoluteClippingRect);
			__IRR_DRIVER()->draw2DRectangle(0xffff0000, r2i(p2i(x + 9, y), d2i(2, 20)), &AbsoluteClippingRect);
		}
	}

	int x = FloatRemap(m_time, 0, m_levelEndTime,
		AbsoluteRect.UpperLeftCorner.X, AbsoluteRect.LowerRightCorner.X);
	__IRR_DRIVER()->draw2DLine(p2i(x, y0), p2i(x, y1), 0xff000000);

	IGUIElement::draw();
}

r2i AUTO_SIZE(0, 0, 0, 0);

r2i uiLayoutAutoWidth (int w)
{
	return r2i(s_ui.layoutCurrent, d2i(w, s_ui.layoutRowH));
}

void uiLayoutBegin (p2i pos, int rowH)
{
	s_ui.layoutStart = pos;
	s_ui.layoutCurrent = pos;
	s_ui.layoutRowH = rowH;
}

void uiLayoutNextColumn (int space)
{
	s_ui.layoutCurrent.X += s_ui.lastSize.Width + space;
}

void uiLayoutNextRow (int space)
{
	s_ui.layoutCurrent.X = s_ui.layoutStart.X;
	s_ui.layoutCurrent.Y += s_ui.layoutRowH + space;
}

void uiLayoutSpace (int width)
{
	s_ui.layoutCurrent.X += width;
}

void uiLayoutDisableAutoNextColumnForNextItem ()
{
	s_ui.disableAutoNext = true;
}

void uiLayoutAutoNextColumn ()
{
	if (!s_ui.disableAutoNext)
		uiLayoutNextColumn();
	s_ui.disableAutoNext = false;
}

UIID uiGenId ()
{
	return UIID(&s_ui, s_ui.lastId++);
}

IDData& uiTouchData (UIID id)
{
	IDData& result = s_ui.idToData[id];
	result.touched = true;
	return result;
}

void uiCreateData (UIID id, IGUIElement* elem)
{
	IDData data;
	elem->grab();
	data.element = elem;
	data.touched = false;
	data.clicked = false;
	data.up = uiGenId();
	data.down = uiGenId();
	s_ui.idToData[id] = data;
	s_ui.elemToId[elem] = id;
	s_ui.trackedIds.insert(id);
}

void uiPushParent (IGUIElement* elem)
{
	s_ui.parent.push(elem);
}

void uiPopParent ()
{
	s_ui.parent.pop();
}

// widgets
bool uiButton (UIID id, const wchar_t* text, r2i rc, bool enabled, const wchar_t* tooltip)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		// doesn't exist, create it!
		IGUIButton* button = __IRR_GUI()->addButton(rc, s_ui.parent.top(), -1, text, tooltip);
		uiCreateData(id, button);
	}

	IDData& data = uiTouchData(id);

	if (rc == AUTO_SIZE)
	{
		rc.UpperLeftCorner = s_ui.layoutCurrent;
		rc.LowerRightCorner.X = rc.UpperLeftCorner.X +
			__IRR_GUI()->getSkin()->getFont()->getDimension(text).Width + 6;
		rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + s_ui.layoutRowH;
	}

	s_ui.lastSize = rc.getSize();
	uiLayoutAutoNextColumn();

	data.element->setEnabled(enabled);
	data.element->setRelativePosition(rc);
	data.element->setText(text);
	data.element->setToolTipText(tooltip);

	bool result = data.clicked;
	data.clicked = false;
	return result;
}

bool uiPushButton (UIID id, bool* state, const wchar_t* text, r2i rc, bool enabled, const wchar_t* tooltip, ITexture* image, r2i imageRect)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		// doesn't exist, create it!
		IGUIButton* button = __IRR_GUI()->addButton(rc, s_ui.parent.top(), -1, text, tooltip);
		button->setIsPushButton(true);
		button->setUseAlphaChannel(true);
		button->setDrawBorder(false);
		uiCreateData(id, button);
	}

	IDData& data = uiTouchData(id);
	IGUIButton* button = static_cast<IGUIButton*>(data.element);

	if (rc == AUTO_SIZE)
	{
		rc.UpperLeftCorner = s_ui.layoutCurrent;
		rc.LowerRightCorner.X = rc.UpperLeftCorner.X +
			__IRR_GUI()->getSkin()->getFont()->getDimension(text).Width + 6;
		rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + s_ui.layoutRowH;
	}

	if (image)
	{
		if (imageRect.getArea() != 0)
		{
			button->setImage(image, imageRect);
			imageRect.UpperLeftCorner -= p2i(1,1);
			imageRect.LowerRightCorner += p2i(1,1);
			button->setPressedImage(image, imageRect);
		}
		else
		{
			button->setImage(image);
		}
	}
	else
		button->setImage(0);

	data.element->setEnabled(enabled);
	data.element->setRelativePosition(rc);
	data.element->setText(text);
	data.element->setToolTipText(tooltip);

	bool result = data.clicked;
	if (result)
	{
		*state = !*state;
	}
	button->setPressed(*state);
	data.clicked = false;

	if (*state)
	{
		uiLayoutDisableAutoNextColumnForNextItem();
		r2i rc2 = rc;
		rc2.UpperLeftCorner -= p2i(1,0);
		rc2.LowerRightCorner += p2i(1,1);
		uiFilledRect(rc2, 0x80ffffff);
		s_ui.parent.top()->bringToFront(button);
	}

	s_ui.lastSize = rc.getSize();
	uiLayoutAutoNextColumn();

	return result;
}

void uiEditString (UIID id, stringw* text, r2i rc, int maxChars, bool autoscroll)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		IGUIEditBox* edit = __IRR_GUI()->addEditBox(text->c_str(), rc, true, s_ui.parent.top());
		edit->setMax(maxChars);
		edit->setAutoScroll(autoscroll);
		uiCreateData(id, edit);
	}

	if (rc == AUTO_SIZE)
	{
		rc.UpperLeftCorner = s_ui.layoutCurrent;
		rc.LowerRightCorner.X = rc.UpperLeftCorner.X + (10 + 10 * maxChars);
		rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + s_ui.layoutRowH;
	}
	s_ui.lastSize = rc.getSize();
	uiLayoutAutoNextColumn();

	IDData& data = uiTouchData(id);
	data.element->setRelativePosition(rc);
	static_cast<IGUIEditBox*>(data.element)->setMax(maxChars);
	static_cast<IGUIEditBox*>(data.element)->setAutoScroll(autoscroll);
	if (__IRR_GUI()->hasFocus(data.element))
		*text = data.element->getText();
	else
		data.element->setText(text->c_str());
}

void uiEditFloat (UIID id, float* value, r2i rc, float min, float max, int maxChars, bool autoscroll)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		IGUIEditBox* edit = __IRR_GUI()->addEditBox(L"", rc, true, s_ui.parent.top());
		edit->setMax(maxChars);
		edit->setAutoScroll(autoscroll);
		uiCreateData(id, edit);
	}

	if (rc == AUTO_SIZE)
	{
		rc.UpperLeftCorner = s_ui.layoutCurrent;
		rc.LowerRightCorner.X = rc.UpperLeftCorner.X + (10 + 10 * maxChars);
		rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + s_ui.layoutRowH;
	}
	s_ui.lastSize = rc.getSize();
	uiLayoutAutoNextColumn();

	IDData& data = uiTouchData(id);
	data.element->setRelativePosition(rc);
	static_cast<IGUIEditBox*>(data.element)->setMax(maxChars);
	static_cast<IGUIEditBox*>(data.element)->setAutoScroll(autoscroll);
	if (__IRR_GUI()->hasFocus(data.element))
	{
		swscanf(data.element->getText(), L"%f", value);
		if (*value < min) *value = min;
		if (*value > max) *value = max;
	}
	else
	{
		data.element->setText(stringw(*value).c_str());
	}
}

void uiEditInt (UIID id, int* value, r2i rc, int min, int max, int maxChars, bool autoscroll)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		IGUIEditBox* edit = __IRR_GUI()->addEditBox(L"", rc, true, s_ui.parent.top());
		edit->setMax(maxChars);
		edit->setAutoScroll(autoscroll);
		uiCreateData(id, edit);
	}

	if (rc == AUTO_SIZE)
	{
		rc.UpperLeftCorner = s_ui.layoutCurrent;
		rc.LowerRightCorner.X = rc.UpperLeftCorner.X + (15 + 10 * maxChars);
		rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + s_ui.layoutRowH;
	}

	rc.LowerRightCorner.X -= rc.getSize().Height / 2;

	IDData& data = uiTouchData(id);
	data.element->setRelativePosition(rc);

	uiLayoutDisableAutoNextColumnForNextItem();
	if (uiButton(data.up, L"",
		r2i(p2i(rc.LowerRightCorner.X, rc.UpperLeftCorner.Y),
			d2i(rc.getSize().Height / 2, rc.getSize().Height / 2)),
		*value < max))
	{
		++*value;
	}
	uiLayoutDisableAutoNextColumnForNextItem();
	if (uiButton(data.down, L"",
		r2i(p2i(rc.LowerRightCorner.X, rc.UpperLeftCorner.Y + rc.getSize().Height / 2),
			d2i(rc.getSize().Height / 2, rc.getSize().Height / 2)),
		*value > min))
	{
		--*value;
	}

	s_ui.lastSize = rc.getSize();
	s_ui.lastSize.Width += rc.getSize().Height / 2;
	uiLayoutAutoNextColumn();

	static_cast<IGUIEditBox*>(data.element)->setMax(maxChars);
	static_cast<IGUIEditBox*>(data.element)->setAutoScroll(autoscroll);
	if (__IRR_GUI()->hasFocus(data.element))
	{
		swscanf(data.element->getText(), L"%d", value);
		if (*value < min) *value = min;
		if (*value > max) *value = max;
	}
	else
	{
		data.element->setText(stringw(*value).c_str());
	}
}

void uiCheckbox (UIID id, bool* value, r2i rc, const wchar_t* text)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		IGUICheckBox* check = __IRR_GUI()->addCheckBox(*value, rc, s_ui.parent.top(), -1, text);
		uiCreateData(id, check);
	}

	if (rc == AUTO_SIZE)
	{
		rc.UpperLeftCorner = s_ui.layoutCurrent;
		rc.LowerRightCorner.X = rc.UpperLeftCorner.X +
			__IRR_GUI()->getSkin()->getFont()->getDimension(text).Width + s_ui.layoutRowH + 4;
		rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + s_ui.layoutRowH;
	}
	s_ui.lastSize = rc.getSize();
	uiLayoutAutoNextColumn();

	IDData& data = uiTouchData(id);
	data.element->setRelativePosition(rc);
	IGUICheckBox* check = static_cast<IGUICheckBox*>(data.element);
	if (data.clicked)
	{
		data.clicked = false;
		*value = check->isChecked();
	}
	else
		check->setChecked(*value);
}

d2i uiParentSize ()
{
	return s_ui.parent.top()->getRelativePosition().getSize();
}

p2i uiParentAbsolutePos ()
{
	return s_ui.parent.top()->getAbsolutePosition().UpperLeftCorner;
}

int uiParentWidth ()
{
	return uiParentSize().Width;
}

int uiParentHeight ()
{
	return uiParentSize().Height;
}

int uiBeginTabs (UIID id, r2i rc, ...)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		IGUITabControl* tabs = __IRR_GUI()->addTabControl(rc, s_ui.parent.top(), true, true);
		tabs->setTabVerticalAlignment(EGUIA_LOWERRIGHT);
		tabs->setTabHeight(20);
		uiCreateData(id, tabs);

		va_list lst;
		va_start(lst, rc);
		const wchar_t* tab = 0;
		while (tab = va_arg(lst, const wchar_t*))
		{
			tabs->addTab(tab);
		}
		va_end(lst);
	}

	IDData& data = uiTouchData(id);
	data.element->setRelativePosition(rc);
	IGUITabControl* tabs = static_cast<IGUITabControl*>(data.element);
	uiPushParent(tabs->getTab(tabs->getActiveTab()));
	return tabs->getActiveTab();
}

void uiEndTabs (UIID id)
{
	uiPopParent();
}

void uiBeginList (UIID id, r2i rc)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		IGUIListBox* lst = __IRR_GUI()->addListBox(rc, s_ui.parent.top(), -1, true);
		lst->setSpriteBank(s_ui.sprites);
		uiCreateData(id, lst);
	}

	IDData& data = uiTouchData(id);
	data.element->setRelativePosition(rc);
	data.lastListItem = 0;
	uiPushParent(data.element);
}

void uiListItem (UIID id, const wchar_t* item, int icon)
{
	IDData& data = uiTouchData(id);
	assert(data.element == s_ui.parent.top());

	IGUIListBox* lst = static_cast<IGUIListBox*>(data.element);
	if (lst->getItemCount() > data.lastListItem)
		lst->setItem(data.lastListItem, item, icon);
	else
	{
		lst->addItem(item, icon);
	}
	++data.lastListItem;
}

void uiEndList (UIID id, int* selected)
{
	IDData& data = uiTouchData(id);
	IGUIListBox* lst = static_cast<IGUIListBox*>(data.element);

	int selectedElem = lst->getSelected();

	// delete all elements after last
	while (lst->getItemCount() > data.lastListItem)
		lst->removeItem(lst->getItemCount() - 1);

	// adjust selection
	if (__IRR_GUI()->hasFocus(lst))
	{
		*selected = lst->getSelected();
	}
	else
	{
		if (*selected < 0 || *selected >= lst->getItemCount())
		{
			*selected = -1;
		}
		if (lst->getSelected() != *selected)
		{
			lst->setSelected(*selected);
		}
	}

	uiPopParent();
}

void uiBeginComboBox (UIID id, r2i rc)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		IGUIComboBox* combo = __IRR_GUI()->addComboBox(rc, s_ui.parent.top());
		uiCreateData(id, combo);
	}

	s_ui.lastSize = rc.getSize();
	uiLayoutAutoNextColumn();

	IDData& data = uiTouchData(id);
	data.element->setRelativePosition(rc);
	data.lastListItem = 0;
	uiPushParent(data.element);
}

void uiComboBoxItem (UIID id, const wchar_t* item)
{
	IDData& data = uiTouchData(id);
	assert(data.element == s_ui.parent.top());

	IGUIComboBox* combo = static_cast<IGUIComboBox*>(data.element);
	if (combo->getItemCount() > data.lastListItem)
	{
		if (wcscmp(combo->getItem(data.lastListItem), item) != 0)
		{
			// kill all items below!
			while (combo->getItemCount() > data.lastListItem)
				combo->removeItem(combo->getItemCount() - 1);
			combo->addItem(item);
		}
	}
	else
		combo->addItem(item);
	++data.lastListItem;
}

void uiEndComboBox (UIID id, int* selected)
{
	IDData& data = uiTouchData(id);
	IGUIComboBox* combo = static_cast<IGUIComboBox*>(data.element);

	// delete all elements after last
	while (combo->getItemCount() > data.lastListItem)
		combo->removeItem(combo->getItemCount() - 1);

	// adjust selection
	if (__IRR_GUI()->hasFocus(combo))
	{
		*selected = combo->getSelected();
	}
	else
	{
		if (*selected < 0 || *selected >= combo->getItemCount())
		{
			*selected = -1;
		}
		if (combo->getSelected() != *selected)
		{
			combo->setSelected(*selected);
		}
	}

	uiPopParent();
}

void uiStaticText (const wchar_t* text, r2i rc, SColor color)
{
	if (rc == AUTO_SIZE)
	{
		rc.UpperLeftCorner = s_ui.layoutCurrent;
		rc.LowerRightCorner.X = rc.UpperLeftCorner.X +
			__IRR_GUI()->getSkin()->getFont()->getDimension(text).Width;
		rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + s_ui.layoutRowH;
	}
	rc.UpperLeftCorner += p2i(0, 2);
	rc.LowerRightCorner += p2i(0, 2);
	s_ui.lastSize = rc.getSize();
	uiLayoutAutoNextColumn();
	IGUIStaticText* txt = __IRR_GUI()->addStaticText(text, rc, false, false, s_ui.parent.top());
	txt->setOverrideColor(color);
	uiCreateData(uiGenId(), txt);
}

void uiFilledRect (r2i rc, SColor color)
{
	if (rc == AUTO_SIZE)
	{
		rc.UpperLeftCorner = s_ui.layoutCurrent;
		rc.LowerRightCorner.X = rc.UpperLeftCorner.X + s_ui.layoutRowH;
		rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + s_ui.layoutRowH;
	}
	s_ui.lastSize = rc.getSize();
	uiLayoutAutoNextColumn();
	CFilledRect* simpRc = new CFilledRect(rc, color, s_ui.parent.top());
	uiCreateData(uiGenId(), simpRc);
}

void uiIcon (int icon, r2i rc, SColor color)
{
	if (rc == AUTO_SIZE)
	{
		rc.UpperLeftCorner = s_ui.layoutCurrent;
		rc.LowerRightCorner.X = rc.UpperLeftCorner.X + s_ui.layoutRowH;
		rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + s_ui.layoutRowH;
	}
	s_ui.lastSize = rc.getSize();
	uiLayoutAutoNextColumn();
	CIcon* simpRc = new CIcon(icon, rc, color, s_ui.parent.top());
	uiCreateData(uiGenId(), simpRc);
}

void uiDrawRect (r2i rc, SColor color)
{
	if (rc == AUTO_SIZE)
	{
		rc.UpperLeftCorner = s_ui.layoutCurrent;
		rc.LowerRightCorner.X = rc.UpperLeftCorner.X + s_ui.layoutRowH;
		rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + s_ui.layoutRowH;
	}
	s_ui.lastSize = rc.getSize();
	uiLayoutAutoNextColumn();
	CDrawRect* simpRc = new CDrawRect(rc, color, s_ui.parent.top());
	uiCreateData(uiGenId(), simpRc);
}

void uiBegin (IGUIElement* root)
{
	if (s_ui.sprites == 0)
		s_ui.sprites = uiCreateSprites();

	if (s_ui.mgr == 0)
	{
		s_ui.mgr = __IRR_SCENE()->createNewSceneManager();

		// create light
		ILightSceneNode* light = s_ui.mgr->addLightSceneNode(s_ui.mgr->getRootSceneNode());
		SLight& data = light->getLightData();
		data.Type = ELT_AMBIENT;
		data.AmbientColor  = SColorf(1.0f, 1.0f, 1.0f, 1.0f);
		data.DiffuseColor  = SColorf(0.0f, 0.0f, 0.0f, 1.0f);
		data.SpecularColor = SColorf(0.0f, 0.0f, 0.0f, 1.0f);
		light->setVisible(true);

		// create camera
		ICameraSceneNode* cam = s_ui.mgr->addCameraSceneNode(s_ui.mgr->getRootSceneNode(), v3f(10, 10, 10), v3f(0, 0, 0));

		// add ground
		s_ui.ground = s_ui.mgr->addCubeSceneNode(1, s_ui.mgr->getRootSceneNode());
		s_ui.ground->setScale(v3f(10.0f, 0.1f, 10.0f));
		s_ui.ground->setPosition(v3f(0.0f, -0.05f, 0.0f));
		s_ui.ground->getMaterial(0).setAmbientColor(0xff204020);
		s_ui.ground->getMaterial(0).setEmissiveColor(0xff000000);
		s_ui.ground->setVisible(true);
	}

	std::vector<IGUIElement*> toDrop;

	// clear non touched ids
	for (std::set<UIID>::iterator it = s_ui.trackedIds.begin(); it != s_ui.trackedIds.end();)
	{
		IDData& data = s_ui.idToData[*it];
		if (!data.touched)
		{
			// kill this one!
			std::set<UIID>::iterator toKill = it;
			data.element->remove();
			toDrop.push_back(data.element);
			s_ui.elemToId.erase(data.element);
			s_ui.idToData.erase(*it);
			++it;
			s_ui.trackedIds.erase(toKill);
		}
		else
		{
			data.touched = false;
			++it;
		}
	}

	s_ui.disableAutoNext = false;

	for (int i = 0; i < toDrop.size(); ++i)
		toDrop[i]->drop();

	root->setRelativePosition(r2i(p2i(0, 0), __IRR_DRIVER()->getScreenSize()));
	uiPushParent(root);
}

void uiEnd ()
{
	uiPopParent();
}

void uiDrawGridIfGroundVisible ()
{
	if (s_ui.ground->getVisibility())
	{
		__IRR_DRIVER()->setTransform(ETS_WORLD, matrix4::EM4CONST_IDENTITY);
		SMaterial mat;
		mat.setFlag(EMF_LIGHTING, false);
		__IRR_DRIVER()->setMaterial(mat);
		for (int i = -5; i <= 5; ++i)
		{
			__IRR_DRIVER()->draw3DLine(v3f(i, 0.01f, -5.0f), v3f(i, 0.01f, +5.0f), 0xff60e060);
			__IRR_DRIVER()->draw3DLine(v3f(-5.0f, 0.01f, i), v3f(+5.0f, 0.01f, i), 0xff60e060);
		}
	}
}

void uiBeginUnitIconTimeline (UIID id, int levelEndTime, r2i rc)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		CUnitIconTimeline* timeline = new CUnitIconTimeline(rc, s_ui.parent.top());
		uiCreateData(id, timeline);
		timeline->drop();
	}

	IDData& data = uiTouchData(id);
	CUnitIconTimeline* timeline = static_cast<CUnitIconTimeline*>(data.element);
	timeline->setRelativePosition(rc);
	timeline->setLevelEndTime(levelEndTime);
	timeline->clearItems();
}

void uiUnitIconTimelineItem (UIID id, float time, int icon, UnitIconState state)
{
	static_cast<CUnitIconTimeline*>(uiTouchData(id).element)->addItem(time, icon, state);
}

void uiEndUnitIconTimeline (UIID id, float time)
{
	CUnitIconTimeline* timeline = static_cast<CUnitIconTimeline*>(uiTouchData(id).element);
	timeline->setTime(time);
	timeline->sortItems();
}

void uiBeginTimeline (UIID id, int levelEndTime, r2i rc)
{
	if (s_ui.idToData.find(id) == s_ui.idToData.end())
	{
		CTimeline* timeline = new CTimeline(rc, s_ui.parent.top());
		uiCreateData(id, timeline);
		timeline->drop();
	}

	// find my scroll
	int scrollPos = 0;
	IGUIElement* el = s_ui.parent.top();
	const list<IGUIElement*>& kids = el->getChildren();
	for (list<IGUIElement*>::ConstIterator it = kids.begin(); it != kids.end(); it++)
	{
		if ((*it)->getType() == EGUIET_SCROLL_BAR)
		{
			scrollPos = static_cast<IGUIScrollBar*>(*it)->getPos();
			break;
		}
	}

	IDData& data = uiTouchData(id);
	rc.UpperLeftCorner.Y -= scrollPos;
	rc.LowerRightCorner.Y -= scrollPos;
	data.lastListItem = 0;
	data.element->setRelativePosition(rc);
	CTimeline* timeline = static_cast<CTimeline*>(data.element);
	timeline->m_levelEndTime = levelEndTime;

	uiPushParent(data.element);
}

void uiTimelineItem (UIID id, Wave* item)
{
	IDData& data = uiTouchData(id);
	assert(data.element == s_ui.parent.top());

	CTimeline* timeline = static_cast<CTimeline*>(data.element);
	if (timeline->m_rows.size() > data.lastListItem)
	{
		timeline->m_rows[data.lastListItem]->m_levelEndTime = timeline->m_levelEndTime;
		if (__IRR_GUI()->hasFocus(timeline->m_rows[data.lastListItem]))
		{
			item->timeStart = timeline->m_rows[data.lastListItem]->m_timeStart;
			item->timeInterval = timeline->m_rows[data.lastListItem]->m_timeInterval;
			item->repeatInterval = timeline->m_rows[data.lastListItem]->m_repeatInterval;
			item->repeatCount = timeline->m_rows[data.lastListItem]->m_repeatCount;
		}
		else
		{
			timeline->m_rows[data.lastListItem]->m_timeStart = item->timeStart;
			timeline->m_rows[data.lastListItem]->m_timeInterval = item->timeInterval;
			timeline->m_rows[data.lastListItem]->m_repeatInterval = item->repeatInterval;
			timeline->m_rows[data.lastListItem]->m_repeatCount = item->repeatCount;
		}
	}
	else
	{
		int h = 17;
		CTimelineRow* row = new CTimelineRow(r2i(0, data.lastListItem * h, 2000, (data.lastListItem + 1) * h), s_ui.parent.top());
		row->m_levelEndTime = timeline->m_levelEndTime;
		row->m_timeStart = item->timeStart;
		row->m_timeInterval = item->timeInterval;
		row->m_repeatInterval = item->repeatInterval;
		row->m_repeatCount = item->repeatCount;
		timeline->m_rows.push_back(row);
	}
	++data.lastListItem;
}

void uiEndTimeline (UIID id, int* selected)
{
	IDData& data = uiTouchData(id);
	CTimeline* timeline = static_cast<CTimeline*>(data.element);

	// delete all elements after last
	while (timeline->m_rows.size() > data.lastListItem)
	{
		timeline->m_rows[timeline->m_rows.size() - 1]->remove();
		timeline->m_rows[timeline->m_rows.size() - 1]->drop();
		timeline->m_rows.erase(timeline->m_rows.size() - 1);
	}

	for (int i = 0; i < timeline->m_rows.size(); ++i)
		if (__IRR_GUI()->hasFocus(timeline->m_rows[i]))
		{
			*selected = i;
			break;
		}

	uiPopParent();
}

void uiNotifyEvent (const SEvent& ev, r2i cam3dviewport)
{
	switch (ev.EventType)
	{
		case EET_GUI_EVENT:
		{
			switch (ev.GUIEvent.EventType)
			{
				case EGET_BUTTON_CLICKED:
				case EGET_CHECKBOX_CHANGED:
				{
					if (s_ui.elemToId.find(ev.GUIEvent.Caller) != s_ui.elemToId.end())
					{
						s_ui.idToData[s_ui.elemToId[ev.GUIEvent.Caller]].clicked = true;
					}
					break;
				}
			}
		}

		case EET_MOUSE_INPUT_EVENT:
		{
			switch (ev.MouseInput.Event)
			{
				case EMIE_LMOUSE_PRESSED_DOWN:
					if (cam3dviewport.isPointInside(p2i(ev.MouseInput.X, ev.MouseInput.Y)))
					{
						s_ui.rotatingCamera = true;
						s_ui.mouseStart = p2i(ev.MouseInput.X, ev.MouseInput.Y);
					}
					break;

				case EMIE_LMOUSE_LEFT_UP:
					s_ui.rotatingCamera = false;
					break;

				case EMIE_MOUSE_MOVED:
					if (s_ui.rotatingCamera)
					{
						ICameraSceneNode* cam = uiGetSceneManager()->getActiveCamera();
						v3f pos = cam->getPosition();
						v3f right = -cam->getUpVector().crossProduct(pos - cam->getTarget()).normalize();
						float speedX = 0.01f;
						float speedY = 0.01f;
						int dx = ev.MouseInput.X - s_ui.mouseStart.X;
						int dy = ev.MouseInput.Y - s_ui.mouseStart.Y;

						quaternion rotX; rotX.fromAngleAxis(speedX * dx, v3f(0, 1, 0));
						quaternion rotY; rotY.fromAngleAxis(speedY * dy, right);
						pos = rotX * rotY * pos;

						cam->setPosition(pos);

						s_ui.mouseStart = p2i(ev.MouseInput.X, ev.MouseInput.Y);
					}
					break;

				case EMIE_MOUSE_WHEEL:
				{
					if (cam3dviewport.isPointInside(p2i(ev.MouseInput.X, ev.MouseInput.Y)))
					{
						ICameraSceneNode* cam = uiGetSceneManager()->getActiveCamera();
						v3f pos = cam->getPosition();
						if (ev.MouseInput.Wheel < 0)
							pos *= 1.5f;
						else
							pos /= 1.5f;
						cam->setPosition(pos);
						}
					break;
				}
			}
		}
	}
}

ISceneManager* uiGetSceneManager ()
{
	return s_ui.mgr;
}
