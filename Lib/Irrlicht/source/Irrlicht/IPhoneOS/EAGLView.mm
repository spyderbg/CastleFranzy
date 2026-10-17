//
//  EAGLView.m
//  Gangstar2
//
//  Created by Gameloft on 08/09/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//



#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"

char s_VideoRequest[512];

#define USE_DEPTH_BUFFER 0

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


// You must implement this
+ (Class)layerClass {
	return [CAEAGLLayer class];
}


//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {

	if ((self = [super initWithCoder:coder])) {
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
	}
	return self;
}


- (void)drawView {
	
	[EAGLContext setCurrentContext:context];
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	
	[self processVideoRequest];	
	
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}


- (void)layoutSubviews {
	[EAGLContext setCurrentContext:context];
	[self destroyFramebuffer];
	[self createFramebuffer];
	[self drawView];
}


- (BOOL)createFramebuffer {
	
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
	int backingWidth = 0;
	int backingHeight = 0;

	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	if (USE_DEPTH_BUFFER) {
		glGenRenderbuffersOES(1, &depthRenderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
	}

	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	
	return YES;
}


- (void)destroyFramebuffer {
	
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
	self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}


- (void)stopAnimation {
	self.animationTimer = nil;
}


- (void)setAnimationTimer:(NSTimer *)newTimer {
	[animationTimer invalidate];
	animationTimer = newTimer;
}


- (void)setAnimationInterval:(NSTimeInterval)interval {
	
	animationInterval = interval;
	if (animationTimer) {
		[self stopAnimation];
		[self startAnimation];
	}
}


- (void)dealloc {
	
	[self stopAnimation];
	
	if ([EAGLContext currentContext] == context) {
		[EAGLContext setCurrentContext:nil];
	}
	
	[context release];	
	[super dealloc];
}

-(void)processVideoRequest
{
	if(s_VideoRequest[0] != '\0')
	{
		NSBundle *bundle = [NSBundle mainBundle];
		if (bundle) 
		{
			NSString* namePath = [self extractFileName:s_VideoRequest];
			NSString* extensionPath = [self extractFileExt:s_VideoRequest];
			
			NSString *moviePath = [bundle pathForResource:namePath ofType:extensionPath];
			if (moviePath)
			{
				NSURL *mMovieURL = [NSURL fileURLWithPath:moviePath];
				[self playMovieAtURL:mMovieURL];
			}
		}
		
		s_VideoRequest[0] = '\0';
	}
}
-(void)playMovieAtURL:(NSURL*)theURL
{
	MPMoviePlayerController* theMovie = [[MPMoviePlayerController alloc]
										 initWithContentURL:theURL];
	theMovie.scalingMode = MPMovieScalingModeAspectFill;
	//	theMovie.movieControlMode = MPMovieControlModeHidden;
	// Register for the playback finished notification.
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(myMovieFinishedCallback:)
												 name:MPMoviePlayerPlaybackDidFinishNotification
											   object:theMovie];
	// Movie playback is asynchronous, so this method returns immediately.
	[theMovie play];
}
// When the movie is done, release the controller.
-(void)myMovieFinishedCallback:(NSNotification*)aNotification
{
	MPMoviePlayerController* theMovie = [aNotification object];
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:MPMoviePlayerPlaybackDidFinishNotification
												  object:theMovie];
	// Release the movie instance created in playMovieAtURL:
	[theMovie release];
	//g_pGameApp->VideoStopped();
}

//Extract the name part of a file
- (NSString*)extractFileName:(char*)theName
{
	char* extension = strstr(theName, ".");
	if (extension == nil)
		return nil;
	
	int namelenght = (extension - theName);
	char tmp[256];
	memset(tmp, '\0', 256);
	strncpy(tmp, theName, namelenght);
	NSString* namePath = [[NSString alloc] initWithCString:tmp];
	
	return namePath;
}

//Extract the extension part of a file.
- (NSString*)extractFileExt:(char*)theName
{
	char* extension = strstr(theName, ".");
	if (extension == nil)
		return nil;
	
	NSString* extensionPath = [[NSString alloc] initWithCString:(extension+1)];
	return extensionPath;
}

@end
