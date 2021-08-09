#import <Foundation/Foundation.h>
#import "CallbackData.h"
#include "pbio/VideoWorker.h"

#import "TryCatch.mm"

@implementation TrackingCallbackData

-(nonnull instancetype) init{
    
    trackin_callback = new pbio::VideoWorker::TrackingCallbackData();
    
    return self;
}

-(nonnull instancetype) init: (void*) callback{
    
    trackin_callback = callback;
    return  self;
}
-(void) dealloc{
    delete static_cast<pbio::VideoWorker::TrackingCallbackData*>(trackin_callback);
}

-(int64_t) stream_id{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    return callback.stream_id;
}

-(int64_t) frame_id{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    return callback.frame_id;
}

-(NSMutableArray<NSNumber*>*) samples_track_id{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_track_id){
        [result addObject: @(i)];
    }
    
    return result;
}

-(nonnull NSMutableArray<RawSample*>*) samples{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray<RawSample*> *result = [[NSMutableArray alloc] init];
    
    for (auto sample: callback.samples){
        void *buf = new pbio::RawSample::Ptr(sample);
        RawSample *raw_sample = [[RawSample alloc] init: buf];
        [result addObject: raw_sample];
    }
    
    return result;
}

-(nonnull NSMutableArray*) samples_week{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_weak){
        [result addObject: [NSNumber numberWithBool:(i)]];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_quality{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_quality){
        [result addObject: [NSNumber numberWithFloat:(i)]];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_good_light_and_blur{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_good_light_and_blur){
        [result addObject: [NSNumber numberWithInt: i]];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_good_andles{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_good_angles){
        [result addObject: [NSNumber numberWithInt: i]];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_good_face_size{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_good_face_size){
        [result addObject: [NSNumber numberWithInt: i]];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_detector_confirmed{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_detector_confirmed){
        [result addObject: [NSNumber numberWithInt: i]];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_depth_liveness_confirmed{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_depth_liveness_confirmed){
        [result addObject: [NSNumber numberWithInt: i]];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_track_age_gender_set{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_track_age_gender_set){
        [result addObject: [NSNumber numberWithBool: i]];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_track_age_gender{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_track_age_gender){
        [result addObject: [[AgeGender alloc] init: new pbio::AgeGenderEstimator::AgeGender(i)]];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_track_emotions_set{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_track_emotions_set){
        [result addObject: [NSNumber numberWithBool:(i)]];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_track_emotions{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    
    for (auto i: callback.samples_track_emotions){
        NSMutableArray *buf = [[NSMutableArray alloc] init];
        for (auto j : i){
            void *q = new pbio::EmotionsEstimator::EmotionConfidence(j);
            
            [buf addObject: [[EmotionsConfidence alloc] init: q]];
        }
        [result addObject: buf];
    }

    return result;
}

-(nonnull NSMutableArray*) samples_active_liveness_status{
    pbio::VideoWorker::TrackingCallbackData callback = (*(pbio::VideoWorker::TrackingCallbackData*)trackin_callback);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.samples_active_liveness_status){
        [result addObject: [[ActiveLivenessStatus alloc] init: new pbio::ActiveLiveness::ActiveLivenessStatus(i)]];
    }

    return result;
}
@end


@implementation MatchFoundCallbackData
-(nonnull instancetype) init:(void *)callback{
    match_found = callback;
    return self;
}

-(void) dealloc{
    /*void (*del_func)(void*);
    del_func = [](void *p){delete static_cast<pbio::VideoWorker::MatchFoundCallbackData*>(p);};
    del_func(match_found);*/
    delete static_cast<pbio::VideoWorker::MatchFoundCallbackData*>(match_found);
}


-(int64_t) frame_id{
    pbio::VideoWorker::MatchFoundCallbackData callback = (*(pbio::VideoWorker::MatchFoundCallbackData*)match_found);
    return callback.frame_id;
}

-(int64_t) stream_id{
    pbio::VideoWorker::MatchFoundCallbackData callback = (*(pbio::VideoWorker::MatchFoundCallbackData*)match_found);
    return callback.stream_id;
}

-(nonnull RawSample *) sample{
    pbio::VideoWorker::MatchFoundCallbackData callback = (*(pbio::VideoWorker::MatchFoundCallbackData*)match_found);
    pbio::RawSample::Ptr samp = callback.sample;
    
    return [[RawSample alloc] init: new pbio::RawSample::Ptr(samp)];
}

-(double) quality{
    pbio::VideoWorker::MatchFoundCallbackData callback = (*(pbio::VideoWorker::MatchFoundCallbackData*)match_found);
    return callback.quality;
}

-(nonnull Template *) templ{
    pbio::VideoWorker::MatchFoundCallbackData callback = (*(pbio::VideoWorker::MatchFoundCallbackData*)match_found);
    pbio::Template::Ptr tmpl = callback.templ;
    
    return [[Template alloc] init: new pbio::Template::Ptr(tmpl)];
}

-(nonnull NSMutableArray*) search_results{
    pbio::VideoWorker::MatchFoundCallbackData callback = (*(pbio::VideoWorker::MatchFoundCallbackData*)match_found);
    
    NSMutableArray *result = [[NSMutableArray alloc] init];
    
    for (auto i: callback.search_results){
        [result addObject: [[SearchResult alloc] init: new pbio::VideoWorker::SearchResult(i)]];
    }

    return result;
}

@end



@implementation AgeGender

-(nonnull instancetype) init:(void *)age_gender{
    pbio::AgeGenderEstimator::AgeGender age_gen = (*(pbio::AgeGenderEstimator::AgeGender*)age_gender);
    
    age = Age(age_gen.age);
    gender = Gender(age_gen.gender);
    age_year = age_gen.age_years;
    
    delete static_cast<pbio::AgeGenderEstimator::AgeGender*>(age_gender);
    return self;
}
@synthesize age;
@synthesize gender;
@synthesize age_year;
@end

@implementation EmotionsConfidence

-(nonnull instancetype) init:(void *)emotions_confidence{
    pbio::EmotionsEstimator::EmotionConfidence emotions = (*(pbio::EmotionsEstimator::EmotionConfidence*)emotions_confidence);
    
    emotion = Emotion(emotions.emotion);
    confidence = emotions.confidence;
    
    delete static_cast<pbio::EmotionsEstimator::EmotionConfidence*>(emotions_confidence);
    return self;
}

@synthesize confidence;
@synthesize emotion;
@end

@implementation SearchResult
-(nonnull instancetype) init:(void *)search_result{
    pbio::VideoWorker::SearchResult search = (*(pbio::VideoWorker::SearchResult*)search_result);
    person_id = search.person_id;
    element_id = search.element_id;
    match_result = [[MatchResult alloc] init: new pbio::Recognizer::MatchResult(search.match_result)];
    
    delete static_cast<pbio::VideoWorker::SearchResult*>(search_result);
    return self;
}

@synthesize person_id;
@synthesize element_id;
@synthesize match_result;
@end

@implementation ActiveLivenessStatus

-(nonnull instancetype) init{
    verdict = NOT_COMPUTED;
    check_type = NONE;
    progress_level = 0;
    return self;
}
-(nonnull instancetype) init: (void* _Nullable) active_liveness{
    pbio::ActiveLiveness::ActiveLivenessStatus liveness = (*(pbio::ActiveLiveness::ActiveLivenessStatus*)active_liveness);
    verdict = Liveness(liveness.verdict);
    check_type = CheckType(liveness.check_type);
    progress_level = liveness.progress_level;
    
    delete static_cast<pbio::ActiveLiveness::ActiveLivenessStatus*>(active_liveness);
    return self;
}

@synthesize verdict;
@synthesize check_type;
@synthesize progress_level;
@end
