 


//TEXTURE FORMAT
#define K_FORMAT_FOR_SPLASHES						K_FORMAT_RGBA8888
#define K_FORMAT_FOR_SCREENSHOTS					K_FORMAT_RGBA8888
#define K_FORMAT_FOR_LOGOS							K_FORMAT_RGBA8888
#define K_FORMAT_FOR_WINDOW							K_FORMAT_RGBA8888
#define K_FORMAT_FOR_TEXT							K_FORMAT_RGBA8888
//(DEBUG_OBJCMEM | DEBUG_CPPCMEM | DEBUG_GAME | DEBUG_LOADCONFIG | DEBUG_UPDATER | DEBUG_INTERRUPTS)
//DEBUG CONFIG
#define DEBUG_FLAGS									(DEBUG_OBJCMEM | DEBUG_CPPCMEM | DEBUG_GAME | DEBUG_LOADCONFIG | DEBUG_UPDATER | DEBUG_INTERRUPTS)
#define DEBUG_UPDATER								(1<<0)
#define DEBUG_TEXTURE								(1<<1)
#define DEBUG_LINKGRABBER							(1<<2)
#define DEBUG_LOADING								(1<<3)
#define DEBUG_GAME									(1<<4)
#define DEBUG_LOADCONFIG							(1<<5)
#define DEBUG_CPPCMEM								(1<<6)
#define DEBUG_OBJCMEM								(1<<7)
#define DEBUG_INTERRUPTS							(1<<8)

/*NEW DESIGN*/
#define K_MAIN_ACTIVITY_RECT						CGRectMake(160 - 40/2, 280 - 40/2, 40, 40)
#define SCREENSHOT_BG_RECT							CGRectMake(5, 45, 310, 210)
#define SCREENSHOT_RECT								CGRectMake(15 + (256 - 170)*0.7/2, 15, 170 * 0.7, 256 * 0.7)
#define SCREENSHOT_RECT_L							CGRectMake(12, 12, 256 * 1.1, 170 * 1.1)
#define SCREENSHOT_LOAD_RECT						CGRectMake((170 * 0.7) /2 - 16, (256 * 0.7) /2 - 16, 32, 32)
#define BUYBUTTON_RECT								CGRectMake(200, 110 - 45, 92, 42)
#define BUYBUTTON_RECT2								CGRectMake(220, 43, 92, 42)
#define INFOBUTTON_RECT								CGRectMake(22, 215 , 20, 20)
#define VIDEOBUTTON_RECT							CGRectMake(210, 240 - 45, 71, 41)
#define VIDEOBUTTON_RECT2							CGRectMake(116, 43, 92, 42)

#define K_MAX_GAME									100

#define PRIO_CA										0
#define PRIO_WN										1
#define PRIO_BS										2
#define PRIOMAX										3
#define sortCatalog									false
#define usePicker									false
/*READ REVIEWS TABLE*/
#define kCellHeight									80.0
// table view cell content offsets
#define kCellLeftOffset								7.0
#define kCellTopOffset								5.0
#define kStarBarWidth								90
#define kReviewCellLineHeight						25

//WRITE REVIEW SCREEN
#define kYTranslationWhenKeyboardOn					70
//#define kOrangeFontColorForReviews					[UIColor colorWithRed:236/255.0 green:57/255.0 blue:28/255.0 alpha:1]
#define kOrangeFontColorForReviewsR					242/255.0
#define kOrangeFontColorForReviewsG					62/255.0
#define kOrangeFontColorForReviewsB					32/255.0


