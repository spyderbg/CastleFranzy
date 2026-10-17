//
//  UIIconButton.mm
//  IGP
//
//  Created by administrador on 17/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "UIIconButton.h"


@implementation UIIconButton
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
{
	if ([self isHighlighted])
	{
		[super touchesEnded:touches withEvent:event];
	}
	else
	{
		[self setHighlighted:NO];
		[self setSelected:NO];
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
{
	
    for (UITouch *myTouch in touches)
    {
		//t.phase			= myTouch.phase;
		CGPoint location	= [myTouch locationInView:self];
		if (location.x < 0 || location.y < 0 || location.x > [self frame].size.width || location.y > [self frame].size.height)
		{
			[self setHighlighted:NO];
			[self setSelected:NO];
		}
		//printf("X:%f Y:%f\n", t.curLocation.x, t.curLocation.y);
		//t.prevLocation	= [myTouch previousLocationInView:self];
		//t.tapCount		= [myTouch tapCount];
		//if (igp) igp->handleTouch(t);
    }
}

@end
