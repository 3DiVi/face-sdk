#import <Foundation/Foundation.h>
#import "Capturer.h"
#include "pbio/Capturer.h"

#import "TryCatch.mm"


@implementation Capturer

-(nonnull instancetype) init: (void*) cap{
    
    capturer = cap;
    return self;
}

-(void) dealloc{
    delete static_cast<pbio::Capturer::Ptr*>(capturer);
}

-(nonnull NSMutableArray*)capture: (RawImage *) raw_image {
    pbio::Capturer::Ptr cap = (*(pbio::Capturer::Ptr*)capturer);
    pbio::RawImage rawImage = (*(pbio::RawImage*)raw_image.raw_image);
    
    std::vector<pbio::RawSample::Ptr> samples =  tryCatch(cap->capture(rawImage));
    
    NSMutableArray *result = [NSMutableArray new];
    
    for (auto sample : samples){
        [result addObject: [[RawSample alloc] init: new pbio::RawSample::Ptr(sample)]];
    }
    
    return result;
}

@synthesize capturer;
@end
