import Foundation


public func CheckException<T>(callback: () throws -> T) throws -> T {
    var returnValue: T!
    var returnError: Error?

    try ExceptionCheck.check {
        do {
            returnValue = try callback()
        } catch {
            returnError = error
        }
    }

    if let error = returnError {
        throw error
    }

    return returnValue
}
