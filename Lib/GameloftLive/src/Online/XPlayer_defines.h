/**
 * The const defination for XPlayer.
 *
 *
 * @version 0.0.1
 *
 */

#if WIN32
#define OS_WIN32
#else
#define OS_IPHONE
#endif

#ifndef __XPLAYER_DEFINES__H__
#define __XPLAYER_DEFINES__H__

#define	XPLAYER_MAX_USERNAME_LENGTH			15
#define	XPLAYER_MAX_PASSWORD_LENGTH			15
#define	XPLAYER_MAX_EMAIL_LENGTH			127
#define	XPLAYER_MAX_UID_LENGTH				128

#define	UTF8_XPLAYER_MAX_USERNAME_LENGTH	(3 * XPLAYER_MAX_USERNAME_LENGTH)
#define	UTF8_XPLAYER_MAX_PASSWORD_LENGTH	(3 * XPLAYER_MAX_PASSWORD_LENGTH)
#define	UTF8_XPLAYER_MAX_EMAIL_LENGTH		(3 * XPLAYER_MAX_EMAIL_LENGTH)

/**
 * XPlayer Configuration
 *
 */

#define XPLAYER_FILE_CONF			"oconf.bar"
#define PROFILE_FILE_NAME 			"profile.sav"
#define PROFILE_FILE_HEAD           "_GLLive_Profile_Head_V_0.0.0.1"
#define XPLAYER_CONF_GGI			"GGI"
#define XPLAYER_CONF_URL			"XPlayerURL"
#define XPLAYER_CONF_DWURL			"XPlayDWURL"
#define XPLAYER_CONF_MPURL			"XPlayMPURL"
#define XPLAYER_CONF_PHPVER			"XPPHPVerNo"
#define XPLAYER_CONF_UPNP_PORT		"UPNPPort"
#define XPLAYER_CONF_UPNP_PORT_TRY	"UPNPPortTry"
#define XPLAYER_CONF_BROADCAST_PORT "BroadcastPort"
#define XPLAYER_CONF_CHAT_SERVER		"ChatServer"
#define XPLAYER_CONF_CHAT_DOMAIN		"ChatDomian"
#define XPLAYER_CONF_CHAT_ROOM_DOMIAN	"ChatRoomDomian"
#define XPLAYER_CONF_YOUTUBE_URL		"XPlayerYoutubeURL"
#define XPLAYER_CONF_YOUTUBE_VER		"XPPHPYoutubeVerNo"
#define XPLAYER_CONF_MMO_URL		    "XPlayerMMOURL"
#define XPLAYER_CONF_GAME_VER 			"GameVer"



/**
 * Enable flag.
 * <p> It is used to enable or disable online functions.
 * <p> Remove it if you want to disable online.
 *
 */
#define ENABLE_ONLINE

/**
* Enables XP_DEBUG_OUT print statements.
* <p> Remove it to disable the log output.
*
*/
#define ENABLE_DEBUG


// Multiplayer (TCP/IP) timeout.
//#define MP_ENABLE_TIMEOUT

/**
* Enables or disables the HTTP outside ("software") timeout mechanism.
* Use this only on phones with timeout problems (Samsung phones).
* On most phones the connection times out if an answer is not received within
* a certain time of request start. However, on some phones, the connection
* thread hangs indefinetly, so we must implement an outside timeout mechanism.
* If uncommented the timeout mechanism is enabled and the server requests
* timeout after CONN_TIMEOUT miliseconds.
*/
#define ENABLE_TIMEOUT

/**
* The time in miliseconds after which a HTTP server request is timed out if a
* response is not received. This is used only if ENABLE_TIMEOUT is set to 1.
*/
#ifdef ENABLE_TIMEOUT
#define CONN_TIMEOUT          18000 // Timeout after 18 seconds of network
                                    // inactivity as required by Verizon.
#endif

#define   NONBLOCKING_CONNECT_TIMEOUT   30000 //for nonblocking connect timeout

