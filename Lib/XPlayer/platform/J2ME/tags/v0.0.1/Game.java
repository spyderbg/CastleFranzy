import javax.microedition.lcdui.*;


import java.io.IOException;
import java.io.InputStream;

public class Game extends Canvas implements Runnable, IGLXPlayerHttpObserver {
	// @div begin #Game_constants.h

	public final static int GS_LOGO = 0;
	public final static int GS_SPLASH = 1;
	public final static int GS_SOUND_CONFIRM = 2;
	public final static int GS_MAIN_MENU = 3;
	public final static int GS_OPTIONS_MENU = 4;
	public final static int GS_PAUSE_MENU = 5;
	public final static int GS_RUN = 6;
	public final static int GS_EXIT_CONFIRM = 7;
	public final static int GS_ABOUT = 8;
	public final static int GS_LOADING = 9;
	public final static int GS_MAIN_MENU_CONFIRM = 10;
	public final static int GS_JABBER = 11;
	public final static int EXIT_FORM_PAUSE = 1;
	public final static int EXIT_FORM_MAIN_MENU = 0;
	private static final String[] s_menu = { "New game", "Options", "Exit",
			"Gameloft Live" };
	private static final String[] s_pause_menu = { "Resume", "Main menu",
			"Exit" };
	private static final String[] s_sound = { "Sound", "On", "Off" };
	private static final String EXIT_INFO = "Are you sure to exit game?";
	private static final String BACK_TO_MENU = "Are you sure to go back to main menu?";
	private static final String YES = "Yes";
	private static final String NO = "No";
	private static final String EXIT = "Exit";
	private static final int S_MENU_Y = 60;

	private static final int C_FRAME_INTERVAL = 83;

	// @div end

	// @div begin #Game_variables.h
	public static int s_gameState = 0;
	public static int s_gameTick;
	public static int s_fps;
	public static boolean s_isSoundOn = !true;

	private int m_menuItem = 0;
	private int m_pause_Item = 0;
	private int m_exit_type;

	private boolean m_isPainting;

	// @div end
	public Game() {
		initConWeb();
		new Thread(this).start();

	}

	// @div begin #Game_run.h
	public void run() {
		setFullScreenMode(true);

		ChangeGameState(GS_LOGO);
		m_isPainting = false;
		long frameElapse, frameTick, fpsTick;
		try {
			frameTick = System.currentTimeMillis();
			int m_frames = 0;
			s_fps = 12;
			fpsTick = System.currentTimeMillis();
			while (true) {
				if (Paintable()) {
					frameElapse = System.currentTimeMillis() - frameTick;

					if (frameElapse < C_FRAME_INTERVAL) {
						Thread.sleep((C_FRAME_INTERVAL - frameElapse));
					}
					repaint();
					// serviceRepaints();
					frameTick = System.currentTimeMillis();
					if (++m_frames == 5) {
						s_fps = (int) (m_frames * 1000 / (frameTick - fpsTick));
						m_frames = 0;
						fpsTick = frameTick;
					}
				}
			}
		} catch (Exception exc) {
			exc.printStackTrace();
		}
	}

	// @div end

	// @div begin #Game_paint.h


	public void paint(Graphics g) {
		
		m_isPainting = true;
		UpdateKey();
		GamePaint(g);

		m_isPainting = false;
	}

	private final boolean Paintable() {
		
		return (m_isPainting == false);

	}

	private final void ChangeGameState(int newState) {
		if (s_gameState != newState) {
			s_gameState = newState;

		}
	}

	private final void GamePaint(Graphics g) {
		switch (s_gameState) {
		case GS_LOGO: {
			PaintLogo(g);
		}
			break;

		case GS_MAIN_MENU: {
			PaintMenu(g);
			UpdateMenu();
		}
			break;
		case GS_JABBER: {
			PaintGameLoftLive(g);
			UpdateGameLoftLive();
		}
			break;
		case GS_LOADING: {

		}
			break;

		case GS_RUN: {

		}
			break;
		case GS_PAUSE_MENU: {
			PaintPauseMenu(g);
			UpdatePauseMenu();
		}
			break;
		case GS_OPTIONS_MENU: {
			PaintOption(g);
			DealPressSoft();
		}
			break;
		case GS_MAIN_MENU_CONFIRM: {
			PaintExit(g, BACK_TO_MENU);
			DealPressSoft();
		}
			break;
		case GS_EXIT_CONFIRM: {
			//
			PaintExit(g, EXIT_INFO);
			DealPressSoft();
		}
			break;

		}

	}

	private final void PaintOption(Graphics g) {
		FillScreen(g, DEF.SCREEN_WIDTH, DEF.SCREEN_HEIGHT);
		g.setColor(0xffffff);
		g.drawString(s_sound[0], (DEF.SCREEN_WIDTH >> 1) - 5, 100, Graphics.RIGHT
				| Graphics.TOP);
		g.drawString(s_sound[s_isSoundOn ? 1 : 2], (DEF.SCREEN_WIDTH >> 1) + 5,
				100, Graphics.LEFT | Graphics.TOP);

		if (IsKeyPressed(DEF.KEY_A)) {
			s_isSoundOn = !s_isSoundOn;

		}
		PaintOkCancel(g, EXIT, null);
	}

	private final void PaintExit(Graphics g, String show) {

		FillScreen(g, DEF.SCREEN_WIDTH, DEF.SCREEN_HEIGHT);
		g.setColor(0xffffff);
		g.drawString(show, DEF.SCREEN_WIDTH >> 1, DEF.SCREEN_HEIGHT >> 1,
				Graphics.BOTTOM | Graphics.HCENTER);
		PaintOkCancel(g, YES, NO);
	}

	private final void PaintOkCancel(Graphics g, String left, String right) {
		if (left != null)
			g.drawString(left, 0, 320, Graphics.LEFT | Graphics.BOTTOM);
		if (right != null)
			g.drawString(right, 240, 320, Graphics.RIGHT | Graphics.BOTTOM);
	}

	private final void PaintPauseMenu(Graphics g) {
		FillScreen(g, DEF.SCREEN_WIDTH, DEF.SCREEN_HEIGHT);
		for (int i = 0; i < s_pause_menu.length; i++) {
			g.setColor(0xffffff);
			if (i == m_pause_Item)
				g.setColor(0x00ff00);
			g.drawString(s_pause_menu[i], DEF.SCREEN_WIDTH >> 1, S_MENU_Y + i
					* g.getFont().getHeight() + 5, Graphics.HCENTER | Graphics.BOTTOM);

		}

	}

	private final void UpdatePauseMenu() {
		if (IsKeyPressed(DEF.KEY_UP)) {
			m_pause_Item = m_pause_Item - 1 > 0 ? m_pause_Item - 1 : 0;
		} else if (IsKeyPressed(DEF.KEY_DOWN)) {
			m_pause_Item = m_pause_Item + 1 >= s_pause_menu.length ? s_pause_menu.length - 1
					: m_pause_Item + 1;
		} else if (IsKeyPressed(DEF.KEY_A) || IsKeyPressed(DEF.KEY_SOFT_LEFT)) {
			switch (m_pause_Item) {
			case 0:
				ChangeGameState(GS_RUN);
				break;
			case 1:
				ChangeGameState(GS_MAIN_MENU_CONFIRM);
				break;
			case 2:
				m_exit_type = EXIT_FORM_PAUSE;
				ChangeGameState(GS_EXIT_CONFIRM);
				break;

			}
		}
	}

	// private static Hashtable s_spriteTable = new Hashtable();

