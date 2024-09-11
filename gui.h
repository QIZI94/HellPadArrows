#ifndef GUI
#define GUI

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include "shared/optional.h"

namespace gui{

using Color565 = uint16_t;


struct Position {
	int16_t x;
	int16_t y;
};

struct Size {
	int16_t width;
	int16_t height;
};

struct ImageBuffer{
	const Size size;
	const uint8_t image[];
};


class Window{
private:
	struct Properties{
		constexpr Properties() : raw(0) {}
		constexpr Properties(int16_t x, int16_t y, bool isHidden, bool isProgmem)
		 : x(x), y(y), isHidden(isHidden), needsUpdate(true), isSRAM(!isProgmem) {}
		union{
			struct{
				int32_t x  : 10;
				int32_t y : 10;
				int32_t isHidden : 1;
				int32_t needsUpdate : 1;
				int32_t isSRAM : 1;
			};
		
			int32_t raw;
			
		};
	};
public:

	constexpr Window() : properties(Properties()), imageBuffer(nullptr) {}
	constexpr Window(int16_t x, int16_t y, const ImageBuffer* imageBuffer, bool isHidden = false, bool isProgmem = true)
	 : properties(Properties(x, y, isHidden, isProgmem)), imageBuffer(imageBuffer) {}

	inline void forceUpdate() {
		properties.needsUpdate = true;
	}

	inline bool needsUpdate() const {
		return properties.needsUpdate;
	}

	inline void updated() {
		properties.needsUpdate = false;
	}

	inline void setImageBuffer(const ImageBuffer* imgBuffer) {
		imageBuffer = imgBuffer;
		if(!isHidden()){
			forceUpdate();
		}
	}
	
	inline void setPosition(const Position& position) {
		
		properties.x = position.x;
		properties.y = position.y;
		if(!isHidden()){
			forceUpdate();
		}
	}

	inline void setHidden(bool hide) {
		properties.isHidden = hide;
		if(properties.isHidden != hide){
			forceUpdate();
		}
	}

	inline const ImageBuffer* getImageBuffer() const {
		return imageBuffer;
	}

	inline Position getPosition() const {
		return Position{.x = int16_t(properties.x), .y = int16_t(properties.y)};
	}
	
	inline bool isProgmem() const {
		return !properties.isSRAM;
	}

	inline bool isHidden() const {
		return properties.isHidden;
	}

private:
	Properties properties;
	const ImageBuffer* imageBuffer;
};


struct ClearSettings {
	using ClearFuncPtr = void(*)(Position, Size);

	Position position;
	ClearFuncPtr clearFn;
};

void drawWindowBitPixel(Adafruit_ILI9341& tft, const gui::Window& window, gui::Color565 color, Option<gui::ClearSettings> maybeClear = None<gui::ClearSettings>());
void drawHorizontalSeparatorWithBorders(Adafruit_ILI9341& tft, int16_t x, int16_t y, int16_t width, int16_t height);

void drawGeneratedGridPattern(Adafruit_ILI9341& tft, int16_t topX, int16_t topY, int16_t width, int16_t height, int16_t gridSpacing, Color565 lineColor, Color565 backgroundColor, int16_t offsetX = 0, int16_t offsetY = 0);


} // gui

#endif