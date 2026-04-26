#ifndef GUI
#define GUI

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#include "shared/optional.h"

#include "decompression.h"

namespace gui{

using Color565 = uint16_t;


struct Position {
	int16_t x;
	int16_t y;

	bool operator ==(const Position& other) const {
		return x == other.x && y == other.y;
	}
	bool operator !=(const Position& other) const {
		return x != other.x || y != other.y;
	}

	Position lerpTo(const Position& to, uint16_t durationMs, uint16_t elapsedTtimeMs) const;

};

struct Position8Bit{
	uint8_t x;
	uint8_t y;

	operator Position() const {
		return Position{.x = int16_t(x), .y = int16_t(y)};
	}

	bool operator ==(const Position8Bit& other) const {
		return x == other.x && y == other.y;
	}
	bool operator !=(const Position8Bit& other) const {
		return x != other.x || y != other.y;
	}

	constexpr Position8Bit withX(uint8_t newX) {
		return Position8Bit{
			.x = newX,
			.y = y
		};
	}
	constexpr Position8Bit withY(uint8_t newY) {
		return Position8Bit{
			.x = x,
			.y = newY
		};
	}

	

	Position8Bit lerpTo(const Position8Bit& to, uint16_t durationMs, uint16_t elapsedTtimeMs) const;

	static Position8Bit from(const Position& position){
		return Position8Bit{
			.x = uint8_t(position.x),
			.y = uint8_t(position.y)
		};
	}
};

struct Size {
	int16_t width;
	int16_t height;

	bool operator ==(const Size& other){
		return width == other.width && height == other.height;
	}
	bool operator !=(const Size& other){
		return width != other.width || height != other.height;
	}
};

struct Size8Bit {
	uint8_t width;
	uint8_t height;

	operator Size() const {
		return Size{.width = int16_t(width), .height = int16_t(height)};
	}
	bool operator ==(const Size& other){
		return width == other.width && height == other.height;
	}
	bool operator !=(const Size& other){
		return width != other.width || height != other.height;
	}

	constexpr Size8Bit withWidth(uint8_t newWidth){
		return Size8Bit{
			.width = newWidth,
			.height = height
		};
	}
	constexpr Size8Bit withHeight(uint8_t newHeight){
		return Size8Bit{
			.width = width,
			.height = newHeight
		};
	}

};

/*
struct ImageBuffer{
	const Size size;
	const uint8_t image[];
};
*/
using CompressedImageBuffer = compression::CompressedImageUnchecked;

enum Flip : uint8_t{
	NONE,
	ROTATED_LEFT,
	VERTICALLY_ROTATED_LEFT,
	HORIZONTALLY_ROTATED_LEFT,
	HORIZONTALLY,
	VERTICALLY,
	HORIZONTALLY_AND_VERTICALLY,
	
};
//#define WINDOW_OPTIMIZE_RAM
//#define WINDOW_OPTIMIZE_ROM

class Window{
#ifdef WINDOW_OPTIMIZE_RAM
private:


	struct Properties{
		constexpr Properties() : raw(0) {}
		constexpr Properties(int16_t x, int16_t y, bool isHidden)
		 : x(x), y(y), isHidden(isHidden), needsUpdate(true) {}
		union{
			struct{
				int32_t x  : 10;
				int32_t y : 10;
				int32_t isHidden : 1;
				int32_t needsUpdate : 1;
			};
		
			int32_t raw;
			
		};
	};

public:

	constexpr Window(int16_t x, int16_t y, const ImageBuffer* imageBuffer, bool isHidden = false, bool isProgmem = true)
	 : properties(Properties(x, y, isHidden)), imageBuffer(imageBuffer) {}

	inline void forceUpdate() {
		properties.needsUpdate = true;
	}

	inline bool needsUpdate() const {
		return properties.needsUpdate;
	}

	inline void updated() {
		properties.needsUpdate = false;
	}


	inline void setPosition(const Position& position) {
		
		
		if(getPosition() != position){
			properties.x = position.x;
			properties.y = position.y;
			if(!isHidden()){
				forceUpdate();
			}
		}
	}

