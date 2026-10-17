#ifndef __DEF_H__
#define __DEF_H__

/**
 * Define for using UnitTest or not.	
 */
//#define ENABLE_UNIT_TEST

/**
 * Define for using MMO.	
 */
//#define MMO_CHAT 

#define MENU_INSTANT_PLAY 			"Instant Play"
#define MENU_MULTI_PLAYER 			"Mutiplayer"
#define MENU_GAMELOFT_LIVE			"Gameloft Live"
#define MENU_OPTION					"Option"
#define MENU_EXIT					"ExitGame"
#define MENU_MMO                    "MMO"

#ifdef ENABLE_UNIT_TEST
	#define MENU_UNIT_TEST						"Online Test"
	#define MENU_UNIT_TEST_GLXPLAYERLOGIN		"Test GLXPlayerLogin"
	#define MENU_UNIT_TEST_GLXPLAYERUSER		"Test GLXPlayerUser"
	#define MENU_UNIT_TEST_GLXPLAYERUSERFRIEND	"Test GLXPlayerUserFriend"
	#define MENU_UNIT_TEST_GLXPLAYERLEADERBOARD	"Test GLXPlayerLeaderboard"
	#define MENU_UNIT_TEST_GLXPLAYERMESSAGE		"Test GLXPlayerMessage"
	#define MENU_UNIT_TEST_GLXPLAYERCHAT		"Test GLXPlayerChat"
	#define MENU_UNIT_TEST_YOUTUBE				"Test GLXPlayerYoutube"
	#define MENU_UNIT_TEST_LOBBY				"Test GLXPlayerLobby"	
	
	#define UNIT_TEST_OUTPUT_NAME	"UnitTestOutput.txt"
#endif //ENABLE_UNIT_TEST

#define MENU_GL_USER_PROFILE		"User Profile"
#define MENU_GL_USER_FRIEND			"User Friends"
#define MENU_GL_MESSAGE				"Message"
#define MENU_GL_CHAT				"Chat"
#define MENU_GL_NEW_GAME			"New Game"
#define MENU_GL_UPLOAD_VIDEO        "Upload Video"
#define MENU_GL_DOWNLOAD            "Download File"

#define MENU_GL_USER_PF_NAME		"Change User Name"
#define MENU_GL_USER_PF_PASS		"Change Password"
#define MENU_GL_USER_PF_EMAIL		"Change Email"
#define MENU_GL_USER_PF_EMAIL_FLAG	"Change Email Flag"
#define MENU_GL_USER_PF_COUNTRY		"Change Country"
#define MENU_GL_USER_PF_AVATAR		"Change Avatar"

#define MENU_GL_USER_ADD_FRIEND		"Add Friends"
#define MENU_GL_USER_LIST_FRIENDS	"Friends List"

#define MENU_GL_USER_INVITE_PLAY 	"Invite to play"
#define MENU_GL_USER_INVITE_CHAT 	"Invite to chat"
#define MENU_GL_USER_SEND_MSG 		"Send message"
#define MENU_GL_USER_COMPARE_GAME 	"Compare game"
#define MENU_GL_USER_MORE_INFO 		"More info"

#define MENU_MP_LOCAL				"Local"
#define MENU_MP_ONLINE				"Online"

#define MENU_MP_ONLINE_JOIN			"Join Game"
#define MENU_MP_ONLINE_CREATE		"Create Game"
#define MENU_MP_ONLINE_HISCORE		"Hiscore"

#define MENU_MP_LOCAL_BLUETOOTH		"Bluetooth"
#define MENU_MP_LOCAL_WIFI			"Wifi"

#define MENU_RANK					"Ranking"
#define MENU_TOP_RANK				"Top Ranking"
#define MENU_RANK_AROUND			"Ranking Around"
#define MENU_TOP_FRIEND_RANK		"Top Friend Ranking"

#define MENU_USER_STATE				"User State"
#define MENU_USER_REPUTATION		"User Reputation"
#define MENU_USER_DATA				"Data Center"
#define MENU_USER_DATA_VIEW			"Stored Data"
#define MENU_USER_DATA_UPLOAD		"Upload Data"

#define MENU_CHAT_WITH_FRIEND		"Chat with my friend"
#define MENU_CHAT_IN_CONFERENCE		"Chat in conference"

#define MENU_MESSAGE_WRITE			"Write Message"
#define MENU_MESSAGE_INBOX			"Inbox"

//lobby
#define MENU_LOBBY_LOGIN			"Lobby Login"
#define MENU_LOBBY_MENU_JOIN		"Join Lobby"
#define MENU_LOBBY_MENU_CREATE		"Create Lobby"
#define MENU_LOBBY_MENU_GET_LIST	"Get Lobby List"
#define MENU_LOBBY_THREAD_SAVE_LOG  "Save Log and Calculator"
#define MENU_LOBBY_MENU_AUTOMATCH           "AutoMatch"
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
		GAME_STATE_LOGO,
		GAME_STATE_MENU,
		GAME_STATE_ONLINE,
		GAME_STATE_TEST_LOBBY,
		GAME_STATE_MULTIPLAYER,
		GAME_STATE_LOCAL,
	#ifdef ENABLE_UNIT_TEST
		GAME_STATE_TEST,
	#endif //ENABLE_UNIT_TEST
		GAME_STATE_CHAT,
		GAME_STATE_LOBBYLOGIN,
		GAME_STATE_LOBBYROOM,
		GAME_STATE_EXIT,
		GAME_STATE_MMO,
		GAME_STATE_GET_SERVER_CONFIG,
	};	

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
	const static int LINE_HEIGHT = 16;

	const static int TEXT_WIDHT = 420;

	const static int BUTTON_UP_X = TEXT_WIDHT;
	const static int BUTTON_UP_Y = 60;
	const static int BUTTON_UP_H = 40;
	const static int BUTTON_UP_W = 50;

	const static int BUTTON_DOWN_X = TEXT_WIDHT;
	const static int BUTTON_DOWN_Y = 140;
	const static int BUTTON_DOWN_H = 40;
	const static int BUTTON_DOWN_W = 50;

	const static int BUTTON_LEFT_X = 5;
	const static int BUTTON_LEFT_Y = 270;
	const static int BUTTON_LEFT_W = 50;
	const static int BUTTON_LEFT_H = 40;

	const static int BUTTON_RIGHT_X = 115;
	const static int BUTTON_RIGHT_Y = 270;
	const static int BUTTON_RIGHT_W = 50;
	const static int BUTTON_RIGHT_H = 40;
	
	const static int BUTTON_MID_X = 60;
	const static int BUTTON_MID_Y = 270;
	const static int BUTTON_MID_W = 50;
	const static int BUTTON_MID_H = 40;
};

#endif // __DEF_H__