#ifndef OPasswordGenerator_h
#define OPasswordGenerator_h
#import <Foundation/Foundation.h>
#import "ORandomGenerator.h"

NS_ASSUME_NONNULL_BEGIN

@interface OPasswordGenerator : NSObject

/// Designated initializer using an ORandomGenerator as entropy source
- (instancetype)initWithRandom:(ORandomGenerator*)random;

// Flags
- (void)setLower:(BOOL)enabled; - (BOOL)getLower;
- (void)setUpper:(BOOL)enabled; - (BOOL)getUpper;
- (void)setDigit:(BOOL)enabled; - (BOOL)getDigit;
- (void)setMinus:(BOOL)enabled; - (BOOL)getMinus;
- (void)setAdd:(BOOL)enabled; - (BOOL)getAdd;
- (void)setShift1_8:(BOOL)enabled; - (BOOL)getShift1_8;
- (void)setBrace:(BOOL)enabled; - (BOOL)getBrace;
- (void)setSpace:(BOOL)enabled; - (BOOL)getSpace;
- (void)setQuestion:(BOOL)enabled; - (BOOL)getQuestion;
- (void)setSlash:(BOOL)enabled; - (BOOL)getSlash;
- (void)setGreaterLess:(BOOL)enabled; - (BOOL)getGreaterLess;

// Length
- (void)setLength:(int)length;
- (int)getLength;

// Generate a password string based on current configuration
- (NSString*)generate;

@end

NS_ASSUME_NONNULL_END

#endif /* OPasswordGenerator_h */