	inline void setHidden(bool hide) {
		properties.isHidden = hide;
		if(properties.isHidden != hide){
			forceUpdate();
		}
	}

	

	inline Position getPosition() const {
		return Position{.x = int16_t(properties.x), .y = int16_t(properties.y)};
	}
	

	inline bool isHidden() const {
		return properties.isHidden;
	}
#else

public: 
	constexpr Window(int16_t x, int16_t y,  CompressedImageBuffer imageBuffer, uint8_t colorPaletteIndex = 0, bool isHidden = false, gui::Flip flipSetting = gui::Flip::NONE)
	 : position(Position{x, y}), b_isHidden(isHidden), b_needsUpdate(!isHidden), imageBuffer(imageBuffer) ,colorPaletteIndex(colorPaletteIndex) ,flipSetting(flipSetting) {}

	inline void forceUpdate() {
		b_needsUpdate = true;
	}

	inline bool needsUpdate() const {
		return b_needsUpdate;
	}

	inline void updated() {
		b_needsUpdate = false;
	}


	inline void setPosition(const Position& position) {
		if(this->position != position){
			this->position = position;
			if(!isHidden()){
				forceUpdate();
			}
		}
	}

	inline void setHidden(bool hide) {
		if(b_isHidden != hide){
			b_isHidden = hide;
			forceUpdate();
		}
	}

	inline void setColorPaletteIndex(uint8_t colorPaletteIndex){
		this->colorPaletteIndex = colorPaletteIndex;
	}

	inline uint8_t getColorPaletteIndex() const {
		return colorPaletteIndex;
	}

	inline const Position& getPosition() const {
		return position;
	}

	inline bool isHidden() const {
		return b_isHidden;
	}

#endif
public:
	inline void setImageBuffer(CompressedImageBuffer imgBuffer) {
		imageBuffer = imgBuffer;
		if(!isHidden()){
			forceUpdate();
		}
	}
	inline CompressedImageBuffer getImageBuffer() const {
		return imageBuffer;
	}

	inline void setFlipSetting(gui::Flip flip) {
		flipSetting = flip;
	}
	inline gui::Flip getFlipSetting() const {
		return flipSetting;
	}
public:
	static void SetColorPaletteBuffer(const Color565* colorPaletteBuffer){
		*GetColorPaletteBuffer() = colorPaletteBuffer;
	}

	static Option<Color565> LoadColorFromColorPalette(uint8_t colorPaletteIndex);

private:
	static const Color565** GetColorPaletteBuffer() {
		static const Color565* colorPaletteBuffer = nullptr;
		return &colorPaletteBuffer; 
	}

private:
#ifdef WINDOW_OPTIMIZE_RAM
	Properties properties;
#else 
	Position position;
	bool b_isHidden;// : 1;
	bool b_needsUpdate;// : 1;
	struct{
		
		gui::Flip flipSetting : 4;// : 6;
		uint8_t colorPaletteIndex : 4;
	};
#endif
	
	CompressedImageBuffer imageBuffer;
};

class AnimatedMovement {
public:
	enum class FinishBehavior : uint8_t{
		REPEAT,
		RUN_ONCE_AND_HIDE,
		RUN_ONCE		
	};
	struct AnimationState{
		union{
			struct{
				bool mb_disabled : 1;
				bool mb_mirroredY : 1;
				bool mb_fadeInOut : 1;
				bool mb_initialized : 1;
				uint8_t e_finishBehavior : 2;
			};
			uint8_t state;
			
		};
		constexpr AnimationState(FinishBehavior finishBehavior, bool disabled, bool mirroredY, bool fadeInOut, bool initialized)
		 : mb_disabled(disabled), mb_mirroredY(mirroredY), e_finishBehavior(uint8_t(finishBehavior)), mb_fadeInOut(fadeInOut), mb_initialized(initialized)
		{}
		constexpr AnimationState(uint8_t state) : state(state){}
		
		
	};
	constexpr AnimatedMovement(const Window& window, const Position& start, const Position& end, uint16_t duration, FinishBehavior finishBehavior = FinishBehavior::REPEAT, bool disabled = false, bool mirroredY = false, bool fadeInOut = true)
	 : window(window), start(start), end(end), mi_duration(duration), mi_startTime(0), animState(AnimationState(finishBehavior, disabled, mirroredY, fadeInOut, false)){

	 }


