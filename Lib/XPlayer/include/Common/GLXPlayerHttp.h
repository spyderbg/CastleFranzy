#ifndef __GL_XPLAYER_HTTP_H__
#define __GL_XPLAYER_HTTP_H__


#include "../Game_defines.h"
#include "GLXPlayerSocketFactory.h"

#ifdef ENABLE_HTTP_REQUEST_QUEUE
#include <deque>
#include <string>

typedef struct _requestqueue
{
	bool sendbyPost;
	//int requestLen;
	std::string request;
}RequestQueue;
#endif

/**
 * Class GLXPlayerHttp
 * The class for communicating with Gameloft Web Server using HTTP protocol.
 * <p>
 * The connection is base on TCP protocl.
 * <p>
 * A http header is added to implement the HTTP protocl on TCP connection.
 * <p>
 * Method StartRequest() should be used to create a new http request, and 
 * it's parameter is the whole url address.
 * <p>
 * Method UpdateRequest() should be invoked at every game frame to finish 
 * the network communication.
 * <p>
 *
 *  @author World Online Team <<online-adm@gameloft.com>>
 * @author Lu Tie Hua <tiehua.lu@gameloft.com>
 * @author Tran Thai Phi <<phi.tranthai@gameloft.com>> 
 * @version 1.0.0
 *
 */
class GLXPlayerHttp : public GLXPlayerSocketObserver
{
public:

	/**
	 * GLXPlayerHttp constructor.
	 * @param server_address Server address, for example: "beta.gameloft.com".
	 * @param php_version Used for identifing different functions.
	 * @param php_address The web request address, for example: "ope/GenericXPlayer_v1.php".
	 *  
	 */
	GLXPlayerHttp(char * server_address, char * php_version, char * php_address);

	/**
	* GLXPlayerHttp destructor. 
	*/
	virtual ~GLXPlayerHttp();
	
#ifdef ENABLE_HTTP_REQUEST_QUEUE
	bool GetRequest();
	void PopRequest()
	{
		--m_numOfUnResolvedRequest;
	}
#endif

	/**
	* Function GetRequestData.
	* Get the request string.	
	* @return Request string.
	*/
	char * GetRequestData();
	
	/**
	* Function GetRequestLength.
	* Get the length of request string.	
	* @return Request string length.
	*/
	int GetRequestLength();
	
	/**
	* Function SetResponseData.
	* Set the response data as the data returned from the server.
	* @param data Response data from server.	
	* @param size Size of response data.		
	*/
	void SetResponseData( char * data, int size );

public:

	/**
	* Function sendByGet.
	* Starts a HTTP request by GET method but has no ver.	
	* @param sUrl URL of the server
	* @param sQuery The query to be sent to the server
	*/
	void sendByGetWithNoVer( char *sUrl, char *sQuery );
	/**
	* Function sendByGet.
	* Starts a HTTP request by GET method.	
	* @param sUrl URL of the server
	* @param sQuery The query to be sent to the server
	*/
	void sendByGet( char *sUrl, char *sQuery );
	
	/**
	* Function sendByPost.
	* Starts a HTTP request by POST method.	
	* @param sUrl URL of the server
	* @param sQuery The query to be sent to the server
	*/
	void sendByPost( char *sUrl, char *sQuery );

	/**
	* Function sendVideoByPost.
	* Starts a HTTP request by POST method.	
	* @param pData: video data.
	* @param size: video's length.
	* @param videoID: the video id
	* @return success: f|106|r|s|
	* @return fail: f|106|e|error code|
	*/
	void sendVideoByPost(char *sUrl, char *sQuery, char *pData, int &size, char* videoID);
	
	/**
	* Function UpdateRequest.
	* Update function to send the request to the server.
	*/
	void UpdateRequest();
	
	/**
	* Function Cancel.
	* Cancel current request.
	*/
	void Cancel();
	
	/**
	* Function IsInProgress.
	* Check if the connection is in process or not.
	* @return True / False
	*/
	bool IsInProgress();
	
	/**
	* Function IsErrorOccurred.
	* Check if error occur or not.
	* @return True / False
	*/
	bool IsErrorOccurred();
	
	/**
	* Function GetResponse.
	* Get the response from HTTP server.
	* @return Response string.
	*/
	char * GetResponse();

	/**
	* Function GetResponseLen.
	* Get the length of response buffer.
	* @return length of Response buffer.
	*/
	int GetResponseLen();

	/**
	* Function SetResponseLen.
	* Set the length of response buffer.
	*/
	void SetResponseLen(int len);
	
	/**
	* Function IsSendByPost.
	* Check if send by POST or not.
	*/
	bool IsSendByPost(){return m_send_by_post;}

#ifdef ENABLE_HTTP_REQUEST_QUEUE
	bool IsRequestQueueEmpty()
	{
		return (m_numOfUnResolvedRequest==0 ? true: false);
	}
#endif

	void Start()
	{
		m_socket->Start();
	}

private:

	/**
	* Variable m_socket.
	* The HTTP connection socket.	
	*/
	GLXPlayerSocket * m_socket;
	
	/**
	* Variable m_abBufferIn.
	* The request buffer.
	*/
	char m_abBufferIn[SEND_BUFFER_SIZE];

	/**
	* Variable m_sResponse.
	* The response buffer.
	*/
	char * m_sResponse;

	/**
	* Variable m_sResponseLen.
	* The length of response buffer.
	*/
	int m_sResponseLen;

	/**
	* Variable m_server_address.
	* The server address.  Read by XPlayer, XPlayer will delete it, do not delete it in XHttp.
	*/	
	char * m_server_address;
	
	/**
	* Variable m_php_script_version.
	* The PHP version.  Read by XPlayer, XPlayer will delete it, do not delete it in XHttp.
	*/
	char * m_php_script_version;
	
	/**
	* Variable m_php_address.
	* The PHP address.  Read by XPlayer, XPlayer will delete it, do not delete it in XHttp.
	*/
	char * m_php_address;	
	
	/**
	* Variable m_post_send_buffer.
	* Request buffer when using HTTP POST method.
	*/
	char * m_post_send_buffer;

	/**
	* Variable m_post_send_buffer_len.
	* Length of Request buffer when using HTTP POST method.
	*/
	int m_post_send_buffer_len;
	
	/**
	* Variable m_send_by_post.
	* Indicate use POST method or not.
	*/
	bool m_send_by_post;

#ifdef ENABLE_HTTP_REQUEST_QUEUE
	std::deque<RequestQueue> m_requestQueue;
	int m_numOfUnResolvedRequest;
#endif
};

#endif // __GL_XPLAYER_HTTP_H__

