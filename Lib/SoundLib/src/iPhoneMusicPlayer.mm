
#import <MediaPlayer/MediaPlayer.h>

bool g_canUsePlayer = false;

void Init_iPodPlayer ()
{
	Class mpc = NSClassFromString(@"MPMusicPlayerController");
	if (mpc)
	{
		g_canUsePlayer = true;
	}
	else
	{
		g_canUsePlayer = false;
	}
}

int iPodPlayerState()
{	
	//printf("  using iPod player\n");
	NSInteger currentPlayerState = [MPMusicPlayerController iPodMusicPlayer].playbackState;
	
	printf("    currentPlayerState = %d\n", currentPlayerState);
	
	if( currentPlayerState == MPMusicPlaybackStateInterrupted )
	{
		[[MPMusicPlayerController iPodMusicPlayer] stop];
	}
	
	return ( currentPlayerState == MPMusicPlaybackStatePlaying );
}

