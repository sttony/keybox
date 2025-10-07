#import "OCKBFile.h"
#import "OPwdEntry.h"

#import <vector>
#import <string>

// Include C++ headers
#import "../../utilities/CKBFile.h"
#import "../../utilities/CPwdEntry.h"
#import "../../utilities/CPwdGroup.h"

@implementation OCKBFile {
    CKBFile *_file;
}

- (instancetype)init {
    if (self = [super init]) {
        _file = new CKBFile();
    }
    return self;
}

- (void)dealloc {
    delete _file;
}

- (void)setMasterKey:(NSData *)key onePad:(NSData *)onePad {
    std::vector<unsigned char> kBytes((const unsigned char *)key.bytes,
                                      (const unsigned char *)key.bytes + key.length);
    std::vector<unsigned char> padBytes((const unsigned char *)onePad.bytes,
                                        (const unsigned char *)onePad.bytes + onePad.length);
    _file->SetMasterKey(kBytes, std::move(padBytes));
}

- (uint32_t)deserializeFromData:(NSData *)data {
    uint32_t realSize = 0;
    return _file->Deserialize((const unsigned char *)data.bytes, (uint32_t)data.length, realSize);
}

- (NSData *)serializeToData:(uint32_t *_Nullable)outError {
    uint32_t needed = 0;
    uint32_t err = _file->Serialize(nullptr, 0, needed);
    if (err && err != ERROR_BUFFER_TOO_SMALL) {
        if (outError) { *outError = err; }
        return nil;
    }
    std::vector<unsigned char> buff;
    buff.resize(needed);
    uint32_t written = 0;
    err = _file->Serialize(buff.data(), (uint32_t)buff.size(), written);
    if (err) {
        if (outError) { *outError = err; }
        return nil;
    }
    return [NSData dataWithBytes:buff.data() length:written];
}

- (NSArray<NSDictionary<NSString *, id> *> *)groups {
    NSMutableArray *arr = [NSMutableArray array];
    const auto &gs = _file->GetGroups();
    for (const auto &g : gs) {
        NSString *gid = [NSString stringWithUTF8String:g.GetID().c_str()];
        NSString *name = [NSString stringWithUTF8String:g.GetName().c_str()];
        [arr addObject:@{ @"id": gid, @"name": name }];
    }
    return arr;
}

// Helper to convert CPwdEntry -> OPwdEntry by copying fields
- (OPwdEntry *)makeObjcEntryFrom:(const CPwdEntry &)src {
    OPwdEntry *oe = [[OPwdEntry alloc] init];
    [oe setTitle:[NSString stringWithUTF8String:src.GetTitle().c_str()]];
    [oe setUsername:[NSString stringWithUTF8String:src.GetUserName().c_str()]];
    [oe setUrl:[NSString stringWithUTF8String:src.GetUrl().c_str()]];
    // Password and note: use zero pad with same length
    NSString *pwd = [NSString stringWithUTF8String:src.GetPassword().c_str()];
    NSData *pwdPad = [NSData dataWithBytesNoCopy:calloc(pwd.length, 1) length:pwd.length freeWhenDone:true];
    [oe setPassword:pwd onePad:pwdPad];

    NSString *note = [NSString stringWithUTF8String:src.GetNote().c_str()];
    NSData *notePad = [NSData dataWithBytesNoCopy:calloc(note.length, 1) length:note.length freeWhenDone:true];
    [oe setNote:note onePad:notePad];

    // Group UUID
    NSUUID *nsuuid = [[NSUUID alloc] initWithUUIDString:[NSString stringWithUTF8String:boost::uuids::to_string(src.GetGroup()).c_str()]];
    if (nsuuid) {
        [oe setGroupUUID:nsuuid];
    }
    return oe;
}

- (NSArray<OPwdEntry *> *)entries {
    NSMutableArray *arr = [NSMutableArray array];
    const auto &es = _file->GetEntries();
    for (const auto &e : es) {
        [arr addObject:[self makeObjcEntryFrom:e]];
    }
    return arr;
}

- (NSArray<OPwdEntry *> *)entriesForGroupId:(NSUUID *)groupId {
    unsigned char buf[16];
    [groupId getUUIDBytes:buf];
    boost::uuids::uuid gid;
    memcpy(gid.data(), buf, 16);

    NSMutableArray *arr = [NSMutableArray array];
    const auto &es = _file->GetEntries();
    for (const auto &e : es) {
        if (e.GetGroup() == gid) {
            [arr addObject:[self makeObjcEntryFrom:e]];
        }
    }
    return arr;
}

@end
