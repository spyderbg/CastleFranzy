//
//  GameInformationViewController.mm
//  IGP
//
//  Created by administrador on 22/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "GameInformationViewController.h"
#import "IGP.h"
#import "IGPcStr.h"

static int scrolliterations = 0;
@implementation GameInformationViewController

- (id)initWithBackgroundImage:(UIImage*) bgImage //Parent:(UIViewController*)parent
{
	if (self = [super init]) 
	{
		
		// create and configure the view
		CGRect cgRct = CGRectMake(0.0, 0.0, 320, 440); 
		self.view = [[UIView alloc] initWithFrame:cgRct];
		self.view.autoresizesSubviews = YES;
		m_backgroundImageView = [[UIImageView alloc] initWithImage: bgImage];
		[self.view addSubview:m_backgroundImageView];
		
		//Add Top images
		//UIImageView *glimage = [[UIImageView alloc] initWithImage:IGP_IMAGE(m_gameloftImage)];
		//[glimage setFrame:CGRectMake(77, 4, glimage.image.size.width, glimage.image.size.height)];
		//[self.view addSubview:glimage];
		
		m_backButton = [[UIButton alloc] initWithFrame:CGRectMake(2, 5, 70, 36)];
		[m_backButton setImage: IGP_IMAGE(m_backButtonImage) forState:UIControlStateNormal];
		[m_backButton addTarget:self action:@selector(popToPreviousView:)  forControlEvents:UIControlEventTouchUpInside];
		
		
		[self.view addSubview:m_backButton];
		[self showTitleOneLine];		//chris++
		
		imgIcon = [[UIImageView alloc] initWithFrame:CGRectMake(10, 10, 63, 63)];
		m_descriptionTextView = [[UITextView alloc] initWithFrame:CGRectMake(0, 102, 320, 400)];
		scrollView = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 45, self.view.frame.size.width, self.view.frame.size.height - 45)];
		m_titleTextView = [[UITextView alloc] initWithFrame:CGRectMake(75, 0, 200, 48)];
		screenshotView = [[UIImageView alloc] init];
		//Add Scroll
		

		
		CGRect reflectionRect = imgIcon.frame;
		reflectionRect.size.height = reflectionRect.size.height * K_DEFAULT_REFLECTION_FRACTION;
		reflectionRect = CGRectOffset(reflectionRect, 0, imgIcon.frame.size.height);
		imgIconReflection = [[UIImageView alloc] initWithFrame:reflectionRect];
		imgIconReflection.alpha = K_DEFAULT_REFLECTION_OPACITY;
		
		[self setDefaultPropertiesText:m_titleTextView Size:14 Bold:true];
		[self setDefaultPropertiesText:m_descriptionTextView Size:12 Bold:false];
		
		[self.view addSubview:scrollView];
		[scrollView addSubview:imgIcon];
		[scrollView addSubview:imgIconReflection];
		[scrollView addSubview:m_titleTextView];
		[scrollView addSubview:m_descriptionTextView];
		[scrollView addSubview:screenshotView];
		[scrollView setDelegate:self];

		m_buyButton = [[UIButton alloc] initWithFrame:BUYBUTTON_RECT2];
		[m_buyButton setImage:IGP_IMAGE(m_buyButtonImage) forState:UIControlStateNormal];
		[m_buyButton addTarget:self action:@selector(openBuyLink:) forControlEvents:UIControlEventTouchUpInside];
		
		m_videoButton = [[UIButton alloc] initWithFrame:VIDEOBUTTON_RECT2];
		[m_videoButton setImage:IGP_IMAGE(m_videoButtonImage) forState:UIControlStateNormal];
		[m_videoButton addTarget:self action:@selector(openVideoLink:) forControlEvents:UIControlEventTouchUpInside];
		

		m_buyLabel = [[UILabel alloc] initWithFrame:CGRectMake(m_buyButton.frame.origin.x + 33,m_buyButton.frame.origin.y+  (m_buyButton.frame.size.height/2) -29, m_buyButton.frame.size.width - 36, 60)];
	
		
		[m_buyLabel setTextColor:[UIColor whiteColor]];
		[m_buyLabel setBackgroundColor:[UIColor clearColor]];

		[m_buyLabel setAdjustsFontSizeToFitWidth: YES];
		m_buyLabel.textAlignment = UITextAlignmentCenter;
					   

		[m_buyButton   addSubview:m_buyLabel];
		[scrollView addSubview:m_buyButton];
   		[scrollView addSubview:m_buyLabel];
		[scrollView addSubview:	m_videoButton];
	}
	return self;
}

