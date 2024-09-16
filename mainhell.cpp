#include "helldef.h"
#include "shared/module.h"

#include "timer.h"
#include "keyboard.h"
#include "displayrgb.h"
#include "buzzer.h"
#include "arrowslots.h"

struct KeyMapping{
    const char key;
    const Arrow arrow;
} static const keyMapping[ARROW_POSITIONS]{
    {'0', Arrow::UP},
    {'5', Arrow::DOWN},
    {'9', Arrow::LEFT},
    {'7', Arrow::RIGHT},
};

static Option<Arrow> matchKeyToArrow(char key){
    for(KeyMapping map : keyMapping){
        if(map.key == key){
            return Some(map.arrow);
        }
    }
    return None<Arrow>();
}


void timedUnlockInput(TimedExecution10ms&);
void timedLibertyStartup(TimedExecution10ms&);
void timedWobbleStop(TimedExecution10ms&);

struct MainModule : public module::ManagedModule{

    MainModule(const char* moduleInstanceName, bool autoenable = true)
    : ManagedModule(moduleInstanceName, autoenable) {}

    void handleArrowInput(Arrow arrow){
        //Serial.print("Matched arrow: ");
		

        Option<uint8_t> nextSlotIndex = m_arrowSlots.nextSlot(arrow);

        if(uint8_t* p_nextIndex = nextSlotIndex.ptr_value()){
            if(!m_blockInputTimed.isEnabled()){
               	module::Display.showArrow(*p_nextIndex, Some(arrow));
             	module::Display.showSlotSelection(Some<uint8_t>(*p_nextIndex));
				/*
                if(Stratagem* p_stratagem = maybePartialStratagem.ptr_value()){
                    module::Display.showText(ArrowSlots::GetStratagemName(*p_stratagem));
                }
                else {
                    module::Display.showText(nullptr);
                }*/
                Option<Stratagem> maybeStratagem = m_arrowSlots.tryMatchStratagemFromSlots();
                if(Stratagem* p_stratagem = maybeStratagem.ptr_value()){
                    #ifdef DEBUG
					Serial.print("Activated combination for: ");
					Serial.println(ArrowSlots::GetStratagemName(*p_stratagem));
					#endif
					module::Display.showSlotSelection(None<uint8_t>());
					module::Display.showStratagemSuggestion(None<Stratagem>(), module::DisplayRGBModule::StratagemSuggestion::SECONDARY);

					module::Display.showStratagemSuggestion(maybeStratagem, module::DisplayRGBModule::StratagemSuggestion::PRIMARY);
					module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::SUCCESS);
					module::Display.showOutcome(maybeStratagem, true);
                    
					m_blockInputTimed.setup(timedUnlockInput, 2000);
                    m_libertyDelayTimed.setup(timedLibertyStartup, 1000);
					/*Serial.print("SUCCESS: activated stratagem");
					if(const char* s_stratagemName = ArrowSlots::GetStratagemName(*p_stratagem)){
						Serial.print(" - ");
						Serial.println(s_stratagemName);
					}*/
					
                }
                else if(m_arrowSlots.getSlotsUsedCount() == ARROW_MAX_SLOTS){
                    module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::FAIL);
					module::Display.showOutcome(None<Stratagem>(), true);
                    m_blockInputTimed.setup(timedUnlockInput,2000);
                }
                else {
					uint8_t sizeUsedSlots = (*p_nextIndex+1);
					Option<Stratagem> maybePartialStratagem = m_arrowSlots.tryMatchStratagemFromSlots(Some(sizeUsedSlots));
					module::Display.showStratagemSuggestion(maybePartialStratagem, module::DisplayRGBModule::StratagemSuggestion::PRIMARY);
					
					maybePartialStratagem = m_arrowSlots.tryMatchStratagemFromSlots(Some(sizeUsedSlots), maybePartialStratagem);
					module::Display.showStratagemSuggestion(maybePartialStratagem, module::DisplayRGBModule::StratagemSuggestion::SECONDARY);
                    module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::BUTTON_PRESS);
                }
            }
        }
        // out of slots
        else if(!m_blockInputTimed.isEnabled()){
            module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::FAIL);
			module::Display.showOutcome(None<Stratagem>(), true);
            m_blockInputTimed.setup(timedUnlockInput,2000);
        }
        else{
			
            module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::FAIL);
        }


		if(m_blockInputTimed.isEnabled()){
			module::Display.wobble(200, 10);
			m_wobbleStopTimed.setup(timedWobbleStop, 500);
		}
		#ifdef DEBUG
        switch (arrow)
        {
            case Arrow::UP:
                Serial.println("Pressed Arrow::UP");
                break;
            
            case Arrow::DOWN:
                Serial.println("Pressed Arrow::DOWN");
                break;
            
            case Arrow::LEFT:
                Serial.println("Pressed Arrow::LEFT");
                break;

            case Arrow::RIGHT:
                Serial.println("Pressed Arrow::RIGHT");
                break;
        }
		#endif
    }

    InitializationState init() override;

    void run() override{
            
    }

    ArrowSlots m_arrowSlots;

    TimedExecution10ms m_blockInputTimed;
    TimedExecution10ms m_libertyDelayTimed;
	TimedExecution10ms m_wobbleStopTimed;

    
    //Option<Arrow> slots[ARROW_MAX_SLOTS];
};

static MainModule Main("MainModule");

 void processKeyEvents(char key) {          
	KeyState event = module::Keyboard.keypad.getState();
    switch (event){
		case PRESSED:
			Option<Arrow> maybeArrow = matchKeyToArrow(key); 
			if(Arrow* p_arrow = maybeArrow.ptr_value()){
				Main.handleArrowInput(*p_arrow);
			}
			if(key == '8'){
				module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::BUTTON_PRESS);
			}
			#ifdef DEBUG
			Serial.print(key);
			Serial.print(" ");
			#endif
		break;
	}            
} 


MainModule::InitializationState MainModule::init() {
	module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::VOLUME_TEST);

	module::Keyboard.keypad.addEventListener(processKeyEvents);
    
    /*Arrow callin[ARROW_MAX_SLOTS];

    uint8_t callinLenght = ArrowSlots::GetStratagemArrows(Stratagem::JumpPack, callin);
    for(uint8_t arrowIdx = 0; arrowIdx < callinLenght; arrowIdx++){
        module::Display.showArrow(arrowIdx, Some(callin[arrowIdx]));
    }*/

    /*module::Display.showArrow(0, Some(Arrow::UP));
	module::Display.showArrow(1, Some(Arrow::DOWN));
	module::Display.showArrow(2, Some(Arrow::LEFT));
	module::Display.showArrow(3, Some(Arrow::RIGHT));
	module::Display.showArrow(4, Some(Arrow::DOWN));
	module::Display.showArrow(5, Some(Arrow::UP));*/
    
    return InitializationState::Initialized;
}


void timedUnlockInput(TimedExecution10ms&){
    module::Display.reset();
    Main.m_arrowSlots.reset();
}

void timedLibertyStartup(TimedExecution10ms&){
    module::Buzzer.playPreset(module::BuzzerSoundsModule::LIBER_TEA);
}

void timedWobbleStop(TimedExecution10ms&){
	module::Display.wobble(1800, 5);
}