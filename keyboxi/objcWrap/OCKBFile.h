
//
//  OCKBFile.h
//  keyboxi
//

#ifndef OCKBFile_h
#define OCKBFile_h

#import <Foundation/Foundation.h>
@class OPwdEntry;
@class OPwdGroup;
@class OCKBFileHeader;

/**
 * Objective-C wrapper for CKBFile
 * Manages password database operations including serialization, entries, and groups
 */
@interface OCKBFile : NSObject

/**
 * Deserialize KBF file from buffer
 * @param buffer The buffer containing serialized file data
 * @param error Pointer to NSError for error information
 * @return YES if deserialization succeeded, NO otherwise
 */
- (BOOL)deserializeFromBuffer:(NSData *)buffer error:(NSError **)error;

/**
 * Load just the header from buffer
 * @param buffer The buffer containing file data
 * @param error Pointer to NSError for error information
 * @return YES if header loaded successfully, NO otherwise
 */
- (BOOL)loadHeaderFromBuffer:(NSData *)buffer error:(NSError **)error;

/**
 * Load the payload (entries/groups) from buffer
 * @param buffer The buffer containing payload data
 * @param error Pointer to NSError for error information
 * @return YES if payload loaded successfully, NO otherwise
 */
- (BOOL)loadPayloadFromBuffer:(NSData *)buffer error:(NSError **)error;

/**
 * Serialize KBF file to buffer
 * @param error Pointer to NSError for error information
 * @return NSData containing serialized file, nil if serialization failed
 */
- (NSData *)serializeWithError:(NSError **)error;

/**
 * Lock and serialize file to buffer with encryption
 * @param error Pointer to NSError for error information
 * @return NSData containing locked/encrypted file, nil if operation failed
 */
- (NSData *)lockWithError:(NSError **)error;

/**
 * Add a password entry to the file
 * @param entry The OPwdEntry to add
 * @param error Pointer to NSError for error information
 * @return YES if entry added successfully, NO otherwise
 */
- (BOOL)addEntry:(OPwdEntry *)entry error:(NSError **)error;

/**
 * Get entry at specified index
 * @param index The index of the entry
 * @param error Pointer to NSError for error information
 * @return OPwdEntry at the specified index, nil if index out of range
 */
- (OPwdEntry *)getEntryAtIndex:(NSUInteger)index error:(NSError **)error;

/**
 * Get all entries
 * @return NSArray of OPwdEntry objects
 */
- (NSArray<OPwdEntry *> *)getAllEntries;

/**
 * Update an existing entry
 * @param entry The updated OPwdEntry
 * @param error Pointer to NSError for error information
 * @return YES if entry updated successfully, NO otherwise
 */
- (BOOL)updateEntry:(OPwdEntry *)entry error:(NSError **)error;

/**
 * Remove entry by UUID
 * @param entryUUID The NSUUID of the entry to remove
 * @param error Pointer to NSError for error information
 * @return YES if entry removed successfully, NO otherwise
 */
- (BOOL)removeEntryWithUUID:(NSUUID *)entryUUID error:(NSError **)error;

/**
 * Query entry by title
 * @param title The title to search for
 * @return OPwdEntry matching the title, nil if not found
 */
- (OPwdEntry *)queryEntryByTitle:(NSString *)title;

/**
 * Get the file header
 * @return OCKBFileHeader object
 */
- (OCKBFileHeader *)getHeader;

/**
 * Set master key for encryption/decryption
 * @param key NSData containing the master key
 * @param onePad NSData containing the one-time pad
 */
- (void)setMasterKeyWithKey:(NSData *)key onePad:(NSData *)onePad;

/**
 * Set derivative parameters for key derivation
 * @param salt NSData containing the salt
 * @param numRounds Number of derivation rounds (default 60000)
 * @param error Pointer to NSError for error information
 * @return YES if successful, NO otherwise
 */
- (BOOL)setDerivativeParametersWithSalt:(NSData *)salt
                                  rounds:(int)numRounds
                                  error:(NSError **)error;

/**
 * Get all password groups
 * @return NSArray of OPwdGroup objects
 */
- (NSArray<OPwdGroup *> *)getAllGroups;

/**
 * Remove a group by UUID string
 * @param uuidString The UUID string of the group to remove
 * @param error Pointer to NSError for error information
 * @return YES if group removed successfully, NO otherwise
 */
- (BOOL)removeGroupWithUUID:(NSString *)uuidString error:(NSError **)error;

/**
 * Update a group name
 * @param uuidString The UUID string of the group
 * @param name The new group name
 * @param error Pointer to NSError for error information
 * @return YES if group updated successfully, NO otherwise
 */
- (BOOL)updateGroupWithUUID:(NSString *)uuidString name:(NSString *)name error:(NSError **)error;

/**
 * Retrieve database from remote server
 * @param outMessage Pointer to NSString for server response message
 * @param error Pointer to NSError for error information
 * @return YES if retrieval succeeded, NO otherwise
 */
- (BOOL)retrieveFromRemoteWithMessage:(NSString **)outMessage error:(NSError **)error;

/**
 * Retrieve database from remote server (backward-compatible)
 * @param error Pointer to NSError for error information
 * @return YES if retrieval succeeded, NO otherwise
 */
- (BOOL)retrieveFromRemoteWithError:(NSError **)error;

/**
 * Push database to remote server
 * @param outMessage Pointer to NSString for server response message
 * @param error Pointer to NSError for error information
 * @return YES if push succeeded, NO otherwise
 */
- (BOOL)pushToRemoteWithMessage:(NSString **)outMessage error:(NSError **)error;

/**
 * Push database to remote server (backward-compatible)
 * @param error Pointer to NSError for error information
 * @return YES if push succeeded, NO otherwise
 */
- (BOOL)pushToRemoteWithError:(NSError **)error;

/**
 * Register with remote server
 * @param outMessage Pointer to NSString for server response message
 * @param error Pointer to NSError for error information
 * @return YES if registration succeeded, NO otherwise
 */
- (BOOL)registerWithMessage:(NSString **)outMessage error:(NSError **)error;

/**
 * Register with remote server (backward-compatible)
 * @param error Pointer to NSError for error information
 * @return YES if registration succeeded, NO otherwise
 */
- (BOOL)registerWithError:(NSError **)error;

/**
 * Setup new client with remote server
 * @param outUrl Pointer to NSData for encrypted URL response
 * @param outMessage Pointer to NSString for server response message
 * @param error Pointer to NSError for error information
 * @return YES if setup succeeded, NO otherwise
 */
- (BOOL)setupNewClientWithUrl:(NSData **)outUrl
                        message:(NSString **)outMessage
                          error:(NSError **)error;

/**
 * Setup new client with remote server (backward-compatible)
 * @param outUrl Pointer to NSData for encrypted URL response
 * @param error Pointer to NSError for error information
 * @return YES if setup succeeded, NO otherwise
 */
- (BOOL)setupNewClientWithUrl:(NSData **)outUrl error:(NSError **)error;

@end

#endif /* OCKBFile_h */
