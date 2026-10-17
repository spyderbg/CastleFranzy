
/**
 * the confing of glxplayer,it will be removed by obfuscated
 * 
 * @author World Online Team <<online-adm@gameloft.com>>
 * @author yi.he <<yi.he@gameloft.com>>
 * @version 0.0.1
 */
public class GLXPlayerConfig {
	public interface Configuration {
		/**
		 * The default encoding used, this encoding will be used for data
		 * transfer between client-to-server and client-to-server-to-client
		 */
		public static final String DEFAULT_ENCODING = "UTF-8";
		/**
		 * The XPlayer Version.
		 * <p>
		 * It is used to make sure the application connects to the correct
		 * server.
		 * </p>
		 */
		public static final int XPLAYER_VERSION = 4;

		/**
		 * Enables debug print statements.
		 * <p>
		 * Set to <code>false</code> before releasing a version to either
		 * testing or submission.
		 * </p>
		 */
		public static final boolean ENABLE_DEBUG = true;

		/**
		 * Enables or disables the outside ("software") timeout mechanism. Use
		 * this only on phones with timeout problems (some Samsung phones).
		 * <p>
		 * On most phones the connection times out if an answer is not received
		 * within a certain time of request start. However, on some phones, the
		 * connection thread hangs indefinetly, so we must implement an outside
		 * timeout mechanism.
		 * </p>
		 * <p>
		 * If set to <code>true</code>, the timeout mechanism is enabled and
		 * the server requests timeout after <code>CONN_TIMEOUT</code>
		 * miliseconds.
		 * </p>
		 * 
		 * @see #CONN_TIMEOUT
		 */
		public static final boolean ENABLE_TIMEOUT = false;

		/**
		 * The time in milliseconds after which a server request is timed out if
		 * a response is not received.
		 * <p>
		 * This is used only if <code>ENABLE_TIMEOUT</code> is set to
		 * <code>true</code>.
		 * </p>
		 * 
		 * @see #ENABLE_TIMEOUT
		 */
		public static int CONN_TIMEOUT = 30000; // 30 seconds.

		/**
		 * Sets the cancel operation to either interrupt or not the connection
		 * in progress.
		 * <p>
		 * On some phones, if the current conection is stopped on
		 * <code>cancel</code>, the game crashes.
		 * </p>
		 * <p>
		 * To avoid this, the cancel operation must not interrupt the
		 * communication and the thread should be left to finish naturally.
		 * Also, if another connection must be performed, the application should
		 * wait for the previous one to finish.
		 * </p>
		 * <p>
		 * All this can be done by setting this constant to <code>true</code>.
		 * </p>
		 * <p>
		 * It is recommended to set this variable <code>true</code> on the
		 * following phones: Sanyo MIDP1, LG MIDP 1
		 * </p>
		 * <p>
		 * It is highly recommended to be set it to <code>false</code> if not
		 * needed otherwise.
		 * </p>
		 */
		public static final boolean HTTP_NO_CANCEL = false;

		/**
		 * Sets wether to use HTTP <b>POST</b> requests instead of <b>GET</b>
		 * requests or not.
		 * <p>
		 * Some phones have problems if the HTTP GET request is too big.
		 * Normally they should support more than 500 characters per request,
		 * but some Nokia phones do not work with more than 120.
		 * </p>
		 * <p>
		 * On those phones, it is necesarry to use HTTP POST requests instead of
		 * GET ones.
		 * </p>
		 * <p>
		 * Set this to <code>true</code> only on those phones (Nokia 6225 and
		 * related phones, some Samsung - ex.D500).
		 * </p>
		 */
		public static final boolean USE_HTTP_POST = false;

		/**
		 * Sets wether to enable or disable the fullscreen bug related code.
		 * <p>
		 * Some MIDP2 phones have problems when a connection is open for the
		 * first time. Upon displaying the "Allow network access?" dialog, the
		 * game is forced to run in a window, although it has been set to run
		 * fullscreen. When that dialog ends (by input from the user, allowing
		 * or disallowing access to the network for the app), the game is not
		 * returned in fullscreen mode and it has to force itself in windowed
		 * mode and then in fullscreen mode again if it still has to run in
		 * fullscreen mode.
		 * </p>
		 * <p>
		 * On those phones, it is necesarry to set this value to
		 * <code>true</code> and use the <code>needScreenModeChange()</code>
		 * function in the main loop to check wether the screen mode has to be
		 * switched.
		 * </p>
		 * <p>
		 * Set this to <code>true</code> only on those phones (Samsung A900 is
		 * one of them).
		 * </p>
		 * 
		 * @see #needScreenModeChange()
		 */
		public static final boolean FULLSCREEN_CHANGE = false;

