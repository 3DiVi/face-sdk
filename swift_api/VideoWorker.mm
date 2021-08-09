#import <Foundation/Foundation.h>
#import "VideoWorker.h"
#include "pbio/VideoWorker.h"

#import "TryCatch.mm"

void trakingCallbackU(const pbio::VideoWorker::TrackingCallbackData &data, void *const userdata){
    VideoWorker *t1 = (__bridge VideoWorker*)userdata;
    
    TrackingCallbackData *new_data = [[TrackingCallbackData alloc] init: new pbio::VideoWorker::TrackingCallbackData(data)];

    [t1.handler trakingCallbackWithData: new_data];
}

void matchFoundCallbackU(const pbio::VideoWorker::MatchFoundCallbackData &data, void *const userdata){
    VideoWorker *t1 = (__bridge VideoWorker*)userdata;
    
    MatchFoundCallbackData *new_data = [[MatchFoundCallbackData alloc] init:new pbio::VideoWorker::MatchFoundCallbackData(data)];
    
    [t1.handler matchFoundCallbackWithData: new_data];
}

@implementation VideoWorker

-(id) init: (void*) worker{
    video_worker = worker;
    return self;
}

-(void) dealloc{
    delete static_cast<pbio::VideoWorker::Ptr*>(video_worker);
}

-(int) addMatchFoundCallbackU:(const void *)callback :(void *const)userdata{
    pbio::VideoWorker::Ptr videoWorker = (*(pbio::VideoWorker::Ptr*)video_worker);
    pbio::VideoWorker::MatchFoundCallbackU data = (*(pbio::VideoWorker::MatchFoundCallbackU*)callback);
    
    return tryCatch(videoWorker->addMatchFoundCallbackU(data, userdata));
}

-(int) addTrackingCallbackU:(const void *)callback :(void *const)userdata{
    pbio::VideoWorker::Ptr videoWorker = (*(pbio::VideoWorker::Ptr*)video_worker);
    pbio::VideoWorker::TrackingCallbackU data = (*(pbio::VideoWorker::TrackingCallbackU*)callback);
        
    return tryCatch(videoWorker->addTrackingCallbackU(data, userdata));
}

-(void) addHandlerCallback:(CallbackHandler *)handler{
    
    self.handler = handler;

    [self addTrackingCallbackU: new pbio::VideoWorker::TrackingCallbackU(trakingCallbackU) : (__bridge void*)self];
    
    [self addMatchFoundCallbackU: new pbio::VideoWorker::MatchFoundCallbackU(matchFoundCallbackU) : (__bridge void*)self];
}

-(int) addVideoFrame: (RawImage*) image : (int) stream_id{
    const std::string resources_dir = [[[NSBundle mainBundle] resourcePath] UTF8String];
    
    pbio::VideoWorker::Ptr videoWorker = (*(pbio::VideoWorker::Ptr*)video_worker);
    pbio::RawImage raw_image = (*(pbio::RawImage*)image.raw_image);

    return tryCatch(videoWorker->addVideoFrame(raw_image, stream_id));
}

-(void) enableProcessingOnStream:(const int)stream_id{
    pbio::VideoWorker::Ptr videoWorker = (*(pbio::VideoWorker::Ptr*)video_worker);
    
    tryCatch(videoWorker->enableProcessingOnStream(stream_id));
}

-(void) disableProcessingOnStream:(const int)stream_id{
    pbio::VideoWorker::Ptr videoWorker = (*(pbio::VideoWorker::Ptr*)video_worker);
    
    tryCatch(videoWorker->disableProcessingOnStream(stream_id));
}

-(void) resetStream:(const int)stream_id{
    pbio::VideoWorker::Ptr videoWorker = (*(pbio::VideoWorker::Ptr*)video_worker);
    
    tryCatch(videoWorker->resetStream(stream_id));
}

-(void) setDatabase:(NSArray<DatabaseElement *> *)elements{
    pbio::VideoWorker::Ptr videoWorker = (*(pbio::VideoWorker::Ptr*)video_worker);
    
    std::vector<pbio::VideoWorker::DatabaseElement> vector_elements;
    
    for(DatabaseElement* elem : elements){
        pbio::VideoWorker::DatabaseElement buff = (*(pbio::VideoWorker::DatabaseElement*)elem.base_element);
        vector_elements.push_back(buff);
    }

    tryCatch(videoWorker->setDatabase(vector_elements));
    
}
-(void) checkException{
    pbio::VideoWorker::Ptr videoWorker = (*(pbio::VideoWorker::Ptr*)video_worker);
    
    tryCatch(videoWorker->checkExceptions());
}

@synthesize handler;
@end

@implementation DatabaseElement
@synthesize base_element;
@synthesize element_id;
@synthesize person_id;
@synthesize face_template;
@synthesize distance_treshold;

-(id) init:(void *)element{
    base_element = element;
    return self;
}
-(id) init:(u_int64_t)elem_id :(u_int64_t)pers_id :(Template *)face_temp :(float)distance{
    distance_treshold = distance;
    element_id = elem_id;
    person_id = pers_id;
    face_template = face_temp;
    
    pbio::Template::Ptr temp = (*(pbio::Template::Ptr*)face_temp._template);
    
    pbio::VideoWorker::DatabaseElement elem;
    elem.distance_threshold = distance_treshold;
    elem.person_id = person_id;
    elem.element_id = element_id;
    elem.face_template = temp;
    
    base_element = new pbio::VideoWorker::DatabaseElement(elem);
    
    return self;
    
}

-(void) dealloc{
    void (*del_func)(void*);
    del_func = [](void *p){delete static_cast<pbio::VideoWorker::DatabaseElement*>(p);};
    del_func(base_element);
}

@end
