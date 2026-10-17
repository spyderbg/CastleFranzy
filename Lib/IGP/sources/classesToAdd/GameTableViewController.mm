//
//  GameTableViewController.mm
//  IGP
//
//  Created by Christian Mazzeo on 18/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "GameTableViewController.h"

#import "IGP.h"
@implementation GameTableViewController
/*
gameTable=[[UITableView alloc] initWithFrame:CGRectMake(0,0,320,400) style:UITableViewStylePlain];
[gameTable setDataSource:self];

[self.view addSubview:gameTable];
*/
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)init {
    if (self = [super init]) {
        [self loadView];
		self.view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 320, 480)];
		self.tabBarItem.image = IGP_IMAGE(m_feedbackTabIcon);
		self.title = self.title = IGPcStr::GetNSString(TXT_FEEDBACK);
		gameTable = [[UITableView alloc] initWithFrame:CGRectMake(0, 50, 320, 380) style:UITableViewStyleGrouped];
		[gameTable setDataSource:self];
		[gameTable setDelegate:self];
		[gameTable setBackgroundColor:[UIColor clearColor]];
		m_backgroundImageView = [[UIImageView alloc] initWithImage: IGP_IMAGE(m_bgImage)];
		[m_backgroundImageView setFrame:CGRectMake(0,0, 320, 480)];
		
		[self.view addSubview:m_backgroundImageView];
		
		m_backButton = [[UIButton alloc] initWithFrame:CGRectMake(2, 5, 70, 36)];
		[m_backButton setImage: IGP_IMAGE(m_backButtonImage) forState:UIControlStateNormal];
		[m_backButton addTarget:self action:@selector(pop:) forControlEvents:UIControlEventTouchUpInside];
		[self.view addSubview:m_backButton];
		//[m_backgroundImageView setTransform:CGAffineTransformMakeRotation(3.14)];
		[self.view addSubview:gameTable];
		[self showTitleOneLine];		//chris++
		lastIndexPath = NULL;
	}
    return self;
}

-(void) showTitleOneLine
{
	
	
	m_titleLabel[0] = nil;
	NSString *labelTxt= IGPcStr::GetNSString(TXT_CHOOSE_GAME);	
	m_titleLabel[0] = [[UILabel alloc] initWithFrame:CGRectMake(90,12,220,25)];
	[m_titleLabel[0] setBackgroundColor: [UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
	[m_titleLabel[0] setText:labelTxt];
	m_titleLabel[0].textColor=[UIColor colorWithRed:242/255.0 green:62/255.0 blue:32/255.0 alpha:1];
	[m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:22]];
	
	if (IGP::s_igpInstance->_currentLanguage==1)
	{

		[m_titleLabel[0] setFont:[UIFont boldSystemFontOfSize:18]];	
	}
	 
	m_titleLabel[0].textAlignment= UITextAlignmentRight;
	[self.view addSubview:m_titleLabel[0]];	
	
}
/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
	[super loadView];
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/


/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    NSIndexPath*    selection = [gameTable indexPathForSelectedRow];
    if (selection)
        [gameTable deselectRowAtIndexPath:selection animated:NO];
   
    [gameTable reloadData];
	
	//salcideon double check, clear the edit box when entering here
	[IGP::s_igpInstance->_reviewtab.fromTextField resignFirstResponder];
	[IGP::s_igpInstance->_reviewtab.reviewTextView resignFirstResponder];
	[IGP::s_igpInstance->_reviewtab.eMailTextField resignFirstResponder];


/*	[super viewWillAppear:animated];
	if (lastIndexPath != NULL)
	{

	[gameTable deselectRowAtIndexPath:lastIndexPath animated:NO];
	}
*/
	
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	
    return 1;
}

