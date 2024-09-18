#define USE_DMA
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include "displayrgb.h"
#include "display_resources.h"

#include "timer.h"
#include "arrowslots.h"


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



constexpr gui::Color565 CLEAR_COLOR 					= gui::ConvertRGBtoRGB565(0,140,235);//ILI9341_DARKCYAN;
constexpr gui::Color565 GRIDLINE_COLOR					= gui::ConvertRGBtoRGB565(0,140,240);//ILI9341_NAVY;
constexpr gui::Color565 HELL_MAIN_COLOR 				= ILI9341_YELLOW;
constexpr gui::Color565 INVALID_COMBINATION_COLOR		= gui::ConvertRGBtoRGB565(255,0,0);//ILI9341_RED;
constexpr gui::Color565 OUTLINE_COLOR					= ILI9341_BLACK;
constexpr gui::Color565 SELECTOR_COLOR					= gui::ConvertRGBtoRGB565(255,150,0);//ILI9341_ORANGE;
constexpr gui::Color565 SELECTOR_OUTLINE_COLOR			= ILI9341_BLACK;

constexpr int16_t GRID_LINES_OFFSET_X 	= +1;
constexpr int16_t GRID_LINES_OFFSET_Y 	= +5;
constexpr int16_t GRID_SPACING			= 29;

constexpr int16_t ARROWS_OFFSET_X		= 26;
constexpr int16_t ARROWS_OFFSET_Y		= 104;
constexpr int16_t SELECTOR_ARROW_OFFSET = -3;



constexpr gui::Position TEXT_FRAME_POSITION = {.x = 19, .y = 160};
constexpr gui::Size TEXT_FRAME_SIZE = {.width = 201, .height = 80};
//32, 173
constexpr int16_t TEXT_SUGGESTION_PRIMARY_POSITION_X	= TEXT_FRAME_POSITION.x+13;
constexpr int16_t TEXT_SUGGESTION_PRIMARY_POSITION_Y	= TEXT_FRAME_POSITION.y+13;
constexpr int16_t ARROWS_SUGGESTION_PRIMARY_OFFSET_X	= TEXT_FRAME_POSITION.x+20;
constexpr int16_t ARROWS_SUGGESTION_PRIMARY_OFFSET_Y	= TEXT_FRAME_POSITION.y+24;

//32, 205
constexpr int16_t TEXT_SUGGESTION_SECONDARY_POSITION_X	= TEXT_FRAME_POSITION.x+13;
constexpr int16_t TEXT_SUGGESTION_SECONDARY_POSITION_Y	= TEXT_FRAME_POSITION.y+45;
constexpr int16_t ARROWS_SUGGESTION_SECONDARY_OFFSET_X	= TEXT_FRAME_POSITION.x+20;
constexpr int16_t ARROWS_SUGGESTION_SECONDARY_OFFSET_Y	= TEXT_FRAME_POSITION.y+58;
 

constexpr int16_t ARROWS_OFFSETS_HORIZONTAL[ARROW_MAX_SLOTS] = {
	0, 	
	27, 
	54, 
	81, 
	108, 
	135,
	162,
};

constexpr int16_t ARROWS_TINY_OFFSETS_HORIZONTAL[ARROW_MAX_SLOTS] = {
	0, 	
	17, 
	34, 
	51, 
	68, 
	85,
	102,
};



Adafruit_ILI9341 tft(
	Pinout::Assignment::TFT_CS,
	Pinout::Assignment::TFT_DC
);

static uint32_t frameStartTime = 0;
static uint32_t averageFPS = 0;
static uint32_t averageSamples = 300;
static gui::Color565 slotArrowColor = HELL_MAIN_COLOR; 



const ArrowToImageMapping bigArrowMapping[]{
	{Arrow::UP,		&DPS_ArrowUpBigBMP},
	{Arrow::DOWN,	&DPS_ArrowDownBigBMP},
	{Arrow::LEFT,	&DPS_ArrowLeftBigBMP},
	{Arrow::RIGHT,	&DPS_ArrowRightBigBMP},
};

const ArrowToImageMapping tinyArrowMapping[]{
	{Arrow::UP,		&DPS_ArrowUpTinyBMP},
	{Arrow::DOWN,	&DPS_ArrowDownTinyBMP},
	{Arrow::LEFT,	&DPS_ArrowLeftTinyBMP},
	{Arrow::RIGHT,	&DPS_ArrowRightTinyBMP},
};


