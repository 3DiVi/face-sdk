#import <Foundation/Foundation.h>
#import "RawImage.h"
#import "Point.h"

typedef  NS_ENUM(int32_t, SampleType){
    SAMPLE_TYPE_FRONTAL = 0,
    SAMPLE_TYPE_LEFT_PROFILE = 1,
    SAMPLE_TYPE_RIGHT_PROFILE = 2,
};

typedef  NS_ENUM(int32_t, ImageFromat){
    IMAGE_FORMAT_JPG = 0,
    IMAGE_FORMAT_PNG = 1,
    IMAGE_FORMAT_TIF = 2,
    IMAGE_FORMAT_BMP = 3,
};

typedef  NS_ENUM(int32_t, FaceCutType){
    FACE_CUT_BASE = 0,
    FACE_CUT_FULL_FRONTAL = 1,
    FACE_CUT_TOKEN_FRONTAL = 2,
};

@interface Angles : NSObject
{
    float yaw;
    float pitch;
    float roll;
}

-(nonnull instancetype) init: (float) yaw : (float) pitch : (float) roll;
@property float yaw;
@property float pitch;
@property float roll;
@end

@interface RawSample : NSObject
{
    void *raw_sample;
}
-(nonnull instancetype)init: (void* _Nonnull) sample;
-(void) dealloc;

-(nonnull Rectangle *) getRectangle;
-(nonnull NSMutableArray *) getLandmarks;
-(nonnull Point2 *) getLeftEye;
-(nonnull Point2 *) getRightEye;
-(nonnull Angles *) getAngles;
-(int) getID;
-(int) getFrameID;
-(bool) hasOriginalImage;
-(SampleType) getType;
-(nonnull RawImage *) cutFaceRawImage;
-(void) save: (const char*_Nullable) file_path;
-(nonnull NSMutableArray *) getFaceCutRectangle: (FaceCutType) cut_type;


@property (readonly, nonnull) void* raw_sample;
@end
