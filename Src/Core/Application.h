// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Application
//
//		The main (common) class handling the application/game
//
//	Notes:
//		- Init*() should be platform-dependant, so its parameters may vary as needed
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include <irrlicht.h>

#include <list>
#include <map>
#include <new>

#include "IO/Audio/SoundManager.h"

#include "Common/StateMachine.h"
#include "Common/Languages.h"

#include "Game/Config/UserProfile.h"

///////////////////////////////////////////////////////////////////////

#define __APP()							( Application::GetInstance() )

#define __IRR_DEVICE()					( Application::GetInstance()->GetDevice() )
#define __IRR_DRIVER()					( Application::GetInstance()->GetDriver() )
#define __IRR_SCENE()					( Application::GetInstance()->GetSceneManager() )
#define __IRR_GUI()						( Application::GetInstance()->GetGUIEnv() )
#define __IRR_FILE_SYSTEM()				( Application::GetInstance()->GetFileSystem() )

#define __MGR_ACCELEROMETER()			( Application::GetInstance()->GetAccelerometer() )
#define __MGR_SOUNDS()					( CSoundManager::GetInstance() )
#define __MGR_STRINGS()					( CStringTable::GetInstance() )
#define __MGR_STATES()					( CStateMachine::GetInstance() )
#define __MGR_EVENTS()					( Application::GetInstance()->GetEventManager() )

#define __PROFILE()						( CUserProfileManager::GetInstance()->GetCreateProfile( "Player1", "user_profile" ) )
#define __PROFILE_ID(id)				( CUserProfileManager::GetInstance()->GetCreateProfile( id, "user_profile" ) )

///////////////////////////////////////////////////////////////////////

class IUpdatable;
class IDrawable;
class AccelerometerBase;
class IMediaPlayer;
//class GamePad;
class EventManager;
//class HUD;
class CLevel;

class IGPSendInfo;

///////////////////////////////////////////////////////////////////////
/// Allocators
///////////////////////////////////////////////////////////////////////

void AllocatorsEnable(bool enable);

//void* operator new(size_t size) throw (std::bad_alloc);
//void* operator new[](size_t size) throw (std::bad_alloc);
//void operator delete(void* p) throw ();
//void operator delete[](void* p) throw ();

///////////////////////////////////////////////////////////////////////
/// Application
///////////////////////////////////////////////////////////////////////

class Application : public irr::IEventReceiver
{
///////////////////////////////////////////////////////////////////////
/// Typedefs
///////////////////////////////////////////////////////////////////////

private:
	typedef std::list<IUpdatable*>			UpdatableList;
	typedef UpdatableList::iterator			UpdatableListIter;

	typedef std::list<IDrawable*>			DrawableList;
	typedef DrawableList::iterator			DrawableListIter;

	typedef std::list<IEventReceiver*>	EventReceiverList;
	typedef EventReceiverList::iterator		EventReceiverListIter;


///////////////////////////////////////////////////////////////////////
/// Initialize
///////////////////////////////////////////////////////////////////////

private:
							Application();
							Application(const Application&);	// copy-const de-activated
	Application&			operator=(const Application&);		// assignation copy de-activated
	void					Init( irr::IrrlichtDevice* device );

public:
	irr::gui::IGUIStaticText*			m_fpstext;

	virtual					~Application();

	// Singleton
	static Application*		GetInstance();

	// Called immediately after the first GetInstance() (i.e. after the Ctor)
	#ifdef OS_LINUX
 void InitLinux(irr::IrrlichtDevice* device);
#endif
	void					InitWin32( irr::IrrlichtDevice* device );
	void					InitIPhone( irr::IrrlichtDevice* device, float screenLeft, float screenTop, float screenRight, float screenBottom );

	// separate initializations
	void					InitializeAudio();
	void					InitializeTouchPad();
	void					InitializeStrings();
	void					InitializeProfile();
	void					InitializeAchievments();
	void					InitializeOrientations();
	void					InitializeOthers();


///////////////////////////////////////////////////////////////////////
/// Getters of different modules
///////////////////////////////////////////////////////////////////////

public:
	irr::IrrlichtDevice*		GetDevice()			{ return m_irrDevice; }
	irr::scene::ISceneManager*	GetSceneManager()	{ return m_irrSceneManager; }
	irr::gui::IGUIEnvironment*	GetGUIEnv()			{ return m_irrGUIEnv; }
	irr::video::IVideoDriver*	GetDriver()			{ return m_irrVideoDriver; }
	irr::io::IFileSystem*		GetFileSystem()		{ return m_irrFileSystem; }

