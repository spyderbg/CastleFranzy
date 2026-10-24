//
//  EAGLView.mm
//
//  Castle Defence
//
//  Created by Plamen Chernev on 03/09/09.
//  Copyright Gameloft 2009. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import <MediaPlayer/MediaPlayer.h>
#import "EAGLView.h"
#import "IGP.h"
#import "GLLiveGame.h"

#import "Core/Application.h"
#import "IO/TouchScreen/TouchPad.h"
#import "IO/Accelerometer/AccelerometerBase.h"

#pragma comment(lib, "libIrrlicht.a")

#include <irrlicht.h>
#include <sys/time.h>

///////////////////////////////////////////////////////////////////////

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

///////////////////////////////////////////////////////////////////////

#define USE_DEPTH_BUFFER 1

///////////////////////////////////////////////////////////////////////

extern IGP* 		igp;
extern int 			_launchIGPLang;

extern CGLLive* 	glLive;
extern int 			_launchGLLiveLang;

///////////////////////////////////////////////////////////////////////

#if USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD

	pthread_t 			gameUpdateThread = 0;
	pthread_mutex_t 	g_igpMutex;

#endif

NSString* g_movieURL = nil;
pthread_mutex_t g_movieURLMutex;
id g_eaglView = nil;

bool 			g_gameIsRunning	= false;
volatile bool 	g_enableRender	= true;
volatile bool g_movieIsPlaying = false;
id g_mpMoviePlayer = nil;

// A class extension to declare private methods
@interface EAGLView ()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *animationTimer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

@implementation EAGLView

@synthesize context;
@synthesize animationTimer;
@synthesize animationInterval;


// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

void SetResourcePathASCII(char* cBuffer, int iLength)
{
	NSString* readPath = [[NSBundle mainBundle] resourcePath];
	[readPath getCString:cBuffer maxLength:iLength encoding:NSASCIIStringEncoding];
}


//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {
    
    if ((self = [super initWithCoder:coder])) {
		//Set up the ability to track multiple touches
		[self setMultipleTouchEnabled:YES];
		
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context]) {
            [self release];
            return nil;
        }
        
        animationInterval = 1.0 / 60.0;
    
		UIScreen* screen = [UIScreen mainScreen];
		CGRect screenBounds = [screen bounds];
		
				
		IrrlichtDevice* device = createDevice( video::EDT_OPENGL, dimension2d<int>(320, 480), 16, false, false, false, 0);
		
		[self generatePaths];
		
		__APP()->InitIPhone(device, screenBounds.origin.x,
						screenBounds.origin.y,
						screenBounds.origin.x+screenBounds.size.width,
						screenBounds.origin.y+screenBounds.size.height);
		
		g_eaglView = self;

#if USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD	
		
		pthread_mutex_init(&g_movieURLMutex, NULL);
		pthread_mutex_init(&g_igpMutex, NULL);				
		
#endif	
	}
    return self;
}


- (void)drawView
{
	
	long time = __IRR_DEVICE()->getTimer()->getRealTime();
	
	bool is_update = true;
	
#if USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD	
	pthread_mutex_lock(&g_movieURLMutex);	
	if(g_movieURL && g_mpMoviePlayer == nil)		
	{
		[self playMovie:g_movieURL];	
	}
	pthread_mutex_unlock(&g_movieURLMutex);				
	
	is_update = _launchIGPLang != -1 || _launchGLLiveLang != -1;
	
#endif
	
	if( is_update )
	{	
		IGP_MUTEX_LOCK		

		if( __IRR_DEVICE()->run() && g_enableRender )
		{
			if( !igp || igp->MustRender() )
			{
				[self setRenderbuffer];
			}
			
			__APP()->Update();
		
			if( !igp || igp->MustRender() )
			{
				[self presentRenderbuffer];
			}
		}		
		
		IGP_MUTEX_UNLOCK
		
		/// Update timer
//		int dt = __IRR_DEVICE()->getTimer()->getRealTime() - time;
//		int diff = 1000 / __APP()->GetTargetFPS() - dt;
		
//		if(diff < 5)	diff = 5;
		
//		animationInterval = (1.0 * diff / 1000);
//		[self startAnimation];			
	}
}

- (void)setRenderbuffer
{	
	[EAGLContext setCurrentContext:context];
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
}

- (void)presentRenderbuffer
{	
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];	
}

- (void)layoutSubviews
{
	IGP_MUTEX_LOCK

    [EAGLContext setCurrentContext:context];
    [self destroyFramebuffer];
    [self createFramebuffer];
	
	IGP_MUTEX_UNLOCK		

	[self startAnimation];	
}

- (BOOL)createFramebuffer
{
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if (USE_DEPTH_BUFFER) {
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    }
    
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
//        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }
	
	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT);
    
    return YES;
}

- (void)destroyFramebuffer
{    
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}

