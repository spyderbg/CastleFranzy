//
//  CatalogViewController.mm
//  IGP
//
//  Created by administrador on 22/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "CatalogViewController.h"
#import "CatalogIconsViewController.h"
#import "IGP.h"
#import "EN.h"

@implementation CatalogIconsViewController

@synthesize m_scrollView;
@synthesize m_pageControl;

- (id)initWithBackgroundImage:(UIImage*) bgImage //Parent:(UIViewController*)parent
{
	if (self = [super init]) 
	{
		//_parent = parent;
		self.title = IGPcStr::GetNSString(TXT_CATALOG);
		CGRect cgRct = CGRectMake(0.0, 40.0, 320, 370); //!AC_: Check rect for NxM icons
		self.view = [[UIView alloc] initWithFrame:cgRct];
		self.view.autoresizesSubviews = YES;
		[self.view setOpaque:YES];
		m_backgroundImageView = [[UIImageView alloc] initWithImage: bgImage];
		[self.view addSubview:m_backgroundImageView];
		
		//Gameloft logo
		//m_glLogoImage = [[UIImageView alloc] initWithImage:IGP_IMAGE(m_gameloftImage)];
		//[m_glLogoImage setFrame:CGRectMake(77, 4, m_glLogoImage.image.size.width, m_glLogoImage.image.size.height)];
		//[self.view addSubview:m_glLogoImage];	
	
		[self showTitleOneLine];

		//ScrollView
		m_scrollView = [[UIScrollView alloc] initWithFrame:cgRct];
		m_scrollView.pagingEnabled = YES;
		m_scrollView.showsHorizontalScrollIndicator = NO;
		m_scrollView.showsVerticalScrollIndicator = NO;
		m_scrollView.scrollsToTop = NO;
		m_scrollView.delegate = self;
		
		m_pageControl = [[UIPageControl alloc] initWithFrame:CGRectMake(0, m_scrollView.frame.origin.y + m_scrollView.frame.size.height,  m_scrollView.frame.size.width, 18)];
		[m_pageControl addTarget:self action:@selector(changePage:) forControlEvents:UIControlEventValueChanged];
		
		m_backButton = [[UIButton alloc] initWithFrame:CGRectMake(2, 5, 70, 36)];
		[m_backButton setImage: IGP_IMAGE(m_backButtonImage) forState:UIControlStateNormal];
		[m_backButton addTarget:self action:@selector(quitIGP:) forControlEvents:UIControlEventTouchUpInside];

		[self.view addSubview:m_scrollView];
		[self.view addSubview:m_pageControl];
		[self.view addSubview: m_backButton];
		m_gameCount = 0;
		m_gameList = NULL;
	}
	return self;
}

- (void) setGameList
{

	[self loadIconList];
}

- (void)unhighlightIcons
{
	for (int i = 0; i < m_gameCount; i++)
	{
		[m_iconButtons[i] setHighlighted:NO];
	}
}

