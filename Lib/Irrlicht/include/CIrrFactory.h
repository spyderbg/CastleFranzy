#ifndef __C_IRRFACTORY_H_INCLUDED__
#define __C_IRRFACTORY_H_INCLUDED__

namespace irr
{
	class IOSOperator;

	namespace io
	{
		class IFileSystem;
	}
	namespace scene
	{
		class ISceneManager;
	}
	namespace gui
	{
		class ICursorControl;
		class IGUIEnvironment;
	}
	namespace video
	{
		class IVideoDriver;
		class CNullDriver;
		class CTextureManager;
	}

	//
	class CIrrFactory
	{
	public:

		CIrrFactory();
		virtual ~CIrrFactory();
		static CIrrFactory* getInstance();

		// factory functions
		virtual io::IFileSystem* createFileSystem();

		// creates a scenemanager
		virtual scene::ISceneManager* createSceneManager(
			irr::video::IVideoDriver* driver,
			irr::io::IFileSystem* fs, irr::gui::ICursorControl* cursorcontrol,
			irr::gui::IGUIEnvironment *guiEnvironment);
		
		virtual gui::IGUIEnvironment* createGUIEnvironment(io::IFileSystem* fs, 
			video::IVideoDriver* Driver, IOSOperator* op);

		virtual video::CTextureManager* createTextureManager(video::CNullDriver* Driver);
	protected:
		static CIrrFactory* s_instance;

	};

} // end namespace

#endif

