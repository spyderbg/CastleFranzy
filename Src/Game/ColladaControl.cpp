#include "stdafx.h"
#include "ColladaControl.h"

ColladaControl::ColladaControl ()
{
	m_root = 0;
	m_nextAnim = -1;

	m_nextAnimLoop = 0;
	m_nextAnimSpeed = 0;
	m_currentAnim = -1;
	m_animOver = false;
}

ColladaControl::~ColladaControl ()
{
}

void ColladaControl::setRoot (ISceneNode* root)
{
	if (m_root)
	{
		collada::ISceneNodeAnimator* anim = (collada::ISceneNodeAnimator*)m_root->getAnimators().getFirstElem();
		anim->getTimelineCtrl()->setCallback(0, 0);
		anim->setEventsCallback(0);
		m_root = 0;
	}
	
	GX_ASSERT(!m_root);
	{
		m_root = root;
		m_animOver = false;
		m_currentAnim = -1;

		collada::ISceneNodeAnimator* anim = (collada::ISceneNodeAnimator*)m_root->getAnimators().getFirstElem();
		anim->getTimelineCtrl()->setCallback(&AnimEndCallback, this);
		anim->setEventsCallback(&EventsCallback, this);
	}
}

void ColladaControl::updateAtEndOfFrame (f32 time)
{
	m_events.clear();
}

ISceneNode* ColladaControl::getRoot ()
{
	return m_root;
}

// the event "anim_end" is generated when the animation is over
// other events can be added from 3ds max
bool ColladaControl::checkEvent (const char* name)
{
	for (int i = 0; i < m_events.size(); ++i)
		if (strcmp(name, m_events[i].c_str()) == 0)
			return true;
	return false;
}

void ColladaControl::setAnim (const char* name, bool loop, float speed)
{
	setAnim(getAnimIndex(name), loop, speed);
}

void ColladaControl::setAnim (int index, bool loop, float speed)
{
	ITimelineController* ctrl = m_root->getAnimators().getFirstElem()->getTimelineCtrl();
	ctrl->setClip(index);
	ctrl->setLoop(loop);
	ctrl->setScale(speed);
	m_currentAnim = index;

	m_nextAnim = -1;
}

void ColladaControl::setNextAnim (const char* name, bool loop, float speed)
{
	setNextAnim(getAnimIndex(name), loop, speed);
}

void ColladaControl::setNextAnim (int index, bool loop, float speed)
{
	m_nextAnim = index;
	m_nextAnimLoop = loop;
	m_nextAnimSpeed = speed;
}

void ColladaControl::setAnimAnim (const char* name1, const char* name2, bool loop2, float speed1, float speed2)
{
	setAnimAnim(getAnimIndex(name1), getAnimIndex(name2), loop2, speed1, speed2);
}

void ColladaControl::setAnimAnim (int index1, int index2, bool loop2, float speed1, float speed2)
{
	setAnim(index1, false, speed1);
	setNextAnim(index2, loop2, speed2);
}

const char* ColladaControl::getCurrentAnimName ()
{
	return getAnimName(getCurrentAnimIndex());
}

int ColladaControl::getCurrentAnimIndex ()
{
	return m_currentAnim;
}

int ColladaControl::getAnimIndex (const char* name)
{
	ITimelineController* ctrl = m_root->getAnimators().getFirstElem()->getTimelineCtrl();
	return ctrl->getClipIndex(name);
}

const char* ColladaControl::getAnimName (int index)
{
	ITimelineController* ctrl = m_root->getAnimators().getFirstElem()->getTimelineCtrl();
	const char* result = ctrl->getClipName(index);
	return result ? result : "";
}

void ColladaControl::EventsCallback (const collada::STriggeredEvent& ev, void* self)
{
	ColladaControl* me = (ColladaControl*)self;
	me->m_events.push_back(ev.name);
}

void ColladaControl::AnimEndCallback (ITimelineController* ctrl, void* self)
{
	ColladaControl* me = (ColladaControl*)self;
	if (me->m_nextAnim != -1)
	{
		me->setAnim(me->m_nextAnim, me->m_nextAnimLoop, me->m_nextAnimSpeed);
		me->m_events.push_back("anim_next");
	}
	else
	{
		me->m_events.push_back("anim_end");
	}
}