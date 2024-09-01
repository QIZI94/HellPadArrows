#ifndef HELLARROWS_DEFINITIONS
#define HELLARROWS_DEFINITIONS

#include <Arduino.h>


#define CONST_LENGTH(arr) (sizeof(arr)/sizeof(arr[0]))

enum class Arrow{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

constexpr uint8_t ARROW_POSITIONS = 4;
constexpr uint8_t ARROW_MAX_SLOTS = 6;


enum class Stratagem{
    Bomb500kg,
    EagleSmokeStrike,
    EagleAirstrike,
    EagleStrafingRun,
    EagleClusterBomb,
    JumpPack,
    Eagle110MMRocketPod,
    
    NUM_OF_STRATAGEMS
};

constexpr uint8_t BUZ_D10   = 10;
constexpr uint8_t BUZ_PIN   = BUZ_D10;

constexpr uint8_t TFT_DC    = 10;
constexpr uint8_t TFT_CS    = 9;


#endif