
#include "stdafx.h"


#include "MediaPlayer/MediaPlayerIPhone.h"
#include "Core/Logs.h"
#include "MediaPlayer/MoviePlayerIPhone.h"
//#include "MediaPlayer/AudioPlayerIPhone.h"

void MediaPlayerIPhone::_trace(const char* msg) const
{
	Logs::trace("MediaPlayerIPhone", msg);
}

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ctors / Dtor

MediaPlayerIPhone::MediaPlayerIPhone()
{
	_trace( "->MediaPlayerIPhone()");
	_movieplayer = new MoviePlayerIPhone();
//	_audioplayer = new AudioPlayerIPhone();
	
	_trace( "<-MediaPlayerIPhone()");
	
}

void MediaPlayerIPhone::playMovie(const char* name)
{
	
	_trace( "->playMovie()");
	if(_movieplayer)
	{
		_movieplayer->playMovie(name);
		_trace( "_movieplayer is ok");
	}

	_trace( "<-playMovie()");
}


void MediaPlayerIPhone::stopMovie()
{
	
	_trace( "->stopMovie()");
	if(_movieplayer)
	{
		_movieplayer->stopMovie();
	}

	_trace( "<-stopMovie()");
}
