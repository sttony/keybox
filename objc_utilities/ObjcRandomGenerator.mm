#include "ObjcRandomGenerator.h"
using namespace std;
@implementation ObjRandomGenerator

- (instancetype)initWithType:(uint32_t)type {
    _m_type = type;
    return self;
}

- (bool)initWithKey:(NSData *)vKey iv:(NSData *)vIV error:(NSError **)error {
    if( [vIV length] != 8){
        return false;
    }
    delete _m_rg;
    _m_rg = new CRandomGenerator(_m_type);
    vector<unsigned char> key;
    key.reserve([vKey length]);
    [vKey getBytes:key.data() length:[vKey length]];

    array<unsigned char, 8> IV = {0xe8, 0x30, 0x09, 0x4b, 0x97, 0x20, 0x5d, 0x2a};
    [vIV getBytes:&IV[0] length:[vIV length]];
    return _m_rg->init(key, IV);
}

- (NSMutableData*)getNextBytes:(uint32_t)num{
    vector<unsigned char> buff( num);
    _m_rg->GetNextBytes(num, buff);
    NSMutableData *data = [NSMutableData dataWithCapacity:num];
    return [NSMutableData dataWithBytes:buff.data() length:num];
}
@end