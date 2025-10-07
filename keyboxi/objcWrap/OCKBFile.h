#ifndef OCKBFile_h
#define OCKBFile_h

#import <Foundation/Foundation.h>

@class OPwdEntry;

NS_ASSUME_NONNULL_BEGIN

/// Objective-C wrapper around the C++ CKBFile model so it can be used from Swift.
@interface OCKBFile : NSObject

/// Initialize an empty Keybox file model
- (instancetype)init;

/// Set the master key (and one-time pad) required for serialization/deserialization
- (void)setMasterKey:(NSData *)key onePad:(NSData *)onePad;

/// Deserialize the given buffer into the model. Returns 0 on success or an error code.
- (uint32_t)deserializeFromData:(NSData *)data;

/// Serialize the current model into data. If an error occurs, returns nil and writes the error code to outError if provided.
- (nullable NSData *)serializeToData:(uint32_t *_Nullable)outError;

/// List of groups. Each item is an NSDictionary with keys: @"id" (NSString UUID), @"name" (NSString)
- (NSArray<NSDictionary<NSString *, id> *> *)groups;

/// All entries as OPwdEntry wrappers
- (NSArray<OPwdEntry *> *)entries;

/// Entries for a specific group UUID
- (NSArray<OPwdEntry *> *)entriesForGroupId:(NSUUID *)groupId;

@end

NS_ASSUME_NONNULL_END

#endif /* OCKBFile_h */