static gui::Window arrowWindowSlots[ARROW_MAX_SLOTS] = {
	gui::Window(ARROWS_OFFSETS_HORIZONTAL[0] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, true),
	gui::Window(ARROWS_OFFSETS_HORIZONTAL[1] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, true),
	gui::Window(ARROWS_OFFSETS_HORIZONTAL[2] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, true),
	gui::Window(ARROWS_OFFSETS_HORIZONTAL[3] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, true),
	gui::Window(ARROWS_OFFSETS_HORIZONTAL[4] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, true),
	gui::Window(ARROWS_OFFSETS_HORIZONTAL[5] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, true),
	gui::Window(ARROWS_OFFSETS_HORIZONTAL[6] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, true),
};

static gui::Window suggestionArrows[][ARROW_MAX_SLOTS] = {
	// PRIMARY SUGGESTION ARROWS
	{
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[0] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[1] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[2] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[3] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[4] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[5] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[6] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, true),
	},
	// SECONDARY SUGGESTION ARROWS
	{
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[0] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[1] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[2] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[3] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[4] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[5] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[6] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, true),
	}
};


static gui::Window slotUpperSelection(
	640, ARROWS_OFFSET_Y - BIG_SELECTOR_HEIGHT - 5, &DPS_ArrowSelectorUpperBMP, true
);

static gui::Window slotLowerSelection(
	640, ARROWS_OFFSET_Y + BIG_ARROW_HEIGHT + 5, &DPS_ArrowSelectorLowerBMP, true
);


static gui::Position selectedUpperSlotPreviousPosition = {-100,-100};
static gui::Position selectedLowerSlotPreviousPosition = {-100.-100};


/*gui::AnimatedMovement animEagle1(
	gui::Window(0, 0, &DPS_Eagle1),
	gui::Position{0, 100},	gui::Position{200, 100},
	2000,
	true
);*/


static gui::AnimatedMovement lowPriorityAnimations[] = {
/*	gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_SmallStarOneBMP),
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



struct WindowColorMapping {
	const gui::Window* windowPtr;
	const ColorAndOutline color;
} const PROGMEM windowColorMapping[] = {
	//{.windowPtr = &animEagle1.window, .color = {.mainColor = ILI9341_RED, .outlineColor = OUTLINE_COLOR}},
	//{.windowPtr = &lowPriorityAnimations[0].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	//{.windowPtr = &lowPriorityAnimations[1].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	
	/*{.windowPtr = &lowPriorityAnimations[2].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[3].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[4].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[5].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[6].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	{.windowPtr = &lowPriorityAnimations[7].window, .color = {.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR}},
	*/
};

static void clearWithDarkGrid(gui::Position pos, gui::Size size){
	//tft.fillRect(pos.x, pos.y, size.width, size.height, ILI9341_BLACK);
	gui::drawGeneratedGridPattern(tft, pos.x, pos.y, size.width, size.height, GRID_SPACING, gui::ConvertRGBtoRGB565(100,0,100), gui::ConvertRGBtoRGB565(70,70,70), GRID_LINES_OFFSET_X, GRID_LINES_OFFSET_Y);
}

static void clearWithGrid(gui::Position pos, gui::Size size){
	gui::drawGeneratedGridPattern(tft, pos.x, pos.y, size.width, size.height, GRID_SPACING, GRIDLINE_COLOR, CLEAR_COLOR, GRID_LINES_OFFSET_X, GRID_LINES_OFFSET_Y);
}
/*
static void clearWithGrid(gui::Position pos, gui::Size size){
	gui::drawGeneratedGridPattern(tft, pos.x, pos.y, size.width, size.height, GRID_SPACING, ILI9341_DARKGREY, ILI9341_DARKGREEN, GRID_LINES_OFFSET_X, GRID_LINES_OFFSET_Y);
}*/

static void drawWindowBitPixel(const gui::Window& window, gui::Color565 color, Option<gui::Color565> maybeOutline = None<gui::Color565>(), Option<gui::Position> clearPrevious = None<gui::Position>()){
	if(const gui::Position* p_clearPosition = clearPrevious.ptr_value()){
		gui::drawWindowBitPixel(tft, window, color, maybeOutline, Some(gui::ClearSettings{.position = *p_clearPosition, .clearFn = clearWithGrid}));
	}
	else {
		gui::drawWindowBitPixel(tft, window, color, maybeOutline);
	}
}

