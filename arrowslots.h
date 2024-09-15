#ifndef ARROWSLOTS
#define ARROWSLOTS

#include "helldef.h"
#include "shared/optional.h"

class ArrowSlots{
public: 
	
public:
    Option<uint8_t> nextSlot(Arrow arrow);
    Option<Stratagem> tryMatchStratagemFromSlots(Option<uint8_t> maybeOverrideMatchLenght = None<uint8_t>()) const;

    void reset();

    uint8_t getSlotsUsedCount() const;
public:
    static uint8_t GetStratagemArrows(Stratagem stratagem, Arrow* arrowsOut);
    static const char* GetStratagemName(Stratagem stratagem);

private:
    Option<Arrow> m_slots[ARROW_MAX_SLOTS];
};


#endif