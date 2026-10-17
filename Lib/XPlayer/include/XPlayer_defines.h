/**
 * The const defination for XPlayer.
 *
 *
 * @version 0.0.1
 *
 */

#ifndef __XPLAYER_DEFINES__H__
#define __XPLAYER_DEFINES__H__

#if WIN32
#ifndef OS_WIN32
#define OS_WIN32
#endif
#else //WIN32
#ifndef OS_IPHONE
#define OS_IPHONE
#endif
#endif //WIN32

//>>lqguo for unicode 
typedef unsigned short Unicode2Bytes;
#define PER_CHARACTER_COUNT  3
//<<end for unicode 

//>>for lobby
//for lobby test
#ifdef OS_WIN32
//#define LOBBY_THREAD_TEST
//#define LOBBY_TEST_FILE_NAME         ".\\lobbytest\\lobbytest"
#endif

#define GAME_PARAMETER_COUNT "GAME_PARAM_COUNT"
#define GAME_PARAMETER_TYPE  "GAME_PARAM_TYPE"

#define	MAX_FRIEND_COUNT_SEND_TO_SERVER		( 30 ) //the max count of friends when sending the friend filter

enum LoginLobby
{
	SET_LOBBY_STATUS = 0,
	KEEP_LOBBY_STATUS
};

enum LobbyStatus
{
	LOBBY_UNLAUNCHED = 0,
	LOBBY_LAUNCHED = 1
};

enum LobbyLocked
{
	LOBBY_UNLOCKED = 0,
	LOBBY_LOCKED = 1
};

enum LobbyVisibility
{
	LOBBY_UNVISIBLE = 0,
	LOBBY_VISIBLE = 1
};

enum UserStatus
{
	USER_NONE = 0,
	USER_NOT_IN_LOBBY = 1,
	USER_PLAYER_NOT_READY = 2,
	USER_PLAYER_READY = 3,
	USER_SPECTATOR = 4,
	USER_PLAYER_OFFLINE = 5,
	USER_SPECTATOR_OFFLINE = 6
};


/**
 * CMSG: Client -> Server
 * SMSG: Server -(return)-> Client
 * PMSG: Server -(push)-> Client
 *
 */
enum LOBBY_MESSAGE_TYPE
{
	MSG_UNKNOW					= 0x0000,
	CMSG_CONNECT				= 0x0001,
	SMSG_CONNECT				= 0x0002,
	CMSG_DISCONNECT				= 0x0003,
	SMSG_DISCONNECT				= 0x0004,
	CMSG_LOGIN					= 0x0005,
	SMSG_LOGIN					= 0x0006,
	CMSG_LOGOUT					= 0x0007,
	SMSG_LOGOUT					= 0x0008,
	CMSG_KEEP_ALIVE				= 0x0009,
	SMSG_KEEP_ALIVE				= 0x000a,
	CMSG_CREATE_LOBBY			= 0x000b,
	SMSG_CREATE_LOBBY			= 0x000c,
	CMSG_QUERY_LOBBY			= 0x000d,
	SMSG_QUERY_LOBBY			= 0x000e,
	CMSG_JOIN_LOBBY				= 0x000f,
	SMSG_JOIN_LOBBY				= 0x0010,
	PMSG_JOIN_LOBBY_UNLAUNCHED	= 0x0011,
	PMSG_JOIN_LOBBY_LAUNCHED	= 0x0012,
	CMSG_LEAVE_LOBBY			= 0x0013,
	SMSG_LEAVE_LOBBY			= 0x0014,
	PMSG_LEAVE_LOBBY			= 0x0015,
	CMSG_GET_LOBBY_INFOR		= 0x0016,
	SMSG_GET_LOBBY_INFOR		= 0x0017,
	CMSG_KICK_OUT_PLAYER		= 0x0018,
	SMSG_KICK_OUT_PLAYER		= 0x0019,
	PMSG_KICK_OUT_PLAYER		= 0x0020,
	CMSG_SET_GAME_PARAMETER		= 0x0021,
	SMSG_SET_GAME_PARAMETER		= 0x0022,
	PMSG_SET_GAME_PARAMETER		= 0x0023,
	CMSG_SET_USER_PARAMETER		= 0x0024,
	SMSG_SET_USER_PARAMETER		= 0x0025,
	PMSG_SET_USER_PARAMETER		= 0x0026,
	CMSG_SET_USER_STATUS		= 0x0027,
	SMSG_SET_USER_STATUS		= 0x0028,
	PMSG_SET_USER_STATUS		= 0x0029,
	CMSG_START_GAME				= 0x002A,
	SMSG_START_GAME				= 0x002B,
	PMSG_START_GAME				= 0x002C,
	CMSG_REJOIN_GAME			= 0x002D,
	SMSG_REJOIN_GAME			= 0x002E,
	PMSG_REJOIN_GAME			= 0x002F,
	CMSG_GAMESERVER_ADDR		= 0x0030, // from game server
	SMSG_GAMESERVER_ADDR		= 0x0031, // to game server
	PMSG_GAMESERVER_ADDR		= 0x0032, // push to users in lobby 
	PMSG_GAMESERVER_PARAMS		= 0x0033, // push to game server

