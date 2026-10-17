

#import <UIKit/UIKit.h>

@class buy_buttonViewController;

@interface buy_buttonAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    buy_buttonViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet buy_buttonViewController *viewController;

@end

