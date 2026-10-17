#ifndef __DEF_H__
#define __DEF_H__

/*
 * function macros
 */

//#define SHOW_TOUCH_RECT
#define USE_NEW_STYLE_INTERFACE

/*
 * const value macros
 */
#define LOGIN_CHAT_TIMEOUT      (40000)
#define NUMBER_OF_AVATAR		32
#define CUSTOMIZE_AVATAR_ID		(NUMBER_OF_AVATAR + 1)
#define GAME_INVITAON_STR       "-gameinv-"
#define GLLIVE_SAVE_FOLDER		"GLLive_Data"

/*
 * config path
 */

#define CONFIG_AVATAR_PATH      "avatar"

/*
 * math macros
 */

#ifndef ABS
#define ABS(a)   ((a) > 0 ? (a) : (-1 * (a)))
#endif /* ABS */
#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif /* MIN */
#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif	/* MAX */


/*
 * The menu item ID define
 */

//Below command bar
#define IDC_BACK                     0x0000
#define IDC_GO                       0x0001

//Login menu
#define IDC_LOGIN                    0x0100
#define IDC_FORGET                   0x0101
#define IDC_REGISTER                 0x0102
#define IDE_LOGIN_USERNAME           0x0103
#define IDE_LOGIN_PASSWORD           0x0104
#define IDC_REMEMBER_ME              0x0105

//Register menu
#define IDC_CREATE                   0x0200
#define IDE_REG_USERNAME             0x0201
#define IDE_REG_EMAIL                0x0202
#define IDE_REG_PASSWORD             0x0203
#define IDE_REG_CONFIRM              0x0204
#define IDC_REG_EMAIL_FLAG           0x0205
#define IDC_REG_ACCEPT               0x0206
#define IDC_REG_CONTINUE             0x0207

//Forgot menu
#define IDE_FORGOT_USERNAME          0x0300

//Main menu
#define IDC_TROPHY                   0x0400
#define IDC_CHAT_ROOM                   0x0401
#define IDC_MESSAGE                  0x0402
#define IDC_LOGOUT                   0x0403
#define IDC_PROFILE                      0x0404
#define IDC_FRIEND                  0x0405
#define IDC_LEADER_BOARD             0x0406

//Friends list
#define IDC_ADD                      0x0500

//Friend detail
#define IDC_DEL                      0x0600
#define IDC_INVITE_PLAY              0x0601
#define IDC_INVITE_CHAT              0x0602
#define IDC_SEND_MSG                 0x0603
#define IDC_COMPARE                  0x0604
#define IDC_QUICK_PLAY               0x0605
#define IDC_CUSTOM_PLAY              0x0606

#define IDC_MSG_YES                  0x0700
#define IDC_MSG_NO                   0x0701
#define IDC_MSG_OK                   0x0702

//Message list
#define IDC_MSG_NEW                  0x0800
#define IDC_MSG_DEL                  0x0801
#define IDC_MSG_READ                 0x0802
#define IDC_MSG_DELALL               0x0803
#define IDC_MSG_REFRESH              0x0804
#define IDC_MSG_SEND                 0x0805

//Read message
#define IDC_MSGREAD_REPLY            0x0900
#define IDC_MSGREAD_DEL              0x0901
#define IDC_MSGREAD_ADD2LIST         0x0902

//Profile
#define IDC_PROFILE_SEL_AVATAR       0x0a00
#define IDC_PROFILE_SEL_COUNTRY      0x0a01
#define IDE_PROFILE_USERNAME         0x0a02
#define IDE_PROFILE_EMAIL            0x0a03
#define IDE_PROFILE_PASSWORD         0x0a04
#define IDC_PROFILE_EMAIL_FLAG       0x0a05
//#define IDC_PROFILE_REMEMBER         0x0a06
#define IDE_PROFILE_CUR_PWD          0x0a07
#define IDE_PROFILE_NEW_PWD          0x0a08
#define IDE_PROFILE_CONFIRM_PWD      0x0a09
#define IDC_PROFILE_CHANGE           0x0a0a
#define IDC_PROFILE_CHANGE_PWD       0x0a0b

