#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#import "EAGLView.h"
#import "IGP.h"
#define USE_DEPTH_BUFFER 1

extern IGP* igp;
@interface EAGLView (EAGLViewPrivate)


- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;

@end

@interface EAGLView (EAGLViewSprite)

- (void)setupView;

@end

@implementation EAGLView

@synthesize animationInterval;

// You must implement this
+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder
{
	if((self = [super initWithCoder:coder])) {
		// Get the layer
		CAEAGLLayer *eaglLayer = (CAEAGLLayer*) self.layer;
		
		m_frameBuffer = false;
		eaglLayer.opaque = YES;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		
		if(!context || ![EAGLContext setCurrentContext:context] || ![self createFramebuffer]) {
			[self release];
			return nil;
		}
		
		animationInterval = 1.0 / 60.0;
		
		[self setupView];
	}
	
	return self;
}


- (void)layoutSubviews
{
	// [EAGLContext setCurrentContext:context];
	// if (!m_frameBuffer)
	// {
		// [self destroyFramebuffer];
		// [self createFramebuffer];
	// //	m_frameBuffer = true;
	// }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
{
    for (UITouch *myTouch in touches)
    {
		Touch t;
		t.phase			= myTouch.phase;
		t.curLocation	= [myTouch locationInView:self];
		t.prevLocation	= [myTouch previousLocationInView:self];
		t.tapCount		= [myTouch tapCount];
		if (igp) igp->handleTouch(t);
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
{
    for (UITouch *myTouch in touches)
    {
		Touch t;
		t.phase			= myTouch.phase;
		t.curLocation	= [myTouch locationInView:self];
		t.prevLocation	= [myTouch previousLocationInView:self];
		t.tapCount		= [myTouch tapCount];
		if (igp) igp->handleTouch(t);
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
{
    for (UITouch *myTouch in touches)
    {
		Touch t;
		t.phase			= myTouch.phase;
		t.curLocation	= [myTouch locationInView:self];
		t.prevLocation	= [myTouch previousLocationInView:self];
		t.tapCount		= [myTouch tapCount];
		if (igp) igp->handleTouch(t);
    }
}

- (BOOL)createFramebuffer
{
	glGenFramebuffersOES(1, &viewFramebuffer);
	glGenRenderbuffersOES(1, &viewRenderbuffer);
	
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);

	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
	
	if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	
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


- (void)startAnimation
{
	animationTimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:YES];
}


- (void)stopAnimation
{
	[animationTimer invalidate];
	animationTimer = nil;
}


- (void)setAnimationInterval:(NSTimeInterval)interval
{
	animationInterval = interval;
	
	if(animationTimer) {
		[self stopAnimation];
		[self startAnimation];
	}
}


- (void)setupView
{
	glViewport(0, 0, backingWidth, backingHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrthof(-1.0f, 1.0f, -1.5f, 1.5f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
	
}

// Updates the OpenGL view when the timer fires
- (void)pre
{
	// Make sure that you are drawing to the current context
	[EAGLContext setCurrentContext:context];	
	//glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
}
- (void) post
{
	//glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];	
}

// Stop animating and release resources when they are no longer needed.
- (void)dealloc
{
	[self stopAnimation];
	
	if([EAGLContext currentContext] == context) {
		[EAGLContext setCurrentContext:nil];
	}
	
	[context release];
	context = nil;
	
	[super dealloc];
}

@end
