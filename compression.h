#ifndef COMPRESSION_H
#define COMPRESSION_H
#include "shared/compression_defs.h"
namespace compression{
template<size_t N>
constexpr uint16_t computeCompressedSize(const uint8_t (&image)[N]) {
    uint16_t compressedIndex = 0;
    bool firstRun = true;
    bool lastBit = false;
    uint8_t currentBitCount = 0;

    for(uint8_t byte : image) {

        for(size_t bit = 0; bit < 8; ++bit) {
            constexpr uint8_t bitMask = 0b10000000;
            bool bitValue = (byte & (bitMask >> bit)) != 0;
            if(firstRun){
                lastBit = bitValue;
                firstRun = false;
            }
            else if(bitValue != lastBit || currentBitCount == 127) {
                compressedIndex++;
				currentBitCount = 0;
            }
            currentBitCount++;
            lastBit = bitValue;
            
        }
    }
    return compressedIndex;
}


template<int16_t WIDTH, int16_t HEIGHT, bool IS_NULL_TERMINATED, uint16_t COMPRESSED_SIZE, uint16_t BUFFER_SIZE>
constexpr CompressedImageBufferStorage<IS_NULL_TERMINATED, IS_NULL_TERMINATED ? COMPRESSED_SIZE+1 : COMPRESSED_SIZE>
compiletimeCompress1Bitmap(const uint8_t (&image)[BUFFER_SIZE]){

	constexpr uint16_t BUFFER_SIZE_BITS = BUFFER_SIZE * 8;
	static_assert((WIDTH * HEIGHT) <= BUFFER_SIZE_BITS);

	CompressedImageBufferStorage<IS_NULL_TERMINATED, IS_NULL_TERMINATED ? COMPRESSED_SIZE+1 : COMPRESSED_SIZE>
		compressedBuffer{};

	compressedBuffer.size = Size{.width = WIDTH, .height = HEIGHT};
	
	bool firstRun = true;
    bool lastBit = false;
    size_t compressedIndex = 0;
    for(uint8_t byte : image) {

        for(size_t bit = 0; bit < 8; ++bit) {
            constexpr uint8_t bitMask = 0b10000000;
            bool bitValue = (byte & (bitMask >> bit)) != 0;
            if(firstRun){
                lastBit = bitValue;
                firstRun = false;
            }
            else if(bitValue != lastBit || compressedBuffer.compressedImage[compressedIndex].count == 127) {
                compressedIndex++;
            }
            compressedBuffer.compressedImage[compressedIndex].count++;
            compressedBuffer.compressedImage[compressedIndex].isOne = bitValue ? 1 : 0;
            lastBit = bitValue;
            
        }
    }

	if constexpr(IS_NULL_TERMINATED){
		compressedBuffer.compressedImage[COMPRESSED_SIZE] = CompressedByte::makeNull();
	}
	

	return compressedBuffer;
};


#define COMPRESS_IMAGE(width, height, nullterminated, buf) compression::compiletimeCompress1Bitmap<width, height, nullterminated, compression::computeCompressedSize(buf)>(buf)

} // compression


#endif