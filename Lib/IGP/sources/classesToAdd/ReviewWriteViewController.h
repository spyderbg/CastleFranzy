//
//  ReviewWriteViewController.h
//  IGP
//
//  Created by administrador on 22/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "IGPDefines.h"
#include "IGPGame.h"
#import "GameTableViewController.h"
@interface ReviewWriteViewController : UINavigationController <UITextFieldDelegate, UITextViewDelegate, UIApplicationDelegate, UIAlertViewDelegate>
{

	UIViewController*	m_subVC;
	NSURLResponse*		downloadResponse;
	NSMutableData*		receivedData;
	UILabel*			eMailLabel;
	UITextField*		fromLabel;
	UITextField*		reviewLabel;
//	CustomizedPicker*	gamePicker;
	GameTableViewController* gameTableView; 
	UIButton*			chooseGameButton;
	UIImageView*		backgroundImageView;
	UIImageView*		reviewFieldSep[3];
	UIImageView*		reviewFieldsBG;
	UIImageView*		reviewStarsBG;
	UIImageView*		submitReview;
	UIImageView*		m_arrowUpImage;
	UITextField*		fromTextField;
	UITextView*			reviewTextView;
	UITextField*		eMailTextField;
	UIButton*			sendButton;
	UIButton*			cancelButton;
	UIAlertView*		alertView;
	bool				m_postingReview;
	bool				m_controlsVisible;
	UIActivityIndicatorView* m_activity;
	UIButton*			m_backButton;
	
	IGPGame*			m_currentGame;
	IGPGame**			m_gameList;
	int					m_gameListCount;
	UILabel*			m_titleLabel[2];
	UILabel*			m_sendText;
	UILabel*			m_cancelText;
	NSString*	m_currentGameCode;
	
	int					m_curOffset;
	bool				bReviewHasBeenPostIt; //we use it to prevent that if the user press the send button and the backbutton at the same time
}
@property(nonatomic,retain) UITextView*			reviewTextView;
@property(nonatomic,retain) UITextField*		fromTextField;
@property(nonatomic,retain) UITextField*		eMailTextField;

@property(nonatomic,retain)	UIButton*			m_backButton;
@property(nonatomic,retain) NSString*			m_currentGameCode;

- (IBAction) cancelQWERTY:(id) sender;
- (void)removeViews;
-(void) showTitle;
- (void) setCurrentGame:(IGPGame*) game;
- (void) setCurrentGamewithName:(NSString*) gameName;
- (void) setCurrentGameDefault;
- (id)initWithBackgroundImage:(UIImage*) bgImage;
- (IBAction) postReview: (id) sender;
- (void) setGameList:(IGPGame**) gameList count:(int) count;
- (void) moveControlsVertically: (int ) yOffset;
-(void) showTitleOneLine;
-(void) showGameList;
- (void) raiseReviewTextForEdit;
- (void) getDownReviewTextForEdit;
- (bool) noCharactersInString:(NSString *)text;

@end