- (void)loadIconList
{
	if (m_gameList)
	{
		for (int i = 0; i < m_gameCount; i++)
		{
			[m_iconLabels[i] removeFromSuperview];
			[m_iconButtons[i] removeFromSuperview];
			[m_badgeImageView[i] removeFromSuperview];
			[m_iconLabels[i] release];
			[m_iconButtons[i] release];
			[m_badgeImageView[i] release];
		}
		SAFE_DELETE_ARRAY(m_iconLabels);
		SAFE_DELETE_ARRAY(m_iconButtons);
		SAFE_DELETE_ARRAY(m_badgeImageView);
	}
	m_gameList	= IGP::s_igpInstance->_catalog;
	m_gameCount = IGP::s_igpInstance->_catalogCount ;
	
	const int maxRowsPerPage = 4;
	const int maxColsPerPage = 4;
	const int maxItemsPerPage = maxRowsPerPage * maxColsPerPage;
	m_pageCount = ceil((m_gameCount / (float)maxItemsPerPage));

	int viewWidth = m_scrollView.frame.size.width;
	int viewHeight = m_scrollView.frame.size.height;
	
	m_scrollView.contentSize = CGSizeMake(m_scrollView.frame.size.width * m_pageCount, m_scrollView.frame.size.height);
	
	int imageHeight = 57;
	int labelHeight = 14;
	int tileWidth = 57;
	int tileHeight = imageHeight + labelHeight;
	
	int tileSpacingX = (viewWidth - tileWidth * maxColsPerPage) / (maxColsPerPage + 1);
	int tileSpacingY = (viewHeight - tileHeight * maxRowsPerPage) / (maxRowsPerPage + 1);
	
	int iconStart = 0; //(pageNumber * maxItemsPerPage); //!AC_ Adding all pages
	int iconCount = (m_gameCount - iconStart); //!AC_: Testing, use Total of elements
	if (iconCount > maxItemsPerPage) 
	{
		iconCount = maxItemsPerPage;
	}
	
	[m_pageControl setNumberOfPages: m_pageCount];
	[m_pageControl setCurrentPage: 0];

	m_iconButtons	= new UIIconButton*[m_gameCount];
	m_iconLabels	= new UILabel*[m_gameCount];
	m_badgeImageView = new UIImageView*[m_gameCount];
	
	for (int i = 0; i < m_gameCount; i++)
	{
		int tx = (i % maxColsPerPage);
		int ty = (i % maxItemsPerPage) / maxColsPerPage;
		m_iconButtons[i] = [[UIIconButton buttonWithType:UIButtonTypeCustom] retain];
		[m_iconButtons[i] setFrame:CGRectMake(tx * (tileWidth + tileSpacingX) + tileSpacingX + (i / maxItemsPerPage) * viewWidth, ty * (tileHeight + tileSpacingY) + tileSpacingY, tileWidth, imageHeight)];
		[m_iconButtons[i] setImage:m_gameList[i]->GetIcon() forState:UIControlStateNormal];
		[m_iconButtons[i] addTarget:self action:@selector(iconPressed:) forControlEvents:UIControlEventTouchUpInside];
		[m_iconButtons[i] setTag:i % IGP::s_igpInstance->_catalogCount];

		m_iconLabels[i] = [[UILabel alloc] initWithFrame:CGRectMake(tx * (tileWidth + tileSpacingX) + tileSpacingX + (i / maxItemsPerPage) * viewWidth - tileWidth, ty * (tileHeight + tileSpacingY) + tileSpacingY + imageHeight, tileWidth + 2 * tileWidth, labelHeight)];
		[m_iconLabels[i] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
		[m_iconLabels[i] setText:m_gameList[i]->GetShortName()];
		[m_iconLabels[i] setTextColor: [UIColor whiteColor]];
		[m_iconLabels[i] setFont:[UIFont fontWithName:@"Arial" size:10]];
		[m_iconLabels[i] setTextAlignment:UITextAlignmentCenter];


		m_badgeImageView[i] = [[UIImageView alloc] initWithImage: IGP::m_badgeImage];
		[m_badgeImageView[i] setFrame:CGRectMake(tx * (tileWidth + tileSpacingX) + tileSpacingX + (i / maxItemsPerPage) * viewWidth +tileWidth - 10, ty * (tileHeight + tileSpacingY) + tileSpacingY  - 10, 20, 20)];
		
	//	[m_iconButtons[i] setFrame:CGRectMake([m_iconButtons[i] frame].origin.x, [m_iconButtons[i] frame].origin.y, 35, 35)];
		
		[m_scrollView addSubview:m_iconButtons[i]];
		[m_scrollView addSubview:m_iconLabels[i]];
		
		char fileSave[255];
		char fileSavePath[255];
		
		sprintf(fileSave, "%s_%s_save",m_gameList[i]->m_gameCode,IGP::s_igpDataLang);
		GetFileName(fileSave, true, fileSavePath);
		
		if(m_gameList[i]->m_priceUpdated||fileExists(fileSavePath))
		{
			[m_scrollView addSubview:m_badgeImageView[i]];
			IGP::s_igpInstance->m_newThings++;
		}
	}
	if(IGP::s_igpInstance->m_newThings>0)
		IGP::s_igpInstance->_catalogtab.tabBarItem.badgeValue = [[NSString alloc] initWithFormat:@"%d",IGP::s_igpInstance->m_newThings];
	else
		IGP::s_igpInstance->_catalogtab.tabBarItem.badgeValue = nil;

	
}

- (void)setParent: (UIViewController*)parent
{
	_parent = parent;
}

- (void)iconPressed:(id)sender
{
	CatalogViewController* parent = (CatalogViewController*)_parent;
	[parent pushGameInfo:[((UIButton*)sender) tag]];
}

- (void)scrollViewDidScroll:(UIScrollView *)sender
{
    CGFloat pageWidth = m_scrollView.frame.size.width;
    int curPage = floor((m_scrollView.contentOffset.x - pageWidth / 2) / pageWidth) + 1;
	[m_pageControl setCurrentPage: curPage];
}

- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
   // pageControlUsed = NO;
}

