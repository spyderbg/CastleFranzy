

import javax.microedition.midlet.MIDlet;



/**
 * Class GLXPlayerWebComponent It is the class that need to connect to web
 * server
 * <p>
 * GLXPlayerHttp is used to send data to and receive data from web server by
 * methods SendByGet() and SendByPost().
 * </p>
 * <p>
 * Tag-GLLIVE
 * </p>
 * <p>
 * <code>you can find detailes of the requirement by the tag in requirement.doc</code>
 * </p>
 * {@link http://10.15.200.220/trac_prj/xplayer.client/attachment/wiki/XPlayer_project_xpj2me/Requirement%20%20for%20XPL%20J2ME.odt}
 * <p>
 * How to use
 * </p>
 * function update in main loop <code>
 * at the begining of the main loop you must call the function Update() of this class.
 * switch (online_substate) {
 case OnlineSubstate.SET: 
 //the request is composed and sent
 break;
 case OnlineSubstate.WAIT:
 //do nothing only  wait to be notified from server
 break;
 case OnlineSubstate.PROCESS:
 //setOnlineState(ONLINESTATE_GETUSERINFO);
 //state change
 break;

 }
 </code>
 * <p>
 * 
 * function paint in main loop <code>
 *switch (online_substate)
 {case
 OnlineSubstate.SET: 
 //paint what you want to show
 break; 
 case OnlineSubstate.WAIT: 
 case OnlineSubstate.PROCESS: 
 //g.drawString("please wait");
 break;
 }
 *  </code>
 * </p>
 * 
 * @author World Online Team <<online-adm@gameloft.com>>
 * @author yi.he <yi.he@gameloft.com>
 * @version 0.0.1
 */
public final class GLXPlayerWebComponent extends GLXPlayerComponent {
	/**
	 * @see #GetInst(MIDlet)
	 */
	private static GLXPlayerWebComponent s_inst;

	/**
	 * The time the current request started - used for timeout implementation.
	 * Use only in conjunction with ENABLE_TIMEOUT.
	 */
	static long s_callstarttime;

	/**
	 * The <b>Gameloft Game Identifier</b> associated with the game, read from
	 * the .jad file.
	 * <p>
	 * This should be unique for every <i>game + device</i> or <i>game + family
	 * of compatible devices</i> combination.
	 * </p>
	 */
	private static String GGI;
	/**
	 * The Gameloft Server url - it is read from the game's .jad file.
	 */
	private String URL;

	/**
	 * User Identifier
	 * 
	 * @see #GetUid()
	 */
	private String UID;

	/**
	 * user profile get from web server
	 */
	private String m_userName;
	private String m_userNum;
	private String m_password;
	private String m_changePassword;
	private String m_myEmail;
	private String m_myEmailflag;
	private String m_myCountry;
	private String m_myReputation;
	private String m_firstname;
	private String m_lastname;
	private String m_mySex;
	private String m_myBirthday;
	private String m_myTrophy;

	private StringBuffer m_requestBuffer = new StringBuffer(1);
	/**
	 * The flag indicate that if download file from web server
	 */
	protected static boolean isDownloadingFile = !true;

	/**
	 * Variable m_observer. GLXPlayerComponent instance observer
	 * 
	 * @see #RegisterObserver(IGLXPlayerHttpObserver)
	 */
	protected IGLXPlayerHttpObserver m_observer;

	/**
	 * Function RegisterObserver. it is a final function can not be overrided by
	 * subclass.
	 * 
	 * @param observe
	 * @see #m_observer
	 */
	public void RegisterObserver(Object observe) {
		m_observer = (IGLXPlayerHttpObserver)observe;
	}

	/**
	 * getter method to get user profile
	 * 
	 * @return
	 */
	public String GetUid() {

		return UID;
	}

	public String GetUserName() {

		return m_userName;
	}

	public String GetUserNum() {
		return m_userNum;
	}

	public String GetUserPassword() {

		return m_password;
	}

	public String GetEmail() {

		return m_myEmail;
	}

	public String GetEmailFlag() {

		return m_myEmailflag;
	}

	public String GetCountry() {

		return m_myCountry;
	}

	public String GetReputation() {

		return m_myReputation;
	}

	public String GetFirstName() {

		return m_firstname;
	}

	public String GetLastName() {

		return m_lastname;
	}

	public String GetSex() {

		return m_mySex;
	}

	public String GetBirthday() {

		return m_myBirthday;
	}

	public String GetTrophy() {

		return m_myTrophy;
	}

	static void ScreenModeChange(boolean flag) {
	}

	/**
	 * constructor
	 */
	private GLXPlayerWebComponent() {

		// TODO Auto-generated constructor stub
	}

	/**
	 * Design pattern : Singleton
	 * <p>
	 * Get GLXPlayerWebComponent inst
	 * </p>
	 * <p>
	 * It reads the values of the <b>URL</b> and <b>GGI</b> properties from
	 * the .jad file of <code>midlet</code> for initialisation purposes.
	 * </p>
	 * 
	 * @param midlet
	 *            Reference to the MIDlet of the game which implements this API.
	 * @return
	 */
	public static GLXPlayerWebComponent GetInst(MIDlet midlet) {
		if (s_inst == null) {

			s_inst = new GLXPlayerWebComponent();

			s_inst.URL = midlet.getAppProperty("XPlayerURL");
			s_inst.GGI = midlet.getAppProperty("GGI");
			s_inst.m_Connect_engine = new GLXPlayerHttp();
			s_inst.m_Connect_engine.RegisterObserver(s_inst);
		}
		return s_inst;
	}

	public static GLXPlayerWebComponent JunitGetInst() {
		if (s_inst == null) {

			s_inst = new GLXPlayerWebComponent();

		}
		return s_inst;
	}

	// the HTTP object used for low level server communication.
	protected GLXPlayerHttp m_Connect_engine;
	// True if the player is logged in.
	// This will happen only after successful login or registration
	// If this is true, no login is required anymore.
	private boolean m_isLoggedIn = false;
	// param to keep alive
	private long m_lastSendKeepAliveTime = 0;;
	private long m_keepAliveReturns = 0;;

	/**
	 * override method SetResponseData(String data))
	 */
	protected void SetResponseData(String data) {
		// TODO Auto-generated method stub
		try {

			// poor verification of a request response
			System.out.println("SetResponseData:" + data);
			String funType = GetValue(data, 0, '|');
			if (!(funType.equals("f") || funType.equals("g"))) {
				m_observer.OnNetworkError();
				return;
			}
			String tmpHR = GetValue(data, 1, '|');
			if (tmpHR == null) {
				m_observer.OnNetworkError();
				return;
			}
			int funId = Integer.parseInt(tmpHR);
			funId = GetFunctionID(funType, funId);
			tmpHR = GetValue(data, 2, '|');
			if (tmpHR.compareTo("r") != 0) {
				m_observer.OnNetworkError();
				return;
			}
			tmpHR = GetValue(data, 3, '|');

			if (tmpHR.compareTo("e") == 0) {
				OnUpdateFailure(funId, data);
			} else {
				OnUpdateSuccess(funId, data);
			}
		} catch (Exception e) {
			m_observer.OnRequestFailure(-1, -1, null);
		}
	}

