//
//  ViewController.mm
//
//  Created by liwei2 on 22/04/09.
//  Copyright 2009 Gameloft. All rights reserved.
//
#include "GLLiveGame.h"
#import "ViewController.h"

@implementation GLLiveViewController 

@synthesize textView;
@synthesize textField;

- (id)init:(char *)text LengthOfText:(int)maxlen IsView:(BOOL)isView IsPwd:(BOOL)isPwd  Width:(int)w HMiddle:(int)hMiddle
{
	if(gll_isLandscapeRight)
	{
		[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeLeft];
	}
	else
	{
		[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeRight];
	}
	
	m_text = text;
	m_maxlen = maxlen;
	m_isView = isView;
	m_isPwd = isPwd;
	m_width = w;
	m_middle = hMiddle;
	self = [super init];

	return self;
}

- (void) myInit
{
	if(m_isView)
	{
		textView = [[UITextView alloc] initWithFrame:CGRectMake(0, 0, m_width, 80)];
		//[textView setBorderStyle:UITextBorderStyleBezel];
		[textView setBackgroundColor:[UIColor whiteColor]];//clearColor]];
		[textView setTextColor:[UIColor blackColor]];
		[textView setFont:[UIFont systemFontOfSize:20]];
		[textView setDelegate:self];
		[textView setTag:(NSInteger)self];
		[textView setContentMode:UIViewContentModeRedraw];
		//[textView setPlaceholder:@"<enter text>"];
		//[textView setBackgroundColor:[UIColor whiteColor]];
		[textView setHidden:YES];
		[textView setText:[[NSString alloc] initWithUTF8String:m_text]];
		textView.keyboardType = UIKeyboardTypeDefault;//UIKeyboardTypeASCIICapable;
		//add it to a UIView
		m_view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 320, 480)];
		//[m_view setBackgroundColor:[UIColor clearColor]];
		[m_view addSubview:textView];
		if(gll_isLandscapeRight)
		{
			[m_view setCenter:CGPointMake(320 - (160 + (80 / 2)),  480 - m_middle)];
			[m_view setBounds:CGRectMake(0, 0, m_width, 80)];
			[m_view setTransform: CGAffineTransformMakeRotation(-3.1415926/2)];
		}
		else
		{
			[m_view setCenter:CGPointMake(160 + (80 / 2),  m_middle)];
			[m_view setBounds:CGRectMake(0, 0, m_width, 80)];
			[m_view setTransform: CGAffineTransformMakeRotation(3.1415926/2)];
		}
		[(UIView*)(g_pGLLiveInstance->m_view) addSubview:m_view];
		[textView becomeFirstResponder];
	}
	else
	{	
		textField = [[UITextField alloc] initWithFrame:CGRectMake(0, 0, 10, 10)];
		[textField setBorderStyle:UITextBorderStyleRoundedRect];
		[textField setTextColor:[UIColor blackColor]];
		[textField setFont:[UIFont systemFontOfSize:20]];
		[textField setDelegate:self];
		[textField setTag:(NSInteger)self];
		if(gll_isLandscapeRight)
		{
			[textField setTransform: CGAffineTransformMakeRotation(-3.1415926/2)];
			[textField setCenter:CGPointMake(320 - (212 - (30 / 2)),  480 - m_middle)];
		}
		else
		{
			[textField setTransform: CGAffineTransformMakeRotation(3.1415926/2)];
			[textField setCenter:CGPointMake(212 - (30 / 2),  m_middle)];
		}
		[textField setBounds:CGRectMake(0, 0, m_width, 30)];
		//[textField setPlaceholder:@"<enter text>"];
		[textField setBackgroundColor:[UIColor clearColor]];
		[textField setHidden:YES];
		//[textField s :@"<enter text>"];
		[textField setText:[[NSString alloc] initWithUTF8String:m_text]];
		textField.keyboardType = UIKeyboardTypeASCIICapable;
		[(UIView*)(g_pGLLiveInstance->m_view) addSubview:textField];
		//SetControl(textField);
		if(m_isPwd)
		{
			textField.secureTextEntry = YES;
		}
		[textField becomeFirstResponder];
	}	
	//ViewController * m_VC = [[ViewController alloc] init];
	
	//	UIImage *buttonImage = [UIImage imageNamed:@"ok.png"];
	//	BackButton = [[UIButton alloc] initWithFrame:CGRectMake(140.0, 20.0, 80.0, 40.0)];
	//	[BackButton setTitle:@"Back" forState:UIControlStateNormal];
	//	[BackButton setTitleColor:[UIColor blackColor] forState:UIControlEventTouchDown];
	//	[BackButton setBackgroundImage:buttonImage forState:UIControlStateNormal];
	//	BackButton.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
	//	BackButton.contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
	//	[BackButton addTarget:self action:@selector(clickBack:) forControlEvents:UIControlEventTouchUpInside];
	//	[BackButton setBackgroundColor:[UIColor clearColor]];
	//	[BackButton setTransform:CGAffineTransformMakeRotation(3.1415926/2)];
	//	OKButton = [[UIButton alloc] initWithFrame:CGRectMake(140.0, 420.0, 80.0, 40.0)];
	//	[OKButton setTitle:@"OK" forState:UIControlStateNormal];
	//	[OKButton setTitleColor:[UIColor blackColor] forState:UIControlEventTouchDown];
	//	[OKButton setBackgroundImage:buttonImage forState:UIControlStateNormal];
	//	OKButton.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
	//	OKButton.contentHorizontalAlignment = UIControlContentHorizontalAlignmentCenter;
	//	[OKButton addTarget:self action:@selector(clickOK:) forControlEvents:UIControlEventTouchUpInside];
	//	[OKButton setBackgroundColor:[UIColor clearColor]];
	//	[OKButton setTransform:CGAffineTransformMakeRotation(3.1415926/2)];
	//then add the button to a UIView like this:
	//	[g_pUIView addSubview: BackButton];
	//	[g_pUIView addSubview: OKButton];
	//SetControl(textView);
}

