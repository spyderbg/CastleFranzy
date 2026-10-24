///////////////////////////////////////////////////////////////////////

#include <string.h>

#import "iCDAppDelegate.h"

#import "EAGLView.h"

#import "TextInput.h"

///////////////////////////////////////////////////////////////////////

static TextInput* g_sharedTextInputInstance = nil;

///////////////////////////////////////////////////////////////////////

@implementation TextInput

-(id)initWithTextBuffer:(char*)textBuff WithSize:(unsigned int)buffSize AtX:(int)x Y:(int)y Width:(int)w Height:(int)h PasswordFlag:(bool)pass Orientation:(int)textFieldOrientation
{
	self = [super init];
	
	if (self != nil)
	{
		done = false;
		
		textBuffer = textBuff;
		textBufferSize = buffSize;

		int len = 0;

		for(int i = 0; i < textBufferSize; i++)
		{
			if(textBuffer[i])
				len++;
		}
		
		CGRect rect;
		CGPoint point;

		rect.origin.x =0;
		rect.origin.y = 0;		
		rect.size.width = w;
		rect.size.height = h;
		
		textField = [[UITextField alloc] init];
		textField.bounds = CGRectMake(0, 0, w, h);
		textField.delegate = self;
		textField.borderStyle				=	UITextBorderStyleRoundedRect;
		textField.autocapitalizationType	=	UITextAutocapitalizationTypeNone;
		textField.keyboardType				=	UIKeyboardTypeASCIICapable;
		textField.secureTextEntry			=	pass;
		textField.autocorrectionType		=	UITextAutocorrectionTypeNo;
		
		if( textFieldOrientation == 3 )
		{
		//	int nx = 320 - y - h - (w-h)/2;
		//	int	ny = x + (w-h)/2;
			int nx = 320 - y - h/2;//320 - y;
			int ny = x + w/2;
						
			textField.transform = CGAffineTransformConcat(textField.transform, CGAffineTransformMakeRotation( 3.141592f/2.0f));
			point.x = nx;
			point.y = ny;                                                                                                                                                                         
			textField.center = point;			
		}
		else
		{
//			int nx = y- (w-h)/2 - w/2;	
//			int ny = 480 - (w-h)/2 - x - h - h/2;
			int nx = y + h/2;
			int ny = 480-x - w/2;

			point.x = nx;
			point.y = ny;                                                                                                                                                                         
			textField.center = point;
			
			textField.transform = CGAffineTransformConcat(textField.transform, CGAffineTransformMakeRotation( -3.141592f/2.0f));		
		}

		NSString* placeHolderString = [[NSString alloc] initWithUTF8String: textBuffer];
		textField.text = placeHolderString;
		[placeHolderString release];
		
		// When the user starts typing, show the clear button in the text field.
		textField.clearButtonMode = UITextFieldViewModeWhileEditing;
		
		textField.textColor = [UIColor blackColor];
		textField.userInteractionEnabled = YES;
//		textField.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;		// problem with text
//		textField.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
		textField.clearsOnBeginEditing = NO;
		
		// Register to receive a notification when the movie has finished playing. 		
		[[NSNotificationCenter defaultCenter] addObserver:self 
												 selector:@selector(textFieldDidChange:) 
													 name:UITextFieldTextDidChangeNotification 
												   object:textField];

		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
 
		[[[iCDAppDelegate sharedInstance] glView] addSubview:textField];
	}
	
	return self;
}

-(void)setOrientation:(int)normalOrient AtX:(int)x Y:(int)y Width:(int)w Height:(int)h 
{
	if(normalOrient == 3)
	{		
		int nx = 320 - y - h - (w-h)/2;
		int ny = x + (w-h)/2;
		
		CGAffineTransform t = CGAffineTransformMakeTranslation(nx, ny);
		t = CGAffineTransformRotate(t, 3.141592f/2.0f);
		textField.transform = t;
		[textField setNeedsDisplay];
	}
	
	if(normalOrient == 2)
	{
		int nx = 320 - y - h - (w-h)/2;
		int ny = x + (w-h)/2;
		
		CGAffineTransform t = CGAffineTransformMakeTranslation(nx, ny);
		t = CGAffineTransformRotate(t, -3.141592f/2.0f);
		textField.transform = t;
		[textField setNeedsDisplay];
	}
		
	textField.enabled = YES;
}

