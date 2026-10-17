//
//  CatalogViewController.mm
//  IGP
//
//  Created by administrador on 22/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "CatalogViewController.h"
#import "IGP.h"
#import "EN.h"

@implementation CatalogViewController

@synthesize m_infoViewController;

- (id)initWithBackgroundImage:(UIImage*) bgImage
{
	if (self = [super init]) 
	{
		self.title = IGPcStr::GetNSString(TXT_CATALOG);
		self.tabBarItem.image = IGP_IMAGE(m_catalogTabIcon);
		
		m_iconsViewController = [[CatalogIconsViewController alloc] initWithBackgroundImage:bgImage];
		m_infoViewController = [[GameInformationViewController alloc] initWithBackgroundImage:bgImage];
		[m_iconsViewController setParent:self];
		[m_infoViewController setParent:self];
		
		[self setNavigationBarHidden:YES];
		[self pushViewController:m_iconsViewController animated:YES];
	}
	return self;
}
- (void) unhighlightIcons
{
	[m_iconsViewController unhighlightIcons];
}
- (void)viewWillAppear:(BOOL)animated
{
	if (self.view != m_iconsViewController.view)
	{
		[self popViewControllerAnimated:animated];
	}
	[super viewWillAppear:animated];
}

- (void) setGameList
{
	[m_iconsViewController setGameList];
}

- (void) loadIconList
{
	[m_iconsViewController loadIconList];
}

- (void) refreshMe
{
    CGFloat pageWidth = m_iconsViewController.m_scrollView.frame.size.width;
    int curPage = floor((m_iconsViewController.m_scrollView.contentOffset.x - pageWidth / 2) / pageWidth) + 1;
	[m_iconsViewController.m_pageControl setCurrentPage: curPage];
}

- (void) pushGameInfo:(int) gameIndex
{
	IGP::s_igpInstance->FreeScreenshots();
	IGP::s_igpInstance->_catalog[gameIndex]->LoadScreenshots();
	[m_infoViewController setFrom:""];
	[m_infoViewController setGame:IGP::s_igpInstance->_catalog[gameIndex] from:(UINavigationController*)self isNewPrice:IGP::s_igpInstance->_catalog[gameIndex]->m_priceUpdated];
	IGP::s_igpInstance->_catalog[gameIndex]->m_priceUpdated = false;
	char fileSave[255];
	char fileSavePath[255];
	
	sprintf(fileSave, "%s_%s_save",IGP::s_igpInstance->_catalog[gameIndex]->m_gameCode,IGP::s_igpDataLang);
	GetFileName(fileSave, true, fileSavePath);
	
	if(fileExists(fileSavePath))
	{
		remove(fileSavePath);
	}
	IGP::s_igpInstance->savePricesStates();
	[m_iconsViewController loadIconList];//->m_badgeImageView[gameIndex] removeFromSuperview];
	[self pushViewController:m_infoViewController animated:YES];
	
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


- (void)didReceiveMemoryWarning
{
	trace(DEBUG_OBJCMEM, "CatalogViewController threw memory warning!\n");
    [super didReceiveMemoryWarning];
}

- (void)removeViews
{
	[self popToRootViewControllerAnimated:NO];
	self.tabBarItem.image = nil;
	[m_iconsViewController removeViews];
	[m_infoViewController removeViews];
}

- (void)dealloc
{
	trace(DEBUG_OBJCMEM, "Releasing CatalogViewController\n");
	[m_iconsViewController release];
	[m_infoViewController release];
	m_infoViewController = nil;
    [super dealloc];
}

//this was made to cover for a retain inc that happens on each view enter
- (void)viewDidDisappear:(BOOL)animated
{

	[self release];

}

- (void)viewDidAppear:(BOOL)animated
{
	IGP::s_igpInstance->m_selectedIndex = 2;
}


@end
