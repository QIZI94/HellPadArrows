


#include "gui.h"

#include "helldef.h"




namespace gui{



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

    uint8_t r_start = GetRedFromRGB565(color_start);
    uint8_t g_start = GetGreenFromRGB565(color_start);
    uint8_t b_start = GetBlueFromRGB565(color_start);

    uint8_t r_end = GetRedFromRGB565(color_end);
    uint8_t g_end = GetGreenFromRGB565(color_end);
    uint8_t b_end = GetBlueFromRGB565(color_end);

    uint8_t r_lerp = lerp(r_start, r_end, durationMs, elapsedTtimeMs);
    uint8_t g_lerp = lerp(g_start, g_end, durationMs, elapsedTtimeMs);
    uint8_t b_lerp = lerp(b_start, b_end, durationMs, elapsedTtimeMs);

    return ConvertRGBtoRGB565(r_lerp, g_lerp, b_lerp);
}



Position Position::lerpTo(const Position& to, uint16_t durationMs, uint16_t elapsedTtimeMs) const {
	return lerp(*this, to, durationMs, elapsedTtimeMs);
}


Position8Bit Position8Bit::lerpTo(const Position8Bit &to, uint16_t durationMs, uint16_t elapsedTtimeMs) const {
	return Position8Bit::from(lerp(*this, to, durationMs, elapsedTtimeMs));
}

Option<Color565> Window::LoadColorFromColorPalette(uint8_t colorPaletteIndex){
	const Color565* colorPaletteBuffer = *GetColorPaletteBuffer();
	if(colorPaletteBuffer != nullptr){
		Color565 color;
		PROGMEM_READ_STRUCTURE(&color, &colorPaletteBuffer[colorPaletteIndex]);
		return Some(color);
	}
	return None;
}

Position AnimatedMovement::animateMovement(){
	Position oldPosition = window.getPosition();
 
	
	if(isDisabled()){
		return oldPosition;
	}
	else if(animState.mb_initialized == false){
		mi_startTime = millis();
		animState.mb_initialized = true;
	}
	else if(isFinished()){
		switch (FinishBehavior(animState.e_finishBehavior))
		{
			case FinishBehavior::REPEAT:
				restart();
				break;
			case FinishBehavior::RUN_ONCE_AND_HIDE:
				window.setHidden(true);
			case FinishBehavior::RUN_ONCE:
				return oldPosition;
		

		}

	}

	uint16_t elapsedTime = millis() - mi_startTime;
	Position newPosition = getStartPos().lerpTo(getEndPos(), mi_duration, elapsedTime < mi_duration ?  elapsedTime : mi_duration);
	window.setPosition(newPosition);
	
	return oldPosition;
}



void drawWindowBitPixel(Adafruit_ILI9341& tft, const gui::Window& window, Option<Color565> maybeOutlineColor, Option<ClearSettings> maybeClear){
	if(!window.needsUpdate()){
		return;
	}
	
	const gui::Position windowPosition = window.getPosition();
	const CompressedImageBuffer imageBuffer = window.getImageBuffer();
	if(imageBuffer.compressedStorage == nullptr){
		return;
	}

	compression::Size windowSize;
	PROGMEM_READ_STRUCTURE(&windowSize, &imageBuffer.compressedStorage->size);

	gui::Color565 mainColor =  Window::LoadColorFromColorPalette(window.getColorPaletteIndex()).valueOr(ILI9341_BLACK);

	if(const gui::ClearSettings* p_clearSettings = maybeClear.ptr_value()){
		p_clearSettings->clearFn(p_clearSettings->position, gui::Size{.width = windowSize.width, .height = windowSize.height});
	}
	if(!window.isHidden()){
		Color565 outlineColor = maybeOutlineColor.valueOr(mainColor);
		drawBitmapWithOutline(tft, imageBuffer.iterate(), windowPosition.x, windowPosition.y, windowSize.width, windowSize.height, mainColor, outlineColor, window.getFlipSetting());
	}
}

