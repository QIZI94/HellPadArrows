#include "arrowslots.h"

static const Arrow Stratagem_Bomb500kg[] = {
    Arrow::UP, Arrow::LEFT, Arrow::RIGHT, Arrow::RIGHT, Arrow::RIGHT
};

static const Arrow Stratagem_EagleSmokeStrike[] = {
    Arrow::UP, Arrow::DOWN, Arrow::UP, Arrow::LEFT
};

static const Arrow Stratagem_EagleAirstrike[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::DOWN, Arrow::RIGHT
};

static const Arrow Stratagem_EagleStrafingRun[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::RIGHT
};

static const Arrow Stratagem_EagleClusterBomb[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::DOWN, Arrow::DOWN, Arrow::RIGHT, Arrow::DOWN
};

static const Arrow Stratagem_JumpPack[] = {
    Arrow::UP, Arrow::DOWN, Arrow::UP, Arrow::LEFT
};

static const Arrow Stratagem_Eagle110MMRocketPods[] = {
    Arrow::DOWN, Arrow::DOWN, Arrow::RIGHT, Arrow::RIGHT, Arrow::RIGHT
};

struct StratagemMapping {
    const Stratagem stratagemId;
    const Arrow* stratagemCallin;
    const uint8_t stratagemCallinLength;
    const char* displayName;
};

static const StratagemMapping Stratagems[] = {
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

Option<Stratagem> ArrowSlots::tryMatchStratagemFromSlots(){
    
    for(StratagemMapping stratagem : Stratagems){
        uint8_t countMatched = 0;
        uint8_t stratagemCallinLength = stratagem.stratagemCallinLength;
        for(uint8_t strataIdx = 0; strataIdx < stratagemCallinLength; strataIdx++ ){
            const Arrow stratagemArrow = stratagem.stratagemCallin[strataIdx];
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

const char* ArrowSlots::GetStratagemName(Stratagem stratagem){
	for(StratagemMapping stratagemEntry : Stratagems){
		if(stratagemEntry.stratagemId == stratagem){
			return stratagemEntry.displayName;
		}
	}

	return nullptr;
}
