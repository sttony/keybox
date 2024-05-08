#include "utilities/CMaskedBlob.h"
#import <Foundation/Foundation.h>

@interface ObjMaskedBlob : NSObject {
@public
NSData *password;

}
//// assuming this is a method in your C++ code that sets the password using string and random generator. You need to translate it into Objective-C yourself as there's no direct equivalent
//- (uint32_t)setPasswordFromString:(NSString *)plainPassword withRandomGenerator:(_IRandomGenerator *)randomGenerator;
//// assuming this is a method in your C++ code that sets the password using binary data and random generator. You need to translate it into Objective-C yourself as there's no direct equivalent
//- (uint32_t)setPasswordFromData:(NSData *)plainPassword withRandomGenerator:(_IRandomGenerator *)randomGenerator;
@end
