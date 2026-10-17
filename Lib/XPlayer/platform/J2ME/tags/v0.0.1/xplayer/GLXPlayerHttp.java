import java.io.*;
import javax.microedition.io.*;

/**
 * HTTP connection class.
 * <p>
 * Provides services for creating and managing HTTP requests. This is
 * non-blocking and runs as a separate thread on the device. It also provides a
 * method for canceling an ongoing transaction.
 * </p>
 * <p>
 * How to use:
 * </p>
 * <p>
 * <ol>
 * <li>create an instance</li>
 * <li>call <code>sendByGet()</code> to begin a transaction</li>
 * <li>call <code>isInProgress()</code> to find out the status of an on going
 * transaction</li>
 * <li>call <code>cancel()</code> to stop at anytime</li>
 * <li>check <code>m_response</code> for return from server</li>
 * <li>call <code>cleanup()</code> to free resources (will free the
 * <code>m_response</code> buffer, so use it before cleanup)</li>
 * </ol>
 * 
 * @author World Online Team <<online-adm@gameloft.com>>
 * @author yi.he <<yi.he@gameloft.com>>
 * @version 0.0.1
 *          <DT><B>Copyright:</B></DT>
 *          <DD>Gameloft SA � 2006</DD>
 */
final class GLXPlayerHttp implements Runnable {
	protected GLXPlayerWebComponent m_observer;
	private boolean m_bsendByPost = false;

	protected void RegisterObserver(GLXPlayerWebComponent observe) {
		m_observer = observe;

	}

	// VERSION 1.0.9

	// Comment this to build as part of the program rather than package.

	// Number of bytes to read from response at one time.
	private final int RECEIVEBUFFERSIZE = 16;

	// Pointer to current thread doing transfer, used to wait for previous
	// transfer to end.
	private Thread m_thread = null;

	// Where to send
	private String m_sUrl;

	// Used only with POST requests (XPlayer.Configuration.USE_HTTP_POST ==
	// true)
	private String m_sQuery; // what to send

	// Network objects.
	private HttpConnection m_c = null;
	private InputStream m_i = null;

	// Used only with POST requests (XPlayer.Configuration.USE_HTTP_POST ==
	// true)
	private OutputStream m_o = null;

	/**
	 * The received message buffer.
	 * <p>
	 * Check this after a transaction completes to get the received data.
	 * </p>
	 */
	public String m_response;
	/**
	 * The received file buffer.
	 * <p>
	 * Check this after a file transaction completes to get the received file
	 * data.
	 */
	public byte[] m_dataResponse;
	// Set to true when in the middle of a http trasaction.
	private boolean m_bInProgress = false;

	// Set to true when the transaction is canceled.
	private boolean m_bCanceled = false;

	/**
	 * Tells wether an error occured during the HTTP transaction.
	 * <p>
	 * It is set to <b><code>true</code></b> if an error has been
	 * encountered and <b><code>false</code></b> otherwise.
	 * </p>
	 */
	public boolean m_bError = false;

	/**
	 * The User-Agent to be used.
	 * <p>
	 * Default is "GameloftClient/1.0"; set to whatever User-Agent you need. If
	 * <code>null</code>, no user agent will be set (device defaults will be
	 * used instead).
	 * </p>
	 */
	// public static String userAgent = "GameloftClient/1.0";
	public static String userAgent = null;

	/**
	 * Stores the emulated value of the ClientId when running on an emulator.
	 * <p>
	 * Each Sprint phone comes with a factory preset unique ClientId. This is
	 * not true for the emulator though, which does not have such a value
	 * associated with it.
	 * </p>
	 * <p>
	 * Because this value is requested by the server, for testing on emulator
	 * this needs to be emulated somehow. To do this, set the <b>ClientId</b>
	 * property in the .jad file to something like "SZZYourName01@sprintpcs.com"
	 * to enable sending of a client id while debugging in emulator. The
	 * property value will be read from the .jad, set accordingly in here and
	 * used troughout the running of the transaction.
	 * </p>
	 * <p>
	 * DO NOT do this on a phone release though. The phone's ClientId will be
	 * automatically used there.
	 * </p>
	 * <p>
	 * Default is <code>null</code>.
	 * </p>
	 */
	public static String clientId = null;