- (IBAction)changePage:(id)sender
{
    int curPage = [m_pageControl currentPage];
    
    CGRect frame = m_scrollView.frame;
    frame.origin.x = frame.size.width * curPage;
    frame.origin.y = 0;
    [m_scrollView scrollRectToVisible:frame animated:YES];

   // pageControlUsed = YES;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)didReceiveMemoryWarning
{
	trace(DEBUG_OBJCMEM, "CatalogIconsViewController threw memory warning!\n");
    [super didReceiveMemoryWarning];
}

-(void) showTitleOneLine
{
	
	
	m_titleLabel[0] = nil;
	NSString *labelTxt= IGPcStr::GetNSString(TXT_GAME_CATALOG);	
	m_titleLabel[0] = [[UILabel alloc] initWithFrame:CGRectMake(70,12,240,25)];
	[m_titleLabel[0] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
	[m_titleLabel[0] setText:labelTxt];
	m_titleLabel[0].textColor=[UIColor colorWithRed:242/255.0 green:62/255.0 blue:32/255.0 alpha:1];
	[m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:22]];
	m_titleLabel[0].textAlignment= UITextAlignmentRight;
	[self.view addSubview:m_titleLabel[0]];	
	
}


-(void) showTitle
{
		
		NSString *labelTxt= IGPcStr::GetNSString(TXT_GAME_CATALOG);
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
			m_titleLabel[1] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel,10,180,25)];
			labelDown= [labelAux objectAtIndex:0];
			[m_titleLabel[1] setFont:[UIFont boldSystemFontOfSize:22]];
		}
		else if([labelAux count]>=2)//concatenate string [2] " " and [3] and ....
		{
			m_titleLabel[1] = [[UILabel alloc] initWithFrame:CGRectMake(posxStartLabel,19,180,25)];
			labelDown=[labelAux objectAtIndex:1];
			for (int i=2;i<[labelAux count];i++)
			{
				labelDown=[labelDown stringByAppendingString:@" "];
				labelDown=[labelDown stringByAppendingString:[labelAux objectAtIndex:i]];	
			}
		
			[m_titleLabel[1] setFont:[UIFont boldSystemFontOfSize:19]];
		}
		[m_titleLabel[1] setText:labelDown];
		[m_titleLabel[1] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
		m_titleLabel[1].textColor=[UIColor colorWithRed:242/255.0 green:62/255.0 blue:32/255.0 alpha:1];
		m_titleLabel[1].textAlignment= UITextAlignmentRight;
		[self.view addSubview:m_titleLabel[1]];
		
}

- (void ) quitIGP:(id) sender
{
	if (/*m_isGoingToLink ||*/ IGP::s_igpInstance->m_state == IGP_STATE_RETRIEVING_LINK)
		return;
	[IGP::s_igpInstance->_tabBarController setSelectedIndex:0];
	[IGP::s_igpInstance->_tabBarController setViewControllers: nil];

	IGP::s_igpInstance->_whatsnewtab.tabBarItem.enabled = NO;
	IGP::s_igpInstance->_catalogtab.tabBarItem.enabled = NO;
	IGP::s_igpInstance->_bestsellerstab.tabBarItem.enabled = NO;
	IGP::s_igpInstance->_reviewtab.tabBarItem.enabled = NO;

	IGP::s_igpInstance->setState(IGP_STATE_END);
}

- (void)removeViews
{
	[m_backButton setImage: nil forState:UIControlStateNormal];
	
	for (int i = 0; i < m_gameCount; i++)
	{
		[m_iconLabels[i] removeFromSuperview];
		[m_iconButtons[i] removeFromSuperview];
		[m_badgeImageView[i] removeFromSuperview];
	}
	[m_backgroundImageView removeFromSuperview];
	
	if (m_titleLabel[0])
	{
		[m_titleLabel[0] removeFromSuperview];
	}
	if (m_titleLabel[1])
	{
		[m_titleLabel[1] removeFromSuperview];
	}
	
	[m_pageControl removeFromSuperview];
	[m_backButton removeFromSuperview];
	[m_scrollView removeFromSuperview];


}

- (void)dealloc
{
	trace(DEBUG_OBJCMEM, "Releasing CatalogIconsViewController\n");
	for (int i = 0; i < m_gameCount; i++)
	{
		SAFE_DELETE_OBJC(m_iconLabels[i]);
		SAFE_DELETE_OBJC(m_iconButtons[i]);
		SAFE_DELETE(m_badgeImageView[i]);
	}
	SAFE_DELETE_ARRAY(m_iconLabels);
	SAFE_DELETE_ARRAY(m_iconButtons);
	SAFE_DELETE_ARRAY(m_badgeImageView);
	
	SAFE_DELETE_OBJC(m_backgroundImageView);

	SAFE_DELETE_OBJC(m_titleLabel[0]);
	SAFE_DELETE_OBJC(m_titleLabel[1]);

	SAFE_DELETE_OBJC(m_pageControl);
	SAFE_DELETE_OBJC(m_backButton);
	SAFE_DELETE_OBJC(m_scrollView);

	[self.view release];

    [super dealloc];
}


@end
