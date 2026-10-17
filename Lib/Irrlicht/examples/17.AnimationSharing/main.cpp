/** Example 009 Mesh Viewer

This tutorial show how to create a more complex application with the engine.
We construct a simple mesh viewer using the user interface API and the
scene management of Irrlicht. The tutorial show how to create and use Buttons,
Windows, Toolbars, Menus, ComboBoxes, Tabcontrols, Editboxes, Images,
MessageBoxes, SkyBoxes, and how to parse XML files with the integrated XML
reader of the engine.

We start like in most other tutorials: Include all nesessary header files, add
a comment to let the engine be linked with the right .lib file in Visual
Studio, and declare some global variables. We also add two 'using namespace'
statements, so we do not need to write the whole names of all classes. In this
tutorial, we use a lot stuff from the gui namespace.
*/
#include <irrlicht.h>
#include <iostream>
#include "CColladaDatabase.h"
#include "CColladaConverter.h"
#include "CColladaSceneNodeAnimator.h"

using namespace irr;
using namespace gui;

#pragma comment(lib, "Irrlicht.lib")


/*
Some global variables used later on
*/
IrrlichtDevice *Device = 0;
core::stringc StartUpModelFile, StartUpAnimationFile;
core::stringw MessageText;
core::stringw Caption;
scene::IAnimatedMeshSceneNode* Model = 0;
scene::ISceneNode* SkyBox = 0;
scene::ISceneNodeAnimator *g_pAnimator = 0;
scene::ICameraSceneNode* Camera[2] = { 0, 0};
core::array<scene::ISceneNode*> g_Cameras;

/*
Toggle between various cameras
*/
void setActiveCamera ( scene::ICameraSceneNode* newActive )
{
	if ( 0 == Device )
		return;

	scene::ICameraSceneNode* active = Device->getSceneManager()->getActiveCamera ();

	newActive->setInputReceiverEnabled ( true );
	Device->getSceneManager()->setActiveCamera ( newActive );
}

/*
The three following functions do several stuff used by the mesh viewer. The
first function showAboutText() simply displays a messagebox with a caption and
a message text. The texts will be stored in the MessageText and Caption
variables at startup.
*/
void showAboutText()
{
	// create modal message box with the text
	// loaded from the xml file.
	Device->getGUIEnvironment()->addMessageBox(
		Caption.c_str(), MessageText.c_str());
}


/*
The second function loadModel() loads a model and displays it using an
addAnimatedMeshSceneNode and the scene manager. Nothing difficult. It also
displays a short message box, if the model could not be loaded.
*/
scene::ISceneNode* loadModel(const c8* fn)
{
	// modify the name if it a .pk3 file

	core::stringc filename ( fn );

	core::stringc extension;
	core::getFileNameExtension ( extension, filename );
	extension.make_lower();

	// load a model into the engine
	scene::ISceneNode* m;

	if(extension == ".dae")
	{
		core::stringc newFilename;
		newFilename = filename + ".bdae";
		if(collada::ConvertDAE2BDAE(filename.c_str(), newFilename.c_str()) != 0)
		{
			return 0;
		}

		scene::ISceneNode* m = Device->getSceneManager()->loadColladaScene( newFilename.c_str() );
		//scene::ISceneNode* m = Device->getSceneManager()->loadColladaScene( filename.c_str() );

		if (!m)
		{
			// model could not be loaded

			if (StartUpModelFile != filename)
				Device->getGUIEnvironment()->addMessageBox(
				Caption.c_str(), L"The model could not be loaded. " \
				L"Maybe it is not a supported file format.");
			return 0;
		}

		//listAnimationClip(m);
		
		Device->getSceneManager()->getRootSceneNode()->addChild(m);

		m->setPosition(core::vector3df(100,100,0));
	}
	else if(extension == ".bdae")
	{
		scene::ISceneNode* pScene = collada::CColladaDatabase::constructScene(filename.c_str(), true);

		//listAnimationClip(pScene);

		Device->getSceneManager()->getRootSceneNode()->addChild(pScene);
		pScene->setPosition(core::vector3df(100,100,0));
	}

	g_Cameras.clear();
	Device->getSceneManager()->getSceneNodesFromType(scene::ESNT_CAMERA, g_Cameras);
	return m;
}


