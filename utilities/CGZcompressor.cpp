//
// Created by tongl on 12/15/2023.
//

#include "CGZcompressor.h"

uint32_t CGZcompressor::decompressData(const unsigned char *pCompressedData,
                                       size_t cbCompressedDataSize,
                                       unsigned char *pUncompressedData,
                                       size_t cbUncompressedDataSize) {
    m_zs.next_in = (Bytef *) pCompressedData;
    m_zs.avail_in = static_cast<uInt>(cbCompressedDataSize);

    do {
        m_zs.next_out = reinterpret_cast<Bytef *>(pUncompressedData);
        m_zs.avail_out = cbUncompressedDataSize;

        auto status = inflate(&m_zs, Z_SYNC_FLUSH);
        if (status == Z_STREAM_ERROR) {
            inflateEnd(&m_zs);
            return -1;
        }

        size_t decompressedSize = cbUncompressedDataSize - m_zs.avail_out;
        // Process or save the decompressed data as needed
    } while (m_zs.avail_out == 0);

    inflateEnd(&m_zs);
    return 0;
}
