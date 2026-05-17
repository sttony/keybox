
//
//  OCipherEngine.h
//  keyboxi
//

#ifndef OCipherEngine_h
#define OCipherEngine_h

#import <Foundation/Foundation.h>

@interface OCipherEngine : NSObject

/**
 * AES256 Decrypt using the CCipherEngine
 * @param data The encrypted data
 * @param key The 32-byte key
 * @param iv The 16-byte IV
 * @param error Pointer to NSError for error information
 * @return NSData containing decrypted data, nil if failed
 */
- (NSData *)decryptData:(NSData *)data
                    key:(NSData *)key
                     iv:(NSData *)iv
                  error:(NSError **)error;

/**
 * Convert hex string to NSData
 * @param hexString The hex string to convert
 * @return NSData containing the bytes, nil if invalid hex string
 */
- (NSData *)dataFromHexString:(NSString *)hexString;

@end

#endif /* OCipherEngine_h */
