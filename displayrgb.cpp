#define USE_DMA
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include "displayrgb.h"
#include "display_resources.h"

#include "timer.h"
#include "arrowslots.h"


struct ArrowToImageMapping{

	constexpr ArrowToImageMapping(Arrow arrow, gui::CompressedImageBuffer image, gui::Flip flip = gui::Flip::NONE)
	: image(image), arrow(arrow), flip(flip) {}
	ArrowToImageMapping(){}

//	const Arrow arrow;

	gui::CompressedImageBuffer image;
	struct {
		Arrow arrow : 4;
		gui::Flip flip : 4;
	};
};

static StaticTimer10ms delayScriptedAnimationTimer;

struct ScriptedDelay{
	uint16_t delayFor;
	constexpr ScriptedDelay(uint16_t delay) : delayFor(delay) {};


	bool runDelay() const {
		if(!delayScriptedAnimationTimer.isEnabled()){
			delayScriptedAnimationTimer.enable();
			delayScriptedAnimationTimer.reset(delayFor);
		}
		else if(delayScriptedAnimationTimer.isDown()) {
			delayScriptedAnimationTimer.disable();
			return true;
		}

		return false;
	}
};

struct ScriptedAction{
	using ScriptedFunction = bool(* const)();
	enum class ActionType : uint8_t{
		NONE,
		DELAY,
		WINDOW,
		ANIMATION,
		ANIMATION_NON_BLOCKING,
		FUNCTION
	};

	constexpr ScriptedAction() : window(nullptr), actionType(ActionType::NONE) {}
	constexpr ScriptedAction(ScriptedDelay scriptedDelay) : scriptedDelay(scriptedDelay), actionType(ActionType::DELAY){}
	constexpr ScriptedAction(gui::Window* window) : window(window), actionType(ActionType::WINDOW){}
	constexpr ScriptedAction(gui::AnimatedMovement* animation, bool nonBlocking = false)
		: animation(animation), actionType(nonBlocking ? ActionType::ANIMATION_NON_BLOCKING : ActionType::ANIMATION){}
	constexpr ScriptedAction(const ScriptedFunction function) : function(function), actionType(ActionType::FUNCTION){}

	union{
		ScriptedDelay scriptedDelay;
		gui::Window* const window;
		gui::AnimatedMovement* const animation;
		const ScriptedFunction function;
	};
	const ActionType actionType;


	bool run(){
		switch (actionType){
			case ScriptedAction::ActionType::DELAY:
				return scriptedDelay.runDelay();
			case ScriptedAction::ActionType::WINDOW:
				if(window->isHidden()){
					window->setHidden(false);
				}
				return true;
			
			case ScriptedAction::ActionType::ANIMATION:
				
		
				if(animation->isDisabled()){
					animation->setDisabled(false);
					animation->window.setHidden(false);
					animation->restart();
				}
				else if(animation->isFinished()){
					//animation->window.setHidden(true);
					animation->setDisabled(true);
					return true;
				}
				
				break;
			case ScriptedAction::ActionType::ANIMATION_NON_BLOCKING:
				animation->setDisabled(false);
				animation->window.setHidden(false);
				animation->restart();
				return true;

			case ScriptedAction::ActionType::FUNCTION:
				return function();
		
			default:
				break;
		}
		return false;
	}
	static constexpr ScriptedAction None() { return ScriptedAction(); }
};


struct SlowVerticalClearParams {
	gui::Position position;
	gui::Size size;
};

struct WobbleParams {
	uint16_t mi_wobbleStartTime = 0;
	uint16_t mi_wobbleTargetTime = 1700;
	int8_t mi_wobbleStart = 0;
	int8_t mi_wobbleStop = 5;
};
struct ProgressiveWobbleParams {
	uint16_t startWobbleTimeAmount;
	uint16_t endWobbleTimeAmount;
	uint16_t targetTime;
	uint16_t startTime;
	uint16_t previousWobbleTime;
	int8_t previousWobbleAmount;
};


struct FlashParams {
	uint16_t switchTime;
	uint8_t moduloCounter;
};

struct ExplosionParams {
	gui::Position8Bit position;
	uint8_t currentRadius;
	uint8_t targetRadius;

	gui::Color565 computeColor() const {
		return gui::lerpColor565(ILI9341_WHITE, ILI9341_ORANGE, targetRadius, currentRadius);
	}
};




