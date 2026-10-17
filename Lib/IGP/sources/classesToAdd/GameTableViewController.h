//
//  GameTableViewController.h
//  IGP
//
//  Created by Christian Mazzeo on 18/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "IGPDefines.h"

@interface GameTableViewController : UIViewController<UITableViewDataSource, UITableViewDelegate>
{
	UITableView*	gameTable;
	UIImageView*	m_backgroundImageView;
	UIButton*		m_backButton;
	UILabel*		m_titleLabel[2];
	NSIndexPath *	lastIndexPath;
}
-(void) showTitleOneLine;
- (void)removeViews;
@end
