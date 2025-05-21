//
// Created by tongl on 12/15/2023.
//

#ifndef KEYBOX_CGZCOMPRESSOR_H
#define KEYBOX_CGZCOMPRESSOR_H

#include <zlib.h>
#include <vector>
#include <cstdint>

class CGZcompressor {
private:
    z_stream m_zs = {};

public:
    CGZcompressor() {
    }

    uint32_t decompressData(const unsigned char *pCompressedData,
                            size_t cbCompressedDataSize,
                            std::vector<unsigned char> &vUnCompressedData);

    uint32_t compressData(const unsigned char *pUncompressedData,
                          size_t cbUncompressedData,
                          std::vector<unsigned char> &vCompressedData
                          );
};


#endif //KEYBOX_CGZCOMPRESSOR_H
