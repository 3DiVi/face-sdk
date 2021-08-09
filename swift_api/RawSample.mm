#import <Foundation/Foundation.h>
#import "RawSample.h"
#include "pbio/RawSample.h"
#include <fstream>
#include <iostream>

#import "TryCatch.mm"

@implementation RawSample
@synthesize raw_sample;


-(nonnull instancetype)init:(void *)sample{
    
    raw_sample = sample;
    return self;
}

-(void) dealloc{
    delete static_cast<pbio::RawSample::Ptr*>(raw_sample);
}

-(nonnull Rectangle *) getRectangle{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
    pbio::Rectangle rec = tryCatch(sample->getRectangle());


    return [[Rectangle alloc] init:rec.x :rec.y :rec.width :rec.height];
}

-(nonnull NSMutableArray *) getLandmarks{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
    std::vector<pbio::Point> points = tryCatch(sample->getLandmarks());
    
    NSMutableArray *result = [NSMutableArray new];
    
    for (auto p : points){
        Point2 *buf = [[Point2 alloc] init:p.x :p.y :p.z];
        [result addObject:buf];
    }
    
    return result;
}

-(nonnull Point2 *) getLeftEye{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
    pbio::Point p = tryCatch(sample->getLeftEye());
    
    return [[Point2 alloc] init:p.x :p.y :p.y];
}

-(nonnull Point2 *) getRightEye{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
    pbio::Point p = tryCatch(sample->getRightEye());

    return [[Point2 alloc] init:p.x :p.y :p.y];
}

-(nonnull Angles *) getAngles{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
    pbio::RawSample::Angles angles = tryCatch(sample->getAngles());
    
    return [[Angles alloc] init: angles.yaw :angles.pitch :angles.roll];
}

//========save
-(void) save: (const char *)file_path{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
    
    std::ofstream stream(file_path, std::ios_base::binary);
    
    if (stream.is_open()){
        tryCatch(sample->save(stream, pbio::RawSample::IMAGE_FORMAT_JPG));
        stream.close();
    }
}


-(nonnull RawImage *) cutFaceRawImage{
    
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
    pbio::RawImage image = sample->cutFaceRawImage(pbio::RawImage::Format::FORMAT_BGR, pbio::RawSample::FaceCutType::FACE_CUT_FULL_FRONTAL);
    return [[RawImage alloc] init: new pbio::RawImage(image)];
}

-(int) getID{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
        
    return tryCatch(sample->getID());
}

-(int) getFrameID{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);

    return tryCatch(sample->getFrameID());
}

-(bool) hasOriginalImage{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
        
    return tryCatch(sample->hasOriginalImage());
}

-(SampleType) getType{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
        
    return tryCatch(SampleType(sample->getType()));
}

-(nonnull NSMutableArray *) getFaceCutRectangle: (FaceCutType) cut_type{
    pbio::RawSample::Ptr sample = (*(pbio::RawSample::Ptr*)raw_sample);
    std::vector<pbio::Point> points = tryCatch(sample->getFaceCutRectangle(pbio::RawSample::FaceCutType(cut_type)));

    
    NSMutableArray *result = [NSMutableArray new];
    
    for (auto p : points){
        Point2 *buf = [[Point2 alloc] init:p.x :p.y :p.z];
        [result addObject:buf];
    }
    
    return result;
}


@end

@implementation Angles

-(nonnull instancetype) init: (float)yaw : (float)pitch : (float)roll{
    self.yaw = yaw;
    self.pitch = pitch;
    self.roll = roll;
    return self;
}
@synthesize yaw;
@synthesize pitch;
@synthesize roll;
@end