- (IBAction) popToPreviousView:(id) sender
{
	[m_gotHereFrom popViewControllerAnimated:YES];
	[m_gotHereFrom refreshMe];
}

- (void)setDefaultPropertiesText:(UITextView*)text Size:(int)size Bold:(bool)bold
{
	text.editable = NO;
	text.scrollEnabled = NO;
	[text setUserInteractionEnabled:NO];
	[text setBackgroundColor:[UIColor colorWithWhite:0 alpha:0]];
	[text setTextColor:[UIColor whiteColor]];
	if (bold)
	{
		[text setFont:[UIFont fontWithName:@"Arial-BoldMT" size:size]];
	}
	else
	{
		[text setFont:[UIFont fontWithName:@"Arial" size:size]];
	}
}

- (void)setParent: (UIViewController*)parent
{
	_parent = parent;
}

- (void) setGame:(IGPGame*) game from:(UINavigationController*) from isNewPrice:(bool) paramNewPrice
{
	CGPoint p;
	p.x = 0;
	p.y = 0;
	NSUInteger reflectionHeight = imgIcon.bounds.size.height * K_DEFAULT_REFLECTION_FRACTION;
	[scrollView setContentOffset:p animated:NO];
	m_gotHereFrom = from;
	m_game = game;
	
	m_paramNewPrice = paramNewPrice;
	m_titleTextView.text			= m_game->GetName();
	if(m_game->GetPrice())
		[m_buyLabel setText:m_game->GetPrice()];
	else
		[m_buyLabel setText:IGPcStr::GetNSString(TXT_BUY)];
	
	m_descriptionTextView.text	= m_game->GetDescription();
	imgIcon.image			= m_game->GetIcon();
	screenshotView.image	= m_game->GetScreenshot(0);
	//screenshotView.frame	= CGRectMake(, 0, 0, 0);
	imgIconReflection.image = [self reflectedImage:imgIcon withHeight:reflectionHeight];

	CGSize sz = [m_descriptionTextView.text sizeWithFont: m_descriptionTextView.font constrainedToSize:CGSizeMake(310,3000)  lineBreakMode:UILineBreakModeWordWrap];//pup m wrong constraint fized

	NSLog(@"Text Size = %f:%f:", sz.width, sz.height);
	
	[m_descriptionTextView setFrame:CGRectMake(0, 102, 320, sz.height + 40)];
	[screenshotView setFrame:CGRectMake(160 - screenshotView.image.size.width /2, m_descriptionTextView.frame.origin.y + m_descriptionTextView.frame.size.height+25, screenshotView.image.size.width, screenshotView.image.size.height)];
	[m_descriptionTextView setClipsToBounds:NO];
	scrollView.contentOffset.y = 0;
	scrollView.contentSize = CGSizeMake(self.view.frame.size.width, screenshotView.frame.origin.y + screenshotView.frame.size.height + 10);
	
	if(m_paramNewPrice)
		[m_buyLabel setTextColor:[UIColor yellowColor]];
	else
		[m_buyLabel setTextColor:[UIColor whiteColor]];
			
	[m_titleTextView setNeedsDisplay];
	[m_descriptionTextView setNeedsDisplay];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void) setFrom:(char*) from
{
	sprintf(m_currentFrom, from);
}

