//
//  EAGLView.m
//  iPhoneOnlineDemo
//
//  Created by Mou Bao Yu on 09-1-7.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//



#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import <sys/time.h>
#import "EAGLView.h"

#include "Game.h"


#define USE_DEPTH_BUFFER 0

const char*     g_mySavePath;
const char*     RECORD_SAVE_FILE_PATH;
char            record_Name[200];
unsigned int g_mouseX;
unsigned int g_mouseY;
bool	g_bMousePressed;

CGame*			g_pGameApp = NULL;
char			g_moudlePath[512];

void InitMacSavePath();

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
        
		[self setMultipleTouchEnabled:YES];
		[self setExclusiveTouch:YES];
		
        animationInterval = 1.0 / 60.0;
		
		// initialize game
		srand(time(NULL));
		g_pGameApp = NEW CGame();		
		
		g_pGameApp->Initialize();
		g_pGameApp->GetDisplay()->SetDeviceOrientation(0);
		g_pGameApp->UpdateOrientation();
		
    }
    return self;
}


- (void)drawView {
    
    g_pGameApp->Update();
	g_pGameApp->Paint();
    
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
	
	if(g_pGameApp->IsTerminal())
	{
		[[UIApplication sharedApplication] terminate];
	}
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

void InitMacSavePath()
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES); 
	NSString *documentsDirectory = [paths objectAtIndex:0];
	g_mySavePath = [documentsDirectory UTF8String];
	strcpy(record_Name, g_mySavePath);
	NSUInteger len = [documentsDirectory length];
	//strcat(record_Name,"/rfSave");
	RECORD_SAVE_FILE_PATH = record_Name;
}

#pragma mark -
#pragma mark === Touch handling  ===
#pragma mark

// Handles the start of a touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch *touch in touches) {
		//[self CheckIgspTouch:touch];
		
		CGPoint iPoint = [touch locationInView:self];
		g_mouseX = iPoint.x;
		g_mouseY = iPoint.y;
		g_bMousePressed = true;
	}
}

// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch *touch in touches) {
	    //[self CheckIgspTouch:touch];
		//CheckIgpTouch( touch );
		CGPoint iPoint = [touch locationInView:self];
		g_mouseX = iPoint.x;
		g_mouseY = iPoint.y;
		g_bMousePressed = true;
	}
}

// Handles the end of a touch event.
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch *touch in touches) {
		//[self CheckIgspTouch:touch];		
		CGPoint iPoint = [touch locationInView:self];
		g_mouseX = iPoint.x;
		g_mouseY = iPoint.y;
		g_bMousePressed = false;
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch *touch in touches) {
	    //[self CheckIgspTouch:touch];
		CGPoint iPoint = [touch locationInView:self];
		g_mouseX = iPoint.x;
		g_mouseY = iPoint.y;
		g_bMousePressed = false;
	}
}

uint32 GetMSTime()
{
	timeval t;
	gettimeofday(&t, NULL);
	return (t.tv_sec & 0xffff) * 1000 + t.tv_usec / 1000;	
}

@end
