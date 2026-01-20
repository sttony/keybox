
//
//  OCKBFile.mm
//  keyboxi
//

#import "OCKBFile.h"
#import "OPwdEntry.h"
#import "OPwdGroup.h"
#import "OCKBFileHeader.h"

#import "../../utilities/CKBFile.h"
#import "../../utilities/CPwdEntry.h"
#import "../../utilities/CPwdGroup.h"
#import "../../utilities/CMaskedBlob.h"

NSString *const OCKBFileErrorDomain = @"OCKBFileErrorDomain";

typedef NS_ENUM(NSInteger, OCKBFileErrorCode) {
    OCKBFileErrorDeserializationFailed = 1,
    OCKBFileErrorSerializationFailed = 2,
    OCKBFileErrorInvalidInput = 3,
    OCKBFileErrorOperationFailed = 4,
    OCKBFileErrorIndexOutOfRange = 5,
    OCKBFileErrorEntryNotFound = 6,
    OCKBFileErrorRemoteSyncFailed = 7
};

@interface OCKBFile ()

@property (nonatomic, assign) CKBFile *cppFile;
@property (nonatomic, strong) OCKBFileHeader *header;

@end

@implementation OCKBFile

- (instancetype)init {
    self = [super init];
    if (self) {
        _cppFile = new CKBFile();
        _header = [[OCKBFileHeader alloc] initWithCppHeader:&(_cppFile->GetHeader())];
    }
    return self;
}

- (void)dealloc {
    if (_cppFile != nullptr) {
        delete _cppFile;
        _cppFile = nullptr;
    }
}

- (BOOL)deserializeFromBuffer:(NSData *)buffer error:(NSError **)error {
    if (!buffer || buffer.length == 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid buffer input"}];
        }
        return NO;
    }
    
    uint32_t realSize = 0;
    uint32_t result = _cppFile->Deserialize(
        (const unsigned char *)buffer.bytes,
        (uint32_t)buffer.length,
        realSize
    );
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorDeserializationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to deserialize KBF file"}];
        }
        return NO;
    }
    
    return YES;
}

- (BOOL)loadHeaderFromBuffer:(NSData *)buffer error:(NSError **)error {
    if (!buffer || buffer.length == 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid buffer input"}];
        }
        return NO;
    }
    
    uint32_t realSize = 0;
    uint32_t result = _cppFile->LoadHeader(
        (const unsigned char *)buffer.bytes,
        (uint32_t)buffer.length,
        realSize
    );
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorDeserializationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to load header"}];
        }
        return NO;
    }
    
    return YES;
}

- (BOOL)loadPayloadFromBuffer:(NSData *)buffer error:(NSError **)error {
    if (!buffer || buffer.length == 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid buffer input"}];
        }
        return NO;
    }
    
    uint32_t realSize = 0;
    uint32_t result = _cppFile->LoadPayload(
        (const unsigned char *)buffer.bytes,
        (uint32_t)buffer.length,
        realSize
    );
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorDeserializationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to load payload"}];
        }
        return NO;
    }
    
    return YES;
}

- (NSData *)serializeWithError:(NSError **)error {
    // Estimate buffer size - adjust if needed
    uint32_t bufferSize = 65536;
    unsigned char *buffer = new unsigned char[bufferSize];
    
    uint32_t realSize = 0;
    uint32_t result = _cppFile->Serialize(buffer, bufferSize, realSize);
    
    NSData *resultData = nil;
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorSerializationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to serialize KBF file"}];
        }
    } else {
        resultData = [NSData dataWithBytes:buffer length:realSize];
    }
    
    delete[] buffer;
    return resultData;
}

- (NSData *)lockWithError:(NSError **)error {
    uint32_t bufferSize = 65536;
    unsigned char *buffer = new unsigned char[bufferSize];
    
    uint32_t realSize = 0;
    uint32_t result = _cppFile->Lock(buffer, bufferSize, realSize);
    
    NSData *resultData = nil;
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorOperationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to lock KBF file"}];
        }
    } else {
        resultData = [NSData dataWithBytes:buffer length:realSize];
    }
    
    delete[] buffer;
    return resultData;
}

