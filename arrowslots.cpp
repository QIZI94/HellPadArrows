#include "arrowslots.h"

static const PROGMEM Arrow Stratagem_Bomb500kg[] = {
    Arrow::UP, Arrow::LEFT, Arrow::RIGHT, Arrow::RIGHT, Arrow::RIGHT
};

static const PROGMEM Arrow Stratagem_EagleSmokeStrike[] = {
    Arrow::UP, Arrow::DOWN, Arrow::UP, Arrow::RIGHT
};

static const PROGMEM Arrow Stratagem_EagleAirstrike[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::DOWN, Arrow::RIGHT
};

static const PROGMEM Arrow Stratagem_EagleStrafingRun[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::RIGHT
};

static const PROGMEM Arrow Stratagem_EagleClusterBomb[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::DOWN, Arrow::DOWN, Arrow::RIGHT, Arrow::DOWN
};

static const PROGMEM Arrow Stratagem_JumpPack[] = {
    Arrow::UP, Arrow::DOWN, Arrow::UP, Arrow::LEFT
};

static const PROGMEM Arrow Stratagem_Eagle110MMRocketPods[] = {
    Arrow::DOWN, Arrow::DOWN, Arrow::RIGHT, Arrow::RIGHT, Arrow::RIGHT
};

struct StratagemMapping {
    const Stratagem stratagemId;
    const Arrow* stratagemCallin;
    const uint8_t stratagemCallinLength;
    const char* displayName;
};
constexpr static const StratagemMapping EMPTY_MAPPING{Stratagem::NUM_OF_STRATAGEMS, nullptr, 0, nullptr};

static const PROGMEM StratagemMapping Stratagems[] = {
    {Stratagem::Bomb500kg, Stratagem_Bomb500kg, CONST_LENGTH(Stratagem_Bomb500kg), "500kg Bomb"},
    {Stratagem::EagleSmokeStrike, Stratagem_EagleSmokeStrike, CONST_LENGTH(Stratagem_EagleSmokeStrike), "Eagle Smoke Strike"},
    {Stratagem::EagleAirstrike, Stratagem_EagleAirstrike, CONST_LENGTH(Stratagem_EagleAirstrike), "Eagle Airstrike"},
    {Stratagem::EagleStrafingRun, Stratagem_EagleStrafingRun, CONST_LENGTH(Stratagem_EagleStrafingRun), "Eagle Strafing Run"},
    {Stratagem::EagleClusterBomb, Stratagem_EagleClusterBomb, CONST_LENGTH(Stratagem_EagleClusterBomb), "Eagle Cluster Bomb"},
    {Stratagem::JumpPack, Stratagem_JumpPack, CONST_LENGTH(Stratagem_JumpPack), "Jump Pack"},
    {Stratagem::Eagle110MMRocketPod, Stratagem_Eagle110MMRocketPods, CONST_LENGTH(Stratagem_Eagle110MMRocketPods), "Eagle 110mm Rocket Pods"},

};

Option<uint8_t> ArrowSlots::nextSlot(Arrow arrow){
    uint8_t index = 0;
    for(Option<Arrow>& slot : m_slots){
        if(!slot.hasValue()){
            slot = Some(arrow);
            return Some(index);
        }

        index++;
    }

    return None<uint8_t>();
}

Option<Stratagem> ArrowSlots::tryMatchStratagemFromSlots(Option<uint8_t> maybeOverrideMatchLenght) const {
    
    for(const StratagemMapping& stratagemProgmem : Stratagems){

		StratagemMapping stratagem = EMPTY_MAPPING;
        PROGMEM_READ_STRUCTURE(&stratagem, &stratagemProgmem);
		
		uint8_t countMatched = 0;
        uint8_t stratagemCallinLength = maybeOverrideMatchLenght.hasValue() ?  *maybeOverrideMatchLenght.ptr_value() : stratagem.stratagemCallinLength;
        
        if(stratagemCallinLength > stratagem.stratagemCallinLength){
            continue;
        }
        for(uint8_t strataIdx = 0; strataIdx < stratagemCallinLength; strataIdx++ ){
            Arrow stratagemArrow = Arrow::DOWN;
			PROGMEM_READ_STRUCTURE(&stratagemArrow, &stratagem.stratagemCallin[strataIdx]);
            Option<Arrow> maybeArrow = m_slots[strataIdx];

            if(const Arrow* p_arrow = maybeArrow.ptr_value()){
                if(*p_arrow == stratagemArrow){
                    countMatched++;
                }
                else{
                    break;
                }
            }

        }

        if(countMatched == stratagemCallinLength){
            return Some(stratagem.stratagemId);
        }  
    }
    return None<Stratagem>();
}

void ArrowSlots::reset(){
    for(Option<Arrow>& maybeArrow : m_slots){
        maybeArrow = None<Arrow>();
    }
}

uint8_t ArrowSlots::getSlotsUsedCount() const{
    uint8_t slotsUsedCounter = 0;
    for(const auto& slot : m_slots){
        if(slot.hasValue()){
            slotsUsedCounter++;
        }
    }
    return slotsUsedCounter;
}

uint8_t ArrowSlots::GetStratagemArrows(Stratagem stratagem, Arrow* arrowsOut){
    for(const StratagemMapping& stratagemProgmemEntry : Stratagems){
		StratagemMapping stratagemEntry = EMPTY_MAPPING;
        PROGMEM_READ_STRUCTURE(&stratagemEntry, &stratagemProgmemEntry);
		if(stratagemEntry.stratagemId == stratagem){
            uint8_t stratagemCallinLength = stratagemEntry.stratagemCallinLength;
            memcpy_P(
                arrowsOut,
                stratagemEntry.stratagemCallin,
                stratagemCallinLength * sizeof(Arrow)
            );
			return stratagemCallinLength;
		}
	}
    return 0;
}


const char* ArrowSlots::GetStratagemName(Stratagem stratagem){
	for(const StratagemMapping& stratagemProgmemEntry : Stratagems){
		StratagemMapping stratagemEntry = EMPTY_MAPPING;
        PROGMEM_READ_STRUCTURE(&stratagemEntry, &stratagemProgmemEntry);
		if(stratagemEntry.stratagemId == stratagem){
			return stratagemEntry.displayName;
		}
	}

	return nullptr;
}
