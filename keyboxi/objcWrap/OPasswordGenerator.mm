#import "OPasswordGenerator.h"
#import "OPwdEntry.h" // ensure Objective-C++ mode
#import <vector>
#import <string>

// C++ includes
#import "../../utilities/CPasswordGenerator.h"
#import "../../utilities/InitGlobalRG.h"

@implementation OPasswordGenerator {
    CPasswordGenerator* _gen;
}

- (instancetype)initWithRandom:(ORandomGenerator*)random {
    self = [super init];
    if (self) {
        // Use global random generator like desktop code does
        _gen = new CPasswordGenerator(g_RG);
        // defaults already set in C++ ctor
    }
    return self;
}

- (void)dealloc {
    delete _gen;
}

// Flags
- (void)setLower:(BOOL)enabled { _gen->SetLower(enabled); }
- (BOOL)getLower { return _gen->GetLower(); }

- (void)setUpper:(BOOL)enabled { _gen->SetUpper(enabled); }
- (BOOL)getUpper { return _gen->GetUpper(); }

- (void)setDigit:(BOOL)enabled { _gen->SetDigit(enabled); }
- (BOOL)getDigit { return _gen->GetDigit(); }

- (void)setMinus:(BOOL)enabled { _gen->SetMinus(enabled); }
- (BOOL)getMinus { return _gen->GetMinus(); }

- (void)setAdd:(BOOL)enabled { _gen->SetAdd(enabled); }
- (BOOL)getAdd { return _gen->GetAdd(); }

- (void)setShift1_8:(BOOL)enabled { _gen->SetShift1_8(enabled); }
- (BOOL)getShift1_8 { return _gen->GetShift1_8(); }

- (void)setBrace:(BOOL)enabled { _gen->SetBrace(enabled); }
- (BOOL)getBrace { return _gen->GetBrace(); }

- (void)setSpace:(BOOL)enabled { _gen->SetSpace(enabled); }
- (BOOL)getSpace { return _gen->GetSpace(); }

- (void)setQuestion:(BOOL)enabled { _gen->SetQuestion(enabled); }
- (BOOL)getQuestion { return _gen->GetQuestion(); }

- (void)setSlash:(BOOL)enabled { _gen->SetSlash(enabled); }
- (BOOL)getSlash { return _gen->GetSlash(); }

- (void)setGreaterLess:(BOOL)enabled { _gen->SetGreaterLess(enabled); }
- (BOOL)getGreaterLess { return _gen->GetGreaterLess(); }

// Length
- (void)setLength:(int)length { _gen->SetLength(length); }
- (int)getLength { return _gen->GetLength(); }

// Generate
- (NSString*)generate {
    std::string s = _gen->GeneratePassword();
    return [NSString stringWithUTF8String:s.c_str()];
}

@end
