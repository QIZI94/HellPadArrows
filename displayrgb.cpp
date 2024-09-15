#define USE_DMA
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include "displayrgb.h"
#include "display_resources.h"

#include "timer.h"


struct ArrowToImageMapping{
	constexpr ArrowToImageMapping(Arrow arrow, const gui::ImageBuffer* image)
	: arrow(arrow), image(image) {}

	const Arrow arrow;
	const gui::ImageBuffer* image = nullptr;
};

struct ColorAndOutline{
	gui::Color565 mainColor;
	gui::Color565 outlineColor;
};



constexpr gui::Color565 CLEAR_COLOR 			= ILI9341_DARKCYAN;
constexpr gui::Color565 LINE_COLOR 				= ILI9341_NAVY;
constexpr gui::Color565 HELL_MAIN_COLOR 		= ILI9341_YELLOW;
constexpr gui::Color565 OUTLINE_COLOR			= ILI9341_BLACK;
constexpr gui::Color565 SELECTOR_COLOR			= ILI9341_RED;
constexpr gui::Color565 SELECTOR_OUTLINE_COLOR	= ILI9341_BLACK;
constexpr int16_t GRID_LINES_OFFSET_X 	= +1;
constexpr int16_t GRID_LINES_OFFSET_Y 	= +5;
constexpr int16_t GRID_SPACING 			= 29;

constexpr int16_t ARROWS_TOP_OFFSET 	= 104;
constexpr int16_t ARROWS_LEFT_OFFSET	= 26;
constexpr int16_t SELECTOR_ARROW_OFFSET = -3;


constexpr int16_t ARROWS_LEFT_OFFSETS[ARROW_MAX_SLOTS] = {
	0, 	
	27, 
	54, 
	81, 
	108, 
	135,
	162,
};



Adafruit_ILI9341 tft(
	Pinout::Assignment::TFT_CS,
	Pinout::Assignment::TFT_DC
);

static TimedExecution10ms timedAnimation;
static uint32_t frameStartTime = 0;
static uint32_t averageFPS = 0;
static uint32_t averageSamples = 300;



const ArrowToImageMapping imageMapping[]{
	{Arrow::UP,		&DPS_ArrowUpBigBMP},
	{Arrow::DOWN,	&DPS_ArrowDownBigBMP},
	{Arrow::LEFT,	&DPS_ArrowLeftBigBMP},
	{Arrow::RIGHT,	&DPS_ArrowRightBigBMP},
};


static gui::Window arrowWindowSlots[ARROW_MAX_SLOTS] = {
	gui::Window(ARROWS_LEFT_OFFSETS[0] + ARROWS_LEFT_OFFSET, ARROWS_TOP_OFFSET, nullptr, true),
	gui::Window(ARROWS_LEFT_OFFSETS[1] + ARROWS_LEFT_OFFSET, ARROWS_TOP_OFFSET, nullptr, true),
	gui::Window(ARROWS_LEFT_OFFSETS[2] + ARROWS_LEFT_OFFSET, ARROWS_TOP_OFFSET, nullptr, true),
	gui::Window(ARROWS_LEFT_OFFSETS[3] + ARROWS_LEFT_OFFSET, ARROWS_TOP_OFFSET, nullptr, true),
	gui::Window(ARROWS_LEFT_OFFSETS[4] + ARROWS_LEFT_OFFSET, ARROWS_TOP_OFFSET, nullptr, true),
	gui::Window(ARROWS_LEFT_OFFSETS[5] + ARROWS_LEFT_OFFSET, ARROWS_TOP_OFFSET, nullptr, true),
	gui::Window(ARROWS_LEFT_OFFSETS[6] + ARROWS_LEFT_OFFSET, ARROWS_TOP_OFFSET, nullptr, true),
};


static gui::Window slotUpperSelection(
	0, ARROWS_TOP_OFFSET - BIG_SELECTOR_HEIGHT - 5, &DPS_ArrowSelectorUpperBMP, true
);

static gui::Window slotLowerSelection(
	0, ARROWS_TOP_OFFSET + BIG_ARROW_HEIGHT + 5, &DPS_ArrowSelectorLowerBMP, true
);


static gui::Position selectedUpperSlotPreviousPosition = {-100,-100};
static gui::Position selectedLowerSlotPreviousPosition = {-100.-100};


