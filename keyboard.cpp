#include "helldef.h"
#include "keyboard.h"

namespace module{ // keyboard



constexpr uint8_t KB_D2     = 2;
constexpr uint8_t KB_D3     = 3;
constexpr uint8_t KB_D4     = 4;
constexpr uint8_t KB_D5     = 5;
constexpr uint8_t KB_D6     = 6;
constexpr uint8_t KB_D7     = 7;
constexpr uint8_t KB_D8     = 8;
constexpr uint8_t KB_D9     = 9;


static const uint8_t KB_RowPins[] = {
	KB_D9, KB_D8, KB_D7, KB_D6
};
constexpr uint8_t KB_RowCount = CONST_LENGTH(KB_RowPins);

static const uint8_t KB_ColumnPins[] = {
	KB_D5, KB_D4, KB_D3, KB_D2
};
constexpr uint8_t KB_ColumnCount = CONST_LENGTH(KB_ColumnPins);


static const char keyChars[KB_ColumnCount][KB_RowCount] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


static bool handleKeyboard(
    KeyboardModule::KeyHandler p_pressHandler,
    KeyboardModule::KeyHandler p_releaseHandler,
    KeyboardModule::KeyHandler p_continuousHandler
){
   bool keyDown = false;

   // scan for keypress
   for (byte col = 0; col < CONST_LENGTH(KB_ColumnPins); col++){
       
    	// set column pin mode to OUTPUT and 
        // set column pin to LOW
       	pinMode(KB_ColumnPins[col], OUTPUT);
       	digitalWrite(KB_ColumnPins[col],LOW);
      
       	for (byte row = 0; row < CONST_LENGTH(KB_RowPins); row++){ 
           	// check each row for LOW
           	pinMode(KB_RowPins[row], INPUT_PULLUP);           
           	if (digitalRead(KB_RowPins[row]) == LOW){
               // detected possible keypress - debounce               
               delay(10);
               
               // now check row again
               if (digitalRead(KB_RowPins[row]) == LOW)
                {
                  	// valid keypress - send coordinates to processKey
                	keyDown = true;
					if(p_pressHandler != NULL){
						p_pressHandler(row, col);
					}
                  
                                   
                  	// Wait for key to be released.
                	do
                	{
                      	//delay before checking pin state
                    	delay(1); 
						if(p_continuousHandler != NULL){
							p_continuousHandler(row, col);
						}
					} while (digitalRead(KB_RowPins[row]) == LOW); 
					
					if(p_releaseHandler != NULL){
						p_releaseHandler(row, col);
					}
                	break;                     
                }              
            }
          pinMode(KB_RowPins[row], INPUT);
          if (keyDown == true) break;
       	}
       pinMode(KB_ColumnPins[col], INPUT);
       if (keyDown == true)break;       
    }
     
     return keyDown;
}

void KeyboardModule::setPressHandler(KeyHandler p_pressHandler){
    mp_pressHandler = p_pressHandler;
}
void KeyboardModule::setReleaseHandler(KeyHandler p_releaseHandler){
    mp_releaseHandler = p_releaseHandler;
}
void KeyboardModule::setContinuousHandler(KeyHandler p_continuousHandler){
    mp_continuousHandler = p_continuousHandler;
}

Option<char> KeyboardModule::rowColToChar(uint8_t row, uint8_t col)
{
    if(row < KB_RowCount && col < KB_ColumnCount){
        return Some(keyChars[row][col]);
    }
    
    return None<char>();
}

KeyboardModule::InitializationState KeyboardModule::init()
{
	for(byte row = 0; row < CONST_LENGTH(KB_RowPins); row++){
       pinMode(KB_RowPins[row], INPUT);
    }

    for(byte column = 0; column < CONST_LENGTH(KB_ColumnPins); column++){
        pinMode(KB_ColumnPins[column], INPUT);
    }

    return InitializationState::Initialized;
}

void KeyboardModule::run(){
    handleKeyboard(
        mp_pressHandler,
        mp_releaseHandler,
        mp_continuousHandler
    );
}


}// keyboard