#define   GETHOSTBYNAME_TIMEOUT         10000 //for gethostbyname() time out

/**
* Use user number or user name
*/
//#define USE_USER_NUMBER

/**
* Enables code required for Brew version 2.1 or greater.
*/
//#define BREW_VERSION_2_1_OR_GREATER

/**
* Enables support for M7 operators Game Lobby- AMP'd.
* <p>
* Define this only for games indended for these operators. 
* <p>
* Offers access to functions for rating a game, recommending a game and getting
* user states from the M7 Server
*/
#define ENABLE_M7_SUPPORT

#define ENABLE_MULTIPLAYER

#define MULTIPLAYER_BY_SERVER     0
#define MULTIPLAYER_P2P           1
#define MULTIPLAYER_TYPE          MULTIPLAYER_BY_SERVER
/**
* Enables support for multiplayer games using TCP connections.
* <p>
* Define this only for games having multiplayer features. 
*/

#ifdef ENABLE_MULTIPLAYER

// Multiplayer (TCP/IP) timeout value.
#ifdef MP_ENABLE_TIMEOUT
#define MP_CONN_TIMEOUT       18000 // Timeout after 18 seconds of network
                                    // inactivity as required by Verizon.
#endif

/**
* When one TCP connection is idle for too long, the server disconnects it. Use
* this timer to send keep alive packets when no other transmission being
* performed for a long time and the connection needs to be open.
* <p>
* Use this only in conjunction with ENABLE_MULTIPLAYER
*/
#define KEEP_ALIVE_TIMEOUT 10000

#define KEEP_ALIVE_WITH_WEBSERVER_INTERVAL   50000//90000 because the time out is 120000
#define KEEP_ALIVE_WITH_CHATSERVER_INTERVAL  (3*60*1000) //3minutes

#define BROADCAST_TIME_INTERVAL             500
#define BROADCAST_TIME_REMOVE				3000

#define TEST_TIMEOUT                        2000

/**
 * In some multi-player on-line games, peer-to-peer is needed for 
 * the instant communication. So it is used to enable P2P communication.
 * <p>
 * In addition, it will descrease the load of game server.
 * <p>
 * Use this only in conjunction with ENABLE_MULTIPLAYER
 * <p>
 * If P2P is not necessary, please remove the definition of it. 
 *
 */
#define MP_ENABLE_P2P

// Only for debug
//#define MP_P2P_DEBUG_DO_NOT_CHECK_ALIVE
//#define MP_P2P_DEBUG_DO_NOT_KEEP_ALIVE_WITH_WEB_SERVER

#define MP_KEEP_ALIVE_TIMEOUT 15000
#define MP_KEEP_ALIVE_SENDTIME 5000

#define MP_P2P_MAX_OPPONENT_COUNT			4	

#define MP_P2P_WITH_UDP
//#define MP_P2P_WITH_TCP

#define MP_P2P_HEAD_FIELD_COUNT				7

/**
* Enables 2 different connections for sending and receiving data using TCP. Some
* phones don't support reading and writing using the same connection 
* - ex. : Motorola phones.
* <p>
* Use this only in conjunction with ENABLE_MULTIPLAYER
*/
//#define ENABLE_DUAL_TCP

#endif//ENABLE_MULTIPLAYER


/**
* If present, download feature is available (download XP_API_NEW items,
* get download list, get downlad preview )
*/
// #define ENABLE_DOWNLOAD // [3/12/2008 ROvidenie]


/**
******************************************************************************
******************************************************************************
**                     END XPlayer Configuration                       **
******************************************************************************
******************************************************************************
*/

#define ONLINE_MAX_PACKET_SIZE				1024
#define SCORE_POLICY_SINGLE_SCORE                     0
#define SCORE_POLICY_SINGLE_SCORE_SUPPLEMENTAL_DATA   1
#define SCORE_POLICY_MULTIPLE_SCORE                   2
#define SCORE_POLICY_MULTIPLE_SCORE_SUPPLEMENTAL_DATA 3

#define NB_SUPP_DATA_FIELDS 0

