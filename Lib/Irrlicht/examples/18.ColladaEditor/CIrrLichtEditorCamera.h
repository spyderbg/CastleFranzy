#ifndef __CIRRLICHTEDITORCAMERA_H__
#define __CIRRLICHTEDITORCAMERA_H__

#include "CCameraSceneNode.h"
#include "CSceneNodeAnimatorCameraMaya.h"
#include "irrTypes.h"

namespace irr
{
class CEditorCameraSceneNode
	: public scene::CCameraSceneNode
{
public:
	CEditorCameraSceneNode()
		: irr::scene::CCameraSceneNode(0)
		, GameScroll(0)
		, CanvasScroll(0)
		, GameMouseX(0)
		, GameMouseY(0)
		, CanvasMouseX(0)
		, CanvasMouseY(0)
	{
		for(int i = 0; i < 4; i++)
		{
			KeysDown[i] = false;
		}

		for(int i = 0; i < 3; i++)
		{
			MouseDown[i] = false;
		}
	}
//! OnAnimate() is called just before rendering the whole scene.
//! nodes may calculate or store animations here, and may do other useful things,
//! dependent on what they are.
	virtual void OnAnimate(u32 timeMs)
	{
		if(KeysDown[EK_UP])
		{
			irr::core::vector3df direction = getUpVector().crossProduct(getUpVector().crossProduct((Target - RelativeTranslation).normalize())).normalize();
			irr::core::vector3df translation = ((Target - RelativeTranslation).getLength() * direction * timeMs / 1000.0f * 0.001f);
			setPosition(RelativeTranslation - translation);
			setTarget(getTarget() - translation);
		}
		else if(KeysDown[EK_DOWN])
		{
			irr::core::vector3df direction = getUpVector().crossProduct(getUpVector().crossProduct((Target - RelativeTranslation).normalize())).normalize();
			irr::core::vector3df translation = ((Target - RelativeTranslation).getLength() * direction * timeMs / 1000.0f * 0.001f);
			setPosition(RelativeTranslation + translation);
			setTarget(getTarget() + translation);
		}
		else if(KeysDown[EK_RIGHT])
		{
			irr::core::vector3df direction = getUpVector().crossProduct((Target - RelativeTranslation).normalize());
			irr::core::vector3df translation = ((Target - RelativeTranslation).getLength() * direction * timeMs / 1000.0f * 0.001f);
			setPosition(RelativeTranslation + translation);
			setTarget(getTarget() + translation);
		}
		else if(KeysDown[EK_LEFT])
		{
			irr::core::vector3df direction = getUpVector().crossProduct((Target - RelativeTranslation).normalize());
			irr::core::vector3df translation = ((Target - RelativeTranslation).getLength() * direction * timeMs / 1000.0f * 0.001f);
			setPosition(RelativeTranslation - translation);
			setTarget(getTarget() - translation);
		}

		if(GameScroll != CanvasScroll)
		{
			irr::core::vector3df translation =  (RelativeTranslation - Target);
			setPosition(Target + translation * pow(1.25f, (CanvasScroll - GameScroll)));
			GameScroll = CanvasScroll;
		}

		if(MouseDown[EM_LEFT])
		{
			if(GameMouseY != CanvasMouseY)
			{
				
				irr::core::quaternion quat;
				quat.fromAngleAxis(((CanvasMouseY-GameMouseY)*3.141596)/100, (RelativeTranslation - Target).normalize().crossProduct(UpVector).normalize());
				irr::core::vector3df translation = RelativeTranslation - Target;
				translation = quat * translation;
				setPosition(translation + Target);
				GameMouseY = CanvasMouseY;
			}

			if(GameMouseX != CanvasMouseX) 
			{
				irr::core::quaternion quat;
				quat.fromAngleAxis(((CanvasMouseX-GameMouseX)*3.141596)/100, UpVector);
				irr::core::vector3df translation = RelativeTranslation - Target;
				translation = quat * translation;
				setPosition(translation + Target);
				GameMouseX = CanvasMouseX;
			}
		}
		else if(MouseDown[EM_RIGHT])
		{
			if(GameMouseX != CanvasMouseX) 
			{
				irr::core::vector3df direction = getUpVector().crossProduct((Target - RelativeTranslation).normalize());
				irr::core::vector3df translation = (CanvasMouseX - GameMouseX) * ((Target - RelativeTranslation).getLength() * direction * 0.01f);
				setPosition(RelativeTranslation - translation);
				setTarget(getTarget() - translation);
				GameMouseX = CanvasMouseX;
			}

			if(GameMouseY != CanvasMouseY) 
			{
				irr::core::vector3df direction = getUpVector();
				irr::core::vector3df translation = (CanvasMouseY - GameMouseY) * ((Target - RelativeTranslation).getLength() * direction * 0.01f);
				setPosition(RelativeTranslation + translation);
				setTarget(getTarget() + translation);
				GameMouseY = CanvasMouseY;
			}
		}

		ISceneNode::OnAnimate(timeMs);
	}

//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for 
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
	virtual bool OnEvent(const irr::SEvent& event)
	{
		CCameraSceneNode::OnEvent(event);

	/*	if (event.EventType != irr::EET_KEY_INPUT_EVENT)
			return false;*/

		if(event.EventType == irr::EET_KEY_INPUT_EVENT)
		{
			switch(event.KeyInput.Key)
			{
			case KEY_UP:
				KeysDown[EK_UP] = event.KeyInput.PressedDown;
				break;
			case KEY_DOWN:
				KeysDown[EK_DOWN] = event.KeyInput.PressedDown;
				break;
			case KEY_LEFT:
				KeysDown[EK_LEFT] = event.KeyInput.PressedDown;
				break;
			case KEY_RIGHT:
				KeysDown[EK_RIGHT] = event.KeyInput.PressedDown;
				break;
			}
		} else if(event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			switch(event.MouseInput.Event)
			{
			case EMIE_MOUSE_MOVED:
				if(MouseDown[EM_LEFT] || MouseDown[EM_MIDDLE] || MouseDown[EM_RIGHT])
				{
					CanvasMouseX = event.MouseInput.X;
					CanvasMouseY = event.MouseInput.Y;
				}
				break;
			case EMIE_LMOUSE_PRESSED_DOWN:
				MouseDown[EM_LEFT] = true;
				CanvasMouseX = GameMouseX = event.MouseInput.X;
				CanvasMouseY = GameMouseY = event.MouseInput.Y;
				break;
			case EMIE_RMOUSE_PRESSED_DOWN:
				MouseDown[EM_RIGHT] = true;
				CanvasMouseX = GameMouseX = event.MouseInput.X;
				CanvasMouseY = GameMouseY = event.MouseInput.Y;
				break;
			case EMIE_MMOUSE_PRESSED_DOWN:
				MouseDown[EM_MIDDLE] = true;
				CanvasMouseX = GameMouseX = event.MouseInput.X;
				CanvasMouseY = GameMouseY = event.MouseInput.Y;
				break;
			case EMIE_LMOUSE_LEFT_UP:
				MouseDown[EM_LEFT] = false;
				if(!MouseDown[EM_LEFT] && !MouseDown[EM_MIDDLE] && !MouseDown[EM_RIGHT])
				{
					CanvasMouseX = GameMouseX;
					CanvasMouseY = GameMouseY;
				}
				break;
			case EMIE_RMOUSE_LEFT_UP:
				MouseDown[EM_RIGHT] = false;
				if(!MouseDown[EM_LEFT] && !MouseDown[EM_MIDDLE] && !MouseDown[EM_RIGHT])
				{
					CanvasMouseX = GameMouseX;
					CanvasMouseY = GameMouseY;
				}
				break;
			case EMIE_MMOUSE_LEFT_UP:
				MouseDown[EM_MIDDLE] = false;
				if(!MouseDown[EM_LEFT] && !MouseDown[EM_MIDDLE] && !MouseDown[EM_RIGHT])
				{
					CanvasMouseX = GameMouseX;
					CanvasMouseY = GameMouseY;
				}
				break;
			case EMIE_MOUSE_WHEEL:
				CanvasScroll += event.MouseInput.Wheel;
				break;
			}
			
			
		}
		return true;
	}

	enum E_Keys
	{
		EK_UP = 0,
		EK_DOWN = 1,
		EK_LEFT = 2,
		EK_RIGHT = 3,
	};

	enum E_MouseButtons
	{
		EM_LEFT = 0,
		EM_MIDDLE = 1,
		EM_RIGHT = 2,
	};

	bool KeysDown[4];
	bool MouseDown[3];
	int CanvasMouseX, CanvasMouseY;
	int GameMouseX, GameMouseY;
	float CanvasScroll, GameScroll;
};
}; // namespace irr

#endif //__CIRRLICHTEDITORCAMERA_H__