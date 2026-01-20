
#ifndef OPwdEntry_h
#define OPwdEntry_h
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface OPwdEntry : NSObject

/// Initialize the wrapper
- (instancetype)init;

/// Initialize with existing C++ CPwdEntry pointer
- (instancetype)initWithCppEntry:(void *)cppEntry;

/**
 * Get the underlying C++ CPwdEntry object
 * @return Copy of the C++ CPwdEntry object
 */

- (void*)getCppEntry;

/// Get and  the ID
- (NSUUID*) getID;

/// Get and set the title
- (NSString *)getTitle;
- (void)setTitle:(NSString *)title;

/// Get and set the username
- (NSString *)getUsername;
- (void)setUsername:(NSString *)username;

/// Get and set the URL
- (NSString *)getUrl;
- (void)setUrl:(NSString *)url;

/// Get the password and set a new password with one-time pad
- (NSString *)getPassword;
- (void)setPassword:(NSString *)plainPassword onePad:(NSData *)onePad;

/// Get the note and set a new note with one-time pad
- (NSString *)getNote;
- (void)setNote:(NSString *)plainNote onePad:(NSData *)onePad;

/// Get and set the group UUID
- (NSUUID*) getGroupUUID;
- (void)setGroupUUID:(NSUUID*)uuid;



@end

NS_ASSUME_NONNULL_END



#endif /* OPwdEntry_h */