// Score types:
/**
* Used to define scores that refer to points.
*/
#define SCORE_TYPE_POINTS 1

/**
* Used to define scores that are time-related.
*/
#define SCORE_TYPE_TIME   2


/**
******************************************************************************
******************************************************************************
**                     XPlayer Error Codes                              **
******************************************************************************
******************************************************************************
*/

#define	ERROR_NO_RESPONSE						27
#define ERROR_NO_USERNAME						46
#define ERROR_UUID_NOTEXISTS					58

/**
* Initialisation value.
* <p>
* Getting this error means that actually no requests have been yet made since
* the MIDlet started so there is no last error.
*/
#define	ERROR_INIT                  (-100)

//for network test error
#define	ERROR_TEST_TCP_CANNOT_BE_CONNECTED                  (-5)
#define	ERROR_TEST_UDP_CANNOT_BE_CONNECTED                  (-4)
/**
* there was an error writing to file ..
*/
#define ERROR_FILE_WRITE                  -3

/**
* No connection to server.
* <p>
* Either there is no network available or the server cannot be contacted for
* some reason.
*/
#define ERROR_CONNECTION                  -2

/**
* Result is pending.&nbsp;Transaction with server is ongoing.
*/
#define ERROR_PENDING                     -1

/**
* The request finished succesfully.
*/
#define ERROR_NONE                         0

/**
* The uuid is missing from the request
*/
#define ERROR_NO_UUID                      1

/**
* The nickname is missing from the registration request
*/
#define ERROR_NO_NICKNAME                  2

/**
* The registration process failed
*/
#define ERROR_REGISTER_FAILED              3

/**
* This user is not registered
*/
#define ERROR_NOT_REGISTERED               4

/**
* This nickname is taken, another one is suggested
*/
#define ERROR_NICK_TAKEN                   5

/**
* The score was not posted for some reason
*/
#define ERROR_SCORE_UPLOAD_FAILED         21

/**
* The get rankings operation failed for some reason
*/
#define ERROR_GET_RANKINGS_FAILED         22

#ifdef ENABLE_M7_SUPPORT
/**
* Failed to rate this game ...
*/
#define ERROR_RATE_GAME_FAILED            23

/**
* Failed to recommend this game ..
*/
#define ERROR_RECOMMEND_GAME_FAILED       24

/**
* There is no phone number in the request
*/
#define ERROR_NO_PHONE_NUMBER             25

/**
* Programming error on emulator: no phone id
*/
#define ERROR_NO_CLIENT_ID                26

/**
* Failed to get the states for the player
*/
#define ERROR_GET_STATS_FAILED            29
#endif //ENABLE_M7_SUPPORT

/**
* Programming error : the supplemental data needed to upload this leaderboad was
* not supplied properly
*/
#define ERROR_SUPPLEMENTAL_DATA_NEEDED    31

/**
* The server response has incorrect format.
* <p>
* Make sure the request is correct. If it is a leaderboard request, make sure
* the number of supplemental data needed match the requested leaderboard
*/
#define ERROR_BAD_RESPONSE                40

/**
* Error - Time out expired
*/
#ifdef ENABLE_TIMEOUT
#define ERROR_REQUEST_TIME_OUT            41
#endif

#define ERROR_BUDDY_BAD_USERNAME		  42
#define ERROR_BUDDY_YOUR_USERNAME		  43


#define ERROR_NO_USERNAME						46
#define ERROR_NO_PASSWORD						47
#define ERROR_USERNAME_TAKEN					48
#define ERROR_INVALID_USERNAMEORPASSWORD		49
#define ERROR_NO_EMAIL							54
#define ERROR_USERNAME_NOTEXISTS				67
#define ERROR_REPUTATION_REPEAT					69
#define ERROR_BAD_USER_NAME						70