		/**
		 * Sets specific behaviour for MX Telcel.
		 * <p>
		 * Set to <code>true</code> if the game is intended for MX Telcel.
		 * </p>
		 */
		public static final boolean CARRIER_MXTELCEL = false;

		/**
		 * Sets specific behaviour for Canada Roger.
		 * <p>
		 * Set to <code>true</code> if the game is intended for CN Roger.
		 * </p>
		 */

		public static final boolean CARRIER_CNROGER = false;

		// BEGIN MULTIPLAYER SUPPORT
		/**
		 * Enables support for multiplayer games using TCP connections.
		 * <p>
		 * Set this to <code>true</code> only for games having multiplayer
		 * features. If the application does not have any multiplayer features,
		 * set this to <code>false</code> and don't use any of the multiplayer
		 * functions and variables, the obfuscator will remove them from the
		 * package.
		 * </p>
		 */
		public static final boolean ENABLE_MULTIPLAYER = false;

		/**
		 * Enables 2 different connections for sending and receiving data using
		 * TCP.
		 * <p>
		 * Some phones don't support reading and writing using the same
		 * connection - ex. : Motorola phones.
		 * </p>
		 * <p>
		 * Use this only in conjunction with ENABLE_MULTIPLAYER.
		 * </p>
		 */
		public static final boolean ENABLE_DUAL_TCP = false;

		/**
		 * Enable data read optimized method.
		 * <p>
		 * Some carriers use an optimized method of data communication on its
		 * servers. ex.: Nextel and Boost, Data Servers. If we try to read 255
		 * bytes from the server and don't have all the information the request
		 * will be suspended until the data package is complete. The server only
		 * sends the information requested if it have all the volume of data.
		 * </p>
		 * <p>
		 * Use this only in conjunction with ENABLE_MULTIPLAYER.
		 * </p>
		 */
		public static final boolean ENABLE_READ_COMMUNICATION_SUPPORT = false;

		/**
		 * Enables the use of player specific data for the multiplayer server.
		 * <p>
		 * This data is sent when the player logs on the multiplayer server, and
		 * can be retrieved by any other player using
		 * REQUEST_MESSAGE_GET_PLAYER_DATA.
		 * </p>
		 * <p>
		 * Use this only in conjunction with ENABLE_MULTIPLAYER.
		 * </p>
		 */
		public static final boolean ENABLE_PLAYER_SPECIFIC_DATA = false;

		/**
		 * Enables the search player feature.
		 * <p>
		 * A user can be seached and his status returned. If he is currently in
		 * a session, the session name and number of players are also returned.
		 * </p>
		 * <p>
		 * Use this only in conjunction with ENABLE_MULTIPLAYER.
		 * </p>
		 */
		public static final boolean ENABLE_FIND_PLAYER = true;

		/**
		 * The idle time allowed before the TCP connection to the server is
		 * disconnected.
		 * <p>
		 * Use this timer to send keep alive packets when no other transmission
		 * being performed for a long time and the connection needs to be open.
		 * </p>
		 * <p>
		 * Use this only in conjunction with ENABLE_MULTIPLAYER.
		 * </p>
		 */
		public static final int KEEP_ALIVE_TIME = 7000;
		// END MULTIPLAYER SUPPORT

		/**
		 * The multiplayer part (tcp) can be integrated with Xplayer database
		 * (pure) or not, when the tcp server functions without an Xplayer
		 * database. This usualy should be set <b>true</b>. The value false can
		 * be handy in debugging/development stages
		 */
		public static final boolean ENABLE_PURE_XPLAYER = true;

		public static final boolean ENABLE_KEEP_ALIVE_WITH_WEB = true;
		public static final int KEEP_ALIVE_WITH_WEBSERVER_INTERVAL = 90000;
	}

	public interface Error {
		// Error codes :

		/**
		 * Initialisation value.
		 * <p>
		 * Getting this error means that actually no requests have been made yet
		 * since the MIDlet started so there is no last error.
		 * </p>
		 */
		public static final int ERROR_INIT = -100;

		/**
		 * No connection to server.
		 * <p>
		 * Either there is no network available or the server cannot be
		 * contacted for some reason.
		 * </p>
		 */
		public static final byte ERROR_NOT_M7_ENABLED = -3;

		/**
		 * No connection to server.
		 * <p>
		 * Either there is no network available or the server cannot be
		 * contacted for some reason.
		 * </p>
		 */
		public static final byte ERROR_CONNECTION = -2;

