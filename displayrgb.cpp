#define USE_DMA
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include "displayrgb.h"
#include "display_resources.h"

#include "timer.h"


struct AnimatedMovement {
	constexpr AnimatedMovement(int16_t startX, int16_t startY, int16_t stepX, bool repeat, int16_t stopX, int16_t stopY, int16_t stepY, bool disabled)
		 : startX(startX), startY(startY), stepX(stepX), repeat(repeat), endX(endX), stepY(stepY), disabled(disabled) {}
	gui::Window window;
	struct  {
		union{
			struct {

				int32_t startX : 10;
				int32_t startY : 10;
				int32_t stepX  : 10;
				int32_t repeat : 1;
			};
			uint32_t rawPart1;
			
			
		};
		
	};
	struct {
		union {
			struct {

				int32_t endX 		: 10;
				int32_t endY 		: 10;
				int32_t stepY  		: 10;
				int32_t disabled 	: 1;
			};
			uint32_t rawPart2;
			
			
		};
	};
};

struct ArrowToImageMapping{
	constexpr ArrowToImageMapping(Arrow arrow, const gui::ImageBuffer* image)
	: arrow(arrow), image(image) {}

	const Arrow arrow;
	const gui::ImageBuffer* image = nullptr;
};


const ArrowToImageMapping imageMapping[]{
	{Arrow::UP,		&DPS_ArrowUpBMP},
	{Arrow::DOWN,	&DPS_ArrowDownBMP},
	{Arrow::LEFT,	&DPS_ArrowLeftBMP},
	{Arrow::RIGHT,	&DPS_ArrowRightBMP},
};


constexpr gui::Color565 CLEAR_COLOR 	= ILI9341_DARKCYAN;
constexpr gui::Color565 LINE_COLOR 		= ILI9341_NAVY;
constexpr int16_t GRID_LINES_OFFSET_X 	= +1;
constexpr int16_t GRID_LINES_OFFSET_Y 	= +5;
constexpr int16_t GRID_SPACING 			= 29;


Adafruit_ILI9341 tft(
	Pinout::Assignment::TFT_CS,
	Pinout::Assignment::TFT_DC
);

static TimedExecution10ms timedAnimation;
static uint32_t frameStartTime = 0;
static uint32_t averageFPS = 0;
static uint32_t averageSamples = 300;


static void clearWithGrid(gui::Position pos, gui::Size size){
	gui::drawGeneratedGridPattern(tft, pos.x, pos.y, size.width, size.height, GRID_SPACING, LINE_COLOR, CLEAR_COLOR, GRID_LINES_OFFSET_X, GRID_LINES_OFFSET_Y);
}

static void drawWindowBitPixel(const gui::Window& window, gui::Color565 color, Option<gui::Position> clearPrevious = None<gui::Position>()){
	if(const gui::Position* p_clearPosition = clearPrevious.ptr_value()){
		gui::drawWindowBitPixel(tft, window, color, Some(gui::ClearSettings{.position = *p_clearPosition, .clearFn = clearWithGrid}));
	}
	else {
		gui::drawWindowBitPixel(tft, window, color);
	}
}


namespace module{ // display

void DisplayRGBModule::setTargetFPS(uint8_t fps){
	mi_targetFpsDeltaMs = 1000/uint16_t(fps);
}

void DisplayRGBModule::showText(const char *str_c){

}

void DisplayRGBModule::showArrow(uint8_t slot, Option<Arrow> arrow) {
	if(slot < ARROW_MAX_SLOTS) {
		m_slots[slot] = arrow;
		update();
	}
}

void DisplayRGBModule::showSlotSelection(Option<uint8_t> slot)
{
}

void DisplayRGBModule::reset() {
	for(Option<Arrow>& slot : m_slots){
		slot = None<Arrow>();
	}

	m_selectedSlot = None<uint8_t>();

	update();
}

void DisplayRGBModule::update(){
	mb_redraw = true;
}

Option<Arrow> DisplayRGBModule::getArrowFromSlot(uint8_t slot) const
{
	if(slot >= ARROW_MAX_SLOTS) {
		return None<Arrow>();
	}

	return m_slots[slot];	
}

Option<uint8_t> DisplayRGBModule::getSelection() const
{
	return m_selectedSlot;
}

uint8_t DisplayRGBModule::getTargetFPS() const {
	return 1000/mi_targetFpsDeltaMs;
}

int16_t i = 0;



void anim(TimedExecution10ms&){
	
	/*if(i > 2){
		tft.fillRect(i-3, 130, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		tft.fillRect(i-3 + 25, 130, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		//tft.drawBitmap(i-6,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
		//tft.drawBitmap(i-6 + 25,130, DPS_ArrowDownBMP, ARROW_WIDTH, ARROW_HEIGHT, CLEAR_COLOR);
	}*/
	
	

	

	frameStartTime = millis();


	timedAnimation.restart(10);
	//delay(100);
}

DisplayRGBModule::InitializationState DisplayRGBModule::init(){
	//pinMode(Pinout::Assignment::TFT_CS, OUTPUT);
	//digitalWrite(Pinout::Assignment::TFT_CS	, HIGH); 
	tft.begin();
	
	uint8_t x = tft.readcommand8(ILI9341_RDMODE);
	Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDMADCTL);
	Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDPIXFMT);
	Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDIMGFMT);
	Serial.print("Image Format: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDSELFDIAG);
	Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
	
	tft.setRotation(uint8_t(DisplayRGBModule::DEFAULT_ROTATION));
	tft.setScrollMargins(0, 0);

	drawStaticContent();
		
	//timedAnimation.setup(anim, 10);
	frameStartTime = millis();
	return InitializationState::Initialized;
}

