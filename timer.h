#ifndef TIMER
#define TIMER

#include "shared/module.h"
#include "shared/timedexecution.h"

using StaticTimer10ms = StaticTimer<10>;
using TimedExecution10ms = TimedExecution<StaticTimer10ms>;

namespace module{ // timer

	class TimerModule : public ManagedModule {
	public:
		TimerModule(const char* moduleInstanceName, bool autoenable = true)
    	: ManagedModule(moduleInstanceName, autoenable) {}
	protected:
		InitializationState init() override;
		void run() override;
	private:
		unsigned long mi_lastTime = 0;
	};

	inline TimerModule ManagedTimers("TimerModule");
} // timer

#endif