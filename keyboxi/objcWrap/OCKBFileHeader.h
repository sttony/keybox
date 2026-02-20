//
//  OCKBFileHeader.h
//  keyboxi
//

#ifndef OCKBFileHeader_h
#define OCKBFileHeader_h

#import <Foundation/Foundation.h>

@interface OCKBFileHeader : NSObject

/**
 * Deserialize file header from buffer
 * @param buffer The buffer containing serialized header data
 * @param error Pointer to NSError for error information
 * @return YES if deserialization succeeded, NO otherwise
 */
- (BOOL)deserializeFromBuffer:(NSData *)buffer error:(NSError **)error;

/**
 * Serialize file header to buffer
 * @param error Pointer to NSError for error information
 * @return NSData containing serialized header, nil if serialization failed
 */
- (NSData *)serializeWithError:(NSError **)error;

/**
 * Get the compression flag
 * @return The compression flag value
 */
- (uint32_t)getCompressFlag;

/**
 * Set the compression flag
 * @param flag The compression flag value to set
 */
- (void)setCompressFlag:(uint32_t)flag;

/**
 * Get the encryption IV (Initialization Vector)
 * @return NSData containing the IV
 */
- (NSData *)getIV;

/**
 * Set the encryption IV
 * @param iv NSData containing the IV to set
 */
- (void)setIV:(NSData *)iv;

/**
 * Get the HMAC SHA256 signature
 * @return NSData containing the HMAC signature
 */
- (NSData *)getHMACSignature;

/**
 * Set derivative parameters for key derivation
 * @param salt NSData containing the salt
 * @param numRounds Number of derivation rounds
 * @param error Pointer to NSError for error information
 * @return YES if successful, NO otherwise
 */
- (BOOL)setDerivativeParameters:(NSData *)salt
                                  rounds:(int)numRounds
                                  error:(NSError **)error;

/**
 * Calculate HMAC signature
 * @param masterKey NSData containing the master key
 * @param payload NSData containing the payload to sign
 * @param error Pointer to NSError for error information
 * @return YES if calculation succeeded, NO otherwise
 */
- (BOOL)calculateHMACWithMasterKey:(NSData *)masterKey
                            payload:(NSData *)payload
                              error:(NSError **)error;

/**
 * Get sync URL
 * @return The sync URL string
 */
- (NSString *)getSyncUrl;

/**
 * Set sync URL
 * @param syncUrl The sync URL string to set
 */
- (void)setSyncUrl:(NSString *)syncUrl;

/**
 * Get sync email
 * @return The sync email string
 */
- (NSString *)getSyncEmail;

/**
 * Set sync email
 * @param syncEmail The sync email string to set
 */
- (void)setSyncEmail:(NSString *)syncEmail;

/**
 * Initialize with an existing C++ CKBFileHeader pointer
 * @param cppHeader Pointer to an existing CKBFileHeader object
 * @return Initialized OCKBFileHeader instance
 */
- (instancetype)initWithCppHeader:(void *)cppHeader;

@end

#endif /* OCKBFileHeader_h */
