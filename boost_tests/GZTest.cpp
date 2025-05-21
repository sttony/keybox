
#include <boost/test/unit_test.hpp>
#include "../utilities/CGZcompressor.h"
#include <string>
#include <vector>
#include <random>


// Helper function to create test data
std::vector<unsigned char> createTestData(size_t size) {
    std::vector<unsigned char> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<unsigned char>(dis(gen));
    }
    return data;
}

BOOST_AUTO_TEST_SUITE(CGZcompressorTest)

    BOOST_AUTO_TEST_CASE(TestBasicDecompression) {
        CGZcompressor compressor;

        // Test data
        const std::string original = "Hello, World! This is a test string for compression.";
        std::vector<unsigned char> input(original.begin(), original.end());

        // First compress the data
        std::vector<unsigned char> compressed; // Ensure enough space
        BOOST_REQUIRE_EQUAL(compressor.compressData(
            input.data(),
            input.size(),
            compressed
        ), 0);

        // Now test decompression
        std::vector<unsigned char> decompressed;
        BOOST_REQUIRE_EQUAL(compressor.decompressData(
            compressed.data(),
            compressed.size(),
            decompressed
        ), 0);

        // Verify the result
        std::string result(decompressed.begin(), decompressed.end());
        BOOST_CHECK_EQUAL(result, original);
    }

    BOOST_AUTO_TEST_CASE(TestLargeDataDecompression) {
        CGZcompressor compressor;

        // Create large test data (1MB)
        std::vector<unsigned char> original = createTestData(1024 * 1321);

        // Compress the data
        std::vector<unsigned char> compressed; // Most data should compress smaller
        BOOST_REQUIRE_EQUAL(compressor.compressData(
            original.data(),
            original.size(),
            compressed
        ), 0);

        // Decompress
        std::vector<unsigned char> decompressed;
        BOOST_REQUIRE_EQUAL(compressor.decompressData(
            compressed.data(),
            compressed.size(),
            decompressed
        ), 0);

        // Verify the result
        BOOST_CHECK_EQUAL_COLLECTIONS(
            original.begin(), original.end(),
            decompressed.begin(), decompressed.end()
        );
    }

    BOOST_AUTO_TEST_CASE(TestEmptyInput) {
        CGZcompressor compressor;

        std::vector<unsigned char> empty;
        std::vector<unsigned char> output;

        // Should return error for empty input
        BOOST_CHECK_NE(compressor.decompressData(
            empty.data(),
            empty.size(),
            output
        ), 0);
    }

BOOST_AUTO_TEST_SUITE_END()