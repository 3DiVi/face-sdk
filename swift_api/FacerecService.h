#import <Foundation/Foundation.h>

#import "VideoWorker.h"
#import "Recognizer.h"
#import "Capturer.h"


//===CounterState====
@interface CounterState : NSObject
{
    NSString *name;
    int value;
    int max_value;
}
@property (nonnull) NSString *name;
@property int value;
@property int max_value;
@end

//===LicenseState===
@interface LicenseState : NSObject
{
    bool online;
    NSString *android_app_id;
    NSString *android_serial;
    NSString *ios_app_id;
    NSString *hardware_reg;
    
    NSMutableArray *counters;
}
@property bool online;
@property (nonnull) NSString *android_app_id;
@property (nonnull) NSString *android_serial;
@property (nonnull) NSString *ios_app_id;
@property (nonnull) NSString *hardware_reg;
@property (nonnull) NSMutableArray *counters;
@property (nonatomic, nonnull) NSString *ios;

@end

//===Config===
@interface Config : NSObject{
    void *config;
    const char* file_path;
}
-(nonnull instancetype) init: (const char*_Nonnull) path;
-(void) dealloc;
-(nonnull instancetype) overrideParameter: (const char*_Nonnull) parametr : (const double) value;
@property (readonly) void* _Nonnull config;
@end


@interface Params : NSObject{

    Config *video_worker_config;
    Config *recognizer_config;
    int streams_count;
    int processing_threads_count;
    int matching_threads_count;
    int age_gender_estimation_threads_count;
    int emotions_estimation_threads_count;
    NSMutableArray* active_liveness_checks_order;
}
-(nonnull instancetype) init;

@property (nonnull) Config *video_worker_config;
@property (nonnull) Config *recognizer_config;
@property int streams_count;
@property int processing_threads_count;
@property int matching_threads_count;
@property int age_gender_estimation_threads_count;
@property int emotions_estimation_threads_count;
@property NSMutableArray* _Nullable active_liveness_checks_order;

@property (readonly, nonnull) void* params;
@end


//===FacerecService===
@interface FacerecService : NSObject
{
    
    void *service;
}

-(nonnull instancetype)init;
-(nonnull instancetype)init: (const char*_Nonnull) lib_path : (const char*_Nonnull) conf_path : (const char*_Nonnull) license_path;

-(void) dealloc;

-(nonnull NSString *) getVersion;

-(nonnull Recognizer *) createRecognizer: (Config* _Nonnull) config : (const bool) processing : (const bool) matching : (const bool)processing_less_memory_consumption;

-(nonnull VideoWorker *) createVideoWorker: (Config* _Nonnull) config : (const char * _Nonnull) recognizer_ini_file : (const int) streams_count : (const int) processing_threads_count : (const int) matching_threads_count;

-(nonnull VideoWorker *) createVideoWorker: (Params *_Nonnull)params;

-(nonnull Capturer *) createCapturer: (Config* _Nonnull) config;

-(nonnull LicenseState *) getLicenseState;
@end

