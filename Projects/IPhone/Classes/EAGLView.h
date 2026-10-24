//
//  EAGLView.h
//
//  Castle Defence
//
//  Created by Plamen Chernev on 03/09/09.
//  Copyright Gameloft 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>


#define USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD 1

#if USE_PRESENT_RENDER_BUFFER_ON_DIFFERENT_THREAD

	extern "C" float 	GetTimeMilis (void);
	extern "C" void* 	gameUpdateRoutine(void* p);
	
	extern pthread_t 			gameUpdateThread;
	extern pthread_mutex_t 		g_renderMutex;

	#define RENDER_MUTEX_LOCK		pthread_mutex_lock(&g_renderMutex);
	#define RENDER_MUTEX_UNLOCK	pthread_mutex_unlock(&g_renderMutex);

#else

	#define RENDER_MUTEX_LOCK		
	#define RENDER_MUTEX_UNLOCK	

#endif

#define MAX_TOUCHES 5
#define UPDATE_TIMER		(1.0 / 60.0) //(1.0 / 60.0)
#define ACCELEROMETRE_TIMER	(1.0 / 30.0) //(1.0 / 10.0)

extern "C" void MP_updateRoutineCallback();

/*
This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
The view content is basically an EAGL surface you render your OpenGL scene into.
Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
*/
@interface EAGLView : UIView {
    
@private
    /* The pixel dimensions of the backbuffer */
    GLint backingWidth;
    GLint backingHeight;
    
    EAGLContext *context;
    
    /* OpenGL names for the renderbuffer and framebuffers used to render to this view */
    GLuint viewRenderbuffer, viewFramebuffer;
    
    /* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
    GLuint depthRenderbuffer;
    
    NSTimer *animationTimer;
    NSTimeInterval animationInterval;
	
	CGPoint	cursorStartPos;
	bool b_isMoviePaused;
	
	/// Touches
	int						m_touchCount;
	UITouch*				m_crtTouches[MAX_TOUCHES];	
}

@property NSTimeInterval animationInterval;

- (void)generatePaths;

- (void)startAnimation;
- (void)stopAnimation;
- (void)restartAnimation;
- (void)drawView;

- (int) addTouch: (UITouch*) touch;
- (int) getTouchIndex: (UITouch*) touch;
- (int) removeTouch: (UITouch*) touch;

- (CGPoint)ConvertCoordP:(CGPoint)p;

- (void)setRenderbuffer;
- (void)presentRenderbuffer;

- (void)movieFinishedCallback:(NSNotification*)aNotification;
- (void)moviePreloadedCallback:(NSNotification*)aNotification;
- (void)playMovie:(NSString*)url;
- (void)pause;
- (void)resume;

@end

