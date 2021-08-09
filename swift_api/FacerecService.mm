#import <Foundation/Foundation.h>

#import "FacerecService.h"
#include "pbio/FacerecService.h"

#import "TryCatch.mm"


BOOL copyLicense(NSString *srcDir, NSString *dstDir)
{
    NSString *dstLicenseFile = [dstDir stringByAppendingPathComponent:@"3divi_face_sdk.lic"];
    NSFileManager* fileMgr = [NSFileManager defaultManager];
    if ([fileMgr fileExistsAtPath:dstLicenseFile])
    {
        NSLog(@"The license file already exists in the target directory");
        return YES;
    }

    NSError *error;
    // create destination directory
    if (![fileMgr createDirectoryAtPath:dstDir
        withIntermediateDirectories:YES
        attributes:nil
        error:&error])
    {
        NSLog(@"Create license directory error: %@", error);
        return NO;
    }

    // copy license and config files
    if (![fileMgr copyItemAtPath:[srcDir stringByAppendingPathComponent:@"3divi_face_sdk.lic"]
        toPath:dstLicenseFile
        error:&error])
    {
        NSLog(@"Copy file error: %@", error);
        return NO;
    }
    if (![fileMgr copyItemAtPath:[srcDir stringByAppendingPathComponent:@"net_params.xml"]
        toPath:[dstDir stringByAppendingPathComponent:@"net_params.xml"]
        error:&error])
    {
        NSLog(@"Copy file error: %@", error);
        return NO;
    }
    if (![fileMgr copyItemAtPath:[srcDir stringByAppendingPathComponent:@"log_params.xml"]
        toPath:[dstDir stringByAppendingPathComponent:@"log_params.xml"]
        error:&error])
    {
        NSLog(@"Copy file error: %@", error);
        return NO;
    }

    return YES;
};

@implementation FacerecService

-(nonnull instancetype) init{
    
    std::cout << "Start application" << std::endl;

    std::cout << "Copy license to writable directory" << std::endl;
    // FaceSDK requires write permission to the license directory
    // copy the license from the resource directory to the application support directory
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString *applicationSupportDirectory = [paths firstObject];
    NSString *dstLicenseDir = [applicationSupportDirectory  stringByAppendingPathComponent:@"license"];
    NSString *srcLicenseDir = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"license"];
    if(!copyLicense(srcLicenseDir, dstLicenseDir))
    {
        std::cout << "Something went wrong while copying the license to a writable location" << std::endl;
    };
    
    const std::string frameworks_dir = [[[NSBundle mainBundle] privateFrameworksPath] UTF8String];
    const std::string resources_dir = [[[NSBundle mainBundle] resourcePath] UTF8String];
    
    std::cout << "Create FacerecService" << std::endl;
    
    const pbio::FacerecService::Ptr serv = tryCatch(pbio::FacerecService::createService(
        frameworks_dir + "/facerec.framework/libfacerec.dylib",
        resources_dir + "/conf/facerec",
        [dstLicenseDir UTF8String]));
        
    service = new pbio::FacerecService::Ptr(serv);    
    
    return self;
}

-(nonnull instancetype) init:(const char *)lib_path :(const char *)conf_path :(const char *)license_path{
    
    const pbio::FacerecService::Ptr serv = tryCatch(pbio::FacerecService::createService(
        lib_path,
        conf_path,
        license_path));
        
    service = new pbio::FacerecService::Ptr(serv);
    
    return self;
}

-(void) dealloc{
    delete static_cast<pbio::FacerecService::Ptr*>(service);
}

/**
    \~English
    \brief
        Get version of face recognition library. Thread-safe.
    \return
        The version of face recognition library.
 */
-(nonnull NSString *) getVersion{
    pbio::FacerecService::Ptr s = (*(pbio::FacerecService::Ptr*)service);
        
    return tryCatch(@(s->getVersion().c_str()));
}

/**
    \~English
    \brief
        Creates a Recognizer object. Thread-safe.
    \param[in]  ini_file
        Name of the config file.
    \param[in]  processing
        Flag to toggle the Recognizer::processing method in the created recognizer.
    \param[in]  matching
        Flag to toggle the Recognizer::verifyMatch method in the created recognizer.
    \param[in]  processing_less_memory_consumption
        Flag to toggle the Recognizer::processing member function optimizations that consume a lot of RAM when creating the recognizer (see the docs).
    \return
        Created Recognizer object.
 */
-(nonnull Recognizer*) createRecognizer:(Config* _Nonnull) config : (const bool)processing : (const bool)matching : (const bool)processing_less_memory_consumption{
    
    pbio::FacerecService::Ptr s = (*(pbio::FacerecService::Ptr*)service);
    const pbio::FacerecService::Config conf = (*(pbio::Config*)config.config);
    
    return [[Recognizer alloc] init: new pbio::Recognizer::Ptr(tryCatch(s->createRecognizer(conf, processing, matching, processing_less_memory_consumption)))];
}

