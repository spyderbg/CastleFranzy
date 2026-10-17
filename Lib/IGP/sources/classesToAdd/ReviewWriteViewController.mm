//
//  ReviewWriteViewController.mm
//  IGP
//
//  Created by administrador on 22/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "ReviewWriteViewController.h"
#import "IGP.h"
#import "EN.h"


@implementation ReviewWriteViewController 

@synthesize reviewTextView;
@synthesize fromTextField;
@synthesize eMailTextField;

@synthesize m_backButton;
@synthesize m_currentGameCode;



- (id)initWithBackgroundImage:(UIImage*) bgImage
{
	if (self = [super init]) 
	{
		m_currentGameCode = [[NSString alloc] initWithString:@"ALLG" ];
		[self setNavigationBarHidden:YES];
		
		// create and configure the view
		CGRect cgRct = CGRectMake(0.0, 0.0, 480, 320); 
		m_subVC = [[UIViewController alloc] init];
		self.view = NULL;
	
		backgroundImageView = [[UIImageView alloc] initWithImage: bgImage];
		[m_subVC.view addSubview:backgroundImageView];
		m_subVC.title = self.title = IGPcStr::GetNSString(TXT_FEEDBACK);
		m_subVC.tabBarItem.image = IGP_IMAGE(m_feedbackTabIcon);
		
		
		chooseGameButton= [[UIButton  buttonWithType:UIButtonTypeRoundedRect] retain];
		chooseGameButton.frame=CGRectMake(15,50,290,50 );
		
		[chooseGameButton addTarget:self action:@selector(showGameList) forControlEvents:UIControlEventTouchUpInside];
		[chooseGameButton setTitle:IGPcStr::GetNSString(TXT_CHOOSE_GAME) forState:UIControlStateNormal];	
		if ([chooseGameButton.currentTitle length]>10)
		{
			[chooseGameButton setFont:[UIFont boldSystemFontOfSize:18]];	
		}
		else
		{
			[chooseGameButton setFont:[UIFont boldSystemFontOfSize:20]];
		}
		[chooseGameButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		//[chooseGameButton s

	
		reviewFieldsBG = [[UIImageView alloc] initWithFrame:CGRectMake(0, 100, 320, 380)];
		[reviewFieldsBG setBackgroundColor:[UIColor clearColor]];
		
		
		m_backButton = [[UIButton alloc] initWithFrame:CGRectMake(2, 5, 70, 36)];
		[m_backButton setImage: IGP_IMAGE(m_backButtonImage) forState:UIControlStateNormal];
		[m_backButton addTarget:self action:@selector(quitIGP:) forControlEvents:UIControlEventTouchUpInside];
		
		reviewStarsBG = [[UIImageView alloc] initWithFrame:CGRectMake(0, 45, 320, 380)];
		[reviewStarsBG setBackgroundColor:[UIColor clearColor]];


		sendButton = [[UIButton alloc] initWithFrame:CGRectMake(230,chooseGameButton.frame.origin.y + chooseGameButton.frame.size.height + 5, 85, 39)];
		cancelButton= [[UIButton alloc] initWithFrame:CGRectMake(10,chooseGameButton.frame.origin.y + chooseGameButton.frame.size.height + 5, 85, 39)];
		
		reviewFieldSep[0] = [[UIImageView alloc] initWithFrame:CGRectMake(0, 100, 320, 1)];
		reviewFieldSep[1] = [[UIImageView alloc] initWithFrame:CGRectMake(0, sendButton.frame.origin.y + sendButton.frame.size.height , 320, 1)];

		[reviewFieldSep[0] setBackgroundColor:[UIColor blackColor]];
		[reviewFieldSep[1] setBackgroundColor:[UIColor blackColor]];
		
		fromLabel = [[UITextField alloc] initWithFrame:CGRectMake(10, reviewFieldSep[1].frame.origin.y + reviewFieldSep[1].frame.size.height + 5, 320, 25)];
		eMailLabel=[[UILabel alloc] initWithFrame:CGRectMake(10, fromLabel.frame.origin.y + fromLabel.frame.size.height-10 , 320, 25)];
		reviewLabel = [[UITextField alloc] initWithFrame:CGRectMake(10,eMailLabel.frame.origin.y + eMailLabel.frame.size.height-3 , 320, 25)];
		
		
		
		//[cancelButton setTitle:cStr::GetNSString(TXT_OK) forState:Nil];
//		if ([cStr::GetNSString(TXT_OK) length] > 8)
//				cancelButton.font = [UIFont systemFontOfSize:15];
//		else
//			cancelButton.font = [UIFont systemFontOfSize:16];
		//[cancelButton.currentTitle setAdjustsFontSizeToFitWidth: YES];
		//Labels
//		[fromLabel setTextColor:[UIColor colorWithRed:75/255.0 green:128/255.0 blue:246/255.0 alpha:1]];
//		[reviewLabel setTextColor:[UIColor colorWithRed:75/255.0 green:128/255.0 blue:246/255.0 alpha:1]];
		[fromLabel setFont:[UIFont boldSystemFontOfSize:14]];
		[eMailLabel setFont:[UIFont boldSystemFontOfSize:14]];
		[reviewLabel setFont:[UIFont boldSystemFontOfSize:14]];
		[fromLabel setBackgroundColor:[UIColor clearColor]];
		[reviewLabel setBackgroundColor:[UIColor clearColor]];
		[eMailLabel setBackgroundColor:[UIColor clearColor]];
		
		[eMailLabel setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
		[eMailLabel setTextColor:[UIColor blackColor]];
		[eMailLabel setFont:[UIFont boldSystemFontOfSize:14]];
		 
		[eMailLabel setText:IGPcStr::GetNSString(TXT_EMAIL)];
		[fromLabel setText: IGPcStr::GetNSString(TXT_FROM)];		
		[reviewLabel setText: IGPcStr::GetNSString(TXT_REVIEW)];

		//titleTextField	= [[UITextField alloc] initWithFrame:CGRectMake([titleLabel.text	sizeWithFont:titleLabel.font constrainedToSize:CGSizeMake(320,480) lineBreakMode:UILineBreakModeTailTruncation ].width + titleLabel.frame.origin.x + 5 , titleLabel.frame.origin.y -2 , 320, 40)];

		reviewTextView	= [[UITextView alloc] initWithFrame:CGRectMake(0, reviewLabel.frame.origin.y+17 , 320 , 90)];
		int xCoord = [fromLabel.text	sizeWithFont:fromLabel.font constrainedToSize:CGSizeMake(320,480) lineBreakMode:UILineBreakModeTailTruncation].width + fromLabel.frame.origin.x + 5  ;
		fromTextField	= [[UITextField alloc] initWithFrame:CGRectMake( xCoord, fromLabel.frame.origin.y  , 320 -xCoord  , 25)];
		 xCoord = [eMailLabel.text	sizeWithFont:eMailLabel.font constrainedToSize:CGSizeMake(320,480) lineBreakMode:UILineBreakModeTailTruncation].width + eMailLabel.frame.origin.x + 5   ;
		eMailTextField= [[UITextField  alloc] initWithFrame:CGRectMake(xCoord, eMailLabel.frame.origin.y+4, 320 -xCoord, 25)];
		[eMailTextField setTextColor:[UIColor colorWithRed:kOrangeFontColorForReviewsR green:kOrangeFontColorForReviewsG blue:kOrangeFontColorForReviewsB alpha:1]];
		[eMailTextField setText:IGPcStr::GetNSString(TXT_DEFAULT_EMAIL)];
		[eMailTextField setFont:[UIFont boldSystemFontOfSize:14]];
		if ([eMailTextField.text length]>38)
		{
			[eMailTextField setFont:[UIFont boldSystemFontOfSize:12]];
			eMailTextField.frame=CGRectMake(eMailTextField.frame.origin.x, eMailTextField.frame.origin.y+2,eMailTextField.frame.size.width,eMailTextField.frame.size.height);
		}
		[eMailTextField setKeyboardType:UIKeyboardTypeEmailAddress];
		[eMailTextField setAutocapitalizationType:UITextAutocapitalizationTypeNone];
		[fromTextField setFont:[UIFont boldSystemFontOfSize:14]];
		[reviewTextView setFont:[UIFont boldSystemFontOfSize:14]];
		
		[fromLabel setEnabled:NO];
		[reviewLabel setEnabled:NO];

		
		[fromTextField setBackgroundColor:[UIColor clearColor]];
		[fromTextField setTextColor:[UIColor colorWithRed:kOrangeFontColorForReviewsR green:kOrangeFontColorForReviewsG blue:kOrangeFontColorForReviewsB alpha:1]];

		[reviewTextView setBackgroundColor:[UIColor clearColor]];
		[reviewTextView setTextColor:[UIColor colorWithRed:kOrangeFontColorForReviewsR green:kOrangeFontColorForReviewsG blue:kOrangeFontColorForReviewsB alpha:1]];
		[cancelButton setBackgroundImage:IGP_IMAGE(m_sendButtonImage) forState:UIControlStateNormal];
		[sendButton setBackgroundImage:IGP_IMAGE(m_sendButtonImage) forState:UIControlStateNormal];
		//UILabel *sendText= [UILabel alloc] initWithFrame:CGRectMake(0, 0, 0, 0);
		
		//[sendButton setTitle:cStr::GetNSString(TXT_SEND) forState:Nil];
		sendButton.font = [UIFont systemFontOfSize:16];
		[m_subVC.view addSubview:reviewStarsBG];

		m_sendText =  [[UILabel alloc] initWithFrame:CGRectMake(sendButton.frame.origin.x ,sendButton.frame.origin.y - 4,sendButton.frame.size.width ,sendButton.frame.size.height)];
		m_sendText.text = IGPcStr::GetNSString(TXT_SEND);
		m_sendText.font = [UIFont systemFontOfSize:16];
		m_sendText.backgroundColor = [UIColor clearColor];
		m_sendText.textAlignment = UITextAlignmentCenter;
		m_sendText.textColor = [UIColor whiteColor];
		m_cancelText = [[UILabel alloc] initWithFrame:CGRectMake(cancelButton.frame.origin.x ,cancelButton.frame.origin.y - 4,cancelButton.frame.size.width ,cancelButton.frame.size.height)];
		m_cancelText.font = [UIFont systemFontOfSize:16];
		m_cancelText.backgroundColor = [UIColor clearColor];
		m_cancelText.text = IGPcStr::GetNSString(TXT_OK);
		m_cancelText.textAlignment = UITextAlignmentCenter;
		m_cancelText.textColor = [UIColor whiteColor];
		[m_cancelText setAlpha:0];
		
		[m_subVC.view addSubview:reviewFieldsBG];
		[m_subVC.view addSubview:chooseGameButton];
					
		[m_subVC.view addSubview:eMailTextField];		
		[m_subVC.view addSubview:fromTextField];
		[m_subVC.view addSubview:reviewTextView];		
		[m_subVC.view addSubview:fromLabel];
		[m_subVC.view addSubview:reviewLabel];
		[m_subVC.view addSubview:eMailLabel];
		[m_subVC.view addSubview:reviewFieldSep[1]];
		
		[m_subVC.view addSubview:submitReview];
		[m_subVC.view addSubview:sendButton ];
		[m_subVC.view addSubview:cancelButton ];
		[m_subVC.view bringSubviewToFront:sendButton];
		

		
		
		[self showTitleOneLine];		//chris++
		[m_subVC.view addSubview: m_backButton];
		
		[m_subVC.view addSubview: m_cancelText];
		[m_subVC.view addSubview: m_sendText];
		

		[fromTextField setDelegate:self];
		[eMailTextField setDelegate:self];
		[reviewTextView setDelegate:self];

		[fromTextField setText:IGPcStr::GetNSString(TXT_DEFAULT_FROM)];
		[reviewTextView setText:IGPcStr::GetNSString(TXT_DEFAULT_REVIEW)];
		[fromTextField setClearsOnBeginEditing:YES];
		[eMailTextField setClearsOnBeginEditing:YES];
		[sendButton addTarget:self action:@selector(postReview:) forControlEvents:UIControlEventTouchUpInside];
		[cancelButton addTarget:self action:@selector(cancelQWERTY:) forControlEvents:UIControlEventTouchUpInside];

		m_activity = [[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
		[m_activity setFrame : CGRectMake(160 - 32/2, 240 - 32/2, 32, 32)];
		[m_activity startAnimating];
		
		m_currentGame = NULL;
		m_postingReview = false;
		m_controlsVisible = true;
		[cancelButton setAlpha:0];
		gameTableView=[[GameTableViewController alloc] init];
		[self pushViewController:m_subVC animated:NO];
		m_arrowUpImage = [[UIImageView alloc] initWithImage:IGP_IMAGE(m_arrowUpImage)];
		[m_arrowUpImage setTransform:CGAffineTransformMakeRotation(3.14/2)];
		[m_arrowUpImage setFrame:CGRectMake(chooseGameButton.frame.origin.x+chooseGameButton.frame.size.width-40,chooseGameButton.frame.origin.y-chooseGameButton.frame.size.height+15, 20, 20)];
		[chooseGameButton addSubview:m_arrowUpImage];
		
	}
	m_curOffset = 0;
	bReviewHasBeenPostIt=false;
	return self;
}



-(void) showGameList
{
	
	if ([reviewTextView.text length]==0)
	{	
		reviewTextView.text=@" ";
		[fromTextField setEnabled: YES];
		[fromTextField becomeFirstResponder];
	}
	[fromTextField resignFirstResponder];
	[reviewTextView resignFirstResponder];
	[eMailTextField resignFirstResponder];
	[cancelButton setAlpha:0];
	[m_cancelText setAlpha:0];
	[self pushViewController:gameTableView animated:YES];

	
	
}
-(void) showTitleOneLine
{
	
	 
	 m_titleLabel[0] = nil;
	 NSString *labelTxt= IGPcStr::GetNSString(TXT_SUBMIT_REVIEW);	
	 m_titleLabel[0] = [[UILabel alloc] initWithFrame:CGRectMake(90,12,220,25)];
	 [m_titleLabel[0] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
	 [m_titleLabel[0] setText:labelTxt];
	 m_titleLabel[0].textColor=[UIColor colorWithRed:242/255.0 green:62/255.0 blue:32/255.0 alpha:1];
	 [m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:22]];
	if ([m_titleLabel[0].text length]>18)
	{
		[m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:20]];	
	}
	 m_titleLabel[0].textAlignment= UITextAlignmentRight;
	 [m_subVC.view addSubview:m_titleLabel[0]];	
	 
}
-(void) showTitle
{
	
	NSString *labelTxt= IGPcStr::GetNSString(TXT_SUBMIT_REVIEW);
	NSArray *labelAux=[labelTxt componentsSeparatedByString:@" "];
	int posxStartLabel=120;
	
	m_titleLabel[0] = nil;
	m_titleLabel[1] = nil;
	
	NSString *labelUP=NULL;
	NSString *labelDown=NULL;
	if ([labelAux count]>1)
	{
		labelUP= [labelAux objectAtIndex:0];
		m_titleLabel[0] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel+20,0,160,20)];
		[m_titleLabel[0] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
		[m_titleLabel[0] setText:labelUP];
		[m_titleLabel[0] setTextColor: [UIColor orangeColor]];
		[m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:19]];
		m_titleLabel[0].textAlignment= UITextAlignmentRight;
		[m_subVC.view addSubview:m_titleLabel[0]];	
	}

	if ([labelAux count]==1)
	{
		m_titleLabel[1] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel,10,180,20)];
		labelDown= [labelAux objectAtIndex:0];
		if ([labelDown length]>8)
			[m_titleLabel[1] setFont:[UIFont boldSystemFontOfSize:19]];
		else
			[m_titleLabel[1] setFont:[UIFont boldSystemFontOfSize:22]];
	}
	else if([labelAux count]>=2)//concatenate string [2] " " and [3] and ....
	{
		m_titleLabel[1] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel,19,180,20)];
		labelDown=[labelAux objectAtIndex:1];
		for (int i=2;i<[labelAux count];i++)
		{
			labelDown=[labelDown stringByAppendingString:@" "];
			labelDown=[labelDown stringByAppendingString:[labelAux objectAtIndex:i]];	
		}
		[m_titleLabel[1] setFont:[UIFont boldSystemFontOfSize:19]];		
	}
	[m_titleLabel[1] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
	[m_titleLabel[1] setText:labelDown];
	m_titleLabel[1].textColor=[UIColor colorWithRed:242/255.0 green:62/255.0 blue:32/255.0 alpha:1];
	m_titleLabel[1].textAlignment= UITextAlignmentRight;
	[m_subVC.view addSubview:m_titleLabel[1]];
		
}


