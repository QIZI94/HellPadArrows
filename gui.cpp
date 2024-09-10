


#include "gui.h"

namespace gui{
// do not touch this functions, it's for some reason very fast, but only if packet buffer for width is 1
void drawGeneratedGridPattern(Adafruit_ILI9341& tft, int16_t topX, int16_t topY, int16_t width, int16_t height, int16_t gridSpacing, Color565 lineColor, Color565 backgroundColor, int16_t offsetX = 0, int16_t offsetY = 0) {
	gridSpacing += 1;

    
	tft.startWrite();
  	tft.setAddrWindow(topX, topY, width, height);

	constexpr int16_t MAX_DRAWABLE_WIDTH = 1;
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
