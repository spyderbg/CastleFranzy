
import java.io.*;
import javax.microedition.io.*;

import java.util.Vector;

/**
 * TCP connection class.
 * <p>
 * Provides services for creating and managing TCP connections to the
 * multiplayer server.
 * </p>
 * <p>
 * How to use:
 * </p>
 * <ol>
 * <li>Create an instance</li>
 * <li>Call <code>connect()</code> to establish the connection.</li>
 * <li>Send the establish connection packages, one on the receive connection
 * and one on the sending connection.</li>
 * <li>Call <code>sendPacket()</code> send a message to the server.</li>
 * <li>Call <code>recvPacket()</code> to check for server messages.</li>
 * <li>Call <code>disconnect</code> to disconnect from the multiplayer server
 * and cleanup resources.</li>
 * </ol>
 * 
 * @author World Online Team <<online-adm@gameloft.com>>
 * @author yi.he <<yi.he@gameloft.com>>
 * @version 0.0.1
 *          <DT><B>Copyright:</B></DT>
 *          <DD>Gameloft SA � 2006</DD>
 */

final class GLXPlayerTcp extends Thread {

	private class TCPRecv extends Thread {
		// Nested class using to receive the packages from the server.
		// The sending and receiving of messages are done on different threads.

		// The connection input stream.
		private InputStream is;

		// Vector to hold the incoming messages from the server.
		private Vector v;

		// True if the thread is still running.
		private boolean m_running;

		// Buffer for receiving raw data from the input stream on each read
		// operation.
		private byte localReceiveBuffer[];

		// The length of data received from the input stream.
		private int len;

		// The current offset in the receive buffer.
		private int receiveBufferOffset;

		// The receive buffer, all data read from the server is placed in this
		// buffer and interpreted afterwards.
		private byte receiveBufferQueue[];

		// The constructor for the receiving class.
		public TCPRecv(InputStream inputstream, Vector vector) {
			is = inputstream;
			v = vector;
			localReceiveBuffer = new byte[255];
		}

		/**
		 * This appends the byte[]buffer with the data from net and returns new
		 * buffer starting with offset <i>offset</i>
		 * 
		 * @param buffer
		 *            byte[]
		 * @param offset
		 *            int
		 * @return byte[]
		 */
		void recvBytes() {
			len = -2;

			byte numByte = 0;

			try {
				if (GLXPlayerConfig.Configuration.ENABLE_READ_COMMUNICATION_SUPPORT) {
					numByte = (byte) is.read();
					len = is.read(localReceiveBuffer, 0, numByte);
				} else {
					len = is.read(localReceiveBuffer);
				}

			} catch (Exception e) {
				m_bError = true;

				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					m_bErrorString = "Recv th: "
							+ (e.getMessage() == null ? e.toString() : e
									.getMessage());
				}

				m_running = false;
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					System.out.println("TCP: recv exception: " + e.toString());
					e.printStackTrace();
				}
			}
			if (len <= 0) {
				m_running = false;
				return;
			}

			if (GLXPlayerConfig.Configuration.ENABLE_READ_COMMUNICATION_SUPPORT) {
				len++;
			}

			byte retbuf[] = new byte[len + receiveBufferQueue.length
					- receiveBufferOffset];
			System.arraycopy(receiveBufferQueue, receiveBufferOffset, retbuf,
					0, receiveBufferQueue.length - receiveBufferOffset);

			if (GLXPlayerConfig.Configuration.ENABLE_READ_COMMUNICATION_SUPPORT) {
				retbuf[receiveBufferQueue.length - receiveBufferOffset] = numByte;

				System.arraycopy(localReceiveBuffer, 0, retbuf,
						(receiveBufferQueue.length - receiveBufferOffset) + 1,
						len - 1);
			} else {
				System.arraycopy(localReceiveBuffer, 0, retbuf,
						receiveBufferQueue.length - receiveBufferOffset, len);
			}

