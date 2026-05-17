
#import "OCipherEngine.h"
#include "../../utilities/CipherEngine.h"
#include "../../utilities/Base64Coder.h"
#include <vector>

@implementation OCipherEngine

- (NSData *)decryptData:(NSData *)data
                    key:(NSData *)key
                     iv:(NSData *)iv
                  error:(NSError **)error {
    if (!data || !key || !iv) {
        if (error) {
            *error = [NSError errorWithDomain:@"OCipherEngine" code:1 userInfo:@{NSLocalizedDescriptionKey: @"Missing input data, key, or iv"}];
        }
        return nil;
    }

    CCipherEngine engine;
    std::vector<unsigned char> vInput((const unsigned char *)[data bytes], (const unsigned char *)[data bytes] + [data length]);
    std::vector<unsigned char> vKey((const unsigned char *)[key bytes], (const unsigned char *)[key bytes] + [key length]);
    std::vector<unsigned char> vIV((const unsigned char *)[iv bytes], (const unsigned char *)[iv bytes] + [iv length]);
    std::vector<unsigned char> vOutput;

    uint32_t result = engine.AES256EnDecrypt(
        vInput.data(),
        vInput.size(),
        vKey,
        vIV,
        CCipherEngine::AES_CHAIN_MODE_CBC,
        CCipherEngine::AES_PADDING_MODE_PKCS7,
        false, // decrypt
        vOutput
    );

    if (result != 0) {
        if (error) {
            *error = [NSError errorWithDomain:@"OCipherEngine" code:result userInfo:@{NSLocalizedDescriptionKey: [NSString stringWithFormat:@"AES decryption failed with code %u", result]}];
        }
        return nil;
    }

    return [NSData dataWithBytes:vOutput.data() length:vOutput.size()];
}

- (NSData *)dataFromHexString:(NSString *)hexString {
    if (!hexString) return nil;
    
    std::vector<unsigned char> v = hex_to_bytes([hexString UTF8String]);
    return [NSData dataWithBytes:v.data() length:v.size()];
}

@end
