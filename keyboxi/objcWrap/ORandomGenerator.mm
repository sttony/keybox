#import <Foundation/Foundation.h>
#include "ORandomGenerator.h"
#import "../../utilities/CRandomGenerator.h"

using namespace std;

@implementation ORandomGenerator {
    CRandomGenerator* m_rg;
    BOOL _configured;
    uint32_t _configuredType;
}

+ (instancetype)shared {
    static ORandomGenerator *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        // Default construct with a sensible type; you can change via -configureWithType:
        instance = [[self alloc] init:kSalsa20Type];
    });
    return instance;
}

- (NSMutableData *)getNextBytes:(uint32_t)num {
    vector<unsigned char> buff;
    m_rg->GetNextBytes(num, buff);
    NSMutableData *data = [NSMutableData dataWithBytes:buff.data() length:buff.size()];
    return data;
}

- (uint32_t)getNextInt {
    return m_rg->GetNextInt32();
}

// MARK: - Configuration

- (BOOL)configureWithType:(uint32_t)type {
    // If already configured with the same type, treat as success.
    if (_configured && _configuredType == type) {
        return YES;
    }
    // Reinitialize the underlying generator.
    if (m_rg != nullptr) {
        delete m_rg;
        m_rg = nullptr;
    }
    m_rg = new CRandomGenerator(type);
    _configured = YES;
    _configuredType = type;
    return YES;
}

- (BOOL)configureWithKey:(NSData *)vKey iv:(NSData *)vIV {
    if (m_rg == nullptr) {
        // If not initialized yet, default to a type first.
        m_rg = new CRandomGenerator(kSalsa20Type);
    }

    std::vector<unsigned char> key;
    key.resize([vKey length]);
    std::memcpy(key.data(), [vKey bytes], [vKey length]);

    std::array<unsigned char, 8> IV;
    if ([vIV length] < IV.size()) {
        // Reject invalid IV sizes to avoid UB.
        return NO;
    }
    std::memcpy(IV.data(), [vIV bytes], IV.size());
    m_rg->init(key, IV);

    _configured = YES;
    return YES;
}

// MARK: - Legacy initializers (non-singleton instances)

- (instancetype)init:(uint32_t)_type {
    self = [super init];
    if (self) {
        m_rg = new CRandomGenerator(_type);
        _configured = YES;
        _configuredType = _type;
    }
    return self;
}

- (bool)initWithKey:(NSData *)vKey iv:(NSData *)vIV {
    if (m_rg == nullptr) {
        // default type
        m_rg = new CRandomGenerator(kSalsa20Type);
    }

    std::vector<unsigned char> key;
    key.resize([vKey length]);
    std::memcpy(key.data(), [vKey bytes], [vKey length]);

    std::array<unsigned char, 8> IV;
    if ([vIV length] < IV.size()) {
        return false;
    }
    std::memcpy(IV.data(), [vIV bytes], IV.size());
    m_rg->init(key, IV);
    _configured = YES;
    return true;
}

- (void)dealloc {
    delete m_rg;
    m_rg = nullptr;
}

@end