-(void)textFieldDidBeginEditing:(UITextField *)textField
{
//	[reviewTextView resignFirstResponder];
	[cancelButton setAlpha:1];
	[m_cancelText setAlpha:1];

	//[gamePicker close];
	
	[eMailTextField setFont:[UIFont boldSystemFontOfSize:14]];
	if ([eMailTextField.text length]>38)
	{
		[eMailTextField setFont:[UIFont boldSystemFontOfSize:12]];
		eMailTextField.frame=CGRectMake(eMailTextField.frame.origin.x, eMailTextField.frame.origin.y,eMailTextField.frame.size.width,eMailTextField.frame.size.height);
	}
	[textField setClearsOnBeginEditing:NO];
	if(([textField.text isEqualToString:IGPcStr::GetNSString(TXT_DEFAULT_FROM)])||([textField.text isEqualToString:IGPcStr::GetNSString(TXT_DEFAULT_EMAIL)]))
	{
		[textField setText:@""];
	}
	}

- (void)textViewDidChangeSelection:(UITextView*) textView
{
	//[Bug id:2154657]

	//salcideon commented - this should fix the keyboard bugs
	//[self textViewDidBeginEditing: textView];
	
}

- (BOOL)textViewShouldBeginEditing:(UITextView *)textView
{
	if ([reviewTextView.text isEqualToString: @" "])//pup a avoid can't hide keyboard if text is empty issue
		reviewTextView.text = @"";
	[self raiseReviewTextForEdit];
	[cancelButton setAlpha:1];
	[m_cancelText setAlpha:1];
	reviewTextView.frame = CGRectMake(reviewTextView.frame.origin.x, reviewTextView.frame.origin.y , reviewTextView.frame.size.width, 90);
	if ([textView.text isEqualToString:IGPcStr::GetNSString(TXT_DEFAULT_REVIEW)] || [textView.text isEqualToString: @""])
	{
		[textView setText:@""];
	}
	return true;
}

