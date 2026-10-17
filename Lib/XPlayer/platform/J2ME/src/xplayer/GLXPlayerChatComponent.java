import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.microedition.io.SecureConnection;
import javax.microedition.io.StreamConnection;

import chat.*;

public class GLXPlayerChatComponent extends GLXPlayerComponent implements
		ParserListener, ExceptionListener {
	private class GLXPlayerChatConnector extends Thread {
		/**
		 * The connection to the jabber server
		 */
		private StreamConnection connection = null;

		private int _port;
		private String _hostname;

		/**
		 * Constructor
		 * 
		 * @author Gabriele Bianchi Modified 04/01/2006
		 * @param hostname
		 *            The host to connect to
		 * @param port
		 *            The port to connect to
		 */

		public GLXPlayerChatConnector(String hostname, int port) {
			_hostname = hostname;

			_port = port;
		}

		/**
		 * Modified by Gabriele Bianchi 04/01/2006
		 */
		public void run() {
			StringBuffer connectorStringBuffer;
			if (Datas.isSSL)
				connectorStringBuffer = new StringBuffer("ssl://");
			else
				connectorStringBuffer = new StringBuffer("socket://");
			connectorStringBuffer.append(_hostname);
			connectorStringBuffer.append(":");
			connectorStringBuffer.append(_port);
			connectorStringBuffer.append("");

			String connectorString = connectorStringBuffer.toString().trim();
			System.out.println(connectorString);
			try {
				if (Datas.isSSL)
					connection = (SecureConnection) Connector
							.open(connectorString);

				else
					connection = (StreamConnection) Connector
							.open(connectorString);
				notifyConnect(connection, this.openInputStream(), this
						.openOutputStream());
			} catch (Exception e) {
				e.printStackTrace();
				System.out
						.println("Connessione non riuscita:" + e.getMessage());
				notifyNoConnectionOn("I can't connect, server is unreachable");
			}

			return;
		}

		/**
		 * Method to return the input stream of the connection
		 * 
		 * @return InputStream
		 */

		public InputStream openInputStream() throws IOException {
			return connection.openInputStream();
		}

		/**
		 * Method to return the output stream of the connection
		 * 
		 * @return OutputStream
		 */

		public OutputStream openOutputStream() throws IOException {
			return connection.openOutputStream();
		}

		/**
		 * Method to return the connection
		 * 
		 * @return StreamConnection
		 */

		public StreamConnection getConnection() {
			return connection;
		}
	}

	private IGLXPlayerJabberObserver m_observer;

	// connections & streams: save them for closing in "disconnect"
	private StreamConnection inConn;

	private InputStream is;

	private OutputStream os;
	private CharacterReaderThread readerThread;
	private IWriterThread writerThread;
	GLXPlayerChatStanzaReader stanzaReader; // send him the stanzas
	GLXPlayerChatConnector cinit;

	public void RegisterObserver(Object observe) {
		// TODO Auto-generated method stub
		m_observer = (IGLXPlayerJabberObserver) observe;
	}

	int type_of_connection;

	// constructor
	public GLXPlayerChatComponent() {

	}

	/**
	 * Makes connection Modified by Gabriele Bianchi 04/01/2006
	 */
	public void connect(int state) {
		// *** should ensure it is disconnected...
		cinit = new GLXPlayerChatConnector(Datas.server_name, Datas.port);
		type_of_connection = state;

		cinit.start();
	}

	public void update() {
		// TODO Auto-generated method stub

	}

	/**
	 * Sends out a keep alive space character.
	 */
	public void blank() {
		System.out.println("blank");

		// every time???
		writerThread.write(" ");
	}

	/**
	 * @param Node
	 */
	public void nodeEnd(Node _node) {
		if (_node.parent == null) {
			// end of stream
			System.out.println("<< DOCUMENT ENTITY end: " + _node.toString());
			if (_node.name.equals("stream:stream")) {
				disconnect();
				m_observer.disconnectedEvent();
			} else {
				throw new IllegalStateException();
			}
		} else if (_node.parent.parent == null) {
			// it's a stanza
			System.out.println("<< stanza: " + _node.toString());

			stanzaReader.read(_node);
		} else if (_node.parent.parent.parent == null) {
			// it's a stanza
			System.out.println("<< long stanza: " + _node.toString());

			stanzaReader.read(_node);
		} else if (_node.name != null
				&& (_node.name.equals("message")
						|| _node.name.equals("presence") || _node.name
						.equals("iq"))) {
			System.out.println("<< stanza error: " + _node.toString());
			try {
				// if (_node.name.equals("message"))
				stanzaReader.read(_node);
			} catch (Exception e) {
				System.out.println(e.getMessage());
			}
		} else if (_node.name.equals("stream:features")
				&& stanzaReader.internalstate == 1) {
			try {
				System.out.println("<< stream:features: " + _node.toString());
				stanzaReader.read(_node);
			} catch (Exception e) {
				System.out.println(e.getMessage());
			}

		}
		// else
		// System.out.println("<< generic stanza error: " + _node.toString());

	}

	/**
	 * @param Node
	 */
	public void nodeStart(Node _node) {

		if (_node.parent == null) {
			// document entity (stream) starts...
			System.out.println("<< DOCUMENT ENTITY start: " + _node.toString());
			if (_node.name.equals("stream:stream")) {
				Datas.setSessionId(_node.getValue("id"));
				// IGNORED attributes: xmlns, xmlns:stream, from
				if (_node.getValue("version") == null) {
					// NO SASL
					writerThread
							.write("<iq id='s1' type='get'><query xmlns='jabber:iq:auth'>"
									+ "<username>"
									+ Util.escapeCDATA(Datas.jid.getUsername())
									+ "</username>" + "</query></iq>");

				}

				/*	
				 */
				/*
				 * writerThread.write("<iq id='s1' to='"+Datas.hostname+"'" + "
				 * type=\"set\" id=\"sess_1\">" + "<session
				 * xmlns=\"urn:ietf:params:xml:ns:xmpp-session\"/></iq>");
				 */
				// *** should appear later?
				// midlet.connectedEvent();
			} else {
				disconnect();
				m_observer.disconnectedEvent();
				// throw new IllegalStateException("<stream:stream> expected");
			}
		}
	}

	public void prologEnd(Node _node) {
		System.out.println("<< PROLOG: " + _node.toString());

		// I don't care about the prolog, should I?
	}

	/**
	 * Exceptions handling
	 * 
	 * @param Exception
	 */
	public void reportException(Thread _t, Exception _e) {
		System.err.println("Error " + _e);
		/*
		 * Alert a = new Alert("Error", _e.toString(), null, AlertType.ERROR);
		 * Display.getDisplay(midlet).setCurrent(a);
		 */
	}

	/**
	 * Exceptions handling Modified by Gabriele Bianchi 05/01/2006
	 * 
	 * @param Exception
	 */
	public void reportException(Exception _e) {
		System.err.println("Error " + _e);
		terminateStream();
		m_observer.unauthorizedEvent("Error: " + _e.getMessage());
	}

	/**
	 * @param Exception
	 * @author Gabriele Bianchi
	 */
	public void reportRegistrationError(Exception _e, boolean disconnect) {
		System.err.println("Error " + _e);
		if (disconnect)
			terminateStream();
		m_observer.registrationFailure(_e, disconnect);

	}

	/**
	 * Notifies connection to midlet Modified by Gabriele Bianchi 05/01/2006
	 */
	public void notifyConnect(StreamConnection _inConn, InputStream _is,
			OutputStream _os) {
		inConn = _inConn;
		// outConn = _inConn;
		is = _is;
		os = _os;
		System.out.println("Connessione effettuata");
		try {

			Parser parser = new Parser(this, this);

			// TODO: discriminare se vuole httpBind!! e creare l'obj
			// httpbindthread

			readerThread = new CharacterReaderThread(is, parser, this);

			writerThread = new WriterThread(os, this);
			writerThread.start();
			Datas.writerThread = writerThread; // Datas is already initialized

			/*
			 * String server; if (Datas.subdomain != null) { server =
			 * Datas.subdomain; } else server = Datas.server_name;
			 */
			// stream start
			writerThread
					.write("<?xml version='1.0'?>"
							+ "<stream:stream to="
							+ "'"
							+ Datas.hostname
							+ "'"
							+ " xmlns='jabber:client'"
							+ " xmlns:stream='http://etherx.jabber.org/streams' version='1.0'>");

			stanzaReader = new GLXPlayerChatStanzaReader(this, m_observer,
					type_of_connection);
			readerThread.start();

		} catch (Exception e) {
			System.err.println("Stream Init Error " + e);
		}
	}

	public void notifyNoConnectionOn(String reason) {
		m_observer.unauthorizedEvent(reason);
	}

	public void terminateStream() {
		writerThread.write("</stream:stream>");
		disconnect();
	}

	/**
	 * Disconnect
	 */
	public void disconnect() {
		try {
			// unlock the WriterThread and finish it

			writerThread.terminate();

			// closes the streams
			if (is != null) {
				// writerThread.write("</stream:stream>");
				is.close();
				os.close();
			}

			// closes the connection
			if (inConn != null)
				inConn.close();

		} catch (IOException e) {
		}
	}

	// ///////////////////////////////////////////////////////////

	public Chat getChat(int index) {
		return ((Chat) Datas.conversations.elementAt(index));
	}

	public boolean getIsMucRoom(int index) {
		return getChat(index).isMulti;
	}

	public Vector getChatMsg(int index) {
		return getChat(index).messages;
	}

	public void sendMessage(String message, int index) {
		if (message == null || message.length() == 0)
			return;
		Message msg = new Message("", message);
		getChat(index).appendFromMe(msg);
	}

	public String getChatName(int index) {
		Chat temp = (Chat) Datas.conversations.elementAt(index);
		return temp.littleName;
	}

	public int getChatCount() {
		return Datas.conversations.size();
	}

	public int getChatInVector(Conversation _conv) {

		return Datas.conversations.indexOf(_conv);
	}

	public int creatNewSingleChat(String friendName) {

		try {

			Jid jid = (Jid) Datas.roster.get(friendName.toLowerCase()
					+ "@gameloft.com");
			if (jid == null)
				return -1;
			Enumeration iter = Datas.conversations.elements();
			int index = 0;
			while (iter.hasMoreElements()) {
				index++;
				Chat temp = (Chat) iter.nextElement();
				if (temp.name.toLowerCase().equals(jid.getLittleJid())) {

					return index;
				}
			}

			SingleChat temp = new SingleChat(jid, "chat", "");

			Datas.conversations.addElement(temp);

			return Datas.conversations.size() - 1;
		} catch (OutOfMemoryError e) {
			Datas.multichat.clear();
			Datas.conversations.removeAllElements();
			Datas.conversations.trimToSize();
			Datas.server_services.removeAllElements();
			Datas.conversations.trimToSize();
			terminateStream();
			m_observer.notifyOutOfMemory();
			return -1;
			// TODO Auto-generated catch block

		}

	}

	public int closeSingleChat(int index) {
		if (Datas.conversations.size() > 0
				&& index < Datas.conversations.size()) {
			Chat rev = (Chat) Datas.conversations.elementAt(index);
			if (rev.isMulti) {

				ChatHelper.groupChatLeave(rev.name);
				Datas.multichat.remove(rev.name);

			}
			Datas.conversations.removeElementAt(index);

		}
		return index - 1 >= 0 ? index - 1 : 0;
	}

	// ///////////////////////////////////////////////////////////
	public boolean foundConference() {
		ChatHelper.serviceRequest(Datas.mucServer);
		return true;
	}

	public int getRoomCount() {
		if (Datas.rooms != null) {
			return Datas.rooms.size();
		}
		return 0;
	}

	public void createRoom(String chatRoom) {
		ChatHelper.groupChatJoin(Datas.jid.getUsername(), chatRoom
				.toLowerCase(), Datas.mucServer);

	}

	public void joinRoom(int selectIndex) {
		String room = (String) Datas.rooms.elementAt(selectIndex);
		Conversation c = (Conversation) Datas.multichat.get(room);
		if (c != null) {
			m_observer.newConversationEvent(c);
		} else
			ChatHelper.groupChatJoin(room);
		c = null;

	}
	public void joinRoom(String room) {
		
		Conversation c = (Conversation) Datas.multichat.get(room);
		if (c != null) {
			m_observer.newConversationEvent(c);
		} else
			ChatHelper.groupChatJoin(room);
		c = null;

	}
	public void inviteUser(String name, int roomindex) {
		Chat temp = (Chat) Datas.conversations.elementAt(roomindex);
		ChatHelper.inviteContact(name.toLowerCase() + "@" + Datas.hostname,
				temp.name);
	}
	public void declineUser(String name,String room)
	
	{
		ChatHelper.declineContact(name.toLowerCase(),
				room.toLowerCase());	
		
	}

}
