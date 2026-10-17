//
//  CatalogViewController.h
//  IGP
//
//  Created by administrador on 22/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "IGPGame.h"
#import "UIIconButton.h"

@interface CatalogIconsViewController : UIViewController<UIScrollViewDelegate> {
	UIViewController*_parent;
	IGPGame**		m_gameList;
	int				m_pageCount;
	int				m_gameCount;
	
	//Allocated data
	UIImageView*	m_backgroundImageView;
	UIImageView**	m_badgeImageView;
	UIPageControl*	m_pageControl;
	UIScrollView*	m_scrollView;
	//UIImageView*	m_glLogoImage;
	UIIconButton**		m_iconButtons;
	UILabel**		m_iconLabels;
	UILabel*		m_titleLabel[2];
	UIButton*		m_backButton;
}

@property (nonatomic, retain)		UIScrollView*	m_scrollView;
@property (nonatomic, retain)		UIPageControl*	m_pageControl;

- (id)initWithBackgroundImage:(UIImage*) bgImage;
- (void) loadIconList;
- (void) setParent: (UIViewController*)parent;
- (void) setGameList;
- (void) showTitle;
- (void) unhighlightIcons;
- (void) removeViews;
-(void) showTitleOneLine;
@end
