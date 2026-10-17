
#import <UIKit/UIKit.h>
#import "IGP.h"



@class EAGLView;

@interface IGPAppDelegate : NSObject <UIApplicationDelegate>{
	IBOutlet UIWindow *window;
	IBOutlet EAGLView *glView;
}
char			langs;
IGP*			igp;
IGPTexture*		bg;
UIButton**		buttons;
UITextView*		infoView;
UIApplication*	application;

- (void) initInfoBox;
- (void) enableLanguageButtons;
- (void) disableLanguageButtons;
- (void) initIGPWithLanguageIndex:(int) index;
- (UIButton*)roundedButtonWithText:(NSString*)text andIndex:(int)index;


@property (nonatomic, retain) UIWindow		*window;

@end