- (BOOL)addEntry:(OPwdEntry *)entry error:(NSError **)error {
    if (!entry) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid entry"}];
        }
        return NO;
    }
    
    CPwdEntry* cppEntry = (CPwdEntry*)[entry getCppEntry];
    uint32_t result = _cppFile->AddEntry(*cppEntry);
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorOperationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to add entry"}];
        }
        return NO;
    }
    
    return YES;
}

- (OPwdEntry *)getEntryAtIndex:(NSUInteger)index error:(NSError **)error {
    if (index >= _cppFile->GetEntries().size()) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorIndexOutOfRange
                                    userInfo:@{NSLocalizedDescriptionKey: @"Entry index out of range"}];
        }
        return nil;
    }
    
    CPwdEntry cppEntry = _cppFile->GetEntry((int)index);
    OPwdEntry *objcEntry = [[OPwdEntry alloc] initWithCppEntry:new CPwdEntry(cppEntry)];
    
    return objcEntry;
}

- (NSArray<OPwdEntry *> *)getAllEntries {
    NSMutableArray<OPwdEntry *> *entries = [NSMutableArray array];
    
    for (const auto &cppEntry : _cppFile->GetEntries()) {
        OPwdEntry *objcEntry = [[OPwdEntry alloc] initWithCppEntry:new CPwdEntry(cppEntry)];
        [entries addObject:objcEntry];
    }
    
    return [entries copy];
}

- (BOOL)updateEntry:(OPwdEntry *)entry error:(NSError **)error {
    if (!entry) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid entry"}];
        }
        return NO;
    }
    
    CPwdEntry* cppEntry = (CPwdEntry*)[entry getCppEntry];
    uint32_t result = _cppFile->SetEntry(*cppEntry);
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorOperationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to update entry"}];
        }
        return NO;
    }
    
    return YES;
}

- (BOOL)removeEntryWithUUID:(NSUUID *)entryUUID error:(NSError **)error {
    if (!entryUUID) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid UUID"}];
        }
        return NO;
    }
    
    unsigned char buf[16];
    [entryUUID getUUIDBytes:buf];
    boost::uuids::uuid bUuid;
    memcpy(bUuid.data(), buf, 16);
    
    uint32_t result = _cppFile->RemoveEntry(bUuid);
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorOperationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to remove entry"}];
        }
        return NO;
    }
    
    return YES;
}

- (OPwdEntry *)queryEntryByTitle:(NSString *)title {
    if (!title) {
        return nil;
    }
    
    CPwdEntry cppEntry = _cppFile->QueryEntryByTitle([title UTF8String]);
    OPwdEntry *objcEntry = [[OPwdEntry alloc] initWithCppEntry:new CPwdEntry(cppEntry)];
    
    return objcEntry;
}

- (OCKBFileHeader *)getHeader {
    if (!_header) {
        _header = [[OCKBFileHeader alloc] initWithCppHeader:&(_cppFile->GetHeader())];
    }
    return _header;
}

- (void)setMasterKeyWithKey:(NSData *)key onePad:(NSData *)onePad {
    if (!key || !onePad) {
        return;
    }
    
    std::vector<unsigned char> cppKey((const unsigned char *)key.bytes,
                                      (const unsigned char *)key.bytes + key.length);
    std::vector<unsigned char> cppOnePad((const unsigned char *)onePad.bytes,
                                         (const unsigned char *)onePad.bytes + onePad.length);
    
    _cppFile->SetMasterKey(cppKey, std::move(cppOnePad));
}

- (BOOL)setDerivativeParametersWithSalt:(NSData *)salt
                                  rounds:(int)numRounds
                                  error:(NSError **)error {
    if (!salt || salt.length == 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid salt input"}];
        }
        return NO;
    }
    
    std::vector<unsigned char> cppSalt((const unsigned char *)salt.bytes,
                                       (const unsigned char *)salt.bytes + salt.length);
    uint32_t result = _cppFile->SetDerivativeParameters(cppSalt, numRounds);
    
    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorOperationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to set derivative parameters"}];
        }
        return NO;
    }

    return YES;
}

