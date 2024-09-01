#ifndef MANAGED_MODULE
#define MANAGED_MODULE

namespace module{
class ManagedModule{
public: // definitions
	enum class InitializationState: int8_t{
		NotStarted,
		Initialized,
		Initializing,
		Failed,
		TooManyAttempts
	};

	using InitializationWatcherFunPtr = void (*)(const ManagedModule&);

protected: // member functions
    ManagedModule(const char* moduleInstanceName, bool autoenable = true, uint8_t initializationAttempts = *GetDefaultInitializationAttemptCount())
    : moduleInstanceName(moduleInstanceName), initializationAttemptsLeft(initializationAttempts){
        if(autoenable){
            enable();
        }
    }

	~ManagedModule(){
		disable();
	}

    virtual InitializationState init(){
        return InitializationState::Initialized;
    }

    virtual void run(){}

public: // members
    void enable(){
        if(!isEnabled()){
            ManagedModule** begin = getModulesListBegin();
            if(*begin == nullptr){
                *begin = this;
            }
            else{
                (*begin)->prev = this;
                this->next = *begin;
                *begin = this;
            }
        }
    }

    void disable(){
        if(prev != nullptr)
            prev->next = next;
        else{
            *getModulesListBegin() = next;
        }
        
        if(next != nullptr)
            next->prev = prev;
        
        prev = nullptr;
        next = nullptr;
    }

    const char* name() const {
        return moduleInstanceName;
    }

    bool isEnabled() const{
        return (prev != nullptr || next != nullptr || this == *getModulesListBegin());
    }

    bool isInitialized() const {
        return initializationState == InitializationState::Initialized;
    }

	InitializationState getInitializationState() const {
		return initializationState;
	}

    static void runAllModules(){
        ManagedModule* currentModule = *getModulesListBegin();
		InitializationWatcherFunPtr* initializationWatcher = GetInitializationWatcher();
		

        while(currentModule != nullptr){
			
            if(!currentModule->isInitialized()){
               
				if(currentModule->initializationState == InitializationState::NotStarted){
					if(initializationWatcher != nullptr){
						(*initializationWatcher)(*currentModule);
					}
				}

                currentModule->initializationState = currentModule->init();

				if(currentModule->initializationState == InitializationState::Failed){
					currentModule->disable();
				}
				else if(currentModule->initializationState == InitializationState::Initializing){
					if(currentModule->initializationAttemptsLeft == 0){
						currentModule->initializationState = InitializationState::TooManyAttempts;
						currentModule->disable();
					}
					else{
						currentModule->initializationAttemptsLeft--;
					}
				}
				
				if(initializationWatcher != nullptr){
					(*initializationWatcher)(*currentModule);
				}
            }
            else{
                currentModule->run();
            }
            currentModule = currentModule->next;
        }
    }

public: // static member functions
	static void SetInitializationWatcherCallback(InitializationWatcherFunPtr initializationWatcher){
		*GetInitializationWatcher() = initializationWatcher;
	}

	static void SetDefaultMaxInitializationAttemptsCount(uint8_t maxAttempts){
		*GetDefaultInitializationAttemptCount() = maxAttempts;
	}

private: // member variables

    //basically a singelthon which will return pointer to a pointer of the beging of managed linked list
    static ManagedModule** getModulesListBegin(){
        static ManagedModule* begin = nullptr;
        return &begin;
    }

	static InitializationWatcherFunPtr* GetInitializationWatcher() {
		static InitializationWatcherFunPtr initializationWatcher = nullptr;
		return &initializationWatcher;
	}

	static uint8_t* GetDefaultInitializationAttemptCount() {
		static uint8_t maxAttemptCount = 50;
		return &maxAttemptCount;
	}
    
    ManagedModule* prev = nullptr;
    ManagedModule* next = nullptr;
    const char* moduleInstanceName = "";
    InitializationState initializationState = InitializationState::NotStarted;
	uint8_t initializationAttemptsLeft = 0;
	
	
};
}// module
#endif