- (void)startAnimation {    

	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(movieFinishedCallback:)
												 name:MPMoviePlayerPlaybackDidFinishNotification
											   object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(moviePreloadedCallback:)
												 name:MPMoviePlayerContentPreloadDidFinishNotification
													object:nil];	
		
	self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
	
	if( !g_gameIsRunning )
	{
		g_gameIsRunning = true;	
		
#if USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD
		pthread_attr_t attr;
		
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		
		if (pthread_create(&gameUpdateThread, &attr, &gameUpdateRoutine, self)) 
		{
//			NSLog(@"failed to create the update game thread");		
			exit(1);
		} 
		pthread_attr_destroy(&attr);		
#endif		
	}	
}

- (void)stopAnimation {

	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:MPMoviePlayerContentPreloadDidFinishNotification
														object:nil];

	[[NSNotificationCenter defaultCenter] removeObserver:self
											 name:MPMoviePlayerPlaybackDidFinishNotification
											    object:nil];	
	
    self.animationTimer = nil;

	if( g_gameIsRunning )
	{
		g_gameIsRunning = false;
		
#if (USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD)	
		if (gameUpdateThread)
		{		
			void *status;
			int err = pthread_join(gameUpdateThread, &status);
			if (err)
			{
//				NSLog(@"failed to join the update game thread");		
			}
			gameUpdateThread = 0;
		}
#endif
	}
}

- (void)restartAnimation
{    
	self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}

- (void)setAnimationTimer:(NSTimer *)newTimer
{
    [animationTimer invalidate];
    animationTimer = newTimer;
}

- (void)setAnimationInterval:(NSTimeInterval)interval
{    
    animationInterval = interval;
    if (animationTimer) {
        [self restartAnimation];
    }
}

- (void)dealloc
{    
    [self stopAnimation];
	
#if USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD
    
	pthread_mutex_destroy(&g_movieURLMutex);		
	pthread_mutex_destroy(&g_igpMutex);		
	
#endif	
	
    if ([EAGLContext currentContext] == context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [context release];  
	[g_mpMoviePlayer release];
    [super dealloc];
	
	if( igp ) 
		delete igp;
	if( glLive )
		delete glLive;
	
	delete __APP();
}

///////////////////////////////////////////////////////////////////////
/// Generate paths
///////////////////////////////////////////////////////////////////////

- (void)generatePaths
{
	NSBundle* bundle = [NSBundle mainBundle];
	NSString* exec = [bundle executablePath];
	NSString* reso = [bundle resourcePath];
	
	// Root
	char exec_str[1024];
	[exec getCString:exec_str];
//	NSLog(@"exec path = %s", exec_str);
	
	// Resources 
	char reso_str[1024];
	reso = [reso stringByAppendingString:@"/"];
	[reso getCString:reso_str];
//	NSLog(@"resource path = %s", reso_str);
	
	// Savefiles
	char save_str[1024];
	NSArray *save_path = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* save = [save_path objectAtIndex:0];
	save = [save stringByAppendingString:@"/"];
	[save getCString:save_str];
//	NSLog(@"save path = %s", save_str);
	
	__APP()->setFilePaths( exec_str, reso_str, save_str );		
}


///////////////////////////////////////////////////////////////////////
/// Touch handling
///////////////////////////////////////////////////////////////////////

extern void HandleTouch(NSSet *touches);

- (CGPoint)ConvertCoordP:(CGPoint)p
{
	short tmpX;
	CGPoint result = p;
	UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
		
	switch(orientation)
	{
		case UIInterfaceOrientationPortrait:
			break;
			
		case UIInterfaceOrientationLandscapeRight:
			tmpX = p.x;
			result.x = p.y;
			result.y = 320 - tmpX;
			break;
			
		case UIInterfaceOrientationLandscapeLeft:
			tmpX = p.x;
			result.x = 480 - p.y;
			result.y = tmpX;
			break;
	}
	return result;
}

- (int) getTouchIndex: (UITouch*)touch
{
	int i;
	for(i = 0 ; i < MAX_TOUCHES ; i++)
	{
		if(touch == m_crtTouches[i])
		{
			return i;
		}
	}
	return -1;
}

- (int) addTouch: (UITouch*) touch
{
	int i;
	for (i=0; i < MAX_TOUCHES; i++)
	{
		if (m_crtTouches[i] == touch)
		{
			return -1;
		}
		else
			if (m_crtTouches[i] == NULL)
			{
				m_crtTouches[i] = touch;
				m_touchCount ++;
				return i;
			}
	}
	return -1;
}

- (int) removeTouch: (UITouch*) touch
{
	int touchIndex = [self getTouchIndex:touch];
	
	if(touchIndex != -1)
	{
		m_crtTouches[touchIndex] = NULL;
		m_touchCount --;
		return touchIndex;
	}
	
	return -1;
}

- (void)touchesChangedWithEvent:(UIEvent*)event
{	
	NSSet *touches = [event allTouches];
	
	//remove unused touches
	for (int i=0; i < MAX_TOUCHES; i++)
	{
		if (m_crtTouches[i] != NULL)
		{
			bool bFound = false;
			for (UITouch *myTouch in touches)
			{
				if(m_crtTouches[i] == myTouch)
				{
					bFound = true;
					break;
				}
			}
			
			if(!bFound)
			{
				m_crtTouches[i] = NULL;
				CTouchPad::EventTouchUp(i, -1, -1);
				m_touchCount--;			
			}
		}
	}
	
	for (UITouch *myTouch in touches)
	{
		int ind = [self getTouchIndex:myTouch];
		
		switch (myTouch.phase)
		{
			case UITouchPhaseBegan:
			{
				if (m_touchCount < MAX_TOUCHES && ind == -1)
				{
					ind = [self addTouch:myTouch];
					
					if (ind != -1)
					{
						CGPoint touchPoint = [myTouch locationInView:self];
						
						int fingerX = touchPoint.x;
						int fingerY = touchPoint.y;
						CTouchPad::EventTouchDown(ind, fingerX, fingerY);
					}
				}
			}
				break;
			case UITouchPhaseMoved:
			{
				if (ind != -1)
				{
					CGPoint touchPoint = [myTouch locationInView:self];
					
					int fingerX = touchPoint.x;
					int fingerY = touchPoint.y;
					CTouchPad::EventTouchMove(ind, fingerX, fingerY);
				}
			}
				break;
			case UITouchPhaseEnded:
			case UITouchPhaseCancelled:
			{
				ind = [self removeTouch:myTouch];
				if (ind != -1)
				{
					CGPoint touchPoint = [myTouch locationInView:self];
					
					int fingerX = touchPoint.x;
					int fingerY = touchPoint.y;
					CTouchPad::EventTouchUp(ind, fingerX, fingerY);
				}
			}
				break;
		}
		
	}
}

- (void)handleTouch:(UITouch *)touch
{
	if( igp )
	{
		IGP_MUTEX_LOCK
		
		Touch t;
		t.phase = touch.phase;
		t.curLocation = [touch locationInView:self];
		t.prevLocation = [touch previousLocationInView:self];
		t.tapCount = [touch tapCount];
		igp->handleTouch(t);
		
		IGP_MUTEX_UNLOCK
	}
	
	if( glLive )
	{
		IGP_MUTEX_LOCK
		
		CGPoint iPoint = [touch locationInView:self];
		glLive->handleTouch(iPoint.x, iPoint.y, touch.phase != UITouchPhaseEnded && touch.phase != UITouchPhaseCancelled);

		IGP_MUTEX_UNLOCK		
	}
}

// Handles the start of a touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	if( glLive || igp )
	{
		for (UITouch *touch in touches) 
		{
			[self handleTouch:touch];
		}
	}
			
	else
	{
		[self touchesChangedWithEvent:event];		
	}
}

// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	if( glLive || igp )
	{
		for (UITouch *touch in touches) 
		{
			[self handleTouch:touch];
		}
	}
	else
	{
		[self touchesChangedWithEvent:event];
	}
}

// Handles the end of a touch event.
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	if( glLive || igp )
	{
		for (UITouch *touch in touches) 	
		{
			[self handleTouch:touch];
		}
	}
	else
	{
		[self touchesChangedWithEvent:event];
	}
}

// Cancel the touch event.
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	if( glLive || igp )
	{
		for (UITouch *touch in touches)
		{
			[self handleTouch:touch];
		}
	}
	else
	{
		[self touchesChangedWithEvent:event];		
	}
}

///////////////////////////////////////////////////////////////////////

- (void)pause
{
	g_enableRender = false;	

	if( igp )
	{		
   		igp->HideNotify();		
	}
	else if( glLive )
	{
//		glLive->Pause();
	}		
	else
	{
		__APP()->Pause();
	}	
}

- (void)resume
{
	g_enableRender = true;	
	
	if( igp )
	{
   		igp->ShowNotify();
	}
	else if( glLive )
	{
//		glLive->Resume();
	}	
	else
	{
		__APP()->Resume();
	}	
}

- (void)movieFinishedCallback:(NSNotification*)aNotification
{
	if(!b_isMoviePaused)
	{
		pthread_mutex_lock(&g_movieURLMutex);

		MPMoviePlayerController* theMovie =[aNotification object];

		[[NSNotificationCenter defaultCenter] removeObserver:self
					name:MPMoviePlayerPlaybackDidFinishNotification
					object:theMovie];
			
		//[theMovie release];
		
		if(g_mpMoviePlayer != nil)
		{
			[theMovie release];
			theMovie = nil;
		}
		g_mpMoviePlayer = nil;
		[g_movieURL release];
		g_movieURL = nil;
		g_movieIsPlaying = false;	
		g_enableRender = true;	
		b_isMoviePaused = FALSE;
		
		pthread_mutex_unlock(&g_movieURLMutex);
	}
}