gui::AnimatedMovement animEagle1(
	gui::Window(0, 0, &DPS_Eagle1),
	gui::Position{0, 100},	gui::Position{200, 100},
	2000,
	true
);


static gui::AnimatedMovement lowPriorityAnimations[] = {
	/*gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_Eagle1),
		gui::Position{0, 100},	gui::Position{200, 100},
		2000,
		true
	),*/


// STARS
	gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_SmallStarOneBMP),
		gui::Position{20,290},	gui::Position{10, 331},
		1200,
		true, false, true
	),
	gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_SmallStarOneBMP),
		gui::Position{50,290},	gui::Position{40, 331},
		1500,
		true, false, true
	),

	gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_SmallStarOneBMP),
		gui::Position{80,290},	gui::Position{70, 331},
		2000,
		true, false, true
	),

	gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_SmallStarOneBMP),
		gui::Position{115,292},	gui::Position{115, 331},
		1700,
		true, false, true
	),
	gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_SmallStarOneBMP),
		gui::Position{140,290},	gui::Position{170, 331},
		1300,
		true, false, true
	),

	gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_SmallStarOneBMP),
		gui::Position{170,290},	gui::Position{200, 331},
		1900,
		true, false, true
	),

	gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_SmallStarOneBMP),
		gui::Position{200,290},	gui::Position{230, 331},
		2200,
		true, false, true
	),

	
};
static uint8_t lowPriorityAnimationsIndex = 0;