void drawBitmapWithOutline(Adafruit_ILI9341& tft, CompressedImageBuffer::iterator imageBufferIterator, int16_t topX, int16_t topY, int16_t width, int16_t height, Color565 mainColor, Color565 outlineColor, gui::Flip flip){
	constexpr int16_t BITWISE_MODULO_8BITS_MASK = 7;
	constexpr uint8_t IS_PIXEL_MASK = 0x80;
	
	const int16_t byteWidth = (width + 7) / 8; // Bitmap scanline pad = whole byte
  	uint16_t currentByteLoadedIndex = 0;
	uint8_t b = 0;
	bool wasPreviousPointPixel = false;

	tft.startWrite();
	bool alreadyPreloaded = false;
	for (int16_t pixelY = 0; pixelY < height; pixelY++) {
		bool wasPreviousPointPixel = false;
		
		for (int16_t pixelX = 0; pixelX < width; pixelX++) {
			if (pixelX & BITWISE_MODULO_8BITS_MASK){
				b <<= 1;
			}
			else{
				//currentByteLoadedIndex = getByteIndexInBitset(i, j, byteWidth);
				if(alreadyPreloaded){
					alreadyPreloaded = false;
				}
				else {
					b = imageBufferIterator.next().valueCopyUnchecked();
				}
				
			}

			if (b & IS_PIXEL_MASK){
				const bool neghborLeft = wasPreviousPointPixel;
				Color565 pixelColor = mainColor;
				if(!neghborLeft){
					pixelColor = outlineColor;
				}
				else if(pixelX == width - 1){
					pixelColor = outlineColor;
				}
				else {
					int16_t rightNegihborPixelPosition = pixelX + 1;
					uint8_t n;
					if(rightNegihborPixelPosition & BITWISE_MODULO_8BITS_MASK){
						n = (b << 1);
					}
					else {
						n = b = imageBufferIterator.next().valueCopyUnchecked();
						alreadyPreloaded = true;
					}

					if (n & IS_PIXEL_MASK == 0){
						pixelColor = outlineColor;
					}

					/*uint16_t rightNeighborByteIndex = getByteIndexInBitset(i + 1, j, byteWidth);
					if(currentByteLoadedIndex == rightNeighborByteIndex){
						const uint8_t neghborRight = (b << 1) & 0x80;
						if (neghborRight == 0){
							pixelColor = outlineColor;
						}
					}
					else{
						const uint8_t neghborRight = pgm_read_byte(&image[rightNeighborByteIndex]);
						if((neghborRight & 0x80) == 0){
							pixelColor = outlineColor;
						} 
					}*/
					
				}
				
				//uint8_t localX;
				int16_t finalX;
				int16_t finalY;
				
			
				switch (flip)
				{
					case gui::Flip::ROTATED_LEFT:
					case gui::Flip::HORIZONTALLY_ROTATED_LEFT:
						finalX = pixelY;
						break;
					case gui::Flip::HORIZONTALLY:
					case gui::Flip::HORIZONTALLY_AND_VERTICALLY:
						finalX = width - 1 - pixelX;
						break;
				
					case gui::VERTICALLY_ROTATED_LEFT:
						finalX = height - 1 - pixelY;
						break;
					default:
						finalX = pixelX;
						break;
				}
				switch(flip){

					case gui::Flip::ROTATED_LEFT:
					case gui::Flip::VERTICALLY_ROTATED_LEFT:
						finalY = width - 1 - pixelX;
						break;
					case gui::Flip::HORIZONTALLY_ROTATED_LEFT:
						finalY = height - (width - 1 - pixelX);
						break;
					case gui::Flip::VERTICALLY:
					case gui::Flip::HORIZONTALLY_AND_VERTICALLY:
						finalY = height - 1 - pixelY;
						break;
					
					default:
						finalY = pixelY;
						break;
				}

				

				
				tft.writePixel(topX + finalX, topY + finalY, pixelColor);
				
				wasPreviousPointPixel = true;
			}
			else {
				wasPreviousPointPixel = false;
			}
		}
 	 }
  	tft.endWrite();
}

void drawHorizontalSeparatorWithBorders(Adafruit_ILI9341& tft, int16_t x, int16_t y, int16_t width, int16_t height){
	width = max(8, width);
	tft.drawFastHLine(x + 2, y - 2, 		width -7, ILI9341_BLACK);
	tft.drawFastHLine(x,     y - 1, 		width -4, ILI9341_DARKGREY);
	tft.fillRoundRect(x,     y,     		width -3, height, 4, ILI9341_YELLOW);
	tft.drawFastHLine(x,     y + height , 	width -4, ILI9341_DARKGREY);
	tft.drawFastHLine(x + 2, y + height + 1,width -7, ILI9341_BLACK);
}



// do not touch this functions, it's for some reason very fast, but only if packet buffer for width is 1
void drawGeneratedGridPattern(Adafruit_ILI9341& tft, int16_t topX, int16_t topY, int16_t width, int16_t height, int16_t gridSpacing, Color565 lineColor, Color565 backgroundColor, int16_t offsetX = 0, int16_t offsetY = 0) {
	gridSpacing += 1;

	topX = max(topX, 0);
	topY = max(topY, 0);
    
	width = min(topX + width + 2, tft.width()) - topX;
	height = min(topY + height + 2, tft.height()) - topY;

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