	/**
	 * 
	 */
	protected void HandleConnect() {
		// TODO Auto-generated method stub
		if (m_Connect_engine.isInProgress()) {
			if (GLXPlayerConfig.Configuration.ENABLE_TIMEOUT) {
				if ((System.currentTimeMillis() - s_callstarttime) > GLXPlayerConfig.Configuration.CONN_TIMEOUT) {
					Cancel();
					// lastErrorCode = XPlayer.Error.ERROR_CONNECTION;
					m_observer.OnRequestTimeout(0);
				}
			}
			return;
		}

		if (m_Connect_engine.m_bError) {
			Cancel();
			m_observer.OnNetworkError();
			m_Connect_engine.m_bError = false;
			return;
		}
	}

	/**
	 * Function KeepAlive. Keep connection to web server alive.
	 */
	protected void KeepAlive() {
		// TODO Auto-generated method stub
		boolean ret = ShouldSendKeepAlive();
		if (ret) {
			m_requestBuffer.delete(0, m_requestBuffer.length());
			m_requestBuffer.append("f|").append(FUNCTION_KEEP_ALIVE).append(
					"|i|").append(GGI).append("|u|").append(UID).append("|t|")
					.append(m_keepAliveReturns).append("|");
			m_lastSendKeepAliveTime = System.currentTimeMillis();
			String request = String2Blob(m_requestBuffer.toString());
			System.out.println("keep alive");
			m_Connect_engine.sendByGet(URL, request);
		}
	}

	/**
	 * function ShouldSendKeepAlive. Calculate should call send keep alive or
	 * not
	 * 
	 * @return True / False
	 */
	private boolean ShouldSendKeepAlive() {
		if (!m_isLoggedIn) {
			return false;
		}

		long current_time = System.currentTimeMillis();

		if ((current_time - m_lastSendKeepAliveTime) > GLXPlayerConfig.Configuration.KEEP_ALIVE_WITH_WEBSERVER_INTERVAL) {
			return true;
		}

		return false;
	}

	public void Cancel() {
		// TODO Auto-generated method stub
		if (GLXPlayerConfig.Configuration.ENABLE_TIMEOUT) {
			s_callstarttime = 0;
		}
		m_Connect_engine.cancel();
	}

	/* blobbing functions */
	private String String2Blob(String str) {
		byte[] bs = str.getBytes();

		byte[] sBlob; // blob buffer

		int nBlobPos = 0; // current index of blob
		int nSPos = 0; // current index of string
		int nBitsNotUsed = 8; // how many bits not used in the current
		// s[nSPos]
		int nKeyIndex; // the index of key calcualted using s

		// calculate blob length and allocate memory
		int nBlobLength = (str.length()) * 8 / 6;

		if ((((str.length()) * 8) % 6) != 0) { // has remainder, need one more
			// char to hold it
			nBlobLength += 2; // remainder + ending 0
		}

		else {
			nBlobLength++; // need space for ending 0
		}

		sBlob = new byte[nBlobLength];

		for (int k = 0; k < nBlobLength; ++k) {
			sBlob[k] = 0;
		}

		while (nSPos < str.length()) {
			// get next 6 not used bits from s

			// first shift not used bits in current pos all the way to right

			nKeyIndex = bs[nSPos] & 0xFF;

			nKeyIndex = nKeyIndex >> (8 - nBitsNotUsed);

			if (nBitsNotUsed < 6) { // not enough bits in current pos
				if (++nSPos < str.length()) {
					// there's still char in the next pos, so use its bits
					// (nSPos
					// points to next pos now)
					// - get 6-nBitsNotUsed number of bits from next char
					// - to do so, we first shift the char in next pos to left
					// nBitsNotUsed times
					// - then | with leftover bits to form a 6 bit number
					nKeyIndex |= (bs[nSPos] << nBitsNotUsed);

					// since 6-nBitsNotUsed bits is used from next char,
					// 8-(6-nBitsNotUsed) will be left there
					nBitsNotUsed += 2;
				}
				// else
				// there's no next char, just use what we have left in the
				// current
				// pos (nSPos is now pointing pass the end of s)
			}

			else {
				// enough bits not used in current pos
				nBitsNotUsed -= 6; // we used 6 more bits from current pos

				if (nBitsNotUsed == 0) {
					// used up all the bits in current pos, so go to next
					nBitsNotUsed = 8;

					nSPos++;
				}
			}

			// use right most 6 bits as key index
			// & with 00111111(63) to take only the right most 6 bits
			nKeyIndex &= 63;

			// then use index to get a char from key to fill the blob
			sBlob[nBlobPos] = SSEncDec_GetCharFromKeyByIndex((byte) nKeyIndex);
			nBlobPos++;
		}

		String retval = "";
		try {
			retval = new String(sBlob, 0, nBlobPos,
					GLXPlayerConfig.Configuration.DEFAULT_ENCODING);
		} catch (Exception e) {
			System.out.println("Error while decoding!");
		}
		return retval;
	}

	private byte SSEncDec_GetCharFromKeyByIndex(byte nKeyIndex) {
		if (nKeyIndex < 26) {
			// key index 0-25 is a-z
			return (byte) (nKeyIndex + (byte) 97);
		}

		else if (nKeyIndex < 52) {
			// key index 26-51 is A-Z
			return (byte) (nKeyIndex + (byte) 39);
		}

		else if (nKeyIndex < 62) {
			// key index 52-61 is 0-9
			return (byte) ((byte) nKeyIndex - (byte) 4);
		}

		else if (nKeyIndex == 62) {
			// key index 62 is '_'
			return 95; // '_';
		}

		else {
			// key index 63 is '-'
			return 45; // '-';
		}

		// should never get here
		// DBGPRINTF("GetCharFromKeyByIndex error.");
		// return 0;
	}

	private void SendByGet(String requestBuffer) {
		System.out.println(requestBuffer);
		String request = String2Blob(requestBuffer);
		// lastErrorCode = XPlayer.Error.ERROR_INIT;

		if (GLXPlayerConfig.Configuration.ENABLE_TIMEOUT) {
			s_callstarttime = System.currentTimeMillis();
		}
		m_Connect_engine.sendByGet(URL, request);
	}