- (void)textViewDidBeginEditing:(UITextView*) textView
{
//	[self raiseReviewTextForEdit];
	//fromTextField
//	reviewTextView.frame = CGRectMake(reviewTextView.frame.origin.x, reviewTextView.frame.origin.y, reviewTextView.frame.size.width, reviewTextView.frame.size.height -10);
	if ([reviewTextView.text isEqualToString: @" "])//pup a avoid can't hide keyboard if text is empty issue
		reviewTextView.text = @"";
//	[gamePicker close];
	//pup c no more fields moving [self  moveControlsVertically: - (kYTranslationWhenKeyboardOn)];
	//if(fromTextField.text.length > 0 && eMailTextField.text.length > 0 && reviewTextView.text.length > 0)
	[cancelButton setAlpha:1];
	[m_cancelText setAlpha:1];
	if ([textView.text isEqualToString:IGPcStr::GetNSString(TXT_DEFAULT_REVIEW)] || [textView.text isEqualToString: @""])
	{
		[textView setText:@""];
	}
//	reviewTextView.frame = CGRectMake(reviewTextView.frame.origin.x, reviewTextView.frame.origin.y , reviewTextView.frame.size.width, 40);
}



	

- (void)textViewDidEndEditing:(UITextView *)textView;
{
	//if (textView.isEditing)
	//[textView resignFirstResponder];
//	[gamePicker open];
	[ self getDownReviewTextForEdit];
	
	//pup c no more fields moving 	[self  moveControlsVertically:  kYTranslationWhenKeyboardOn];
	//[textView resignFirstResponder];
	[reviewTextView setSelectedRange:NSMakeRange(0, reviewTextView.frame.size.height)];
	reviewTextView.frame = CGRectMake(reviewTextView.frame.origin.x, reviewTextView.frame.origin.y , reviewTextView.frame.size.width, 200);


}


- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    [connection release];
	
	alertView = [[UIAlertView alloc] initWithTitle:IGPcStr::GetNSString(TXT_ERROR) message:IGPcStr::GetNSString(TXT_ERROR_POST) delegate:nil cancelButtonTitle:IGPcStr::GetNSString(TXT_OK) otherButtonTitles: nil];
	alertView.tag = -1;
	alertView.delegate = self;
	m_postingReview = false;
	self.view.userInteractionEnabled = true;
	IGP::s_igpInstance->_whatsnewtab.tabBarItem.enabled = YES;
	IGP::s_igpInstance->_catalogtab.tabBarItem.enabled = YES;
	IGP::s_igpInstance->_bestsellerstab.tabBarItem.enabled = YES;
	IGP::s_igpInstance->_reviewtab.tabBarItem.enabled = YES;
	[m_activity removeFromSuperview];
	[alertView show];
	bReviewHasBeenPostIt=false;
	
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	
	char* receivedBytes = (char*)[receivedData mutableBytes];
	printf("REC:%s\n", receivedBytes);

	m_postingReview = false;
	alertView = [[UIAlertView alloc] initWithTitle:IGPcStr::GetNSString(TXT_REVIEW_SENT)  message:IGPcStr::GetNSString(TXT_THANK_FEEDBACK)  delegate:nil cancelButtonTitle:IGPcStr::GetNSString(TXT_OK) otherButtonTitles: nil];
	alertView.tag = 1;
	alertView.delegate = self;
	self.view.userInteractionEnabled = true;
	IGP::s_igpInstance->_whatsnewtab.tabBarItem.enabled = YES;
	IGP::s_igpInstance->_catalogtab.tabBarItem.enabled = YES;
	IGP::s_igpInstance->_bestsellerstab.tabBarItem.enabled = YES;
	IGP::s_igpInstance->_reviewtab.tabBarItem.enabled = YES;
	[m_activity removeFromSuperview];
	[alertView show];
	SAFE_DELETE_OBJC(downloadResponse);
	bReviewHasBeenPostIt=false;
	
}
- (void)setDownloadResponse:(NSURLResponse *)aDownloadResponse
{
    [aDownloadResponse retain];
    [downloadResponse release];
    downloadResponse = aDownloadResponse;
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
	[receivedData setLength:0];
    [self setDownloadResponse:response];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
//    bytesReceived = bytesReceived + [data length];
	[receivedData appendData:data];
}