//Write message
#define IDC_MSGWRITE_FRIENDLIST      0x0b01

//Chat
#define IDC_CHAT_SEND                0x0c00
#define IDC_CHAT_BACK                0x0c01
#define IDC_CHAT_COLOR               0x0c02
#define IDC_CHAT_ADD                 0x0c03
#define IDC_CHAT_QUIT_ROOM           0x0c04
#define IDC_CHAT_NEW_ROOM            0x0c05
#define IDE_CHAT_INPUT               0x0c06


/*
 * Error code
 */
#define ERROR_WRONG_EMAIL             55
#define ERROR_DATA_SIZE_INVALID       65
#define ERROR_FRIENDS_TOO_MANY        93
#define ERROR_EMAIL_USED              97
#define ERROR_PASSWORD_CONFIRM_FAILED 1000
#define ERROR_NO_CURRENT_PWD          1001
#define ERROR_NO_NEW_PWD              1002
#define ERROR_WRONG_INPUT             1003
#define ERROR_REG_NO_PASSWORD         1004


/**
 * The const definition.
 * <p>
 * It is only designed for game loop.
 * <p>
 * If you are focus on XPlayer, please omit it.
 * <p>
 *
 * @version 0.0.1
 *
 */
class DEF
{

public:

	/**
	 * Game state.
	 *
	 */
	enum GAME_STATE_ENUM
	{	
		GAME_STATE_NULL,
		GAME_STATE_PROFILE,
		GAME_STATE_FRIENDS,
		//GAME_STATE_MULTIPLAYER,
		//GAME_STATE_LOCAL,
		//GAME_STATE_TEST,
		GAME_STATE_MENU,
		GAME_STATE_LOGIN,
		GAME_STATE_ONLINE,
		GAME_STATE_CHAT,
		GAME_STATE_EDIT,
		GAME_STATE_MESSAGE,
		GAME_STATE_TROPHY,
		GAME_STATE_CHAT_INVITATION,
		GAME_STATE_DISCONNECT,
		GAME_STATE_EXIT
	};

	/**
	 * Screen parameters.
	 *
	 */
	//const static int SCREEN_WIDTH = 240;
	//const static int SCREEN_HEIGHT = 320;

	/**
	 * Frame restrict.
	 *
	 */
	const static int FRAMES_PER_SECOND = 24;

	/**
	 * Parameters used for SDL.
	 *
	 */
	const static int SCREEN_BPP = 32;
	const static int START_POS_X = 20;
	const static int START_POS_Y = 20;
	const static int FONT_SIZE	= 20;
	//const static int LINE_HEIGHT = 16;

	const static int TEXT_WIDHT = 420;

	//const static int BUTTON_UP_X = TEXT_WIDHT;
	//const static int BUTTON_UP_Y = 60;
	//const static int BUTTON_UP_H = 40;
	//const static int BUTTON_UP_W = 50;

	//const static int BUTTON_DOWN_X = TEXT_WIDHT;
	//const static int BUTTON_DOWN_Y = 140;
	//const static int BUTTON_DOWN_H = 40;
	//const static int BUTTON_DOWN_W = 50;

	//const static int BUTTON_LEFT_X = 0;
	//const static int BUTTON_LEFT_Y = 280;
	//const static int BUTTON_LEFT_W = 72;
	//const static int BUTTON_LEFT_H = 40;

	//const static int BUTTON_RIGHT_X = 425;
	//const static int BUTTON_RIGHT_Y = 270;
	//const static int BUTTON_RIGHT_W = 50;
	//const static int BUTTON_RIGHT_H = 40;
	//
	//const static int BUTTON_MID_X = 60;
	//const static int BUTTON_MID_Y = 270;
	//const static int BUTTON_MID_W = 50;
	//const static int BUTTON_MID_H = 40;
};

#endif // __DEF_H__