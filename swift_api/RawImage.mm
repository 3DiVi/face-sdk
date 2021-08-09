#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#import "FacerecService.h"

#import "RawImage.h"
#include "pbio/RawImage.h"

#import "TryCatch.mm"
#include <iostream>


@implementation Rectangle

-(nonnull instancetype) init: (int)X :(int)Y : (int)Width : (int)Height{
    x = X;
    y = Y;
    width = Width;
    height = Height;
    return  self;
}
@synthesize x;
@synthesize y;
@synthesize width;
@synthesize height;

@end


@implementation RawImage
@synthesize raw_image;
-(nonnull instancetype) init{
    
    return self;
}

-(nonnull instancetype) init:(int)width :(int)height :(int)format : (const unsigned char *)data{
    raw_image = tryCatch(new pbio::RawImage(pbio::RawImage(width, height, pbio::RawImage::Format(format), data)));
    
    return self;
}

-(nonnull instancetype) init: (CMSampleBufferRef)sampleBuffer : (int) format{
    
    
    CVImageBufferRef imgBuf = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVImageBufferIsFlipped(imgBuf);
    
    // lock the buffer
    CVPixelBufferLockBaseAddress(imgBuf, 0);

    // get the address to the image data
    void *imgBufAddr = CVPixelBufferGetBaseAddressOfPlane(imgBuf, 0);

    int w = (int)CVPixelBufferGetWidth(imgBuf);
    int h = (int)CVPixelBufferGetHeight(imgBuf);
    static unsigned char * data = new unsigned char[w * h * 3];
    static unsigned char * data2 = new unsigned char[w * h * 4];
    
    memcpy(data2, imgBufAddr, w * h * 4);
    int j = 0;
    int i = 0;
    while (i < w * h * 3){
        if (j % 4 != 0){
            data[i++] = data2[j];
        }
        j++;
    }

    CVPixelBufferUnlockBaseAddress(imgBuf, 0);
    
    raw_image = tryCatch(new pbio::RawImage(w, h, pbio::RawImage::Format::FORMAT_BGR, (const unsigned char*)data));
    return self;
}

-(nonnull instancetype)init: (void*) image{
    raw_image = image;
    return self;
}

-(int) width{
    pbio::RawImage image = (*(pbio::RawImage*)raw_image);
    return image.width;
}

-(int) height{
    pbio::RawImage image = (*(pbio::RawImage*)raw_image);
    return image.height;
}

-(const unsigned char*) data{
    pbio::RawImage image = (*(pbio::RawImage*)raw_image);
    return image.data;
}

-(void) dealloc{
    delete static_cast<pbio::RawImage*>(raw_image);
}

-(nonnull RawImage *) crop:(Rectangle *)rec{
    pbio::RawImage image = (*(pbio::RawImage*)raw_image);
    
    pbio::RawImage crop_image = tryCatch(image.crop(pbio::Rectangle(rec.x, rec.y, rec.height, rec.width)));
    
    return [[RawImage alloc] init: new pbio::RawImage(crop_image)];
}

@end
