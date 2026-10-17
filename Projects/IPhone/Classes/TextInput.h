///////////////////////////////////////////////////////////////////////

#undef WIN32
#import <UIKit/UIKit.h>

///////////////////////////////////////////////////////////////////////

@interface TextInput : UIViewController <UITextFieldDelegate>
{
	UITextField *textField;
	Boolean done;
	char* textBuffer;
	unsigned int textBufferSize;
}

//@property (nonatomic, retain) UITextField *textField;

-(id)initWithTextBuffer:(char*)textBuff WithSize:(unsigned int)buffSize AtX:(int)x Y:(int)y Width:(int)w Height:(int)h PasswordFlag:(bool)pass Orientation:(int)textFieldOrientation;
-(BOOL)isDone;

-(void)Enable;
-(void)Disable;

-(void)Remove;

+(TextInput*)getSharedInstance;
+(void)setSharedInstance:(TextInput*)instance;

@end


@interface TextInputInitParams : NSObject
{
@public
	char* textBuff;
	unsigned int buffSize;
	int x;
	int y;
	int w;
	int h;
	bool pass;
	int orientationTf;

	TextInput* ti;
}

@end
