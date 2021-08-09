import Foundation

func bridgeRetained<T : AnyObject>(obj : T) -> UnsafeRawPointer {
    return UnsafeRawPointer(Unmanaged.passRetained(obj).toOpaque())
}

func bridgeTransfer<T : AnyObject>(ptr : UnsafeRawPointer) -> T {
    return Unmanaged<T>.fromOpaque(ptr).takeRetainedValue()
}


@objc(CallbackHandler)
public class CallbackHandler: NSObject {
    
    @objc
    init(userdata: UnsafeRawPointer){}
    
    @objc
    public func trakingCallback(data: TrackingCallbackData) {}
    
    @objc
    public func matchFoundCallback(data: MatchFoundCallbackData) {}
    
}