	/**
	 * Stores the emulated value of the x_up_subno when running on an emulator.
	 * <p>
	 * Each Cingular Blue Network phone comes with a factory preset unique
	 * x_up_subno. This is not true for the emulator though, which does not have
	 * such a value associated with it.
	 * </p>
	 * <p>
	 * Because this value is requested by the server, for testing on emulator
	 * this needs to be emulated somehow. To do this, set the <b>x_up_subno</b>
	 * property in the .jad file to something like "YourName01@gameloft.com" to
	 * enable sending of a client id while debugging in emulator. The property
	 * value will be read from the .jad, set accordingly in here and used
	 * troughout the running of the transaction.
	 * </p>
	 * <p>
	 * DO NOT set this in the .jad on a phone release though. The phone's
	 * x_up_subno will be automatically added by the carrier.
	 * </p>
	 * <p>
	 * Default is <code>null</code>.
	 * </p>
	 */
	public static String x_up_subno = null;

	/**
	 * Stores the emulated value of the CarrierDeviceId when running on an
	 * emulator.
	 * <p>
	 * Each Cingular Orange Network phone comes with a factory preset unique
	 * CarrierDeviceId. This is not true for the emulator though, which does not
	 * have such a value associated with it.
	 * </p>
	 * <p>
	 * Because this value is requested by the server, for testing on emulator
	 * this needs to be emulated somehow. To do this, set the <b>CarrierDeviceId</b>
	 * property in the .jad file to something like "YourName01@gameloft.com" to
	 * enable sending of a CarrierDeviceId while debugging in emulator. The
	 * property value will be read from the .jad, set accordingly in here and
	 * used troughout the running of the transaction.
	 * </p>
	 * <p>
	 * DO NOT set this in the .jad on a phone release though. The phone's
	 * CarrierDeviceId will be automatically added by the carrier.
	 * </p>
	 * <p>
	 * Default is <code>null</code>.
	 * </p>
	 */
	public static String CarrierDeviceId = null;

	/**
	 * Stores the emulated value of the WCID when running on an emulator.
	 * <p>
	 * Each Roger Network phone comes with a factory preset unique WCID. This is
	 * not true for the emulator though, which does not have such a value
	 * associated with it.
	 * </p>
	 * <p>
	 * Because this value is requested by the server, for testing on emulator
	 * this needs to be emulated somehow. To do this, set the <b>WCID</b>
	 * property in the .jad file to something like "YourName01@gameloft.com" to
	 * enable sending of a WCID while debugging in emulator. The property value
	 * will be read from the .jad, set accordingly in here and used troughout
	 * the running of the transaction.
	 * </p>
	 * <p>
	 * DO NOT set this in the .jad on a phone release though. The phone's WCID
	 * will be automatically added by the carrier.
	 * </p>
	 * <p>
	 * Default is <code>null</code>.
	 * </p>
	 */
	public static String WCID = null;

	/**
	 * Stores the emulated value of the UPSUBID when running on an emulator.
	 * <p>
	 * Each Nextel Network phone comes with a factory preset unique UPSUBID.
	 * This is not true for the emulator though, which does not have such a
	 * value associated with it.
	 * </p>
	 * <p>
	 * Because this value is requested by the server, for testing on emulator
	 * this needs to be emulated somehow. To do this, set the <b>UPSUBID</b>
	 * property in the .jad file to something like "YourName01@gameloft.com" to
	 * enable sending of a UPSUBID while debugging in emulator. The property
	 * value will be read from the .jad, set accordingly in here and used
	 * troughout the running of the transaction.
	 * </p>
	 * <p>
	 * DO NOT do this on a phone release though. The phone's UPSUBID will be
	 * automatically used there.
	 * </p>
	 * <p>
	 * Default is <code>null</code>.
	 * </p>
	 */
	public static String upsubid = null;

