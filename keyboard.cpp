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


static uint8_t KB_RowPins[] = {
	Pinout::Assignment::KB_3,
	Pinout::Assignment::KB_2,
	Pinout::Assignment::KB_1,
	Pinout::Assignment::KB_0
	//KB_D5, KB_D4, KB_D3, KB_D2
};
constexpr uint8_t KB_RowCount = CONST_LENGTH(KB_RowPins);

static uint8_t KB_ColumnPins[] = {
	Pinout::Assignment::KB_6,
	Pinout::Assignment::KB_5,
	Pinout::Assignment::KB_4,

	//KB_D8, KB_D7, KB_D6	
};
constexpr uint8_t KB_ColumnCount = CONST_LENGTH(KB_ColumnPins);

static char keyChars[KB_RowCount][KB_ColumnCount] = {
    {'1','2','3',},
    {'4','5','6',},
    {'7','8','9',},
    {'*','0','#',}
};

KeyboardModule::KeyboardModule(const char* moduleInstanceName, bool autoenable = true)
: ManagedModule(moduleInstanceName, autoenable), keypad(makeKeymap(keyChars), KB_RowPins, KB_ColumnPins, KB_RowCount, KB_ColumnCount) {}

void KeyboardModule::run(){
	(void)keypad.getKeys();
}


}// keyboard