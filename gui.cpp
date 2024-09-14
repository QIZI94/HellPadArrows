


#include "gui.h"

#include "helldef.h"




namespace gui{


static uint8_t getRed(Color565 color) {
    return (color >> 11) & 0x1F; 
}

static uint8_t getGreen(Color565 color) {
    return (color >> 5) & 0x3F; 
}

static uint8_t getBlue(Color565 color) {
    return color & 0x1F; 
}

static Color565 combineRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 11) | (g << 5) | b; 
}

int16_t lerp(int16_t start, int16_t end, uint16_t durationMs, uint16_t elapsedTtimeMs) {
	if(durationMs == 0){
		return end;
	}
    return static_cast<int32_t>(start) + static_cast<int32_t>(end - start) * elapsedTtimeMs / durationMs;
}
Position lerp(const Position& start, const Position& end, uint16_t durationMs, uint16_t elapsedTtimeMs) {
    if(durationMs == 0){
		return end;
	}
    int32_t x_lerp = lerp(start.x, end.x, durationMs, elapsedTtimeMs); // static_cast<int32_t>(start.x) + static_cast<int32_t>(end.x - start.x) * elapsedTtimeMs / durationMs;
    int32_t y_lerp = lerp(start.y, end.y, durationMs, elapsedTtimeMs);//static_cast<int32_t>(start.y) + static_cast<int32_t>(end.y - start.y) * elapsedTtimeMs / durationMs;
    return Position{.x = static_cast<int16_t>(x_lerp), .y = static_cast<int16_t>(y_lerp)};
}


Color565 lerpColor565(Color565 color_start, Color565 color_end, uint16_t durationMs, uint16_t elapsedTtimeMs) {
    if(durationMs == 0){
		return color_end;
	}

    uint8_t r_start = getRed(color_start);
    uint8_t g_start = getGreen(color_start);
    uint8_t b_start = getBlue(color_start);

    uint8_t r_end = getRed(color_end);
    uint8_t g_end = getGreen(color_end);
    uint8_t b_end = getBlue(color_end);

    uint8_t r_lerp = lerp(r_start, r_end, durationMs, elapsedTtimeMs);
    uint8_t g_lerp = lerp(g_start, g_end, durationMs, elapsedTtimeMs);
    uint8_t b_lerp = lerp(b_start, b_end, durationMs, elapsedTtimeMs);

    return combineRGB565(r_lerp, g_lerp, b_lerp);
}



Position Position::lerpTo(const Position& to, uint16_t durationMs, uint16_t elapsedTtimeMs) const {
	return lerp(*this, to, durationMs, elapsedTtimeMs);
}


Position AnimatedMovement::animateMovement(){
	Position oldPosition = window.getPosition();
 
	
	if(isDisabled()){
		return oldPosition;
	}
	else if(mb_initialized == false){
		mi_startTime = millis();
		mb_initialized = true;
	}
	else if(isFinished()){
		if(isRepeated()){
			
			restart();
		}
		else {
			return oldPosition;
		}
	}

	uint16_t elapsedTime = millis() - mi_startTime;
	Position newPosition = getStartPos().lerpTo(getEndPos(), mi_duration, elapsedTime < mi_duration ?  elapsedTime : mi_duration);
	window.setPosition(newPosition);
	
	return oldPosition;
}



void drawWindowBitPixel(Adafruit_ILI9341& tft, const gui::Window& window, gui::Color565 color, Option<ClearSettings> maybeClear){
	if(!window.needsUpdate()){
		return;
	}
	
	const gui::Position windowPosition = window.getPosition();
	const gui::ImageBuffer* imageBuffer = window.getImageBuffer();
	if(imageBuffer == nullptr){
		return;
	}

	gui::Size windowSize;
	PROGMEM_READ_STRUCTURE(&windowSize, &imageBuffer->size);

	if(const gui::ClearSettings* p_clearSettings = maybeClear.ptr_value()){
		p_clearSettings->clearFn(p_clearSettings->position, windowSize);
	}
	if(!window.isHidden()){
		tft.drawBitmap(windowPosition.x, windowPosition.y, imageBuffer->image, windowSize.width, windowSize.height, color);
	}
}

void drawHorizontalSeparatorWithBorders(Adafruit_ILI9341& tft, int16_t x, int16_t y, int16_t width, int16_t height){
	width = max(8, width);
	tft.drawFastHLine(x + 2, y - 2, 		width -7, ILI9341_LIGHTGREY);
	tft.drawFastHLine(x,     y - 1, 		width -4, ILI9341_DARKGREY);
	tft.fillRoundRect(x,     y,     		width -3, height, 4, ILI9341_YELLOW);
	tft.drawFastHLine(x,     y + height , 	width -4, ILI9341_DARKGREY);
	tft.drawFastHLine(x + 2, y + height + 1,width -7, ILI9341_LIGHTGREY);
}


// do not touch this functions, it's for some reason very fast, but only if packet buffer for width is 1
void drawGeneratedGridPattern(Adafruit_ILI9341& tft, int16_t topX, int16_t topY, int16_t width, int16_t height, int16_t gridSpacing, Color565 lineColor, Color565 backgroundColor, int16_t offsetX = 0, int16_t offsetY = 0) {
	gridSpacing += 1;

    
	tft.startWrite();
  	tft.setAddrWindow(topX, topY, width, height);

	constexpr int16_t MAX_DRAWABLE_WIDTH = 240/4;
	uint16_t displayWidthBuffer[MAX_DRAWABLE_WIDTH];

	for (int16_t y = 0; y < height; ++y) {
		int16_t x = 0;
		int16_t widthToDraw = width;
		while(x < width){
			int16_t localWidth = min(MAX_DRAWABLE_WIDTH, widthToDraw);
			for(int16_t localX = 0; localX < localWidth; ++localX, ++x){
				bool isGridLine = (x + topX + offsetX) % gridSpacing == 0 || (y + topY + offsetY) % gridSpacing == 0;
				if (isGridLine) {
                	displayWidthBuffer[localX] = lineColor;
				}
				else {
					displayWidthBuffer[localX] = backgroundColor;
				}
			}
			tft.writePixels(displayWidthBuffer, localWidth);
		
			widthToDraw -= localWidth;
			
		}
    }

  	tft.endWrite();
}




} // gui
