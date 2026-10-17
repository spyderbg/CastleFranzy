#ifndef __CONFIG_VERSION_H__
#define __CONFIG_VERSION_H__


////////////////////////////////////////////////////////////////////////////////////////////////////
//"FOR RELEASE VERSION PLEASE SET THE RELEASE VARIABLE TO 1 !!!"
#define RELEASE_VERSION		(1)
////////////////////////////////////////////////////////////////////////////////////////////////////

//#define JP_LANG

////////////////////////////////////////////////////////////////////////////////////////////////////

#if (RELEASE_VERSION)

	#define DEBUG_MODE					(0)
	#define DEBUG_LOG					(0)
	#define DEBUG_FACES_OUT				(0)
	#define DEBUG_DUPLICATED_FACES		(0)

	#define CHEAT_HEADSOOT				(0)
	#define CHEAT_INSTANT_KILL			(0)

	#define	CHEATS_FULL_ENABLED			(0)
	#define FAST_START					(0)

	#define DEBUG_TANKS_LIFE			(0)

	#define DEBUG_SHOW_MP_BANDWIDTH     (0)

	#define SHOW_FPS					(1)	//Shows the fps and other debug info
	#define SHOW_ADVANCED				(1) //show fog and distance options
	#define GOD_MODE					(1)

#else

	#define DEBUG_MODE					(1)
	#define DEBUG_LOG					(1)
	#define DEBUG_FACES_OUT				(0)
	#define DEBUG_DUPLICATED_FACES		(0)
	
	#define CHEAT_HEADSOOT				(0)
	#define CHEAT_INSTANT_KILL			(0)

	#define	CHEATS_FULL_ENABLED			(0)
	#define FAST_START					(1) /* windows one ;) */

	#define DEBUG_TANKS_LIFE			(0)

	#define DEBUG_SHOW_MP_BANDWIDTH     (1)

	#define SHOW_FPS					(1)	//Shows the fps and other debug info
	#define GOD_MODE					(1)

#endif

#define DEBUG DEBUG_MODE

////////////////////////////////////////////////////////////////////////////////////////////////////

#define USE_AUTO_SCALING		(0)
#define OPTIMIZE_MEMORY			(0)

#define USE_LZMA				(0)
#define USE_Z_SORT_OPT			(0)

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef AEE_SIMULATOR
	#define DEBUG_LEVEL 3
#else
	#define DEBUG_LEVEL 0
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

const int TARGET_FPS = 27;
const int FRAME_TIME = 1000 / TARGET_FPS;

////////////////////////////////////////////////////////////////////////////////////////////////////

#define USE_ALL_MID_SOUND

#define S_SND_VOLUMN   20
#define M_SND_VOLUMN   40
#define L_SND_VOLUMN   60

////////////////////////////////////////////////////////////////////////////////////////////////////
#define ENABLE_MENU_HINT

/////////////////////////////////////////////////////////////////////////////////////////////////////

#endif