	/**
	 * Stores the emulated value of the x_up_calling_line_id when running on an
	 * emulator.
	 * </p>
	 * <p>
	 * For testing Virgin Mobile games only. Set <b>x_up_calling_line_id</b> in
	 * .jad to a 10 digit phone number to enable sending of a
	 * x_up_calling_line_id while debugging in emulator.
	 * </p>
	 * <p>
	 * Default is <code>null</code>.
	 * </p>
	 */
	public static String x_up_calling_line_id = null;

	/**
	 * On some phones, the connection thread hangs indefinitely if forced to
	 * cancel, blocking out access to the HTTP connector forever. For that
	 * reason the thread sometimes has to be left to finish by itself.
	 * <p>
	 * This value tells the cancel function if it has to actually try and stop
	 * the thread. If it is set to <code>false</code>, the cancel function
	 * will not close the current connection thread, leaving it to finish by
	 * itself. If set to <code>true</code>, the thread is killed.
	 * </p>
	 * <p>
	 * Default is <code>false</code>.
	 * </p>
	 */
	private static boolean must_cancel = false;

	/**
	 * Provides information wether this class' instance is currently doing a
	 * transfer or not.
	 * 
	 * @return <code>true</code> if a call is in progress, <code>false</code>
	 *         otherwise.
	 */
	public boolean isInProgress() {
		return m_bInProgress;
	}

	/**
	 * Tells wether an error occured during the transaction.
	 * 
	 * @return <code>true</code> if an error has been encountered,
	 *         <code>false</code> otherwise.
	 */
	public boolean isErrorOccurred() {
		return m_bError;
	}

