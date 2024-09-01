#include "helldef.h"
#include "timer.h"

namespace module{ // timer

	TimerModule::InitializationState TimerModule::init(){
		mi_lastTime = millis();
		return InitializationState::Initialized;
	}

	void TimerModule::run() {
		constexpr uint32_t TICK_VALUE = TimedExecution10ms::StaticTimerBase::TICK_VALUE;
			
		unsigned long i_delta = millis() - mi_lastTime;

		if(i_delta >= TICK_VALUE){
			unsigned long i_compensate = i_delta / TICK_VALUE;
			for(uint32_t i_count = 0; i_count < i_compensate; i_count++){
				TimedExecution10ms::StaticTimerBase::tickAllTimers();
			}
			mi_lastTime = millis();
		}
					
		TimedExecution10ms::executeAllTimedExecutions();
	}
} // timer
