#ifndef COMPRESSION_DEFS_H
#define COMPRESSION_DEFS_H
#include <inttypes.h>
namespace compression{
struct Size {
	uint8_t width;
	uint8_t height;

	bool operator ==(const Size& other){
		return width == other.width && height == other.height;
	}
	bool operator !=(const Size& other){
		return width != other.width || height != other.height;
	}
};



struct CompressedByte{
    uint8_t isOne : 1;
    uint8_t count : 7;

	inline bool isNull(){
		return *((uint8_t*)this) == 0;
	}

	static constexpr CompressedByte makeDefault(){
		return CompressedByte{.isOne = 0, .count = 0};
	}

	static constexpr CompressedByte makeNull(){
		return makeDefault();
	}

	static constexpr CompressedByte from(uint8_t value){
		return *((CompressedByte*)&value);
	}
};

struct FirstRunAndBitsToSkip{
	constexpr FirstRunAndBitsToSkip() : firstRun(1), bitsToSkip(0) {}
	uint8_t firstRun : 1;
   	uint8_t bitsToSkip : 7;
};


struct ReinterpretImageBufferStorage{
	constexpr static uint16_t FAKE_SIZE = 0;
	ReinterpretImageBufferStorage() = delete;
	Size size;
	CompressedByte compressedImage[FAKE_SIZE];
};

template<bool IS_NULL_TERMINATED, uint16_t COMPRESSED_SIZE>
struct CompressedImageBufferStorage{


	constexpr CompressedImageBufferStorage(Size size, const uint8_t (&compressedImage)[COMPRESSED_SIZE])
	: size(size), compressedImage(compressedImage)
	 {}
	constexpr CompressedImageBufferStorage()
	: size({}), compressedImage({})
	 {}
	constexpr static uint16_t compressedSize() {
		return COMPRESSED_SIZE;
	}

	constexpr static bool isNullTerminated() {
		return IS_NULL_TERMINATED;
	}

	constexpr const ReinterpretImageBufferStorage* reinterpretStorage() const {
		return reinterpret_cast<const ReinterpretImageBufferStorage*>(this);
	}
	
	Size size;
	CompressedByte compressedImage[COMPRESSED_SIZE];
};



} // cimg
#endif