
#import <Foundation/Foundation.h>
#import "Capturer.h"
#import "Template.h"
#import "CallbackData.h"
#import "RawImage.h"
#import "swift_callback.h"

@interface DatabaseElement : NSObject
{
    void *base_element;
    u_int64_t element_id;
    u_int64_t person_id;
    Template *face_template; //Temlate::Ptr
    float distance_treshold;
}
-(nonnull instancetype) init: (void* _Nullable) element;
-(nonnull instancetype) init: (u_int64_t) elem_id : (u_int64_t) pers_id : (Template *_Nullable) face_temp : (float) distance;
-(void) dealloc;

@property (readonly, nonnull)void *base_element;
@property u_int64_t element_id;
@property u_int64_t person_id;
@property float distance_treshold;
@property Template * _Nullable face_template;

@end

@interface VideoWorker : NSObject
{
    void *video_worker;
    CallbackHandler *handler;
    
}
-(nonnull instancetype) init: (void* _Nonnull) worker;
-(void) dealloc;

-(int) addTrackingCallbackU: (const void* _Nonnull)callback : (void * _Nonnull const)userdata;
-(int) addMatchFoundCallbackU: (const void* _Nonnull)callback : (void * _Nonnull const)userdata;


-(void) addHandlerCallback: (CallbackHandler * _Nonnull) handler;

-(int) addVideoFrame: (RawImage * _Nonnull) image : (int) stream_id;

-(void) setDatabase: (NSArray<DatabaseElement*>* _Nonnull) elements;

-(void) enableProcessingOnStream: (const int) stream_id;
-(void) disableProcessingOnStream: (const int) stream_id;
-(void) resetStream: (const int) stream_id;
-(void) checkException;
@property (nonnull) CallbackHandler *handler;

@end

