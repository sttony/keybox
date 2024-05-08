#include "utilities/CRandomGenerator.h"
#import "utilities/Salsa20Cipher.h"
#import <Foundation/Foundation.h>

const int kSalsa20Type = 0x01;
@protocol IRandomGeneratorProtocol <NSObject>
@required
- (NSMutableData*)getNextBytes:(uint32_t)num;
@end

@interface ObjRandomGenerator : NSObject <IRandomGeneratorProtocol>
@property uint32_t m_type;
@property CRandomGenerator* m_rg;
- (instancetype)initWithType:(uint32_t)_type;
- (bool)initWithKey:(NSData *)vKey iv:(NSData *)vIV error:(NSError **)error;
- (NSMutableData*)getNextBytes:(uint32_t)num;
@end