#import "FaceSdkPlugin.h"
#if __has_include(<face_sdk_3divi/face_sdk_3divi-Swift.h>)
#import <face_sdk_3divi/face_sdk_3divi-Swift.h>
#else
// Support project import fallback if the generated compatibility header
// is not copied when this plugin is created as a library.
// https://forums.swift.org/t/swift-static-libraries-dont-copy-generated-objective-c-header/19816
#import "face_sdk_3divi-Swift.h"
#endif

@implementation FaceSdkPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftFaceSdkPlugin registerWithRegistrar:registrar];
}
@end