- (IBAction) openBuyLink:(id) sender
{
	if ( !IGP::s_igpInstance->_cheatEnabled)   //pup a 	2180565 (don't show loading if is in cheat mode)
		{
			m_activity = [[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
			[m_activity setFrame : CGRectMake(K_SCREEN_W / 2 - 16, K_SCREEN_H /2 - 16	, 32,32)];
			[m_activity startAnimating];
		}	
	[self.view addSubview:m_activity];
	m_game->OpenBuyLink(m_currentFrom);
}

- (IBAction) openVideoLink:(id) sender
{
	m_activity = [[UIActivityIndicatorView alloc]initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
	if ( !IGP::s_igpInstance->_cheatEnabled)   //pup a 	2180565 (don't show loading if is in cheat mode)
	{
		[m_activity setFrame : CGRectMake(K_SCREEN_W / 2 - 16, K_SCREEN_H /2 - 16	, 32,32)];
		[m_activity startAnimating];
		[self.view addSubview:m_activity];
	}
	m_game->OpenVideoLink(m_currentFrom);
}

- (void)didReceiveMemoryWarning {
	trace(DEBUG_OBJCMEM, "GameInformationViewController threw memory warning!\n");
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}

-(void) showTitleOneLine
{
	
	
	m_titleLabel[0] = nil;
	NSString *labelTxt= IGPcStr::GetNSString(TXT_GAME_INFORMATION);	
	m_titleLabel[0] = [[UILabel alloc] initWithFrame:CGRectMake(70,12,240,25)];
	[m_titleLabel[0] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
	[m_titleLabel[0] setText:labelTxt];
	m_titleLabel[0].textColor=[UIColor colorWithRed:242/255.0 green:62/255.0 blue:32/255.0 alpha:1];
	[m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:22]];
	if ([m_titleLabel[0].text length]>18)
	{
		[m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:20]];	
	}
	m_titleLabel[0].textAlignment= UITextAlignmentRight;
	[self.view addSubview:m_titleLabel[0]];	
	
}
-(void) showTitle
{
		
	NSString *labelTxt= IGPcStr::GetNSString(TXT_GAME_INFORMATION);
	NSArray *labelAux=[labelTxt componentsSeparatedByString:@" "];
	int posxStartLabel=120;
	NSString *labelUP=NULL;
	NSString *labelDown=NULL;
	if ([labelAux count]>1)
	{
		labelUP= [labelAux objectAtIndex:0];
		m_titleLabel[0] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel+20,0,160,21)];
		[m_titleLabel[0] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
		[m_titleLabel[0] setText:labelUP];
		[m_titleLabel[0] setTextColor: [UIColor orangeColor]];
		[m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:19]];
		m_titleLabel[0].textAlignment= UITextAlignmentRight;
		[self.view addSubview:m_titleLabel[0]];	
	}

	if ([labelAux count]==1)
	{
		m_titleLabel[1] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel,10,180,21)];
		labelDown= [labelAux objectAtIndex:0];
		[m_titleLabel[1] setFont:[UIFont boldSystemFontOfSize:22]];
	}
	else if([labelAux count]>=2)//concatenate string [2] " " and [3] and ....
	{
		m_titleLabel[1] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel,19,180,21)];
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
	[self.view addSubview:m_titleLabel[1]];
}

- (void) removeViews
{
	[m_buyButton setImage:nil forState:UIControlStateNormal];
	[m_videoButton setImage:nil forState:UIControlStateNormal];
	
	[m_backgroundImageView removeFromSuperview];
	[m_backButton removeFromSuperview];
	[m_titleTextView removeFromSuperview];
	[scrollView removeFromSuperview];
	[m_descriptionTextView removeFromSuperview];
	[m_buyButton removeFromSuperview];
	[imgIcon removeFromSuperview];
	[screenshotView removeFromSuperview];
	[imgIconReflection removeFromSuperview];
	[m_buyLabel removeFromSuperview];
	[m_titleLabel[0] removeFromSuperview];
	[m_titleLabel[1] removeFromSuperview];
	[scrollView setDelegate:nil];
	
}

- (void)dealloc 
{
	trace(DEBUG_OBJCMEM, "Releasing GameInformationViewController\n");
	SAFE_DELETE_OBJC(m_backgroundImageView);
	SAFE_DELETE_OBJC(m_backButton);
	SAFE_DELETE_OBJC(m_titleTextView) ;
	SAFE_DELETE_OBJC(scrollView);
	SAFE_DELETE_OBJC(m_descriptionTextView);
	SAFE_DELETE_OBJC(m_buyButton);
	SAFE_DELETE_OBJC(imgIcon);
	SAFE_DELETE_OBJC(screenshotView);
	SAFE_DELETE_OBJC(imgIconReflection);
	SAFE_DELETE_OBJC(m_buyLabel);
	SAFE_DELETE_OBJC(m_titleLabel[0]);
	SAFE_DELETE_OBJC(m_titleLabel[1]);
	SAFE_DELETE_OBJC(m_videoButton);
	SAFE_DELETE_OBJC(m_activity);
	[self.view release];
	
	[super dealloc];
   
}

//-----------------------------------------------------------------
///Reflection
//-----------------------------------------------------------------