		/**
		 * Result is pending, the transaction with the server is ongoing.
		 */
		public static final byte ERROR_PENDING = -1;

		/**
		 * The request finished succesfully.
		 */
		public static final byte ERROR_NONE = 0;

		/**
		 * The uuid is missing from the request.
		 */
		public static final byte ERROR_NO_UUID = 1;

		/**
		 * The nickname is missing from the registration request.
		 */
		public static final byte ERROR_NO_NICKNAME = 2;

		/**
		 * The registration process failed.
		 */
		public static final byte ERROR_REGISTER_FAILED = 3;

		/**
		 * This user is not registered.
		 */
		public static final byte ERROR_NOT_REGISTERED = 4;

		/**
		 * This nickname is taken, another one is suggested.
		 */
		public static final byte ERROR_NICK_TAKEN = 5;

		/**
		 * The score was not posted for some reason.
		 */
		public static final byte ERROR_SCORE_UPLOAD_FAILED = 21;

		/**
		 * The get rankings operation failed for some reason.
		 */
		public static final byte ERROR_GET_RANKINGS_FAILED = 22;

		// BEGIN M7 SUPPORT
		/**
		 * Failed to rate this game.
		 */
		public static final byte ERROR_RATE_GAME_FAILED = 23;

		/**
		 * Failed to recommend this game.
		 */
		public static final byte ERROR_RECOMMEND_GAME_FAILED = 24;

		/**
		 * There is no phone number in the register request.
		 */
		public static final byte ERROR_NO_PHONE_NUMBER = 25;

		/**
		 * Programming error on emulator: no phone id
		 */
		public static final byte ERROR_NO_CLIENT_ID = 26;

		/**
		 * Configuration error: Most likely an invalid GGI.
		 */
		public static final byte ERROR_INVALID_GGI = 27;

		/**
		 * Failed to get the states for the player.
		 */
		public static final byte ERROR_GET_STATS_FAILED = 29;
		// END M7 SUPPORT

		/**
		 * Programming error: the supplemental data needed to upload this
		 * leaderboard was not correctly supplied.
		 */
		public static final byte ERROR_SUPPLEMENTAL_DATA_NEEDED = 31;

		/**
		 * Cingular MRC License Validation failed.
		 */
		public static final byte ERROR_VALIDATE_LICENSE_FAILED = 32;

		/**
		 * Change username process failed.
		 */
		public static final byte ERROR_CHANGE_USERNAME_FAILED = 33;

		/**
		 * The server response has incorrect format.
		 * <p>
		 * Make sure the request is correct. If it is a leaderboard request,
		 * make sure the number of supplemental data sent match the number for
		 * the leaderboard.
		 * </p>
		 */
		public static final byte ERROR_BAD_RESPONSE = 40;

		// BEGIN MULTIPLAYER SUPPORT ..
		/**
		 * Multiplayer error - the call has no relevance in this state.
		 * <p>
		 * The game and XPlayer think they're in different states.
		 * </p>
		 */
		public static final byte ERROR_WRONGFULL_QSTATE = 50;

		/**
		 * Multiplayer error - this response is not expected.
		 * <p>
		 * The game and XPlayer think they're in different states.
		 * </p>
		 */
		public static final byte ERROR_WRONGFULL_RSTATE = 51;

		/**
		 * Multiplayer error - there was an error starting the game.
		 */
		public static final byte ERROR_START_GAME = 52;

		/**
		 * Multiplayer error - there was an error joining the game.
		 */
		public static final byte ERROR_JOIN_GAME = 53;

		/**
		 * Multiplayer error - there was a server error processing the request.
		 */
		public static final byte ERROR_REQUEST_FAILED = 54;
		// END MULTIPLAYER SUPPORT ..

		// CONTENT DOWNLOAD SUPPORT
		public static final byte ERROR_DOWNLOAD_FILE_NOT_FOUND = 30; // the
																		// file
																		// that
																		// was
																		// requsted
																		// was
																		// not
																		// found
																		// on
																		// the
																		// server
		public static final byte ERROR_DOWNLOAD_LIST_NO_FILES = 31; // the list
																	// files
																	// request
																	// cannot
																	// find any
																	// file that
																	// matches
																	// the
																	// criteria
		public static final byte ERROR_DOWNLOAD_FAILED = 32; // the download
																// (either list,
																// preview or
																// file failed
																// for some
																// reason)
		public static final byte ERROR_DOWNLOAD_NO_SPACE = 33;

		// /////////////////////////////////////////
	}

}
