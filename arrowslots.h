#ifndef ARROWSLOTS
#define ARROWSLOTS

#include "helldef.h"
#include "shared/optional.h"

class ArrowSlots{
public: 
	
public:
    Option<uint8_t> nextSlot(Arrow arrow);
    Option<Stratagem> tryMatchStratagemFromSlots();

    void reset();
public:

    static const char* GetStratagemName(Stratagem stratagem);

private:
    Option<Arrow> m_slots[ARROW_MAX_SLOTS];
};


#endif