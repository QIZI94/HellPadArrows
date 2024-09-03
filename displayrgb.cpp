#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include "displayrgb.h"
#include "display_resources.h"

#define TFT_DC 7
#define TFT_CS 8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);



namespace module{ // display

	void DisplayRGBModule::showText(const char *str_c){

	}

	void DisplayRGBModule::showArrow(uint8_t slot, Option<Arrow> arrow)
	{
	}

	void DisplayRGBModule::showSlotSelection(Option<uint8_t> slot)
	{
	}

	void DisplayRGBModule::reset()
	{
	}

	void DisplayRGBModule::update()
	{
	}

	Option<Arrow> DisplayRGBModule::getArrowFromSlot(uint8_t slot) const
	{
		return Option<Arrow>();
	}

	Option<uint8_t> DisplayRGBModule::getSelection() const
	{
		return Option<uint8_t>();
	}

	DisplayRGBModule::InitializationState DisplayRGBModule::init(){
		/*tft.begin();
		  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
		Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
		x = tft.readcommand8(ILI9341_RDMADCTL);
		Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
		x = tft.readcommand8(ILI9341_RDPIXFMT);
		Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
		x = tft.readcommand8(ILI9341_RDIMGFMT);
		Serial.print("Image Format: 0x"); Serial.println(x, HEX);
		x = tft.readcommand8(ILI9341_RDSELFDIAG);
		Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); */

		//tft.drawBitmap(0,0, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
		
		//tft.fillTriangle()
		
		return InitializationState::Initialized;
	}

	void DisplayRGBModule::run(){

	}
}