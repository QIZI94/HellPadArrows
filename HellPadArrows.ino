#include "shared/module.h"

void initializationWatcher(const module::ManagedModule& module){
	using InitState = module::ManagedModule::InitializationState;
	InitState moduleInitializationState = module.getInitializationState();

	switch(moduleInitializationState){
		case InitState::NotStarted:
			return;
			//Serial.print("STARTED     - ");
			break;
		
		case InitState::Initializing:
			return;
			//Serial.print("INITIALIZING- ");
			break;
		
		case InitState::Initialized:
			Serial.print("INITIALIZED - ");
			break;

		case InitState::Failed:
			Serial.print("FAILED      - ");
			break;
		case InitState::TooManyAttempts:
			Serial.print("TIMEDOUT    - ");
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


