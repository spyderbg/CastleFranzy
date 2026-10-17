#ifndef __GL_XPLAYER_DOWNLOAD_H__
#define __GL_XPLAYER_DOWNLOAD_H__

#include "../Common/GLXPlayerWebComponent.h"


class GLXPlayerDownload : public GLXPlayerWebComponent
{
public:

	GLXPlayerDownload();

	virtual ~GLXPlayerDownload();

	bool SendDownloadFile(int ggi, char *uid, char *filename);

protected:	
	/**
	* Function OnUpdateParse.
	* The default process to handle response.
	* The subclass can replace it with its own implementation.
	*
	*/
	bool OnUpdateParse();
	
	/**
	* Protected function OnUpdateSuccess.
	* Handle success message returned from the Web server.	
	* @param func_id Function ID.
	* @return True / False.
	*/
	virtual bool OnUpdateSuccess( int func_id );

	/**
	 * Function LoadConfig.
	 * Load all configuration.
	 * @return True / False.
	 */
	bool LoadConfig();	
};

#endif