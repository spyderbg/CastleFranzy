#include "stdafx.h"
#include "CIrrFactory.h"
#include "CFileSystem.h"
#include "CSceneManager.h"
#include "CGUIEnvironment.h"
#include "CTextureManager.h"

namespace irr
{
	CIrrFactory* CIrrFactory::s_instance = 0;

	CIrrFactory::CIrrFactory()
	{
		s_instance = this;
	}

	CIrrFactory::~CIrrFactory()
	{
	}

	CIrrFactory* CIrrFactory::getInstance()
	{
		if (!s_instance)
		{
			// create the default factory
			static CIrrFactory inst;
			return &inst;
		}
		return s_instance;
	}

	// factory functions
	io::IFileSystem* CIrrFactory::createFileSystem()
	{
		return irrnew io::CFileSystem();
	}

	// creates a scenemanager
	scene::ISceneManager* CIrrFactory::createSceneManager(video::IVideoDriver* driver,
			io::IFileSystem* fs, gui::ICursorControl* cursorcontrol,
			gui::IGUIEnvironment *guiEnvironment)
	{
		return irrnew scene::CSceneManager(driver, fs, cursorcontrol, 0, guiEnvironment );
	}

	gui::IGUIEnvironment* CIrrFactory::createGUIEnvironment(io::IFileSystem* fs, 
			video::IVideoDriver* Driver, IOSOperator* op)
	{
#ifdef _IRR_COMPILE_WITH_GUI_
		return irrnew gui::CGUIEnvironment(fs, Driver, op);
#else
		return 0;
#endif 
	}

	video::CTextureManager* CIrrFactory::createTextureManager(video::CNullDriver* Driver)
	{
		return irrnew video::CTextureManager(Driver);
	}

} // end namespace irr

