package chat;


import java.util.Vector;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.Hashtable;

import javax.microedition.midlet.MIDlet;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotOpenException;
import javax.microedition.rms.InvalidRecordIDException;


/**
 * Storage class Modified by Gabriele Bianchi 04/01/2006
 */
public class Datas {

	// stream connection parameters

	public static Jid jid;
	public static String hostname;
	public static String server_name;
	public static String subdomain = null;
	public static String mucServer;
	public static int port;
	public static boolean isSSL = false;
	public static boolean isHTTP = false;
	public static boolean noData = true;
	public static String avatarFile = null;
	private static String password;
	private static String sessionId;

	// other fields
	public static Vector conversations;
	public static Vector rooms;
	public static Vector server_services;
	public static Vector gateways;
	public static Hashtable multichat;
	public static Contents images;
	public static Hashtable roster; // rosters, key= jid without resource
	public static Vector roster_vector;
	// public static RosterClassification rosterClasses;
	public static IWriterThread writerThread;
	public static boolean readRoster = false;

	public static int HTTP_PORT = 80;
	public static int TCP_PORT = 5222;
	public static int SSL_PORT = 5223;
	public static String customPort = TCP_PORT + "";

	/**
	 * initialization ..........
	 */
	public static void load(MIDlet midlet, String userName, String userpassword) {
		// init the variables from RMS...
		readRoster=false;
		conversations = new Vector(1, 1);
		roster = new Hashtable(2);
		multichat = new Hashtable(1);
		server_services = new Vector(1, 1);
		Datas.gateways = new Vector(3, 1);
		images = new Contents();
		port = TCP_PORT;
		userName=userName.toLowerCase();
		jid = new Jid(userName + "@gameloft.com/GLXChatJ2me");

		password = userpassword;
		server_name =midlet.getAppProperty("ChatServer");
		hostname=midlet.getAppProperty("ChatDomian");
		mucServer=midlet.getAppProperty("ChatRoomDomian");
		subdomain=hostname;
	}

	/**
	 * set jid AVATAR
	 * 
	 */
	public static void setJidAvatar() {
		try {
			InputStream in = null;
			Object o = new Object();
			if (avatarFile == null)
				return;
			in = o.getClass().getResourceAsStream(avatarFile);

			if (in.available() > 8000) {
				System.out.println("Error: avatar too big");
				// TODO lanciare alert all'utente
			} else {

				byte[] data = new byte[in.available()];
				in.read(data);
				jid.setAvatar(data);
			}
		} catch (Exception e) {
			System.out.println("cannot load avatar");
		}
	}

	/**
	 * Computes the password in conformance with JEP-0078
	 * 
	 * @return String (digest password)
	 */
	public static String getDigestPassword() {
		return Util.sha1(sessionId + password);
	}

	/**
	 * Gets plain password
	 * 
	 * @return String
	 */
	public static String getPassword() {
		return password;
	}

	/**
	 * Sets the password
	 * 
	 * @param String
	 */
	public static void setPassword(String _val) {
		// *** escape the password?
		password = _val;
	}

	/**
	 * Sets the session id
	 * 
	 * @param String
	 */
	public static void setSessionId(String _val) {
		sessionId = _val;
	}

	/**
	 * Inserts in Datas a new roster item Modified by Gabriele Bianchi
	 * 04/01/2006
	 * 
	 * @param _jid
	 */
	public static void registerRoster(Jid _jid) {

		roster.put(_jid.getLittleJid(), _jid);

		return;
	}

	/**
	 * Get all roster items
	 * 
	 * @param update
	 */
	public static Vector createRosterVector(boolean update) {
		if (!update)
			return roster_vector;
		Enumeration contacts = Datas.roster.elements();
		roster_vector = new Vector();
		while (contacts.hasMoreElements()) {
			roster_vector.addElement((Jid) contacts.nextElement());
		}

		return roster_vector;
	}

	/**
	 * Get online roster items
	 * 
	 * @param update
	 */
	public static Vector createOnlineRosterVector(boolean update) {
		if (!update)
			return roster_vector;
		Enumeration contacts = Datas.roster.elements();
		roster_vector = new Vector();
		while (contacts.hasMoreElements()) {
			Jid temp = (Jid) contacts.nextElement();
			if (!Presence.getPresence("unavailable").toLowerCase().equals(
					temp.getPresence().toLowerCase()))
				roster_vector.addElement(temp);
		}

		return roster_vector;
	}

