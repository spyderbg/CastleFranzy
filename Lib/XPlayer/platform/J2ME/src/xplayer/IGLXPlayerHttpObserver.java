
/**
 * interface IGLXPlayerHttpObserver.
 * <p>
 * It is the observer of GLXPlayerwebComponent.
 * </p>
 * </p>
 * <p>
 * This class designed as an abstract class. So client developer has to
 * implements it and implements all functions.
 * </p>
 * 
 * @author World Online Team <<online-adm@gameloft.com>>
 * @author yi.he <<yi.he@gameloft.com>>
 * @version 0.0.1
 * 
 */
public interface IGLXPlayerHttpObserver {
	/**
	 * Abstract Function OnNetworkError. Indicate that network is broken or can
	 * not connect to the server.
	 * <p>
	 */
	
	public void OnNetworkError();

	/**
	 * Abstract Function OnRequestTimeout. Indicate that network connection or
	 * request is time out.
	 * <P>
	 * 
	 * @param func_id
	 */
	public void OnRequestTimeout(int func_id);

	/**
	 * Abstract Function OnRequestSuccess. The correct response is notified by String
	 * 
	 * @param func_id
	 * @param data
	 */
	public void OnRequestSuccess(int func_id, String data);

	/**                                           
	 * Abstract Function OnRequestSuccessByAray. The correct response is notified by
	 * byte[]
	 * 
	 * @param func_id
	 * @param data
	 */
	public void OnRequestSuccessByAray(int func_id, byte[] data);

	/**
	 * Abstract Function OnRequestFailure. The wrong response or the operation
	 * is failed on server side.
	 * 
	 * @param func_id
	 * @param error_code
	 * @param data
	 */
	public void OnRequestFailure(int func_id, int error_code, String data);
}
