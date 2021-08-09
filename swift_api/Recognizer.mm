#import <Foundation/Foundation.h>
#import "Recognizer.h"
#include "pbio/Recognizer.h"

#import "TryCatch.mm"
#include <fstream>


@implementation Recognizer

-(nonnull instancetype) init: (void*) reco{
    recognizer = reco;
    return self;
}

-(void) dealloc{
    delete static_cast<pbio::Recognizer::Ptr*>(recognizer);
}

-(nonnull Template*) loadTemplate:(const void *const)data : (const int)size{
    pbio::Recognizer::Ptr reco = (*(pbio::Recognizer::Ptr*)recognizer);
    
    return [[Template alloc] init: new pbio::Template::Ptr(tryCatch(reco->loadTemplate(data, size)))];
}

-(nonnull Template*) loadTemplate:(const char*)file_path{
    pbio::Recognizer::Ptr reco = (*(pbio::Recognizer::Ptr*)recognizer);
    
    std::ifstream istream(file_path, std::ios_base::binary);

    return [[Template alloc] init: new pbio::Template::Ptr(tryCatch(reco->loadTemplate(istream)))];
}

-(nonnull MatchResult*) getROCCurvePointByFAR:(const double)desired_far{
    pbio::Recognizer::Ptr reco = (*(pbio::Recognizer::Ptr*)recognizer);
    
    pbio::Recognizer::MatchResult match_result = tryCatch(reco->getROCCurvePointByFAR(desired_far));
    
    return [[MatchResult alloc] init: new pbio::Recognizer::MatchResult(match_result)];
}

-(nonnull Template *) processing:(RawSample *)raw_sample{
    pbio::Recognizer::Ptr reco = (*(pbio::Recognizer::Ptr*)recognizer);
    const pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample.raw_sample);
    
    return [[Template alloc] init: new pbio::Template::Ptr(reco->processing(*sample))];
}

@end

@implementation MatchResult

-(nonnull instancetype) init: (void *)match{
    match_result = match;
    return self;
}
-(void) dealloc{
    void (*del_func)(void*);
    del_func = [](void *p){delete static_cast<pbio::Recognizer::MatchResult*>(p);};
    del_func(match_result);
}

-(double) distance{
    pbio::Recognizer::MatchResult match = (*(pbio::Recognizer::MatchResult*)match_result);
    
    return match.distance;
}

-(double) fa_r{
    pbio::Recognizer::MatchResult match = (*(pbio::Recognizer::MatchResult*)match_result);
    
    return match.fa_r;
}

-(double) fr_r{
    pbio::Recognizer::MatchResult match = (*(pbio::Recognizer::MatchResult*)match_result);
    
    return match.fr_r;
}

-(double) score{
    pbio::Recognizer::MatchResult match = (*(pbio::Recognizer::MatchResult*)match_result);
    
    return match.score;
}
@end