	EventManager*				GetEventManager()	{return m_eventManager;};
	AccelerometerBase*			GetAccelerometer()	{return m_accelerometer;};
//	IMediaPlayer*				GetMediaPlayer()	{return m_mediaPlayer;};

private:
	irr::IrrlichtDevice*		m_irrDevice;
	irr::scene::ISceneManager*	m_irrSceneManager;
	irr::gui::IGUIEnvironment*	m_irrGUIEnv;
	irr::video::IVideoDriver*	m_irrVideoDriver;
	irr::io::IFileSystem*		m_irrFileSystem;

	EventManager*				m_eventManager;
	AccelerometerBase*			m_accelerometer;
//	IMediaPlayer*				m_mediaPlayer;


///////////////////////////////////////////////////////////////////////
/// Registers related
///////////////////////////////////////////////////////////////////////

public:
	void	RegisterForUpdate(IUpdatable* cls);
	void	UnRegisterForUpdate(IUpdatable* cls);

	void	RegisterForDrawingInScene(IDrawable* cls);
	void	UnRegisterForDrawingInScene(IDrawable* cls);

	void	RegisterForDrawingOnHUD(IDrawable* cls);
	void	UnRegisterForDrawingOnHUD(IDrawable* cls);

	void	RegisterForIrrlichtEvents(IEventReceiver* cls);
	void	UnRegisterForIrrlichtEvents(IEventReceiver* cls);


///////////////////////////////////////////////////////////////////////
/// File system paths
///////////////////////////////////////////////////////////////////////

private:
	static const int		PATH_LEN = 255;

	char					m_rootPath[PATH_LEN];
	char					m_resourcesPath[PATH_LEN];
	char					m_savefilesPath[PATH_LEN];

public:
	void					setFilePaths( const char* root, const char* resources, const char* save_file );

	const char*				getFileRootPath()	const;
	const char*				getFileResPath()	const;
	const char*				getFileSavePath()	const;


///////////////////////////////////////////////////////////////////////
/// Audio related
///////////////////////////////////////////////////////////////////////

public:
	void		Audio_RegisterSounds();


///////////////////////////////////////////////////////////////////////
/// Language
///////////////////////////////////////////////////////////////////////

public:
	ELanguages	GetDeviceLanguage();


///////////////////////////////////////////////////////////////////////
/// Update, draw, interrupts
///////////////////////////////////////////////////////////////////////

public:
	void				Draw();
	void				Update();
	void				Quit();

	inline bool			isPaused()					{ return m_paused; }
	void				Pause();
	void				Resume();

	inline int			GetTargetFPS()	const		{ return m_targetFPS; }
	inline void			SetTargetFPS(int fps)		{ m_targetFPS = fps; }
	float				ScaleByFPS (float x) const	{ return x / float(m_targetFPS); }
	
	bool				OnEvent(const irr::SEvent& ev);	// IEventReceiver impl.

private:	
	void				_Update(int timestamp);

private:
	bool				m_paused;

	UpdatableList		m_updatableList;
	DrawableList		m_drawableSceneList;
	DrawableList		m_drawableHUDList;
	EventReceiverList	m_eventReceivers;

	int					m_targetFPS;
	long				m_lastTicks;


///////////////////////////////////////////////////////////////////////
/// IGP, Gameloft Live, Twitter
///////////////////////////////////////////////////////////////////////

public:
	bool	UpdateIGP();
	bool	UpdateGLLive();

	void	GoToIGP();
	void	PerformLoadIGP();
	bool	IsInIGP();

	void	GoToGLLive();
	void	PerformLoadGLLive();
	bool	IsInGLLive();

	void	GoToTwitter();
	bool	IsInTwitter();

public:
	static IGPSendInfo* s_igpInfo;


///////////////////////////////////////////////////////////////////////
/// orientations 
///////////////////////////////////////////////////////////////////////
public:
	static bool isDeviceIPod;
	static bool isDeviceSetted;
	bool		m_deviceOrientationLocked;	
private:
	void		UpdateOrientationChanges();
	void		NativeStatusBar_UpdateOrientation();
	bool		m_profileReadOrientations;


///////////////////////////////////////////////////////////////////////
/// Status bar 
///////////////////////////////////////////////////////////////////////
public:
	void ShowStatusBar( bool visible );


///////////////////////////////////////////////////////////////////////
/// Sepcials!!!
///////////////////////////////////////////////////////////////////////
/*
private:
	typedef std::map<void*, const char*>	MonitoredPointers;
	MonitoredPointers		__monitoredPointers;

public:
	void					startMonitoring(void* ptr, const char* className);
	void					stopMonitoring(void* ptr);
	void					dumpMonitoredPointers();
*/
};

