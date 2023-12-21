//
// Created by tongl on 12/12/2023.
//

#ifndef KEYBOX_KEEPASSREADER_H
#define KEYBOX_KEEPASSREADER_H

#include <vector>
#include <string>

class KBDBXHeaderField{
public:
    // constants
    static const uint8_t EndOfHeader = 0;
    static const uint8_t Comment = 1;
    static const uint8_t CipherID = 2;
    static const uint8_t CompressionFlags = 3;
    static const uint8_t MasterSeed = 4;
    static const uint8_t TransformSeed = 5;
    static const uint8_t TransformRounds = 6;
    static const uint8_t EncryptionIV = 7;
    static const uint8_t InnerRandomStreamKey = 8;
    static const uint8_t StreamStartBytes = 9;
    static const uint8_t InnerRandomStreamID = 10;
    static const uint8_t KdfParameters = 11;
    static const uint8_t PublicCustomData = 12;
    // end constants
    uint8_t btFieldID;
    uint16_t cbSize;
    std::vector<uint8_t> data;
};


class CKdbxReader{
private:
    uint32_t m_sig1=0;
    uint32_t m_sig2=0;
    union {
        uint32_t  m_nVer=0;
    };
    std::vector<KBDBXHeaderField> fields;

    uint32_t cbTotalHeaderSize=0;
public:

    uint32_t LoadHeader(FILE*);
    uint32_t ReadNextField(FILE*, bool&);

    std::vector<unsigned char> getTransformSeed();
    uint32_t  getTransformRounds();
    std::vector<unsigned char> getEncryptionIV();
    std::vector<unsigned char> getMasterSeed();
    std::vector<unsigned char> getInnerRandomStreamKey();

};


#endif //KEYBOX_KEEPASSREADER_H
