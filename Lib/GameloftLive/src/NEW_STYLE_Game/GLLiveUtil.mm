//
//  GLLiveUtil.mm
//
//  Created by liwei2 on 05/06/09.
//  Copyright 2009 Gameloft. All rights reserved.
//
#include "GLLiveGame.h"
#import <sys/time.h>
#import "GLLiveUtil.h"
#include <sys/sysctl.h>

@implementation GLLiveUtil 

#pragma mark -
#pragma mark === util ===
#pragma mark -

char	g_gllResourcePath[512] = {0};
char	g_gllSavePath[512] = {0};

void InitMacPathForGLL()
{
	//resource path
	NSString *appDirectory = [[NSBundle mainBundle] resourcePath];
	const char* appPath = [appDirectory UTF8String];
	strcpy(g_gllResourcePath, appPath);
	int len = strlen(g_gllResourcePath);
	g_gllResourcePath[len] = '/';
	g_gllResourcePath[len + 1] = 0;
	
	//save file path
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	const char* savePath = [documentsDirectory UTF8String];
	strcpy(g_gllSavePath, savePath);
	len = strlen(g_gllSavePath);
	g_gllSavePath[len] = '/';
	g_gllSavePath[len + 1] = 0;
}

void GLLGetSaveFilePath(char* filePath, const char* filename)
{
	//for GLLiveCheckFW
	if(g_gllSavePath[0] == 0)
		InitMacPathForGLL();
	sprintf(filePath, "%s%s", g_gllSavePath, filename);
}

void GLLGetConfigFilePath(char* filePath, const char* filename)
{
	sprintf(filePath, "%s%s", g_gllResourcePath, filename);
}

void GLLGetUserCountry(char* buf)
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
}

