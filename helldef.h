#ifndef HELLARROWS_DEFINITIONS
#define HELLARROWS_DEFINITIONS

#include <Arduino.h>

#define CONST_LENGTH(arr) (sizeof(arr)/sizeof(arr[0]))
// Reads dereferenceable struct or primitive type from program memory that has been marked with PROGMEM
#define PROGMEM_READ_STRUCTURE(p_dst, p_src) do { memcpy_P(p_dst, p_src, sizeof(*p_dst));} while (0)

template<class T>
struct RemovePointer_t{

};


enum class Arrow{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

constexpr uint8_t ARROW_POSITIONS = 4;
constexpr uint8_t ARROW_MAX_SLOTS = 7;


enum class Stratagem{
	// orbital
	OrbitalGatlingBarrage,
	OrbitalAirburstStrike,
	Orbital120MM_HEBarrage,
	Orbital380MM_HEBarrage,
	OrbitalWalkingBarrage,
	OrbitalLaser,
	OrbitalRailcannonStrike,

	// eagle1
    Bomb500kg,
    EagleSmokeStrike,
    EagleAirstrike,
    EagleStrafingRun,
    EagleClusterBomb,
	EagleNapalmAirstrike,
    //JumpPack,
    Eagle110MMRocketPod,
    
    NUM_OF_STRATAGEMS
};

constexpr uint8_t BUZ_D10   = 10;
constexpr uint8_t BUZ_PIN   = BUZ_D10;

constexpr uint8_t TFT_DC    = 10;
constexpr uint8_t TFT_CS    = 9;




struct Pinout{
	static constexpr uint8_t P_A0 = PIN_A0;
	static constexpr uint8_t P_A1 = PIN_A1;
	static constexpr uint8_t P_A2 = PIN_A2;
	static constexpr uint8_t P_A3 = PIN_A3;
	static constexpr uint8_t P_A4 = PIN_WIRE_SDA;
	static constexpr uint8_t P_A5 = PIN_WIRE_SCL;
	static constexpr uint8_t P_A6 = PIN_A6;
	static constexpr uint8_t P_A7 = PIN_A7;

	static constexpr uint8_t P_D0 = 0;
	static constexpr uint8_t P_D1 = 1;
	static constexpr uint8_t P_D2 = 2;
	static constexpr uint8_t P_D3 = 3;
	static constexpr uint8_t P_D4 = 4;
	static constexpr uint8_t P_D5 = 5;
	static constexpr uint8_t P_D6 = 6;
	static constexpr uint8_t P_D7 = 7;
	static constexpr uint8_t P_D8 = 8;
	static constexpr uint8_t P_D9 = 9;

	static constexpr uint8_t P_D10 = PIN_SPI_SS;
	static constexpr uint8_t P_D11 = PIN_SPI_MOSI;
	static constexpr uint8_t P_D12 = PIN_SPI_MISO;
	static constexpr uint8_t P_D13 = PIN_SPI_SCK;

	struct Assignment{
		
		static constexpr uint8_t TFT_CLK	= P_D13;
		static constexpr uint8_t TFT_MISO	= P_D12;
		static constexpr uint8_t TFT_MOSI	= P_D11;
		static constexpr uint8_t TFT_CS		= P_D10;
		static constexpr uint8_t TFT_DC		= P_D9;

		static constexpr uint8_t KB_0		= P_D2;
		static constexpr uint8_t KB_1		= P_D3;
		static constexpr uint8_t KB_2		= P_D4;
		static constexpr uint8_t KB_3		= P_D5;
		static constexpr uint8_t KB_4		= P_D6;
		static constexpr uint8_t KB_5		= P_D7;
		static constexpr uint8_t KB_6		= P_D8;

		static constexpr uint8_t BUZ_HIGH	= P_A0;
		static constexpr uint8_t BUZ_LOW	= P_A1;
	};
};


#endif