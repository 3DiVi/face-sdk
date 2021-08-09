#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>


typedef  NS_ENUM(int32_t, Format){
    FORMAT_GRAY = 0,
    FORMAT_RGB = 1,
    FORMAT_BGR = 2,
    FORMAT_YUV_NV21 = 3,
    FORMAT_YUV_NV12 = 4
};


@interface Rectangle : NSObject
{
    int x;
    int y;
    int width;
    int height;
}
-(nonnull instancetype) init: (int) X : (int) Y : (int) Width : (int) Height;

@property int x;
@property int y;
@property int width;
@property int height;

@end

@interface RawImage : NSObject
{
    void *raw_image;
    int width;
    int height;
    const unsigned char * data;
}
-(nonnull instancetype) init: (int) width : (int) height : (int) format : (const unsigned char *_Nullable) data;

-(nonnull instancetype) init: (void *_Nonnull)image;
-(nonnull instancetype) init;
-(nonnull instancetype) init: (CMSampleBufferRef _Nullable)sampleBuffer  : (int) format;

-(void) dealloc;

-(nonnull RawImage*) crop: (Rectangle *_Nonnull) rec;

@property (readonly ,nonnull) void *raw_image;
@property (readonly) int width;
@property (readonly) int height;
@property (readonly, nonnull) const unsigned char * data;

@end
