#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include "displayrgb.h"
#include "display_resources.h"

#include "timer.h"



Adafruit_ILI9341 tft = Adafruit_ILI9341(
	Pinout::Assignment::TFT_CS,
	Pinout::Assignment::TFT_DC
);


constexpr uint16_t CLEAR_COLOR = ILI9341_DARKCYAN;


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


int16_t i = 0;

static TimedExecution10ms timedAnimation;

void anim(TimedExecution10ms&){
	
	if(i > 2){
		tft.fillRect(i-3, 130, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		tft.fillRect(i-3 + 25, 130, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		//tft.drawBitmap(i-6,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		//tft.drawBitmap(i-6 + 25,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
	}
	if(i > 230){
		i = 0;
	}
	if(i > 2){
		tft.fillRect(i-3, 130, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		tft.fillRect(i-3 + 25, 130, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		//tft.drawBitmap(i-6,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		//tft.drawBitmap(i-6 + 25,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
	}
	
	tft.drawBitmap(i,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	tft.drawBitmap(i + 25,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	i+=3;
	timedAnimation.restart(70);
}

DisplayRGBModule::InitializationState DisplayRGBModule::init(){
	//pinMode(Pinout::Assignment::TFT_CS, OUTPUT);
	//digitalWrite(Pinout::Assignment::TFT_CS	, HIGH); 
	tft.begin();
	
	/*uint8_t x = tft.readcommand8(ILI9341_RDMODE);
	Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDMADCTL);
	Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDPIXFMT);
	Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDIMGFMT);
	Serial.print("Image Format: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDSELFDIAG);
	Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); */

	if(tft.readcommand8(ILI9341_RDSELFDIAG) != 0xE0){
		return InitializationState::Failed;
	}


	
	tft.setRotation(uint8_t(DisplayRGBModule::DEFAULT_ROTATION));
	tft.fillScreen(ILI9341_DARKCYAN);
	tft.setCursor(0, 0);
	tft.setTextColor(ILI9341_WHITE); 
	tft.setTextSize(2);
	tft.println("Hello World!");

	
	
	//delay(100);
	tft.drawBitmap(25,100, DPS_ArrowUpBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	tft.drawBitmap(50,100, DPS_ArrowUpBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	tft.drawBitmap(75,100, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	tft.drawBitmap(100,100, DPS_ArrowUpBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);

	tft.drawBitmap(125,100, DPS_ArrowRightBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	tft.drawBitmap(150,100, DPS_ArrowUpBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	tft.drawBitmap(175,100, DPS_ArrowLeftBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	tft.drawBitmap(200,100, DPS_ArrowUpBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	tft.drawBitmap(225,100, DPS_ArrowUpBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	
	
	//tft.fillTriangle()
		// read diagnostics (optional but can help debug problems)


	timedAnimation.setup(anim, 70);


	
	return InitializationState::Initialized;
}


void DisplayRGBModule::run(){
	/*if(i > 200){
		i = 0;
	}*/

	/*if(i > 1){
		tft.drawFastVLine(i-1, 130, ARROW_HEIGHT, CLEAR_COLOR);
		tft.drawFastVLine(i-2, 130, ARROW_HEIGHT, CLEAR_COLOR);
	}
	else if(i > 0){
		tft.drawFastVLine(i-1, 130, ARROW_HEIGHT, CLEAR_COLOR);
	}*/
	//tft

	/*if(i > 2){
		tft.fillRect(i-3, 130, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		tft.fillRect(i-3 + 25, 130, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		//tft.drawBitmap(i-6,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		//tft.drawBitmap(i-6 + 25,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
	}
	
	tft.drawBitmap(i,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	tft.drawBitmap(i + 25,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, ILI9341_YELLOW);
	i+=3;
	delay(50);*/
	//delay(100);
}
}



