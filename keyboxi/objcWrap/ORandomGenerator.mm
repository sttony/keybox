#import <Foundation/Foundation.h>
#include "ORandomGenerator.h"
#import "../../utilities/CRandomGenerator.h"
#import "../../utilities/InitGlobalRG.h"

// Ensure InitGlobalRG is compiled into this translation unit if needed, 
// or at least available to the linker.
#include "../../utilities/InitGlobalRG_mac.cpp"

using namespace std;

const int kSalsa20Type = 0x01;

@implementation ORandomGenerator {
    CRandomGenerator* m_rg;
    BOOL _configured;
    uint32_t _configuredType;
    BOOL _isShared;
}

+ (instancetype)shared {
    static ORandomGenerator *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] initShared];
    });
    return instance;
}

- (instancetype)initShared {
    self = [super init];
    if (self) {
        m_rg = &g_RG;
        _configured = YES;
        _configuredType = kSalsa20Type;
        _isShared = YES;
    }
    return self;
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
    // If it's the shared instance, we don't allow changing the type for now as g_RG is already initialized.
    if (_isShared) {
        return type == kSalsa20Type;
    }

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
        _isShared = NO;
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
    if (!_isShared && m_rg != nullptr) {
        delete m_rg;
    }
    m_rg = nullptr;
}

@end