- (BOOL)textField:(UITextField *)textfield shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string;   // return NO to not change text
{
	//length limit
	int len = strlen([textField.text UTF8String]);
	
	int len2 = strlen([string UTF8String]);
	//if current key is backspace, strlen(str) == 0
	if(len + len2 > m_maxlen && len2 > 0)
		return NO;
	else
		return YES;
}

- (BOOL)textView:(UITextView *)textview shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text;
{
	//length limit
	int len = strlen([textView.text UTF8String]);
	
	int len2 = strlen([text UTF8String]);
	//if current key is backspace, strlen(str) == 0
	if(len + len2 > m_maxlen && len2 > 0)
		return NO;
	else
		return YES;
}

- (BOOL) textFieldShouldReturn:(UITextField *)textfield
{
	[textfield setHidden:YES];
	[textfield resignFirstResponder];
	//	[BackButton removeFromSuperview];
	//	[OKButton removeFromSuperview];
	const char* szTemp = [textfield.text UTF8String];
	memset(m_text, 0, m_maxlen + 1);
	memcpy(m_text, szTemp, m_maxlen);
	//m_text[m_maxlen] = 0;
	NSString* temp = [[NSString alloc] initWithUTF8String:m_text];
	while(temp == nil)
	{
		m_text[strlen(m_text) - 1] = 0;
		[temp release];
		temp = [[NSString alloc] initWithUTF8String:m_text];
	}
	[temp release];
	g_pGLLiveInstance->PopGLLiveState();
	return YES;
}

- (void)textFieldDidBeginEditing:(UITextField *)textfield
{
	[textfield setHidden:NO];
}

- (void)textViewDidBeginEditing:(UITextView *)textview
{
	[textview setHidden:NO];
}

