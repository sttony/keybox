//
//  OCKBFileHeader.mm
//  keyboxi
//

#import "OCKBFileHeader.h"
#import "../../utilities/CKBFileHeader.h"

NSString *const OCKBFileHeaderErrorDomain = @"OCKBFileHeaderErrorDomain";

typedef NS_ENUM(NSInteger, OCKBFileHeaderErrorCode) {
    OCKBFileHeaderErrorDeserializationFailed = 1,
    OCKBFileHeaderErrorSerializationFailed = 2,
    OCKBFileHeaderErrorInvalidInput = 3,
    OCKBFileHeaderErrorOperationFailed = 4
};

@interface OCKBFileHeader ()

@property (nonatomic, assign) CKBFileHeader *cppHeader;

@end

@implementation OCKBFileHeader

- (instancetype)initWithCppHeader:(void *)cppHeader {
    self = [super init];
    if (self) {
        _cppHeader = (CKBFileHeader *)cppHeader;
    }
    return self;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        _cppHeader = new CKBFileHeader();
    }
    return self;
}

- (void)dealloc {
    if (_cppHeader != nullptr) {
        delete _cppHeader;
        _cppHeader = nullptr;
    }
}

- (BOOL)deserializeFromBuffer:(NSData *)buffer error:(NSError **)error {
    if (!buffer || buffer.length == 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileHeaderErrorDomain
                                        code:OCKBFileHeaderErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid buffer input"}];
        }
        return NO;
    }
    
    uint32_t realSize = 0;
    uint32_t result = _cppHeader->Deserialize(
        (const unsigned char *)buffer.bytes,
        (uint32_t)buffer.length,
        realSize
    );
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileHeaderErrorDomain
                                        code:OCKBFileHeaderErrorDeserializationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to deserialize file header"}];
        }
        return NO;
    }
    
    return YES;
}

- (NSData *)serializeWithError:(NSError **)error {
    // Estimate buffer size - adjust if needed
    uint32_t bufferSize = 4096;
    unsigned char *buffer = new unsigned char[bufferSize];
    
    uint32_t realSize = 0;
    uint32_t result = _cppHeader->Serialize(buffer, bufferSize, realSize);
    
    NSData *resultData = nil;
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileHeaderErrorDomain
                                        code:OCKBFileHeaderErrorSerializationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to serialize file header"}];
        }
    } else {
        resultData = [NSData dataWithBytes:buffer length:realSize];
    }
    
    delete[] buffer;
    return resultData;
}

- (uint32_t)getCompressFlag {
    return _cppHeader->GetCompressFlag();
}

- (void)setCompressFlag:(uint32_t)flag {
    _cppHeader->SetCompressFlag(flag);
}

- (NSData *)getIV {
    const std::vector<unsigned char> &iv = _cppHeader->GetIV();
    return [NSData dataWithBytes:iv.data() length:iv.size()];
}

- (void)setIV:(NSData *)iv {
    if (iv && iv.length > 0) {
        std::vector<unsigned char> cppIV((const unsigned char *)iv.bytes,
                                         (const unsigned char *)iv.bytes + iv.length);
        _cppHeader->SetIV(cppIV);
    }
}

- (NSData *)getHMACSignature {
    const std::vector<unsigned char> &signature = _cppHeader->GetHMACSignature();
    return [NSData dataWithBytes:signature.data() length:signature.size()];
}

- (BOOL)setDerivativeParametersWithSalt:(NSData *)salt
                                  rounds:(int)numRounds
                                  error:(NSError **)error {
    if (!salt || salt.length == 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileHeaderErrorDomain
                                        code:OCKBFileHeaderErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid salt input"}];
        }
        return NO;
    }
    
    std::vector<unsigned char> cppSalt((const unsigned char *)salt.bytes,
                                       (const unsigned char *)salt.bytes + salt.length);
    uint32_t result = _cppHeader->SetDerivativeParameters(cppSalt, numRounds);
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileHeaderErrorDomain
                                        code:OCKBFileHeaderErrorOperationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to set derivative parameters"}];
        }
        return NO;
    }
    
    return YES;
}

- (BOOL)calculateHMACWithMasterKey:(NSData *)masterKey
                            payload:(NSData *)payload
                              error:(NSError **)error {
    if (!masterKey || masterKey.length == 0 || !payload || payload.length == 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileHeaderErrorDomain
                                        code:OCKBFileHeaderErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid master key or payload input"}];
        }
        return NO;
    }
    
    std::vector<unsigned char> cppMasterKey((const unsigned char *)masterKey.bytes,
                                            (const unsigned char *)masterKey.bytes + masterKey.length);
    
    uint32_t result = _cppHeader->CalculateHMAC(cppMasterKey,
                                               (const unsigned char *)payload.bytes,
                                               payload.length);
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileHeaderErrorDomain
                                        code:OCKBFileHeaderErrorOperationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to calculate HMAC"}];
        }
        return NO;
    }
    
    return YES;
}

- (NSString *)getSyncUrl {
    std::string url = _cppHeader->GetSyncUrl();
    return [NSString stringWithUTF8String:url.c_str()];
}

- (void)setSyncUrl:(NSString *)syncUrl {
    if (syncUrl) {
        _cppHeader->SetSyncUrl(std::string([syncUrl UTF8String]));
    }
}

- (NSString *)getSyncEmail {
    std::string email = _cppHeader->GetSyncEmail();
    return [NSString stringWithUTF8String:email.c_str()];
}

- (void)setSyncEmail:(NSString *)syncEmail {
    if (syncEmail) {
        _cppHeader->SetSyncEmail(std::string([syncEmail UTF8String]));
    }
}

@end
