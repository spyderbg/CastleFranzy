//
//  this is a c++ wrapper around obj-c MPMoviePlayerController
//
#ifndef MEDIAPLAYER_MOVIEPLAYER_H
#define MEDIAPLAYER_MOVIEPLAYER_H
#include "stdafx.h"

//#include <objc/objc.h>
//#import <MediaPlayer/MediaPlayer.h>

class MoviePlayerIPhone
	{
	private:
//		MPMovieControlMode icontrolMode;
//		MPMovieScalingMode iscalingMode;
		int icontrolMode;
		int iscalingMode;
		
//		UIColor ibackgroundColor;
		int ibackgroundColor;
		

		// ** Methods
	// ////////////////////////////////////////////////////////
	protected:
		void				_trace(const char* msg) const;
		//void setBackgroundUIColor(UIColor color);

//		void setScalingMode(MPMovieScalingMode);
//		void setControlMode(MPMovieControlMode);
		void setScalingMode(int);
		void setControlMode(int);
		
	public:
		MoviePlayerIPhone() EMPTY_BODY ;
		//~MoviePlayerIPhone();

		void setBackgroundColor(int argb) EMPTY_BODY ;
		void setBackgroundColor(int red,int blue,int green ,int alpha) EMPTY_BODY ;

		void setMovieURL(const char* url) EMPTY_BODY ;

		void playMovie() EMPTY_BODY ;
		void playMovie(const char* url) EMPTY_BODY ;

		void stopMovie() EMPTY_BODY ;


	};


#endif