bool isFW3()
{
	//iPhone1,1-->iPhone 2G
	//iPhone1,2-->iPhone 3G
	//iPhone2,1-->iPhone 3Gs
	//iPod1,1-->iPod touch 1G
	//iPod2,1-->iPod touch 2G
/*	size_t size;
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	char *machine = (char*)malloc(size);
	sysctlbyname("hw.machine", machine, &size, NULL, 0);
	//NSString* platform = [NSString stringWithCString: machine encoding: NSUTF8StringEncoding];
	free(machine);*/
	
	NSString * firmware = [[UIDevice currentDevice] systemVersion];
	NSString * major = [firmware substringToIndex:1];
	int sdkMajorVal = [major intValue];
	
	if ( sdkMajorVal >= 3 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GLLGetUDID(char* udid)
{
	NSString* UDID = [[UIDevice currentDevice] uniqueIdentifier];
	strcpy(udid, [UDID UTF8String]);
}

NSString* PushWarning[2][6] = {
{@"OK",@"OK",@"OK",@"OK",@"Ok",@"Acep."},
{@"To receive notifications for game invitations, you need to upgrade your iPhone's firmware to version 3.0 or higher.",
@"To receive notifications for game invitations, you need to upgrade your iPhone's firmware to version 3.0 or higher.",
@"Um Benachrichtigungen über Spieleinladungen zu erhalten, musst du die Firmware deines iPhones auf Version 3.0 oder höher upgraden.",
@"Pour recevoir des notifications concernant les invitations à jouer, vous devez mettre à jour votre iPhone en installant la version 3.0 ou une version plus récente.",
@"Per ricevere notifiche di inviti a giocare, devi aggiornare il firmware del tuo iPhone alla versione 3.0 o successiva.",
@"Para recibir notificaciones de invitaciones de juego, necesitas actualizar el sistema operativo de tu iPhone a la versión 3.0 u otra más reciente.",
},
};

void GLLiveCheckFW(int lang)
{
	if ( !isFW3() )
	{
		char filename[FILENAME_MAX];
		GLLGetSaveFilePath(filename, "checkFW.sav");
		FILE * file = fopen(filename, "r");
		if( !file )
		{
			file = fopen(filename, "w+");
			if ( file )
				fclose(file);
		}
		else
		{
			fclose(file);
			return;
		}
		UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"UNO" message:PushWarning[1][lang] delegate:nil cancelButtonTitle:PushWarning[0][lang] otherButtonTitles:nil];
		[alert show];
		[alert release];
	}
}

uint32 GLLGetMSTime()
{
	timeval t;
	gettimeofday(&t, NULL);
	return (t.tv_sec & 0xffff) * 1000 + t.tv_usec / 1000;	
}

int GenTextureFromPNGBuf(char* buf, int buf_size, int &width, int &height)
{
	NSData* data = [[NSData alloc]initWithBytes:buf length:buf_size];
	UIImage* img = [UIImage imageWithData:data];
	[data release];
	int canvasSize = 64;
	CGImageRef spriteImage = img.CGImage;
	
	while (canvasSize < img.size.width or canvasSize < img.size.height)
	{
		canvasSize = canvasSize * 2;
	}
	width = canvasSize;
	height = canvasSize;
	
	GLuint spriteTexture = 0;
	if(spriteImage) {
		GLubyte* spriteData = (GLubyte *) malloc(canvasSize * canvasSize * 4);
		CGContextRef spriteContext = CGBitmapContextCreate(spriteData, canvasSize, canvasSize, 8, canvasSize * 4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
		CGContextDrawImage(spriteContext, CGRectMake(0, 0, img.size.width, img.size.height), spriteImage);
		CGContextRelease(spriteContext);
		glGenTextures(1, &spriteTexture);
		glBindTexture(GL_TEXTURE_2D, spriteTexture);
		
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvasSize, canvasSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, spriteData);
		
		SAFE_DELETE_ARRAY(spriteData);
	}
	return spriteTexture;
}

UIInterfaceOrientation gll_originalOrientation; 
void setStatusBarOrientation()
{
	gll_originalOrientation = [UIApplication sharedApplication].statusBarOrientation;
	[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeRight];
	
	//notify current device
	[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
}

void resetStatusBarOrientation()
{
	[[UIApplication sharedApplication] setStatusBarOrientation: gll_originalOrientation];
}

void gllSetOrientation(int type)
{
	if(type == 0)//landscapeLeft
	{
		gll_isLandscapeRight = false;
		[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeRight];
	}
	else
	{
		gll_isLandscapeRight = true;
		[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeLeft];
	}
}

bool gll_isLandscapeRight = false;
UIDeviceOrientation gll_prevDevOrientation;
void updateDeviceOrientation()
{
	UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
	if(orientation == UIDeviceOrientationLandscapeLeft && gll_prevDevOrientation != UIDeviceOrientationLandscapeLeft)
	{
		gllSetOrientation(0);//landscapeLeft
	}
	else if(orientation == UIDeviceOrientationLandscapeRight && gll_prevDevOrientation != UIDeviceOrientationLandscapeRight)
	{
		gllSetOrientation(1);//landscapeRight
	}
	
	gll_prevDevOrientation = orientation;
}

UIActivityIndicatorView * GLL_activity = nil;
void DisplayIndicator(void* window, bool flag)
{
	if ( GLL_activity == nil )
	{
		GLL_activity = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
	}
	
	UIWindow* _window = (UIWindow*)window;
	
	if ( flag )
	{
		_window.userInteractionEnabled = NO;
		[GLL_activity setFrame:CGRectMake(140, 220, 40, 40)];
		[GLL_activity setAlpha:1.0];
		[_window addSubview:GLL_activity];
		[GLL_activity performSelectorOnMainThread:@selector(startAnimating) withObject:nil waitUntilDone:YES];
	}
	else
	{
		_window.userInteractionEnabled = YES;
		[GLL_activity performSelectorOnMainThread:@selector(removeFromSuperview) withObject:nil waitUntilDone:YES];
	}
}

extern char gll_country[3];
void checkLocal()
{
	gll_country[0] = 'U';
	gll_country[1] = 'S';
	gll_country[2] = '\0';
	
	NSLocale*  currLocale = [NSLocale currentLocale];
	if(currLocale != nil)
	{
		//NSString* lang = [currLocale objectForKey:NSLocaleLanguageCode];
		NSString* country = [currLocale objectForKey:NSLocaleCountryCode];
		if(country != nil)
		{
			const char * k_country = [country UTF8String];
			gll_country[0] = k_country[0];
			gll_country[1] = k_country[1];
		}
	}
}

void changeIconBadgeNumber(int msgNumber)
{
	//we don't use it from HQ
	//((UIApplication*)(g_pGLLiveInstance->m_app)).applicationIconBadgeNumber = msgNumber;
}

void GLLGoToLink(const char* link)
{
	NSString* realLink = [[NSString alloc] initWithUTF8String:link];
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:realLink]];
}

#pragma mark -
#pragma mark === Photo library ===
#pragma mark -

UIImagePickerController * g_pImagePickerController = NULL;

unsigned char ** g_ppPicBuf = NULL;
int * g_piPicLength = 0;
void (* g_EndCallBack)();

void ShowPhotoLibrary(unsigned char ** ppPicBuf, int *piPicLength, void (* pCallBack)())
{
	g_ppPicBuf = ppPicBuf;
	g_piPicLength = piPicLength;
	g_EndCallBack = pCallBack;
	
	[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationPortrait];
	[GLLiveUtil startPhotoLibraryPiker:(UIWindow*)(g_pGLLiveInstance->m_window)];
}

