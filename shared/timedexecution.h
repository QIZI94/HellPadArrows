#ifndef STATIC_MODULE
#define STATIC_MODULE

#include "statictimer.h"

template<class TimerBase>
class TimedExecution{
public:
using ExecFunPtr = void (*)(TimedExecution&);
using StaticTimerBase = TimerBase;
public:
    TimedExecution(){
        // register timer upon creating the instance into doubly linked list
        //enable();
    }
    ~TimedExecution(){
        // unregister timer upon destruction,
        // this more of a memory safety measure if timer is defined on stack and function goes out of scope it will remove it-self
        disable();        
    }
    // register timer instance into doubly linked list
    // enables countdown by tickAllTimers() function
    void enable(){
        if(!isEnabled()){
            TimedExecution** begin = getTimedExecutionListBegin();
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
            *getTimedExecutionListBegin() = next;
        }
        
        if(next != nullptr)
            next->prev = prev;
        
        prev = nullptr;
        next = nullptr;
    }

    bool isEnabled() const{
        return (prev != nullptr || next != nullptr || this == *getTimedExecutionListBegin());
    }

	void setExecFunction(ExecFunPtr exec){
        execPtr = exec;
    }

	void setup(ExecFunPtr execFunction, uint32_t interval, bool enableTimer = true){
		setExecFunction(execFunction);
		timer.reset(interval);
		if(enableTimer){
			enable();
			timer.enable();
		}
	}

	void restart(uint32_t interval){
		timer.reset(interval);
		enable();
		timer.enable();
	}
    
    void exec(){
        if(execPtr != nullptr){
            if(timer.isDown()){
				disable();
                execPtr(*this);
            }
        }
    }
    
    // this function will be called by timer interrupt and handle registered timers countdowns
    static void executeAllTimedExecutions(){
        TimedExecution* currentTimedExecution = *getTimedExecutionListBegin();

        while(currentTimedExecution != nullptr){
           	currentTimedExecution->exec();
            currentTimedExecution = currentTimedExecution->next;
        }
    }
public:
	StaticTimerBase timer;

private:
    //basically a singelthon which will return pointer to a pointer of the beging of managed linked list
    static TimedExecution** getTimedExecutionListBegin(){
        static TimedExecution* begin = nullptr;
        return &begin;
    }
	
    TimedExecution* prev = nullptr;
    TimedExecution* next = nullptr;
	ExecFunPtr execPtr = nullptr;
};










#endif