- (NSArray<OPwdGroup *> *)getAllGroups {
    NSMutableArray<OPwdGroup *> *groups = [NSMutableArray array];

    for (const auto &cppGroup : _cppFile->GetGroups()) {
        OPwdGroup *objcGroup = [[OPwdGroup alloc] initWithCppGroup:new CPwdGroup(cppGroup)];
        [groups addObject:objcGroup];
    }

    return [groups copy];
}

- (BOOL)removeGroupWithUUID:(NSString *)uuidString error:(NSError **)error {
    if (!uuidString || uuidString.length == 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid UUID string"}];
        }
        return NO;
    }

    uint32_t result = _cppFile->RemoveGroup([uuidString UTF8String]);

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorOperationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to remove group"}];
        }
        return NO;
    }

    return YES;
}

- (BOOL)updateGroupWithUUID:(NSString *)uuidString name:(NSString *)name error:(NSError **)error {
    if (!uuidString || !name || uuidString.length == 0 || name.length == 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"Invalid UUID or name"}];
        }
        return NO;
    }

    uint32_t result = _cppFile->UpdateGroup([uuidString UTF8String], [name UTF8String]);

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorOperationFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to update group"}];
        }
        return NO;
    }

    return YES;
}

- (BOOL)retrieveFromRemoteWithMessage:(NSString **)outMessage error:(NSError **)error {
    std::string message;
    uint32_t result = _cppFile->RetrieveFromRemote(message);

    if (outMessage) {
        *outMessage = [NSString stringWithUTF8String:message.c_str()];
    }

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorRemoteSyncFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to retrieve from remote"}];
        }
        return NO;
    }

    return YES;
}

- (BOOL)retrieveFromRemoteWithError:(NSError **)error {
    uint32_t result = _cppFile->RetrieveFromRemote();

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorRemoteSyncFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to retrieve from remote"}];
        }
        return NO;
    }

    return YES;
}

- (BOOL)pushToRemoteWithMessage:(NSString **)outMessage error:(NSError **)error {
    std::string message;
    uint32_t result = _cppFile->PushToRemote(message);

    if (outMessage) {
        *outMessage = [NSString stringWithUTF8String:message.c_str()];
    }

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorRemoteSyncFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to push to remote"}];
        }
        return NO;
    }

    return YES;
}

- (BOOL)pushToRemoteWithError:(NSError **)error {
    uint32_t result = _cppFile->PushToRemote();

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorRemoteSyncFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to push to remote"}];
        }
        return NO;
    }

    return YES;
}

- (BOOL)registerWithMessage:(NSString **)outMessage error:(NSError **)error {
    std::string message;
    uint32_t result = _cppFile->Register(message);

    if (outMessage) {
        *outMessage = [NSString stringWithUTF8String:message.c_str()];
    }

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorRemoteSyncFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to register"}];
        }
        return NO;
    }

    return YES;
}

- (BOOL)registerWithError:(NSError **)error {
    uint32_t result = _cppFile->Register();

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorRemoteSyncFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to register"}];
        }
        return NO;
    }

    return YES;
}

- (BOOL)setupNewClientWithUrl:(NSData **)outUrl
                        message:(NSString **)outMessage
                          error:(NSError **)error {
    if (!outUrl) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"outUrl pointer required"}];
        }
        return NO;
    }

    std::vector<unsigned char> url;
    std::string message;
    uint32_t result = _cppFile->SetupNewClient(url, message);

    *outUrl = [NSData dataWithBytes:url.data() length:url.size()];
    if (outMessage) {
        *outMessage = [NSString stringWithUTF8String:message.c_str()];
    }

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorRemoteSyncFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to setup new client"}];
        }
        return NO;
    }

    return YES;
}

- (BOOL)setupNewClientWithUrl:(NSData **)outUrl error:(NSError **)error {
    if (!outUrl) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorInvalidInput
                                    userInfo:@{NSLocalizedDescriptionKey: @"outUrl pointer required"}];
        }
        return NO;
    }

    std::vector<unsigned char> url;
    uint32_t result = _cppFile->SetupNewClient(url);

    *outUrl = [NSData dataWithBytes:url.data() length:url.size()];

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:OCKBFileErrorDomain
                                        code:OCKBFileErrorRemoteSyncFailed
                                    userInfo:@{NSLocalizedDescriptionKey: @"Failed to setup new client"}];
        }
        return NO;
    }

    return YES;
}

@end
