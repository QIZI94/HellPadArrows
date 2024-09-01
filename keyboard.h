#ifndef HELLKEYBOARD
#define HELLKEYBOARD
#include "helldef.h"
#include "shared/module.h"
#include "shared/optional.h"


namespace module{
class KeyboardModule : public ManagedModule{
public:
    typedef void(*KeyHandler)(byte,byte);
public: 
    KeyboardModule(const char* moduleInstanceName, bool autoenable = true)
    : ManagedModule(moduleInstanceName, autoenable) {}
    void setPressHandler(KeyHandler p_pressHandler);
    void setReleaseHandler(KeyHandler p_releaseHandler);
    void setContinuousHandler(KeyHandler p_continuousHandler);
public:
    static Option<char> rowColToChar(uint8_t row, uint8_t col);
private:
    InitializationState init() override;
    void run() override;

private:
    KeyHandler mp_pressHandler = nullptr;
    KeyHandler mp_releaseHandler = nullptr;
    KeyHandler mp_continuousHandler = nullptr;
};

inline KeyboardModule Keyboard("KeyboardModule");

} // module

#endif