	CMSG_QUERY_LOBBY_BY_FRIENDID= 0x0034,
	SMSG_QUERY_LOBBY_BY_FRIENDID= 0x0035,
	CMSG_QUERY_LOBBY_OF_FRIENDS = 0x0036,
	SMSG_QUERY_LOBBY_OF_FRIENDS = 0x0037,
	CMSG_AUTOMATCH_USER			= 0x0038,
	SMSG_AUTOMATCH_USER			= 0x0039,
	CMSG_AUTOMATCH_USER_CANCLE	= 0x004A,
	SMSG_AUTOMATCH_USER_CANCLE	= 0x004B,
	PMSG_AUTOMATCH_USER			= 0x004C,

	PMSG_SAMEACCOUNT_LOGIN		= 0x0050,

	CMSG_QUERY_LOBBY_GAMEPAMAM	= 0x0060,
	SMSG_QUERY_LOBBY_GAMEPAMAM	= 0x0061,
	CMSG_QUERY_LOBBY_BY_FRIENDID_GAMEPARAM = 0x0061,
	SMSG_QUERY_LOBBY_BY_FRIENDID_GAMEPARAM = 0x0062,
	CMSG_QUERY_LOBBY_OF_FRIENDS_GAMEPARAM = 0x0063,
	SMSG_QUERY_LOBBY_OF_FRIENDS_GAMEPARAM = 0x0064,
};