#ifdef ENABLE_MULTIPLAYER
/**
* Multiplayer error - the call has no relevance in this state.
* <p>
* The game and XPlayer think they're in different states.
*/
#define ERROR_WRONGFULL_QSTATE            50
/**
* Multiplayer error - this response is not expected.
* <p>
* The game and XPlayer think they're in different states.
*/
#define ERROR_WRONGFULL_RSTATE            51

/**
* Multiplayer error - there was an error starting the game.
*/
#define ERROR_START_GAME                  52
/**
* Multiplayer error - there was an error joining the game.
*/
#define ERROR_JOIN_GAME                   53
/**
* Multiplayer error - there was an error processing the request.
*/
#define ERROR_REQUEST_FAILED              54

//--------------keep alive to php server--------
#define ERROR_UID_NOT_EXIST                58

#define ERROR_IN_EXECUTE                   59

#define ERROR_USER_HAS_LOGIN               60

#define ERROR_UNKONWN                      61
//--------------keep alive to php server--------
#define ERROR_HAS_NOT_LOGIN                62

#define ERROR_FRIEND_NOT_EXISTS            63

#define ERROR_DATA_NOT_STORE               64

#define ERROR_MP_UPNP_DISABLE				501

#define ERROR_MP_UPNP_GET_EXTERNAL_IP		502

#define ERROR_MP_UPNP_PORT_MAPPING			503

#define ERROR_MP_JOIN_ROOM_FULL				504

#define ERROR_MP_JOIN_ROOM_PASSWORD			505

#endif //ENABLE_MULTIPLAYER

// for download .. 
#ifdef ENABLE_DOWNLOAD
/**
* the file that was requsted was not found on the server .. 
*/
#define ERROR_DOWNLOAD_FILE_NOT_FOUND     70

/**
* the list files request cannot find any file that matches the criteria
*/
#define ERROR_DOWNLOAD_LIST_NO_FILES      71
/**
* the download (either list, preview or file failed for some reason) ..
*/
#define ERROR_DOWNLOAD_FAILED             72

/**
* the download was successfull .. - server side error code
*/
#define ERROR_DOWNLOAD_OPERATION_SUCCESS   3

/**
* the download failed .. - server side error code
*/
#define ERROR_DOWNLOAD_OPERATION_FAIL      1

#endif //ENABLE_DOWNLOAD

/**
******************************************************************************
******************************************************************************
**                     END XPlayer Error Codes                          **
******************************************************************************
******************************************************************************
*/

////////////////////////////////////////////////
// The server functions for HTTP communication:
#define		FUNCTION_GET_WEBCONFIG						1
#define		FUNCTION_REGISTER							11
#define		FUNCTION_LOGIN								15
#define		FUNCTION_UPLOAD_SCORE						0
#define		FUNCTION_GET_RANKINGS						12
#define		FUNCTION_GET_RANKINGS_AROUND_PLAYER			13

#define		FUNCTION_GET_RANKINGS_FRIENDS			    8888 //temp

#define		FUNCTION_CHANGE_USERNAME					14
#define		FUNCTION_CHANGE_PASSWORD					50
#define		FUNCTION_CHANGE_EMAIL						51
#define		FUNCTION_RETRIEVAL_PASSWORD					52
#define		FUNCTION_GET_COUNTRY						53
#define		FUNCTION_CHANGE_COUNTRY						54
#define		FUNCTION_GET_USED_APPS 						57

#define		FUNCTION_GET_USER_UID 						58
#define		FUNCTION_ADD_USER_FRIEND 					59
#define		FUNCTION_DEL_USER_FRIEND 					60
#define		FUNCTION_GET_USER_FRIENDS 					61
#define		FUNCTION_CONFIRM_USER_FRIEND 				62

#define		FUNCTION_GET_USER_GAMELIST 					63
#define		FUNCTION_GET_USER_STATE 					64
#define		FUNCTION_GET_USER_STORE_DATA 				65
#define		FUNCTION_SET_USER_STORE_DATA 				66
#define		FUNCTION_GET_REPUTATION 					67
#define		FUNCTION_POINT_USER_REPUTATION 				68

