#include "displaysmall.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>

#include "displaysmall_resources.h"


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

namespace module{ // display




Adafruit_SSD1306 small_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


struct ArrowToImageMapping{
	constexpr ArrowToImageMapping(Arrow arrow, const uint8_t* image)
	: arrow(arrow), image(image) {}

	const Arrow arrow;
	const uint8_t* image = nullptr;
};


const ArrowToImageMapping imageMapping[]{
	{Arrow::UP,		DPS_ArrowUpBMP},
	{Arrow::DOWN,	DPS_ArrowDownBMP},
	{Arrow::LEFT,	DPS_ArrowLeftBMP},
	{Arrow::RIGHT,	DPS_ArrowRightBMP},
};


void drawSelection(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t edgeWidth){
		uint8_t upperBeginX 	= x;
		uint8_t upperEndX 		= x + w;
		uint8_t upperY 			= y;
		
		// make line
		small_display.drawLine(upperBeginX, upperY, upperEndX, upperY,SSD1306_WHITE);

		// make edges
		small_display.drawLine(upperBeginX, upperY, upperBeginX, upperY + edgeWidth, SSD1306_WHITE);
		small_display.drawLine(upperEndX, upperY, upperEndX, upperY + edgeWidth, SSD1306_WHITE);

		uint8_t lowerBeginX 	= x;
		uint8_t lowerEndX 		= x + w;
		uint8_t lowerY 			= y + h;

		// make line
		small_display.drawLine(lowerBeginX, lowerY, lowerEndX, lowerY, SSD1306_WHITE);

		// make edges
		small_display.drawLine(lowerBeginX,lowerY, lowerBeginX, lowerY - edgeWidth, SSD1306_WHITE);
		small_display.drawLine(lowerEndX,lowerY, lowerEndX, lowerY - edgeWidth, SSD1306_WHITE);
}



void DisplaySmallOLEDModule::showText(const char *str_c)
{
	update();
}

void DisplaySmallOLEDModule::showArrow(uint8_t slot, Option<Arrow> arrow){
	if(slot < ARROW_MAX_SLOTS) {
		m_slots[slot] = arrow;
		update();
	}
}

void DisplaySmallOLEDModule::showSlotSelection(Option<uint8_t> slot){
	m_selectedSlot = slot;
	update();
}

void DisplaySmallOLEDModule::reset(){

	for(Option<Arrow>& slot : m_slots){
		slot = None<Arrow>();
	}

	m_selectedSlot = None<uint8_t>();

	update();
}


void DisplaySmallOLEDModule::update(){
	mb_redraw = true;
}


Option<Arrow> DisplaySmallOLEDModule::getArrowFromSlot(uint8_t slot) const {
	if(slot >= ARROW_MAX_SLOTS) {
		return None<Arrow>();
	}

	return m_slots[slot];	
}

Option<uint8_t> DisplaySmallOLEDModule::getSelection() const
{
	return m_selectedSlot;
}

DisplaySmallOLEDModule::InitializationState DisplaySmallOLEDModule::init()
{

	if(small_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
		delay(10);
		
		/*small_display.clearDisplay();
		
		small_display.drawBitmap(0,7, DPS_ArrowRightBMP, ARROW_WIDTH, ARROW_HEIGHT, SSD1306_WHITE);
		small_display.drawBitmap(2+20,7, DPS_ArrowRightBMP, ARROW_WIDTH, ARROW_HEIGHT, SSD1306_WHITE);
		small_display.drawBitmap(4+40,7, DPS_ArrowRightBMP, ARROW_WIDTH, ARROW_HEIGHT, SSD1306_WHITE);
		small_display.drawBitmap(6+60,7, DPS_ArrowRightBMP, ARROW_WIDTH, ARROW_HEIGHT, SSD1306_WHITE);
		small_display.drawBitmap(8+80,7, DPS_ArrowRightBMP, ARROW_WIDTH, ARROW_HEIGHT, SSD1306_WHITE);
		small_display.drawBitmap(10+100,7, DPS_ArrowRightBMP, ARROW_WIDTH, ARROW_HEIGHT, SSD1306_WHITE);*/
		

		

		return InitializationState::Initialized;
	}


    return InitializationState::Failed;
}

void DisplaySmallOLEDModule::run(){
	if(mb_redraw){
		uint8_t imageIdx = 0;
		constexpr uint8_t slotOffsetY = 7;
		small_display.clearDisplay();
		for(uint8_t slotIdx = 0; slotIdx < ARROW_MAX_SLOTS; slotIdx++){
			uint8_t slotOffsetX = m_slotOffsetsX[slotIdx];
			const Option<Arrow>& arrow = m_slots[slotIdx];

			if(const Arrow* p_arrow = arrow.ptr_value()){
				for(const ArrowToImageMapping& entry : imageMapping){
					if(entry.arrow == *p_arrow){
						small_display.drawBitmap(slotOffsetX, slotOffsetY, entry.image, ARROW_WIDTH, ARROW_HEIGHT, SSD1306_WHITE);
						break;
					}
				}
				
			}			
		}
		
		if(uint8_t* p_slot = m_selectedSlot.ptr_value()){
			uint8_t selectionOffsetX = m_selectionOffsetX[*p_slot];
			constexpr uint8_t selectionOffsetY = 0;
			constexpr uint8_t selectionWidth = 19;
			constexpr uint8_t selectionHeight = 31;
			constexpr uint8_t selectionEdgeHeight = 2;
			//drawSelection(selectionOffsetX, slotOffsetY,selectionWidth,selectionHeight, selectionEdgeHeight);
		}

		/*small_display.drawLine(0,0, 19,0,SSD1306_WHITE);
		small_display.drawLine(0,0, 0, 2, SSD1306_WHITE);
		small_display.drawLine(19,0, 19, 2, SSD1306_WHITE);

		small_display.drawLine(0,31, 19,31,SSD1306_WHITE);
		small_display.drawLine(0,31, 0, 29, SSD1306_WHITE);
		small_display.drawLine(19,31, 19, 29, SSD1306_WHITE);*/
		//drawSelection(0,0,19,31, 2);
		
		
		/*small_display.setTextSize(2); // Draw 2X-scale text
  		small_display.setTextColor(SSD1306_WHITE);
  		small_display.setCursor(6, 8);
		small_display.println("Successful");*/

		small_display.display();
		
		mb_redraw = false;
	}
}


} // display