	private void SendByPost(String requestBuffer) {
		String request = String2Blob(requestBuffer);
		// lastErrorCode = XPlayer.Error.ERROR_INIT;

		if (GLXPlayerConfig.Configuration.ENABLE_TIMEOUT) {
			s_callstarttime = System.currentTimeMillis();
		}
		m_Connect_engine.sendByPost(URL, request);
	}

	/**
	 * Function SendLogin.
	 * <p>
	 * Login web server.
	 * <p>
	 * Tag--GLLIVE_LOGIN
	 * 
	 * @param name
	 * @param passWord
	 * @param type
	 * @return Success: |r|s / Fail: |r|e|error code
	 */
	public void SendLogin(String name, String passWord, int type) {

		m_requestBuffer.delete(0, m_requestBuffer.length());
		{
			if (type > 0)
				m_requestBuffer.append("f|").append(FUNCTION_LOGIN).append(
						"|i|").append(GGI).append("|u|").append(name).append(
						"|p|").append(passWord).append("|o|").append(type)
						.append("|n|1|");

			else
				m_requestBuffer.append("f|").append(FUNCTION_LOGIN).append(
						"|i|").append(GGI).append("|u|").append(name).append(
						"|p|").append(passWord).append("|");
		}
		m_password = passWord;
		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * Function SendLogout. Logout web server.
	 * 
	 * @return Success: |r|s / Fail: |r|e|error code
	 */
	public void SendLoginOut() {

		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_LOGOUT).append("|i|")
				.append(GGI).append("|u|").append(UID).append("|t|").append(
						m_keepAliveReturns).append("|");

		SendByGet(m_requestBuffer.toString());

	}