enum LOBBY_ERROR_TYPE
{
	ERROR_NONE									= 0x0000,
	ERROR_INVALID_INPUT_LOBBY					= 0x4001,
	ERROR_NO_SESSION							= 0x4002,
	ERROR_SESSION_CLOSED						= 0x4003,
	ERROR_NOT_MASTER							= 0x4004,
	ERROR_LOGIN_NICKNAME_USED					= 0x4005,
	ERROR_LOGIN_INVALID_NICKNAME				= 0x4006,
	ERROR_LOGIN_AUTHENTICATION_FAILED			= 0x4007,
	ERROR_MAX_USER_LIMIT						= 0x4008,
	ERROR_MAX_SESSION_LIMIT						= 0x4009,
	ERROR_CREATE_SESSION_INVALID_NAME           = 0x400a,
	ERROR_CREATE_SESSION_NAME_USED              = 0x400b,
	ERROR_BLOCKED_SESSION						= 0x400c,
	ERROR_LIST_SESSION_INVALID_INDEX            = 0x400d,
	ERROR_JOIN_SESSION_TOO_MANY_PLAYERS_LOBBY   = 0x400e,
	ERROR_JOIN_SESSION_CANNOT_MATCH_CRITERIA    = 0x400f,
	ERROR_NO_PLAYER								= 0x4010,
	ERROR_KICK_OUT_PLAYER_IS_MASTER             = 0x4011,
	ERROR_START_GAME_NOT_ENOUGH_PLAYERS         = 0x4012,
	ERROR_LOGIN_INVALID_UID						= 0x4013,
	ERROR_LOGIN_COMMUNICATE_WITH_GLLIVE			= 0x4014,
	//ERROR_SET_USER_STATUS_NOUSER_INDB			= 0x0015,
	ERROR_SET_USER_STATUS_NOLOBBYID_INDB		= 0x4016,
	ERROR_SET_USER_STATUS_INVALID_STATUS		= 0x4017,
	ERROR_JOIN_SESSION_INVALID_STATUS			= 0x4018,
	ERROR_START_GAME_NOT_CREATOR				= 0x4019,
	ERROR_START_GAME_NOT_ALLPLAYER_READY		= 0x4020,
	ERROR_START_GAME_LAUNCHGAME_FAILED			= 0x4021,
	ERROR_SET_GAME_PARAMETER_NOTINLOBBY			= 0x4022,
	ERROR_SET_GAME_PARAMETER_NOT_CREATOR		= 0x4023,
	ERROR_SET_GAME_PARAMETER_ALREADY_READY		= 0x4024,
	ERROR_SET_USER_PARAMETER_NOTINLOBBY			= 0x4025,
	ERROR_SET_USER_PARAMETER_SPECTATOR			= 0x4026,
	ERROR_SET_USER_PARAMETER_ALREADY_READY		= 0x4027,
	ERROR_GET_LOBBY_DETAILS_NOTEXIST_LOBBY		= 0x4028,
	ERROR_CREATE_SESSION_CANNOT_INSERTTODB		= 0x4029,
	ERROR_KEEP_ALIVE_NOTEXIST_LOBBY				= 0x402A,
	ERROR_KEEP_ALIVE_NOTINLOBBY					= 0x402B,
	ERROR_LOGIN_ALREADY_LOGINED					= 0x402C,
	ERROR_LOGIN_CANNOT_INSERT_TO_DB				= 0x402D,
	ERROR_JOIN_SESSION_LOBBY_LAUNCHED_AND_LOCKED= 0x402E,
	ERROR_REJOIN_NOT_INLOBBY					= 0x402F,
	ERROR_REJOIN_FAIL_RESTORE_ROOM				= 0x4030,
	ERROR_AUTOMATCH_WAITING						= 0X4031,
	ERROR_LOBBY_TIME_OUT						= 0X4032,  //this is used when time out after login lobby
	ERROR_SAMEACCOUNT_LOGIN						= 0x4050,
};
//<<end lobby

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
#define XPLAYER_CONF_GAME_VER "GameVer"



/**
 * Enable flag.
 * <p> It is used to enable or disable online functions.
 * <p> Remove it if you want to disable online.
 *
 */
#define ENABLE_ONLINE

// Multiplayer (TCP/IP) timeout.
#define MP_ENABLE_TIMEOUT

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

#define ENABLE_DOWNLOAD

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

//#define ENABLE_HTTP_REQUEST_QUEUE

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
#define USE_USER_NUMBER

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

#define KEEP_ALIVE_WITH_WEBSERVER_INTERVAL   90000

#define KEEP_ALIVE_WITH_CHATSERVER_INTERVAL  (3*60*1000) //3minutes

#define BROADCAST_TIME_INTERVAL             500
#define BROADCAST_TIME_REMOVE				3000

#define TEST_TIMEOUT                        2000


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

#define ONLINE_MAX_PACKET_SIZE				2048

/** 
* Every game can have a different score policy :
* Every leader board can be send separately, or multiple leader boards can be sent in a single request
* Each leader board can have only one or multiple supplemental data fields
* If the leader boards has supplemental data fields, the score cannot be uploaded unless all the
* supplemental data fields are contained in the request
* This test application contains the demo code for sending one single score without supplemental data,
* one single score for a leader board having one supplemental data field, or two leader boards at the same time,
* without or with supplemental data.
* When integrating into a game, keep the code that is appropriate for the leader boards included in that specific game..
* When using this test application, make sure you set the scoring policy according to the leader board settings that
* correspond to the GGI you are using, otherwise the request will fail
*/
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