- (IBAction) cancelQWERTY:(id) sender
{
	[cancelButton setAlpha:0];
	[m_cancelText setAlpha:0];
	[fromTextField resignFirstResponder];
	[eMailTextField resignFirstResponder];
	if ([reviewTextView.text length]==0)//pup a avoid can't hide keyboard if text is empty issue
		reviewTextView.text = @" ";
	[reviewTextView resignFirstResponder];
	//[gamePicker open];
}
- (IBAction) postReview:(id) sender
{
	
	if (!m_postingReview)
	{
		NSString* error = nil;
		if (([reviewTextView.text isEqualToString:IGPcStr::GetNSString(TXT_DEFAULT_REVIEW)])||([reviewTextView.text length]==0)|| [self noCharactersInString: reviewTextView.text])
		{
			error = IGPcStr::GetNSString(TXT_PLEASE_WRITE_REVIEW);
		}
		else if (([fromTextField.text isEqualToString:IGPcStr::GetNSString(TXT_DEFAULT_FROM)])||([fromTextField.text length]==0)|| [self noCharactersInString: fromTextField.text])
		{
			error = IGPcStr::GetNSString(TXT_PLEASE_WRITE_FROM);
		}
		else if ([chooseGameButton.currentTitle  isEqualToString:IGPcStr::GetNSString(TXT_CHOOSE_GAME) ])
		{
			error = IGPcStr::GetNSString(TXT_CHOOSE_GAME);
			
		}
		else
		{
			//activate flag only if it's about to send
			bReviewHasBeenPostIt=true;
		}
		
		
		if (error != nil)
		{
			alertView = [[UIAlertView alloc] initWithTitle:IGPcStr::GetNSString(TXT_ERROR) message:error delegate:nil cancelButtonTitle:IGPcStr::GetNSString(TXT_OK) otherButtonTitles: nil];
			alertView.tag = -1;
			alertView.delegate = self;
			[alertView show];
			return;
		}
		[cancelButton setAlpha:0];
		[m_cancelText setAlpha:0];
		[reviewTextView resignFirstResponder];
		[fromTextField resignFirstResponder];
		[eMailTextField resignFirstResponder];
		
		[self.view addSubview:m_activity];
		m_postingReview = true;
		self.view.userInteractionEnabled = false;
		IGP::s_igpInstance->_whatsnewtab.tabBarItem.enabled = NO;
		IGP::s_igpInstance->_catalogtab.tabBarItem.enabled = NO;
		IGP::s_igpInstance->_bestsellerstab.tabBarItem.enabled = NO;
		IGP::s_igpInstance->_reviewtab.tabBarItem.enabled = NO;
		NSMutableURLRequest* req = [[NSMutableURLRequest alloc] initWithURL:[NSURL URLWithString:@"http://ingameads.gameloft.com/redir/review.php"] cachePolicy:nil timeoutInterval:10];
		
		[req setHTTPMethod: @"POST"];
		
		NSString *post = @"game=";
		post = [post stringByAppendingString:[NSString stringWithUTF8String:IGP::s_igpDataGame]];
		post = [post stringByAppendingString:@"&action=post"];
		post = [post stringByAppendingString:@"&ver="];
		post = [post stringByAppendingString:[NSString stringWithUTF8String:IGP::s_igpDataVersion]];	
		post = [post stringByAppendingString:@"&country="];
		post = [post stringByAppendingString:[NSString stringWithUTF8String:IGP::s_igpDataLocale]];	
		post = [post stringByAppendingString:@"&lg="];
		post = [post stringByAppendingString:[NSString stringWithUTF8String:IGP::s_igpDataLang]];	
		post = [post stringByAppendingString:@"&d=DEVICE"];
		
		post = [post stringByAppendingString:@"&f=FIRMWARE"];
		
		post = [post stringByAppendingString:@"&udid=UDIDPHONE"];
		
		
		post = [post stringByAppendingString:@"&nickname="];
		post = [post stringByAppendingString:[fromTextField text]];
		post = [post stringByAppendingString:@"&email="];
		post = [post stringByAppendingString:[eMailTextField text]];
		post = [post stringByAppendingString:@"&title="];
		//NSString* game;

		/*if (m_currentGame == NULL)
		{
			game = @"ALLG";
		}
		else
		{
			game = [NSString stringWithUTF8String:m_currentGame->m_gameCode];
		}*/
		post = [post stringByAppendingString:m_currentGameCode];

		post = [post stringByAppendingString:@"&text="];
		post = [post stringByAppendingString:reviewTextView.text];
		post = [post stringByAppendingString:@"&rating="];
		post = [post stringByAppendingString:[NSString stringWithFormat:@"%d", 0]];
		
		post = [post stringByReplacingOccurrencesOfString:@"DEVICE"  withString:[[[UIDevice currentDevice] model]stringByReplacingOccurrencesOfString:@" " withString: @"_"]      ];
		post = [post stringByReplacingOccurrencesOfString:@"FIRMWARE"  withString:[[UIDevice currentDevice] systemVersion]];
		post = [post stringByReplacingOccurrencesOfString:@"UDIDPHONE"  withString:[[UIDevice currentDevice] uniqueIdentifier]];
		
		//	post = [post stringByAppendingString:@"post"];
		//	post = [post stringByAppendingString:@"list"];
		NSData *postData = [post dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:YES];
		
		[req setHTTPBody:postData];
		
		NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:req delegate:self];
		if (theConnection)
		{
			receivedData=[[NSMutableData data] retain];
		}
		else
		{
		}
	}
	
}
- (void) setCurrentGame:(IGPGame*) game
{
	m_currentGame = game;
	if (game)
	{
		NSString *newname = game->GetName();
		
		if([newname sizeWithFont: [UIFont boldSystemFontOfSize:18] constrainedToSize:CGSizeMake(290,100)  lineBreakMode:UILineBreakModeWordWrap].width > chooseGameButton.frame.size.width - 70)//will exceed, ad '...' and cut text
			
		{
			while ([newname sizeWithFont: [UIFont boldSystemFontOfSize:18] constrainedToSize:CGSizeMake(290,50)  lineBreakMode:UILineBreakModeWordWrap].width > chooseGameButton.frame.size.width - 70)
			{	
				newname = [newname substringWithRange:NSMakeRange(0, newname.length - 2)];
				
			}
			newname = [newname stringByAppendingString:@"..."];	
			[chooseGameButton setTitle:newname forState:UIControlStateNormal];
		}
	   else //normal size
		   {
			[chooseGameButton setTitle:newname forState:UIControlStateNormal];		   
			}
	[m_currentGameCode release];	
	m_currentGameCode = [NSString stringWithUTF8String:m_currentGame->m_gameCode];			   
	[m_currentGameCode retain];		
	}
	else
	{
		[chooseGameButton setTitle:IGPcStr::GetNSString(TXT_ALL_GAMES) forState:UIControlStateNormal];	
		[m_currentGameCode release];
		m_currentGameCode = @"ALLG";
		[m_currentGameCode retain];
	}		
	
/*	if (_currrentGameCode)
	{
		[_currrentGameCode release];
	}*/
	//_currrentGameCode = [[NSString alloc] stringWithUTF8String:IGP::s_igpDataGame ];
	
	
}

