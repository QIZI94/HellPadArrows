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

private:
	static constexpr DisplayRotation DEFAULT_ROTATION = DisplayRotation::South;

public: 
    DisplayRGBModule(const char* moduleInstanceName, bool autoenable = true)
    : ManagedModule(moduleInstanceName, autoenable) {}

	void showText(const char* str_c);
	void showArrow(uint8_t slot, Option<Arrow> arrow);
	void showSlotSelection(Option<uint8_t> slot);

	void reset();

	void update();

	Option<Arrow> getArrowFromSlot(uint8_t slot) const;
	Option<uint8_t> getSelection() const;

private:
    InitializationState init() override;
    void run() override;

private:
	Option<Arrow> m_slots[ARROW_MAX_SLOTS];

	int8_t m_slotOffsetsX[ARROW_MAX_SLOTS]{
		0, 22, 44, 66, 88, 110
	};
	uint16_t m_selectionOffsetX[ARROW_MAX_SLOTS]{
		0, 18, 40, 62, 84,106
	};
	Option<uint8_t> m_selectedSlot;

	bool mb_redraw = true;
};

inline DisplayRGBModule Display("DisplayRGBModule");

} // display

#endif