#define		FUNCTION_GET_USER_BEST_SCORE 				69
#define		FUNCTION_GET_USER_AVATAR 					70
#define		FUNCTION_UPLOAD_USER_AVATAR 				71
#define		FUNCTION_DEL_USER_STORE_DATA 				72

#define		FUNCTION_GET_USER_FRIENDS_COUNT 			73

#define		FUNCTION_GET_MESSAGE_COUNT 					74
#define		FUNCTION_GET_MESSAGE_HEADER 				75
#define		FUNCTION_READ_MESSAGE 						76
#define		FUNCTION_SEND_MESSAGE 						77
#define		FUNCTION_DEL_MESSAGE 						78

#define		FUNCTION_GET_USER_TROPHY 					79
#define		FUNCTION_SET_USER_TROPHY 					80
#define		FUNCTION_GET_USER_INFO 						81
#define		FUNCTION_UPDATE_USER_INFO 					82

#define		FUNCTION_APPLE_PUSH_NOTIFICATION_SERVER		83
#define		FUNCTION_GET_TROPHY_ICON					84
#define		FUNCTION_AWARD_TROPHY						85
#define		FUNCTION_GET_GAME_TROPHYLIST				86
#define		FUNCTION_GET_USER_GAME_TROPHYLIST			87

#define		FUNCTION_GET_ADV_RSS						88
#define		FUNCTION_GET_ADV_ATTACHMENT					89
#define		FUNCTION_GET_GAME_ICON						90
#define		FUNCTION_GET_MY_SENT_MESSAGE_COUNT			91
#define		FUNCTION_GET_MY_SENT_MESSAGE_HEADER			92
#define     FUNCTION_READ_MY_SENT_MESSAGE               93
#define     FUNCTION_DEL_MY_SENT_MESSAGE                94

#define     FUNCTION_GET_GENERAL_CONDITIONS             98
#define		FUNCTION_GET_CHAT_WARNING					99
#define     FUNCTION_DOWNLOAD_USER_AVATAR               101
#define     FUNCTION_DEL_USER_AVATAR                    102
#define     FUNCTION_GET_USER_AVATAR_LIST               103

#define     FUNCTION_KEEP_ALIVE                         16
#define		FUNCTION_LOGOUT								17
#define		FUNCTION_AUTH_CHARGE						42

#define     FUNCTON_NETWORK_TEST                        -1

#define		FUNCTION_ADD_BUDDY							30
#define		FUNCTION_REMOVE_BUDDY						31
#define		FUNCTION_GET_BUDDIES_LIST					32

#ifdef ENABLE_M7_SUPPORT
// Motricity (M7 Networks) specific functionality.
#define FUNCTION_RATE_GAME								8
#define FUNCTION_RECOMMEND_GAME_TO_BUDDIES              9
#define FUNCTION_GET_STATS								10
#endif


#ifdef ENABLE_DOWNLOAD
// used for download functionality.
#define FUNCTION_DOWNLOAD_ITEM                          5
#define FUNCTION_DOWNLOAD_LIST_TYPE_ALL                15
#define FUNCTION_DOWNLOAD_LIST_TYPE_ALREADY_DOWNLOADED 16
#define FUNCTION_DOWNLOAD_LIST_TYPE_NOT_DOWNLOADED     17
#define FUNCTION_DOWNLOAD_PREVIEW                      18
#define FUNCTION_DOWNLOAD_LIST_TYPE_NEW_ITEMS          20
#endif // ENABLE_DOWNLOAD

//End server functions for HTTP communication:///////////////////////////////


//Define for User status, using for Login system
#define XPLAYER_USER_STATE_OFFLINE						1
#define XPLAYER_USER_STATE_ONLINE						2
#define XPLAYER_USER_STATE_PLAYING						3

//Define user avatar status
#define AVATAR_STATE_NOTHING                            0
#define AVATAR_STATE_CHECKING                           1
#define AVATAR_STATE_CHECKE_FAILED                      2
#define AVATAR_STATE_CHECKE_PASS                        3

