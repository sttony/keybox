

#import <Foundation/Foundation.h>
#include "ORandomGenerator.h"
#import "../../utilities/CRandomGenerator.h"

using namespace std;
@implementation ORandomGenerator
{
    CRandomGenerator* m_rg;
}
- (NSMutableData *)getNextBytes:(uint32_t)num {
    vector<unsigned char> buff;
    m_rg->GetNextBytes(num, buff);
    NSMutableData *data = [NSMutableData dataWithBytes:buff.data() length:buff.size()];
    return data;
}

- (uint32_t)getNextInt { 
    return 0;
}


- (instancetype)init:(uint32_t)_type {
    self = [super init];
    if (self) {
        m_rg = new CRandomGenerator(_type);
    }
    return self;
}

- (bool)initWithKey:(NSData *)vKey iv:(NSData *)vIV {
    std::vector<unsigned char> key;
    key.resize([vKey length]);
    std::memcpy(key.data(), [vKey bytes], [vKey length]);
    
    
    std::array<unsigned char, 8> IV;
    std::memcpy(IV.data(), [vIV bytes], [vIV length]);
    m_rg->init(key, IV);
    return true;
}

-(void)dealloc {
    delete m_rg;
}

@end
