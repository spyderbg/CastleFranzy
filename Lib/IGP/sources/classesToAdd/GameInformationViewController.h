//
//  GameInformationViewController.h
//  IGP
//
//  Created by administrador on 22/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "IGPGame.h"


@interface GameInformationViewController : UIViewController <UIScrollViewDelegate> {
	UIViewController *_parent;
	UINavigationController* m_gotHereFrom;
	
	UIImageView* m_backgroundImageView;
	UITextView*  m_titleTextView;
	UITextView*	 m_descriptionTextView;
	UIScrollView* scrollView;
	UIButton*   m_buyButton;
	UIButton*   m_videoButton;
	UIButton*	m_backButton;
	IGPGame*	m_game;
	UIImageView *imgIcon;
	UIImageView *screenshotView;
	UIImageView *imgIconReflection;
	UILabel *	m_buyLabel;
	UILabel *	m_buyLabelPrice;
	UILabel*	m_titleLabel[2];
	char		m_currentFrom[10];
	UIActivityIndicatorView*	m_activity;//show loading animation
	bool		m_paramNewPrice;
}



- (void)removeViews;
- (id)initWithBackgroundImage:(UIImage*) bgImage;
- (void) setParent: (UIViewController*)parent;
- (void) setDefaultPropertiesText:(UITextView*)text Size:(int)size Bold:(bool)bold;
- (void) setGame:(IGPGame*) game from:(UINavigationController*) from isNewPrice:(bool) paramNewPrice;
- (void) showTitle;
- (void) setFrom:(char*) from;
- (UIImage *)reflectedImage:(UIImageView *)fromImage withHeight:(NSUInteger)height;
-(void) showTitleOneLine;
-(void ) stopLoadingAnimation;
@end
