
#import "buy_buttonViewController.h"

#define K_LINK_BUY @"http://ingameads.gameloft.com/redir/?from=FROMGAME&op=IPHO&pp=1&game=TOGAME&ver=VERSION&lg=LANG&country=COUNTRY&d=DEVICE&f=FIRMWARE&udid=UDIDPHONE"
#define K_LINK_TWITTER @"http://ingameads.gameloft.com/redir/?from=FROMGAME&op=IPHO&pp=1&t=twitter&game=TOGAME&ver=VERSION&lg=LANG&country=COUNTRY&d=DEVICE&f=FIRMWARE&udid=UDIDPHONE"
#define K_LINK_FACEBOOK @"http://ingameads.gameloft.com/redir/?from=FROMGAME&op=IPHO&pp=1&t=facebook&game=TOGAME&ver=VERSION&lg=LANG&country=COUNTRY&d=DEVICE&f=FIRMWARE&udid=UDIDPHONE"

#define NB_LANG  6
#define NB_TEXTS 2

//MODIFY HERE :: defines for the languages supported in this test app

#define LANG_EN 	0
#define LANG_FR		1
#define LANG_DE		2
#define LANG_IT		3
#define LANG_SP		4
#define LANG_JP		5

NSString* texts[NB_TEXTS][NB_LANG] = {
{@"OK",@"OK",@"OK",@"OK",@"Acep.",@"Ok"},
{@"Cannot connect to server. No network found. Please try again later.",@"Impossible de se connecter au serveur. Aucun réseau trouvé.Veuillez réessayer plus tard.",@"Verbindung zum Server konnte nicht hergestellt werden. Kein Netzwerk gefunden. Versuche es bittespäter noch einmal.",@"Impossibile connettersi al server. Rete non trovata. Si prega di riprovare più tardi.",@"No se puede conectarcon el servidor. No se encuentra ninguna red de conexión. Por favor, inténtalo mástarde.",@"サーバーに接続できません。ネットワークが見つかりません。のちほどもう一度お試しください。"},
};

@implementation buy_buttonViewController


@synthesize _activity;
/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

// Implement loadView to create a view hierarchy programmatically,
// without using a nib.
- (void)loadView 
{
	//allocate the view
	self.view = [[UIView alloc] initWithFrame:[[UIScreen mainScreen] applicationFrame]];
	   
	//set the view's background color
	self.view.backgroundColor = [UIColor whiteColor];

	//create the button
	UIButton *button = [UIButton buttonWithType:UIButtonTypeRoundedRect];
	   
	//set the position of the button
	button.frame = CGRectMake(100, 170, 100, 30);

	//set the button's title
	[button setTitle:@"Click Me!" forState:UIControlStateNormal];

	//listen for clicks
	[button addTarget:self action:@selector(buttonPressed)
	forControlEvents:UIControlEventTouchUpInside];

	//add the button to the view
	[self.view addSubview:button];
	
	//add activity
	_activity = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
	[_activity setFrame:CGRectMake(160-20,280-20,40,40)];	
	[self.view addSubview:_activity];
	_activity.hidden = YES;
}

-(void)buttonPressed 
{
  NSLog(@"Button Pressed!");
   
    NSString *link= K_LINK;		//MODIFY HERE :: use K_LINK_BUY or K_LINK_TWITTER or K_LINK_FACEBOOK
    link = [link stringByReplacingOccurrencesOfString:@"FROMGAME" withString:@"XXXX"];	//MODIFY HERE :: the code of the application you integrate this feature in (LITE or FULL)
    link = [link stringByReplacingOccurrencesOfString:@"TOGAME" withString:@"ASP4"];	//MODIFY HERE :: the code of the Full Version application
    link = [link stringByReplacingOccurrencesOfString:@"VERSION" withString:@"1.2.3"];	//MODIFY HERE :: your version string
    link = [link stringByReplacingOccurrencesOfString:@"LANG" withString:@"EN"];		//MODIFY HERE :: current language of the application
    link = [link 
				stringByReplacingOccurrencesOfString:@"COUNTRY" 
				withString:[[NSString alloc] initWithString: [[NSLocale currentLocale] objectForKey:NSLocaleCountryCode]]];
    link = [link 
				stringByReplacingOccurrencesOfString:@"DEVICE" 
				withString:[[[UIDevice currentDevice] model]stringByReplacingOccurrencesOfString:@" " withString: @"_"]];
    link = [link stringByReplacingOccurrencesOfString:@"FIRMWARE" withString:[[UIDevice currentDevice] systemVersion]];
    link = [link stringByReplacingOccurrencesOfString:@"UDIDPHONE" withString:[[UIDevice currentDevice] uniqueIdentifier]];

	
//    _window.userInteractionEnabled = NO;
    _activity.hidden = NO;
	[_activity startAnimating];
    [_activity setAlpha:1.0];

    NSURLRequest *theRequest=[NSURLRequest 
								requestWithURL:[NSURL URLWithString:link] 
								cachePolicy:NSURLRequestReloadIgnoringCacheData 
								timeoutInterval:13.0];

    // create the connection with the request
    // and start loading the data
    NSURLConnection *theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
    if (theConnection) 
	{
        // Create the NSMutableData that will hold
        // the received data
        // receivedData is declared as a method instance elsewhere
               
		receivedData=[[NSMutableData data] retain];
            
    } 
	else 
	{
        // inform the user that the download could not be made
        //_window.userInteractionEnabled = YES;
        _activity.hidden = YES;
		
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"" 
									message:texts[1][LANG_JP] 
									delegate:nil 
									cancelButtonTitle:texts[0][LANG_JP] 
									otherButtonTitles:nil];
        [alert show];
        [alert release];
    }
     
}

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

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}



- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    // this method is called when the server has determined that it
    // has enough information to create the NSURLResponse
	
    // it can be called multiple times, for example in the case of a
    // redirect, so each time we reset the data.
    // receivedData is declared as a method instance elsewhere
    [receivedData setLength:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    // append the new data to the receivedData
    // receivedData is declared as a method instance elsewhere
    [receivedData appendData:data];
}

- (void)connection:(NSURLConnection *)connection
  didFailWithError:(NSError *)error
{
    // release the connection, and the data object
    [connection release];
    // receivedData is declared as a method instance elsewhere
    [receivedData release];
	
    // inform the user
    NSLog(@"Connection failed! Error - %@ %@",
          [error localizedDescription],
          [[error userInfo] objectForKey:NSErrorFailingURLStringKey]);
	//_window.userInteractionEnabled = YES;
	_activity.hidden = YES;
                                 
	UIAlertView *alert = [[UIAlertView alloc] 
									initWithTitle:@"" 
									message:texts[1][LANG_JP] 
									delegate:nil 
									cancelButtonTitle:texts[0][LANG_JP] 
									otherButtonTitles:nil];
	[alert show];                    
	[alert release];                    
}                                
                                 
- (void)connectionDidFinishLoading:(NSURLConnection *)connection      
{                                
    // do something with the data                    
    // receivedData is declared as a method instance elsewhere        
    NSLog(@"Succeeded! Received %d bytes of data",[receivedData length]);
	                               
	NSString *realLink = [[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding];
	[connection release];                    
	[receivedData release];                    

	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:realLink]];
    // release the connection, and the data object                    
    
}






@end

