import chat.*;

/**
 * 
 * @author yi.he
 * 
 */
public interface IGLXPlayerJabberObserver  {
	final static int OFFLINE = 0;
	final static int ONLINE = 1;
	final static int ROSTER = 2;
	final static int CONVERSATION = 3;
	final static int SUBSCRIPTION = 4;
	final static int PARAMS = 5;
	final static int WAIT_CONNECT = 6;
	final static int WAIT_DISCONNECT = 7;
	final static int ROSTER_DETAILS = 8;
	final static int MULTI_CHAT = 9;
	final static int INVITATION = 10;
	final static int OPTIONS = 11;
	final static int JUD = 12;

	// ROSTER
	public void notifyRoster();

	public void notifyJudInfo(String info);

	// CONVERSATION
	/**
	 * A roster (or group of rosters) has set up a conversation with the user
	 */
	public void newConversationEvent(Conversation _conversation);


	/**
	 * A new message has been received for the given conversation.
	 */
	public void newMessageEvent(Conversation _conversation, int tab);

	public void newComposingEvent(Conversation _conv);

	/**
	 * A new invitation has been received.
	 * 
	 * @author Gabriele Bianchi
	 */
	public void newInvitationEvent(String from, String room);

	/**
	 * An error occured on the given conversation
	 */
	public void notifyError(Conversation _conversation, Message _errorMessage);

	public void disconnectedEvent();

	public void connectedEvent();

	// PRESENCE

	public void notifyPresence(Jid _roster, String _presence);

	public void notifyPresenceError(String _error);



	// CONNECTION
	public void unauthorizedEvent(String reason);

	public void registrationFailure(Exception _e, boolean disconnect);

	public void notifyOutOfMemory();
	public void notifyFindRoom();
}
