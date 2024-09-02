#ifndef HELLKEYBOARD
#define HELLKEYBOARD

#include <Keypad.h>

#include "shared/module.h"
#include "shared/optional.h"

namespace module{
class KeyboardModule : public ManagedModule{
public: // member functions
    KeyboardModule(const char* moduleInstanceName, bool autoenable = true);
private: // member functions
    void run() override;
public: 
	Keypad keypad;
};

inline KeyboardModule Keyboard("KeyboardModule");

} // module

#endif