#include "shared/module.h"

void initializationWatcher(const module::ManagedModule& module){
	using InitState = module::ManagedModule::InitializationState;
	InitState moduleInitializationState = module.getInitializationState();

	switch(moduleInitializationState){
		case InitState::NotStarted:
			return;
			//Serial.print("STARTED     - ");
			break;
		
		case InitState::InitializationInProgress:
			return;
			//Serial.print("INITIALIZING- ");
			break;
		
		case InitState::Initialized:
			Serial.print(F("INITIALIZED - "));
			break;

		case InitState::Failed:
			Serial.print(F("FAILED      - "));
			break;
		case InitState::TooManyAttempts:
			Serial.print(F("TIMEDOUT    - "));
			break;
	}

	Serial.println(module.name());
}


void setup(){
	Serial.begin(115200);
	Serial.println("------ Started -------");
	module::ManagedModule::SetInitializationWatcherCallback(initializationWatcher);
	module::ManagedModule::SetDefaultMaxInitializationAttemptsCount(50);
}

void loop() {
	module::ManagedModule::runAllModules();
}


