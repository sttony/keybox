#include <algorithm>
#import "OPwdEntry.h"
#import "../../utilities/CPwdEntry.h"


@implementation OPwdEntry
{
    CPwdEntry* _entry;
}

/// Initialize the wrapper with a new `CPwdEntry`
- (instancetype)init {
    if (self = [super init]) {
        _entry = new CPwdEntry();
    }
    return self;
}

- (instancetype)initWithCppEntry:(void *)cppEntry {
    self = [super init];
    if (self) {
        _entry = (CPwdEntry*)cppEntry;
    }
    return self;
}

/// Deallocate the C++ object when the Objective-C object is destroyed
- (void)dealloc {
    delete _entry;
}

/// Get ID
- (NSUUID*) getID {
    auto id = _entry->GetID();
    NSUUID *nsUuid = [[NSUUID alloc] initWithUUIDBytes:id.data];
    return nsUuid;
}


/// Get and set the title
- (NSString *)getTitle {
    return [NSString stringWithUTF8String:_entry->GetTitle().c_str()];
}
- (void)setTitle:(NSString *)title {
    _entry->SetTitle([title UTF8String]);
}

/// Get and set the username
- (NSString *)getUsername {
    return [NSString stringWithUTF8String:_entry->GetUserName().c_str()];
}
- (void)setUsername:(NSString *)username {
    _entry->SetUserName([username UTF8String]);
}

/// Get and set the URL
- (NSString *)getUrl {
    return [NSString stringWithUTF8String:_entry->GetUrl().c_str()];
}
- (void)setUrl:(NSString *)url {
    _entry->SetUrl([url UTF8String]);
}

/// Get and set the password with a one-time pad
- (NSString *)getPassword {
    return [NSString stringWithUTF8String:_entry->GetPassword().c_str()];
}
- (void)setPassword:(NSString *)plainPassword onePad:(NSData *)onePad {
    std::string password = [plainPassword UTF8String];
    std::vector<unsigned char> pad((const unsigned char *)onePad.bytes, (const unsigned char *)onePad.bytes + onePad.length);
    _entry->SetPassword(password, std::move(pad));
}

/// Get and set the note with a one-time pad
- (NSString *)getNote {
    return [NSString stringWithUTF8String:_entry->GetNote().c_str()];
}
- (void)setNote:(NSString *)plainNote onePad:(NSData *)onePad {
    std::string note = [plainNote UTF8String];
    std::vector<unsigned char> pad((const unsigned char *)onePad.bytes, (const unsigned char *)onePad.bytes + onePad.length);
    _entry->SetNote(note, std::move(pad));
}

/// Get and set the group UUID
- (NSUUID*)getGroupUUID {
    auto gid = _entry->GetGroup() ;
    NSUUID *nsUuid = [[NSUUID alloc] initWithUUIDBytes:gid.data];
    return nsUuid;
}
- (void)setGroupUUID:(NSUUID*)nsUuid {
    unsigned char buf[16];
    [nsUuid getUUIDBytes:buf];                       // fill 16 bytes

    boost::uuids::uuid bUuid;
    memcpy(bUuid.data(), buf, 16);              // copy into Boost struct
    _entry->SetGroup(bUuid);
}

@end