//used only when selected game is the one that launched IGP
- (void) setCurrentGamewithName:(NSString*) gameName
{
	if (gameName)
	{
	
		
		if([gameName sizeWithFont: [UIFont boldSystemFontOfSize:18] constrainedToSize:CGSizeMake(290,100)  lineBreakMode:UILineBreakModeWordWrap].width > chooseGameButton.frame.size.width - 70)//will exceed, ad '...' and cut text
			
		{
			while ([gameName sizeWithFont: [UIFont boldSystemFontOfSize:18] constrainedToSize:CGSizeMake(290,50)  lineBreakMode:UILineBreakModeWordWrap].width > chooseGameButton.frame.size.width - 70)
			{	
				gameName = [gameName substringWithRange:NSMakeRange(0, gameName.length - 2)];
				
			}
			gameName = [gameName stringByAppendingString:@"..."];	
			[chooseGameButton setTitle:gameName forState:UIControlStateNormal];
		}
		else //normal size
		{
			[chooseGameButton setTitle:gameName forState:UIControlStateNormal];		   
		}
	[m_currentGameCode release];	
	m_currentGameCode = [NSString stringWithUTF8String:IGP::s_igpDataGame];	
	[m_currentGameCode retain];	
	}
	else
	{
		[chooseGameButton setTitle:IGPcStr::GetNSString(TXT_ALL_GAMES) forState:UIControlStateNormal];	
		m_currentGameCode = @"ALLG";
	}
	//set code of the game that launched IGP
/*	if (_currrentGameCode)
	{
		[_currrentGameCode release];
	}*/

	
}