	// private ASprite creatAsprite(String name) {
	// ASprite sprite = null;
	// sprite = (ASprite) s_spriteTable.get(name);
	// if (sprite != null)
	// return sprite;
	// InputStream is = name.getClass().getResourceAsStream(name);
	// try {
	// int l = is.available();
	// byte[] data = new byte[l];
	// is.read(data);
	// sprite = new ASprite();
	// sprite.Load(data, 0);
	// s_spriteTable.put(name, sprite);
	// return sprite;
	// } catch (IOException e) {
	// 
	// e.printStackTrace();
	// }
	// return null;
	// }
	private final byte[] GetData(String name) {
		InputStream is = name.getClass().getResourceAsStream(name);
		int l;
		try {
			l = is.available();
			byte[] data = new byte[l];
			is.read(data);
			return data;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;

	}

	// free res
	private final void Dispose() {

		// s_spriteTable.clear();
		System.gc();
	}

	public static final void DrawLoadingBar(Graphics g, int len, int total,
			int w, int h) {
		FillScreen(g, DEF.SCREEN_WIDTH, DEF.SCREEN_HEIGHT);
		int x = (DEF.SCREEN_WIDTH - w) >> 1;
		g.setColor(0xffffff);
		g.drawRect(x, 280, w, h);
		int insideW = w * len / total;
		g.setClip(x, 280, w, h);
		g.setColor(0x0000ff);
		g.fillRect(x + 1, 281, insideW, h - 1);
	}

	private final void PaintMenu(Graphics g) {
		FillScreen(g, DEF.SCREEN_WIDTH, DEF.SCREEN_HEIGHT);
		for (int i = 0; i < s_menu.length; i++) {
			g.setColor(0xffffff);
			if (i == m_menuItem)
				g.setColor(0x00ff00);
			g.drawString(s_menu[i], DEF.SCREEN_WIDTH >> 1, S_MENU_Y + i
					* g.getFont().getHeight() + 5, Graphics.HCENTER | Graphics.BOTTOM);

		}

	}

	private final void UpdateMenu() {
		if (IsKeyPressed(DEF.KEY_UP)) {
			m_menuItem = m_menuItem - 1 > 0 ? m_menuItem - 1 : 0;
		} else if (IsKeyPressed(DEF.KEY_DOWN)) {
			m_menuItem = m_menuItem + 1 >= s_menu.length ? s_menu.length - 1
					: m_menuItem + 1;
		} else if (IsKeyPressed(DEF.KEY_A) || IsKeyPressed(DEF.KEY_SOFT_LEFT)) {

			switch (m_menuItem) {
			case 0:

				break;
			case 1:
				ChangeGameState(GS_OPTIONS_MENU);
				break;
			case 2:
				m_exit_type = EXIT_FORM_MAIN_MENU;
				ChangeGameState(GS_EXIT_CONFIRM);
				break;
			case 3:
				ChangeGameState(GS_JABBER);

				setOnlineState(ONLINESTATE_LOGIN);
				break;

			}
		}
	}

	private final void PaintLogo(Graphics g) {
		FillScreen(g, DEF.SCREEN_WIDTH, DEF.SCREEN_HEIGHT);
		_font = g.getFont();
		ChangeGameState(GS_MAIN_MENU);

	}

	private final void Quit() {
		Sparrow.s_sparrow.destroyApp(true);// quit
	}

	/**
	 * draw ActionPhase
	 * 
	 * @param g
	 */

	static public void FillScreen(Graphics g, int w, int h) {
		g.setClip(0, 0, w, h);
		g.setColor(0);
		g.fillRect(0, 0, w, h);

	}

	private final void PressRightSoftKey() {
	
		switch (s_gameState) {
		case GS_RUN:
			ChangeGameState(GS_PAUSE_MENU);
			s_keyPressed ^= GetCancelKey();
			m_pause_Item = 0;
			break;
		case GS_EXIT_CONFIRM:
			if (m_exit_type == EXIT_FORM_PAUSE)
				ChangeGameState(GS_PAUSE_MENU);
			else
				ChangeGameState(GS_MAIN_MENU);
			s_keyPressed ^= GetCancelKey();
			break;
		case GS_MAIN_MENU_CONFIRM:
			ChangeGameState(GS_PAUSE_MENU);
			s_keyPressed ^= GetCancelKey();
			break;
		}
	}

	private final int GetCancelKey() {
		// TODO Auto-generated method stub
		return DEF.KEY_SOFT_RIGHT;
	}

	private final void PressLeftSoftKey() {
		// TODO Auto-generated method stub
		switch (s_gameState) {

		case GS_EXIT_CONFIRM:
			Quit();

			break;
		case GS_MAIN_MENU_CONFIRM:
			ChangeGameState(GS_MAIN_MENU);
			Dispose();
			s_keyPressed ^= GetOkKey();
			break;
		case GS_OPTIONS_MENU:
			ChangeGameState(GS_MAIN_MENU);
			s_keyPressed ^= GetOkKey();
			break;
		}
	}

	private final int GetOkKey() {
		// TODO Auto-generated method stub
		return DEF.KEY_SOFT_LEFT;
	} 

	protected void hideNotify() {
		// TODO Auto-generated method stub
		// super.hideNotify();
		switch (s_gameState) {
		case GS_RUN:
			ChangeGameState(GS_PAUSE_MENU);
			break;
		}
	}

	// @div end

	// @div begin #Game_keyProcess.h
	char curInputChar = 0;
	StringBuffer inputString = new StringBuffer();
	int lastKey = 0;
	int Index = 0;
	long lastPressTime = 0;
	int _keycode = 0;
	boolean confirm = false;
	boolean num = false;

	private void initInput() {
		lastPressTime = 0;
		lastKey = 0;
		confirm = false;
		curInputChar = 0;
		inputString.delete(0, inputString.length());
		modifyInfoString = "";
	}

	final static char keymap[][] = { { '0' }, { '_', '.', '@', '1' },
			{ 'A', 'B', 'C', 'a', 'b', 'c', '2' },
			{ 'D', 'E', 'F', 'd', 'e', 'f', '3' },
			{ 'G', 'H', 'I', 'g', 'h', 'i', '4' },
			{ 'J', 'K', 'L', 'j', 'k', 'l', '5' },
			{ 'M', 'N', 'O', 'm', 'n', 'o', '6' },
			{ 'P', 'Q', 'R', 'S', 'p', 'q', 'r', 's', '7' },
			{ 'T', 'U', 'V', 't', 'u', 'c', '8' },
			{ 'W', 'X', 'Y', 'Z', 'w', 'x', 'y', 'z', '9' } };

	public String input(int max) {
		String temp;
		if (_keycode != 0) {
			if ((_keycode >= KEY_NUM0) && (_keycode <= KEY_NUM9)) {

				lastPressTime = System.currentTimeMillis();

				if (lastKey == _keycode && num == false && _keycode != KEY_NUM0) {
					{
						Index = (Index + 1)
								% keymap[_keycode - KEY_NUM0].length;
						confirm = false;

					}

				} else {
					if (num != true)
						Index = 0;
					else
						Index = keymap[_keycode - KEY_NUM0].length - 1;
					if (lastKey != 0) {
						confirm = true;
						if (inputString.length() < max) {
							// inputString += curInputChar;
							inputString.append(curInputChar);
						}
					} else
						confirm = false;

				}
				curInputChar = keymap[_keycode - KEY_NUM0][Index];

				lastKey = _keycode;

			} else {
				if (_keycode == KEY_POUND) {
					if (lastPressTime != 0) {

						lastPressTime = 0;
						lastKey = 0;
						confirm = false;
						curInputChar = 0;
					} else {
						if (inputString.length() != 0) {
							// inputString = inputString.substring(0,
							// inputString
							// .length() - 1);
							inputString.deleteCharAt(inputString.length() - 1);
						}
					}

				} else if (_keycode == KEY_STAR) {
					num = !num;
				}

			}
			_keycode = 0;
		}
		if (lastPressTime != 0)
			if (System.currentTimeMillis() - lastPressTime > 1000) {
				if (inputString.length() < max)
					// inputString += curInputChar;
					inputString.append(curInputChar);
				lastPressTime = 0;
				lastKey = 0;
				confirm = false;
				curInputChar = 0;

			}

		if (inputString == null)
			return "";
		if (curInputChar == 0)
			temp = inputString.toString();
		else {
			if (inputString.length() < max) {
				temp = (inputString.append(curInputChar)).toString();
				inputString.deleteCharAt(inputString.length() - 1);
			} else
				temp = inputString.toString();
		}

		return temp;
	}

	static int s_keyCurrent;
	static int s_keyPressed;
	static int s_keyReleased;
	static int s_keyDblPressed;
	static int s_lastPressed;
	static int s_keyTick;
	static int s_fastCurrentKey;
	public static final int GAMEKEY_QUEUE_SIZE = 10;
	private static int[] s_gkQueue = new int[GAMEKEY_QUEUE_SIZE];
	private static int s_queueStart;
	private static int s_queueEnd;

	static void InitKey() {
		s_keyCurrent = 0;
		s_keyPressed = 0;
		s_keyReleased = 0;
		s_keyDblPressed = 0;
		s_lastPressed = 0;
		s_keyTick = 0;
		s_fastCurrentKey = 0;
		for (int i = 0; i < GAMEKEY_QUEUE_SIZE; i++) {
			s_gkQueue[i] = 0;
		}
		s_queueStart = s_queueEnd = 0;
	}

	static boolean PushQueue(int key) {
		if ((s_queueStart + 1) % GAMEKEY_QUEUE_SIZE == s_queueEnd) {
			return false;
		} else {
			s_gkQueue[s_queueStart] = key;
			s_queueStart = (s_queueStart + 1) % GAMEKEY_QUEUE_SIZE;
		}
		return true;
	}

	static int PopQueue() {
		if (s_queueStart == s_queueEnd) {
			return s_fastCurrentKey;
		}
		int key = s_gkQueue[s_queueEnd];

		s_queueEnd = (s_queueEnd + 1) % GAMEKEY_QUEUE_SIZE;
		return key;
	}

	static boolean IsKeyHold(int gameKey) {
		return (s_keyCurrent & gameKey) != 0;
	}

	static boolean IsAnyKeyPressed() {
		return s_keyPressed != 0;
	}

	static boolean IsKeyDblPressed(int gameKey) {
		return (s_keyDblPressed & gameKey) != 0;
	}

	static boolean IsKeyPressed(int gKey) {
		return (s_keyPressed & gKey) != 0;
	}

	static void ClearKeyHold(int gKey) {
		s_fastCurrentKey &= ~gKey;
		PushQueue(s_fastCurrentKey);
	}

	protected void keyPressed(int keyCode) {
		int key = GetKey(keyCode);
		_keycode = keyCode;
		s_fastCurrentKey |= key;

		PushQueue(s_fastCurrentKey);

	}

	private final void DealPressSoft() {
		if (IsKeyPressed(GetOkKey())) {
			PressLeftSoftKey();
		} else if (IsKeyPressed(GetCancelKey())) {
			PressRightSoftKey();
		}
	}

	protected void keyReleased(int keyCode) {
		s_fastCurrentKey &= ~(GetKey(keyCode) | s_keyCurrent); // also release
		// the current
		// key
		_keycode = 0;
		PushQueue(s_fastCurrentKey);
	}

	static void UpdateKey() // call this function at beginning of your paint()
	{
		int currentKey;

		currentKey = PopQueue();
		s_keyPressed = ~s_keyCurrent & currentKey;
		s_keyReleased = s_keyCurrent & ~currentKey;
		s_keyCurrent = currentKey;

		s_keyTick++;
	}

	static int GetKey(int keyCode) {
		int ret = 0;
		switch (keyCode) {
		case -1:
		case UP:

			ret = DEF.KEY_UP;
			break;
		case -2:
		case DOWN:

			ret = DEF.KEY_DOWN;
			break;
		case -3:
		case LEFT:
		case KEY_NUM4:
			ret = DEF.KEY_LEFT;
			break;
		case -4:
		case RIGHT:
		case KEY_NUM6:
			ret = DEF.KEY_RIGHT;
			break;
		case -5:

			ret = DEF.KEY_A;
			break;
		case KEY_NUM0:
			ret = DEF.KEY_0;
			break;
		case KEY_NUM1:
			ret = DEF.KEY_1;
			break;
		case KEY_NUM3:
			ret = DEF.KEY_3;
			break;
		case KEY_NUM7:
			ret = DEF.KEY_7;
			break;
		case KEY_NUM9:
			ret = DEF.KEY_9;
			break;
		case KEY_STAR:
			ret = DEF.KEY_B;
			break;
		case KEY_POUND:
			ret = DEF.KEY_C;
			break;
		case -6:
			ret = DEF.KEY_SOFT_LEFT;
			break;
		case -7:

		case -11:

			ret = DEF.KEY_SOFT_RIGHT;
			break;
		}
		return ret;
	}

	// @div end
	/** ********************************** */
	public static final int ONLINESTATE_LOGIN = 0;
	public static final int ONLINESTATE_GETUSERINFO = 1;
	public static final int ONLINESTATE_GLIVE_MENU = 2;
	public static final int ONLINESTATE_USER_PROFILE = 3;
	public static final int ONLINESTATE_CHANGE_USERNAME = 4;
	public static final int ONLINESTATE_CHANGE_USERPASSWORD = 5;
	public static final int ONLINESTATE_CHANGE_USEREMAIL = 6;
	public static final int ONLINESTATE_CHANGE_USERCOUNTRY = 7;
	public static final int ONLINESTATE_CHANGE_USEREMAILFLAG = 8;
	public static final int ONLINESTATE_CHANGE_USERAVATAR = 9;
	public static final int ONLINESTATE_REGISTER = 10;
	public static final int ONLINESTATE_FRIEND = 11;
	public static final int ONLINESTATE_FRIEND_LIST = 12;
	public static final int ONLINESTATE_FRIEND_SIGLE = 13;
	public static final int ONLINESTATE_FRIEND_ADD = 14;

	public static final int ONLINESTATE_MESSAGE_GETCOUNT = 15;
	public static final int ONLINESTATE_MESSAGE_SHOWCOUNT = 16;
	public static final int ONLINESTATE_MESSAGE_LIST = 17;
	public static final int ONLINESTATE_MESSAGE_READ = 18;
	public static final int ONLINESTATE_MESSAGE = 19;
	public static final int ONLINESTATE_MESSAGE_GETCOUNTINBOX = 20;
	public static final int ONLINESTATE_MESSAGE_GETHEADINBOX = 21;
	public static final int ONLINESTATE_MESSAGE_INBOX = 22;
	public static final int ONLINESTATE_MESSAGE_WRITE = 23;
	public static final int ONLINESTATE_FRIEND_SENDMESSAGE = 24;

	private static final int SHOW_COUNT = 5;
	private static final String[] s_inputLogin = { "Username: ", "Password: " };
	private static final String[] s_inputRegister = { "Username: ",
			"Password: ", "Nationality: ", "Email: ", "Receive Email:" };
	private static final String[] s_gliveMenu = { "User Profile",
			"User Friend ", "Message", "Trophy", "Chat", "New Game" };
	private static final String[] s_profile = { "Change UserName",
			"Change UserPassword ", "Change Email","Change Country", "Change EmailFlag",
			 "Change Avatar" };
	private static final String[] s_friendMenu = { "Add friend", "Friend List" };
	private static final String[] s_singleFriendMenu = { "invite to play",
			"invite to chat", "send message", "compare game", "more info" };
	private static final String[] s_addFriends = { "Friends name :",
			"Invite Message :" };
	private static final String[] s_messageCount = { "Game Message: ",
			"User Message: ", "Friend invitation: ", "Play game invitation: ",
			"Chat invitation: " };
	private static final String[] s_message = { "Write Message", "InBox" };
	private static final String[] s_messageWrite = { "<Message Receiver>",
			"<Message Title>", "<CMessage ontent>" };
	private int m_selectItem;
	private static int m_selectFriend;
	private String inputUserName = "he4";
	private String inputUserPassword = "123";
	private String inputUserNationality = "CN";

	private String inputMessageReceiver = "";
	private String inputMessageTitle = "";
	private String inputMessageContent = "";

	private String modifyInfoString = "";
	private String recommendedName = "";
	private String inviteFriendName = "";
	private String inviteMessage = "";
	private String[] m_divString;
	private int m_currentMsgType;

	private int m_gameMsgCount;
	private int m_userMsgCount;
	private int m_friendInvitedMsgCount;
	private int m_gameInvitedMsgCount;
	private int m_chatInvitedMsgCount;
	private int m_allMsgCount;
	private int m_msgInPageCount;
	private int m_currentMsgPage;

	private void initMessageParam() {
		m_msgInPageCount = 0;
		m_currentMsgPage = 0;
	}

	public static int online_substate = 0;
	private int m_friendSigleMenuBegin;
	private boolean m_binBox = false;
	private StringBuffer m_stringBuffer = new StringBuffer(0);

	private static int online_state = 0;
	GLXPlayerWebComponent m_web;
	private int m_errorCode;

	void setOnlineState(int state) {
		if (online_state != state) {

			online_state = state;
			online_substate = OnlineSubstate.SET;
			m_selectItem = 0;
			initInput();
		}
	}

	// when sending a request to the server, there are 4 substates :
	public interface OnlineSubstate {
		public static final byte SET = 0; // the request is composed and sent
		public static final byte WAIT = 1; // the request is pending, waiting
		// for answer from server
		public static final byte PROCESS = 2; // the request answer is
		// processed
		public static final byte ANALYZE_ERROR = 3;
		public static final byte NET_ERROR = 4;
		public static final byte REQUEST_FAIL = 5;

	}

	private void setOnlineSubstate(int substate) {
		switch (substate) {
		case OnlineSubstate.WAIT:
			if (online_substate == OnlineSubstate.SET)
				online_substate = substate;
			break;

		default:
			online_substate = substate;
			break;

		}

	}

	public void initConWeb() {
		m_web = GLXPlayerWebComponent.GetInst(Sparrow.s_sparrow);
		m_web.RegisterObserver(this);

	}

	private void PaintGameLoftLive(Graphics g) {
		FillScreen(g, 240, 320);
		g.setColor(0xffffff);
		if (num == false)
			g.drawString("en", 10, 10, 0);
		else
			g.drawString("num", 10, 10, 0);

		switch (online_substate) {
		case OnlineSubstate.NET_ERROR: {
			g.drawString("net error", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			return;
		case OnlineSubstate.ANALYZE_ERROR: {
			g.drawString("analyze wrong", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			return;
		case OnlineSubstate.REQUEST_FAIL:

			switch (online_state) {
			case ONLINESTATE_CHANGE_USERNAME: {
				g.drawString("ERROR: " + m_errorCode, 120, 160, Graphics.HCENTER
						| Graphics.BOTTOM);
				if (m_errorCode == 48) {
					g.drawString("name is taken recommended name is ", 120,
							190, Graphics.HCENTER | Graphics.BOTTOM);
					g.drawString(recommendedName, 120, 210, Graphics.HCENTER
							| Graphics.BOTTOM);
				}
				PaintOkCancel(g, null, EXIT);
			}
				return;
			case ONLINESTATE_LOGIN: {
				if (m_errorCode == 60)
					g.drawString("ERROR: user is online!", 120, 160, Graphics.HCENTER
							| Graphics.BOTTOM);
				else if (m_errorCode == 60 || m_errorCode == 67) {

					break;
				} else
					g.drawString("ERROR: wrong password!", 120, 160, Graphics.HCENTER
							| Graphics.BOTTOM);
				PaintOkCancel(g, null, EXIT);
			}
				return;
			case ONLINESTATE_REGISTER: {
				g.drawString("ERROR: " + m_errorCode, 120, 160, Graphics.HCENTER
						| Graphics.BOTTOM);
				if (m_errorCode == 48) {
					g.drawString("name is taken recommended name is ", 120,
							190, Graphics.HCENTER | Graphics.BOTTOM);
					g.drawString(recommendedName, 120, 210, Graphics.HCENTER
							| Graphics.BOTTOM);
				}
				PaintOkCancel(g, null, EXIT);
			}
				return;
			default:
				g.drawString("ERROR: " + m_errorCode, 120, 160, Graphics.HCENTER
						| Graphics.BOTTOM);

				PaintOkCancel(g, null, EXIT);
			}

			return;

		}

		switch (online_state) {
		case ONLINESTATE_LOGIN: {
			PaintLiveLogin(g);
		}
			break;
		case ONLINESTATE_REGISTER:
			PaintLiveRegister(g);
			break;

		case ONLINESTATE_GETUSERINFO: {
			PaintLiveGetUserInfo(g);
		}
			break;
		case ONLINESTATE_GLIVE_MENU: {
			PaintLiveMenu(g);
		}
			break;
		case ONLINESTATE_USER_PROFILE: {
			PaintLiveProfile(g);
		}
			break;
		case ONLINESTATE_CHANGE_USERNAME: {
			PaintLiveChangeUserName(g);
		}
			break;
		case ONLINESTATE_CHANGE_USERPASSWORD: {
			PaintLiveChangeUserPassword(g);
		}
			break;
		case ONLINESTATE_CHANGE_USEREMAIL: {
			PaintLiveChangeUserEmail(g);
		}
			break;
		case ONLINESTATE_CHANGE_USERCOUNTRY: {
			PaintLiveChangeUserCountry(g);
		}
			break;
		case ONLINESTATE_CHANGE_USEREMAILFLAG: {
			PaintLiveChangeUserEmailFlag(g);
		}
			break;
		case ONLINESTATE_CHANGE_USERAVATAR: {
			PaintLiveChangeUserAvatar(g);
		}
			break;
		case ONLINESTATE_FRIEND: {
			PaintLiveFriend(g);
		}
			break;
		case ONLINESTATE_FRIEND_LIST: {
			PaintLiveFriendList(g);
		}
			break;
		case ONLINESTATE_FRIEND_SIGLE: {
			PaintLiveFriendSingle(g);
			break;
		}
		case ONLINESTATE_FRIEND_ADD: {
			PaintLiveFriendAdd(g);
		}
			break;
		case ONLINESTATE_MESSAGE_SHOWCOUNT:
			PaintLiveMessageShowCount(g);
			break;
		case ONLINESTATE_MESSAGE_LIST:
			PaintLiveMessageList(g);
			break;
		case ONLINESTATE_MESSAGE_READ:
			PaintLiveMessageRead(g);
			break;
		case ONLINESTATE_MESSAGE_GETCOUNTINBOX:
		case ONLINESTATE_MESSAGE_GETHEADINBOX:
		case ONLINESTATE_MESSAGE_GETCOUNT: {
			PaintLiveMessageGetCount(g);
		}
			break;
		case ONLINESTATE_MESSAGE:
			PaintLiveMessage(g);
			break;
		case ONLINESTATE_MESSAGE_INBOX:
			PaintLiveMessageInbox(g);
			break;
		case ONLINESTATE_FRIEND_SENDMESSAGE:
		case ONLINESTATE_MESSAGE_WRITE:
			PaintLiveMessageWrite(g);
			break;

		}

	}

	private void PaintLiveMessageWrite(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("--Send Message--", 120, 10, Graphics.HCENTER | Graphics.TOP);
			for (int i = 0; i < s_messageWrite.length; i++) {
				g.drawString(s_messageWrite[i], 20, 30 + i
						* (g.getFont().getHeight() * 2 + 10), 0);

			}
			g.drawString(inputMessageReceiver, 20, 30 + 0
					* (g.getFont().getHeight() * 2 + 10)
					+ g.getFont().getHeight(), 0);
			g.drawString(inputMessageTitle, 20, 30 + 1
					* (g.getFont().getHeight() * 2 + 10)
					+ g.getFont().getHeight(), 0);
			g.drawString(inputMessageContent, 20, 30 + 2
					* (g.getFont().getHeight() * 2 + 10)
					+ g.getFont().getHeight(), 0);
			g.setColor(0x00ff00);
			g.drawString(">>", 20, 30 + m_selectItem
					* (g.getFont().getHeight() * 2 + 10)
					+ g.getFont().getHeight(), Graphics.TOP | Graphics.RIGHT);

			PaintOkCancel(g, "Send", EXIT);
			//
			g.drawString("Max is 64", 120, 270, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 300, Graphics.TOP
					| Graphics.HCENTER);

		}
			break;
		case OnlineSubstate.WAIT:
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
			break;
		}
	}

	private void PaintLiveMessageInbox(Graphics g) {
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("--Inbox--", 120, 10, Graphics.HCENTER | Graphics.TOP);
			int total = 0;
			if ((m_allMsgCount % SHOW_COUNT) == 0) {
				total = (m_allMsgCount / SHOW_COUNT);
			} else {
				total = (m_allMsgCount / SHOW_COUNT) + 1;
			}

			if (total == 0) {
				total = 1;
			}
			m_stringBuffer.delete(0, m_stringBuffer.length());
			m_stringBuffer.append("Total: ").append(m_allMsgCount).append(
					"  Page:").append(m_currentMsgPage + 1).append("/").append(
					total);

			g.drawString(m_stringBuffer.toString(), 120, 30, Graphics.HCENTER | Graphics.TOP);
			for (int i = 0; i < m_msgInPageCount; i++) {

				m_stringBuffer.delete(0, m_stringBuffer.length());

				m_stringBuffer.append(i + 1).append(" Id: ").append(
						m_web.getMsgID(i)).append(" from: ").append(
						m_web.getMsgSender(i));
				if (!m_web.getMsgReadState(i)) {
					m_stringBuffer.append("<Unread>");
				}

				g.drawString(m_stringBuffer.toString(), 10, 50 + i * 2
						* g.getFont().getHeight(), 0);
				m_stringBuffer.delete(0, m_stringBuffer.length());
				m_stringBuffer.append("<<").append(m_web.getMsgTitle(i))
						.append(">>");

				g.drawString(m_stringBuffer.toString(), 10, 50 + (i * 2 + 1)
						* g.getFont().getHeight(), 0);

			}

			//
			g.setColor(0x00ff00);

			g.drawString(">>", 10, 50 + m_selectItem * 2
					* g.getFont().getHeight(), Graphics.TOP | Graphics.RIGHT);
			PaintOkCancel(g, null, EXIT);
			//
			g.drawString(" ok to check # to del", 120, 270, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 300, Graphics.TOP
					| Graphics.HCENTER);

		}
			break;
		case OnlineSubstate.WAIT:
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
			break;
		}
	}

	private void PaintLiveMessage(Graphics g) {
		// TODO Auto-generated method stub

		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("----Message----", 120, 10, Graphics.HCENTER | Graphics.TOP);

			for (int i = 0; i < s_message.length; i++) {
				g.drawString(s_message[i], 20, 40 + i * 30, Graphics.TOP | Graphics.LEFT);

			}
			g.setColor(0x00ff00);
			g.drawString(">>", 20, 40 + m_selectItem * 30, Graphics.TOP | Graphics.RIGHT);
			PaintOkCancel(g, null, EXIT);

			g.drawString("press ok to confirm", 120, 200, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 230, Graphics.TOP
					| Graphics.HCENTER);

		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}

	}