- (void)onRotateDevice
{
	if(m_isView)
	{
		[textView resignFirstResponder];
		//[textView removeFromSuperview];
		if(gll_isLandscapeRight)
		{
			[m_view setCenter:CGPointMake(320 - (160 + (80 / 2)),  480 - m_middle)];
			[m_view setBounds:CGRectMake(0, 0, m_width, 80)];
			[m_view setTransform: CGAffineTransformMakeRotation(-3.1415926/2)];
		}
		else
		{
			[m_view setCenter:CGPointMake(160 + (80 / 2),  m_middle)];
			[m_view setBounds:CGRectMake(0, 0, m_width, 80)];
			[m_view setTransform: CGAffineTransformMakeRotation(3.1415926/2)];
		}
		//[(UIView*)(g_pGLLiveInstance->m_view) addSubview:m_view];
		[textView becomeFirstResponder];
	}
	else
	{
		[textField resignFirstResponder];
		//[textField removeFromSuperview];
		if(gll_isLandscapeRight)
		{
			[textField setTransform: CGAffineTransformMakeRotation(-3.1415926/2)];
			[textField setCenter:CGPointMake(320 - (212 - (30 / 2)),  480 - m_middle)];
		}
		else
		{
			[textField setTransform: CGAffineTransformMakeRotation(3.1415926/2)];
			[textField setCenter:CGPointMake(212 - (30 / 2),  m_middle)];
		}
		//[(UIView*)(g_pGLLiveInstance->m_view) addSubview:textField];
		[textField becomeFirstResponder];
	}
}

- (void)clickBack
{
	if(m_isView)
	{
		[textView setHidden:YES];
		[textView resignFirstResponder];
	}
	else
	{
		[textField setHidden:YES];
		[textField resignFirstResponder];
	}
	//	[BackButton removeFromSuperview];
	//	[OKButton removeFromSuperview];
	//	g_pGLLiveInstance->PopGameState();
}

- (void)clickOK
{
	if(m_isView)
	{
		[textView setHidden:YES];
		[textView resignFirstResponder];
		const char* szTemp = [textView.text UTF8String];
		memset(m_text, 0, m_maxlen + 1);
		memcpy(m_text, szTemp, m_maxlen);
		//m_text[m_maxlen] = 0;
		NSString* temp = [[NSString alloc] initWithUTF8String:m_text];
		while(temp == nil)
		{
			m_text[strlen(m_text) - 1] = 0;
			[temp release];
			temp = [[NSString alloc] initWithUTF8String:m_text];
		}
		[temp release];
	}
	else
	{
		[textField setHidden:YES];
		[textField resignFirstResponder];
		const char* szTemp = [textField.text UTF8String];
		memset(m_text, 0, m_maxlen + 1);
		memcpy(m_text, szTemp, m_maxlen);
		//m_text[m_maxlen] = 0;
		NSString* temp = [[NSString alloc] initWithUTF8String:m_text];
		while(temp == nil)
		{
			m_text[strlen(m_text) - 1] = 0;
			[temp release];
			temp = [[NSString alloc] initWithUTF8String:m_text];
		}
		[temp release];
	}
	//	[BackButton removeFromSuperview];
	//	[OKButton removeFromSuperview];
	
	//	g_pGLLiveInstance->PopGameState();
}

- (void)endInput
{
	if(m_isView)
	{
		[textView setHidden:YES];
		[textView resignFirstResponder];
		[textView removeFromSuperview];
		[textView setDelegate:nil];
		[textView release];
		[m_view removeFromSuperview];
		[m_view release];
	}
	else
	{
		[textField setHidden:YES];
		[textField resignFirstResponder];
		[textField removeFromSuperview];
		[textField setDelegate:nil];
		[textField release];
	}
}

- (void)dealloc
{
	//trace(DEBUG_OBJCMEM, "Releasing ViewController\n");
	[super dealloc];
}

@end