//for compress error when compress the video data
#define ERROR_COMPRESS_UNKNOWN                              (-7)  //unknown error
#define ERROR_COMPRESS_MEM                                  (-6)  //not enough memory

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
#define ERROR_BAD_USER_NAME						70
#define ERROR_YOUTUBE_ACCOUNT_REJECTED			71
#define ERROR_YOUTUBE_ACCOUNT_NOT_EXIT			72
#define	ERROR_GOOGLE_ACCOUNT_EMPTY				73
#define	ERROR_GOOGLE_PASSWORD_EMPTY				74
#define	ERROR_TITLE_EMPTY 						75
#define	ERROR_GOOGLE_ACCOUNT_INVALID 			76
#define ERROR_RECORD_NOT_EXIST              	77
#define ERROR_FAIL_UPDATE_DB                	78
#define ERROR_VIDEO_EXIST                		79
#define ERROR_VID_INVALID                  		80
#define ERROR_VIDEO_MAX_INVALID           		81
#define ERROR_FRIENDS_TOO_MANY 					93
#define ERROR_EMAIL_USED 						97


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

#define ERROR_MP_UPNP_EXCUTE_FUN			506

#define ERROR_MP_UPNP_MAPPING		507


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



/**
******************************************************************************
******************************************************************************
**                     Char Error Codes                              **
******************************************************************************
******************************************************************************
*/

/**
 * This describes connection error conditions.
 */
enum ChatConnectionError
{
  ChatConnNoError,                    /**< Not really an error. Everything went just fine. */
  ChatConnStreamError,                /**< A stream error occured. The stream has been closed.
                                   * Use ClientBase::streamError() to find the reason. */
  ChatConnStreamVersionError,         /**< The incoming stream's version is not supported */
  ChatConnStreamClosed,               /**< The stream has been closed (by the server). */
  ChatConnProxyAuthRequired,          /**< The HTTP/SOCKS5 proxy requires authentication.
                                   * @since 0.9 */
  ChatConnProxyAuthFailed,            /**< HTTP/SOCKS5 proxy authentication failed.
                                   * @since 0.9 */
  ChatConnProxyNoSupportedAuth,       /**< The HTTP/SOCKS5 proxy requires an unsupported auth mechanism.
                                   * @since 0.9 */
  ChatConnIoError,                    /**< An I/O error occured. */
  ChatConnParseError,                 /**< An XML parse error occurred. */
  ChatConnConnectionRefused,          /**< The connection was refused by the server (on the socket level).
                                   * @since 0.9 */
  ChatConnDnsError,                   /**< Resolving the server's hostname failed.
                                   * @since 0.9 */
  ChatConnOutOfMemory,                /**< Out of memory. Uhoh. */
  ChatConnNoSupportedAuth,            /**< The auth mechanisms the server offers are not supported
                                   * or the server offered no auth mechanisms at all. */
  ChatConnTlsFailed,                  /**< The server's certificate could not be verified or the TLS
                                   * handshake did not complete successfully. */
  ChatConnTlsNotAvailable,            /**< The server didn't offer TLS while it was set to be required
                                   * or TLS was not compiled in.
                                   * @since 0.9.4 */
  ChatConnCompressionFailed,          /**< Negotiating/initializing compression failed.
                                   * @since 0.9 */
  ChatConnAuthenticationFailed,       /**< Authentication failed. Username/password wrong or account does
                                   * not exist. Use ClientBase::authError() to find the reason. */
  ChatConnUserDisconnected,           /**< The user (or higher-level protocol) requested a disconnect. */
  ChatConnNotConnected                /**< There is no active connection. */
};

/**
 * This decribes stream error conditions as defined in RFC 3920 Sec. 4.7.3.
 */