-(nonnull VideoWorker *) createVideoWorker: (Config* _Nonnull) config : (const char * _Nonnull) recognizer_ini_file : (const int) streams_count : (const int) processing_threads_count : (const int) matching_threads_count{
    pbio::FacerecService::Ptr s = (*(pbio::FacerecService::Ptr*)service);
    const pbio::FacerecService::Config conf = (*(pbio::Config*)config.config);
    
    return [[VideoWorker alloc] init: new pbio::VideoWorker::Ptr(tryCatch(s->createVideoWorker(conf, std::string( recognizer_ini_file), streams_count, processing_threads_count, matching_threads_count)))];
    
}

-(nonnull VideoWorker *) createVideoWorker:(Params *)params{
    pbio::FacerecService::Ptr s = (*(pbio::FacerecService::Ptr*)service);
    pbio::Config conf_vw = (*(pbio::Config*)params.video_worker_config.config);
    pbio::Config conf_reco = (*(pbio::Config*)params.recognizer_config.config);
    
    pbio::VideoWorker::Params param;
    
    param.video_worker_config(conf_vw);
    param.recognizer_config(conf_reco);
    param.processing_threads_count(params.processing_threads_count);
    param.matching_threads_count(params.matching_threads_count);
    param.streams_count(params.streams_count);
    param.age_gender_estimation_threads_count(params.age_gender_estimation_threads_count);
    param.emotions_estimation_threads_count(params.emotions_estimation_threads_count);
    
    std::vector<pbio::ActiveLiveness::CheckType> checkType;
    
    for (NSNumber *i in params.active_liveness_checks_order){
        checkType.push_back(pbio::ActiveLiveness::CheckType([i intValue]));
    }

    param.active_liveness_checks_order(checkType);
    
    return [[VideoWorker alloc] init: new pbio::VideoWorker::Ptr(tryCatch(s->createVideoWorker(param)))];
}

/**
    \~English
    \brief
        Creates a Capturer object. Type and features depend on the content of the configuration file. Thread-safe.
    \param[in]  config
        Сonfiguration file with optionally overridden parameters.
    \return
        Created Capturer object.
 */
-(nonnull Capturer *) createCapturer:(Config *) config{
    pbio::FacerecService::Ptr s = (*(pbio::FacerecService::Ptr*)service);
    const pbio::FacerecService::Config conf = (*(pbio::Config*)config.config);
    
    const pbio::Capturer::Ptr cap = tryCatch(s->createCapturer(conf));
    
    return [[Capturer alloc] init: new pbio::Capturer::Ptr(cap)];
}

/**
    \~English
    \brief
        Get the license state. Thread-safe.
    \return
        License state.
 */
-(nonnull LicenseState *) getLicenseState{
    pbio::FacerecService::Ptr s = (*(pbio::FacerecService::Ptr*)service);
    pbio::FacerecService::LicenseState license_state;
    
    license_state = tryCatch(s->getLicenseState());
    
    LicenseState *license = [LicenseState new];
    license.android_app_id = @(license_state.android_app_id.c_str());
    license.online = license_state.online;
    license.ios_app_id = @(license_state.ios_app_id.c_str());
    license.hardware_reg = @(license_state.hardware_reg.c_str());
    
    NSMutableArray *counters = [[NSMutableArray alloc] init];
    
    for (auto counter: license_state.counters){
        CounterState *counts = [CounterState new];
        counts.value = int(counter.value);
        counts.max_value = int(counter.max_value);
        counts.name = @(counter.name.c_str());
        [counters addObject:counts];
    }
    license.counters = counters;    
    
    return license;
}

@end


@implementation LicenseState

@synthesize online;
@synthesize android_serial;
@synthesize android_app_id;
@synthesize ios_app_id;
@synthesize hardware_reg;
@synthesize counters;

@end

@implementation CounterState

@synthesize name;
@synthesize value;
@synthesize max_value;

@end

@implementation Config
@synthesize config;

-(nonnull instancetype)init: (const char*) path{
    file_path = path;
    
    config = new pbio::Config(tryCatch(pbio::Config(file_path)));
    
    return self;
}

-(void) dealloc{
    delete static_cast<pbio::Config*>(config);
}

-(nonnull instancetype)overrideParameter:(const char *)parameter :(const double)value{
    pbio::Config conf = (*(pbio::Config*)config);
    
    tryCatch(conf.overrideParameter(parameter, value));
    return self;
}

@end

@implementation Params

-(nonnull instancetype) init{
    streams_count = 1;
    processing_threads_count = 0;
    matching_threads_count = 0;
    age_gender_estimation_threads_count = 0;
    emotions_estimation_threads_count = 0;
    active_liveness_checks_order = [NSMutableArray new];
    return self;
}
@synthesize video_worker_config;
@synthesize recognizer_config;
@synthesize streams_count;
@synthesize processing_threads_count;
@synthesize matching_threads_count;
@synthesize age_gender_estimation_threads_count;
@synthesize emotions_estimation_threads_count;
@synthesize active_liveness_checks_order;
@end