	/**
	 * Save user jid info in persistent storage (RMS)
	 * 
	 * @author Gabriele Bianchi
	 * @return boolean
	 */
	public static boolean saveRecord() {
		RecordStore rs = null;
		try {
			rs = RecordStore.openRecordStore("MyOptions", true);
		} catch (RecordStoreException ex) {
			return false;
		}
		// insert
		try {
			if (rs.getNumRecords() == 0) {

				byte bytes[] = jid.getFullJid().getBytes();
				try {
					rs.addRecord(bytes, 0, bytes.length);
					bytes = password.getBytes();
					rs.addRecord(bytes, 0, bytes.length);
					bytes = server_name.getBytes();
					rs.addRecord(bytes, 0, bytes.length);
					if (isSSL) {
						bytes = "yes".getBytes();
						port = SSL_PORT;
					} else if (isHTTP) {
						bytes = "http".getBytes();
						port = HTTP_PORT;
					} else {
						bytes = "no".getBytes();
						port = TCP_PORT;
					}
					rs.addRecord(bytes, 0, bytes.length);
					if (jid.getMail() == null || jid.getMail().equals(""))
						bytes = "null".getBytes();
					else
						bytes = jid.getMail().getBytes();
					rs.addRecord(bytes, 0, bytes.length);
					if (subdomain == null)
						bytes = "null".getBytes();
					else
						bytes = subdomain.getBytes();
					rs.addRecord(bytes, 0, bytes.length);
					if (Datas.avatarFile == null)
						bytes = "null".getBytes();
					else
						bytes = Datas.avatarFile.getBytes(); // AVATAR
					rs.addRecord(bytes, 0, bytes.length);

					if (Datas.customPort.equals("")) {
						customPort = String.valueOf(port); // custom port
					} else
						port = Integer.parseInt(customPort);
					bytes = customPort.getBytes(); // custom port
					rs.addRecord(bytes, 0, bytes.length);

					noData = false; // data set
				} catch (RecordStoreException exc2) {
					System.out.println(exc2.getMessage());
					try {
						rs.closeRecordStore();
					} catch (RecordStoreException exc1) {
						System.out.println(exc1.getMessage());
						return false;
					}
					return false;
				}

			}
			// update
			else {
				byte bytes[] = jid.getFullJid().getBytes();
				try {
					rs.setRecord(1, bytes, 0, bytes.length);
					bytes = password.getBytes();
					rs.setRecord(2, bytes, 0, bytes.length);
					bytes = server_name.getBytes();
					rs.setRecord(3, bytes, 0, bytes.length);
					if (isSSL) {
						bytes = "yes".getBytes();
						port = SSL_PORT;
					} else if (isHTTP) {
						bytes = "http".getBytes();
						port = HTTP_PORT;
					} else {
						bytes = "no".getBytes();
						port = TCP_PORT;
					}
					rs.setRecord(4, bytes, 0, bytes.length);
					if (jid.getMail() == null || jid.getMail().equals(""))
						bytes = "null".getBytes();
					else
						bytes = jid.getMail().getBytes();
					rs.setRecord(5, bytes, 0, bytes.length);
					if (subdomain == null)
						bytes = "null".getBytes();
					else
						bytes = subdomain.getBytes();
					rs.setRecord(6, bytes, 0, bytes.length);

					// AVATAR
					if (avatarFile == null)
						bytes = "null".getBytes();
					else
						bytes = avatarFile.getBytes();
					rs.setRecord(7, bytes, 0, bytes.length);

					if (customPort.equals("")) {
						// bytes = "null".getBytes();
						customPort = String.valueOf(port);
					} else
						port = Integer.parseInt(customPort);
					bytes = customPort.getBytes();
					rs.setRecord(8, bytes, 0, bytes.length);

					noData = false; // data set
				} catch (RecordStoreException exc1) {
					System.out.println(exc1.getMessage());
					try {
						rs.closeRecordStore();
					} catch (RecordStoreException exc2) {
						System.out.println(exc2.getMessage());
						return false;
					}
					return false;
				}
			}
		} catch (Exception exc3) {

			try {
				rs.closeRecordStore();
			} catch (RecordStoreException exc1) {
				return false;
			}
			return false;
		}
		try {
			rs.closeRecordStore();
		} catch (RecordStoreException exc1) {
			return false;
		}
		return true;
	}

	public static boolean isGateway(String gtw) {
		if (gateways.size() == 0) {
			if (gtw.indexOf("@") == -1 && !gtw.toLowerCase().equals(hostname.toLowerCase())) // weak
				// condition!!!
				return true;
			else
				return false;

		}

		for (int i = 0; i < gateways.size(); i++) {
			if (gtw.toLowerCase().equals(((String) gateways.elementAt(i)).toLowerCase()))
				return true;
		}
		return false;
	}

}