//VISUAL DISPOSITION
#define K_IGP_BG_Y									(25)
#define K_IGP_DISTANCE_TO_CENTER					(120)
#define K_IGP_CURRENT_SPLASH_DY_LANDSCAPE			(-30)
#define K_IGP_SPLASH_SCALE_FACTOR					(1.0)
#define K_IGP_SPLASH_SCALE_FACTOR2					(1.3)
#define	K_IGP_SPLASH_Y								(0)
#define K_IGP_SPLASH_ANGLE_MAX						(55)
#define K_IGP_DISTANCE_BETWEEN						(50)
#define K_IGP_LOGO_SCALE							(0.75)
#define K_IGP_DISTANCE_BETWEEN_FINGER				(50)
#define K_BUTTON_EXIT_X								(-190)
#define K_BUTTON_EXIT_Y								(120)
#define K_BUTTON_BUY_X								(190)
#define K_BUTTON_BUY_Y								(120)
#define K_BUTTON_VIDEO_X							(190)
#define K_BUTTON_VIDEO_Y							(70)

//BUTTONS
#define K_BUTTON_SIZE_X								(55)
#define K_BUTTON_SIZE_Y								(41)
#define K_BUTTON_SCALE_NORMAL						(1)
#define K_BUTTON_EFFECT_SCALE						(1.2)
#define K_BUTTON_EFFECT_TIME						(0.6)
#define K_BUTTON_EFFECT_TIME2						(0.1)

//BUTTON IDs
#define K_BUTTON_NONE								(0)
#define K_BUTTON_EXIT								(1)
#define K_BUTTON_APPSTORE							(2)
#define K_BUTTON_YOUTUBE							(3)
#define K_BUTTON_COUNT								(3)

#define BUTTONARRAY							\
{											\
{K_BUTTON_EXIT_X,	K_BUTTON_EXIT_Y},		\
{K_BUTTON_BUY_X,	K_BUTTON_BUY_Y},		\
{K_BUTTON_VIDEO_X,	K_BUTTON_VIDEO_Y}		\
};

#define IGP_IMAGE(img)								(IGP::s_igpInstance->img)
//HELPERS
#define LIMIT(v, vmin, vmax)						MIN(MAX(v, vmin), vmax)
#define SIGN(x)										(((x) >= 0) ? 1 : -1)
#define EXACTSIGN(x)								(((x) > 0) ? 1 : (((x)==0)?0:-1))
#define SAFE_DELETE(p)								{ if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)						{ if(p) { delete[] (p);   (p)=NULL; } }


#define SAFE_DELETE_OBJC2(p)						\
{													\
	if (p)											\
	{												\
		int rCount = [p retainCount];				\
		for(int rc = 0; rc < rCount; rc++)			\
		{											\
			[p release];							\
		}											\
		p = nil;									\
	}												\
}

#define SAFE_DELETE_OBJC(p)							\
{													\
	if (p)											\
	{												\
		int blah = [p retainCount];					\
		[p release];								\
		if (blah == 1) p = nil;						\
	}												\
}

#define K_IMAGE_WIDTH								161.0
#define K_IMAGE_HEIGHT								255.0

//MIN CANVAS SIZE FOR TEXTURES
//WILL MAKE IT AS BIG AS NECESSARY UP FROM THAT
#define K_CANVAS_SIZE								64

#define K_ALPHA_FULL								1.0
#define K_ALPHA_NONE								0.0
#define K_LIGHT_FULL								1.0
#define K_LIGHT_NONE								0.0

//OPENGL CAMERA DEFINITION
#define K_FRUSTRUM_Z_NEAR							(410)
#define K_FRUSTRUM_Z_FAR							(620)
#define K_DISTANCE_TO_CAMARA						(100)

//DISTANCE A TOUCH CAN MOVE WITHOUT BEING CONSIDERED A MOVE
#define K_TOUCHNOTMOVED_TOLERANCE					15

//double touch settings
//#define USE_DOUBLE_TOUCH							
//#define USE_INFO_BUTTON								

//DEVICE W/H
#define K_SCREEN_W									320
#define K_SCREEN_H									480

//IGP WINDOW DIMENSIONS
#define K_WINDOW_W									290
#define K_WINDOW_H									480