struct WindowColorMapping {
	const gui::Window* windowPtr;
	const ColorAndOutline color;
} const PROGMEM windowColorMapping[] = {
	{.windowPtr = &animEagle1.window, .color = {.mainColor = ILI9341_RED, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[0].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[1].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[2].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[3].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[4].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[5].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[6].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[7].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	
};


static void clearWithGrid(gui::Position pos, gui::Size size){
	gui::drawGeneratedGridPattern(tft, pos.x, pos.y, size.width, size.height, GRID_SPACING, LINE_COLOR, CLEAR_COLOR, GRID_LINES_OFFSET_X, GRID_LINES_OFFSET_Y);
}

static void drawWindowBitPixel(const gui::Window& window, gui::Color565 color, Option<gui::Color565> maybeOutline = None<gui::Color565>(), Option<gui::Position> clearPrevious = None<gui::Position>()){
	if(const gui::Position* p_clearPosition = clearPrevious.ptr_value()){
		gui::drawWindowBitPixel(tft, window, color, maybeOutline, Some(gui::ClearSettings{.position = *p_clearPosition, .clearFn = clearWithGrid}));
	}
	else {
		gui::drawWindowBitPixel(tft, window, color, maybeOutline);
	}
}

static Option<ColorAndOutline> matchWindowWithColor(const gui::Window* windowPtr){
	for(const WindowColorMapping& mappingEntry : windowColorMapping){
		WindowColorMapping entry{nullptr, ColorAndOutline{0,0}};
		PROGMEM_READ_STRUCTURE(&entry, &mappingEntry);
		if(entry.windowPtr == windowPtr){
			return Some(entry.color);
		}
	}
	return None<ColorAndOutline>();
}


namespace module{ // display

void DisplayRGBModule::setTargetFPS(uint8_t fps){
	mi_targetFpsDeltaMs = 1000/uint16_t(fps);
}

void DisplayRGBModule::showText(const char *str_c){
	//mi_ strlen(str_c)
	if(ms_text == str_c){
		return;
	}
	ms_text = str_c;
	mb_textChanged = true;
}

void DisplayRGBModule::showArrow(uint8_t slot, Option<Arrow> arrow) {
	if(slot < ARROW_MAX_SLOTS) {
		gui::Window& arrowWindow = arrowWindowSlots[slot];
		if(const Arrow* p_arrow = arrow.ptr_value()){			
			arrowWindow.setHidden(false);
			for(const ArrowToImageMapping& entry : imageMapping){
				if(entry.arrow == *p_arrow){
					arrowWindow.setImageBuffer(entry.image);
				}
			}
		}
		else {
			arrowWindow.setHidden(true);
		}
		update();
	}
}

void DisplayRGBModule::showSlotSelection(Option<uint8_t> slot) {
	if(const uint8_t* p_slot = slot.ptr_value()){
		if(*p_slot < ARROW_MAX_SLOTS){
			
			selectedUpperSlotPreviousPosition = slotUpperSelection.isHidden() ? gui::Position{0,0} : slotUpperSelection.getPosition();
			selectedLowerSlotPreviousPosition = slotLowerSelection.isHidden() ? gui::Position{0,0} :slotLowerSelection.getPosition();
			//slotLowerSelection.setHidden(false);
			gui::Position arrowSlotWindowPosition
				= arrowWindowSlots[*p_slot].getPosition();
			arrowSlotWindowPosition.x += SELECTOR_ARROW_OFFSET;
			/*gui::Size arrowSlotWindowSize
				= arrowWindowSlots[*p_slot].getImageBuffer();*/
			arrowSlotWindowPosition.y = int16_t(arrowSlotWindowPosition.y + BIG_ARROW_HEIGHT + 5);
			slotLowerSelection.setPosition(arrowSlotWindowPosition);
			slotLowerSelection.setHidden(false);

			arrowSlotWindowPosition
				= arrowWindowSlots[*p_slot].getPosition();	
			arrowSlotWindowPosition.x += SELECTOR_ARROW_OFFSET;	
			
			arrowSlotWindowPosition.y = int16_t(arrowSlotWindowPosition.y - BIG_SELECTOR_HEIGHT - 5);
			slotUpperSelection.setPosition(arrowSlotWindowPosition);
			slotUpperSelection.setHidden(false);
			
			
			return;
		}

	}
	slotUpperSelection.setHidden(true);
	slotLowerSelection.setHidden(true);
	
	
}

void DisplayRGBModule::reset() {
	for(gui::Window& arrowWindow : arrowWindowSlots){
		arrowWindow.setHidden(true);
	}

	showSlotSelection(None<uint8_t>());
	selectedUpperSlotPreviousPosition = slotUpperSelection.getPosition();
	selectedLowerSlotPreviousPosition = slotLowerSelection.getPosition();

	//m_selectedSlot = None<uint8_t>();

	showText(nullptr);
	update();
}

void DisplayRGBModule::update(){
	mb_redraw = true;
}

void DisplayRGBModule::wobble(uint32_t changeDirectionAfterMS){

}


uint8_t DisplayRGBModule::getTargetFPS() const {
	return 1000/mi_targetFpsDeltaMs;
}

int16_t i = 0;



DisplayRGBModule::InitializationState DisplayRGBModule::init(){
	//pinMode(Pinout::Assignment::TFT_CS, OUTPUT);
	//digitalWrite(Pinout::Assignment::TFT_CS	, HIGH); 
	tft.begin();
	
	uint8_t x = tft.readcommand8(ILI9341_RDMODE);
	/*Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDMADCTL);
	Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDPIXFMT);
	Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDIMGFMT);
	Serial.print("Image Format: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDSELFDIAG);
	Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);*/
	
	tft.setRotation(uint8_t(DisplayRGBModule::DEFAULT_ROTATION));
	tft.setScrollMargins(0, tft.height());

	drawStaticContent();
		
	//timedAnimation.setup(anim, 10);
	frameStartTime = millis();
	return InitializationState::Initialized;
}

gui::Window flyingArrow{25, 130, &DPS_Eagle1};

void DisplayRGBModule::run(){
	uint32_t delta = millis() - frameStartTime;

	if(delta >= mi_targetFpsDeltaMs){
		frameStartTime = millis();
		drawDynamicContent(delta);	

		uint32_t fps = 1000/delta;

		tft.fillRect(10, 220, 35, 15, CLEAR_COLOR);	
		tft.setCursor(10, 220);
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
			tft.fillRect(50, 220, 35, 15, CLEAR_COLOR);
			itoa(averageFPS / averageSamples, s_fps, 10);
			tft.setCursor(50, 220);
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

	drawWindowBitPixel(logoWindow, HELL_MAIN_COLOR, Some(OUTLINE_COLOR));
	gui::drawHorizontalSeparatorWithBorders(tft, 1, logoWindow.getPosition().y + 35, screenWidth, 4);

	logoWindow.setPosition({10, 262});
	logoWindow.forceUpdate();
	drawWindowBitPixel(logoWindow, HELL_MAIN_COLOR, Some(OUTLINE_COLOR));
	gui::drawHorizontalSeparatorWithBorders(tft, 1, logoWindow.getPosition().y - 10, screenWidth, 4);

	constexpr gui::Position slotFramePosition = {.x = 19, .y = 85};
	constexpr gui::Size slotFrameSize = {.width = 201, .height = 62};

	
	tft.fillRect(
		slotFramePosition.x, slotFramePosition.y,
		slotFrameSize.width, slotFrameSize.height,
		ILI9341_YELLOW
	);
	//clearWithGrid({19+4, 85+4}, {201-9,62 - 9});
	tft.drawRect(
		slotFramePosition.x+3,slotFramePosition.y+3,
		slotFrameSize.width-6,slotFrameSize.height - 6,
		ILI9341_BLACK
	);
	clearWithGrid(
		{slotFramePosition.x+4, slotFramePosition.y+4},
		{slotFrameSize.width-10,slotFrameSize.height - 10}
	);

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


void DisplayRGBModule::drawDynamicContent(uint32_t delta) {
	/*TimedExecution10ms** begin = TimedExecution10ms::List::getTimedExecutionListBegin();
	if(*begin == nullptr){
		Serial.println("Is null");
	} */
	/*static uint32_t howManyFrames = 0;
	howManyFrames++;
	for(auto it = TimedExecution10ms::List::begin(); it.timedExecution != nullptr; ++it){
		
		//Serial.println(howManyTimes, 10);
		//Serial.println(" Is null");
		
		if(it->timer.isDown()){
			Serial.print("Frame skipped: ");
			Serial.println(howManyFrames, 10);
			return;
		}
	}*/


	if(mb_redraw){

		for(gui::Window& arrowWindow : arrowWindowSlots){
			drawWindowBitPixel(arrowWindow, HELL_MAIN_COLOR, Some(OUTLINE_COLOR), Some(arrowWindow.getPosition()));
			arrowWindow.updated();
		}

		drawWindowBitPixel(slotUpperSelection, SELECTOR_COLOR, Some(SELECTOR_OUTLINE_COLOR), Some(selectedUpperSlotPreviousPosition));
		slotUpperSelection.updated();

		drawWindowBitPixel(slotLowerSelection, SELECTOR_COLOR, Some(SELECTOR_OUTLINE_COLOR), Some(selectedLowerSlotPreviousPosition));
		slotLowerSelection.updated();

		mb_redraw = false;
	}
	else if(mb_textChanged) {
		clearWithGrid({20,180}, {int16_t(13*mi_previousTextSize), 16});
		mi_previousTextSize = strlen(ms_text);

		tft.setCursor(20, 180);
		tft.setTextSize(1);
		tft.setTextColor(ILI9341_ORANGE);
		if(ms_text != nullptr){
			tft.println(ms_text);
		}

		mb_textChanged = false;
	}
	// IDLE
	else {

		/*gui::Position oldPositionEagle1 = animEagle1.animateMovement();
		Option<gui::Color565> maybeEagle1Color = matchWindowWithColor(&animEagle1.window);
		if(const gui::Color565* p_color = maybeEagle1Color.ptr_value()){
			drawWindowBitPixel(animEagle1.window, *p_color, Some(oldPositionEagle1));
		}*/
		

		
		if(lowPriorityAnimationsIndex >= CONST_LENGTH(lowPriorityAnimations)){
			lowPriorityAnimationsIndex = 0;
		}

		gui::AnimatedMovement& animation = lowPriorityAnimations[lowPriorityAnimationsIndex];
		gui::Position oldPosition = animation.animateMovement();

		Option<ColorAndOutline> maybeColor = matchWindowWithColor(&animation.window);
		if(const ColorAndOutline* p_color = maybeColor.ptr_value()){
			drawWindowBitPixel(animation.window, p_color->mainColor, Some(p_color->outlineColor), Some(oldPosition));
			if(animation.isMirroredY()){
				int16_t halfDisplayWidth = tft.width();

				gui::Position positionMirrored = animation.window.getPosition();
				gui::Position oldPositionMirrored = oldPosition;
				positionMirrored.y = 70 - (positionMirrored.y - halfDisplayWidth);
				oldPositionMirrored.y = 70 - (oldPositionMirrored.y - halfDisplayWidth);	
				drawWindowBitPixel(gui::Window(positionMirrored.x, positionMirrored.y, animation.window.getImageBuffer()), p_color->mainColor, Some(p_color->outlineColor), Some(oldPositionMirrored));				
			}
		}
		else {
			drawWindowBitPixel(animation.window, ILI9341_PINK, None<gui::Color565>(), Some(oldPosition));
		}
		animation.window.updated();

		lowPriorityAnimationsIndex++;
	

		
	}

	
}

} // rgb display module











