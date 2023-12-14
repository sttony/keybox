//
// Created by tongl on 12/15/2023.
//

#ifndef KEYBOX_CGZCOMPRESSOR_H
#define KEYBOX_CGZCOMPRESSOR_H

#include <zlib.h>
#include <vector>

class CGZcompressor {
private:
    z_stream m_zs = {};
    std::vector<unsigned char> m_buff;
public:
    CGZcompressor(): m_buff(4096){
    }

    uint32_t reset(){
        return inflateInit2(&m_zs, 16 + MAX_WBITS);
    }

    uint32_t decompressData(const unsigned char* pCompressedData,
                            size_t cbCompressedDataSize,
                            unsigned char* pUncompressedData,
                            size_t cbUncompressedDataSize);
};


#endif //KEYBOX_CGZCOMPRESSOR_H
