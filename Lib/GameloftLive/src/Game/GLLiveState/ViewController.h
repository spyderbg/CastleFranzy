//
//  ViewController.h
//
//  Created by liwei2 on 22/04/09.
//  Copyright 2009 Gameloft. All rights reserved.
//

#import <UIKit/UIKit.h>
@interface GLLiveViewController : UINavigationController <UITextFieldDelegate, UITextViewDelegate, UIApplicationDelegate, UIAlertViewDelegate>
{

	UIViewController*	m_subVC;

	UITextView*			textView;
	UITextField*		textField;
	UIView*				m_view;
	//UIButton*			OKButton;
	//UIButton*			BackButton;
	char*				m_text;
	int					m_maxlen;
	bool				m_isView;
	bool				m_isPwd;
	int                 m_width;
	int					m_middle;
}
@property(nonatomic,retain) UITextView*			textView;
@property(nonatomic,retain) UITextField*		textField;

- (id)init:(char*)value LengthOfText:(int)maxlen IsView:(BOOL)isView IsPwd:(BOOL)isPwd Width:(int)w HMiddle:(int)hMiddle;
- (void) myInit;
- (void)onRotateDevice;
- (void)clickBack;
- (void)clickOK;
- (void)endInput;
@end
