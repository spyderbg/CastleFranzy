#ifndef __GL_XPLAYER_YOUTUBE_H__
#define __GL_XPLAYER_YOUTUBE_H__

#include "../Porting.h"
#include "../Common/GLXPlayerWebComponent.h"
//#include "memory.h"

/**
* Class GLXPlayerYoutube. 
* Class GLXPlayerYoutube is designed for Youtube functions. 
*
* @author  World Online Team <<online-adm@gameloft.com>>
 * @version 1.0.0 
 *  
 */
class GLXPlayerYoutube : public GLXPlayerWebComponent
{

public:
	/**
	* GLXPlayerYoutube constructor.
	* Create an instance of GLXPlayerYoutube.
	*/
	GLXPlayerYoutube();
	
	/**
	* GLXPlayerYoutube destructor.
	* Destroy an instance of GLXPlayerYoutube.
	*/
	virtual ~GLXPlayerYoutube(void);	
	
	/**
	* Function sendCheckReplayVideoAvailable.
	* Check for replay video available or not.
	* @return success: |r|s|t|value (0/1)
	* @return fail: |e|error code
	*/
	bool sendCheckReplayVideoAvailable();

	/**
	* Function sendAddVideoInfo.
	* Add video's informations which contains title comments and ggi.
	* 
	* @param title video's title.
	* @param comment video's comments.
	* @param isPrivate Set Youtube video to private on youtube's side.
	* @return success: |r|s|vi|video_ id
	* @return fail: |e|error code; ERROR_YOUTUBE_ACCOUNT_NOT_EXIT = 72
	*/
	bool sendAddVideoInfo(char* account, char* password, char* title, char* comment, bool isPrivate = false);

	/**
	* Function sendAddVideoData.
	* Add video data.
	* 
	* @param pData: video data.
	* @param size: video's length.
	* @param videoID: video's ID.
	* @return success:
	* @return fail:
	*/
	bool sendAddVideoData(char *pData, int &size);
	
	/**
	* Function getVideoID.
	* Get Video's ID.
	* @see processAddVideoInfo()
	* @return m_videoID.
	*/
	char* getVideoID(){ return m_videoID;}
	
protected:
	/**
	* Function SendVideoByPost.
	 * Send request to HTTP server by POST method.
	 * @param pData: video data.
	* @param size: video's length.
	* @param videoID: video id
	* @return success:
	* @return fail:
	**/
	virtual bool SendVideoByPost(char *url, char *pData, int &size, char* videoID);
	
	/**
	* Private function OnUpdateSuccess.
	* Handle success message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	bool OnUpdateSuccess( int func_id );

	/**
	* Private function processAddVideoInfo.
	* Get video information after call sendAddVideoInfo().
	* @see sendAddVideoInfo()
	* @param data Data returned from the server.
	*/
	void processAddVideoInfo(char* data);
	
private:	
	/**
	* Variable m_videoID.
	* Store video's ID.
	* @see sendAddVideoInfo()
	*/
	char* m_videoID;
};
#endif
