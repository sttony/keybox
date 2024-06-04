
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
- (instancetype)init:(uint32_t)_type;
- (bool)initWithKey:(NSData *)vKey iv:(NSData *)vIV;
- (NSMutableData*)getNextBytes:(uint32_t)num;
- (uint32_t)getNextInt;
@end



#endif /* ORandomGenerator_h */