- (void) setCurrentGameDefault
{
	[chooseGameButton setTitle:IGPcStr::GetNSString(TXT_CHOOSE_GAME) forState:UIControlStateNormal];

}
/*
// Override initWithNibName:bundle: to load the view using a nib file then perform additional customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically.
- (void)loadView {
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview

    // Release anything that's not essential, such as cached data
}


- (BOOL)textFieldShouldReturn:(UITextField *)textField {


	[textField resignFirstResponder];
	[fromTextField resignFirstResponder];
	[eMailTextField resignFirstResponder];
	[reviewTextView resignFirstResponder];

	return YES;
}


- (void)textFieldDidEndEditing:(UITextField *)textField {
	[cancelButton setAlpha:0];
//	[gamePicker open];
	//pup c no more fields moving [self  moveControlsVertically:  kYTranslationWhenKeyboardOn];

	[m_cancelText setAlpha:0];	[self dismissModalViewControllerAnimated:YES];
	[self dismissModalViewControllerAnimated:YES];
}

- (void)alertView:(UIAlertView *)av didDismissWithButtonIndex:(NSInteger)buttonIndex;  // after animation
{
	if (av.tag != -1)
	{
		//[fromTextField setText:cStr::GetNSString(TXT_DEFAULT_FROM)];
		[reviewTextView setText:IGPcStr::GetNSString(TXT_DEFAULT_REVIEW)];
	}
}
- (void)removeViews
{
	if (m_titleLabel[0]) [m_titleLabel[0] removeFromSuperview];
	if (m_titleLabel[1]) [m_titleLabel[1] removeFromSuperview];
	[gameTableView removeViews];
	
	self.tabBarItem.image = nil;
	[fromLabel removeFromSuperview];
	[reviewLabel removeFromSuperview];
	[sendButton removeFromSuperview];
	[reviewFieldsBG removeFromSuperview];
	[reviewStarsBG removeFromSuperview];
	[submitReview removeFromSuperview];
	[fromTextField removeFromSuperview];
	[reviewTextView removeFromSuperview];
	//[gamePicker removeFromSuperview];
	[m_backButton setImage: nil forState:UIControlStateNormal];
	[m_backButton removeFromSuperview];
	[eMailLabel removeFromSuperview];
	[eMailTextField removeFromSuperview];
	[cancelButton removeFromSuperview];
	[backgroundImageView removeFromSuperview];
	[m_cancelText removeFromSuperview];
	[m_sendText removeFromSuperview];
	[m_arrowUpImage removeFromSuperview];
	[chooseGameButton removeFromSuperview];
	[m_titleLabel[0] removeFromSuperview];

//	for(int i = 0;i < 3;i++)
	//{
		[reviewFieldSep[1] removeFromSuperview];
		
//	}
	[fromTextField setDelegate:nil];
	[eMailTextField setDelegate:nil];
	[reviewTextView setDelegate:nil];
}

- (void)dealloc
{
	trace(DEBUG_OBJCMEM, "Releasing ReviewWriteViewController\n");
	self.tabBarItem.image = nil;
	
	[reviewFieldSep[0] release];
	[reviewFieldSep[1] release];
	[reviewFieldSep[2] release];
	
	SAFE_DELETE_OBJC(downloadResponse);
	SAFE_DELETE_OBJC(receivedData);
	SAFE_DELETE(m_activity);
	//[gameTableView dealloc];

	if (m_titleLabel[0]) [m_titleLabel[0] release];
	if (m_titleLabel[1]) [m_titleLabel[1] release];
	[fromLabel release];
	[reviewLabel release];
	[chooseGameButton release];
	[reviewFieldsBG release];
	[reviewStarsBG release];
	[submitReview release];
	[gameTableView release];
	[fromTextField release];
	//[reviewTextView release];
	[m_backButton release];
	[m_cancelText release];
	[m_sendText release];
	[sendButton release];
	[cancelButton release];
	[eMailLabel release];
	[eMailTextField release];
	[m_arrowUpImage release];
	[backgroundImageView release];
	[m_subVC release];
    [super dealloc];
}

/*- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
	[gamePicker didSelectRow:row];
	[pickerView selectRow:row inComponent:component animated:YES]; //pup a trying to fix 	2161813
}

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
	if (row == 0)
	{
		return cStr::GetNSString(TXT_ALL_GAMES);
	}
	else
	{
		return m_gameList[row - 1]->GetName();
	}
}*/
/*
- (CGFloat)pickerView:(UIPickerView *)pickerView widthForComponent:(NSInteger)component
{

	return 320;
}

- (CGFloat)pickerView:(UIPickerView *)pickerView rowHeightForComponent:(NSInteger)component
{
	return 40.0;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
	return m_gameListCount + 1;
}
*/
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
	return 1;
}

- (void) setGameList:(IGPGame**) gameList count:(int) count
{
	m_gameList		= gameList;
	m_gameListCount = count;
	//[gamePicker reloadData];
}

- (void ) quitIGP:(id) sender
{
	if (bReviewHasBeenPostIt)
	{
		return;	
	}
	if ([reviewTextView.text length]==0)
	{	
	
		reviewTextView.text  = @" ";
		[reviewTextView resignFirstResponder];
		reviewTextView.text  = @"";
	}
	else
		[reviewTextView resignFirstResponder];
	[fromTextField resignFirstResponder];
	[eMailTextField resignFirstResponder];
	[IGP::s_igpInstance->_tabBarController setSelectedIndex:0];
	[IGP::s_igpInstance->_tabBarController setViewControllers: nil];
	IGP::s_igpInstance->setState(IGP_STATE_END);
}