	private void PaintLiveMessageGetCount(Graphics g) {
		// TODO Auto-generated method stub

		g.drawString("please wait..", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
	}

	private void PaintLiveMessageRead(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("--Message View--", 120, 10,Graphics.HCENTER | Graphics.TOP);
			int position = m_web.getCurrentMsgPosition();
			g.drawString("Message ID: " + m_web.getCurrentMsgPosition(), 10,
					30, 0);
			g.drawString("Subject: " + m_web.getMsgTitle(position), 10, 50, 0);
			g.drawString("From: " + m_web.getMsgSender(position), 10, 70, 0);
			g.drawString("Content: ", 10, 110, 0);
			g.setColor(0x00ff00);
			if (m_divString != null)

				for (int i = 0; i < m_divString.length; i++) {
					g.drawString(m_divString[i], 20, 130 + i
							* g.getFont().getHeight(), 0);

				}
			int t = m_web.getMsgType(position);
			if (t == GLXPlayerWebComponent.USER_MESSAGE_TYPE_GAMEMESSAGE
					|| t == GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE) {
				PaintOkCancel(g, null, EXIT);
			} else {
				boolean rt = m_web.getMsgReadState(position);
				if (rt == false) {
					PaintOkCancel(g, "Accept", "Decline");
				} else {
					PaintOkCancel(g, null, EXIT);
				}
			}

		}
			break;
		case OnlineSubstate.WAIT:
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
			break;
		}
	}

	private void PaintLiveMessageList(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("--Message List--", 120, 10, Graphics.HCENTER | Graphics.TOP);
			int l = m_web.getMsgListCount();
			for (int i = 0; i < l; i++) {

				m_stringBuffer.delete(0, m_stringBuffer.length());
				m_stringBuffer.append(i + 1).append(" mesId: ").append(
						m_web.getMsgID(i)).append(" from: ").append(
						m_web.getMsgSender(i));
				// g.drawString(arg0, arg1, arg2, arg3)
				g.drawString(m_stringBuffer.toString(), 10, 30 + i * 2
						* g.getFont().getHeight(), 0);
				m_stringBuffer.delete(0, m_stringBuffer.length());
				m_stringBuffer.append("<<").append(m_web.getMsgTitle(i))
						.append(">>");

				// g.drawString(arg0, arg1, arg2, arg3)
				g.drawString(m_stringBuffer.toString(), 10, 30 + (i * 2 + 1)
						* g.getFont().getHeight(), 0);
			}

			g.setColor(0x00ff00);
			if (l != 0)
				g.drawString(">>", 10, 30 + m_selectItem * 2
						* g.getFont().getHeight(), Graphics.TOP | Graphics.RIGHT);
			PaintOkCancel(g, null, EXIT);

			g.drawString("press ok to check", 120, 270, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 300, Graphics.TOP
					| Graphics.HCENTER);

		}
			break;
		case OnlineSubstate.WAIT:
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
			break;
		}
	}

	private void PaintLiveMessageShowCount(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g
					.drawString("-You Have Some Messages-", 120, 10, Graphics.HCENTER
							| Graphics.TOP);

			for (int i = 0; i < s_messageCount.length; i++) {
				g.drawString(s_messageCount[i], 10, 30 + i * 30, 0);

			}

			g.drawString(Integer.toString(m_gameMsgCount), 10 + g.getFont()
					.stringWidth(s_messageCount[0]), 30, 0);
			g.drawString(Integer.toString(m_userMsgCount), 10 + g.getFont()
					.stringWidth(s_messageCount[1]), 30 + 30, 0);
			g.drawString(Integer.toString(m_friendInvitedMsgCount), 10 + g
					.getFont().stringWidth(s_messageCount[2]), 30 + 60, 0);
			g.drawString(Integer.toString(m_gameInvitedMsgCount), 10 + g
					.getFont().stringWidth(s_messageCount[3]), 30 + 90, 0);
			g.drawString(Integer.toString(m_chatInvitedMsgCount), 10 + g
					.getFont().stringWidth(s_messageCount[4]), 30 + 120, 0);

			g.setColor(0x00ff00);
			g.drawString(">>", 10, 30 + m_selectItem * 30, Graphics.TOP | Graphics.RIGHT);
			PaintOkCancel(g, null, "TO GLMENU");

			g.drawString("press ok to confirm", 120, 200, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 230, Graphics.TOP
					| Graphics.HCENTER);

		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveFriendAdd(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("----Add Friends----", 120, 10, Graphics.HCENTER | Graphics.TOP);

			for (int i = 0; i < s_addFriends.length; i++) {
				g.drawString(s_addFriends[i], 10, 140 + i * 30, Graphics.BOTTOM
						| Graphics.LEFT);

			}
			g.setColor(0x00ff00);
			g.drawString(">>", 10, 140 + m_selectItem * 30, Graphics.BOTTOM | Graphics.RIGHT);
			PaintOkCancel(g, null, EXIT);
			g.drawString(inviteFriendName, 120, 140, Graphics.BOTTOM | Graphics.LEFT);
			g.drawString(inviteMessage, 120, 170, Graphics.BOTTOM | Graphics.LEFT);
			g
					.drawString("press ok to send invite", 120, 200, Graphics.TOP
							| Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 230, Graphics.TOP
					| Graphics.HCENTER);

		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveFriendSingle(Graphics g) {
		
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("Friend", 120, 10, Graphics.HCENTER | Graphics.TOP);

			PaintOkCancel(g, null, EXIT);
			for (int i = m_friendSigleMenuBegin; i < s_singleFriendMenu.length; i++) {
				g.drawString(s_singleFriendMenu[i], 30, 30
						+ (i - m_friendSigleMenuBegin)
						* g.getFont().getHeight(), 0);
			}

			g.setColor(0x00ff00);
			g.drawString(">>", 30, 30 + (m_selectItem - m_friendSigleMenuBegin)
					* g.getFont().getHeight(), Graphics.TOP | Graphics.RIGHT);
			g.drawString("press ok to confirm", 120, 200, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 230, Graphics.TOP
					| Graphics.HCENTER);

		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveFriendList(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("FriendList", 120, 10, Graphics.HCENTER | Graphics.TOP);
			int friendList = m_web.GetFriendsCount();
			g.drawString("No-Name-Country-Rep-State", 5, 30, 0);
			PaintOkCancel(g, null, EXIT);
			if (friendList <= 0)
				break;
			for (int i = 0; i < friendList; i++) {
				int y = 45 + i * g.getFont().getHeight();
				m_stringBuffer.delete(0, m_stringBuffer.length());
				m_stringBuffer.append(i).append(" ").append(
						m_web.GetFriendName(i)).append(" ").append(
						m_web.GetFriendCountry(i)).append(" ").append(
						m_web.GetFriendReputation(i)).append(" ").append("<<")
						.append(m_web.GetFriendStateInfo(i)).append(">>");
				g.drawString(m_stringBuffer.toString(), 10, y, 0);
			}

			g.setColor(0x00ff00);
			g.drawString(">>", 10, 45 + m_selectItem * g.getFont().getHeight(),
					Graphics.TOP | Graphics.RIGHT);

			g
					.drawString(" ok to confirm # to del", 120, 200, Graphics.TOP
							| Graphics.HCENTER);
			g.drawString("press up or down to move", 120, 230, Graphics.TOP
					| Graphics.HCENTER);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveFriend(Graphics g) {
		// TODO Auto-generated method stub

		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("Friend", 120, 10, Graphics.HCENTER | Graphics.TOP);
			PaintOkCancel(g, null, EXIT);
			for (int i = 0; i < s_friendMenu.length; i++) {
				g.drawString(s_friendMenu[i], 30, 30 + i
						* g.getFont().getHeight(), 0);
			}

			g.setColor(0x00ff00);
			g.drawString(">>", 30, 30 + m_selectItem * g.getFont().getHeight(),
					Graphics.TOP | Graphics.RIGHT);
			g.drawString("press ok to confirm", 120, 200, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to move", 120, 230, Graphics.TOP
					| Graphics.HCENTER);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160,Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveChangeUserAvatar(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("Change UserAvatar", 120, 10, Graphics.HCENTER | Graphics.TOP);

			g.setColor(0x00ff00);

			if (avatar != null)
				g.drawImage(avatar, 40, 40, 0);
			g.drawString("press ok to send", 120, 230, Graphics.TOP | Graphics.HCENTER);
			PaintOkCancel(g, null, EXIT);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;
		case OnlineSubstate.PROCESS:
			break;

		}
	}

	private void PaintLiveChangeUserEmailFlag(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("Change UserEmail ", 120, 10, Graphics.HCENTER | Graphics.TOP);
			g.drawString("Current get email option: " + m_web.GetEmailFlag(),
					30, 30, 0);
			g.drawString("Do you want to get email from gameloft?", 120, 50,
					Graphics.HCENTER | Graphics.TOP);
			g.drawString("Yes(1)/No(0): " + modifyInfoString, 30, 70, 0);
			g.setColor(0x00ff00);

			g.drawString("please input 1 or 0", 120, 200, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press ok to send", 120, 230, Graphics.TOP | Graphics.HCENTER);
			PaintOkCancel(g, null, EXIT);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveChangeUserCountry(Graphics g) {
		
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("Change UserCountry", 120, 10, Graphics.HCENTER | Graphics.TOP);
			g.drawString("Current Country: " + m_web.GetCountry(), 30, 30, 0);
			g.drawString("New Country: " + modifyInfoString, 30, 50, 0);
			g.setColor(0x00ff00);

			g.drawString("please input your new Country", 120, 200, Graphics.TOP
					| Graphics.HCENTER);
			g.drawString("press ok to send", 120, 230, Graphics.TOP | Graphics.HCENTER);
			PaintOkCancel(g, null, EXIT);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveChangeUserEmail(Graphics g) {
	
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("Change UserEmail", 120, 10, Graphics.HCENTER | Graphics.TOP);
			g.drawString("Current Email: " + m_web.GetEmail(), 30, 30, 0);
			g.drawString("New Name: " + modifyInfoString, 30, 50, 0);
			g.setColor(0x00ff00);

			g.drawString("please input your new Email", 120, 200, Graphics.TOP
					| Graphics.HCENTER);
			g.drawString("press ok to send", 120, 230, Graphics.TOP | Graphics.HCENTER);
			PaintOkCancel(g, null, EXIT);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveChangeUserPassword(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("Change UserPassword", 120, 10, Graphics.HCENTER | Graphics.TOP);
			g.drawString("Current Password " + m_web.GetUserPassword(), 30, 30,
					0);
			g.drawString("New PassWord: " + modifyInfoString, 30, 50, 0);
			g.setColor(0x00ff00);

			g.drawString("please input your new PassWord", 120, 200, Graphics.TOP
					| Graphics.HCENTER);
			g.drawString("press ok to send", 120, 230, Graphics.TOP | Graphics.HCENTER);
			PaintOkCancel(g, null, EXIT);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveChangeUserName(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("Change UserName", 120, 10, Graphics.HCENTER | Graphics.TOP);
			g.drawString("Current Name: " + m_web.GetUserName(), 30, 30, 0);
			g.drawString("New Name: " + modifyInfoString, 30, 50, 0);
			g.setColor(0x00ff00);

			g.drawString("please input your new name", 120, 200, Graphics.TOP
					| Graphics.HCENTER);
			g.drawString("press ok to send", 120, 230, Graphics.TOP | Graphics.HCENTER);
			PaintOkCancel(g, null, EXIT);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveProfile(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("profile", 120, 10, Graphics.HCENTER | Graphics.TOP);
			PaintOkCancel(g, null, EXIT);
			for (int i = 0; i < s_profile.length; i++) {
				g.drawString(s_profile[i], 30,
						30 + i * g.getFont().getHeight(), 0);
			}

			g.setColor(0x00ff00);
			g.drawString(">>", 30, 30 + m_selectItem * g.getFont().getHeight(),
					Graphics.TOP | Graphics.RIGHT);
			g.drawString("press ok to confirm", 120, 200, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 230, Graphics.TOP
					| Graphics.HCENTER);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("please wait...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveMenu(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.drawString("Gameloft Live", 120, 10, Graphics.HCENTER | Graphics.TOP);
			PaintOkCancel(g, null, EXIT);
			for (int i = 0; i < s_gliveMenu.length; i++) {
				g.drawString(s_gliveMenu[i], 30, 80 + i
						* g.getFont().getHeight(), Graphics.BOTTOM | Graphics.LEFT);

			}
			g.setColor(0x00ff00);
			g.drawString(">>", 30, 80 + m_selectItem * g.getFont().getHeight(),
					Graphics.BOTTOM | Graphics.RIGHT);
			g.drawString("press ok to confirm", 120, 200, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 230, Graphics.TOP
					| Graphics.HCENTER);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("logout...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveGetUserInfo(Graphics g) {
		// TODO Auto-generated method stub
		g.drawString("welcome: " + m_web.GetUserName(), 220, 40, Graphics.RIGHT
				| Graphics.TOP);
		g.drawString("load user info..", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
	}

	private void PaintLiveRegister(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.setColor(0xffffff);
			g.drawString("----REGISTER----", 120, 60, Graphics.TOP | Graphics.HCENTER);

			for (int i = 0; i < s_inputRegister.length; i++) {
				g.drawString(s_inputRegister[i], 100, 140 + i
						* g.getFont().getHeight(), Graphics.BOTTOM | Graphics.RIGHT);

				if (m_selectItem == i) {
					g.drawString(">>", 100 - g.getFont().stringWidth(
							s_inputRegister[i]), 140 + i
							* g.getFont().getHeight(), Graphics.BOTTOM | Graphics.RIGHT);

				}
			}
			g.drawString(inputUserName, 100, 140, Graphics.BOTTOM | Graphics.LEFT);
			g.drawString(inputUserPassword, 100, 140 + g.getFont().getHeight(),
					Graphics.BOTTOM | Graphics.LEFT);
			g.drawString(inputUserNationality, 100, 140 + 2 * g.getFont()
					.getHeight(), Graphics.BOTTOM | Graphics.LEFT);
			g.drawString("user@gameloft.com", 100, 140 + 3 * g.getFont()
					.getHeight(), Graphics.BOTTOM |Graphics.LEFT);
			g.drawString("0", 100, 140 + 4 * g.getFont().getHeight(), Graphics.BOTTOM
					| Graphics.LEFT);
			g.setColor(0x00ff00);
			g.drawString("press ok to register", 120, 230, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 260, Graphics.TOP
					| Graphics.HCENTER);
			PaintOkCancel(g, null, EXIT);
		}
			break;
		case OnlineSubstate.WAIT: {
			g.drawString("register...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
		}
			break;

		}
	}

	private void PaintLiveLogin(Graphics g) {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			g.setColor(0xffffff);
			g.drawString("----LOGIN----", 120, 60, Graphics.TOP | Graphics.HCENTER);

			for (int i = 0; i < s_inputLogin.length; i++) {
				g.drawString(s_inputLogin[i], 120, 140 + i * 30, Graphics.BOTTOM
						| Graphics.RIGHT);

				if (m_selectItem == i) {
					g.drawString(">>", 120 - g.getFont().stringWidth(
							s_inputLogin[i]), 140 + i * 30, Graphics.BOTTOM | Graphics.RIGHT);

				}
			}
			g.drawString(inputUserName, 120, 140, Graphics.BOTTOM | Graphics.LEFT);
			g.drawString(inputUserPassword, 120, 170, Graphics.BOTTOM | Graphics.LEFT);
			g.setColor(0x00ff00);
			g.drawString("press ok to login", 120, 200, Graphics.TOP | Graphics.HCENTER);
			g.drawString("press up or down to choose", 120, 230, Graphics.TOP
					| Graphics.HCENTER);
			PaintOkCancel(g, null, EXIT);
		}
			break;
		case OnlineSubstate.WAIT:
		case OnlineSubstate.PROCESS:
			g.drawString("login...", 120, 160, Graphics.HCENTER | Graphics.BOTTOM);
			break;

		}
	}

	private void UpdateGameLoftLive() {
		// handle keep alive
		m_web.update();

		// handle error!
		switch (online_substate) {
		case OnlineSubstate.NET_ERROR:
		case OnlineSubstate.ANALYZE_ERROR:
		case OnlineSubstate.REQUEST_FAIL:

			switch (online_state) {
			case ONLINESTATE_GETUSERINFO:
				setOnlineState(ONLINESTATE_GLIVE_MENU);
				break;
			case ONLINESTATE_MESSAGE_GETCOUNT:

				setOnlineState(ONLINESTATE_GLIVE_MENU);

				break;
			case ONLINESTATE_MESSAGE_GETCOUNTINBOX:
			case ONLINESTATE_MESSAGE_GETHEADINBOX:
				m_binBox = false;
				setOnlineState(ONLINESTATE_MESSAGE);
				break;
			case ONLINESTATE_LOGIN:
				if (m_errorCode == 67) {
					setOnlineState(ONLINESTATE_REGISTER);
					break;
				}

			default:
				if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
					setOnlineSubstate(OnlineSubstate.SET);
					initInput();
				}
			}

			return;

		}

		switch (online_state) {
		case ONLINESTATE_LOGIN: {
			UpdateLiveLogin();
		}
			break;
		case ONLINESTATE_REGISTER:
			UpdateLiveRegister();
			break;
		case ONLINESTATE_GETUSERINFO: {
			UpdateLiveGetUserInfo();
		}
			break;
		case ONLINESTATE_GLIVE_MENU: {
			UpdateLiveMenu();
		}
			break;
		case ONLINESTATE_USER_PROFILE: {
			UpdateLiveProfile();
		}
			break;
		case ONLINESTATE_CHANGE_USERNAME: {
			UpdateLiveChangeUserName();
		}
			break;
		case ONLINESTATE_CHANGE_USERPASSWORD: {
			UpdateLiveChangeUserPassword();
		}
			break;
		case ONLINESTATE_CHANGE_USEREMAIL: {
			UpdateLiveChangeUserEmail();
		}
			break;
		case ONLINESTATE_CHANGE_USERCOUNTRY: {
			UpdateLiveChangeUserCountry();
		}
			break;
		case ONLINESTATE_CHANGE_USEREMAILFLAG: {
			UpdateLiveChangeUserEmailFlag();
		}
			break;
		case ONLINESTATE_CHANGE_USERAVATAR: {
			UpdateLiveChangeUserAvatar();
		}
			break;
		case ONLINESTATE_FRIEND: {
			UpdateLiveFriend();
		}
			break;
		case ONLINESTATE_FRIEND_LIST:
			UpdateLiveFriendList();
			break;
		case ONLINESTATE_FRIEND_SIGLE:
			UpdateLiveFriendSigle();
			break;
		case ONLINESTATE_FRIEND_ADD:
			UpdateLiveFriendAdd();
			break;
		case ONLINESTATE_MESSAGE_GETCOUNT:
			UpdateLiveMessageGetCount();
			break;
		case ONLINESTATE_MESSAGE_SHOWCOUNT:
			UpdateLiveMessageShowCount();
			break;
		case ONLINESTATE_MESSAGE_LIST:
			UpdateLiveMessageList();
			break;
		case ONLINESTATE_MESSAGE_READ:
			UpdateLiveMessageRead();
			break;
		case ONLINESTATE_MESSAGE:
			UpdateLiveMessage();
			break;
		case ONLINESTATE_MESSAGE_GETCOUNTINBOX:
			UpdateLiveMessageGetCountInbox();
			break;
		case ONLINESTATE_MESSAGE_GETHEADINBOX:
			UpdateLiveMessageGetHeadInbox();
			break;
		case ONLINESTATE_MESSAGE_INBOX:
			UpdateLiveMessageInbox();
			break;
		case ONLINESTATE_MESSAGE_WRITE:
			UpdateLiveMessageWrite();
			break;
		case ONLINESTATE_FRIEND_SENDMESSAGE:
			UpdateLiveFriendSendMessage();
			break;
		}

	}

	private void UpdateLiveFriendSendMessage() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET:
			String ret = input(64);
			if (ret.length() > 0) {
				if (m_selectItem == 1) {
					inputMessageTitle = ret;
				} else if (m_selectItem == 2) {
					inputMessageContent = ret;
				}
			}
			if (IsKeyPressed(DEF.KEY_DOWN)) {
				m_selectItem = m_selectItem + 1 >= s_messageWrite.length ? 1
						: m_selectItem + 1;

				initInput();

			} else if (IsKeyPressed(DEF.KEY_UP)) {
				initInput();
				m_selectItem = m_selectItem - 1 < 1 ? s_messageWrite.length - 1
						: m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)
					|| IsKeyPressed(DEF.KEY_SOFT_LEFT)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendOnlineMessageByNum(m_web.GetFriendUserNumber(m_selectFriend),
						GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE,
						inputMessageTitle, inputMessageContent,
						inputMessageContent.length());
			}

			else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_FRIEND_SIGLE);
				m_selectItem=m_friendSigleMenuBegin;
			}
			break;
		case OnlineSubstate.WAIT:
			break;
		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_FRIEND_SIGLE);
			m_selectItem=m_friendSigleMenuBegin;
			break;

		}
	}

	private void UpdateLiveMessageWrite() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET:
			String ret = input(64);
			if (ret.length() > 0) {
				if (m_selectItem == 0) {
					inputMessageReceiver = ret;
				} else if (m_selectItem == 1) {
					inputMessageTitle = ret;
				} else if (m_selectItem == 2) {
					inputMessageContent = ret;
				}
			}
			if (IsKeyPressed(DEF.KEY_DOWN)) {

				m_selectItem = (m_selectItem + 1) % s_messageWrite.length;
				initInput();

			} else if (IsKeyPressed(DEF.KEY_UP)) {
				initInput();
				m_selectItem = m_selectItem - 1 < 0 ? s_messageWrite.length - 1
						: m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)
					|| IsKeyPressed(DEF.KEY_SOFT_LEFT)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendOnlineMessageByName(inputMessageReceiver,
						GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE,
						inputMessageTitle, inputMessageContent,
						inputMessageContent.length());
			}

			else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_MESSAGE);
			}
			break;
		case OnlineSubstate.WAIT:
			break;
		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_MESSAGE);
			break;

		}
	}

