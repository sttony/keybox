//
// Created by tongl on 12/15/2023.
//

#include "CGZcompressor.h"

#include "error_code.h"

using namespace std;

uint32_t CGZcompressor::decompressData(const unsigned char *pCompressedData,
                                       size_t cbCompressedDataSize,
                                       vector<unsigned char> &vUnCompressedData) {
    vector<unsigned char> vBuffer( 4096);
    memset(&m_zs, 0, sizeof(m_zs));
    auto status = inflateInit2(&m_zs, 16 + MAX_WBITS);
    if (status) {
       return status;
    }

    m_zs.next_in = (Bytef *) pCompressedData;
    m_zs.avail_in = static_cast<uInt>(cbCompressedDataSize);

    do {
        m_zs.next_out = reinterpret_cast<Bytef *>(vBuffer.data());
        m_zs.avail_out = vBuffer.size();
        status = inflate(&m_zs, Z_SYNC_FLUSH);
        if (status != Z_OK && status != Z_STREAM_END) {
            inflateEnd(&m_zs);
            return status;
        }
        vUnCompressedData.insert(vUnCompressedData.end(), vBuffer.begin(), vBuffer.begin() + 4096 - m_zs.avail_out);
        if (status == Z_STREAM_END) {
            break;
        }
    } while ( m_zs.avail_in > 0);

    inflateEnd(&m_zs);
    return 0;
}

uint32_t CGZcompressor::compressData(const unsigned char *pUncompressedData, size_t cbUncompressedData,
    vector<unsigned char> &vCompressedData) {
    const size_t worstCaseSize = compressBound(cbUncompressedData) + 16;
    vCompressedData.resize(worstCaseSize);

    memset(&m_zs, 0, sizeof(m_zs));
    auto status = deflateInit2(&m_zs,
                               Z_DEFAULT_COMPRESSION,
                               Z_DEFLATED,
                               16 + MAX_WBITS,
                               8,
                               Z_DEFAULT_STRATEGY);
    if (status) {
        vCompressedData.clear();
        return status;
    }
    m_zs.next_in = (Bytef *) pUncompressedData;
    m_zs.avail_in = static_cast<uInt>(cbUncompressedData);
    m_zs.next_out = (Bytef *) (vCompressedData.data());
    m_zs.avail_out = worstCaseSize;

    do {
        status = deflate(&m_zs, Z_FINISH);
        if (status != Z_OK && status != Z_STREAM_END) {
            deflateEnd(&m_zs);
            vCompressedData.clear();
            return status;
        }
    } while (status != Z_STREAM_END && m_zs.avail_out == 0);
    vCompressedData.resize(m_zs.total_out);
    deflateEnd(&m_zs);
    return 0;
}
