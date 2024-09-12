


#include "gui.h"

#include "helldef.h"




namespace gui{

static Position lerp(const Position& start, const Position& end, int32_t duration_ms, int32_t delta_ms) {

    //assert(duration_ms > 0);

    int16_t x_lerp = int32_t(start.x + (end.x - start.x)) * delta_ms / duration_ms;
    int16_t y_lerp = int32_t(start.y + (end.y - start.y)) * delta_ms / duration_ms;

    return Position{x_lerp, y_lerp};
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
	if(window.isProgmem()){
		PROGMEM_READ_STRUCTURE(&windowSize, &imageBuffer->size);
	}
	else{
		windowSize = imageBuffer->size;
	}

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


Position animateMovement(AnimatedMovement& animation, uint32_t delta){
	Position oldPosition = animation.window.getPosition();
	if(animation.isRepeated() && oldPosition == animation.getEndPos()){
		animation.window.setPosition(animation.getStartPos());
		return;
	}

	
	Position newPosition = lerp(animation.getStartPos(), animation.getEndPos(), animation.getDuration(), delta);
	animation.window.setPosition(newPosition);
	

	return oldPosition;

}

} // gui