gui::Window flyingArrow{25, 130, &DPS_ArrowDownBMP};

void DisplayRGBModule::run(){
	uint32_t delta = millis() - frameStartTime;

	if(delta >= mi_targetFpsDeltaMs){
		frameStartTime = millis();
		drawDynamicContent();	

		uint32_t fps = 1000/delta;

		tft.fillRect(10, 200, 35, 15, CLEAR_COLOR);
		tft.setCursor(10, 200);
		tft.setTextColor(ILI9341_WHITE); 
		tft.setTextSize(2);
		char s_fps[6];
		itoa(fps, s_fps, 10);
		tft.println(s_fps);


		if(averageSamples < 30){
			averageFPS += fps;

			averageSamples++;
		}
		else {
			tft.fillRect(50, 200, 35, 15, CLEAR_COLOR);
			itoa(averageFPS / averageSamples, s_fps, 10);
			tft.setCursor(50, 200);
			tft.println(s_fps);
			averageFPS = averageFPS / averageSamples;
			averageSamples = 0;
		}

		
	}
}



void DisplayRGBModule::drawStaticContent(){	
	clearWithGrid(gui::Position{0, 0}, gui::Size{tft.width(), tft.height()});

	int16_t screenWidth = tft.width();
	gui::Window logoWindow{10, 30, &DPS_LogoSmall};

	drawWindowBitPixel(logoWindow, ILI9341_YELLOW);
	gui::drawHorizontalSeparatorWithBorders(tft, 1, logoWindow.getPosition().y + 35, screenWidth, 4);

	logoWindow.setPosition({10, 262});
	logoWindow.forceUpdate();
	drawWindowBitPixel(logoWindow, ILI9341_YELLOW);
	gui::drawHorizontalSeparatorWithBorders(tft, 1, logoWindow.getPosition().y - 10, screenWidth, 4);


/*
	drawWindowBitPixel(gui::Window{25,100, &DPS_ArrowUpBMP}, ILI9341_YELLOW);
	drawWindowBitPixel(gui::Window{50,100, &DPS_ArrowUpBMP}, ILI9341_YELLOW);
	drawWindowBitPixel(gui::Window{75,100, &DPS_ArrowDownBMP}, ILI9341_YELLOW);
	drawWindowBitPixel(gui::Window{100,100, &DPS_ArrowUpBMP}, ILI9341_YELLOW);
	
	drawWindowBitPixel(gui::Window{125,100, &DPS_ArrowRightBMP}, ILI9341_YELLOW);
	drawWindowBitPixel(gui::Window{150,100, &DPS_ArrowUpBMP}, ILI9341_YELLOW);
	drawWindowBitPixel(gui::Window{175,100, &DPS_ArrowLeftBMP}, ILI9341_YELLOW);
	drawWindowBitPixel(gui::Window{200,100, &DPS_ArrowUpBMP}, ILI9341_YELLOW);
	drawWindowBitPixel(gui::Window{225,100, &DPS_ArrowUpBMP}, ILI9341_YELLOW);*/


	//drawGeneratedGridPattern(0,200, 200, 200, 10, ILI9341_NAVY, CLEAR_COLOR -1, -1);
	
}


void DisplayRGBModule::drawDynamicContent() {


	if(mb_redraw){

		uint8_t imageIdx = 0;
		constexpr uint8_t slotOffsetY = 7;
		for(uint8_t slotIdx = 0; slotIdx < ARROW_MAX_SLOTS; slotIdx++){
			uint8_t slotOffsetX = m_slotOffsetsX[slotIdx];
			const Option<Arrow>& arrow = m_slots[slotIdx];

			if(const Arrow* p_arrow = arrow.ptr_value()){
				for(const ArrowToImageMapping& entry : imageMapping){
					if(entry.arrow == *p_arrow){
						
						drawWindowBitPixel(gui::Window{slotOffsetX, slotOffsetY, entry.image}, ILI9341_YELLOW, Some(gui::Position{slotOffsetX, slotOffsetY}));
						//small_display.drawBitmap(slotOffsetX, slotOffsetY, entry.image, ARROW_WIDTH, ARROW_HEIGHT, SSD1306_WHITE);
						break;
					}
				}
				
			}
			else {
				clearWithGrid(gui::Position{slotOffsetX, slotOffsetY}, gui::Size{ARROW_WIDTH, ARROW_HEIGHT});
			}	
		}

		mb_redraw = false;
	}
	// IDLE
	else {
		if(i > 230){
			i = 0;
		}

		gui::Position lastPosition =  flyingArrow.getPosition();
		flyingArrow.setPosition({i, 100});
		drawWindowBitPixel(flyingArrow, ILI9341_YELLOW, Some(lastPosition));
		i+=8;
	}

	
}

} // rgb display module