enum ChatStreamErrorReason
{
  ChatStreamErrorUndefined,           /**< An undefined/unknown error occured. Also used if a diconnect was
                                   * user-initiated. Also set before and during a established connection
                                   * (where obviously no error occured). */
  ChatStreamErrorBadFormat,           /**< The entity has sent XML that cannot be processed;
                                   * this error MAY be used instead of the more specific XML-related
                                   * errors, such as &lt;bad-namespace-prefix/&gt;, &lt;invalid-xml/&gt;,
                                   * &lt;restricted-xml/&gt;, &lt;unsupported-encoding/&gt;, and
                                   * &lt;xml-not-well-formed/&gt;, although the more specific errors are
                                   * preferred. */
  ChatStreamErrorBadNamespacePrefix,  /**< The entity has sent a namespace prefix that is unsupported, or has
                                   * sent no namespace prefix on an element that requires such a prefix
                                   * (see XML Namespace Names and Prefixes (Section 11.2)). */
  ChatStreamErrorConflict,            /**< The server is closing the active stream for this entity because a
                                   * new stream has been initiated that conflicts with the existing
                                   * stream. */
  ChatStreamErrorConnectionTimeout,   /**< The entity has not generated any traffic over the stream for some
                                   * period of time (configurable according to a local service policy).*/
  ChatStreamErrorHostGone,            /**< the value of the 'to' attribute provided by the initiating entity
                                   * in the stream header corresponds to a hostname that is no longer
                                   * hosted by the server.*/
  ChatStreamErrorHostUnknown,         /**< The value of the 'to' attribute provided by the initiating entity
                                   * in the stream header does not correspond to a hostname that is hosted
                                   * by the server. */
  ChatStreamErrorImproperAddressing,  /**< A stanza sent between two servers lacks a 'to' or 'from' attribute
                                   * (or the attribute has no value). */
  ChatStreamErrorInternalServerError, /**< the server has experienced a misconfiguration or an
                                   * otherwise-undefined internal error that prevents it from servicing the
                                   * stream. */
  ChatStreamErrorInvalidFrom,         /**< The JID or hostname provided in a 'from' address does not match an
                                   * authorized JID or validated domain negotiated between servers via SASL
                                   * or dialback, or between a client and a server via authentication and
                                   * resource binding.*/
  ChatStreamErrorInvalidId,           /**< The stream ID or dialback ID is invalid or does not match an ID
                                   * previously provided. */
  ChatStreamErrorInvalidNamespace,    /**< The streams namespace name is something other than
                                   * "http://etherx.jabber.org/streams" or the dialback namespace name is
                                   * something other than "jabber:server:dialback" (see XML Namespace Names
                                   * and Prefixes (Section 11.2)). */
  ChatStreamErrorInvalidXml,          /**< The entity has sent invalid XML over the stream to a server that
                                   * performs validation (see Validation (Section 11.3)). */
  ChatStreamErrorNotAuthorized,       /**< The entity has attempted to send data before the stream has been
                                   * authenticated, or otherwise is not authorized to perform an action
                                   * related to stream negotiation; the receiving entity MUST NOT process
                                   * the offending stanza before sending the stream error. */
  ChatStreamErrorPolicyViolation,     /**< The entity has violated some local service policy; the server MAY
                                   * choose to specify the policy in the &lt;text/&gt;  element or an
                                   * application-specific condition element. */
  ChatStreamErrorRemoteConnectionFailed,/**< The server is unable to properly connect to a remote entity that is
                                   * required for authentication or authorization. */
  ChatStreamErrorResourceConstraint,  /**< the server lacks the system resources necessary to service the
                                   * stream. */
  ChatStreamErrorRestrictedXml,       /**< The entity has attempted to send restricted XML features such as a
                                   * comment, processing instruction, DTD, entity reference, or unescaped
                                   * character (see Restrictions (Section 11.1)). */
  ChatStreamErrorSeeOtherHost,        /**< The server will not provide service to the initiating entity but is
                                   * redirecting traffic to another host; the server SHOULD specify the
                                   * alternate hostname or IP address (which MUST be a valid domain
                                   * identifier) as the XML character data of the &lt;see-other-host/&gt;
                                   * element. */
  ChatStreamErrorSystemShutdown,      /**< The server is being shut down and all active streams are being
                                   * closed. */
  ChatStreamErrorUndefinedCondition,  /**< The error condition is not one of those defined by the other
                                   * conditions in this list; this error condition SHOULD be used only in
                                   * conjunction with an application-specific condition. */
  ChatStreamErrorUnsupportedEncoding, /**< The initiating entity has encoded the stream in an encoding that is
                                   * not supported by the server (see Character Encoding (Section 11.5)).
                                   */
  ChatStreamErrorUnsupportedStanzaType,/**< The initiating entity has sent a first-level child of the stream
                                   * that is not supported by the server. */
  ChatStreamErrorUnsupportedVersion,  /**< The value of the 'version' attribute provided by the initiating
                                   * entity in the stream header specifies a version of XMPP that is not
                                   * supported by the server; the server MAY specify the version(s) it
                                   * supports in the &lt;text/&gt; element. */
  ChatStreamErrorXmlNotWellFormed     /**< The initiating entity has sent XML that is not well-formed as
                                   * defined by [XML]. */
};