//INTERVALS AND STATE DURATIONS
#define K_WINDOW_APPEAR_TIME						1.5
#define K_WINDOW_DISSAPEAR_TIME						2.0
#define K_SLIDESHOW_AUTO_INTERVAL					5.0
#define K_IGP_STATE_MAIN_CYCLING_TIME				0.6
#define K_IGP_SLIDESHOW_TIME						2.5
#define K_IGP_TRANSITION_TIME						1.2
#define K_IGP_SELECTED_TIME							0.5
#define K_BACK_FROM_SLIDESHOW_TIME					0.5

//TEXTURE PIXEL FORMATS
#define K_FORMAT_RGBA8888							0
#define K_FORMAT_RGBA4444							1
#define K_FORMAT_RGB888								2
#define K_FORMAT_RGBA5551							3

#define K_MESSAGES									2
#define K_CHEAT_DISPLAY_LINK_TIME					3
#define K_ARBITRARY_ITMS_LINK						@"itms://ax.phobos.apple.com.edgesuite.net/WebObjects/MZStore.woa/wa/viewSoftware?id=282764294&mt=8&ign-mscache=1"
#define K_CHEATMESSAGE								@"Hold the buy button for 3 seconds before releasing to check links"
#define K_CHEATTITLE								@"Cheat enabled"
#define K_MAX_SCREENSHOT							5
#define K_CHEAT_SEQUENCE							"12111221"
#define K_DEBUG_SEQUENCE							"121"
#define K_NOTRACK_PARAMETER							"&notrack=1"
#define K_CORNER1_X									50
#define K_CORNER1_Y									350
#define K_CORNER2_X									270
#define K_CORNER2_Y									350

#define K_DEFAULT_REFLECTION_FRACTION				0.65
#define K_DEFAULT_REFLECTION_OPACITY				0.40
// image reflection

//STRINGS CONSTANTS AND DEFINES

typedef unsigned char	u8;
typedef signed char		s8;
typedef unsigned short	u16;
typedef signed short	s16;
typedef unsigned int	u32;
typedef signed int		s32;

typedef signed int		fx32;
typedef	wchar_t			TChar;

#define UNICOD

#if defined ( UNICOD )
	#define ACHAR 		TChar
	#define ACHAR_SIZE  2
	#define _L(x)		L##x				
	#define CHARMASK	0xFFFF
#else
	#define ACHAR 		char
	#define ACHAR_SIZE  1
	#define _L(x)		(x)
	#define CHARMASK	0xFF
#endif

#ifdef CHECK_MEMORY_LEAKS
	#define NEW new(__FILE__, __LINE__)
	#define DELETE delete
#else
	#ifndef NEW
		#define NEW				new
	#endif

	#ifndef DELETE
		#define DELETE			delete
	#endif
#endif

#define USE_3G

//UPDATER CONFIG

//#define K_IGP_CFGURL								@"http://10.176.9.108/getAdvertisedGames.php?lang=LANG&reg=COUNTRY&game=FROM&version=VERSION"
//#define K_IGP_CFGURL								@"http://www.dreamwap.ro/test/getAdvertisedGames.php?lang=LANG&reg=COUNTRY&game=FROM&version=VERSION"
#define K_IGP_CFGURL								@"http://igpiphone.gameloft.com/getAdvertisedGames.php?lang=LANG&reg=COUNTRY&game=FROM&version=VERSION&d=DEVICE&f=FIRMWARE&udid=UDIDPHONE"
#define K_LINK_APPSTORE								@"http://ingameads.gameloft.com/redir/?from=FROM&op=IPHO&pp=1&game=GAME&ver=VERSION&lg=LANG&country=COUNTRY&d=DEVICE&f=FIRMWARE&udid=UDIDPHONE"
#define K_LINK_YOUTUBE								@"http://ingameads.gameloft.com/redir/?from=FROM&op=IPHO&pp=1&t=video&game=GAME&ver=VERSION&lg=LANG&country=COUNTRY&d=DEVICE&f=FIRMWARE&udid=UDIDPHONE"

//fixes for rare issues:

#define WRONG_SIZED_TEXTURES_ISSUE
//#define AVOID_BSWH_VIEWS_RELEASE