- (void) moveControlsVertically: (int ) yOffset
{
	if (yOffset == m_curOffset)
	{
		return;
	}
	m_curOffset = yOffset;
	sendButton.frame = CGRectMake(sendButton.frame.origin.x, sendButton.frame.origin.y + yOffset, sendButton.frame.size.width, sendButton.frame.size.height);
	cancelButton.frame = CGRectMake(cancelButton.frame.origin.x, cancelButton.frame.origin.y + yOffset, cancelButton.frame.size.width, cancelButton.frame.size.height);
	
//	reviewFieldSep[0] = [[UIImageView alloc] initWithFrame:CGRectMake(0, 100, 320, 1)];
	reviewFieldSep[1].frame = CGRectMake(reviewFieldSep[1].frame.origin.x, reviewFieldSep[1].frame.origin.y + yOffset, reviewFieldSep[1].frame.size.width, reviewFieldSep[1].frame.size.height);

	fromLabel.frame = CGRectMake(fromLabel.frame.origin.x, fromLabel.frame.origin.y + yOffset, fromLabel.frame.size.width, fromLabel.frame.size.height);
	eMailLabel.frame = CGRectMake(eMailLabel.frame.origin.x, eMailLabel.frame.origin.y + yOffset, eMailLabel.frame.size.width, eMailLabel.frame.size.height);
	reviewLabel.frame = CGRectMake(reviewLabel.frame.origin.x, reviewLabel.frame.origin.y + yOffset, reviewLabel.frame.size.width, reviewLabel.frame.size.height);
	//int reviewTextEnlargement = yOffset > 0? -30:30;
	reviewTextView.frame = CGRectMake(reviewTextView.frame.origin.x, reviewTextView.frame.origin.y + yOffset, reviewTextView.frame.size.width, reviewTextView.frame.size.height);
	fromTextField.frame = CGRectMake(fromTextField.frame.origin.x, fromTextField.frame.origin.y + yOffset, fromTextField.frame.size.width, fromTextField.frame.size.height);
	eMailTextField.frame = CGRectMake(eMailTextField.frame.origin.x, eMailTextField.frame.origin.y + yOffset, eMailTextField.frame.size.width, eMailTextField.frame.size.height);
	m_sendText.frame = CGRectMake(m_sendText.frame.origin.x, m_sendText.frame.origin.y + yOffset, m_sendText.frame.size.width, m_sendText.frame.size.height);
	m_cancelText.frame = CGRectMake(m_cancelText.frame.origin.x, m_cancelText.frame.origin.y + yOffset, m_cancelText.frame.size.width, m_cancelText.frame.size.height);
	
	
	
	[reviewTextView setSelectedRange:NSMakeRange(0, reviewTextView.frame.size.height)];
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{

//	printf("%d\n", [string UTF8String][0]);
	if ([textField.text sizeWithFont:textField.font constrainedToSize:CGSizeMake(textField.frame.size.width,textField.frame.size.height) lineBreakMode:UILineBreakModeTailTruncation].width >= textField.frame.size.width - 5
		&& [string UTF8String][0] != 0 )//don't add text if field is full, unless is backspace
		return false;
	else
	return true;
}
/*
- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text
{
//	NSLog(@"largo: %f %f %@",[textView.text sizeWithFont:textView.font constrainedToSize:CGSizeMake(textView.frame.size.width,textView.frame.size.height) lineBreakMode:UILineBreakModeTailTruncation].width ,textView.frame.size.width, text);
	if (([textView.text sizeWithFont:textView.font constrainedToSize:CGSizeMake(textView.frame.size.width,textView.frame.size.height) lineBreakMode:UILineBreakModeTailTruncation].height == textView.frame.size.height )
		&& (([text isEqualToString:@"\n"])||([textView.text sizeWithFont:textView.font constrainedToSize:CGSizeMake(textView.frame.size.width,textView.frame.size.height) lineBreakMode:UILineBreakModeTailTruncation].width >= textView.frame.size.width - 12))
		&& [text UTF8String][0] != 0)//don't add text if field is full, unless is backspace
		return false;
	else
		return true;
}
*/
//raise textview & textfield, hide the others
- (void) raiseReviewTextForEdit
{
	eMailTextField.alpha = 0;
	fromTextField.alpha = 0;
	eMailLabel.alpha = 0;
	fromLabel.alpha = 0;
	[eMailTextField setEnabled: NO];
	[fromTextField setEnabled: NO];
	reviewLabel.frame = CGRectMake(reviewTextView.frame.origin.x,reviewFieldSep[1].frame.origin.y + reviewFieldSep[1].frame.size.height + 15, reviewTextView.frame.size.width, reviewTextView.frame.size.height );
	reviewTextView.frame = CGRectMake(reviewTextView.frame.origin.x,183/*reviewLabel.frame.origin.y + reviewLabel.frame.size.height + 5*/, reviewTextView.frame.size.width, reviewTextView.frame.size.height );
	
}


- (void) getDownReviewTextForEdit
{
	eMailTextField.alpha = 1;
	fromTextField.alpha = 1;
	eMailLabel.alpha = 1;
	fromLabel.alpha = 1;
	[eMailTextField setEnabled: YES];
	[fromTextField setEnabled: YES];
	reviewLabel.frame = CGRectMake(10,eMailLabel.frame.origin.y + eMailLabel.frame.size.height-3 , 320, 25) ;
	reviewTextView.frame = CGRectMake(0, reviewLabel.frame.origin.y+17 , 320 , 90 );
	
}

//this was made to cover for a retain inc that happens on each view enter
- (void)viewDidDisappear:(BOOL)animated
{
	//NSLog(@"retain caount 5 %d",self.retainCount);
	if (IGP::s_igpInstance->m_state != IGP_STATE_END)
		[self release];
//	NSLog(@"retain caount 5 %d",self.retainCount);
}


- (void)viewDidAppear:(BOOL)animated
{
	IGP::s_igpInstance->m_selectedIndex = 3;
	[IGP::s_igpInstance->_whatsnewtab setChangingView: YES];

}


//will return true if there are no characters in the string (just spaces or \n)
- (bool) noCharactersInString:(NSString *)text
{
	if (text == Nil || text.length == 0)
		return true;
	//bool charFound = false;
	int i = 0;
	while (i < text.length)
	{
	if	(([text characterAtIndex:i] == ' ') || ([text characterAtIndex:i] == '\n'))
		i++;
	else
		return false;	//char was found
	}


	return true;
		
		
		
}

@end