void EndPhotoLibrary(UIImage * pImage)
{
	//if(g_screenAngle == 90)
	[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeRight];
	//	[[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeLeft];
	if( isFW3() )
	{
		[g_pImagePickerController.view removeFromSuperview];
		[g_pImagePickerController setDelegate: nil];
		[g_pImagePickerController release];
		g_pImagePickerController = NULL;
	}
	if(pImage == NULL)
		return;
	UIImage * pUIImage = [GLLiveUtil ResizeImage:pImage iWidth:64 iHeight:64];
	
	[GLLiveUtil UIImageToPNGBuffer:pUIImage ppBuf:g_ppPicBuf iLenght:g_piPicLength]; 
	
	//[GLLiveUtil UIImageToGameImageBuffer:pUIImage ppGameImageBuf:g_ppGameImageBuf piGameImageLenght:g_piGameLength];
	////////////[pUIImage release];
	g_EndCallBack();
}

+ (BOOL)startPhotoLibraryPiker:(UIWindow *) window
{
	if((![UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypePhotoLibrary])
	   || (window == nil))
		return NO;
	if(g_pImagePickerController == NULL)
	{
		g_pImagePickerController = [[UIImagePickerController alloc] init];
		g_pImagePickerController.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
		g_pImagePickerController.delegate = self;
		g_pImagePickerController.allowsImageEditing = YES;
		[window addSubview:g_pImagePickerController.view];
	}
	
	//Picker is displayed asynchronously.
	((UIView*)(g_pGLLiveInstance->m_view)).hidden = YES;
	g_pImagePickerController.view.hidden = NO;
	return YES;
}

+ (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingImage:(UIImage *)image editingInfo:(NSDictionary *)editingInfo
{
	// Dismiss the image selection, hide the picker and show the image view with the picked image
	[picker dismissModalViewControllerAnimated:YES];
	g_pImagePickerController.view.hidden = YES;
	((UIView*)(g_pGLLiveInstance->m_view)).hidden = NO;
	EndPhotoLibrary(image);
}

+ (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
	// Dismiss the image selection and close the program
	[picker dismissModalViewControllerAnimated:YES];
	g_pImagePickerController.view.hidden = YES;
	((UIView*)(g_pGLLiveInstance->m_view)).hidden = NO;
	EndPhotoLibrary(NULL);
}

+ (UIImage *)ResizeImage:(UIImage *)image iWidth:(int)iWidth iHeight:(int)iHeight
{
	//new image size 64
	CGImageRef ref = image.CGImage;
	CGRect bounds = CGRectMake(0, 0, iWidth, iHeight);
	
	UIGraphicsBeginImageContext(bounds.size);
	CGContextRef context = UIGraphicsGetCurrentContext();
	// turn right and mirrored
	//CGAffineTransform transform = CGAffineTransformIdentity;
	//transform = CGAffineTransformMakeScale(-1.0, 1.0);
	//transform = CGAffineTransformRotate(transform, M_PI_2);
	//CGContextConcatCTM(context, transform);
	CGContextScaleCTM(context, 1.0, -1.0);
	CGContextTranslateCTM(context, 0, -iHeight);
	
	CGContextDrawImage(context, bounds, ref);
	UIImage * imgCopy = UIGraphicsGetImageFromCurrentImageContext();
	UIGraphicsEndImageContext();
	
	
	CGImageRef refCopy = imgCopy.CGImage;
	int iWidthC = CGImageGetWidth(refCopy);
	int iHeightC = CGImageGetHeight(refCopy);
	printf("return copy image size = %d x %d\n", iWidthC, iHeightC);
	//end get new image
	return imgCopy;
}

//need delete pBuf
+ (void)UIImageToJPEGBuffer:(UIImage *)image ppBuf:(unsigned char **)ppBuf iLenght:(int *)iLenght
{ 
	NSData * imageData = UIImageJPEGRepresentation(image, 0.8);
	*iLenght = imageData.length;
	SAFE_DELETE(*ppBuf);
	*ppBuf = new unsigned char [*iLenght];
	memcpy(*ppBuf, [imageData bytes], *iLenght);
	//[imageData release];
}

//need delete pBuf
+ (void)UIImageToPNGBuffer:(UIImage *)image ppBuf:(unsigned char **)ppBuf iLenght:(int *)iLenght
{ 
	NSData * imageData = UIImagePNGRepresentation(image);
	*iLenght = imageData.length;
	*ppBuf = new unsigned char [*iLenght];
	memcpy(*ppBuf, [imageData bytes], *iLenght);
	//[imageData release];
}

@end