//Define for Message type, using for Message Functions
#define	USER_MESSAGE_TYPE_GAMEMESSAGE					1				
#define	USER_MESSAGE_TYPE_USERMESSAGE					2				
#define	USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION	4				
#define	USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION	8				
#define	USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION	16
#define USER_MESSAGE_TYPE_GAMEMESSAGE_AVATAR			32

//Define for Message Read Type, using for Message Functions
#define USER_MESSAGE_READTYPE_ALL						0
#define USER_MESSAGE_READTYPE_NEW						1
#define USER_MESSAGE_READTYPE_READED 					2

//Define data seperator, by default is '|'
#define XPLAYER_DATA_SEPERATOR							'|'

#define		GUNCTION_BASE								500

enum GL_XPLAYER_FUNCTION_ID
{
	GL_XPLAYER_FUNCTION_REGISTER = FUNCTION_REGISTER,
	GL_XPLAYER_FUNCTION_LOGIN = FUNCTION_LOGIN,
	GL_XPLAYER_FUNCTION_LOGOUT = FUNCTION_LOGOUT,
	GL_XPLAYER_FUNCTION_UPLOAD_SCORE = FUNCTION_UPLOAD_SCORE,
	GL_XPLAYER_FUNCTION_GET_COUNTRY = FUNCTION_GET_COUNTRY,
//	GL_XPLAYER_FUNCTION_ITEM = FUNCTION_UPLOAD_SCORE + GUNCTION_BASE
};


#ifdef ENABLE_MULTIPLAYER

// Possible message types for the multiplayer protocol

// the basic message types 
#define BASIC_MESSAGE_TYPE_UNKNOWN           0
#define BASIC_MESSAGE_TYPE_GAME             'g'
#define BASIC_MESSAGE_TYPE_CONNECT          's'
#define BASIC_MESSAGE_TYPE_PING		        'p'

// the connection message types 
#define GLLIVE_CONNECTION_MESSAGE_TYPE_INIT_READ   'r'
#define GLLIVE_CONNECTION_MESSAGE_TYPE_INIT_WRITE  'w'
#define GLLIVE_CONNECTION_MESSAGE_TYPE_SUCCESS     's'
#define GLLIVE_CONNECTION_MESSAGE_TYPE_ERROR       'e'
#define GLLIVE_CONNECTION_MESSAGE_TYPE_DISCONECT   'x'


// the game message types 
#define GAME_MESSAGE_REQUEST                'r'
#define GAME_MESSAGE_PUSH                   'p'
#define GAME_MESSAGE_IN_GAME                'g'
#define GAME_MESSAGE_IN_GAME_RELIABLE		't'
#define GAME_MESSAGE_IN_GAME_TOALL          'h'
#define GAME_MESSAGE_ERROR                  'e'
#define GAME_MESSAGE_KEEP_ALIVE             'a'
#define GAME_MESSAGE_RESPONSE				's'

// the request message types 
#define REQUEST_MESSAGE_LOGIN               'i'
#define REQUEST_MESSAGE_CREATE_SESSION      'c'
#define REQUEST_MESSAGE_LIST_SESSIONS       'l'
#define REQUEST_MESSAGE_GET_QUICK_SESSION   'u'
#define REQUEST_MESSAGE_JOIN_SESSION        'j'
#define REQUEST_MESSAGE_LEAVE_SESSION       'q'
#define REQUEST_MESSAGE_KICK_OUT_PLAYER     'k'
#define REQUEST_MESSAGE_START_GAME          's'
#define REQUEST_MESSAGE_FINISH_GAME         'f'
#define REQUEST_MESSAGE_UPDATE_SESSION		'e'

// sent by the server ... 
#define RESPONSE_MESSAGE_SUCCESS            's'
#define RESPONSE_MESSAGE_ERROR              'e'