/**
 * Describes the defined SASL error conditions.
 */
enum ChatAuthenticationErrorReason
{
  ChatAuthErrorUndefined,             /**< No error occurred, or error condition is unknown. */
  ChatSaslAborted,                    /**< The receiving entity acknowledges an &lt;abort/&gt; element sent
                                   * by the initiating entity; sent in reply to the &lt;abort/&gt;
                                   * element. */
  ChatSaslIncorrectEncoding,          /**< The data provided by the initiating entity could not be processed
                                   * because the [BASE64] encoding is incorrect (e.g., because the encoding
                                   * does not adhere to the definition in Section 3 of [BASE64]); sent in
                                   * reply to a &lt;response/&gt; element or an &lt;auth/&gt; element with
                                   * initial response data. */
  ChatSaslInvalidAuthzid,             /**< The authzid provided by the initiating entity is invalid, either
                                   * because it is incorrectly formatted or because the initiating entity
                                   * does not have permissions to authorize that ID; sent in reply to a
                                   * &lt;response/&gt; element or an &lt;auth/&gt; element with initial
                                   * response data.*/
  ChatSaslInvalidMechanism,           /**< The initiating entity did not provide a mechanism or requested a
                                   * mechanism that is not supported by the receiving entity; sent in reply
                                   * to an &lt;auth/&gt; element. */
  ChatSaslMechanismTooWeak,           /**< The mechanism requested by the initiating entity is weaker than
                                   * server policy permits for that initiating entity; sent in reply to a
                                   * &lt;response/&gt; element or an &lt;auth/&gt; element with initial
                                   * response data. */
  ChatSaslNotAuthorized,              /**< The authentication failed because the initiating entity did not
                                   * provide valid credentials (this includes but is not limited to the
                                   * case of an unknown username); sent in reply to a &lt;response/&gt;
                                   * element or an &lt;auth/&gt; element with initial response data. */
  ChatSaslTemporaryAuthFailure,       /**< The authentication failed because of a temporary error condition
                                   * within the receiving entity; sent in reply to an &lt;auth/&gt; element
                                   * or &lt;response/&gt; element. */
  ChatNonSaslConflict,                /**< XEP-0078: Resource Conflict */
  ChatNonSaslNotAcceptable,           /**< XEP-0078: Required Information Not Provided */
  ChatNonSaslNotAuthorized            /**< XEP-0078: Incorrect Credentials */
};