	void restart() {
		mi_startTime = millis();
		window.setPosition(start);
	}

	void setStartPos(Position start){
		this->start = start;
	}

	void setEndPos(Position end){
		this->end = end;
	}

	void setDuration(uint16_t duration){
		mi_duration = duration;
	}

	void setDisabled(bool disabled){
		animState.mb_disabled = disabled;
	}

	void setRepeat(FinishBehavior finishBehavior){
		animState.e_finishBehavior = uint8_t(finishBehavior);
	}

	void setFadeInOut(bool fadeInOut){
		animState.mb_fadeInOut = fadeInOut;
	}

	void setMirroredY(bool mirrored){
		animState.mb_mirroredY = mirrored;
	}


	const Position& getStartPos() const {
		return start;
	}

	const Position& getEndPos() const {
		return end;
	}

	uint16_t getDuration() const {
		return mi_duration;
	}


	bool isDisabled() const {
		return animState.mb_disabled;
	}

	FinishBehavior getFinishBehavior() const {
		return FinishBehavior(animState.e_finishBehavior);
	}

	bool isFadeInOut() const {
		return animState.mb_fadeInOut;
	}

	bool isFinished() const {
		return window.getPosition() == end;
	}

	bool isMirroredY() const {
		return animState.mb_mirroredY;
	}

	// returns old position
	Position animateMovement();

	gui::Window window;
private:
	
	Position start;
	Position end;
	uint16_t mi_startTime;
	uint16_t mi_duration;
	AnimationState animState;
	/*bool mb_disabled;
	bool mb_mirroredY;
	bool mb_repeat;
	bool mb_fadeInOut;
	bool mb_initialized;*/
	//bool
};



struct ClearSettings {
	using ClearFuncPtr = void(*)(Position, Size);

	Position position;
	ClearFuncPtr clearFn;
};


constexpr Color565 ConvertRGBtoRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 11) | (g << 5) | b; 
}

/*constexpr Color565 ConvertRGBtoRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return static_cast<Color565>(
        (((r * 31 + 127) / 255) << 11) |
        (((g * 63 + 127) / 255) << 5)  |
        ((b * 31 + 127) / 255)
    );
}*/

constexpr uint8_t GetRedFromRGB565(Color565 color) {
    return (color >> 11) & 0x1F; 
}

constexpr uint8_t GetGreenFromRGB565(Color565 color) {
    return (color >> 5) & 0x3F; 
}

constexpr uint8_t GetBlueFromRGB565(Color565 color) {
    return color & 0x1F; 
}


int16_t lerp(int16_t start, int16_t end, uint16_t durationMs, uint16_t elapsedTtimeMs);
Position lerp(const Position& start, const Position& end, uint16_t durationMs, uint16_t elapsedTtimeMs);
Color565 lerpColor565(Color565 color_start, Color565 color_end, uint16_t durationMs, uint16_t elapsedTtimeMs);

void drawWindowBitPixel(Adafruit_ILI9341& tft, const gui::Window& window, Option<Color565> maybeOutlineColor = None, Option<gui::ClearSettings> maybeClear = None);
void drawHorizontalSeparatorWithBorders(Adafruit_ILI9341& tft, int16_t x, int16_t y, int16_t width, int16_t height);

void drawBitmapWithOutline(Adafruit_ILI9341& tft, CompressedImageBuffer::iterator imageBufferIterator, int16_t topX, int16_t topY, int16_t width, int16_t height, Color565 mainColor, Color565 outlineColor, gui::Flip flip = gui::Flip::NONE);
void drawGeneratedGridPattern(Adafruit_ILI9341& tft, int16_t topX, int16_t topY, int16_t width, int16_t height, int16_t gridSpacing, Color565 lineColor, Color565 backgroundColor, int16_t offsetX = 0, int16_t offsetY = 0);



} // gui

#endif