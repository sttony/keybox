#import "OPwdGroup.h"
#import <Foundation/Foundation.h>

#include <string>
#include <vector>
#include <cstring>

#import "../../utilities/CPwdGroup.h"

@implementation OPwdGroup {
    CPwdGroup *_group;
}

// Initialize with a name (creates a new UUID inside CPwdGroup)
- (instancetype)initWithName:(NSString *)name {
    if (self = [super init]) {
        std::string n([name UTF8String]);
        _group = new CPwdGroup(std::move(n));
    }
    return self;
}

// Initialize with a name and explicit UUID
- (instancetype)initWithName:(NSString *)name uuid:(NSUUID *)uuid {
    if (self = [super init]) {
        std::string n([name UTF8String]);
        unsigned char buf[16];
        [uuid getUUIDBytes:buf];
        boost::uuids::uuid bUuid;
        std::memcpy(bUuid.data(), buf, 16);
        _group = new CPwdGroup(std::move(n), bUuid);
    }
    return self;
}

- (void)dealloc {
    delete _group;
}

// Get and set name
- (NSString *)getName {
    return [NSString stringWithUTF8String:_group->GetName().c_str()];
}

- (void)setName:(NSString *)name {
    _group->SetName([name UTF8String]);
}

// UUID accessors
- (NSUUID *)getUUID {
    auto uid = _group->GetUUID();
    return [[NSUUID alloc] initWithUUIDBytes:uid.data];
}

// ID string (UUID string)
- (NSString *)getID {
    return [NSString stringWithUTF8String:_group->GetID().c_str()];
}

// Root group helpers
+ (NSUUID *)rootGroupUUID {
    auto uid = g_RootGroupId;
    return [[NSUUID alloc] initWithUUIDBytes:uid.data];
}

+ (NSString *)rootGroupID {
    return [NSString stringWithUTF8String:g_RootGroup.GetID().c_str()];
}

@end