			receiveBufferOffset = 0;
			receiveBufferQueue = retbuf;
			yield();
		}

		// Get one byte from the receive buffer.
		int getByte() {
			while (m_running
					&& (receiveBufferQueue.length - receiveBufferOffset) < 1) {
				recvBytes();
			}

			return m_running ? receiveBufferQueue[receiveBufferOffset++] : -1;
		}

		// Get more bytes from the receive buffer.
		byte[] getBytes(int length) {
			while (m_running
					&& (receiveBufferQueue.length - receiveBufferOffset) < length) {
				recvBytes();
			}
			byte retval[] = new byte[length];
			System.arraycopy(receiveBufferQueue, receiveBufferOffset, retval,
					0, length);
			receiveBufferOffset += length;
			return retval;
		}

		// Start the thread.
		public void start() {
			m_running = true;
			super.start();
		}

		// Main run method for the thread.
		public void run() {
			receiveBufferQueue = new byte[0];
			receiveBufferOffset = 0;

			int packetlength;
			byte packet[];
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("TCPRecv: Starting receiver");
			}
			while (m_running) {
				packetlength = getByte();
				if (packetlength == -1) {
					m_running = false;
					continue;
				}
				if (packetlength < 0) {
					packetlength += 256;
				}
				packet = getBytes(packetlength);
				synchronized (v) {
					v.addElement(packet);
				}
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					System.out.println("TCPRecv: Received " + packetlength
							+ " bytes packet:" + new String(packet));
				}
				yield();
			}

		}
	}// TCPRecv

	// Public fields, read only !

	/**
	 * <code>true</code> if the connection is established with the multiplayer
	 * server, <code>false</code> otherwise.
	 */
	public boolean m_connected;

	/**
	 * Tells wether an error occured during the TCP/IP transaction.
	 * <p>
	 * It is set to <b><code>true</code></b> if the there was an error in
	 * the communication with the multiplayer server, <b><code>false</code></b>
	 * otherwise.
	 * </p>
	 */
	public boolean m_bError;

	/**
	 * Error string, used only when debugging is enabled
	 */
	public String m_bErrorString;

	// The receiving class.
	private TCPRecv m_receiveThread;

	// Vector for holding the incoming messages
	protected Vector incomingQueue;
	// Vector for holding the outgoing messages
	protected Vector outgoingQueue;

	// When using ENABLE_DUAL_TCP, we need to send one package on the receive
	// connection. There is no need to make a queue of messages, since we'll
	// only
	// send one package.
	protected byte[] outgoing_data_on_receive_connection;

	// The multiplayer server url.
	protected String url;

	// The TCP connection object used for communication with the multiplayer
	// server. This object is shared by the input and output streams, even if
	// the
	// receiving operation runs in a different thread.
	protected StreamConnection connection = null;
	// The input stream for the TCP connection.
	protected InputStream istream = null;
	// The output stream for the TCP connection.
	protected OutputStream ostream = null;

	// Used only in conjunction with ENABLE_DUAL_TCP - separate connections for
	// sending and receiving data.
	protected StreamConnection receive_connection = null;
	// The input stream for the TCP connection.
	protected InputStream receive_istream = null;
	// The output stream for the TCP connection.
	protected OutputStream receive_ostream = null;

	/**
	 * Constructor for the <code>TCP</code> class.
	 * 
	 * @param url
	 *            The multiplayer server url.
	 */
	public GLXPlayerTcp(String url) {
		m_bError = false;

		if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
			m_bErrorString = null;

			if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
				System.out.println("Using dual connection tcp");
			} else {
				System.out.println("Using single connection tcp");
			}
		}

		incomingQueue = new Vector();
		outgoingQueue = new Vector();
		this.url = url;

		if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
			outgoing_data_on_receive_connection = null;
		}
	}

	/**
	 * Puts the data as packet to the sending queue
	 * 
	 * @param data
	 *            byte[]
	 */
	public void sendPacket(byte[] data) {
		synchronized (outgoingQueue) {
			outgoingQueue.addElement(data);
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("Adding packet to outgoing queue");
			}
		}
	}

	/**
	 * Sends the establish message on the receive connection.
	 * <p>
	 * In order to establish a connection, two packages must be sent one on the
	 * receive connection and the other on the send connection.
	 * </p>
	 * <p>
	 * The server will match them an create a new connection.
	 * </p>
	 * 
	 * @param data
	 *            byte[]
	 */
	public void sendEstablishConnectionPackageOnReceive(byte[] data) {
		if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
			outgoing_data_on_receive_connection = data;
		} else {
			sendPacket(data);
		}
	}

	/**
	 * Gets the first available packet from receiving queue, or null
	 * 
	 * @return byte[]
	 */
	public byte[] recvPacket() {
		byte returndata[] = null;
		synchronized (incomingQueue) {
			if (incomingQueue.size() > 0) {
				returndata = (byte[]) incomingQueue.firstElement();
				incomingQueue.removeElementAt(0);
			}
		}
		return returndata;
	}

	/**
	 * Establish the connection.
	 */
	public void connect() {
		if (m_connected == false) {
			try {
				new Thread(this).start();
			} catch (Exception e) {
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					System.out.println("TCP: connect exception: "
							+ e.toString());
					e.printStackTrace();
				}
			}

		}
	}

	/**
	 * Close the connection.
	 */
	public void disconnect() {
		if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
			System.out.println("disconnecting tcp ... ");
		}

		m_connected = false;
	}

	/**
	 * Closes the streams and frees all the resources.
	 */
	private void cleanup() {
		if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
			System.out.println("cleanup tcp .. ");
		}

		// stop the receiving thread ...
		if (m_receiveThread != null) {
			m_receiveThread.m_running = false;
		}

		// close the streams ..
		try {
			if (istream != null) {
				istream.close();
			}
		} catch (Exception e) {
		}
		try {
			if (ostream != null) {
				ostream.close();
			}
		} catch (Exception e) {
		}

		// close the connection ..
		try {
			if (connection != null) {
				connection.close();
			}
		} catch (Exception e) {
		}

		istream = null;
		ostream = null;
		connection = null;

		if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
			// close the streams ..
			try {
				if (receive_istream != null) {
					receive_istream.close();
				}
			} catch (Exception e) {
			}
			try {
				if (receive_ostream != null) {
					receive_ostream.close();
				}
			} catch (Exception e) {
			}

			// close the connection ..
			try {
				if (receive_connection != null) {
					receive_connection.close();
				}
			} catch (Exception e) {
			}

			receive_istream = null;
			receive_ostream = null;
			receive_connection = null;
		}

		synchronized (outgoingQueue) {
			while (outgoingQueue.size() > 0) {
				outgoingQueue.removeElementAt(0);
			}
		}
		synchronized (incomingQueue) {
			while (incomingQueue.size() > 0) {
				incomingQueue.removeElementAt(0);
			}
		}

		try {
			m_receiveThread.join();
		} catch (Exception e) {
		}
	}

	/**
	 * Main running function for the communication thread.
	 */
	public void run() {
		m_connected = false;
		m_bError = false;

		// first, open the connection to the server url
		try {
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("TCP: Opening connection to " + url);
			}
			connection = (StreamConnection) Connector.open(url);
			if (connection == null) {
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					m_bErrorString = "StreamConnection.open to " + url;
				}

				m_bError = true;
				return;
			}

			if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					System.out.println("TCP: Opening receive connection to "
							+ url);
				}
				receive_connection = (StreamConnection) Connector.open(url);
				if (receive_connection == null) {
					if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
						m_bErrorString = "StreamConnection.open to " + url;
					}

					m_bError = true;
					return;
				}
			}
		} catch (Exception e) {
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("TCP: run exception: " + e.toString());
				e.printStackTrace();
			}
			m_bError = true;
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				m_bErrorString = "StreamConnection.open to " + url
						+ " exception: " + e;
			}
			return;
		}

		// open the connection output stream for writing messages
		ostream = null;

		if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
			receive_ostream = null;
		}

		try {
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("TCP: Opening output stream");
			}

			ostream = connection.openOutputStream();
			if (ostream == null) {
				m_bError = true;
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					m_bErrorString = "StreamConnection.openOutputStream to "
							+ url;
				}
				return;
			}

			if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
				receive_ostream = receive_connection.openOutputStream();
				if (receive_ostream == null) {
					m_bError = true;
					if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
						m_bErrorString = "StreamConnection.openOutputStream to "
								+ url;
					}
					return;
				}
			}
		} catch (Exception e) {
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("TCP: run exception: " + e.toString());
				e.printStackTrace();
			}

			m_bError = true;

			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				m_bErrorString = "StreamConnection.openOutputStream to " + url
						+ " exception: " + e;
			}
			return;
		}

		// Open the connection input stream for receiving messages.
		istream = null;

		if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
			receive_istream = null;
		}

		try {
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("TCP: Opening input stream");
			}

			istream = connection.openInputStream();
			if (istream == null) {
				m_bError = true;
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					m_bErrorString = "StreamConnection.openInputStream to "
							+ url;
				}
				return;
			}

			if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
				receive_istream = receive_connection.openInputStream();
				if (receive_istream == null) {
					m_bError = true;
					if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
						m_bErrorString = "StreamConnection.openInputStream to "
								+ url;
					}
					return;
				}
			}
		} catch (Exception e) {
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("TCP: run exception: " + e.toString());
				e.printStackTrace();
			}

			m_bError = true;
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				m_bErrorString = "StreamConnection.openInputStream to " + url
						+ " exception: " + e;
			}
			return;
		}

		// we are now connected !
		m_connected = true;
		if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
			System.out
					.println("TCP: Connection established, starting recv thread");
		}

		// we have all streams attached, create secondary thread for receiving
		// data
		if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
			m_receiveThread = new TCPRecv(receive_istream, incomingQueue);
		} else {
			m_receiveThread = new TCPRecv(istream, incomingQueue);
		}

		m_receiveThread.start();

		// this exit will be actually triggered by a call to disconnect()
		// from another thread
		byte[] packet;
		byte psize[] = new byte[1];
		while (m_connected && m_receiveThread.m_running) {
			synchronized (outgoingQueue) {
				packet = null;
				if (outgoingQueue.size() > 0) {
					// get the next packet to be sent
					packet = (byte[]) outgoingQueue.firstElement();
					outgoingQueue.removeElementAt(0);
				}
			}
			if (packet == null) {
				yield();
				continue;
			}
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("TCP: new packet to send");
			}

			psize[0] = (byte) (packet.length & 0xFF);

			// write the packet to the output stream
			try {
				ostream.write(psize);
				ostream.write(packet);

			} catch (Exception e) {
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					System.out.println("TCP: send exception: " + e.toString());
					e.printStackTrace();
				}

				m_bError = true;
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					m_bErrorString = "Send th: "
							+ (e.getMessage() == null ? e.toString() : e
									.getMessage());
				}

				m_connected = false;
			}
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("TCP: Sent " + psize[0] + " bytes packet:"
						+ new String(packet));
			}

			if (GLXPlayerConfig.Configuration.ENABLE_DUAL_TCP) {
				// see if we have something to send on the receive conneciton
				if (outgoing_data_on_receive_connection != null) {
					psize[0] = (byte) (outgoing_data_on_receive_connection.length & 0xFF);

					// write the packet to the output stream of the receive
					// connection
					try {
						receive_ostream.write(psize);
						receive_ostream
								.write(outgoing_data_on_receive_connection);
					} catch (Exception e) {
						if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
							System.out
									.println("TCP: send exception when sending establish connection on receive: "
											+ e.toString());
							e.printStackTrace();
						}

						m_bError = true;
						if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
							m_bErrorString = "Send th: "
									+ (e.getMessage() == null ? e.toString()
											: e.getMessage());
						}

						m_connected = false;
					}

					outgoing_data_on_receive_connection = null;
				}
			}

			yield();
		}
		// we are not connected anymore, cleanup
		m_connected = false;
		cleanup();
		try {
			this.join();
		} catch (Exception e) {
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("TCP: join exception: " + e.toString());
				e.printStackTrace();
			}
		}
	}
}
