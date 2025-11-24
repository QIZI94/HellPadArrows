#include <Arduino.h>
#include "decompression.h"

namespace compression{

	Option<uint8_t> DecompressionIterator::next(){
		uint8_t currentByte = 0;
        uint8_t bitsFilled = 0;
        /// implement reading next byte from compressed input respecting the bits filled
        //for(; compressedIt != compressedEnd; ++compressedIt) {
		while(compressedIt != compressedEnd){
			if(firstRunAndBitsToSkip.firstRun){
				currentCompressedByte = CompressedByte::from(pgm_read_byte(compressedIt)); 
				firstRunAndBitsToSkip.firstRun = false;
			}
            for(uint8_t i = firstRunAndBitsToSkip.bitsToSkip; i < currentCompressedByte.count; ++i) {
                currentByte <<= 1;
                currentByte |= currentCompressedByte.isOne;
                bitsFilled++;
                if(bitsFilled == 8) {
                    firstRunAndBitsToSkip.bitsToSkip = (i + 1);
                    bitsFilled = 0;
                    return Some(currentByte);
                }
            }
            firstRunAndBitsToSkip.bitsToSkip = 0;
			compressedIt++;
			currentCompressedByte = CompressedByte::from(pgm_read_byte(compressedIt));
        }
        return None<uint8_t>();

	}
	
	Option<uint8_t> DecompressionTerminatedIterator::next(){
		uint8_t currentByte = 0;
        uint8_t bitsFilled = 0;
        /// implement reading next byte from compressed input respecting the bits filled
        //for(; compressedIt != compressedEnd; ++compressedIt) {
		while(true){
			if(firstRunAndBitsToSkip.firstRun){
				currentCompressedByte = CompressedByte::from(pgm_read_byte(compressedIt)); 

				firstRunAndBitsToSkip.firstRun = false;
			}

			if(currentCompressedByte.isNull()){
				break;
			}

            for(uint8_t i = firstRunAndBitsToSkip.bitsToSkip; i < currentCompressedByte.count; ++i) {
                currentByte <<= 1;
                currentByte |= currentCompressedByte.isOne;
                bitsFilled++;
                if(bitsFilled == 8) {
                    firstRunAndBitsToSkip.bitsToSkip = (i + 1);
                    bitsFilled = 0;
                    return Some(currentByte);
                }
            }
            firstRunAndBitsToSkip.bitsToSkip = 0;
			compressedIt++;
			currentCompressedByte = CompressedByte::from(pgm_read_byte(compressedIt));
        }
        return None<uint8_t>();

	}


	Option<uint8_t> DecompressionIteratorUnchecked::next(){
		uint8_t currentByte = 0;
        uint8_t bitsFilled = 0;
        /// implement reading next byte from compressed input respecting the bits filled
        //for(; compressedIt != compressedEnd; ++compressedIt) {
		while(true){
			if(firstRunAndBitsToSkip.firstRun){
				currentCompressedByte = CompressedByte::from(pgm_read_byte(compressedIt)); 
				firstRunAndBitsToSkip.firstRun = false;
			}
            for(uint8_t i = firstRunAndBitsToSkip.bitsToSkip; i < currentCompressedByte.count; ++i) {
                currentByte <<= 1;
                currentByte |= currentCompressedByte.isOne;
                bitsFilled++;
                if(bitsFilled == 8) {
                    firstRunAndBitsToSkip.bitsToSkip = (i + 1);
                    bitsFilled = 0;
                    return Some(currentByte);
                }
            }
            firstRunAndBitsToSkip.bitsToSkip = 0;
			compressedIt++;
			currentCompressedByte = CompressedByte::from(pgm_read_byte(compressedIt));
        }
        return None<uint8_t>();
	}
	
} // namespace compression
