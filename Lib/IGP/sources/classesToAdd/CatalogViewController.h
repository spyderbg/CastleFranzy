//
//  CatalogViewController.h
//  IGP
//
//  Created by administrador on 22/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CatalogIconsViewController.h"
#import "GameInformationViewController.h"

@interface CatalogViewController : UINavigationController<UIScrollViewDelegate> {
	CatalogIconsViewController*		m_iconsViewController;
	GameInformationViewController*	m_infoViewController;
}
@property (nonatomic, retain)		GameInformationViewController* m_infoViewController;
- (id)initWithBackgroundImage:(UIImage*) bgImage;
- (void) pushGameInfo:(int) gameIndex;
- (void) setGameList;
- (void) loadIconList;
- (void) unhighlightIcons;
- (void) removeViews;
@end