CGImageRef CreateGradientImage(int pixelsWide, int pixelsHigh)
{
    CGImageRef theCGImage = NULL;
	
    // gradient is always black-white and the mask must be in the gray colorspace
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
    
    // create the bitmap context
    CGContextRef gradientBitmapContext = CGBitmapContextCreate(nil, pixelsWide, pixelsHigh,
                                                               8, 0, colorSpace, kCGImageAlphaNone);
    
    // define the start and end grayscale values (with the alpha, even though
    // our bitmap context doesn't support alpha the gradient requires it)
    CGFloat colors[] = {0.0, 1.0, 1.0, 1.0};
    
    // create the CGGradient and then release the gray color space
    CGGradientRef grayScaleGradient = CGGradientCreateWithColorComponents(colorSpace, colors, NULL, 2);
    CGColorSpaceRelease(colorSpace);
    
    // create the start and end points for the gradient vector (straight down)
    CGPoint gradientStartPoint = CGPointZero;
    CGPoint gradientEndPoint = CGPointMake(0, pixelsHigh);
    
    // draw the gradient into the gray bitmap context
    CGContextDrawLinearGradient(gradientBitmapContext, grayScaleGradient, gradientStartPoint,
                                gradientEndPoint, kCGGradientDrawsAfterEndLocation);
    
    // convert the context into a CGImageRef and release the context
    theCGImage = CGBitmapContextCreateImage(gradientBitmapContext);
    CGContextRelease(gradientBitmapContext);
    
    // return the imageref containing the gradient
    return theCGImage;
}

CGContextRef MyCreateBitmapContext(int pixelsWide, int pixelsHigh)
{
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    // create the bitmap context
    CGContextRef bitmapContext = CGBitmapContextCreate (nil, pixelsWide, pixelsHigh, 8,
                                                        0, colorSpace,
                                                        // this will give us an optimal BGRA format for the device:
                                                        (kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst));
    CGColorSpaceRelease(colorSpace);
	
    return bitmapContext;
}

- (UIImage *)reflectedImage:(UIImageView *)fromImage withHeight:(NSUInteger)height
{
    // create a bitmap graphics context the size of the image
    CGContextRef mainViewContentContext = MyCreateBitmapContext(fromImage.bounds.size.width, height);
    
    // offset the context -
    // This is necessary because, by default, the layer created by a view for caching its content is flipped.
    // But when you actually access the layer content and have it rendered it is inverted.  Since we're only creating
    // a context the size of our reflection view (a fraction of the size of the main view) we have to translate the
    // context the delta in size, and render it.
    //
    CGFloat translateVertical= fromImage.bounds.size.height - height;
    CGContextTranslateCTM(mainViewContentContext, 0, -translateVertical);
    
    // render the layer into the bitmap context
    CALayer *layer = fromImage.layer;
    [layer renderInContext:mainViewContentContext];
    
    // create CGImageRef of the main view bitmap content, and then release that bitmap context
    CGImageRef mainViewContentBitmapContext = CGBitmapContextCreateImage(mainViewContentContext);
    CGContextRelease(mainViewContentContext);
    
    // create a 2 bit CGImage containing a gradient that will be used for masking the 
    // main view content to create the 'fade' of the reflection.  The CGImageCreateWithMask
    // function will stretch the bitmap image as required, so we can create a 1 pixel wide gradient
    CGImageRef gradientMaskImage = CreateGradientImage(1, height);
    
    // create an image by masking the bitmap of the mainView content with the gradient view
    // then release the  pre-masked content bitmap and the gradient bitmap
    CGImageRef reflectionImage = CGImageCreateWithMask(mainViewContentBitmapContext, gradientMaskImage);
    CGImageRelease(mainViewContentBitmapContext);
    CGImageRelease(gradientMaskImage);
    
    // convert the finished reflection image to a UIImage 
    UIImage *theImage = [UIImage imageWithCGImage:reflectionImage];
    
    // image is retained by the property setting above, so we can release the original
    CGImageRelease(reflectionImage);
    
    return theImage;
}

- (void)scrollViewDidScroll:(UIScrollView *)currentScrollView
{
	//int a = 0;
	scrolliterations++;
	if(scrolliterations % 3 == 0)
		m_descriptionTextView.text	= m_game->GetDescription();
	//[textDescription setNeedsDisplay];
}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate
{
	//int a = 0;
	//[textDescription setNeedsDisplay];
}

- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView;
{
	[m_descriptionTextView setNeedsDisplay];
}

/*- (void)scrollViewDidEndScrollingAnimation:(UIScrollView *)scrollView
{
	//[m_descriptionTextView setNeedsDisplay];
}


- (void)scrollViewWillBeginDecelerating:(UIScrollView *)scrollView   // called on finger up as we are moving
{
	//[m_descriptionTextView setNeedsDisplay];
}
- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
	//[m_descriptionTextView setNeedsDisplay];
}

- (BOOL)scrollViewShouldScrollToTop:(UIScrollView *)scrollView
{
	//textDescription.text	= m_game->GetDescription();
	//[textDescription setNeedsDisplay];
}
 */

//- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView;  
- (void)viewWillDisappear:(BOOL)animated
{
	if ([m_activity isAnimating])
		[m_activity stopAnimating];
}

-(void ) stopLoadingAnimation
{
	if(m_activity)
		[m_activity stopAnimating];
}


@end
