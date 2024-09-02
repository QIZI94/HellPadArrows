#include "helldef.h"
#include "shared/module.h"

#include "timer.h"
#include "keyboard.h"
#include "displaysmall.h"
#include "buzzer.h"
#include "arrowslots.h"

struct KeyMapping{
    const char key;
    const Arrow arrow;
} static const keyMapping[ARROW_POSITIONS]{
    {'7', Arrow::UP},
    {'9', Arrow::DOWN},
    {'5', Arrow::LEFT},
    {'0', Arrow::RIGHT},
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

struct MainModule : public module::ManagedModule{

    MainModule(const char* moduleInstanceName, bool autoenable = true)
    : ManagedModule(moduleInstanceName, autoenable) {}

    void handleArrowInput(Arrow arrow){
        //module::Display.showArrow(0, Some(arrow));

        Serial.print("Matched arrow: ");
		

        Option<uint8_t> nextSlotIndex = m_arrowSlots.nextSlot(arrow);

        if(uint8_t* p_nextIndex = nextSlotIndex.ptr_value()){
            if(!m_blockInputTimed.isEnabled()){
                module::Display.showArrow(*p_nextIndex, Some(arrow));
            // module::Display.showSlotSelection(*p_nextIndex);
                

                Option<Stratagem> maybeStratagem = m_arrowSlots.tryMatchStratagemFromSlots();
                if(Stratagem* p_stratagem = maybeStratagem.ptr_value()){
                    module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::SUCCESS);
                    m_blockInputTimed.setup(timedUnlockInput, 2000);
                    m_libertyDelayTimed.setup(timedLibertyStartup, 1000);
					/*Serial.print("SUCCESS: activated stratagem");
					if(const char* s_stratagemName = ArrowSlots::GetStratagemName(*p_stratagem)){
						Serial.print(" - ");
						Serial.println(s_stratagemName);
					}*/
					
                }
                else {
                    module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::BUTTON_PRESS);
                }
            }
        }
        // out of slots
        else if(!m_blockInputTimed.isEnabled()){
            module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::FAIL);
            m_blockInputTimed.setup(timedUnlockInput,2000);
        }
        else{
            module::Buzzer.playPreset(module::BuzzerSoundsModule::SoundPreset::FAIL);
        }

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
    }

    InitializationState init() override;

    void run() override{
            
    }

    ArrowSlots m_arrowSlots;

    TimedExecution10ms m_blockInputTimed;
    TimedExecution10ms m_libertyDelayTimed;

    
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
			Serial.print(key);
			Serial.print(" ");
		break;
	}            
} 


MainModule::InitializationState MainModule::init() {
	module::Keyboard.keypad.addEventListener(processKeyEvents);
    
    module::Display.showArrow(0, Some(Arrow::UP));
	module::Display.showArrow(1, Some(Arrow::DOWN));
	module::Display.showArrow(2, Some(Arrow::LEFT));
	module::Display.showArrow(3, Some(Arrow::RIGHT));
	module::Display.showArrow(4, Some(Arrow::DOWN));
	module::Display.showArrow(5, Some(Arrow::UP));
    
    return InitializationState::Initialized;
}


void timedUnlockInput(TimedExecution10ms&){
    module::Display.reset();
    Main.m_arrowSlots.reset();
}

void timedLibertyStartup(TimedExecution10ms&){
    module::Buzzer.playPreset(module::BuzzerSoundsModule::LIBER_TEA);
}