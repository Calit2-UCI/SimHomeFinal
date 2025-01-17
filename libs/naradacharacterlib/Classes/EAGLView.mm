//
//  EAGLView.m
//  BasicCharacter
//
//  Created by Enrique Oriol on 9/20/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "EAGLView.h"

#import "ES1Renderer.h"
//#import "ES2Renderer.h"

@implementation EAGLView

@synthesize animating;
@synthesize renderer;
@dynamic animationFrameInterval;
@synthesize frameRate;



// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder
{    
    if ((self = [super initWithCoder:coder]))
    {
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] == YES && [[UIScreen mainScreen] scale] == 2.00) {
            // RETINA DISPLAY
            NSLog(@"Using retina display!!");
            self.contentScaleFactor = 2.0;
            eaglLayer.contentsScale=2;

        }
        
        //renderer = [[ES2Renderer alloc] init];
//
  //      if (!renderer)
    //    {
            renderer = [[ES1Renderer alloc] init];

            if (!renderer)
            {
                [self release];
                return nil;
            }
      //  }

        animating = FALSE;
        displayLinkSupported = FALSE;
        animationFrameInterval = 1;
        displayLink = nil;
        animationTimer = nil;
		
		frameRate = 30;

        // A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
        // class is used as fallback when it isn't available.
        NSString *reqSysVer = @"3.1";
        NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
        if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
            displayLinkSupported = TRUE;
    }

    return self;
}

//- (void)updateModelsWithAttitude:(CMAttitude*)attitude
//{
////    [renderer.modelManager rotateModelX:attitude.yaw y:attitude.pitch absolute:YES];
//}


- (EAGLSharegroup*)getRendererContextSharegroup
{
    return renderer.context.sharegroup;
}

- (EAGLContext*)getContext
{
    return renderer.context;
}

- (void)drawView:(id)sender
{
    [renderer render];
}

- (void)layoutSubviews
{
    [renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
    [[NSNotificationCenter defaultCenter] postNotificationName:notificationEAGLViewHasBeenResized object:nil];
    [self drawView:nil];
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    // Frame interval defines how many display frames must pass between each time the
    // display link fires. The display link will only fire 30 times a second when the
    // frame internal is two on a display that refreshes 60 times a second. The default
    // frame interval setting of one will fire 60 times a second when the display refreshes
    // at 60 times a second. A frame interval setting of less than one results in undefined
    // behavior.
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;

        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!animating)
    {
        if (displayLinkSupported)
        {
            // CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
            // if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
            // not be called in system versions earlier than 3.1.

            displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
            [displayLink setFrameInterval:animationFrameInterval];
            [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        }
        else
            animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / frameRate) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];

        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        if (displayLinkSupported)
        {
            [displayLink invalidate];
            displayLink = nil;
        }
        else
        {
            [animationTimer invalidate];
            animationTimer = nil;
        }

        animating = FALSE;
    }
	
}


- (void)dealloc
{
    [renderer release];

    [super dealloc];
}


- (void)deleteRenderer
{
	[renderer deleteRenderer];
}




@end