-(void) changeTransformToTextFieldWithTransform:(CGAffineTransform)t
{
	textField.transform = t;
	[textField becomeFirstResponder];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {  
	if(interfaceOrientation == UIInterfaceOrientationPortrait)
		NSLog(@"baaaaa");	
	else
		NSLog(@"baaaaa222");	
	return YES;//(interfaceOrientation == UIInterfaceOrientationPortrait); 
} 

-(void)Enable
{
	textField.enabled = YES;
}

-(void)Disable
{
	textField.enabled = NO;
}

+(TextInput*)getSharedInstance
{
	return g_sharedTextInputInstance;
}

+(void)setSharedInstance:(TextInput*)instance
{
	if (g_sharedTextInputInstance != nil && g_sharedTextInputInstance != instance)
		[g_sharedTextInputInstance release];
	
	g_sharedTextInputInstance = instance;
}

- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField
{
	return YES;
}

- (BOOL)isDone
{
	return done && !textField.editing;
}

- (BOOL)textFieldShouldReturn:(UITextField *)theTextField
{
	// When the user presses return, take focus away from the text field so that the keyboard is dismissed.
	if (theTextField == textField)
	{
		[textField resignFirstResponder];
		
        // Updating textBuffer
		const char* tmpStr = [textField.text UTF8String];

		if(tmpStr == nil)
		{
			textBuffer[0] = 0;			
		}
		else
		{
			int len = [textField.text length];

			if(len > (textBufferSize - 1))
			{
				len = (textBufferSize - 1);
			}

			for(int i = 0; i < len; i++)
			{
				textBuffer[i] = [textField.text characterAtIndex:(i)];
			}
			textBuffer[len] = 0;
		}
		
		done = true;
	}
	
	return YES;
}

extern "C" 
bool IsCharacterSupported(unsigned short c)
{
	if (c >= 256) 
		return false;
	
	if (c == ' ')
		return true;
	
	if (c >= '0' && c <= '9')
		return true;
	
    if (c >= 'a' && c <= 'z')
		return true;
	
	if (c >= 'A' && c <= 'Z')
		return true;
	
	if (c == '@' || c == '.' || c == '_' || c == '-' || c == '\'')
		return true;
	
	return false;		
}

//  Notification called when the movie finished playing.
- (void) textFieldDidChange:(NSNotification*)notification
{	
	int len = [textField.text length];
	if (len <= 0)
		return;
	
	bool bTrimText = false;
	
	if (len >= textBufferSize)
	{
		len = textBufferSize - 1;
		bTrimText = true;
	}
	
	int len2 = 0;

	for(int i = 0; i < len; i++)
	{		
		unsigned short c = [textField.text characterAtIndex:(i)];
	
		if (!IsCharacterSupported(c))
		{
			bTrimText = true;
		}
		else
		{
			textBuffer[len2++] = c;
		}
	}

	textBuffer[len2] = 0;
	
	if (bTrimText)
	{
	//	NSString* placeHolderString = [[NSString alloc] initWithCharacters:textBuffer length:len2];
		NSString* placeHolderString = [[NSString alloc] initWithUTF8String:textBuffer];
		textField.text = placeHolderString;
		[placeHolderString release];		
	}
}

-(void)Remove
{
	// remove movie notifications
	[[NSNotificationCenter defaultCenter] removeObserver:self
		name:UITextFieldTextDidChangeNotification
		object:textField];

	[textField setDelegate:nil];
	//[textField retain];
	[textField removeFromSuperview];	
	//[textField release];
}

- (void)dealloc
{
	[textField release];
	
	[super dealloc];
}

@end

///////////////////////////////////////////////////////////////////////

@implementation TextInputInitParams

@end

///////////////////////////////////////////////////////////////////////

#import "WTextInput.h"

WTextInput::WTextInput()
{
	m_textInputInstance = nil;
}

WTextInput::~WTextInput()
{
	[m_textInputInstance release];	
}

void WTextInput::Remove()
{
	if(g_nAppExit)
	{
		[[iCDAppDelegate sharedInstance] performSelectorOnMainThread:@selector(removeTextField:) withObject:m_textInputInstance waitUntilDone:NO];
	}
	else
	{
		RENDER_MUTEX_UNLOCK
				
		[[iCDAppDelegate sharedInstance] performSelectorOnMainThread:@selector(removeTextField:) withObject:m_textInputInstance waitUntilDone:YES];
		
		RENDER_MUTEX_LOCK
	}		
}

void WTextInput::Init( char* textBuf, unsigned int bufSize, int x, int y, int w, int h, bool pass, int Orientation1 )
{
	m_textInputInstance = [TextInput alloc];
	
	TextInputInitParams* p = [[TextInputInitParams alloc] init];
	
	p->textBuff = textBuf;
	p->buffSize = bufSize;
	p->x = x;
	p->y = y;
	p->w = w;
	p->h = h;
	p->pass = pass;
	p->orientationTf = Orientation1;
	p->ti = m_textInputInstance;
	
	if(g_nAppExit)
	{
		[[iCDAppDelegate sharedInstance] performSelectorOnMainThread:@selector(initTextInput:) withObject:p waitUntilDone:NO];
	}
	else
	{
		RENDER_MUTEX_UNLOCK
		
		[[iCDAppDelegate sharedInstance] performSelectorOnMainThread:@selector(initTextInput:) withObject:p waitUntilDone:YES];
		
		RENDER_MUTEX_LOCK
	}
	
	[p release];
}

void WTextInput::mySetOrientation(int orientationNormal,int x, int y, int w, int h)
{
		[m_textInputInstance setOrientation:orientationNormal AtX:x Y:y Width:w Height:h];
	
}

void WTextInput::Enable()
{
	[m_textInputInstance Enable];
}

void WTextInput::Disable()
{
	[m_textInputInstance Disable];
}

///////////////////////////////////////////////////////////////////////

/*extern "C" void GetUserCountry(char* buf)
{
	if(!buf)
		return;

	CFLocaleRef userLocaleRef = CFLocaleCopyCurrent();
	CFLocaleRef systemLocaleRef = CFLocaleGetSystem();

	CFStringRef c1 = (CFStringRef)CFLocaleGetValue(userLocaleRef, kCFLocaleCountryCode);

	if(c1 != nil)
	{
		const char* cc1 = [c1 UTF8String];

		//const void* c2 = CFLocaleGetValue(systemLocaleRef, kCFLocaleCountryCode);
		//const char* cc2 = (const char*)c2;	

		strcpy(buf, cc1);
	}
	else
	{
		buf[0] = 0;
	}
}*/