//	private void UpdateTemp() {
//		switch (online_substate) {
//		case OnlineSubstate.SET:
//			if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
//				setOnlineState(ONLINESTATE_MESSAGE_SHOWCOUNT);
//			}
//			break;
//		case OnlineSubstate.WAIT:
//			break;
//		case OnlineSubstate.PROCESS:
//			break;
//
//		}
//	}

	private void UpdateLiveMessageGetHeadInbox() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET:
			setOnlineSubstate(OnlineSubstate.WAIT);
			m_web
					.SendGetMessageHeader(
							GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE
									| GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION
									| GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION
									| GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_ALL,
							SHOW_COUNT * m_currentMsgPage, m_msgInPageCount);
			break;

		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_MESSAGE_INBOX);
			break;

		}
	}

	private void UpdateLiveMessageGetCountInbox() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET:
			setOnlineSubstate(OnlineSubstate.WAIT);
			m_web.SendGetMessageCount();
			break;
		case OnlineSubstate.PROCESS: {
			initMessageParam();
			m_userMsgCount = m_web.getMsgCount(
					GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE,
					GLXPlayerWebComponent.USER_MESSAGE_READTYPE_ALL);
			m_friendInvitedMsgCount = m_web
					.getMsgCount(
							GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_ALL);
			m_gameInvitedMsgCount = m_web
					.getMsgCount(
							GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_ALL);
			m_chatInvitedMsgCount = m_web
					.getMsgCount(
							GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_ALL);
			m_allMsgCount = m_userMsgCount + m_friendInvitedMsgCount
					+ m_gameInvitedMsgCount + m_chatInvitedMsgCount;

			if (m_allMsgCount <= 0) {
				m_msgInPageCount = 0;
				setOnlineState(ONLINESTATE_MESSAGE_INBOX);
				// SetOnlineSubState( ONLINE_MESSAGE_INBOX );
			} else {
				if (m_allMsgCount <= SHOW_COUNT) {
					m_msgInPageCount = m_allMsgCount;
				} else {
					m_msgInPageCount = SHOW_COUNT;
				}

				/**
				 * if you wanna send request twice in one state you can write
				 * another new state to send request else must fouce change
				 * substate to wait,donot call
				 * setOnlineSubstate(OnlineSubstate.WAIT);
				 * <p>
				 * handle back in OnRequestSuccess
				 * </p>
				 * 
				 * @see #OnRequestSuccess
				 */
				setOnlineState(ONLINESTATE_MESSAGE_GETHEADINBOX);

			}

		}
			break;

		}
	}

	private void UpdateLiveMessageInbox() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET:
			if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				m_binBox = false;
				setOnlineState(ONLINESTATE_MESSAGE);

			} else if (IsKeyPressed(DEF.KEY_A)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendReadMessage(m_web.getMsgID(SHOW_COUNT
						* m_currentMsgPage + m_selectItem));
			} else if (IsKeyPressed(DEF.KEY_C)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendDeleteMessage(m_web.getMsgID(SHOW_COUNT
						* m_currentMsgPage + m_selectItem));
			} else if (IsKeyPressed(DEF.KEY_UP)) {
				m_selectItem--;

				if (m_allMsgCount <= SHOW_COUNT) {
					if (m_selectItem < 0) {
						m_selectItem = m_allMsgCount - 1;
					}
				} else {
					if (m_selectItem < 0) {
						m_currentMsgPage--;
						if (m_currentMsgPage < 0) {
							if ((m_allMsgCount % SHOW_COUNT) == 0) {
								m_currentMsgPage = m_allMsgCount / SHOW_COUNT
										- 1;
								m_msgInPageCount = SHOW_COUNT;
							} else {
								m_currentMsgPage = m_allMsgCount / SHOW_COUNT;
								m_msgInPageCount = m_allMsgCount % SHOW_COUNT;
							}
						} else {
							m_msgInPageCount = SHOW_COUNT;
						}

						m_selectItem = m_msgInPageCount - 1;
						setOnlineState(ONLINESTATE_MESSAGE_GETHEADINBOX);
					}
				}

			} else if (IsKeyPressed(DEF.KEY_DOWN)) {

				m_selectItem++;

				if (m_allMsgCount <= SHOW_COUNT) {
					if (m_selectItem > m_allMsgCount - 1) {
						m_selectItem = 0;
					}
				} else {
					if (m_currentMsgPage == m_allMsgCount / SHOW_COUNT) {
						if (m_selectItem > m_msgInPageCount - 1) {
							m_currentMsgPage = 0;
							m_selectItem = 0;

							m_msgInPageCount = SHOW_COUNT;
							setOnlineState(ONLINESTATE_MESSAGE_GETHEADINBOX);
						}
					} else {
						if (m_selectItem > SHOW_COUNT - 1) {
							if ((m_currentMsgPage == m_allMsgCount / SHOW_COUNT
									- 1)) {
								if (m_allMsgCount % SHOW_COUNT != 0) {
									m_currentMsgPage++;
									m_selectItem = 0;
									m_msgInPageCount = m_allMsgCount
											% SHOW_COUNT;
								} else {
									m_currentMsgPage = 0;
									m_selectItem = 0;
									m_msgInPageCount = SHOW_COUNT;
								}
							} else {
								m_currentMsgPage++;
								m_selectItem = 0;
								m_msgInPageCount = SHOW_COUNT;
							}

							setOnlineState(ONLINESTATE_MESSAGE_GETHEADINBOX);
						}
					}
				}

			}

			break;
		case OnlineSubstate.WAIT:
			break;
		case OnlineSubstate.PROCESS:
			
			m_divString = null;
			m_divString = DivString(m_web.getMsgBody(), 200);
			setOnlineState(ONLINESTATE_MESSAGE_READ);
			break;

		}
	}

	private void UpdateLiveMessage() {
	
		switch (online_substate) {
		case OnlineSubstate.SET:

			if (IsKeyPressed(DEF.KEY_DOWN)) {

				m_selectItem = (m_selectItem + 1) % s_message.length;

			} else if (IsKeyPressed(DEF.KEY_UP)) {

				m_selectItem = m_selectItem - 1 < 0 ? s_message.length - 1
						: m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)) {
				switch (m_selectItem) {
				case 0:

					setOnlineState(ONLINESTATE_MESSAGE_WRITE);
					break;
				case 1:
					m_binBox = true;
					setOnlineState(ONLINESTATE_MESSAGE_GETCOUNTINBOX);
					break;
				}
			}

			else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_GLIVE_MENU);
			}
			break;
		case OnlineSubstate.PROCESS:

			break;

		}
	}

	private void UpdateLiveMessageRead() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET:

			int position = m_web.getCurrentMsgPosition();
			int t = m_web.getMsgType(position);
			if (t == GLXPlayerWebComponent.USER_MESSAGE_TYPE_GAMEMESSAGE
					|| t == GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE) {
				if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {

					if (!m_binBox)
						setOnlineState(ONLINESTATE_MESSAGE_LIST);
					else {
						setOnlineState(ONLINESTATE_MESSAGE_INBOX);
						// change readstate true with client
						m_web.setMsgReadState(position);
					}
				}
			} else {
				boolean rt = m_web.getMsgReadState(position);
				if (rt == false) {
					// PaintOkCancel(g, "Accept", "Decline");
					if (IsKeyPressed(DEF.KEY_SOFT_LEFT)) {
						setOnlineSubstate(OnlineSubstate.WAIT);
						m_web.sendConfirmUserFriend(m_web
								.getMsgSenderNum(position), 1);
					} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
						setOnlineSubstate(OnlineSubstate.WAIT);
						m_web.sendConfirmUserFriend(m_web
								.getMsgSenderNum(position), 0);
					}

				} else {
					if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
						if (!m_binBox)
							setOnlineState(ONLINESTATE_MESSAGE_SHOWCOUNT);
						else {
							setOnlineState(ONLINESTATE_MESSAGE_INBOX);
							// change readstate true with client
							m_web.setMsgReadState(position);
						}
					}
				}
			}

			break;
		case OnlineSubstate.WAIT:
			break;
		case OnlineSubstate.PROCESS:
			// update message count
			if (!m_binBox)
				setOnlineState(ONLINESTATE_MESSAGE_GETCOUNT);
			else
				setOnlineState(ONLINESTATE_MESSAGE_GETCOUNTINBOX);
			// setOnlineState(ONLINESTATE_MESSAGE_LIST);
			break;

		}
	}

	private void UpdateLiveMessageList() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET:
			int l = m_web.getMsgListCount();

			if (IsKeyPressed(DEF.KEY_DOWN)) {
				if (l != 0)
					m_selectItem = (m_selectItem + 1) % l;

			} else if (IsKeyPressed(DEF.KEY_UP)) {
				if (l != 0)
					m_selectItem = m_selectItem - 1 < 0 ? l - 1 : m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)) {
				if (l != 0) {
					setOnlineSubstate(OnlineSubstate.WAIT);
					m_web.SendReadMessage(m_web.getMsgID(m_selectItem));
				}
			}

			else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_MESSAGE_SHOWCOUNT);
			}
			break;

		case OnlineSubstate.PROCESS:
		
			m_divString = null;
			m_divString = DivString(m_web.getMsgBody(), 200);
			setOnlineState(ONLINESTATE_MESSAGE_READ);

			break;

		}
	}

	private void UpdateLiveMessageShowCount() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET:

			if (IsKeyPressed(DEF.KEY_DOWN)) {
				m_selectItem = (m_selectItem + 1) % s_messageCount.length;

			} else if (IsKeyPressed(DEF.KEY_UP)) {

				m_selectItem = m_selectItem - 1 < 0 ? s_messageCount.length - 1
						: m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)) {

				if (m_selectItem == 0 && m_gameMsgCount > 0) {
					m_currentMsgType = GLXPlayerWebComponent.USER_MESSAGE_TYPE_GAMEMESSAGE;
					setOnlineSubstate(OnlineSubstate.WAIT);
					m_web.SendGetMessageHeader(m_currentMsgType,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_NEW, 0,
							m_gameMsgCount);

				} else if (m_selectItem == 1 && m_userMsgCount > 0) {
					m_currentMsgType = GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE;
					setOnlineSubstate(OnlineSubstate.WAIT);
					m_web.SendGetMessageHeader(m_currentMsgType,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_NEW, 0,
							m_userMsgCount);

				} else if (m_selectItem == 2 && m_friendInvitedMsgCount > 0) {
					m_currentMsgType = GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION;
					setOnlineSubstate(OnlineSubstate.WAIT);
					m_web.SendGetMessageHeader(m_currentMsgType,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_NEW, 0,
							m_friendInvitedMsgCount);

				} else if (m_selectItem == 3 && m_gameInvitedMsgCount > 0) {
					m_currentMsgType = GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION;
					setOnlineSubstate(OnlineSubstate.WAIT);
					m_web.SendGetMessageHeader(m_currentMsgType,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_NEW, 0,
							m_gameInvitedMsgCount);

				} else if (m_selectItem == 4 && m_chatInvitedMsgCount > 0) {
					m_currentMsgType = GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION;
					setOnlineSubstate(OnlineSubstate.WAIT);
					m_web.SendGetMessageHeader(m_currentMsgType,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_NEW, 0,
							m_chatInvitedMsgCount);

				}

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_GLIVE_MENU);
			}
			break;
		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_MESSAGE_LIST);

			break;

		}
	}

	private void UpdateLiveMessageGetCount() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET:
			setOnlineSubstate(OnlineSubstate.WAIT);
			m_web.SendGetMessageCount();
			break;
		case OnlineSubstate.PROCESS: {
			initMessageParam();
			m_gameMsgCount = m_web.getMsgCount(
					GLXPlayerWebComponent.USER_MESSAGE_TYPE_GAMEMESSAGE,
					GLXPlayerWebComponent.USER_MESSAGE_READTYPE_NEW);
			m_userMsgCount = m_web.getMsgCount(
					GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE,
					GLXPlayerWebComponent.USER_MESSAGE_READTYPE_NEW);
			m_friendInvitedMsgCount = m_web
					.getMsgCount(
							GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_NEW);
			m_gameInvitedMsgCount = m_web
					.getMsgCount(
							GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_NEW);
			m_chatInvitedMsgCount = m_web
					.getMsgCount(
							GLXPlayerWebComponent.USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION,
							GLXPlayerWebComponent.USER_MESSAGE_READTYPE_NEW);
			if (m_gameMsgCount > 0 || m_userMsgCount > 0
					|| m_friendInvitedMsgCount > 0 || m_gameInvitedMsgCount > 0
					|| m_chatInvitedMsgCount > 0) {
				setOnlineState(ONLINESTATE_MESSAGE_SHOWCOUNT);
			} else {
				setOnlineState(ONLINESTATE_GLIVE_MENU);
			}

		}
			break;

		}
	}

	private void UpdateLiveFriendAdd() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {
			String ret = input(64);
			if (ret.length() > 0) {
				if (m_selectItem == 0)

					inviteFriendName = input(64);
				else
					inviteMessage = input(64);
			}
			if (IsKeyPressed(DEF.KEY_DOWN)) {
				m_selectItem = (m_selectItem + 1) % 2;

				initInput();
			} else if (IsKeyPressed(DEF.KEY_UP)) {
				initInput();
				m_selectItem = m_selectItem - 1 < 0 ? 1 : m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)) {

				setOnlineSubstate(OnlineSubstate.WAIT);

				m_web.sendAddUserFriend(inviteFriendName, inviteMessage);
			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_FRIEND);
			}

		}
			break;

		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_FRIEND);
			break;

		}
	}

	private void UpdateLiveFriendSigle() {
		switch (online_substate) {
		case OnlineSubstate.SET: {

			if (IsKeyPressed(DEF.KEY_DOWN)) {
				m_selectItem = (m_selectItem + 1) >= s_singleFriendMenu.length ? m_friendSigleMenuBegin
						: m_selectItem + 1;

			} else if (IsKeyPressed(DEF.KEY_UP)) {

				m_selectItem = m_selectItem - 1 < m_friendSigleMenuBegin ? s_singleFriendMenu.length - 1
						: m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)) {
				switch (m_selectItem) {
				case 2:

					setOnlineState(ONLINESTATE_FRIEND_SENDMESSAGE);
					inputMessageReceiver = m_web.GetFriendName(m_selectFriend);
					m_selectItem = 1;
					break;
				}
			}

			else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_FRIEND_LIST);
			}

		}
			break;

		}
	}

	private void UpdateLiveFriendList() {
		switch (online_substate) {
		case OnlineSubstate.SET: {
			int count = m_web.GetFriendsCount();
			if (count > 0) {
				if (IsKeyPressed(DEF.KEY_DOWN)) {

					m_selectItem = (m_selectItem + 1) % count;

				} else if (IsKeyPressed(DEF.KEY_UP)) {

					m_selectItem = m_selectItem - 1 < 0 ? count - 1
							: m_selectItem - 1;
				} else if (IsKeyPressed(DEF.KEY_A)) {
					m_selectFriend = m_selectItem;
					int state = m_web.GetFriendState(m_selectFriend);
					if (state != 2 && state != 3) {
						m_friendSigleMenuBegin = 2;
					} else
						m_friendSigleMenuBegin = 0;

					setOnlineState(ONLINESTATE_FRIEND_SIGLE);
					m_selectItem = m_friendSigleMenuBegin;

				} else if (IsKeyPressed(DEF.KEY_C)) {
					setOnlineSubstate(OnlineSubstate.WAIT);
					m_web.sendDelUserFriend(m_web.GetFriendUserNumber(m_selectItem));
				}
			}

			if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_FRIEND);
			}

		}
			break;
		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_FRIEND);
			online_substate = OnlineSubstate.WAIT;
			m_web.sendGetUserFriends(0, 0);
			break;
		}
	}

	private void UpdateLiveFriend() {
		// TODO Auto-generated method stub
		switch (online_substate) {
		case OnlineSubstate.SET: {

			if (IsKeyPressed(DEF.KEY_DOWN)) {
				m_selectItem = (m_selectItem + 1) % s_friendMenu.length;

			} else if (IsKeyPressed(DEF.KEY_UP)) {

				m_selectItem = m_selectItem - 1 < 0 ? s_friendMenu.length - 1
						: m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)) {
				switch (m_selectItem) {
				case 0:
					setOnlineState(ONLINESTATE_FRIEND_ADD);
					inviteFriendName = "";
					inviteMessage = "";
					break;
				case 1:
					setOnlineSubstate(OnlineSubstate.WAIT);
					m_web.sendGetUserFriends(0, 0);
					break;
				}

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_GLIVE_MENU);
			}

		}
			break;
		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_FRIEND_LIST);
			break;
		}
	}

	private void UpdateLiveLogin() {
		switch (online_substate) {
		case OnlineSubstate.SET: {
			String ret = input(12);
			if (ret.length() > 0) {
				if (m_selectItem == 0)

					inputUserName = ret;
				else
					inputUserPassword = ret;
			}
			if (IsKeyPressed(DEF.KEY_DOWN)) {
				m_selectItem = (m_selectItem + 1) % 2;

				initInput();
			} else if (IsKeyPressed(DEF.KEY_UP)) {
				initInput();
				m_selectItem = m_selectItem - 1 < 0 ? 1 : m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)) {

				setOnlineSubstate(OnlineSubstate.WAIT);
				m_selectItem = 0;
				m_web.SendLogin(inputUserName, inputUserPassword, 1);
			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				ChangeGameState(GS_MAIN_MENU);
			}

		}
			break;

		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_GETUSERINFO);
			break;

		}
	}

	private void UpdateLiveGetUserInfo() {
		switch (online_substate) {
		case OnlineSubstate.SET:
			setOnlineSubstate(OnlineSubstate.WAIT);
			m_web.SendGetUserInfo();
			break;

		case OnlineSubstate.PROCESS:

			// setOnlineState(ONLINESTATE_GLIVE_MENU);
			m_binBox = false;
			setOnlineState(ONLINESTATE_MESSAGE_GETCOUNT);
			break;

		}
	}

	private void UpdateLiveMenu() {
		switch (online_substate) {
		case OnlineSubstate.SET: {
			if (IsKeyPressed(DEF.KEY_DOWN)) {
				m_selectItem = (m_selectItem + 1) % s_gliveMenu.length;

			} else if (IsKeyPressed(DEF.KEY_UP)) {

				m_selectItem = m_selectItem - 1 < 0 ? s_gliveMenu.length - 1
						: m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)) {
				switch (m_selectItem) {
				case 0:
					setOnlineState(ONLINESTATE_USER_PROFILE);
					break;
				case 1:
					setOnlineState(ONLINESTATE_FRIEND);
					break;
				case 2:

					setOnlineState(ONLINESTATE_MESSAGE);
					break;

				}

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendLoginOut();

			}

		}
			break;

		case OnlineSubstate.PROCESS:
			ChangeGameState(GS_MAIN_MENU);
			break;

		}
	}

	private void UpdateLiveProfile() {
		switch (online_substate) {
		case OnlineSubstate.SET: {

			if (IsKeyPressed(DEF.KEY_DOWN)) {
				m_selectItem = (m_selectItem + 1) % s_profile.length;

			} else if (IsKeyPressed(DEF.KEY_UP)) {

				m_selectItem = m_selectItem - 1 < 0 ? s_profile.length - 1
						: m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				if (m_selectItem != 5)
					m_web.SendGetUserInfo();
				else {
					if (avatar == null)
						m_web.SendGetUserAvatar();
					else
						setOnlineState(ONLINESTATE_CHANGE_USERAVATAR);
				}

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_GLIVE_MENU);
			}

		}
			break;

		case OnlineSubstate.PROCESS:
			switch (m_selectItem) {
			case 0:
				setOnlineState(ONLINESTATE_CHANGE_USERNAME);
				break;
			case 1:
				setOnlineState(ONLINESTATE_CHANGE_USERPASSWORD);
				break;
			case 2:
				setOnlineState(ONLINESTATE_CHANGE_USEREMAIL);
				break;
			case 3:
				setOnlineState(ONLINESTATE_CHANGE_USERCOUNTRY);
				break;
			case 4:
				setOnlineState(ONLINESTATE_CHANGE_USEREMAILFLAG);
				break;
			case 5:
				setOnlineState(ONLINESTATE_CHANGE_USERAVATAR);
				break;

			}
			break;

		}
	}

	private void UpdateLiveChangeUserName() {
		switch (online_substate) {
		case OnlineSubstate.SET: {
			modifyInfoString = input(64);
			if (IsKeyPressed(DEF.KEY_A)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendChangeUserName(modifyInfoString);

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_USER_PROFILE);
			}

		}
			break;

		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_USER_PROFILE);
			break;

		}
	}

	private void UpdateLiveChangeUserEmail() {
		switch (online_substate) {
		case OnlineSubstate.SET: {
			modifyInfoString = input(64);
			if (IsKeyPressed(DEF.KEY_A)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendChangeEmail(modifyInfoString);

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_USER_PROFILE);
			}

		}
			break;

		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_USER_PROFILE);
			break;

		}
	}

	private void UpdateLiveChangeUserPassword() {
		switch (online_substate) {
		case OnlineSubstate.SET: {
			modifyInfoString = input(64);
			if (IsKeyPressed(DEF.KEY_A)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendChangePassword(modifyInfoString, modifyInfoString);

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_USER_PROFILE);
			}

		}
			break;

		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_USER_PROFILE);
			break;

		}
	}

	private void UpdateLiveChangeUserEmailFlag() {
		switch (online_substate) {
		case OnlineSubstate.SET: {
			modifyInfoString = input(64);
			if (IsKeyPressed(DEF.KEY_A)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendChangeEmailFlag(modifyInfoString);

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_USER_PROFILE);
			}

		}
			break;

		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_USER_PROFILE);
			break;

		}
	}

	private void UpdateLiveChangeUserCountry() {
		switch (online_substate) {
		case OnlineSubstate.SET: {
			modifyInfoString = input(64);
			if (IsKeyPressed(DEF.KEY_A)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendChangeCountry(modifyInfoString);

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_USER_PROFILE);
			}

		}
			break;

		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_USER_PROFILE);
			break;

		}
	}

	private void UpdateLiveChangeUserAvatar() {
		switch (online_substate) {
		case OnlineSubstate.SET: {

			if (IsKeyPressed(DEF.KEY_A)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				byte[] file = GetData("/logo.png");

				m_web.SendUpLoadFile(file);

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_USER_PROFILE);
			}

		}
			break;

		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_USER_PROFILE);
			break;

		}
	}

	private void UpdateLiveRegister() {
		switch (online_substate) {
		case OnlineSubstate.SET: {
			String ret = input(64);
			if (ret.length() > 0) {
				if (m_selectItem == 0) {
					inputUserName = ret;
				} else if (m_selectItem == 1) {
					inputUserPassword = ret;
				} else if (m_selectItem == 2) {
					inputUserNationality = ret;
				}
			}
			if (IsKeyPressed(DEF.KEY_DOWN)) {
				m_selectItem = (m_selectItem + 1) % 3;

				initInput();
			} else if (IsKeyPressed(DEF.KEY_UP)) {
				initInput();
				m_selectItem = m_selectItem - 1 < 0 ? 2 : m_selectItem - 1;
			} else if (IsKeyPressed(DEF.KEY_A)) {
				setOnlineSubstate(OnlineSubstate.WAIT);
				m_web.SendRegister(inputUserName, inputUserPassword,
						inputUserNationality, "user@gameloft.com", "0");

			} else if (IsKeyPressed(DEF.KEY_SOFT_RIGHT)) {
				setOnlineState(ONLINESTATE_LOGIN);
			}
		}
			break;
		case OnlineSubstate.WAIT:
			break;
		case OnlineSubstate.PROCESS:
			setOnlineState(ONLINESTATE_LOGIN);
			setOnlineSubstate(OnlineSubstate.WAIT);
			m_web.SendLogin(inputUserName, inputUserPassword, 1);
			break;

		}
	}

	public void OnNetworkError() {
		// TODO Auto-generated method stub
		setOnlineSubstate(OnlineSubstate.NET_ERROR);

	}

	public void OnRequestFailure(int func_id, int error_code, String data) {
		// TODO Auto-generated method stub
		System.out.println(func_id + " err " + error_code);
		switch (func_id) {
		case -1:

			setOnlineSubstate(OnlineSubstate.ANALYZE_ERROR);
			return;

		case GLXPlayerWebComponent.FUNCTION_CHANGE_USERNAME:

			if (error_code == 48)
				recommendedName = data;

			break;

		case GLXPlayerWebComponent.FUNCTION_REGISTER: {
			switch (error_code) {
			case 48:
				recommendedName = data;
				inputUserName = data;
				break;

			}

		}
			break;

		}
		setOnlineSubstate(OnlineSubstate.REQUEST_FAIL);

		m_errorCode = error_code;
	}

	/**
	 * do not call send(xxx) in this method,because this method is callde in
	 * send thread!!
	 */
	public void OnRequestSuccess(int func_id, String data) {
		// TODO Auto-generated method stub
		switch (func_id) {
		case GLXPlayerWebComponent.FUNCTION_DEL_MESSAGE:
			setOnlineState(ONLINESTATE_MESSAGE_GETCOUNTINBOX);
			break;
		default:
			setOnlineSubstate(OnlineSubstate.PROCESS);
			break;
		}

	}

	public void OnRequestTimeout(int func_id) {
		// TODO Auto-generated method stub
		setOnlineSubstate(OnlineSubstate.NET_ERROR);
	}

	public void OnRequestSuccessByAray(int func_id, byte[] data) {
		// TODO Auto-generated method stub

		try {
			avatar = Image.createImage(data, 0, data.length);
		} catch (Exception e) {
			System.out.println("Image creat wrong");
		}
		setOnlineSubstate(OnlineSubstate.PROCESS);

	}

	private Image avatar;

	

	static Font _font;

	// 将字符串切分成字符串数组, width为一行的象素宽度
	public static String[] DivString(String inf, int width) {
		// //read texts
		if (inf == null || inf.length() == 0)
			return null;
		int start = 0, end = 0, length = 0;
		String str = inf;
		int tmp = 1;
		while (end < str.length()) {
			if (str.charAt(end) == '|' || str.charAt(end) == '\n'
					|| length + _font.charWidth(str.charAt(end)) >= width) {
				tmp++;
				length = 0;
			}
			length += _font.charWidth(str.charAt(end));
			end++;
		}
		String[] strings = new String[tmp];
		tmp = 0;
		end = 0;
		length = 0;
		while (end < str.length()) {
			while (end < str.length() && str.charAt(end) != '|'
					&& str.charAt(end) != '\n'
					&& length + _font.charWidth(str.charAt(end)) < width) {
				length += _font.charWidth(str.charAt(end));
				end++;
				// get the string
			}
			strings[tmp] = str.substring(start, end);
			tmp++;
			length = 0;
			// move on the string
			if (end < str.length()
					&& (str.charAt(end) == '|' || str.charAt(end) == '\n'))
				end++;
			start = end;
		}
		// /////
		return strings;
	}
}
// testhy
// testhy after tag release001
