#pragma once

#include "Core/irrlicht.h"
#include "IColladaSceneNodeAnimator.h"

class ColladaControl
{
public:
	ColladaControl ();
	~ColladaControl ();

	void updateAtEndOfFrame (f32 time);

	ISceneNode* getRoot ();
	void setRoot (ISceneNode* root);

	// the event "anim_end" is generated when the animation is over
	// "anim_next" is generated when the first animation set by setAnimAnim ends
	// other events can be added from 3ds max
	bool checkEvent (const char* name);

	void setAnim (const char* name, bool loop = true, float speed = 1.0f);
	void setAnim (int index, bool loop = true, float speed = 1.0f);

	// same as setAnim(x); setNextAnim(y);
	void setAnimAnim (const char* name1, const char* name2, bool loopSecond = true, float speedFirst = 1.0f, float speedSecond = 1.0f);
	void setAnimAnim (int index1, int index2, bool loopSecond = true, float speedFirst = 1.0f, float speedSecond = 1.0f);

	void setNextAnim (const char* name, bool loop = true, float speed = 1.0f);
	void setNextAnim (int index, bool loop = true, float speed = 1.0f);
	
	const char* getCurrentAnimName ();
	int getCurrentAnimIndex ();

	int getAnimIndex (const char* name);
	const char* getAnimName (int index);

private:
	static void EventsCallback (const collada::STriggeredEvent &ev, void* self);
	static void	AnimEndCallback (ITimelineController* ctrl, void* self);

	ISceneNode* m_root;
	bool m_animOver;
	array<stringc> m_events;
	int m_currentAnim;
	int m_nextAnim;
	float m_nextAnimSpeed;
	bool m_nextAnimLoop;
};