//constexpr gui::Color565 CLEAR_COLOR 					= gui::ConvertRGBtoRGB565(0,140,235);//ILI9341_DARKCYAN;
//constexpr gui::Color565 GRIDLINE_COLOR					= gui::ConvertRGBtoRGB565(0,140,240);//ILI9341_NAVY;
constexpr gui::Color565 CLEAR_COLOR 					= ILI9341_DARKCYAN;
constexpr gui::Color565 GRIDLINE_COLOR					= gui::ConvertRGBtoRGB565(0,140,240);//ILI9341_NAVY;
constexpr gui::Color565 HELL_MAIN_COLOR 				= ILI9341_YELLOW;
constexpr gui::Color565 INVALID_COMBINATION_COLOR		= gui::ConvertRGBtoRGB565(255,0,0);//ILI9341_RED;
constexpr gui::Color565 OUTLINE_COLOR					= ILI9341_BLACK;
constexpr gui::Color565 SELECTOR_COLOR					= gui::ConvertRGBtoRGB565(255,150,0);//ILI9341_ORANGE;
constexpr gui::Color565 SELECTOR_OUTLINE_COLOR			= ILI9341_BLACK;
constexpr gui::Color565 EAGLE1_COLOR					= ILI9341_RED;

constexpr int16_t GRID_LINES_OFFSET_X 	= +1;
constexpr int16_t GRID_LINES_OFFSET_Y 	= +5;
constexpr int16_t GRID_SPACING			= 29;

constexpr int16_t ARROWS_OFFSET_X		= 26;
constexpr int16_t ARROWS_OFFSET_Y		= 104;
constexpr int16_t SELECTOR_ARROW_OFFSET = -3;



constexpr gui::Position TEXT_FRAME_POSITION = {.x = 19, .y = 155};
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



static Adafruit_ILI9341 tft(
	Pinout::Assignment::TFT_CS,
	Pinout::Assignment::TFT_DC
);

static uint32_t frameStartTime = 0;
static uint32_t averageFPS = 0;
static uint32_t averageSamples = 300;
static uint8_t targetWobbleAmount = 5;	




static const ArrowToImageMapping PROGMEM bigArrowMapping[]{
	{Arrow::UP,		DPS_ArrowUpBigBMP,		gui::Flip::NONE},
	{Arrow::DOWN,	DPS_ArrowUpBigBMP,		gui::Flip::VERTICALLY},
	{Arrow::LEFT,	DPS_ArrowLeftBigBMP,	gui::Flip::NONE},
	{Arrow::RIGHT,	DPS_ArrowLeftBigBMP,	gui::Flip::HORIZONTALLY},
};

static const ArrowToImageMapping PROGMEM tinyArrowMapping[]{
	{Arrow::UP,		DPS_ArrowUpTinyBMP,	gui::Flip::NONE},
	{Arrow::DOWN,	DPS_ArrowUpTinyBMP,	gui::Flip::VERTICALLY},
	{Arrow::LEFT,	DPS_ArrowRightTinyBMP,	gui::Flip::HORIZONTALLY},
	{Arrow::RIGHT,	DPS_ArrowRightTinyBMP, gui::Flip::NONE},
};

const gui::Color565 PROGMEM colorPaletteBuf[]{
	INVALID_COMBINATION_COLOR,
	HELL_MAIN_COLOR,
	SELECTOR_COLOR,
	EAGLE1_COLOR
	
};

enum class ColorPalette : uint8_t{
	INVALID_COMBINATION_COLOR,
	HELL_MAIN_COLOR,
	SELECTOR_COLOR,
	EAGLE1_COLOR
};