// error codes for the requests 
#define ERROR_INVALID_INPUT                 'i'
#define ERROR_NO_SESSION                    's'
#define ERROR_SESSION_CLOSED                'c'
#define ERROR_NOT_MASTER                    'm'
#define ERROR_LOGIN_NICKNAME_USED           'n'
#define ERROR_LOGIN_INVALID_NICKNAME        'q'
#define ERROR_LOGIN_AUTHENTICATION_FAILED   'a'
#define ERROR_CREATE_SESSION_INVALID_NAME   'v'
#define ERROR_CREATE_SESSION_NAME_USED      'u'
#define ERROR_BLOCKED_SESSION               'b'
#define ERROR_LIST_SESSION_INVALID_INDEX    'l'
#define ERROR_JOIN_SESSION_TOO_MANY_PLAYERS 'j'
#define ERROR_NO_PLAYER                     'k'
#define ERROR_KICK_OUT_PLAYER_IS_MASTER     'd'
#define ERROR_START_GAME_NOT_ENOUGH_PLAYERS 'g'
#define ERROR_JOIN_SESSION_WRONG_PASSWORD	'p'


// the push message types .... 
#define PUSH_MESSAGE_JOIN_SESSION           'j'
#define PUSH_MESSAGE_LEAVE_SESSION          'l'
#define PUSH_MESSAGE_KICK_OUT               'k'
#define PUSH_MESSAGE_START_GAME             's'
#define PUSH_MESSAGE_FINISH_GAME            'f'
#define PUSH_MESSAGE_FINISH_GAME_CONFIRM    'c'
#define PUSH_MESSAGE_FINISH_GAME_RESTART    'r'

#define ERROR_KICK_OUT_KICKED				0
#define ERROR_KICK_OUT_NO_SESSION           1

#define REQUEST_QUERY_BY_NAME					'n'
#define REQUEST_QUERY_BY_DATA					'd'
#define REQUEST_QUERY_ALL						'a'

#endif //ENABLE_MULTIPLAYER

// every game can have a different score policy :
// every leader board can be send separately, or multiple leader boards can be sent in a single request
// each leader board can have only one or multiple supplemental data fields
// if the leader boards has supplemental data fields, the score cannot be uploaded unless all the
// supplemental data fields are contained in the request
// this test application contains the demo code for sending one single score without supplemental data,
// one single score for a leader board having one supplemental data field, or two leader boards at the same time,
// without or with supplemental data.
// when integrating into a game, keep the code that is appropriate for the leader boards included in that specific game..
// when using this test application, make sure you set the scoring policy according to the leader board settings that
// correspond to the GGI you are using, otherwise the request will fail


/**
* Used to define anything that should be a number but it has not been
* initialised or does not have a value yet. For example when the user inquires
* about his score for a level that he has never played yet or anything similar.
*/
#define NOT_A_NUMBER (-666666)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////#define XPLAYER_CONF_URL			"XPlayerURL"

#define PROFILE_FILE_SERVER_CONFIG 	"serverConfig.sav"

#define XPLAYER_SER_CONF_URL			"XplayerURL"
#define XPLAYER_SER_CONF_DWURL			"XplayDWURL"
#define XPLAYER_SER_CONF_MPURL			"XplayMPURL"
#define XPLAYER_SER_CONF_PHPVER			"XPPHPVerNo"

#define XPLAYER_SER_CONF_CHAT_SERVER		"ChatServer"
#define XPLAYER_SER_CONF_CHAT_DOMAIN		"ChatDomain"
#define XPLAYER_SER_CONF_CHAT_ROOM_DOMIAN	"ChatRoomDomain"


#define XPLAYER_SER_CONF_CONFIG_URL_TYPE			"type"
//#define XPLAYER_SER_CONF_CONFIG_RUN_TIME			"RunTimes"
#define XPLAYER_SER_CONF_CONFIG_RUN_DATE			"Date"

#define  XPLAYER_SER_CONF_CONFIG_OUT_DATE            10


#define RUN_TIME_COUNT 20
#define SERVER_CONFIG_URL  "http://gllive-beta.gameloft.com/ope/ServerConfig.php"
#endif//__XPLAYER_DEFINES__H__