static void drawWindowBitPixelWithDarkGrid(const gui::Window& window, gui::Color565 color, Option<gui::Color565> maybeOutline = None<gui::Color565>(), Option<gui::Position> clearPrevious = None<gui::Position>()){
	if(const gui::Position* p_clearPosition = clearPrevious.ptr_value()){
		gui::drawWindowBitPixel(tft, window, color, maybeOutline, Some(gui::ClearSettings{.position = *p_clearPosition, .clearFn = clearWithDarkGrid}));
	}
	else {
		gui::drawWindowBitPixel(tft, window, color, maybeOutline);
	}
}


static ColorAndOutline matchWindowWithColor(const gui::Window* windowPtr){
	for(const WindowColorMapping& mappingEntry : windowColorMapping){
		WindowColorMapping entry{nullptr, ColorAndOutline{0,0}};
		PROGMEM_READ_STRUCTURE(&entry, &mappingEntry);
		if(entry.windowPtr == windowPtr){
			return entry.color;
		}
	}
	return ColorAndOutline{.mainColor = HELL_MAIN_COLOR, .outlineColor = OUTLINE_COLOR};
}


namespace module{ // display

void DisplayRGBModule::setTargetFPS(uint8_t fps){
	mi_targetFpsDeltaMs = 1000/uint16_t(fps);
}

void DisplayRGBModule::showArrow(uint8_t slot, Option<Arrow> arrow) {
	if(slot < ARROW_MAX_SLOTS) {
		gui::Window& arrowWindow = arrowWindowSlots[slot];
		if(const Arrow* p_arrow = arrow.ptr_value()){			
			arrowWindow.setHidden(false);
			for(const ArrowToImageMapping& entry : bigArrowMapping){
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
			
			selectedUpperSlotPreviousPosition = slotUpperSelection.isHidden() ? gui::Position{640,640} : slotUpperSelection.getPosition();
			selectedLowerSlotPreviousPosition = slotLowerSelection.isHidden() ? gui::Position{640,640} : slotLowerSelection.getPosition();
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
void DisplayRGBModule::showStratagemSuggestion(Option<Stratagem> maybeStratagem, StratagemSuggestion suggestion){
	Stratagem stratagem = maybeStratagem.valueOr(Stratagem::NUM_OF_STRATAGEMS);
	const char* s_stratagemDisplayName = ArrowSlots::GetStratagemName(stratagem);

	switch(suggestion) {
		case StratagemSuggestion::PRIMARY:
			if(ms_primarySuggestionText != s_stratagemDisplayName){
				ms_primarySuggestionText = s_stratagemDisplayName;
			}
			else {
				return;
			}
	
		break;

		case StratagemSuggestion::SECONDARY:
			if(ms_secondarySuggestionText != s_stratagemDisplayName){
				ms_secondarySuggestionText = s_stratagemDisplayName;
			}
			else {
				return;
			}
		break;
	}
	
	Arrow arrowCombination[ARROW_MAX_SLOTS];
	uint8_t arrowCombinationLength = ArrowSlots::GetStratagemArrows(stratagem, arrowCombination);

	gui::Window (&suggestionArrowsEntry)[ARROW_MAX_SLOTS] = suggestionArrows[size_t(suggestion)];
	for(uint8_t suggestionSlotIdx = 0; suggestionSlotIdx < ARROW_MAX_SLOTS; suggestionSlotIdx++){
		bool showArrow = suggestionSlotIdx < arrowCombinationLength;
		gui::Window& suggestionArrow = suggestionArrowsEntry[suggestionSlotIdx];
		//suggestionArrowsEntry[arrowIdx].
		if(showArrow){
			Arrow arrow = arrowCombination[suggestionSlotIdx];
			for(const ArrowToImageMapping& entry : tinyArrowMapping){
				if(entry.arrow == arrow){
					suggestionArrow.setImageBuffer(entry.image);
				}
			}
		}
		
		suggestionArrowsEntry[suggestionSlotIdx].setHidden(!showArrow);
		
	}	

	mb_textChanged = true;
}


void DisplayRGBModule::showOutcome(Option<Stratagem> maybeStratagem, bool show = true){
	const char* outcomeText;
	if(!show){
		outcomeText = EMPTY_PROGMEM_STRING;
	}
	else if(const Stratagem* p_stratagem = maybeStratagem.ptr_value()){
		outcomeText = PSTR("SUCCESSFUL");
		slotArrowColor = HELL_MAIN_COLOR;
		mb_wasSuccessful = true;
	}
	else{
		mb_wasSuccessful = false;
		slotArrowColor = INVALID_COMBINATION_COLOR;
		outcomeText = PSTR("FAILED");
	}

	for(gui::Window& arrowWindowSlot : arrowWindowSlots){
		arrowWindowSlot.forceUpdate();
	}
	
	mb_outcomeChanged = outcomeText != ms_outcomeText;
	ms_outcomeText = outcomeText;
	
	
}


void DisplayRGBModule::reset() {
	for(gui::Window& arrowWindow : arrowWindowSlots){
		arrowWindow.setHidden(true);
	}

	showSlotSelection(None<uint8_t>());
	selectedUpperSlotPreviousPosition = slotUpperSelection.getPosition();
	selectedLowerSlotPreviousPosition = slotLowerSelection.getPosition();

	//m_selectedSlot = None<uint8_t>();

	showStratagemSuggestion(None<Stratagem>(), PRIMARY);
	showStratagemSuggestion(None<Stratagem>(), SECONDARY);
	
	showOutcome(None<Stratagem>(), false);

	wobble(1700, 5);

	slotArrowColor = HELL_MAIN_COLOR;

	update();
}

void DisplayRGBModule::update(){
	mb_redraw = true;
}

void DisplayRGBModule::wobble(uint16_t timeToWobble, uint16_t amountOfWobble){
	mi_wobbleTargetTime = timeToWobble;
	mi_wobbleStart = 0;
	mi_wobbleStop = amountOfWobble;
	mi_wobbleStartTime = millis();
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
	tft.setTextSize(1);
	//tft.setScrollMargins(0, tft.height());

	drawStaticContent();
	
	/*for(gui::Window& primaryArrowWindow : primarySuggestionArrows){
		primaryArrowWindow.setImageBuffer(&DPS_ArrowRightTinyBMP);
		primaryArrowWindow.setHidden(false);
	}

	for(gui::Window& secondaryArrowWindow : secondarySuggestionArrows){
		secondaryArrowWindow.setImageBuffer(&DPS_ArrowDownTinyBMP);
		secondaryArrowWindow.setHidden(false);
	}*/
	/*for(auto& suggestionArrowsEntry : suggestionArrows){
		for(gui::Window& suggestionArrow : suggestionArrowsEntry){
			
			
			suggestionArrow.setImageBuffer(&DPS_ArrowDownTinyBMP);
			suggestionArrow.setHidden(false);
		}
	}*/

	//timedAnimation.setup(anim, 10);
	frameStartTime = millis();
	return InitializationState::Initialized;
}


void DisplayRGBModule::run(){
	uint32_t delta = millis() - frameStartTime;

	if(delta >= mi_targetFpsDeltaMs){
		frameStartTime = millis();
		drawDynamicContent(delta);	

		/*uint32_t fps = 1000/delta;

		tft.fillRect(10, 0, 35, 15, CLEAR_COLOR);	
		tft.setCursor(10, 0);
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
			tft.fillRect(50, 0, 35, 15, CLEAR_COLOR);
			itoa(averageFPS / averageSamples, s_fps, 10);
			tft.setCursor(50, 0);
			tft.println(s_fps);
			averageFPS = averageFPS / averageSamples;
			averageSamples = 0;
		}*/

		
	}
}



void DisplayRGBModule::drawStaticContent(){	
	clearWithGrid(gui::Position{0, 0}, gui::Size{tft.width(), tft.height()});

	int16_t screenWidth = tft.width();
	gui::Window logoWindow{10, 30, &DPS_LogoSmall};

	drawWindowBitPixel(logoWindow, HELL_MAIN_COLOR, Some(OUTLINE_COLOR));
	gui::drawHorizontalSeparatorWithBorders(tft, 1, logoWindow.getPosition().y + 35, screenWidth, 4);

	logoWindow.setPosition({10, 262});
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
		slotFrameSize.width-6,slotFrameSize.height-6,
		ILI9341_BLACK
	);
	clearWithDarkGrid(
		{slotFramePosition.x+4, slotFramePosition.y+4},
		{slotFrameSize.width-10,slotFrameSize.height-10}
	);


	tft.fillRect(
		TEXT_FRAME_POSITION.x, TEXT_FRAME_POSITION.y,
		TEXT_FRAME_SIZE.width, TEXT_FRAME_SIZE.height,
		ILI9341_YELLOW
	);
	//clearWithGrid({19+4, 85+4}, {201-9,62 - 9});
	tft.drawRect(
		TEXT_FRAME_POSITION.x+3,TEXT_FRAME_POSITION.y+3,
		TEXT_FRAME_SIZE.width-6,TEXT_FRAME_SIZE.height-6,
		ILI9341_DARKGREY
	);
	clearWithDarkGrid(
		{TEXT_FRAME_POSITION.x+4, TEXT_FRAME_POSITION.y+4},
		{TEXT_FRAME_SIZE.width-10,TEXT_FRAME_SIZE.height-8}
	);
	
	//arrow placeholder

/*
	gui::Window w = primarySuggestionArrows[7];
	w.setPosition({TEXT_FRAME_POSITION.x+15, TEXT_FRAME_POSITION.y+25});
	w.setImageBuffer(tinyArrowMapping[0].image);
	w.setImageBuffer(tinyArrowMapping[1].image);
	w.setImageBuffer(tinyArrowMapping[2].image);
	w.setImageBuffer(tinyArrowMapping[3].image);
	w.setHidden(false);
	drawWindowBitPixel(w, HELL_MAIN_COLOR);

	//arrow placeholder

	tft.fillRect(
		TEXT_FRAME_POSITION.x+15, TEXT_FRAME_POSITION.y+57,
		11,11,
		HELL_MAIN_COLOR
	);*/

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

	//if(mi_wobbleAmountY != 0){
		uint16_t elapsedTime = millis() - mi_wobbleStartTime;
		
		int16_t newPosition = gui::lerp(mi_wobbleStart, mi_wobbleStop, mi_wobbleTargetTime, elapsedTime);
		

		//Serial.println(newPosition);
		tft.scrollTo(newPosition);
		//delay(1000);
		if(elapsedTime > mi_wobbleTargetTime){
			int16_t tmp = mi_wobbleStop;
			mi_wobbleStop = mi_wobbleStart;
			mi_wobbleStart = tmp;
			mi_wobbleStartTime = millis();
			
		}
	//}
	
	


	if(mb_redraw){

		for(gui::Window& arrowWindow : arrowWindowSlots){
			drawWindowBitPixelWithDarkGrid(arrowWindow, slotArrowColor, Some(OUTLINE_COLOR), Some(arrowWindow.getPosition()));
			arrowWindow.updated();
		}

		drawWindowBitPixelWithDarkGrid(slotUpperSelection, SELECTOR_COLOR, Some(SELECTOR_OUTLINE_COLOR), Some(selectedUpperSlotPreviousPosition));
		slotUpperSelection.updated();

		drawWindowBitPixelWithDarkGrid(slotLowerSelection, SELECTOR_COLOR, Some(SELECTOR_OUTLINE_COLOR), Some(selectedLowerSlotPreviousPosition));
		slotLowerSelection.updated();

		mb_redraw = false;
	}
	else if(mb_textChanged) {
		// PRIMARY SUGGESTION
		clearWithDarkGrid(
			{TEXT_SUGGESTION_PRIMARY_POSITION_X, TEXT_SUGGESTION_PRIMARY_POSITION_Y},
			{175, 8}
		);

		tft.setCursor(TEXT_SUGGESTION_PRIMARY_POSITION_X, TEXT_SUGGESTION_PRIMARY_POSITION_Y);
		tft.setTextColor(ILI9341_ORANGE);
		tft.println((const __FlashStringHelper*) ms_primarySuggestionText);
		
		/*for(gui::Window& primaryArrowWindow : primarySuggestionArrows){
			
			//drawWindowBitPixel(primaryArrowWindow, HELL_MAIN_COLOR, None<gui::Color565>(), Some(primaryArrowWindow.getPosition()));
			gui::drawWindowBitPixel(tft, primaryArrowWindow, HELL_MAIN_COLOR, None<gui::Color565>(), Some(gui::ClearSettings{.position = primaryArrowWindow.getPosition(), .clearFn = clearWithBlack}));
			primaryArrowWindow.updated();
		}*/

		// SECONDARY SUGGESTION
		clearWithDarkGrid(
			{TEXT_SUGGESTION_SECONDARY_POSITION_X, TEXT_SUGGESTION_SECONDARY_POSITION_Y},
			{175, 8}
		);

		tft.setCursor(TEXT_SUGGESTION_SECONDARY_POSITION_X, TEXT_SUGGESTION_SECONDARY_POSITION_Y);
		tft.setTextColor(ILI9341_GREENYELLOW);
		tft.println((const __FlashStringHelper*)ms_secondarySuggestionText);
		
		for(auto& suggestionArrowsEntry : suggestionArrows){
			for(gui::Window& suggestionArrow : suggestionArrowsEntry){
				
				drawWindowBitPixelWithDarkGrid(suggestionArrow, HELL_MAIN_COLOR, None<gui::Color565>(), suggestionArrow.getPosition());
				suggestionArrow.updated();
			}
		}
/*
		for(gui::Window& secondaryArrowWindow : secondarySuggestionArrows){
			
			//drawWindowBitPixel(primaryArrowWindow, HELL_MAIN_COLOR, None<gui::Color565>(), Some(primaryArrowWindow.getPosition()));
			gui::drawWindowBitPixel(tft, secondaryArrowWindow, HELL_MAIN_COLOR, None<gui::Color565>(), Some(gui::ClearSettings{.position = secondaryArrowWindow.getPosition(), .clearFn = clearWithBlack}));
			secondaryArrowWindow.updated();
		}
*/



		mb_textChanged = false;
	}
	else if(mb_outcomeChanged){
		int16_t outcomeTextX = mb_wasSuccessful ? 90 : 95;
		clearWithGrid({ int16_t(outcomeTextX -10), 73}, {70, 8});
		tft.setCursor(outcomeTextX, 73);
		tft.setTextColor(mb_wasSuccessful ? ILI9341_GREEN : ILI9341_RED);
		tft.println((const __FlashStringHelper*)ms_outcomeText);
		mb_outcomeChanged = false;
	}
	// IDLE
	else {
		static uint8_t lowPriorityAnimationsIndex = 0;
		/*gui::Position oldPositionEagle1 = animEagle1.animateMovement();
		Option<gui::Color565> maybeEagle1Color = matchWindowWithColor(&animEagle1.window);
		if(const gui::Color565* p_color = maybeEagle1Color.ptr_value()){
			drawWindowBitPixel(animEagle1.window, *p_color, Some(oldPositionEagle1));
		}*/
		

		
		if(lowPriorityAnimationsIndex >= CONST_LENGTH(lowPriorityAnimations)){
			lowPriorityAnimationsIndex = 0;
		}

		gui::AnimatedMovement* p_animation;
		gui::Position oldPosition;
		do {
			if(lowPriorityAnimationsIndex >= CONST_LENGTH(lowPriorityAnimations)){
				return;
			}
			p_animation = &lowPriorityAnimations[lowPriorityAnimationsIndex];
			oldPosition = p_animation->animateMovement();

			lowPriorityAnimationsIndex++;
		}while (!p_animation->window.needsUpdate());
		
		ColorAndOutline matchedColor = matchWindowWithColor(&p_animation->window);

		drawWindowBitPixel(p_animation->window, matchedColor.mainColor, Some(matchedColor.outlineColor), Some(oldPosition));
		if(p_animation->isMirroredY()){
			int16_t halfDisplayWidth = tft.width();

			gui::Position positionMirrored = p_animation->window.getPosition();
			gui::Position oldPositionMirrored = oldPosition;
			positionMirrored.y = 70 - (positionMirrored.y - halfDisplayWidth);
			oldPositionMirrored.y = 70 - (oldPositionMirrored.y - halfDisplayWidth);	
			drawWindowBitPixel(gui::Window(positionMirrored.x, positionMirrored.y, p_animation->window.getImageBuffer()), matchedColor.mainColor, Some(matchedColor.outlineColor), Some(oldPositionMirrored));				
		}
		p_animation->window.updated();

		lowPriorityAnimationsIndex++;
	

		
	}

	
}

} // rgb display module











