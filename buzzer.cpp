#include "buzzer.h"
#include "timer.h"

#include "buzzer_notes.h"

namespace module { // buzzer

struct Sound{
	static constexpr Sound END(){
		return Sound{0,0};
	}

	static constexpr Sound Pause(uint16_t duration){
		return Sound{0, duration};
	}

	bool operator == ( const Sound& rhs) const{
		return (hz == rhs.hz && duration == rhs.duration);
	}

	bool operator != ( const Sound& rhs) const{
		return (hz != rhs.hz || duration != rhs.duration);
	}

	uint16_t hz;
	uint16_t duration;
};

struct SoundMapping{
	constexpr SoundMapping() : soundPreset(BuzzerSoundsModule::SoundPreset::VOLUME_TEST), p_sound(nullptr) {}
	constexpr SoundMapping(const BuzzerSoundsModule::SoundPreset soundPreset, const Sound* p_sound) : soundPreset(soundPreset), p_sound(p_sound) {}
	const BuzzerSoundsModule::SoundPreset soundPreset;
	const Sound* p_sound;
};

static const Sound* p_currentSound = nullptr;
static TimedExecution10ms nextTone;


constexpr uint16_t SHORT_NOTE_DURATION = 80;
constexpr uint16_t MID_NOTE_DURATION = 120;
constexpr uint16_t LONG_NOTE_DURATION = 160;
constexpr uint16_t LONG_NOTE_DURATION_2X = LONG_NOTE_DURATION * 2;
constexpr uint16_t LONG_NOTE_DURATION_3X = LONG_NOTE_DURATION * 3;
constexpr uint16_t LONG_NOTE_DURATION_4X = LONG_NOTE_DURATION * 4;

inline const PROGMEM Sound OF_LIBER_TEA[]{

	{NOTE_A5, LONG_NOTE_DURATION},
	{NOTE_D4, LONG_NOTE_DURATION},

	{NOTE_D3,SHORT_NOTE_DURATION},
	{NOTE_F4, LONG_NOTE_DURATION},
	{NOTE_D4, LONG_NOTE_DURATION},

	{NOTE_D3,SHORT_NOTE_DURATION},
	{NOTE_F4, LONG_NOTE_DURATION},
	{NOTE_D4, LONG_NOTE_DURATION},

	{NOTE_D3,SHORT_NOTE_DURATION},
	{NOTE_F4, LONG_NOTE_DURATION},
	{NOTE_D4, LONG_NOTE_DURATION},

	{NOTE_D3,SHORT_NOTE_DURATION},
	{NOTE_F7, LONG_NOTE_DURATION},
	{NOTE_D7, LONG_NOTE_DURATION},

	{NOTE_D3,SHORT_NOTE_DURATION},
	{NOTE_F7, LONG_NOTE_DURATION},
	{NOTE_D7, LONG_NOTE_DURATION},

	{NOTE_D3,SHORT_NOTE_DURATION},
	{NOTE_F7, LONG_NOTE_DURATION},
	{NOTE_D7, LONG_NOTE_DURATION},

	{NOTE_D3,SHORT_NOTE_DURATION},
	{NOTE_F7, LONG_NOTE_DURATION},
	{NOTE_D7, LONG_NOTE_DURATION},

	{NOTE_D3,SHORT_NOTE_DURATION},
	{NOTE_F7, LONG_NOTE_DURATION},
	{NOTE_D7, LONG_NOTE_DURATION},

	{NOTE_D3,SHORT_NOTE_DURATION},
	{NOTE_F7, LONG_NOTE_DURATION},
	{NOTE_D7, LONG_NOTE_DURATION},
	
	{NOTE_AS5, LONG_NOTE_DURATION_4X-40},
	Sound::Pause(200),

	{NOTE_F5, LONG_NOTE_DURATION_2X},
	{NOTE_E5, LONG_NOTE_DURATION_2X},
	{NOTE_D5, LONG_NOTE_DURATION_2X},
	{NOTE_A4, LONG_NOTE_DURATION_3X},
	{NOTE_C5, LONG_NOTE_DURATION_3X},
	{NOTE_D5, LONG_NOTE_DURATION_3X},
	Sound::Pause(300),
	{NOTE_F5, LONG_NOTE_DURATION_2X},
	{NOTE_E5, LONG_NOTE_DURATION_2X},
	{NOTE_D5, LONG_NOTE_DURATION_2X},
	{NOTE_F5, LONG_NOTE_DURATION_2X},
	Sound::Pause(150),
	{NOTE_C5, LONG_NOTE_DURATION_2X},
	{NOTE_C5, LONG_NOTE_DURATION_2X},
	{NOTE_D5, LONG_NOTE_DURATION_2X},
	{NOTE_A5, LONG_NOTE_DURATION_2X},
	{NOTE_C4, LONG_NOTE_DURATION},
	{NOTE_G5, LONG_NOTE_DURATION_3X},

	Sound::END()
};

static const PROGMEM Sound MELODY_KEY_PRESS[]{
	{NOTE_G5, MID_NOTE_DURATION},
	{NOTE_C7, SHORT_NOTE_DURATION},
	
	Sound::END()
};

static const PROGMEM Sound MELODY_FAIL[]{
	{NOTE_F3, MID_NOTE_DURATION},
	{NOTE_E3, MID_NOTE_DURATION},

	Sound::END()
};

static const PROGMEM Sound MELODY_SUCCESS[]{
	{NOTE_A7, LONG_NOTE_DURATION},
	{NOTE_A6, LONG_NOTE_DURATION},
	{NOTE_A5, LONG_NOTE_DURATION},

	Sound::END()
};

static const PROGMEM Sound MELODY_VOLUME_TEST[]{
	{NOTE_D2, MID_NOTE_DURATION},
	{NOTE_A7, MID_NOTE_DURATION},

	Sound::END()
};

static const PROGMEM SoundMapping soundMapping[]{
	{BuzzerSoundsModule::SoundPreset::BUTTON_PRESS, MELODY_KEY_PRESS},
	{BuzzerSoundsModule::SoundPreset::FAIL, MELODY_FAIL},
	{BuzzerSoundsModule::SoundPreset::SUCCESS, MELODY_SUCCESS},
	{BuzzerSoundsModule::SoundPreset::VOLUME_TEST, MELODY_VOLUME_TEST},
	{BuzzerSoundsModule::SoundPreset::LIBER_TEA, OF_LIBER_TEA},

};



void playNextTone(TimedExecution10ms&){
	if(p_currentSound == nullptr){
		return;
	}
	Sound currentSound;
	PROGMEM_READ_STRUCTURE(&currentSound, ++p_currentSound);
	uint8_t buzzerPin = Buzzer.getCurrentUsedPin();
	if(currentSound == Sound::END()){
		noTone(buzzerPin);
		p_currentSound = nullptr;
		return;
	}
	if(currentSound.hz == 0){
		noTone(buzzerPin);
	}
	else {
		tone(buzzerPin, currentSound.hz, currentSound.duration);
	}
	nextTone.setup(playNextTone, currentSound.duration);
}

void playMelody(const Sound* p_melody){
	uint8_t buzzerPin = Buzzer.getCurrentUsedPin();
	noTone(buzzerPin);
	p_currentSound = p_melody;
	if(p_currentSound == nullptr){
		return;
	}
	Sound currentSound;
	PROGMEM_READ_STRUCTURE(&currentSound, p_melody);
	if(currentSound == Sound::END()){
		return;
	}
		
	tone(buzzerPin, currentSound.hz, currentSound.duration);
	nextTone.setup(playNextTone, currentSound.duration);
}

BuzzerSoundsModule::InitializationState BuzzerSoundsModule::init(){
	setVolume(HIGH);
	
	return InitializationState::Initialized;
}

void BuzzerSoundsModule::setVolume(uint8_t volume) {
	if(volume == HIGH){
		mi_buzzerPin = HIGH_VOLUME_PIN;
	}
	else if(volume == LOW){
		mi_buzzerPin = LOW_VOLUME_PIN;
	}
}

uint8_t BuzzerSoundsModule::getVolume() const {
	return mi_buzzerPin == HIGH_VOLUME_PIN ? HIGH : LOW;
}

uint8_t BuzzerSoundsModule::getCurrentUsedPin() const {
	return mi_buzzerPin;
}

void BuzzerSoundsModule::playPreset(SoundPreset soundPreset){
	SoundMapping soundMappedList[CONST_LENGTH(soundMapping)];
	memcpy_P(soundMappedList, soundMapping, sizeof(soundMappedList));

	for(SoundMapping entry : soundMappedList){
		if(entry.soundPreset == soundPreset){
			playMelody(entry.p_sound);
			break;
		}
	}
}

void BuzzerSoundsModule::stop(){
	playMelody(nullptr);
}

} // buzzer