	/**
	 * Cancels the ongoing transaction.
	 * <p>
	 * It is safe to call at any time, but keep in mind that the request may
	 * have already reached the server, triggering any changes it implied. Those
	 * changes will not be discarded by the server and will be saved.
	 * </p>
	 */
	public void cancel() {
		m_bCanceled = true;

		if (GLXPlayerConfig.Configuration.HTTP_NO_CANCEL && !must_cancel) {
			return;
		} else {
			must_cancel = false;

			if (m_c != null) {
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					System.out.println("HTTP: cancel");
				}
				try {
					synchronized (m_c) {
						m_i.close();
					}
				} catch (Exception e) {
				}
				try {
					synchronized (m_c) {
						m_c.close();
					}
				} catch (Exception e) {
				}

				if (GLXPlayerConfig.Configuration.USE_HTTP_POST || m_bsendByPost) {
					try {
						synchronized (m_o) {
							m_o.close();
						}
					} catch (Exception e) {
					}

					m_o = null;
				}
			}

			m_i = null;
			m_c = null;

			m_bInProgress = false;
			m_thread = null;
			// Run the garbage collector.
			System.gc();
		}

	}

	/**
	 * Frees any resources used by this class.
	 * <p>
	 * It is OK to call at any time, but it should be called after using any
	 * data returned from the server as it also deletes <code>m_response</code>.
	 * </p>
	 * <p>
	 * <DD>
	 * <DL>
	 * <DT><B>Side effect:</B></DT>
	 * <DD>It will cancel the current transaction, if any.</DD>
	 * </DL>
	 * </DD>
	 * </p>
	 * 
	 * @see #m_response
	 */
	public void cleanup() {
		cancel();
		m_response = null;
		m_dataResponse = null;
	}

	/**
	 * The entry point. It starts a HTTP request.
	 * 
	 * @param sUrl
	 *            URL of the server
	 * @param sQuery
	 *            The query to be sent to the server
	 */
	public void sendByGet(String sUrl, String sQuery) {

		// Run the garbage collector.
		System.gc();
		if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
			System.out.println("HTTP: sendByGet:url [" + sUrl + "] [" + sQuery
					+ "] ");
		}
		// blocking wait on previous request end
		while (m_bInProgress) {

			try {
				if (GLXPlayerConfig.Configuration.ENABLE_TIMEOUT) {
					// check if enough time has passed
					if (System.currentTimeMillis()
							- GLXPlayerWebComponent.s_callstarttime > GLXPlayerConfig.Configuration.CONN_TIMEOUT) {
						// Clean up connections and sets m_bInProgress to false.
						must_cancel = true;
						cancel();
						must_cancel = false;

						// TODO: Decide if this is necessary.
						// Reset the starttime so the handle functions wait a
						// bit
						// for the response before timing out.
						// XPlayer.callstarttime = System.currentTimeMillis();
					}
				}

				synchronized (this) {
					// This causes the game thread to block for 50 milliseconds
					// every iteration.so do not call sendXXX in send thread!
					wait(50);
				}
			} catch (Exception e) {
			}
		}

		must_cancel = true;
		cancel();
		must_cancel = false;

		// in the middle of transaction
		m_bInProgress = true;
		m_bCanceled = false;
		m_bsendByPost = false;
		// store where and what to send
		if (GLXPlayerConfig.Configuration.USE_HTTP_POST) {
			// for http get, we need to store separately the server address and
			// the
			// request body
			m_sUrl = sUrl;
			m_sQuery = sQuery;

			// add the version
			m_sQuery += "&v=" + GLXPlayerConfig.Configuration.XPLAYER_VERSION;
		} else {
			// for http get, the url contains the query.
			m_sUrl = sUrl + "?b=" + sQuery;

			// add the version ..
			m_sUrl += "&v=" + GLXPlayerConfig.Configuration.XPLAYER_VERSION;
		}

		// wait for the previous trasaction
		if (m_thread != null) {
			try {
				m_thread.join();
			} catch (Exception e) {
			}
		}

		// begin a thread which will do the transaction
		// m_timeBegin = System.currentTimeMillis();
		m_bError = false;
		m_thread = new Thread(this);
		m_thread.start();
	}

	/**
	 * The entry point. It starts a HTTP request.
	 * 
	 * @param sUrl
	 *            URL of the server
	 * @param sQuery
	 *            The query to be sent to the server
	 */
	public void sendByPost(String sUrl, String sQuery) {
		// Run the garbage collector.
		System.gc();
		if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
			System.out.println("HTTP: sendByGet:url [" + sUrl + "] [" + sQuery
					+ "] ");
		}
		// blocking wait on previous request end
		while (m_bInProgress) {

			// if(m_thread!=null)
			// if(Thread.currentThread().getName().equals(m_thread.getName()))
			// {
			// return;
			// }
			try {
				if (GLXPlayerConfig.Configuration.ENABLE_TIMEOUT) {
					// check if enough time has passed
					if (System.currentTimeMillis()
							- GLXPlayerWebComponent.s_callstarttime > GLXPlayerConfig.Configuration.CONN_TIMEOUT) {
						// Clean up connections and sets m_bInProgress to false.
						must_cancel = true;
						cancel();
						must_cancel = false;

						// TODO: Decide if this is necessary.
						// Reset the starttime so the handle functions wait a
						// bit
						// for the response before timing out.
						// XPlayer.callstarttime = System.currentTimeMillis();
					}
				}

				synchronized (this) {
					// This causes the game thread to block for 50 milliseconds
					// every iteration.so do not call sendXXX in send thread!
					wait(50);
				}
			} catch (Exception e) {
			}
		}

		must_cancel = true;
		cancel();
		must_cancel = false;

		// in the middle of transaction
		m_bInProgress = true;
		m_bCanceled = false;

		// store where and what to send
		m_bsendByPost = true;

		// for http get, we need to store separately the server address and the
		// request body
		m_sUrl = sUrl;
		m_sQuery = sQuery;

		// add the version
		m_sQuery += "&v=" + GLXPlayerConfig.Configuration.XPLAYER_VERSION;

		// wait for the previous trasaction
		if (m_thread != null) {
			try {
				m_thread.join();
			} catch (Exception e) {
			}
		}

		// begin a thread which will do the transaction
		// m_timeBegin = System.currentTimeMillis();
		m_bError = false;
		m_thread = new Thread(this);
		m_thread.start();
	}

	/**
	 * The main thread handling the HTTP request. It is started by the
	 * <code>sendByGet(String, String)</code> function.
	 * 
	 * @see #sendByGet(String, String)
	 */
	public void run() {

		long openTime = 0;
		boolean finishedOpen = false;

		if (m_sUrl == null) {
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("HTTP: run: URL is null");
			}
			cancel();
			m_bError = true;
			m_bInProgress = false;
			m_bCanceled = false;
			return;
		}

		if (GLXPlayerConfig.Configuration.USE_HTTP_POST || m_bsendByPost) {// for http
			// post,
			// need to
			// test the
			// query
			// too.
			if (m_sQuery == null) {
				if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
					System.out.println("HTTP: run: Query is null");
				}
				cancel();
				m_bError = true;
				m_bInProgress = false;
				m_bCanceled = false;
				return;
			}

			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out
						.println("Using HTTP Post - this phone must have problems with long GET REQUESTS.");
			}

		}

		if (GLXPlayerConfig.Configuration.FULLSCREEN_CHANGE) {
			// marking start of connection for the fullscreen change
			openTime = System.currentTimeMillis();
		}

		try {
			m_bError = false;

			// open connection
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("HTTP: run:connecting to [" + m_sUrl + "]");
			}

			if (m_bCanceled) {
				m_bInProgress = false;
				return;
			}
			m_c = (HttpConnection) Connector.open(m_sUrl, Connector.READ_WRITE);

			if (GLXPlayerConfig.Configuration.FULLSCREEN_CHANGE) {
				// connection finished, fullscreen mode has to be changed to
				// avoid the bug
				finishedOpen = true;

				if (System.currentTimeMillis() - openTime > 200) {
					GLXPlayerWebComponent.ScreenModeChange(true);
				}
			}

			if (m_bCanceled) {
				m_bInProgress = false;
				return;
			}

			// set the corresponding HTTP method method
			if (GLXPlayerConfig.Configuration.USE_HTTP_POST || m_bsendByPost) {

				m_c.setRequestMethod(HttpConnection.POST);
			} else {
				m_c.setRequestMethod(HttpConnection.GET);
			}

			m_c.setRequestProperty("Connection", "close");

			if (userAgent != null) {
				m_c.setRequestProperty("User-Agent", userAgent);
			}
			// Marcel Valdez:
			// The HTTP Request will accept a string of chars (or bytes)
			// in UTF-8 format. Basically, the charset directive is used
			// to correctly code/decode the string read from byte[] to String
			// and vice-versa.
			// String accept_rules = "";

			// accept_rules =
			// "text/plain;charset="+XPlayer.Configuration.DEFAULT_ENCODING+";q=1";

			// m_c.setRequestProperty("Accept", accept_rules);

			// add data length dan content type when use post
			if (GLXPlayerConfig.Configuration.USE_HTTP_POST || m_bsendByPost) {
				m_c.setRequestProperty("Content-Type",
						"application/x-www-form-urlencoded");
				String sQuery = "b=" + m_sQuery;
				m_c.setRequestProperty("Content-Length", String.valueOf(sQuery
						.length()));

				m_o = m_c.openOutputStream();
				m_o.write(sQuery.getBytes(), 0, sQuery.length());
				m_o.flush();
			}

			if (m_bCanceled) {
				m_bInProgress = false;
				return;
			}

			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("HTTP: run: receive");
			}

			// actually this opens the input connection
			if (m_c.getResponseCode() != m_c.HTTP_OK) {
				cancel();
				m_bError = true;
				m_bInProgress = false;
				m_bCanceled = false;
				return;
			}

			if (m_bCanceled) {
				m_bInProgress = false;
				return;
			}

			synchronized (m_c) {
				m_i = m_c.openInputStream();
			}

			m_thread.yield();
			if (m_bCanceled) {
				m_bInProgress = false;
				return;
			}

			ByteArrayOutputStream bao = new ByteArrayOutputStream();
			if (GLXPlayerWebComponent.isDownloadingFile) {
				int fileSize = (int) m_c.getLength();
				if (fileSize > 0) {
					m_dataResponse = new byte[fileSize];
				}
			}
			byte[] abInBuffer = new byte[RECEIVEBUFFERSIZE];
			int nBytesRead = 0;
			int i;
			int totalBytesRead = 0;
			while (nBytesRead != -1) {
				if (m_bCanceled) {
					m_bInProgress = false;
					return;
				}

				/***************************************************************
				 * SANYO PHONE SPECIFIC ******************** sanyo phones for
				 * some reason always return result the same as
				 * RECEIVEBUFFERSIZE even when it actually read less so need to
				 * reset buffer to all 0 before read, then cut off the 0's for
				 * it to work properly
				 **************************************************************/
				for (i = 0; i < RECEIVEBUFFERSIZE; i++) {
					abInBuffer[i] = 0;
				}
				/**
				 * ****************** END SANYO PHONE SPECIFIC
				 * *******************
				 */

				nBytesRead = m_i.read(abInBuffer, 0, RECEIVEBUFFERSIZE);

				if (m_bCanceled) {
					m_bInProgress = false;
					return;
				}

				/***************************************************************
				 * SANYO PHONE SPECIFIC ******************** sanyo phones for
				 * some reason always return result the same as
				 * RECEIVEBUFFERSIZE even when it actually read less so need to
				 * reset buffer to all 0 before read, then cut off the 0's for
				 * it to work properly
				 **************************************************************/
				if (nBytesRead == -1) {
					break;
				}

				// note that I am checking backwards, this is required so this
				// code
				// can work with current JCP server which sometimes returns a 0
				// at
				// the begining of response (non-http standard, hope it will be
				// fixed soon)
				if (!GLXPlayerWebComponent.isDownloadingFile) {
					for (i = RECEIVEBUFFERSIZE - 1; i >= 0; i--) {
						if (abInBuffer[i] != 0) {
							break;
						}
					}
				}
				if (i < RECEIVEBUFFERSIZE)
					nBytesRead = i + 1;
				/**
				 * ****************** END SANYO PHONE SPECIFIC
				 * *******************
				 */

				// if (XPlayer.Configuration.ENABLE_DEBUG)
				// {
				// System.out.println("HTTP: run: read [" + nBytesRead +
				// "] bytes from server");
				// }
				if (!GLXPlayerWebComponent.isDownloadingFile) {
					bao.write(abInBuffer, 0, nBytesRead);
				} else if (nBytesRead > 0) {
					if ((totalBytesRead + nBytesRead) <= m_dataResponse.length) {
						System.arraycopy(abInBuffer, 0, m_dataResponse,
								totalBytesRead, nBytesRead);
						totalBytesRead += nBytesRead;
					} else if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
						System.out.println("HTTP: run: file size [ "
								+ m_dataResponse.length
								+ " ] is smaller than the file received [ "
								+ (totalBytesRead + nBytesRead) + " ]");
					}
				}

			}

			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				if (!GLXPlayerWebComponent.isDownloadingFile)
					System.out.println("HTTP: run: received [" + bao.toString()
							+ "]");
				else
					System.out.println("HTTP: run: received [" + totalBytesRead
							+ "] expected [" + m_dataResponse.length + "]");

			}
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("HTTP: run: total bytes read: ["
						+ bao.size() + "]");
			}

			if (m_bCanceled) {
				m_bInProgress = false;
				return;
			}
			if (!GLXPlayerWebComponent.isDownloadingFile)
				m_response = bao.toString();
			else
				m_response = new String(m_dataResponse,
						GLXPlayerConfig.Configuration.DEFAULT_ENCODING);
			m_observer.SetResponseData(m_response);
			bao = null;

		} catch (Exception e) {
			if (GLXPlayerConfig.Configuration.FULLSCREEN_CHANGE) {
				if (System.currentTimeMillis() - openTime > 200
						&& !finishedOpen) {
					GLXPlayerWebComponent.ScreenModeChange(true);
				}
			}
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("HTTP: run: exception : " + e.toString());
			}
			m_bsendByPost = false;
			m_bError = true;
			m_bInProgress = false;
			GLXPlayerWebComponent.isDownloadingFile = false;
			return;
		} finally {
			if (GLXPlayerConfig.Configuration.ENABLE_DEBUG) {
				System.out.println("HTTP: run: finally");
			}

			// clean up
			cancel();
			GLXPlayerWebComponent.isDownloadingFile = false;
			m_thread = null;
			// Run the garbage collector.
			System.gc();
			// done
			m_bsendByPost = false;
			m_bInProgress = false;

		}
	}

}