- (IBAction) pop:(id) sender
{

//	if ([gameTable indexPathForSelectedRow]==nil)
	{
		[IGP::s_igpInstance->_reviewtab setCurrentGameDefault];
		
		//UITableViewCell* cell;
		//[cell setSelected:NO animated:NO];
		//cell=[gameTable cellForRowAtIndexPath:lastIndexPath ];
		//cell.accessoryType=UITableViewCellAccessoryNone;
		lastIndexPath = NULL;
	}
	[IGP::s_igpInstance->_reviewtab popViewControllerAnimated:YES];
	
}

/*
- (UITableViewCellAccessoryType)tableView:(UITableView *)tableView accessoryTypeForRowWithIndexPath:(NSIndexPath *)indexPath
{
	NSIndexPath *tableSelection = [gameTable indexPathForSelectedRow];
	if(tableSelection.row == indexPath.row)
	{
		return UITableViewCellAccessoryCheckmark;
	}
	else
	{
		return UITableViewCellAccessoryNone;
	}
}
 */
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	
//	UITableViewCell* cell;
	
//	cell=[gameTable cellForRowAtIndexPath:lastIndexPath ];

	
	//Fer662 fixed crash when selecting some games after update. Added missing retain/release for the indexpath
	if (lastIndexPath)[lastIndexPath release];
	lastIndexPath= [indexPath retain];
	//- (UITableViewCell *)cellForRowAtIndexPath:(NSIndexPath *)indexPath;
	
	
	if ([indexPath row] != 0 && [indexPath row] != 1)
	{
		[IGP::s_igpInstance->_reviewtab setCurrentGame:IGP::s_igpInstance->_tableCatalog[[indexPath row] - 2]];
	}
	else if( [indexPath row] == 1) //selected game that launched IGP
	{
		[IGP::s_igpInstance->_reviewtab setCurrentGamewithName: IGP::s_mainGameShortTitle];
		
	}
		
	else
	{
		[IGP::s_igpInstance->_reviewtab setCurrentGame:NULL];
		[IGP::s_igpInstance->_reviewtab.m_currentGameCode release];
		IGP::s_igpInstance->_reviewtab.m_currentGameCode = @"ALLG";
		[IGP::s_igpInstance->_reviewtab.m_currentGameCode retain];
	}
//	[cell setSelected:NO animated:NO];
	[IGP::s_igpInstance->_reviewtab popViewControllerAnimated:YES];

	
	
	
}



// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return IGP::s_igpInstance->_tableCatalogCount + 2;
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    NSString *cellIdentifier = [NSString stringWithFormat:@"Cell%d",[indexPath row]];
   // NSLog(@"CELDA: %s",cellIdentifier);
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:cellIdentifier] autorelease];
		cell.selectionStyle = UITableViewCellSelectionStyleBlue;
    }
	
	if( [indexPath row]==0)
	{
		cell.text =	 IGPcStr::GetNSString(TXT_ALL_GAMES);
	}
	else if( [indexPath row]==1)
	{	
		cell.text =	IGP::s_mainGameShortTitle;
	}
	else
	{

		cell.text= IGP::s_igpInstance->_tableCatalog[[indexPath row] - 2]->GetName();
	}
	
    return cell;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}
- (void) removeViews
{
	[m_backButton removeFromSuperview];
	[m_backgroundImageView removeFromSuperview];
	if (m_titleLabel[0]) [m_titleLabel[0] removeFromSuperview];
	if (m_titleLabel[1]) [m_titleLabel[1] removeFromSuperview];
	[gameTable removeFromSuperview];
	[gameTable setDelegate: NULL];
	[gameTable setDataSource:NULL];
	
	
}

- (void)dealloc {
	[self removeViews];
	if (m_titleLabel[0]) [m_titleLabel[0] release];
	if (m_titleLabel[1]) [m_titleLabel[1] release];
	[m_backButton release];
	[m_backgroundImageView release];
	[gameTable release];
	
	[self.view release];
	
    [super dealloc];
}

- (void)viewDidDisappear:(BOOL)animated{
//[IGP::s_igpInstance->_tab5 getDownReviewTextForEdit];
[IGP::s_igpInstance->_reviewtab.reviewTextView resignFirstResponder];
}


@end
