#import <Foundation/Foundation.h>
#import "RawImage.h"
#import "RawSample.h"

@interface Capturer : NSObject
{
    void *capturer;
}

-(nonnull instancetype) init: (void*_Nonnull) cap;
-(void) dealloc;
-(nonnull NSMutableArray *) capture: (RawImage *_Nonnull) raw_image ;

@property void * _Nullable capturer;

@end
