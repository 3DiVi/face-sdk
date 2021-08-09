#import <Foundation/Foundation.h>

@interface Template : NSObject
{
    void *_template;
}
-(nonnull instancetype) init: (void* _Nonnull) temp;
-(void) dealloc;


-(nonnull NSString *) getMethodName;
-(void) save: (const char* _Nonnull) file_path;


@property (readonly, nonnull) void *_template;
@end