/*
Most of the hard work is done. We only need to create the Irrlicht Engine
device and all the buttons, menus and toolbars. We start up the engine as
usual, using createDevice(). To make our application catch events, we set our
eventreceiver as parameter. The #ifdef WIN32 preprocessor commands are not
necessary, but I included them to make the tutorial use DirectX on Windows and
OpenGL on all other platforms like Linux. As you can see, there is also a
unusual call to IrrlichtDevice::setResizeAble(). This makes the render window
resizeable, which is quite useful for a mesh viewer.
*/
int main()
{
	// ask user for driver

	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D8;

	printf("Please select the driver you want for this example:\n"\
		" (a) Direct3D 9.0c\n (b) Direct3D 8.1\n (c) OpenGL 1.5\n"\
		" (d) Software Renderer\n (e) Burning's Software Renderer\n"\
		" (f) NullDevice\n (otherKey) exit\n\n");

	char key;
#ifdef _IRR_WII_PLATFORM_
	key = 'g';
#else
	key = 'c';
#endif

	switch(key)
	{
		case 'a': driverType = video::EDT_DIRECT3D9;break;
		case 'b': driverType = video::EDT_DIRECT3D8;break;
		case 'c': driverType = video::EDT_OPENGL;   break;
		case 'd': driverType = video::EDT_SOFTWARE; break;
		case 'e': driverType = video::EDT_BURNINGSVIDEO;break;
		case 'f': driverType = video::EDT_NULL;     break;
		case 'g': driverType = video::EDT_WII;     break;
		default: return 1;
	}

	// create device and exit if creation failed
	Device = createDevice(driverType, core::dimension2d<irr::s32>(800, 600),
		16, false, false, false, 0);

	if (Device == 0)
		return 1; // could not create selected driver.

	Device->setResizeAble(true);

	Device->setWindowCaption(L"Irrlicht Engine - Loading...");

	video::IVideoDriver* driver = Device->getVideoDriver();
	IGUIEnvironment* env = Device->getGUIEnvironment();
	scene::ISceneManager* smgr = Device->getSceneManager();
	smgr->getParameters()->setAttribute(scene::COLLADA_CREATE_SCENE_INSTANCES, true);

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	smgr->addLightSceneNode();
	smgr->addLightSceneNode(0, core::vector3df(50,-50,100), video::SColorf(1.0f,1.0f,1.0f),20000);
	// add our media directory as "search path"
	Device->getFileSystem()->addFolderFileArchive ( "../../media/" );

	/*
	The next step is to read the configuration file. It is stored in the xml
	format and looks a little bit like this:

	@verbatim
	<?xml version="1.0"?>
	<config>
		<startUpModel file="some filename" />
		<messageText caption="Irrlicht Engine Mesh Viewer">
			Hello!
		</messageText>
	</config>
	@endverbatim

	We need the data stored in there to be written into the global variables
	StartUpModelFile, MessageText and Caption. This is now done using the
	Irrlicht Engine integrated XML parser:
	*/

	// read configuration from xml file

	io::IXMLReader* xml = Device->getFileSystem()->createXMLReader("config.xml");

	while(xml && xml->read())
	{
		switch(xml->getNodeType())
		{
		case io::EXN_TEXT:
			// in this xml file, the only text which occurs is the
			// messageText
			MessageText = xml->getNodeData();
			break;
		case io::EXN_ELEMENT:
			{
				if (core::stringw("startUpModel") == xml->getNodeName())
					StartUpModelFile = xml->getAttributeValue(L"file");
				else if (core::stringw("startUpAnimation") == xml->getNodeName())
					StartUpAnimationFile = xml->getAttributeValue(L"file");
			}
			break;
		}
	}

	if (xml)
		xml->drop(); // don't forget to delete the xml reader

#ifdef _IRR_WINDOWS_
	if (__argc > 1)
		StartUpModelFile = __argv[1];
#endif
	/*
	That wasn't difficult. Now we'll set a nicer font and create the Menu.
	It is possible to create submenus for every menu item. The call
	menu->addItem(L"File", -1, true, true); for example adds a new menu
	Item with the name "File" and the id -1. The following parameter says
	that the menu item should be enabled, and the last one says, that there
	should be a submenu. The submenu can now be accessed with
	menu->getSubMenu(0), because the "File" entry is the menu item with
	index 0.
	*/

	// set a nicer font

	IGUISkin* skin = env->getSkin();
	IGUIFont* font = env->getFont("fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);


	/*
	Below the menu we want a toolbar, onto which we can place colored
	buttons and important looking stuff like a senseless combobox.
	*/

	// create toolbar

	gui::IGUIToolBar* bar = env->addToolBar();


	/*
	To make the editor look a little bit better, we disable transparent gui
	elements, and add an Irrlicht Engine logo. In addition, a text showing
	the current frames per second value is created and the window caption is
	changed.
	*/

	// disable alpha

	for (irr::s32 i=0; i<gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = env->getSkin()->getColor((gui::EGUI_DEFAULT_COLOR)i);
		col.setAlpha(255);
		env->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
	}

	// create fps text

	IGUIStaticText* fpstext = env->addStaticText(L"", core::rect<irr::s32>(400,4,570,23), true, false, bar);

	// set window caption

	Caption += " - [";
	Caption += driver->getName();
	Caption += "]";
	Device->setWindowCaption(Caption.c_str());

	/*
	That's nearly the whole application. We simply show the about message
	box at start up, and load the first model. To make everything look
	better, a skybox is created and a user controled camera, to make the
	application a little bit more interactive. Finally, everything is drawn
	in a standard drawing loop.
	*/

	scene::ISceneNode *pModel = collada::CColladaDatabase::constructScene(StartUpModelFile.c_str());
	scene::ISceneNodeAnimator *pAnim = collada::CColladaDatabase::constructAnimator(StartUpAnimationFile.c_str());
	//scene::ISceneNode *pAnim = loadModel(StartUpAnimationFile.c_str());
	
	//scene::ISceneNodeAnimator *pAnimator = (*pAnim->getAnimators().begin())->createClone();
	pModel->addAnimator(pAnim);
	pAnim->drop();
	smgr->getRootSceneNode()->addChild(pModel);

	// add skybox

	SkyBox = smgr->addSkyBoxSceneNode(
		driver->getTexture("irrlicht2_up.jpg"),
		driver->getTexture("irrlicht2_dn.jpg"),
		driver->getTexture("irrlicht2_lf.jpg"),
		driver->getTexture("irrlicht2_rt.jpg"),
		driver->getTexture("irrlicht2_ft.jpg"),
		driver->getTexture("irrlicht2_bk.jpg"));

	// add a camera scene node
	Camera[0] = smgr->addCameraSceneNodeMaya();
	Camera[0]->setFarValue(20000.f);
	Camera[1] = smgr->addCameraSceneNodeFPS();
	Camera[1]->setFarValue(20000.f);

	setActiveCamera ( Camera[0] );

	// load the irrlicht engine logo
	IGUIImage *img =
		env->addImage(driver->getTexture("irrlichtlogo2.png"),
			core::position2d<irr::s32>(10, driver->getScreenSize().Height - 128));

	// lock the logo's edges to the bottom left corner of the screen
	img->setAlignment(EGUIA_UPPERLEFT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);

	int iAnimationId = 0;
	int iCameraId = -1;
	// draw everything

	while(Device->run() && driver)
	{
		if (Device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(150,50,50,50));

			smgr->drawAll();
			env->drawAll();

			driver->endScene();

			core::stringw str(L"FPS: ");
			str.append(core::stringw(driver->getFPS()));
			str += L" Tris: ";
			str.append(core::stringw(driver->getPrimitiveCountDrawn()));
			fpstext->setText(str.c_str());
		}
		else
			Device->yield();
	}

	Device->drop();
	return 0;
}

/*
**/
