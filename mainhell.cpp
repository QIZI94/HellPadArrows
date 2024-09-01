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
    {'6', Arrow::UP},
    {'#', Arrow::DOWN},
    {'8', Arrow::LEFT},
    {'C', Arrow::RIGHT},
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


 void processKey(byte row, byte col) {          
   
     
	// get the key label from the array and send it to the serial terminal       
	Option<char> maybeKeyVal = module::Keyboard.rowColToChar(row,col);

    if(char* p_keyVal = maybeKeyVal.ptr_value()){
        Option<Arrow> maybeArrow = matchKeyToArrow(*p_keyVal); 
        if(Arrow* p_arrow = maybeArrow.ptr_value()){
			Main.handleArrowInput(*p_arrow);
        }
        Serial.print(*p_keyVal);
	    Serial.print(" ");
    }

	

       
} 


MainModule::InitializationState MainModule::init() {
    module::Keyboard.setPressHandler(processKey);
    
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