static gui::Window arrowArrayWindowSlots[][ARROW_MAX_SLOTS] = {
	// MAIN ARROWS
	{
		gui::Window(ARROWS_OFFSETS_HORIZONTAL[0] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_OFFSETS_HORIZONTAL[1] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_OFFSETS_HORIZONTAL[2] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_OFFSETS_HORIZONTAL[3] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_OFFSETS_HORIZONTAL[4] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_OFFSETS_HORIZONTAL[5] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_OFFSETS_HORIZONTAL[6] + ARROWS_OFFSET_X, ARROWS_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
	},
	// PRIMARY SUGGESTION ARROWS
	{
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[0] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[1] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[2] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[3] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[4] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[5] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[6] + ARROWS_SUGGESTION_PRIMARY_OFFSET_X, ARROWS_SUGGESTION_PRIMARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
	},
	// SECONDARY SUGGESTION ARROWS
	{
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[0] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[1] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[2] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[3] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[4] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[5] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Window(ARROWS_TINY_OFFSETS_HORIZONTAL[6] + ARROWS_SUGGESTION_SECONDARY_OFFSET_X, ARROWS_SUGGESTION_SECONDARY_OFFSET_Y, nullptr, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
	}

};


static gui::Window slotUpperSelection(
	640, ARROWS_OFFSET_Y - BIG_SELECTOR_HEIGHT - 5, DPS_ArrowSelectorLowerBMP, uint8_t(ColorPalette::SELECTOR_COLOR), true, gui::Flip::VERTICALLY
);

static gui::Window slotLowerSelection(
	640, ARROWS_OFFSET_Y + BIG_ARROW_HEIGHT + 5, DPS_ArrowSelectorLowerBMP, uint8_t(ColorPalette::SELECTOR_COLOR), true
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

// eagles ones
	/*gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_Eagle1Tiny),
		gui::Position{-300, 72},	gui::Position{350, 72},
		7000,
		true
	),
	gui::AnimatedMovement(
		gui::Window(0, 0, &DPS_Eagle1TinyFlipped),
		gui::Position{250, 237},	gui::Position{-350, 237},
		7000,
		true
	),*/
	
	gui::AnimatedMovement(
		gui::Window(320, 72, DPS_Eagle1Mid, uint8_t(ColorPalette::EAGLE1_COLOR), true),
		gui::Position{-20, 72},	gui::Position{320, 72},
		10000,
		gui::AnimatedMovement::FinishBehavior::RUN_ONCE_AND_HIDE,
		false
	),
	gui::AnimatedMovement(
		gui::Window(0, 0, DPS_500kgBombHorMid, uint8_t(ColorPalette::HELL_MAIN_COLOR)),
		gui::Position{-20, 72+6},	gui::Position{60, 72+6},
		2300,
		gui::AnimatedMovement::FinishBehavior::RUN_ONCE_AND_HIDE,
		true
	),
	gui::AnimatedMovement(
		gui::Window(0, 0, DPS_500kgBombHorMid, uint8_t(ColorPalette::HELL_MAIN_COLOR), true),
		gui::Position{60, 72+6},	gui::Position{110+1, 200+6},
		4000,
		gui::AnimatedMovement::FinishBehavior::RUN_ONCE_AND_HIDE,
		true
	),
	

// STARS
	gui::AnimatedMovement(
		gui::Window(0, 0, DPS_SmallStarOneBMP, uint8_t(ColorPalette::HELL_MAIN_COLOR)),
		gui::Position{20,290},	gui::Position{10, 331},
		1200,
		gui::AnimatedMovement::FinishBehavior::REPEAT, false, true
	),
	gui::AnimatedMovement(
		gui::Window(0, 0, DPS_SmallStarOneBMP, uint8_t(ColorPalette::HELL_MAIN_COLOR)),
		gui::Position{50,290},	gui::Position{40, 331},
		1500,
		gui::AnimatedMovement::FinishBehavior::REPEAT, false, true
	),

	gui::AnimatedMovement(
		gui::Window(0, 0, DPS_SmallStarOneBMP, uint8_t(ColorPalette::HELL_MAIN_COLOR)),
		gui::Position{80,290},	gui::Position{70, 331},
		2000,
		gui::AnimatedMovement::FinishBehavior::REPEAT, false, true
	),

	gui::AnimatedMovement(
		gui::Window(0, 0, DPS_SmallStarOneBMP, uint8_t(ColorPalette::HELL_MAIN_COLOR)),
		gui::Position{115,292},	gui::Position{115, 331},
		1700,
		gui::AnimatedMovement::FinishBehavior::REPEAT, false, true
	),
	gui::AnimatedMovement(
		gui::Window(0, 0, DPS_SmallStarOneBMP, uint8_t(ColorPalette::HELL_MAIN_COLOR)),
		gui::Position{140,290},	gui::Position{170, 331},
		1300,
		gui::AnimatedMovement::FinishBehavior::REPEAT, false, true
	),

	gui::AnimatedMovement(
		gui::Window(0, 0, DPS_SmallStarOneBMP, uint8_t(ColorPalette::HELL_MAIN_COLOR)),
		gui::Position{170,290},	gui::Position{200, 331},
		1900,
		gui::AnimatedMovement::FinishBehavior::REPEAT, false, true
	),

	gui::AnimatedMovement(
		gui::Window(0, 0, DPS_SmallStarOneBMP, uint8_t(ColorPalette::HELL_MAIN_COLOR)),
		gui::Position{200,290},	gui::Position{230, 331},
		2200,
		gui::AnimatedMovement::FinishBehavior::REPEAT, false, true
	),



	
};

//static TimedExecution10ms invertColorTimer;


WobbleParams wobbleParams;

Option<SlowVerticalClearParams> requestedSlowClear;
Option<ProgressiveWobbleParams> requestedProgressiveWobble;
Option<FlashParams> requestedScreenFlashing;
//Option<ExplosionParams> requestedExplosion;

Option<ExplosionParams> requestedExplosions[4];

static StaticTimer10ms screenFlashTimer;




static void setupWobble(uint16_t timeToWobble, uint8_t amountOfWobble){
	wobbleParams.mi_wobbleTargetTime = timeToWobble;
	wobbleParams.mi_wobbleStart = 0;
	wobbleParams.mi_wobbleStop = amountOfWobble;
	wobbleParams.mi_wobbleStartTime = millis();
}

static void requestSlowClear(gui::Position position, gui::Size size){
	requestedSlowClear = Some(
		SlowVerticalClearParams{
			.position = position,
			.size = {.width = size.width, .height = int16_t(position.y + size.height)}
		}
	);
}

static void requestProgressiveWobble(uint8_t amount, uint16_t startWobbleTime, uint16_t endWobbleTime, uint16_t duration){
	//mi_wobbleStart = amount; // last time here
	requestedProgressiveWobble = Some(
		ProgressiveWobbleParams{
			.startWobbleTimeAmount = startWobbleTime,
			.endWobbleTimeAmount = endWobbleTime,
			.targetTime = duration,
			.startTime = uint16_t(millis()),
			.previousWobbleTime = wobbleParams.mi_wobbleTargetTime,
			.previousWobbleAmount = wobbleParams.mi_wobbleStop > wobbleParams.mi_wobbleStart ? wobbleParams.mi_wobbleStop : wobbleParams.mi_wobbleStart
		}
	);
	module::Display.wobble(startWobbleTime, amount);

}

void requestScreenFlashing(uint16_t switchTime, uint8_t flashAmount, uint16_t delayFlashing = 0){
	if(switchTime == 0 || flashAmount == 0){
		delayFlashing = 0;
		requestedScreenFlashing = None;
	}
	else {
		requestedScreenFlashing = Some(
			FlashParams{
				.switchTime = switchTime,
				.moduloCounter = uint8_t(flashAmount * 2 + 1)
			}
		);
	}
	screenFlashTimer.reset(delayFlashing);

}

struct ExplosionInput{
	gui::Position8Bit position;
	uint8_t radius;
};

void requestExplosion(gui::Position8Bit position, uint8_t radius, uint8_t slot){
	requestedExplosions[slot] = Some(
		ExplosionParams{
			.position = position,
			.currentRadius = 0,
			.targetRadius = radius
		}
		
	);
}

void requestExplosions(const ExplosionInput inputParamsList[], uint8_t inputParamsCount){
	for(uint8_t slot = 0; slot < inputParamsCount; ++slot){
		requestExplosion(inputParamsList[slot].position, inputParamsList[slot].radius, slot);
		/*requestedExplosions[slot] = Some(
			ExplosionParams{
				.position = inputParamsList[slot].position,
				.currentRadius = 0,
				.targetRadius = inputParamsList[slot].radius
			}
			
		);*/
	}
}


void drawOptimizedExplosion(const ExplosionParams& p_explosionParams){
	
	uint16_t finalColor = p_explosionParams.computeColor();
	uint8_t circleOverlayPositionsMask = 0b01111000;
	
	//for(const gui::Position& circleOffsets : circleOverlayPositions){
	while(circleOverlayPositionsMask){
		gui::Position circleOffsets {
			.x = circleOverlayPositionsMask & 0x02,
			.y = circleOverlayPositionsMask & 0x01
		};
		circleOverlayPositionsMask>>=2;
		int16_t ddF_x = 1;
		int16_t x = 0;
		int16_t y = p_explosionParams.currentRadius;
		int16_t f = -p_explosionParams.currentRadius;
		int16_t ddF_y = -2 * p_explosionParams.currentRadius;
		uint8_t halfRadius = (p_explosionParams.currentRadius >> 1);
		
		tft.startWrite();

		while (x < y) {
			if (f >= 0) {
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;
			
			uint8_t yPolaritiesMask = 0b11001100;
			

			for(uint8_t idx = 0; idx < 8; ++idx){
				const uint8_t xPolarity = idx % 2;
				gui::Position offset = idx > 3 ? gui::Position{y,x} : gui::Position{x,y};
				
				offset.x = xPolarity ? -offset.x : offset.x;
				offset.y =  yPolaritiesMask & 0x01 ? -offset.y : offset.y;
				yPolaritiesMask>>=1;
				tft.writePixel(p_explosionParams.position.x - circleOffsets.x - offset.x, p_explosionParams.position.y - circleOffsets.y - offset.y  - halfRadius, finalColor);
				
			}


		}
		tft.endWrite();
		
	}
	
}

static bool startShakingAndScreenFlashing(){
	requestProgressiveWobble(40, 50, 100, 1800);
	requestScreenFlashing(50, 17, 150);
	return true;
}

static bool waitTillExplosionsFinish(){
	for(const Option<ExplosionParams> requestedExplosion : requestedExplosions){
		if(requestedExplosion.hasValue()){
			return false;
		}
	}
	
	return true;
}


static ScriptedAction  scripted500KgBomb_Eagle[]{
	ScriptedDelay(2000),
	ScriptedDelay(2500),
	ScriptedAction(&lowPriorityAnimations[0], true),
	&lowPriorityAnimations[1],
	&lowPriorityAnimations[2],
	startShakingAndScreenFlashing,
	ScriptedAction(
		[]() -> bool {
			
			requestExplosion({121, 206}, 50, 0);
	
			return true;
		}
	),
	ScriptedDelay(1670),
	ScriptedAction(
		[]() -> bool {

			requestExplosion({150, 220}, 20, 1);
			requestExplosion({90, 220}, 20, 2);
			
			return true;
		}
	),
	waitTillExplosionsFinish,
	ScriptedAction::None()
};

static ScriptedAction  scriptedOrbital120MM_HEBarrage[]{
	ScriptedDelay(2000),
	ScriptedDelay(2500),
	ScriptedAction(&lowPriorityAnimations[0], true),
	&lowPriorityAnimations[1],
	&lowPriorityAnimations[2],
	startShakingAndScreenFlashing,
	ScriptedAction(
		[]() -> bool {
			
			requestExplosion({121, 206}, 50, 0);
	
			return true;
		}
	),
	ScriptedDelay(1670),
	ScriptedAction(
		[]() -> bool {

			requestExplosion({150, 220}, 20, 1);
			requestExplosion({90, 220}, 20, 2);
			
			return true;
		}
	),
	waitTillExplosionsFinish,
	ScriptedAction::None()
};


static ScriptedAction* currentScriptedAction = nullptr;



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

static void drawWindowBitPixel(const gui::Window& window, Option<gui::Color565> maybeOutline = None, Option<gui::Position> clearPrevious = None){
	if(const gui::Position* p_clearPosition = clearPrevious.ptr_value()){
		gui::drawWindowBitPixel(tft, window, maybeOutline, Some(gui::ClearSettings{.position = *p_clearPosition, .clearFn = clearWithGrid}));
	}
	else {
		gui::drawWindowBitPixel(tft, window, maybeOutline);
	}
}

static void drawWindowBitPixelWithDarkGrid(const gui::Window& window, Option<gui::Color565> maybeOutline = None, Option<gui::Position> clearPrevious = None){
	if(const gui::Position* p_clearPosition = clearPrevious.ptr_value()){
		gui::drawWindowBitPixel(tft, window, maybeOutline, Some(gui::ClearSettings{.position = *p_clearPosition, .clearFn = clearWithDarkGrid}));
	}
	else {
		gui::drawWindowBitPixel(tft, window, maybeOutline);
	}
}

static void drawSelectionBackgroundGrid(){
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
		TEXT_FRAME_SIZE.width-6,TEXT_FRAME_SIZE.height-8,
		ILI9341_DARKGREY
	);
	clearWithDarkGrid(
		{TEXT_FRAME_POSITION.x+4, TEXT_FRAME_POSITION.y+4},
		{TEXT_FRAME_SIZE.width-10,TEXT_FRAME_SIZE.height-10}
	);
}



namespace module{ // display

void DisplayRGBModule::setTargetFPS(uint8_t fps){
	mi_targetFpsDeltaMs = 1000/uint16_t(fps);
}

void DisplayRGBModule::showArrow(uint8_t slot, Option<Arrow> arrow) {
	if(slot < ARROW_MAX_SLOTS) {
		gui::Window& arrowWindow = arrowArrayWindowSlots[MAIN_ARROWS_IDX][slot];
		if(const Arrow* p_arrow = arrow.ptr_value()){			
			arrowWindow.setHidden(false);
			ArrowToImageMapping loadedEntry;
			for(const ArrowToImageMapping& entry : bigArrowMapping){
				PROGMEM_READ_STRUCTURE(&loadedEntry, &entry);
				if(loadedEntry.arrow == *p_arrow){
					arrowWindow.setImageBuffer(loadedEntry.image);
					arrowWindow.setFlipSetting(loadedEntry.flip);
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
				= arrowArrayWindowSlots[MAIN_ARROWS_IDX][*p_slot].getPosition();
			arrowSlotWindowPosition.x += SELECTOR_ARROW_OFFSET;
			/*gui::Size arrowSlotWindowSize
				= arrowArrayWindowSlots[MAIN_ARROWS_IDX][*p_slot].getImageBuffer();*/
			arrowSlotWindowPosition.y = int16_t(arrowSlotWindowPosition.y + BIG_ARROW_HEIGHT + 5);
			slotLowerSelection.setPosition(arrowSlotWindowPosition);
			slotLowerSelection.setHidden(false);

			arrowSlotWindowPosition
				= arrowArrayWindowSlots[MAIN_ARROWS_IDX][*p_slot].getPosition();	
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

	gui::Window (&suggestionArrowsEntry)[ARROW_MAX_SLOTS] = arrowArrayWindowSlots[suggestion];
	for(uint8_t suggestionSlotIdx = 0; suggestionSlotIdx < ARROW_MAX_SLOTS; suggestionSlotIdx++){
		bool showArrow = suggestionSlotIdx < arrowCombinationLength;
		gui::Window& suggestionArrow = suggestionArrowsEntry[suggestionSlotIdx];
		//suggestionArrowsEntry[arrowIdx].
		if(showArrow){
			Arrow arrow = arrowCombination[suggestionSlotIdx];
			ArrowToImageMapping loadedEntry;
			for(const ArrowToImageMapping& entry : tinyArrowMapping){
				PROGMEM_READ_STRUCTURE(&loadedEntry, &entry);
				if(loadedEntry.arrow == arrow){
					suggestionArrow.setImageBuffer(loadedEntry.image);
					suggestionArrow.setFlipSetting(loadedEntry.flip);
				}
			}
		}
		
		suggestionArrowsEntry[suggestionSlotIdx].setHidden(!showArrow);
		
	}	

	mb_textChanged = true;
}


void DisplayRGBModule::showOutcome(Option<Stratagem> maybeStratagem, bool show = true){
	const char* outcomeText;
	ColorPalette slotArrowColor = ColorPalette::HELL_MAIN_COLOR;
	if(!show){
		outcomeText = EMPTY_PROGMEM_STRING;
	}
	else if(maybeStratagem.hasValue()){
		outcomeText = PSTR("SUCCESSFUL");
		slotArrowColor = ColorPalette::HELL_MAIN_COLOR;
		//mb_wasSuccessful = true;
	}
	else{
		//mb_wasSuccessful = false;
		slotArrowColor = ColorPalette::INVALID_COMBINATION_COLOR;
		outcomeText = PSTR("FAILED");
	}

	maybeSuccessfulStratagemCallin = maybeStratagem;

	for(gui::Window& arrowWindowSlot : arrowArrayWindowSlots[MAIN_ARROWS_IDX]){
		arrowWindowSlot.setColorPaletteIndex(uint8_t(slotArrowColor));
		arrowWindowSlot.forceUpdate();
	}
	
	mb_outcomeChanged = outcomeText != ms_outcomeText;
	ms_outcomeText = outcomeText;
	
	
}


void DisplayRGBModule::reset() {
	for(gui::Window& arrowWindow : arrowArrayWindowSlots[MAIN_ARROWS_IDX]){
		arrowWindow.setHidden(true);
		arrowWindow.setColorPaletteIndex(uint8_t(ColorPalette::HELL_MAIN_COLOR));
	}

	showSlotSelection(None);
	selectedUpperSlotPreviousPosition = slotUpperSelection.getPosition();
	selectedLowerSlotPreviousPosition = slotLowerSelection.getPosition();

	//m_selectedSlot = None<uint8_t>();

	showStratagemSuggestion(None, PRIMARY);
	showStratagemSuggestion(None, SECONDARY);
	
	showOutcome(None, false);

	wobble(1700, 5);

	//mb_wasSuccessful = false;
	maybeSuccessfulStratagemCallin = None;

	update();
}

void DisplayRGBModule::update(){
	mb_redraw = true;
}

void DisplayRGBModule::wobble(uint16_t timeToWobble, uint8_t amountOfWobble){
	setupWobble(timeToWobble, amountOfWobble);
}


uint8_t DisplayRGBModule::getTargetFPS() const {
	return 1000/mi_targetFpsDeltaMs;
}


DisplayRGBModule::InitializationState DisplayRGBModule::init(){
	delayScriptedAnimationTimer.disable();
	//pinMode(Pinout::Assignment::TFT_CS, OUTPUT);
	//digitalWrite(Pinout::Assignment::TFT_CS	, HIGH); 
	tft.begin();
	//uint8_t x = tft.readcommand8(ILI9341_RDMODE);
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
	//tft.invertDisplay(1);
	gui::Window::SetColorPaletteBuffer(colorPaletteBuf);
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

	return InitializationState::Initialized;
}


void DisplayRGBModule::run(){
	uint32_t delta = millis() - frameStartTime;
	if(delta >= mi_targetFpsDeltaMs){
		frameStartTime = millis();
		drawDynamicContent();

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
	gui::Window logoWindow{10, 30, DPS_LogoSmall, uint8_t(ColorPalette::HELL_MAIN_COLOR), false, gui::Flip::VERTICALLY};

	drawWindowBitPixel(logoWindow, Some(OUTLINE_COLOR));
	gui::drawHorizontalSeparatorWithBorders(tft, 1, logoWindow.getPosition().y + 35, screenWidth, 4);

	logoWindow.setPosition({10, 262});
	logoWindow.setFlipSetting(gui::Flip::NONE);
	drawWindowBitPixel(logoWindow, Some(OUTLINE_COLOR));
	gui::drawHorizontalSeparatorWithBorders(tft, 1, logoWindow.getPosition().y - 10, screenWidth, 4);


	drawSelectionBackgroundGrid();
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


void DisplayRGBModule::drawDynamicContent() {
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
	//if(currentScriptedAction != &scriptedAnimations[CONST_LENGTH(scriptedAnimations)]){
	
	if(currentScriptedAction != nullptr){

		
		if(currentScriptedAction->actionType == ScriptedAction::ActionType::NONE){
			clearWithGrid({.x = 70, .y = 147}, {.width = 100, .height = 6});
			drawSelectionBackgroundGrid();
			currentScriptedAction = nullptr;
		}
		else if(currentScriptedAction->run()){
			currentScriptedAction++;
						
		}
		
		
		
	}
	
	

	//if(mi_wobbleAmountY != 0){
	uint16_t timeNow = millis();

	
	uint16_t elapsedTime = timeNow - wobbleParams.mi_wobbleStartTime;
	int16_t newPosition = gui::lerp(wobbleParams.mi_wobbleStart, wobbleParams.mi_wobbleStop, wobbleParams.mi_wobbleTargetTime, elapsedTime);
	

	//Serial.println(newPosition);
	tft.scrollTo(newPosition);
	//delay(1000);
	if(elapsedTime > wobbleParams.mi_wobbleTargetTime){
		if(ProgressiveWobbleParams* p_progressiveWobble = requestedProgressiveWobble.ptr_value()){
			uint16_t elapsedTime = timeNow - p_progressiveWobble->startTime;
			
			//wobble(
			wobbleParams.mi_wobbleTargetTime = gui::lerp(
				p_progressiveWobble->startWobbleTimeAmount,
				p_progressiveWobble->endWobbleTimeAmount,
				p_progressiveWobble->targetTime,
				elapsedTime
			);


			//	mi_wobbleStop
				
			//);
			if(elapsedTime > p_progressiveWobble->targetTime){
				wobble(p_progressiveWobble->previousWobbleTime, p_progressiveWobble->previousWobbleAmount);
				//mi_wobbleStop = 0;
				requestedProgressiveWobble = None;
			}
		}
		int16_t tmp = wobbleParams.mi_wobbleStop;
		wobbleParams.mi_wobbleStop = wobbleParams.mi_wobbleStart;
		wobbleParams.mi_wobbleStart = tmp;
		wobbleParams.mi_wobbleStartTime = millis();
		
	}
	//}
	
	
	if(SlowVerticalClearParams* p_verticalClearParams = requestedSlowClear.ptr_value()){
		gui::Position& position = p_verticalClearParams->position;
		const gui::Size& size = p_verticalClearParams->size;

		clearWithGrid(position, {.width = size.width, .height = 1});
		if(size.height < (++position.y)){
			requestedSlowClear = None;
		}
	}

	if(FlashParams* p_flashParams = requestedScreenFlashing.ptr_value()){
		if(screenFlashTimer.isDown()){
			uint8_t invert;
			if(p_flashParams->moduloCounter == 0){
				requestedScreenFlashing = None;
				invert = 0;
			}
			else {
				invert = p_flashParams->moduloCounter % 2;
				p_flashParams->moduloCounter--;
				screenFlashTimer.reset(p_flashParams->switchTime);
			}
			tft.invertDisplay(invert);
		}
	}
	for(Option<ExplosionParams>& requestedExplosion : requestedExplosions){
		if(ExplosionParams* p_explosionParams = requestedExplosion.ptr_value()){
			//gui::drawOptimizedCircle(tft, p_explosionParams->position, p_explosionParams->currentRadius, ILI9341_WHITE);//drawOptimizedExplosion(p_explosionParams->position, p_explosionParams->currentRadius, p_explosionParams->targetRadius);
			drawOptimizedExplosion(*p_explosionParams);
			if(p_explosionParams->currentRadius == p_explosionParams->targetRadius){
				requestedExplosion = None;
			}
			else {
				p_explosionParams->currentRadius++;
			}
		}
	}


	if(mb_redraw){

		/*for(gui::Window& arrowWindow : arrowArrayWindowSlots[MAIN_ARROWS_IDX]){
			drawWindowBitPixelWithDarkGrid(arrowWindow, slotArrowColor, Some(OUTLINE_COLOR), Some(arrowWindow.getPosition()));
			arrowWindow.updated();
		}*/

		if(mb_textChanged) {
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

			mb_textChanged = false;
		}
		if(currentScriptedAction == nullptr){
			for(auto& suggestionArrowsEntry : arrowArrayWindowSlots){
				Option<gui::Color565> maybeOutline;
				if(&suggestionArrowsEntry == &arrowArrayWindowSlots[MAIN_ARROWS_IDX]){
					maybeOutline = Some(OUTLINE_COLOR);
				}
				for(gui::Window& suggestionArrow : suggestionArrowsEntry){
					
					drawWindowBitPixelWithDarkGrid(suggestionArrow, maybeOutline, suggestionArrow.getPosition());
					suggestionArrow.updated();
				}
			}
			drawWindowBitPixelWithDarkGrid(slotUpperSelection, Some(SELECTOR_OUTLINE_COLOR), Some(selectedUpperSlotPreviousPosition));
			slotUpperSelection.updated();

			drawWindowBitPixelWithDarkGrid(slotLowerSelection, Some(SELECTOR_OUTLINE_COLOR), Some(selectedLowerSlotPreviousPosition));
			slotLowerSelection.updated();
		}

		mb_redraw = false;
	}
	else if(mb_outcomeChanged){
		bool b_wasSuccessful = maybeSuccessfulStratagemCallin.hasValue();
		int16_t outcomeTextX = b_wasSuccessful ? 90 : 95;
		clearWithGrid({ int16_t(outcomeTextX -10), 73}, {70, 8});
		tft.setCursor(outcomeTextX, 73);
		tft.setTextColor(b_wasSuccessful ? ILI9341_GREEN : ILI9341_RED);
		tft.println((const __FlashStringHelper*)ms_outcomeText);

		

		if(ms_outcomeText != EMPTY_PROGMEM_STRING && b_wasSuccessful){
			requestSlowClear({.x = 0, .y = 85}, {.width = 220, .height = 147});
			

			switch(*maybeSuccessfulStratagemCallin.ptr_value()){
				case Stratagem::Orbital120MM_HEBarrage:
					//currentScriptedAction = scriptedOrbital120MM_HEBarrage;
					break;
				default:
					currentScriptedAction = scripted500KgBomb_Eagle;
					break;
			}
			

			//delay(100);
			
			//lowPriorityAnimations[0].restart();
			//lowPriorityAnimations[0].window.setHidden(false);
			//disableStars(true);
		}

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
		

		
		

		gui::AnimatedMovement* p_animation;
		gui::Position oldPosition;
	

		do {
			if(lowPriorityAnimationsIndex >= CONST_LENGTH(lowPriorityAnimations)){
				lowPriorityAnimationsIndex = 0;
			}
			p_animation = &lowPriorityAnimations[lowPriorityAnimationsIndex];
			oldPosition = p_animation->animateMovement();
			lowPriorityAnimationsIndex++;
		}while (!p_animation->window.needsUpdate());
		
		//ColorAndOutline matchedColor// = matchWindowWithColor(&p_animation->window);
		

		drawWindowBitPixel(p_animation->window, Some(OUTLINE_COLOR), Some(oldPosition)/*p_animation->clearBeforeDraw() ? Some(oldPosition) : None<gui::Position>()*/);
		if(p_animation->isMirroredY()){
			int16_t halfDisplayWidth = tft.width();

			gui::Position positionMirrored = p_animation->window.getPosition();
			gui::Position oldPositionMirrored = oldPosition;
			positionMirrored.y = 70 - (positionMirrored.y - halfDisplayWidth);
			oldPositionMirrored.y = 70 - (oldPositionMirrored.y - halfDisplayWidth);	
			drawWindowBitPixel(gui::Window(positionMirrored.x, positionMirrored.y, p_animation->window.getImageBuffer(), p_animation->window.getColorPaletteIndex(), false, gui::Flip::VERTICALLY), Some(OUTLINE_COLOR), Some(oldPositionMirrored));				
		}
		p_animation->window.updated();

		

		
	}

	
}

} // rgb display module











