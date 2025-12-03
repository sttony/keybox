#ifndef OPwdGroup_h
#define OPwdGroup_h

#import <Foundation/Foundation.h>


NS_ASSUME_NONNULL_BEGIN

@interface OPwdGroup : NSObject

/// Initialize with a name (a new UUID will be generated in C++)
- (instancetype)initWithName:(NSString *)name;

/// Initialize with a name and an explicit UUID
- (instancetype)initWithName:(NSString *)name uuid:(NSUUID *)uuid;

/// Get and set the group's name
- (NSString *)getName;
- (void)setName:(NSString *)name;

/// Get the group's UUID
- (NSUUID *)getUUID;

/// Get the group's ID string (UUID string)
- (NSString *)getID;

/// Root group helpers
+ (NSUUID *)rootGroupUUID;
+ (NSString *)rootGroupID;

@end

NS_ASSUME_NONNULL_END

#endif /* OPwdGroup_h */