/**
******************************************************************************
******************************************************************************
**                     END Char Error Codes                          **
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
#define		FUNCTION_READ_MY_SENT_MESSAGE				93
#define		FUNCTION_DEL_MY_SENT_MESSAGE				94

#define		FUNCTION_GET_PRODUCT						95
#define		FUNCTION_SET_USER_STATE						97
#define		FUNCTION_GET_GENERAL_CONDITIONS				98
#define		FUNCTION_GET_CHAT_WARNING					99
#define		FUNCTION_DOWNLOAD_USER_AVATAR				101
#define		FUNCTION_DEL_USER_AVATAR					102
#define		FUNCTION_GET_USER_AVATAR_LIST				103

#define		FUNCTION_CHECK_REPLAYVIDEO_AVAILABLE		104
#define		FUNCTION_ADD_VIDEO_INFO						105
#define		FUNCTION_UPLOAD_VIDEO_DATA					106

#define		FUNCTION_GET_LOBBY_SERVER					107
#define		FUNCTION_GAME_INVITATION_GET_LOBBY			108

#define     FUNCTION_KEEP_ALIVE                         16
#define		FUNCTION_LOGOUT								17
#define		FUNCTION_AUTH_CHARGE						42

#define     FUNCTON_NETWORK_TEST                        -1

#define		FUNCTION_ADD_BUDDY							30
#define		FUNCTION_REMOVE_BUDDY						31
#define		FUNCTION_GET_BUDDIES_LIST					32

//>>lqguo for get lobby  sever
#define		FUNCTION_GET_LOBBY_SERVER					107
#define		FUNCTION_GAME_INVITATION_GET_LOBBY			108
//<<end

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

/**
* Defined function for Youtube services.
*/
#define		FUNCTION_GET_VIDEO_INFO						1
#define		FUNCTION_SET_VIDEO_STATUS					2


//Define for User status, using for Login system
#define XPLAYER_USER_STATE_OFFLINE						1
#define XPLAYER_USER_STATE_ONLINE						2
#define XPLAYER_USER_STATE_PLAYING						3
#define XPLAYER_USER_STATE_WAIT_CONFIRM					4

//Define for Message type, using for Message Functions
#define	USER_MESSAGE_TYPE_GAMEMESSAGE					1				
#define	USER_MESSAGE_TYPE_USERMESSAGE					2				
#define	USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION	4				
#define	USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION	8				
#define	USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION	16

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
#define CONNECTION_MESSAGE_TYPE_INIT_READ   'r'
#define CONNECTION_MESSAGE_TYPE_INIT_WRITE  'w'
#define CONNECTION_MESSAGE_TYPE_SUCCESS     's'
#define CONNECTION_MESSAGE_TYPE_ERROR       'e'
#define CONNECTION_MESSAGE_TYPE_DISCONECT   'x'


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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



enum Opcode
{
	MSG_MMO_NULL_ACTION			= 0x0000,
	
	CMSG_MMO_ECHO 				= 0x0001,
	SMSG_MMO_ECHO 				= 0x0002,
	
	CMSG_MMO_LOGIN 				= 0x0011,
	SMSG_MMO_LOGIN_SUCCESS 		= 0x0012,	
	SMSG_MMO_LOGIN_FAILED		= 0x0013,	

	CMSG_MMO_IN_MAP 			= 0x0014,
	SMSG_MMO_IN_MAP_SUCCESS 	= 0x0015,
	SMSG_MMO_IN_MAP_FAILED   	= 0x0016,

	CMSG_MMO_LOGOUT				= 0x0021,
	SMSG_MMO_LOGOUT_SUCCESS		= 0x0022,	
	SMSG_MMO_LOGOUT_FAILED 		= 0x0023,
	
	CMSG_MMO_OUT_MAP 			= 0x0024,
	SMSG_MMO_OUT_MAP_SUCCESS 	= 0x0025,
	SMSG_MMO_OUT_MAP_FAILED		= 0x0026,

	CMSG_MMO_BYE 				= 0x0027, 

	SMSG_MMO_UPDATE        		= 0x0034,
	SMSG_MMO_ONLINE				= 0x0035,
	SMSG_MMO_OFFLINE			= 0x0036,

	CMSG_MMO_MOVE 				= 0x0041,
	SMSG_MMO_MOVE_SUCCESS 		= 0x0042,	
	SMSG_MMO_MOVE_FAILED		= 0x0043,	
	
};



#endif//__XPLAYER_DEFINES__H__