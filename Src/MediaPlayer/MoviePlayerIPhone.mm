
#import "MoviePlayerIPhone.h"
#import "Core/Logs.h"
#import <MediaPlayer/MediaPlayer.h>



void MoviePlayerIPhone::_trace(const char* msg) const
{
	//if(SHOW_TRACE)
	//	{
	Logs::trace("MoviePlayerIPhone", msg);
	//	}
}

MoviePlayerIPhone::MoviePlayerIPhone()
{
	_trace("->MoviePlayer()");
	icontrolMode = MPMovieControlModeHidden;

#if _DEBUG	
	//if debug allow controls by default
	icontrolMode = MPMovieControlModeDefault;
#endif
	
	iscalingMode = MPMovieScalingModeNone;
//	setScalingMode( MPMovieScalingModeNone);
	
	//imovieURL;
	//setBackgroundUIColor( [UIColor blackColor]);
	
	_trace("->MoviePlayer()");
};

void MoviePlayerIPhone::setControlMode(int cm)
{
	_trace("->setControlMode()");
	//icontrolMode = cm;
	_trace("<-setControlMode()");
}


//void MoviePlayerIPhone::setBackgroundUIColor(UIColor color)
//{
//	_trace("->setBackgroundColor()");
//	ibackgroundColor = color;
//	_trace("<-setBackgroundColor()");
//}

void MoviePlayerIPhone::setBackgroundColor(int red, int blue, int green, int alpha)
{
	_trace("->setBackgroundColor(r,b,g,a)");
	
	//setBackgroundUIColor([UIColor colorWithRed:red green:green blue:blue alpha:alpha]);
	
	_trace("<-setBackgroundColor(r,b,g,a)");
}
void MoviePlayerIPhone::setBackgroundColor(int argb)
{
	_trace("->setBackgroundColor(argb)");
	setBackgroundColor( (argb >> 16) & 0xFF , (argb >> 8) & 0xFF , argb & 0xFF , (argb >> 24) & 0xFF );
	_trace("<-setBackgroundColor()");
}

void MoviePlayerIPhone::setScalingMode(int sm)
{
	_trace("->setScalingMode()");
	//iscalingMode = sm;
	_trace("<-setScalingMode()");
}


void MoviePlayerIPhone::playMovie(const char* name)
{
	
	_trace("->playMovie()");
	
	//NSString* moviePath = [[NSBundle mainBundle] pathForResource:@"glogo" ofType:@"m4v"];
	//NSURL* movieURL = [NSURL fileURLWithPath:moviePath];
	
	//MPMoviePlayerController* mpmpc = [[MPMoviePlayerController alloc]   initWithContentURL:movieURL];

	
	
	//[mpmpc setMovieControlMode:(MPMovieControlMode)icontrolMode];
	//[mpmpc setScalingMode:(MPMovieScalingMode)iscalingMode];
//	[mpmpc setBackgroundColor:ibackgroundColor ];
	//[mpmpc play];
	
	
	_trace("<-playMovie()");	
	
};

void MoviePlayerIPhone::stopMovie()
{
	_trace("->stopMovie()");

	_trace("<-stopMovie()");
}

