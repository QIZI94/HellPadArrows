#include "arrowslots.h"

// orbital

static const PROGMEM Arrow Stratagem_OrbitalGatlingBarrage[] = {
    Arrow::RIGHT, Arrow::DOWN, Arrow::LEFT, Arrow::UP, Arrow::UP
};

static const PROGMEM Arrow Stratagem_OrbitalAirburstStrike[] = {
    Arrow::RIGHT, Arrow::RIGHT, Arrow::RIGHT
};

static const PROGMEM Arrow Stratagem_Orbital120MM_HEBarrage[] = {
    Arrow::RIGHT, Arrow::DOWN, Arrow::LEFT, Arrow::RIGHT, Arrow::DOWN
};

static const PROGMEM Arrow Stratagem_Orbital380MM_HEBarrage[] = {
   Arrow::RIGHT, Arrow::DOWN, Arrow::UP, Arrow::UP, Arrow::LEFT, Arrow::DOWN, Arrow::DOWN
};

static const PROGMEM Arrow Stratagem_OrbitalWalkingBarrage[] = {
    Arrow::RIGHT, Arrow::RIGHT, Arrow::DOWN, Arrow::LEFT, Arrow::RIGHT, Arrow::DOWN
};

static const PROGMEM Arrow Stratagem_OrbitalLaser[] = {
    Arrow::RIGHT, Arrow::DOWN, Arrow::UP, Arrow::RIGHT, Arrow::DOWN
};

static const PROGMEM Arrow Stratagem_OrbitalRailcannonStrike[] = {
    Arrow::RIGHT, Arrow::UP, Arrow::DOWN, Arrow::DOWN, Arrow::RIGHT
};

// eagle1

static const PROGMEM Arrow Stratagem_Bomb500kg[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::DOWN, Arrow::DOWN, Arrow::DOWN
};

static const PROGMEM Arrow Stratagem_EagleSmokeStrike[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::UP, Arrow::DOWN
};

static const PROGMEM Arrow Stratagem_EagleAirstrike[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::DOWN, Arrow::RIGHT
};

static const PROGMEM Arrow Stratagem_EagleStrafingRun[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::RIGHT
};

static const PROGMEM Arrow Stratagem_EagleClusterBomb[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::DOWN, Arrow::DOWN, Arrow::RIGHT
};

static const PROGMEM Arrow Stratagem_EagleNapalmAirstrike[] = {
   Arrow::UP, Arrow::RIGHT, Arrow::DOWN, Arrow::UP
};


/*static const PROGMEM Arrow Stratagem_JumpPack[] = {
    Arrow::UP, Arrow::DOWN, Arrow::UP, Arrow::LEFT
};*/

static const PROGMEM Arrow Stratagem_Eagle110MMRocketPods[] = {
    Arrow::UP, Arrow::RIGHT, Arrow::UP, Arrow::LEFT
};

struct StratagemMapping {
    const Stratagem stratagemId;
    const Arrow* stratagemCallin;
    const uint8_t stratagemCallinLength;
    const char displayName[26];
};
constexpr static const StratagemMapping EMPTY_MAPPING{Stratagem::NUM_OF_STRATAGEMS, nullptr, 0, ""};

static const PROGMEM StratagemMapping Stratagems[] = {
	// orbital
	{Stratagem::OrbitalGatlingBarrage, Stratagem_OrbitalGatlingBarrage, CONST_LENGTH(Stratagem_OrbitalGatlingBarrage), "Orbital Gatling Barrage"},
    {Stratagem::OrbitalAirburstStrike, Stratagem_OrbitalAirburstStrike, CONST_LENGTH(Stratagem_OrbitalAirburstStrike), "Orbital Airburst Strike"},
    {Stratagem::Orbital120MM_HEBarrage, Stratagem_Orbital120MM_HEBarrage, CONST_LENGTH(Stratagem_Orbital120MM_HEBarrage), "Orbital 120MM HE Barrage"},
    {Stratagem::Orbital380MM_HEBarrage, Stratagem_Orbital380MM_HEBarrage, CONST_LENGTH(Stratagem_Orbital380MM_HEBarrage), "Orbital 380MM HE Barrage"},
    {Stratagem::OrbitalWalkingBarrage, Stratagem_OrbitalWalkingBarrage, CONST_LENGTH(Stratagem_OrbitalWalkingBarrage), "Orbital Walking Barrage"},
	{Stratagem::OrbitalLaser, Stratagem_OrbitalLaser, CONST_LENGTH(Stratagem_OrbitalLaser), "Orbital Laser"},
    {Stratagem::OrbitalRailcannonStrike, Stratagem_OrbitalRailcannonStrike, CONST_LENGTH(Stratagem_Eagle110MMRocketPods), "Orbital Railcannon Strike"},

	// eagle1
    {Stratagem::Bomb500kg, Stratagem_Bomb500kg, CONST_LENGTH(Stratagem_Bomb500kg), "500kg Bomb"},
    {Stratagem::EagleSmokeStrike, Stratagem_EagleSmokeStrike, CONST_LENGTH(Stratagem_EagleSmokeStrike), "Eagle Smoke Strike"},
    {Stratagem::EagleAirstrike, Stratagem_EagleAirstrike, CONST_LENGTH(Stratagem_EagleAirstrike), "Eagle Airstrike"},
    {Stratagem::EagleStrafingRun, Stratagem_EagleStrafingRun, CONST_LENGTH(Stratagem_EagleStrafingRun), "Eagle Strafing Run"},
    {Stratagem::EagleClusterBomb, Stratagem_EagleClusterBomb, CONST_LENGTH(Stratagem_EagleClusterBomb), "Eagle Cluster Bomb"},
	{Stratagem::EagleNapalmAirstrike, Stratagem_EagleNapalmAirstrike, CONST_LENGTH(Stratagem_EagleNapalmAirstrike), "Eagle Napalm Airstrike"},
    //{Stratagem::JumpPack, Stratagem_JumpPack, CONST_LENGTH(Stratagem_JumpPack), "Jump Pack"},
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

Option<Stratagem> ArrowSlots::tryMatchStratagemFromSlots(Option<uint8_t> maybeOverrideMatchLenght, Option<Stratagem> blacklistedStratagem) const {
    
    for(const StratagemMapping& stratagemProgmem : Stratagems){

		StratagemMapping stratagem = EMPTY_MAPPING;
        PROGMEM_READ_STRUCTURE(&stratagem, &stratagemProgmem);

		if(const Stratagem* p_blacklistedStratagem = blacklistedStratagem.ptr_value()){
			if(stratagem.stratagemId == *p_blacklistedStratagem){
				continue;
			}
		}
				
		uint8_t countMatched = 0;
        uint8_t stratagemCallinLength = maybeOverrideMatchLenght.valueOr(stratagem.stratagemCallinLength);// .hasValue() ?  *maybeOverrideMatchLenght.ptr_value() : stratagem.stratagemCallinLength;
        
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
