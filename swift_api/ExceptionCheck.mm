#import <Foundation/Foundation.h>
#import "ExceptionCheck.h"

@implementation ExceptionCheck: NSObject

+ (BOOL)check:(__attribute__((noescape)) void(^)(void))tryBlock error:(__autoreleasing NSError **)error {
    @try {
        tryBlock();
        return YES;
    } @catch (NSException *exception) {
        *error = [[NSError alloc] initWithDomain:exception.name code: [exception.reason integerValue] userInfo:nil];
        return NO;
    }
}

@end