	/**
	 * Get User information.
	 */
	public void SendGetUserInfo() {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_GET_USER_INFO).append(
				"|i|").append(GGI).append("|u|").append(UID).append("|");
		System.out.println("sendGetUserInfo");
		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * Register a new account in web server
	 * <p>
	 * Tag--GLLIVE_REGISTER
	 * 
	 * @param userName
	 * @param password
	 * @param nation
	 * @param email
	 * @param emailFlag
	 */
	public void SendRegister(String userName, String password, String nation,
			String email, String emailFlag) {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_REGISTER).append("|i|")
				.append(GGI).append("|u|").append(userName).append("|p|")
				.append(password).append("|e|").append(email).append("|c|")
				.append(nation).append("|ef|").append(emailFlag).append("|");
		System.out.println("sendRegister");
		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * change user name
	 * <p>
	 * Tag--GLLIVE_CHANGE_USERNAME
	 * 
	 * @param newUserName
	 */
	public void SendChangeUserName(String newUserName) {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_CHANGE_USERNAME).append(
				"|i|").append(GGI).append("|u|").append(m_userName).append(
				"|p|").append(m_password).append("|nu|").append(newUserName)
				.append("|");
		System.out.println("sendChangeUserName");
		SendByGet(m_requestBuffer.toString());

	}

	/**
	 * change password
	 * <P>
	 * Tag--GLLIVE_CHANGE_PASSWORD
	 * 
	 * @param newPassWord
	 * @param passwordAgain
	 */
	public void SendChangePassword(String newPassWord, String passwordAgain) {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_CHANGE_PASSWORD).append(
				"|i|").append(GGI).append("|u|").append(m_userName).append(
				"|p|").append(m_password).append("|np|").append(newPassWord)
				.append("|vp|").append(passwordAgain).append("|");
		System.out.println("sendChangePassword");
		m_changePassword = newPassWord;
		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * change email
	 * <p>
	 * Tag--GLLIVE_CHANGE_EMAIL
	 * 
	 * @param newEmail
	 */
	public void SendChangeEmail(String newEmail) {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_CHANGE_EMAIL)
				.append("|i|").append(GGI).append("|u|").append(m_userName)
				.append("|p|").append(m_password).append("|ne|").append(
						newEmail).append("|");
		System.out.println("sendChangeEmail");

		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * change email receive flag from gameloft.com
	 * <p>
	 * Tag--GLLIVE_CHANGE_EMFLAG
	 * 
	 * @param newEmailFlag
	 */
	public void SendChangeEmailFlag(String newEmailFlag) {
		m_requestBuffer.delete(0, m_requestBuffer.length());
		if (!newEmailFlag.equals("1"))
			newEmailFlag = "0";
		m_requestBuffer.append("f|").append(FUNCTION_CHANGE_EMAILFLAG).append(
				"|i|").append(GGI).append("|u|").append(UID).append(
				"|email_flag|").append(newEmailFlag).append("|");
		System.out.println("sendChangeEmailFlag");

		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * change country
	 * <p>
	 * Tag--GLLIVE_CHANGE_NATION
	 * 
	 * @param newCountry
	 */
	public void SendChangeCountry(String newCountry) {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_CHANGE_COUNTRY).append(
				"|i|").append(GGI).append("|u|").append(m_userName).append(
				"|p|").append(m_password).append("|nc|").append(newCountry)
				.append("|");
		System.out.println("sendChangeCountry");

		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * upload the file to the web server
	 * <p>
	 * Tag--GLLIVE_CHANGE_AVATAR
	 * 
	 * @param file
	 */
	public void SendUpLoadFile(byte[] file) {
		// String base64=encode(file);
		String base64 = ByteArrayToBase64(file);

		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_UPLOAD_USER_AVATAR)
				.append("|i|").append(GGI).append("|u|").append(UID).append(
						"|d|").append(base64).append("|");
		System.out.println("SendUpLoadFile");

		SendByPost(m_requestBuffer.toString());
	}

	/**
	 * get Avatar from web server
	 */
	public void SendGetUserAvatar() {
		isDownloadingFile = true;
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_GET_USER_AVATAR).append(
				"|i|").append(GGI).append("|u|").append(UID).append("|");
		System.out.println("SendGetUserAvatar");

		SendByGet(m_requestBuffer.toString());

	}

	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// BEGIN FRIEND
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	/**
	 * Private function processFriendsList. Process friend list if success
	 * message returned from the server.
	 * 
	 * @see sendGetUserFriends()
	 * @param data
	 *            Data returned from the server.
	 */
	private void processFriendsList(String data) {
		clearFriendsList();

		if (data == null || data.length() <= 0) {
			return;
		}

		String tmp = "";
		String num = "";
		tmp = GetValue(data, 5, '|');
		m_friendsCount = Integer.parseInt(tmp);

		m_reputation = new String[m_friendsCount];
		m_state = new int[m_friendsCount];
		m_trophy = new String[m_friendsCount];

		m_usernum = new String[m_friendsCount];
		m_name = new String[m_friendsCount];
		m_nationality = new String[m_friendsCount];
		m_current_game = new String[m_friendsCount];
		m_totalTrophy=new String[m_friendsCount]; 
		int count = 7;
		for (int i = 0; i < m_friendsCount; i++) {
			tmp = GetValue(data, count++, '|');
			m_usernum[i] = GetValue(tmp, 0, '^');
			m_name[i] = GetValue(tmp, 1, '^');

			m_nationality[i] = GetValue(tmp, 2, '^');

			m_reputation[i] = GetValue(tmp, 3, '^');
			
			
			m_totalTrophy[i] = GetValue(tmp, 4, '^');
			num = GetValue(tmp, 5, '^');
			if (num != "")
				m_state[i] = Integer.parseInt(num);

			m_current_game[i] = GetValue(tmp, 6, '^');

			m_trophy[i] = GetValue(tmp, 7, '^');
			

		}
	}

	/**
	 * Private function clearFriendsList. Clear user's friend list.
	 */
	private void clearFriendsList() {
		initFriend();
	}

	/**
	 * Variable m_friendsCount. Store the number of friend in the list.
	 */
	private int m_friendsCount;
	/**
	 * Variable m_usernum. Store the usernum list.
	 */
	private String[] m_usernum;
	/**
	 * Variable m_name. Store the name list.
	 */
	private String[] m_name;

	/**
	 * Variable m_name. Store the country list.
	 */
	private String[] m_nationality;

	/**
	 * Variable m_reputation. Store the reputation list .
	 */
	private String[] m_reputation;

	/**
	 * Variable m_state. Store the state list .
	 */
	private int[] m_state;

	/**
	 * Variable m_current_game. Store the current game list .
	 */
	private String[] m_current_game;

	/**
	 * Variable m_trophy. Store the trophy list .
	 */
	private String[] m_trophy;
	/**
	 * Variable m_trophy. Store the trophy list .
	 */
	private String[] m_totalTrophy;

	/**
	 * init members for friend
	 */
	private void initFriend() {
		m_friendsCount = 0;
		m_name = null;
		m_state = null;
		m_nationality = null;
		m_reputation = null;
		m_current_game = null;
		m_trophy = null;

	}

	private void disposeFriend() {
		clearFriendsList();
	}

	/**
	 * Function sendAddUserFriend. Add a friend and wait for friend
	 * confirmation.
	 * 
	 * @param userName
	 *            Name of user to add as a friend.
	 * @param msg
	 *            Option parameter. Invitation message.
	 * @return Success: |r|s / Fail: |r|e|error code
	 */
	public void sendAddUserFriend(String userName, String message) {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_ADD_USER_FRIEND).append(
				"|i|").append(GGI).append("|u|").append(UID).append("|n|")
				.append(userName).append("|");
		if (message != null) {
			m_requestBuffer.append("m|").append(message).append("|");
		}
		System.out.println("sendAddUserFriend");

		SendByGet(m_requestBuffer.toString());

	}

	/**
	 * Function sendConfirmUserFriend. Allow or refuse to add current user as a
	 * friend.
	 * 
	 * @param userNum
	 *            Name of user to add as a friend.
	 * @param confirmValue
	 *            1: Allow, 0: Refuse.
	 * @see sendAddUserFriend()
	 * @return Success: |r|s / Fail: |r|e|error code
	 */
	void sendConfirmUserFriend(String userNum, int confirmValue) {
		if (confirmValue != 0) {
			confirmValue = 1;
		}
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_CONFIRM_USER_FRIEND)
				.append("|i|").append(GGI).append("|u|").append(UID).append(
						"|un|").append(userNum).append("|c|").append(
						confirmValue).append("|");

		System.out.println("sendConfirmUserFriend");

		SendByGet(m_requestBuffer.toString());

	}

	/**
	 * Function sendDelUserFriend. Delete one of my friend.
	 * 
	 * @param userFriendNum
	 *            Name of user to delete.
	 * @return Success: |r|s / Fail: |r|e|error code
	 */
	void sendDelUserFriend(String userFriendNum) {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_DEL_USER_FRIEND).append(
				"|i|").append(GGI).append("|u|").append(UID).append("|un|")
				.append(userFriendNum).append("|");

		System.out.println("sendDelUserFriend");

		SendByGet(m_requestBuffer.toString());

	}

	/**
	 * Function sendGetUserFriends. Get user's friend list.
	 * 
	 * @param startIndex
	 *            Start index to read..
	 * @param count
	 *            Read count,0 is no limit.
	 * @see processFriendsList()
	 * @return Success: |r|s|c|total friends count|l|friend list / Fail:
	 *         |r|e|error code
	 */
	void sendGetUserFriends(int startIndex, int count) {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_GET_USER_FRIENDS).append(
				"|i|").append(GGI).append("|u|").append(UID).append("|a|")
				.append(startIndex).append("|c|").append(count).append("|");

		System.out.println("sendGetUserFriends");

		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * Function sendGetUserFriendsCount. Get user's friends count.
	 * 
	 * @return Success: |r|s|friends count / Fail: |r|e|error code
	 */
	void sendGetUserFriendsCount() {

		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_GET_USER_FRIENDS_COUNT)
				.append("|i|").append(GGI).append("|u|").append(UID)
				.append("|");

		System.out.println("sendGetUserFriends");

		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * Function GetFriendsCount. Return number of friend in current friends
	 * list.
	 * 
	 * @see sendGetUserFriendsCount()
	 * @see processFriendsList()
	 * @return m_friendsCount
	 */
	int GetFriendsCount() {
		return m_friendsCount;
	}

	/**
	 * Function GetFriendUserNumber. Return usernum of friend in current friends
	 * list, indicated by position.
	 * 
	 * @param position
	 *            Position of friend in the list.
	 * @see sendGetUserFriends()
	 * @see processFriendsList()
	 * @return Friend name at the position indicated.
	 */
	String GetFriendUserNumber(int position) {
		if (m_usernum == null || position < 0 || position > m_friendsCount) {
			return null;
		}
		return m_usernum[position];

	}

	/**
	 * Function GetName. Return name of friend in current friends list,
	 * indicated by position.
	 * 
	 * @param position
	 *            Position of friend in the list.
	 * @see sendGetUserFriends()
	 * @see processFriendsList()
	 * @return Friend name at the position indicated.
	 */
	String GetFriendName(int position) {
		if (m_name == null || position < 0 || position > m_friendsCount) {
			return null;
		}
		return m_name[position];

	}

	/**
	 * Function GetCountry. Return country of friend in current friends list,
	 * indicated by position.
	 * 
	 * @param position
	 *            Position of friend in the list.
	 * @see sendGetUserFriends()
	 * @see processFriendsList()
	 * @return Friend country at the position indicated.
	 */
	String GetFriendCountry(int position) {
		if (m_nationality == null || position < 0 || position > m_friendsCount) {
			return null;
		}
		return m_nationality[position];
	}

	/**
	 * Function GetReputation. Return reputation of friend in current friends
	 * list, indicated by position.
	 * 
	 * @param position
	 *            Position of friend in the list.
	 * @see sendGetUserFriends()
	 * @see processFriendsList()
	 * @return Friend reputation at the position indicated.
	 */
	String GetFriendReputation(int position) {
		if (m_reputation == null || position < 0 || position > m_friendsCount) {
			return null;
		}

		return m_reputation[position];
	}

	/**
	 * Function GetFriendStateInfo. Return state info of friend in current
	 * friends list, indicated by position.
	 * 
	 * @param position
	 *            Position of friend in the list.
	 * @see sendGetUserFriends()
	 * @see processFriendsList()
	 * @return Friend state at the position indicated.
	 */
	String GetFriendStateInfo(int position) {
		if (m_state == null || position < 0 || position > m_friendsCount) {
			return null;
		}
		int state = m_state[position];
		if (state == XPLAYER_USER_STATE_ONLINE)
			return "OnLine";
		else if (state == XPLAYER_USER_STATE_PLAYING) {
			return "playing " + GetFriendCurrentGame(position);
		} else
			return "OffLine";
	}

	/**
	 * Function GetState. Return state of friend in current friends list,
	 * indicated by position.
	 * 
	 * @param position
	 *            Position of friend in the list.
	 * @see sendGetUserFriends()
	 * @see processFriendsList()
	 * @return Friend state at the position indicated.
	 */
	int GetFriendState(int position) {
		if (m_state == null || position < 0 || position > m_friendsCount) {
			return -1;
		}
		return m_state[position];

	}

	/**
	 * Function GetCurrentGame. Return current game of friend in current friends
	 * list, indicated by position.
	 * 
	 * @param position
	 *            Position of friend in the list.
	 * @see sendGetUserFriends()
	 * @see processFriendsList()
	 * @return Friend current game at the position indicated.
	 */
	String GetFriendCurrentGame(int position) {
		if (m_current_game == null || position < 0 || position > m_friendsCount) {
			return null;
		}
		return m_current_game[position];
	}

	/**
	 * Function GetTrophy. Return trophy of friend in current friends list,
	 * indicated by position.
	 * 
	 * @param position
	 *            Position of friend in the list.
	 * @see sendGetUserFriends()
	 * @see processFriendsList()
	 * @return Friend trophy game at the position indicated.
	 */
	String GetFriendTrophy(int position) {
		if (m_trophy == null || position < 0 || position > m_friendsCount) {
			return null;
		}
		return m_trophy[position];
	}

	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// END FRIEND
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////

	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// BEGIN MESSAGE
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	/**
	 * Private function clearMessageList. Clear message list.
	 */
	private void clearMessageList() {

		m_msgId = null;
		m_msgType = null;
		m_msgReadState = null;
		m_msgTitle = null;
		m_msgSender = null;
		m_msgSenderNum = null;
		m_msgCreateTime = null;
		m_MsgListCount = 0;
	}

	/**
	 * Private function clearMsgCount. Reset message count.
	 */
	private void clearMsgCount() {
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 3; j++) {
				m_msgCount[i][j] = 0;
			}
		}
	}

	/**
	 * Variable m_msgId. List of message ID.
	 */
	private int[] m_msgId;

	/**
	 * Variable m_msgTitle. List of message title.
	 */
	private String[] m_msgTitle;

	/**
	 * Variable m_msgSender. List of message sender.
	 */
	private String[] m_msgSender;
	/**
	 * Variable m_msgSenderNum. List of message senderNum.
	 */
	private String[] m_msgSenderNum;

	/**
	 * Variable m_msgCreateTime. List of message created time.
	 */
	private String[] m_msgCreateTime;

	/**
	 * Variable m_msgReadState. List of message read state.
	 */
	private boolean[] m_msgReadState;

	/**
	 * Variable m_msgType. List of message type.
	 */
	private int[] m_msgType;

	/**
	 * Variable m_MsgListCount. Count number of message in the list.
	 */
	private int m_MsgListCount;

	/**
	 * Variable m_currentMsgID. Current message ID to read.
	 */
	private int m_currentMsgID;
	/**
	 * Variable m_currentMsgID. Current message Pos in list to read.
	 */
	private int m_currentMsgPos;

	/**
	 * Variable m_msgBody. Current message body.
	 */
	private String m_msgBody;

	/**
	 * Variable m_msgCount. Array to store message count. Currently, we have 5
	 * type and 3 read type.
	 * 
	 * @see SendGetMessageCount()
	 * @see processMsgCount()
	 * @see getMsgCount()
	 */
	private int m_msgCount[][] = new int[5][3];

	/**
	 * init message
	 */
	void initMessage() {

		m_msgId = null;
		m_msgType = null;
		m_msgTitle = null;
		m_msgSender = null;
		m_msgSenderNum = null;
		m_msgCreateTime = null;
		m_msgReadState = null;
		m_msgBody = null;
		m_MsgListCount = 0;
		m_currentMsgID = -1;
		m_currentMsgPos = -1;
		clearMsgCount();
	}

	void disposeMessage() {

		clearMessageList();
		m_msgBody = null;

	}

	/**
	 * Function SendGetMessageCount. Get user's messages count.
	 * 
	 * @return Success: |r|s|messageType^totalCount^newCount^readedCount|... /
	 *         Fail: |r|e|error code
	 */
	void SendGetMessageCount() {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_GET_MESSAGE_COUNT).append(
				"|i|").append(GGI).append("|u|").append(UID).append("|");
		System.out.println("SendGetMessageCount");
		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * Function SendGetMessageHeader. Get user's message headers list.
	 * 
	 * @param msgType
	 *            Message type: Game message, User message.
	 * @param readType
	 *            Message read type: All, New, Read.
	 * @param startIndex
	 *            Start index to read.
	 * @param count
	 *            Read count, 0 is no limit.
	 * @return Success: |r|s|c|total count|l|message headers list / Fail:
	 *         |r|e|error code
	 */
	void SendGetMessageHeader(int msgType, int readType, int startIndex,
			int count) {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_GET_MESSAGE_HEADER)
				.append("|i|").append(GGI).append("|u|").append(UID).append(
						"|t|");
		if ((msgType & USER_MESSAGE_TYPE_GAMEMESSAGE) != 0) {
			m_requestBuffer.append(USER_MESSAGE_TYPE_GAMEMESSAGE).append(",");
		}
		if ((msgType & USER_MESSAGE_TYPE_USERMESSAGE) != 0) {
			m_requestBuffer.append(USER_MESSAGE_TYPE_USERMESSAGE).append(",");
		}
		if ((msgType & USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION) != 0) {
			m_requestBuffer.append(
					USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION).append(",");
		}
		if ((msgType & USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION) != 0) {
			m_requestBuffer
					.append(USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION)
					.append(",");
		}
		if ((msgType & USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION) != 0) {
			m_requestBuffer
					.append(USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION)
					.append(",");
		}

		if (m_requestBuffer.charAt(m_requestBuffer.length() - 1) == ',') {
			m_requestBuffer.deleteCharAt(m_requestBuffer.length() - 1);
		}

		m_requestBuffer.append("|r|").append(readType).append("|a|").append(
				startIndex).append("|c|").append(count).append("|");
		System.out.println("SendGetMessageHeader");
		SendByGet(m_requestBuffer.toString());

	}

	/**
	 * Function SendReadMessage. Read user's messages by messageId.
	 * 
	 * @param msgID
	 *            Id of message to read.
	 * @return Success: |r|s|message body / Fail: |r|e|error code
	 */
	void SendReadMessage(int msgID) {
		m_currentMsgID = msgID;
		m_currentMsgPos = -1;
		for (int i = 0; i < getMsgListCount(); i++) {
			if (getMsgID(i) == m_currentMsgID) {
				m_currentMsgPos = i;
			}
		}

		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_READ_MESSAGE)
				.append("|i|").append(GGI).append("|u|").append(UID).append(
						"|m|").append(msgID).append("|");
		System.out.println("SendReadMessage");
		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * Function SendOnlineMessage. Send message to another user. Can not send
	 * GameMessage.
	 * 
	 * @param receiverNum
	 *            num of receiver.
	 * @param type
	 *            User message, Invite message.
	 * @param title
	 *            Subject of the message.
	 * @param body
	 *            Content of the message.
	 * @param body_length
	 *            length of message body.
	 * @return Success: |r|s / Fail: |r|e|error code
	 */
	void SendOnlineMessageByNum(String receiverNum, int type, String title,
			String body, int body_length) {

		if (receiverNum == null || title == null || body == null
				|| body_length == 0 || type == USER_MESSAGE_TYPE_GAMEMESSAGE) {
			m_observer.OnRequestFailure(FUNCTION_SEND_MESSAGE, -100, null);
			return;
		}
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_SEND_MESSAGE)
				.append("|i|").append(GGI).append("|u|").append(UID).append(
						"|t|").append(type).append("|l|").append(title).append(
						"|o|").append(body).append("|un|").append(receiverNum)
				.append("|");
		System.out.println("SendOnlineMessage");
		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * Function SendOnlineMessage. Send message to another user. Can not send
	 * GameMessage.
	 * 
	 * @param receiverNum
	 *            num of receiver.
	 * @param type
	 *            User message, Invite message.
	 * @param title
	 *            Subject of the message.
	 * @param body
	 *            Content of the message.
	 * @param body_length
	 *            length of message body.
	 * @return Success: |r|s / Fail: |r|e|error code
	 */
	void SendOnlineMessageByName(String receiver, int type, String title,
			String body, int body_length) {

		if (receiver == null || title == null || body == null
				|| body_length == 0 || type == USER_MESSAGE_TYPE_GAMEMESSAGE) {
			m_observer.OnRequestFailure(FUNCTION_SEND_MESSAGE, -100, null);
			return;
		}
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_SEND_MESSAGE)
				.append("|i|").append(GGI).append("|u|").append(UID).append(
						"|t|").append(type).append("|l|").append(title).append(
						"|o|").append(body).append("|n|").append(receiver)
				.append("|");
		System.out.println("SendOnlineMessage");
		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * Function SendDeleteMessage. Delete user's messag indicated by msgID.
	 * Can't delete the message which sended to ALL.
	 * 
	 * @param msgID
	 *            Id of message to be deleted.
	 * @return Success: |r|s / Fail: |r|e|error code
	 */
	void SendDeleteMessage(int msgID) {
		m_requestBuffer.delete(0, m_requestBuffer.length());

		m_requestBuffer.append("f|").append(FUNCTION_DEL_MESSAGE).append("|i|")
				.append(GGI).append("|u|").append(UID).append("|m|").append(
						msgID).append("|");
		System.out.println("SendDeleteMessage");
		SendByGet(m_requestBuffer.toString());
	}

	/**
	 * Function getMsgListCount. Count messages in the message list.
	 * 
	 * @return Number of messages in the message list.
	 */
	int getMsgListCount() {
		return m_MsgListCount;
	}

	/**
	 * Function getMsgCount. Count messages in the message list by message type
	 * and read type.
	 * 
	 * @param msgType
	 *            Message type: Game message, User message.
	 * @param readType
	 *            Message read type: All, New, Read..
	 * @return Number of messages.
	 */
	int getMsgCount(int msgType, int readType) {
		if (readType < 0 || readType >= 3) {
			return -1;
		}

		if (msgType == USER_MESSAGE_TYPE_GAMEMESSAGE) {
			return m_msgCount[0][readType];
		} else if (msgType == USER_MESSAGE_TYPE_USERMESSAGE) {
			return m_msgCount[1][readType];
		} else if (msgType == USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION) {
			return m_msgCount[2][readType];
		} else if (msgType == USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION) {
			return m_msgCount[3][readType];
		} else if (msgType == USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION) {
			return m_msgCount[4][readType];
		}

		return -1;
	}

	/**
	 * Function getMsgID. Return ID of message in the message list, indicated by
	 * position.
	 * 
	 * @param position
	 *            Index of message in the list.
	 * @return Message ID.
	 */
	int getMsgID(int position) {
		if (m_msgId == null) {
			return -1;
		}
		return m_msgId[position];
	}

	/**
	 * Function getMsgType. Return type of message in the message list,
	 * indicated by position.
	 * 
	 * @param position
	 *            Index of message in the list.
	 * @return Message type.
	 */
	int getMsgType(int position) {
		if (m_msgType == null) {
			return -1;
		}
		return m_msgType[position];
	}

	/**
	 * Function getMsgTitle. Return subject of message in the message list,
	 * indicated by position.
	 * 
	 * @param position
	 *            Index of message in the list.
	 * @return Message title.
	 */
	String getMsgTitle(int position) {
		if (m_msgTitle == null) {
			return null;
		}
		return m_msgTitle[position];
	}

	/**
	 * Function getMsgSenderNum. Return the senderNum of message in the message
	 * list, indicated by position.
	 * 
	 * @param position
	 *            Index of message in the list.
	 * @return Message sender.
	 */
	String getMsgSenderNum(int position) {
		if (m_msgSenderNum == null) {
			return null;
		}
		return m_msgSenderNum[position];
	}

	/**
	 * Function getMsgSender. Return the sender of message in the message list,
	 * indicated by position.
	 * 
	 * @param position
	 *            Index of message in the list.
	 * @return Message sender.
	 */
	String getMsgSender(int position) {
		if (m_msgSender == null) {
			return null;
		}
		return m_msgSender[position];
	}

	/**
	 * Function getMsgCreatedTime. Return the time created of message in the
	 * message list, indicated by position.
	 * 
	 * @param position
	 *            Index of message in the list.
	 * @return Message's created time.
	 */
	String getMsgCreatedTime(int position) {
		if (m_msgCreateTime == null) {
			return null;
		}
		return m_msgCreateTime[position];
	}

	/**
	 * Function getMsgReadState. Return state (readed or not) of message in the
	 * message list, indicated by position.
	 * 
	 * @param position
	 *            Index of message in the list.
	 * @return Message read state.
	 */
	boolean getMsgReadState(int position) {
		if (m_msgReadState == null) {
			return false;
		}
		return m_msgReadState[position];
	}

	void setMsgReadState(int position) {
		if (m_msgReadState == null) {
			return;
		}
		m_msgReadState[position] = true;
	}

	/**
	 * Function getCurrentMsgID. Return current message ID.
	 * 
	 * @return current message ID.
	 */
	int getCurrentMsgID() {
		return m_currentMsgID;
	}

	/**
	 * Function getCurrentMsgPosition. Return current message position in the
	 * list.
	 * 
	 * @return current message position.
	 */
	int getCurrentMsgPosition() {
		return m_currentMsgPos;

	}

	/**
	 * Function getMsgBody. Return current message body.
	 * 
	 * @return current message body
	 */
	String getMsgBody() {
		return m_msgBody;
	}

	/**
	 * Private function processMsgHeaderList. Process message header list if
	 * success message returned from the server.
	 * 
	 * @see SendGetMessageHeader()
	 * @param msgString
	 *            Data returned from the server.
	 */
	void processMsgHeaderList(String msgString) {
		clearMessageList();
		/*
		 * success:f|75|r|s|c|total count|l|message headers list fail:
		 * f|75|r|e|error code
		 * 
		 * Message headers's format is: 1st messageId^1st message type^1st
		 * title^1st source(from who)^1st createTime(UTC time)^1st readState|2nd
		 * messageId^2nd message type^2nd title^2nd source(from who)^2nd
		 * createTime(UTC time)^2nd readState
		 */
		if (msgString == null || msgString.length() <= 0) {
			return;
		}
		String temp = GetValue(msgString, 5, '|');
		if (temp == null)
			return;
		m_MsgListCount = Integer.parseInt(temp);

		m_msgId = new int[m_MsgListCount];
		m_msgType = new int[m_MsgListCount];
		m_msgReadState = new boolean[m_MsgListCount];

		m_msgTitle = new String[m_MsgListCount];
		m_msgSender = new String[m_MsgListCount];
		m_msgSenderNum = new String[m_MsgListCount];
		m_msgCreateTime = new String[m_MsgListCount];

		String tempString;
		for (int i = 0; i < m_MsgListCount; i++) {
			temp = GetValue(msgString, 7 + i, '|');
			tempString = GetValue(temp, 0, '^');
			if (tempString != null)
				m_msgId[i] = Integer.parseInt(tempString);

			tempString = GetValue(temp, 1, '^');
			if (tempString != null)
				m_msgType[i] = Integer.parseInt(tempString);

			m_msgTitle[i] = GetValue(temp, 2, '^');
			m_msgSender[i] = GetValue(temp, 3, '^');
			m_msgSenderNum[i] = GetValue(temp, 4, '^');
			m_msgCreateTime[i] = GetValue(temp, 5, '^');

			tempString = GetValue(temp, 6, '^');
			int rtmp = 0;
			if (tempString != null)
				rtmp = Integer.parseInt(tempString);
			if (rtmp != 0) {
				m_msgReadState[i] = true;
			} else {
				m_msgReadState[i] = false;
			}
		}

	}

	/**
	 * Private function processMsgCount. Process message count if success
	 * message returned from the server.
	 * 
	 * @see SendGetMessageCount()
	 * @param msgString
	 *            Data returned from the server.
	 */
	public void processMsgCount(String msgString) {
		clearMsgCount();
		if (msgString == null || msgString.length() < 0) {
			return;
		}
		// /@Return: f|74|r|s|0^2^0^0|1^0^0^0|2^0^0^0|3^0^0^0|4^0^0^0

		String temp;
		String tempNum;
		for (int i = 0; i < 5; i++) {
			temp = GetValue(msgString, i + 4, '|');
			for (int j = 0; j < 3; j++) {
				tempNum = GetValue(temp, j + 1, '^');

				m_msgCount[i][j] = Integer.parseInt(tempNum);
			}
		}
	}

	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// END MESSAGE
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////////
	/**
	 * adamian: moved to private access... find the 'pos' value in souce string ..
	 * values are separated by '|'
	 * 
	 * @param src
	 * @param pos
	 * @return
	 */
	private String GetValue(String src, int pos, char spit) {
		int start = 0;
		int ini_pos = pos;
		int end = src.indexOf(spit, start + 1);

		while (pos > 0) {
			if (start == -1) {
				return null;
			}

			start = end;
			end = src.indexOf(spit, start + 1);
			pos--;
		}

		if (start == -1) {
			return null;
		}

		if (end == -1) {
			end = src.length();
		}

		if (ini_pos > 0) {
			start++;
		}

		if (start == end) {
			// empty ..
			return "";
		}

		if (start > end) {
			return null;
		}

		try {
			// dnay - the substring function is not reliable on some phones and
			// even emulators. Do it the old way.
			// String value = src.substring(start, end);
			char[] buf = new char[end - start];
			src.getChars(start, end, buf, 0);
			String value = new String(buf);
			return value;
		} catch (IndexOutOfBoundsException e) {
			return null;
		}
	}

	/**
	 * Private function OnUpdateSuccess. Handle success message returned from
	 * the Web server.
	 * 
	 * @param funId
	 * @param data
	 */
	private void OnUpdateSuccess(int funId, String data) {

		switch (funId) {
		case FUNCTION_GET_MESSAGE_COUNT: {
			processMsgCount(data);
			m_observer.OnRequestSuccess(funId, null);
		}
			break;
		case FUNCTION_GET_MESSAGE_HEADER: {
			processMsgHeaderList(data);
			m_observer.OnRequestSuccess(funId, null);
		}
			break;
		case FUNCTION_READ_MESSAGE: {
			m_msgBody = null;
			m_msgBody = GetValue(data, 4, '|');
			m_observer.OnRequestSuccess(funId, m_msgBody);
		}
			break;

		case FUNCTION_GET_USER_FRIENDS_COUNT: {
			String tmpHR = GetValue(data, 4, '|');
			if (tmpHR != null)
				m_friendsCount = Integer.parseInt(tmpHR);
			m_observer.OnRequestSuccess(funId, null);
		}
			break;
		case FUNCTION_GET_USER_FRIENDS:
			processFriendsList(data);
			m_observer.OnRequestSuccess(funId, null);
			break;
		case FUNCTION_REGISTER: {
			m_observer.OnRequestSuccess(funId, null);

		}
			break;
		case FUNCTION_LOGIN: {
			String tmpHR = GetValue(data, 4, '|');
			if (tmpHR.compareTo("u") != 0) {
				m_observer.OnRequestFailure(funId, ERROR_BAD_RESPONSE, null);
				return;
			}
			UID = GetValue(data, 5, '|');

			if (GLXPlayerConfig.Configuration.ENABLE_KEEP_ALIVE_WITH_WEB) {
				tmpHR = GetValue(data, 6, '|');
				if (tmpHR.compareTo("t") != 0) {
					m_observer
							.OnRequestFailure(funId, ERROR_BAD_RESPONSE, null);
					return;
				}
				tmpHR = GetValue(data, 7, '|');
				m_keepAliveReturns = Integer.parseInt(tmpHR);
			}
			m_userName = GetValue(data, 9, '|');
			m_isLoggedIn = true;
			m_observer.OnRequestSuccess(funId, null);

		}
			break;
		case FUNCTION_KEEP_ALIVE: {
			String tmpHR = GetValue(data, 5, '|');
			m_keepAliveReturns = Integer.parseInt(tmpHR);
		}
			break;
		case FUNCTION_LOGOUT: {
			m_isLoggedIn = false;
			Cancel();
			m_observer.OnRequestSuccess(funId, null);
		}
			break;
		case FUNCTION_CHANGE_PASSWORD:
			m_password = m_changePassword;
			m_observer.OnRequestSuccess(funId, null);
			break;
		case FUNCTION_GET_USER_INFO: {
			// f|81|r|s|n|xxxx|username|Tom|email|tom@gameloft.com|email_flag|0|
			// country|CN|reputation|1.3|firstname|Tom|lastname|Jackson|sex|1|
			// birthday|1970-1-1
			m_userNum = GetValue(data, 5, '|');
			m_userName = GetValue(data, 7, '|');
			m_myEmail = GetValue(data, 9, '|');
			m_myEmailflag = GetValue(data, 11, '|');
			m_myCountry = GetValue(data, 13, '|');
			m_myReputation = GetValue(data, 15, '|');
			m_firstname = GetValue(data, 17, '|');
			m_lastname = GetValue(data, 19, '|');
			m_mySex = GetValue(data, 21, '|');
			m_myBirthday = GetValue(data, 23, '|');
			m_myTrophy = GetValue(data, 27, '|');

			m_observer.OnRequestSuccess(funId, null);

		}
			break;
		case FUNCTION_GET_USER_AVATAR: {

			String tmpHR = GetValue(data, 4, '|');

			if (tmpHR.compareTo("n") == 0) {
				tmpHR = GetValue(data, 6, '|');
			}
			// byte[] base64=decode(tmpHR);
			byte[] base64 = Base64ToByteArray(tmpHR);
			m_observer.OnRequestSuccessByAray(funId, base64);
		}
			break;
		default:
			m_observer.OnRequestSuccess(funId, null);
		}

	}

	/**
	 * Private function OnUpdateSuccess. Handle failure message returned from
	 * the Web server.
	 * 
	 * @param funId
	 * @param data
	 */
	private void OnUpdateFailure(int funId, String data) {
		String code = GetValue(data, 4, '|');
		switch (funId) {

		case FUNCTION_REGISTER:

			m_observer.OnRequestFailure(funId, Integer.parseInt(code),
					GetValue(data, 6, '|'));
			break;
		case FUNCTION_CHANGE_USERNAME:

			m_observer.OnRequestFailure(funId, Integer.parseInt(code),
					GetValue(data, 6, '|'));
			break;
		default:
			m_observer.OnRequestFailure(funId, Integer.parseInt(code), null);
			break;
		}

	}

	public void update() {
		KeepAlive();
		HandleConnect();
	}

	// Define for Message Read Type, using for Message Functions
	public static final int USER_MESSAGE_READTYPE_ALL = 0;
	public static final int USER_MESSAGE_READTYPE_NEW = 1;
	public static final int USER_MESSAGE_READTYPE_READED = 2;

	public static final int USER_MESSAGE_TYPE_GAMEMESSAGE = 1;
	public static final int USER_MESSAGE_TYPE_USERMESSAGE = 1 << 1;
	public static final int USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION = 1 << 2;
	public static final int USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION = 1 << 3;
	public static final int USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION = 1 << 4;
	public static final int XPLAYER_USER_STATE_ONLINE = 2;
	public static final int XPLAYER_USER_STATE_PLAYING = 3;

	public static final int FUNCTION_REGISTER = 11;
	public static final int FUNCTION_CHANGE_USERNAME = 14;
	public static final int FUNCTION_LOGIN = 15;
	public static final int FUNCTION_KEEP_ALIVE = 16;
	public static final int FUNCTION_LOGOUT = 17;
	public static final int ERROR_BAD_RESPONSE = 40;
	public static final int FUNCTION_CHANGE_PASSWORD = 50;
	public static final int FUNCTION_CHANGE_EMAIL = 51;
	public static final int FUNCTION_CHANGE_COUNTRY = 54;
	public static final int FUNCTION_GET_USER_AVATAR = 70;
	public static final int FUNCTION_UPLOAD_USER_AVATAR = 71;

	public static final int FUNCTION_GET_USER_INFO = 81;
	public static final int FUNCTION_ADD_USER_FRIEND = 59;
	public static final int FUNCTION_CONFIRM_USER_FRIEND = 62;
	public static final int FUNCTION_DEL_USER_FRIEND = 60;
	public static final int FUNCTION_GET_USER_FRIENDS = 61;
	public static final int FUNCTION_GET_USER_FRIENDS_COUNT = 73;
	public static final int FUNCTION_CHANGE_EMAILFLAG = 82;
	public static final int FUNCTION_UPDATE_USER_INFO = 82;
	public static final int FUNCTION_GET_MESSAGE_COUNT = 74;
	public static final int FUNCTION_GET_MESSAGE_HEADER = 75;
	public static final int FUNCTION_READ_MESSAGE = 76;
	public static final int FUNCTION_SEND_MESSAGE = 77;
	public static final int FUNCTION_DEL_MESSAGE = 78;
}
