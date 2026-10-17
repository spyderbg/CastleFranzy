// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MediaPlayerIPhone
//
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef MEDIAPLAYER_MEDIAPLAYERIPHONE_H
#define MEDIAPLAYER_MEDIAPLAYERIPHONE_H

#include "MediaPlayer/IMediaPlayer.h"

class MoviePlayerIPhone;
class AudioPlayerIPhone;

class MediaPlayerIPhone: public IMediaPlayer
{
	
	// ** Ctors / Dtor
	// ////////////////////////////////////////////////////////
public:
	MediaPlayerIPhone();
//	virtual				~MediaPlayerIPhone();

protected:
	void				_trace(const char* msg) const;
	MoviePlayerIPhone*  _movieplayer;
	AudioPlayerIPhone*  _audioplayer;

	
	/// parent implementation
public:	
	void playMovie(const char* name);
	void stopMovie();
	

	

};

#endif
