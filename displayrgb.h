#ifndef DISPLAY_BIG_RGB
#define DISPLAY_BIG_RGB

#include "helldef.h"
#include "shared/module.h"
#include "shared/optional.h"

namespace module{ // display
class DisplayRGBModule : public ManagedModule{
private:
	enum class DisplayRotation : uint8_t {
		North = 0,
		East = 1,
		South = 2,
		West = 3
	};
public:
	enum StratagemSuggestion{
		PRIMARY = 0,
		SECONDARY = 1
	};

private:
	static constexpr DisplayRotation DEFAULT_ROTATION = DisplayRotation::South;
	static constexpr int16_t TARGET_FPS = 60;
	static constexpr int16_t TARGET_FPS_DELTA_MS = 1000/TARGET_FPS;

public: 
    DisplayRGBModule(const char* moduleInstanceName, bool autoenable = true)
    : ManagedModule(moduleInstanceName, autoenable) {}

	void showArrow(uint8_t slot, Option<Arrow> arrow);
	void showSlotSelection(Option<uint8_t> slot);

	void showStratagemSuggestion(Option<Stratagem> maybeStratagem, StratagemSuggestion suggestion);

	void showOutcome(Option<Stratagem> maybeStratagem, bool wasSuccessful);

	void setTargetFPS(uint8_t fps);

	void reset();

	void update();

	void wobble(uint16_t timeToWobble, uint16_t amountOfWobble);

	uint8_t getTargetFPS() const;

private:
    InitializationState init() override;
    void run() override;

private:
	void drawStaticContent();
	void drawDynamicContent(uint32_t delta);

private:
	uint16_t mi_targetFpsDeltaMs = TARGET_FPS_DELTA_MS;

	uint16_t mi_wobbleStartTime = 0;
	uint16_t mi_wobbleTargetTime = 1700;
	int16_t mi_wobbleStart = 0;
	int16_t mi_wobbleStop = 5;

	const char* ms_primarySuggestionText = EMPTY_PROGMEM_STRING;
	const char* ms_secondarySuggestionText = EMPTY_PROGMEM_STRING;
	const char* ms_outcomeText = EMPTY_PROGMEM_STRING;

	bool mb_redraw = true;
	bool mb_textChanged = false;
	bool mb_outcomeChanged = false;

	bool mb_wasSuccessful = false;
};

inline DisplayRGBModule Display("DisplayRGBModule");

} // display

#endif