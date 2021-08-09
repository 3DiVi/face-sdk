#import <Foundation/Foundation.h>
#import "Point.h"

@implementation Point2
-(nonnull instancetype) init: (float) X : (float) Y : (float) Z{
    x = X;
    y = Y;
    z = Z;    
    return self;
}

@synthesize x;
@synthesize y;
@synthesize z;

@end
