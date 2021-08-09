#import <Foundation/Foundation.h>
#import "RawSample.h"
#import "Template.h"
#import "Recognizer.h"

typedef NS_ENUM(NSInteger, Verdict){
    VERDICT_PASSED = 0,
    VERDICT_FAILED = 1,
    VERDICT_NOT_COMPUTED = 2
};

typedef NS_ENUM(NSInteger, CheckType){
    NONE = 0,
    SMILE = 1,
    BLINK = 2,
    TURN_UP = 3,
    TURN_DOWN = 4,
    TURN_RIGHT = 5,
    TURN_LEFT = 6,
    PERSPECTIVE = 7
};

typedef NS_ENUM(NSInteger, Liveness){
    ALL_CHECKS_PASSED = 0,
    CURRENT_CHECK_PASSED = 1,
    CHECK_FAIL = 2,
    WAITING_FACE_ALIGN = 3,
    IN_PROGRESS = 4,
    NOT_COMPUTED = 5,
};


typedef  NS_ENUM(NSInteger, Age){
    AGE_KIT = 0,
    AGE_YONG = 1,
    AGE_ADULT = 2,
    AGE_SENIOR = 3
};

typedef  NS_ENUM(NSInteger, Gender){
    GENDER_FEMALE = 0,
    GENDER_MALE = 1,
};

typedef  NS_ENUM(NSInteger, Emotion){
    EMOTION_NEUTRAL = 0,
    EMOTION_HAPPY = 1,
    EMOTION_ANGRY = 2,
    EMOTION_SURPRISE = 3
};

@interface ActiveLivenessStatus : NSObject{
    Liveness verdict;
    CheckType check_type;
    float progress_level;
}

-(nonnull instancetype) init;
-(nonnull instancetype) init: (void* _Nullable) active_liveness;

@property Liveness verdict;
@property CheckType check_type;
@property float progress_level;
@end

@interface AgeGender : NSObject
{
    Age age;
    Gender gender;
    float age_year;
}

-(nonnull instancetype) init: (void* _Nullable) age_gender;

@property Age age;
@property Gender gender;
@property float age_year;
@end

@interface EmotionsConfidence : NSObject
{
    float confidence;
    Emotion emotion;
    
}
-(nonnull instancetype) init: (void* _Nullable) emotions_confidence;
@property Emotion emotion;
@property float confidence;
@end

@interface SearchResult : NSObject
{
    UInt64 person_id;
    UInt64 element_id;
    MatchResult *match_result;
}

-(nonnull instancetype) init: (void* _Nonnull) search_result;

@property UInt64 person_id;
@property UInt64 element_id;
@property (readonly, nonnull) MatchResult *match_result;
@end

//======TrackingCallbackData=====
@interface TrackingCallbackData : NSObject
{
    void *trackin_callback;
    int64_t stream_id;
    int64_t frame_id;
    NSMutableArray<RawSample *> *samples; //raw_samples
    NSMutableArray *samples_track_id; //int
    NSMutableArray *samples_week; //bool
    NSMutableArray *samples_quality; //float
    NSMutableArray *samples_good_light_and_blur; //Verdict
    NSMutableArray *samples_good_andles; //Verdict
    NSMutableArray *samples_good_face_size; //Verdict
    NSMutableArray *samples_detector_confirmed; //Verdict
    NSMutableArray *samples_depth_liveness_confirmed;
    NSMutableArray *samples_track_age_gender_set; //bool
    NSMutableArray *samples_track_age_gender; //AgeGender
    NSMutableArray *samples_track_emotions_set; //bool
    NSMutableArray *samples_track_emotions; //EmptionConfidence
    NSMutableArray *samples_active_liveness_status;
    
}
-(nonnull instancetype) init;
-(nonnull instancetype) init: (void *_Nonnull)callback;
-(void) dealloc;

@property (readonly) int64_t stream_id;
@property (readonly) int64_t frame_id;
@property (readonly, nonnull) NSMutableArray<RawSample *> *samples; //raw_samples
@property (readonly, nonnull) NSMutableArray *samples_track_id; //int
@property (readonly, nonnull) NSMutableArray *samples_week; //bool
@property (readonly, nonnull) NSMutableArray *samples_quality; //float
@property (readonly, nonnull) NSMutableArray *samples_good_light_and_blur; //Verdict
@property (readonly, nonnull) NSMutableArray *samples_good_andles; //Verdict
@property (readonly, nonnull) NSMutableArray *samples_good_face_size; //Verdict
@property (readonly, nonnull) NSMutableArray *samples_detector_confirmed; //Verdict
@property (readonly, nonnull) NSMutableArray *samples_depth_liveness_confirmed;
@property (readonly, nonnull) NSMutableArray *samples_track_age_gender_set; //bool
@property (readonly, nonnull) NSMutableArray *samples_track_age_gender; //AgeGender
@property (readonly, nonnull) NSMutableArray *samples_track_emotions_set; //bool
@property (readonly, nonnull) NSMutableArray *samples_track_emotions; //EmptionConfidence
@property (readonly, nonnull) NSMutableArray *samples_active_liveness_status;
@end


@interface MatchFoundCallbackData : NSObject
{
    void* match_found;
    
    int64_t stream_id;
    int64_t frame_id;
    RawSample *sample;
    double quality;
    Template *templ;
    NSMutableArray *search_results;
}
-(nonnull instancetype) init: (void* _Nonnull)callback;
-(void) dealloc;

@property (readonly) int64_t stream_id;
@property (readonly) int64_t frame_id;
@property (readonly, nonnull) RawSample *sample;
@property (readonly) double quality;
@property (readonly, nonnull) Template *templ;
@property (readonly, nonnull) NSMutableArray *search_results;
@end
