
#ifndef BUZZER_MODULE
#define BUZZER_MODULE

#include "helldef.h"
#include "shared/module.h"



namespace module{ // buzzer
class BuzzerSoundsModule : public ManagedModule{

public: 
	enum SoundPreset{
		BUTTON_PRESS,
		FAIL,
		SUCCESS,
		VOLUME_TEST,
		LIBER_TEA
	};

private:
	static constexpr uint8_t LOW_VOLUME_PIN = Pinout::Assignment::BUZ_HIGH;
	static constexpr uint8_t HIGH_VOLUME_PIN = Pinout::Assignment::BUZ_LOW;

public: 
    BuzzerSoundsModule(const char* moduleInstanceName, bool autoenable = true)
    : ManagedModule(moduleInstanceName, autoenable) {}
	// either HIGH or LOW
	void setVolume(uint8_t volume);
	uint8_t getVolume() const;
	uint8_t getCurrentUsedPin() const;


	void playPreset(SoundPreset soundPreset);
	void stop();

private:
    InitializationState init() override;
private:
	uint8_t mi_buzzerPin = HIGH_VOLUME_PIN;
};

inline module::BuzzerSoundsModule Buzzer("BuzzerSoundsModule");

} // buzzer



#endif