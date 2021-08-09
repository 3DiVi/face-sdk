#import <Foundation/Foundation.h>
#import "Template.h"
#import "RawSample.h"


@interface MatchResult : NSObject
{
    void *match_result;
    
    double distance;
    double fa_r;
    double fr_r;
    double score;
}
-(nonnull instancetype) init: (void* _Nonnull) match;
-(void) dealloc;

@property (readonly) double distance;
@property (readonly) double fa_r;
@property (readonly) double fr_r;
@property (readonly) double score;
@end

@interface Recognizer : NSObject
{
    void *recognizer;
}
-(nonnull instancetype) init: (void* _Nonnull) reco;
-(void) dealloc;

-(nonnull Template *) loadTemplate: (const void* _Nonnull const) data : (const int) size;
-(nonnull Template*) loadTemplate:(const char* _Nullable)file_path;
-(nonnull MatchResult *) getROCCurvePointByFAR: (const double) desired_far;
-(nonnull Template *) processing: (RawSample *_Nullable)raw_sample;

@end

