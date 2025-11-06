#ifndef ORandomGenerator_h
#define ORandomGenerator_h
#import <Foundation/Foundation.h>

const int kSalsa20Type = 0x01;

@protocol IRandomGeneratorProtocol <NSObject>
@required
- (NSMutableData*)getNextBytes:(uint32_t)num;
- (uint32_t)getNextInt;
@end

@interface ORandomGenerator : NSObject <IRandomGeneratorProtocol>

// MARK: - Singleton
/// Global, shared instance. Thread-safe, lazily initialized.
+ (instancetype)shared;

// MARK: - Configuration
/// Configure the generator with an algorithm type (e.g., kSalsa20Type).
/// Returns YES if configuration succeeded or was already configured with the same type.
- (BOOL)configureWithType:(uint32_t)type;

/// Configure the generator with a key and IV (8 bytes).
/// Returns YES if configuration succeeded.
- (BOOL)configureWithKey:(NSData *)vKey iv:(NSData *)vIV;

// MARK: - Legacy initializers (optional to keep)
/// Initializes a dedicated instance (non-singleton) with a type.
/// Prefer using +shared and -configureWithType:/-configureWithKey:iv: for the singleton.
- (instancetype)init:(uint32_t)_type;

/// Initializes with key/iv for a dedicated instance.
/// Prefer using -configureWithKey:iv: on the singleton.
- (bool)initWithKey:(NSData *)vKey iv:(NSData *)vIV;

// MARK: - Random API
- (NSMutableData*)getNextBytes:(uint32_t)num;
- (uint32_t)getNextInt;

@end

#endif /* ORandomGenerator_h */
