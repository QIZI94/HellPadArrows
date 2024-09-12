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

	bool operator ==(const Position& other){
		return x == other.x && y == other.y;
	}
	bool operator !=(const Position& other){
		return x != other.x && y != other.y;
	}
};

struct Size {
	int16_t width;
	int16_t height;

	bool operator ==(const Size& other){
		return width == other.width && height == other.height;
	}
	bool operator !=(const Size& other){
		return width != other.width && height != other.height;
	}
};

struct ImageBuffer{
	const Size size;
	const uint8_t image[];
};


//#define WINDOW_OPTIMIZE_RAM
//#define WINDOW_OPTIMIZE_ROM

class Window{
#ifdef WINDOW_OPTIMIZE_RAM
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

	

	inline Position getPosition() const {
		return Position{.x = int16_t(properties.x), .y = int16_t(properties.y)};
	}
	
	inline bool isProgmem() const {
		return !properties.isSRAM;
	}

	inline bool isHidden() const {
		return properties.isHidden;
	}
#else
public: 
	constexpr Window(int16_t x, int16_t y, const ImageBuffer* imageBuffer, bool isHidden = false, bool isProgmem = true)
	 : position(Position{x, y}), b_isHidden(isHidden), b_isSRAM(!isProgmem), b_needsUpdate(true), imageBuffer(imageBuffer) {}

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
		
		this->position = position;
		if(!isHidden()){
			forceUpdate();
		}
	}

	inline void setHidden(bool hide) {
		if(b_isHidden != hide){
			b_isHidden = hide;
			forceUpdate();
		}
	}

	

	inline Position getPosition() const {
		return position;
	}
	
	inline bool isProgmem() const {
		return !b_isSRAM;
	}

	inline bool isHidden() const {
		return b_isHidden;
	}
#endif
public:
	inline void setImageBuffer(const ImageBuffer* imgBuffer) {
		imageBuffer = imgBuffer;
		if(!isHidden()){
			forceUpdate();
		}
	}
	

	inline const ImageBuffer* getImageBuffer() const {
		return imageBuffer;
	}

private:
#ifdef WINDOW_OPTIMIZE_RAM
	Properties properties;
#else 
	Position position;
	bool b_isHidden = false;
	bool b_isSRAM = false;
	bool b_needsUpdate = true;
#endif
	
	const ImageBuffer* imageBuffer;
};

class AnimatedMovement {
public:
	constexpr AnimatedMovement(const Window& window, Position start, Position stop, uint16_t duration, bool repeat = true, bool disabled = false, bool fadeInOut = true)
	 : window(window), start(start), end(end), mi_duration(duration), mb_disabled(disabled), mb_repeat(repeat), mb_fadeInOut(fadeInOut){}

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
		mb_disabled = disabled;
	}

	void setRepeat(bool repeat){
		mb_repeat = repeat;
	}

	void setFadeInOut(bool fadeInOut){
		mb_fadeInOut = fadeInOut;
	}


	Position getStartPos() const {
		return start;
	}

	Position getEndPos() const {
		return end;
	}



	uint16_t getDuration() const {
		return mi_duration;
	}

	bool isDisabled() const {
		return mb_disabled;
	}

	bool isRepeated() const {
		return mb_repeat;
	}

	bool isFadeInOut() const {
		return mb_fadeInOut;
	}
	gui::Window window;
private:
	
	Position start;
	Position end;
	uint16_t mi_duration;
	bool mb_disabled;
	bool mb_repeat;
	bool mb_fadeInOut;
	//bool
};



struct ClearSettings {
	using ClearFuncPtr = void(*)(Position, Size);

	Position position;
	ClearFuncPtr clearFn;
};

void drawWindowBitPixel(Adafruit_ILI9341& tft, const gui::Window& window, gui::Color565 color, Option<gui::ClearSettings> maybeClear = None<gui::ClearSettings>());
void drawHorizontalSeparatorWithBorders(Adafruit_ILI9341& tft, int16_t x, int16_t y, int16_t width, int16_t height);

void drawGeneratedGridPattern(Adafruit_ILI9341& tft, int16_t topX, int16_t topY, int16_t width, int16_t height, int16_t gridSpacing, Color565 lineColor, Color565 backgroundColor, int16_t offsetX = 0, int16_t offsetY = 0);

// returns old position
extern Position animateMovement(AnimatedMovement& animation, uint32_t delta);

} // gui

#endif