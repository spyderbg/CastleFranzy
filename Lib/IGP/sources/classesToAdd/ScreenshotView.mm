//
//  ScreenshotView.mm
//  IGP
//
//  Created by administrador on 28/11/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "ScreenshotView.h"
#import "WhatsNewViewController.h"
#include "IGP.h"
@implementation ScreenshotView


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        // Initialization code
		[self setUserInteractionEnabled:YES];
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    // Drawing code
}


- (void)dealloc {
	trace(DEBUG_OBJCMEM, "Releasing ScreenshotView\n");
    [super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
{
	#ifdef USE_DOUBLE_TOUCH
		if (CFAbsoluteTimeGetCurrent() - m_lastTouchTime < 0.3 && IGP::s_igpInstance->_currentTab != NULL)
	#else
		if ( IGP::s_igpInstance->_currentTab != NULL)
	#endif 
	{
		[(WhatsNewViewController*)IGP::s_igpInstance->_currentTab openGameInformation];
	}
	m_lastTouchTime = CFAbsoluteTimeGetCurrent();
}
@end