- (void)moviePreloadedCallback:(NSNotification*)aNotification
{
//	NSLog(@"Movie preloaded");		
	
	MPMoviePlayerController* theMovie =[aNotification object];
	
//	NSLog(@"Start playing movie");
	
	[theMovie play];	
}

-(void)playMovie:(NSString*)path 
{
//	NSLog(@"path: %@", path);		
	
	NSURL* theURL = [[NSURL alloc] initFileURLWithPath:path];
					
//	NSLog(@"theURL: %@", theURL);		
	
//	NSLog(@"Start preloading movie");
	if(g_mpMoviePlayer == nil)
	{
		MPMoviePlayerController* theMovie = [[MPMoviePlayerController alloc] initWithContentURL:theURL];
		UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
	/*	switch(orientation)
		{
			case UIInterfaceOrientationPortrait:
				break;
				
			case UIInterfaceOrientationLandscapeLeft:
				//orientation = UIDeviceOrientationLandscapeRight;
				[theMovie setOrientation:UIDeviceOrientationLandscapeRight animated:NO];
				break;
				
			case UIInterfaceOrientationLandscapeRight:
				//orientation = UIDeviceOrientationLandscapeLeft;
				[theMovie setOrientation:UIDeviceOrientationLandscapeLeft animated:NO];
				break;
		}*/
		//[theMovie setOrientation:orientation animated:NO];


		[[NSNotificationCenter defaultCenter] addObserver:self
						selector:@selector(movieFinishedCallback:)
						name:MPMoviePlayerPlaybackDidFinishNotification
						object:theMovie];

		[theURL release];
		
		g_mpMoviePlayer = theMovie;

		
		theMovie.scalingMode = MPMovieScalingModeAspectFill;
		theMovie.movieControlMode = MPMovieControlModeDefault;
	}
}



@end

extern "C" bool MP_isMoviePlaying()
{
	return g_movieIsPlaying;
}

extern "C" void MP_stopMovie()
{
	if (g_mpMoviePlayer != nil)
	{
		[g_mpMoviePlayer stop];
	}
}

extern "C" void MP_playMovie(const char* url)
{
	pthread_mutex_lock(&g_movieURLMutex);
	
	if (g_eaglView != nil && !g_movieIsPlaying)
	{
		g_movieURL = [[NSString alloc] initWithUTF8String:url];
//		NSLog(@"Loading movie %s\n", g_movieURL);
		g_movieIsPlaying = true;
		g_enableRender = false;			
	}
	
	pthread_mutex_unlock(&g_movieURLMutex);
	
#if !(USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD)
	if (g_eaglView != nil && !g_movieIsPlaying)
	{
		[g_eaglView playMovie:g_movieURL];
		b_isMoviePaused = FALSE;
	}
#endif	
	
}
#if USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD

extern "C" void LimitFPS(float targetFPS)
{
	const float minFrameTime = 1.0f / targetFPS;
	
	static u32 s_lastTimeMS = os::Timer::getRealTime();
	
	float deltaTime = 0.001f * (os::Timer::getRealTime() - s_lastTimeMS);
	float sleepTime = minFrameTime - deltaTime;
	
	if (sleepTime > 0 && sleepTime <= minFrameTime)
	{
		CFRunLoopRunInMode(kCFRunLoopDefaultMode, sleepTime, FALSE);
	}
	
	s_lastTimeMS = os::Timer::getRealTime();
}

extern "C" void* gameUpdateRoutine(void* p)
{	
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	EAGLView* eaglView = (EAGLView*) p;
	
	while( __IRR_DEVICE()->run() && g_gameIsRunning )
	{	
		LimitFPS(40.0f);
		
		if( g_enableRender )
		{
			if( _launchIGPLang == -1 && _launchGLLiveLang == -1 )
			{
				IGP_MUTEX_LOCK			
				
				[eaglView setRenderbuffer];
				
				__APP()->Update();		
		
				[eaglView presentRenderbuffer];
				
				IGP_MUTEX_UNLOCK				
			}
				
		}	
		
		sched_yield();
	}
	
	pthread_exit(0);	
}

#endif

extern "C" void MP_updateRoutineCallback()
{
#if USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD
	
	// manage game quit while loading
	if( !g_gameIsRunning )
	{
		IGP_MUTEX_UNLOCK		
		
		pthread_exit(0);
		return;
	}
	
	// prevent interrupts from freezing while loading
	IGP_MUTEX_UNLOCK
	
	sched_yield();
	
	IGP_MUTEX_LOCK
	
#endif	
}

	
