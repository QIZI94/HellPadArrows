#ifndef DECOMPRESSION_H
#define DECOMPRESSION_H
//#include <stddef.h>

#include "shared/compression_defs.h"
#include "shared/optional.h"

namespace compression{



class DecompressionIterator{
public:
    constexpr DecompressionIterator(const CompressedByte* compressedBegin, const CompressedByte* compressedEnd) : compressedIt(compressedBegin), compressedEnd(compressedEnd), currentCompressedByte({}) {}
    Option<uint8_t> next();
private:
    const CompressedByte* compressedIt;
    const CompressedByte* compressedEnd;
	FirstRunAndBitsToSkip firstRunAndBitsToSkip;
	CompressedByte currentCompressedByte;
};

class DecompressionTerminatedIterator{
public:
    constexpr DecompressionTerminatedIterator(const CompressedByte* compressedBegin) : compressedIt(compressedBegin), currentCompressedByte({}) {}
    Option<uint8_t> next();
private:
    const CompressedByte* compressedIt;
	FirstRunAndBitsToSkip firstRunAndBitsToSkip;
	CompressedByte currentCompressedByte;
};



class DecompressionIteratorUnchecked{
public:
    constexpr DecompressionIteratorUnchecked(const CompressedByte* compressedBegin) : compressedIt(compressedBegin), currentCompressedByte() {}
    Option<uint8_t> next();
private:
    const CompressedByte* compressedIt;
	FirstRunAndBitsToSkip firstRunAndBitsToSkip;
	CompressedByte currentCompressedByte;
};



struct CompressedImage{
	using iterator = DecompressionIterator;

	template<uint16_t N>
	constexpr CompressedImage(const CompressedImageBufferStorage<false, N>& compressedImageBuffer)
	 : 
	compressedStorage(compressedImageBuffer.reinterpretStorage()),
	compressedImageEnd(&compressedStorage->compressedImage[N])
	{}

	constexpr CompressedImage(uint8_t* p = nullptr)
	 : 
	compressedStorage(nullptr),
	compressedImageEnd(nullptr)
	{}

	inline DecompressionIterator iterate() const {
		return DecompressionIterator(compressedStorage->compressedImage, compressedImageEnd);
	}

	inline DecompressionIteratorUnchecked iterateUnchecked() const {
		return DecompressionIteratorUnchecked(compressedStorage->compressedImage);
	}

	const ReinterpretImageBufferStorage* compressedStorage;
	const CompressedByte* compressedImageEnd;
};

struct CompressedImageTerminated{
	using iterator = DecompressionTerminatedIterator;
	

	template<uint16_t N>
	constexpr CompressedImageTerminated(const CompressedImageBufferStorage<true, N>& compressedImageBuffer)
	 : 	compressedStorage(compressedImageBuffer.reinterpretStorage())
	{}
	constexpr CompressedImageTerminated(uint8_t* p = nullptr)
	 : 	compressedStorage(nullptr)
	{}

	inline DecompressionTerminatedIterator iterate() const {
		return DecompressionTerminatedIterator(compressedStorage->compressedImage);
	}

	inline DecompressionIteratorUnchecked iterateUnchecked() const {
		return DecompressionIteratorUnchecked(compressedStorage->compressedImage);
	}

	 
	const ReinterpretImageBufferStorage* compressedStorage;
};

struct CompressedImageUnchecked{
	using iterator = DecompressionIteratorUnchecked;

	template<bool IS_NULL_TERMINATED, uint16_t N>
	constexpr CompressedImageUnchecked(const CompressedImageBufferStorage<IS_NULL_TERMINATED, N>& compressedImageBuffer)
	 : 	compressedStorage(compressedImageBuffer.reinterpretStorage())
	{}
	constexpr CompressedImageUnchecked(uint8_t* p = nullptr)
	 : 	compressedStorage(nullptr)
	{}

	inline DecompressionIteratorUnchecked iterate() const {
		return DecompressionIteratorUnchecked(compressedStorage->compressedImage);
	}
	inline DecompressionIteratorUnchecked iterateUnchecked() const {
		return DecompressionIteratorUnchecked(compressedStorage->compressedImage);
	}
	 
	const ReinterpretImageBufferStorage* compressedStorage;
};

} // compression


#endif