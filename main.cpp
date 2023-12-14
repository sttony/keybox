#include <iostream>
#include <sstream>
#include "utilities/KeepassReader.h"
#include "utilities/CipherEngine.h"
#include "utilities/CGZcompressor.h"



using namespace std;

struct Block{
    uint32_t blockIndex;
    unsigned char sha_256_hash[32];
    uint32_t block_size;
};

int main() {
    FILE* fp = nullptr;
    auto error  = fopen_s(&fp, "c:\\ttt\\sttony_copy.kdbx", "rb+");
    CKdbxReader header;
    header.LoadHeader(fp);

    CCipherEngine cipherEngine;
    vector<unsigned char> derivation_seed = header.getTransformSeed();
    uint32_t  derivation_round = header.getTransformRounds();
    vector<unsigned char> masterSeed = header.getMasterSeed();
    vector<unsigned char> primay_key;

    cipherEngine.KeepassDerivateKey(
            "TestPassword",
            derivation_seed,
            derivation_round,
            masterSeed,
            primay_key
            );

    uint32_t pay_load_offset = ftell(fp);

    fseek(fp, 0, SEEK_END);
    vector<unsigned char> buff( ftell(fp) - pay_load_offset);
    fseek(fp, pay_load_offset, SEEK_SET);
    fread(&buff[0], 1, buff.size(), fp);


    fclose(fp);
    vector<unsigned char> decrypted;
    vector<unsigned char> iv = header.getEncryptionIV();
    cipherEngine.AES256EnDecrypt(&buff[0],
                                 buff.size(),
                                 primay_key,
                                 iv,
                                 CCipherEngine::AES_CHAIN_MODE_CBC,
                                 CCipherEngine::AES_PADDING_MODE_PKCS7,
                                 false,
                                 decrypted);

    Block* block1 = (Block*)&decrypted[32];


    FILE* fp2 = fopen("c:\\ttt\\block_aaa", "wb");
    fwrite( &decrypted[sizeof(Block) +32], 1, block1->block_size, fp2);
    fclose(fp2);

    CGZcompressor gzCompressor;
    if(gzCompressor.reset()){
        cout<<"Reset zlib failed"<<endl;
    };

    vector<unsigned char> uncoompressed(1024*1024);

    gzCompressor.decompressData(&decrypted[sizeof(Block) +32], block1->block_size, &uncoompressed[0], 1024*1024);

    printf("%s", &uncoompressed[0]);
}
