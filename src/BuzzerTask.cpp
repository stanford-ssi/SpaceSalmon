#include "BuzzerTask.hpp"

TaskHandle_t BuzzerTask::taskHandle = NULL;
StaticTask_t BuzzerTask::xTaskBuffer;
StackType_t BuzzerTask::xStack[stackSize];

BuzzerTask::BuzzerTask(uint8_t priority)
{
    BuzzerTask::taskHandle = xTaskCreateStatic(activity,                  //static function to run
                                               "BuzzerTask",              //task name
                                               stackSize,                 //stack depth (words!)
                                               NULL,                      //parameters
                                               priority,                  //priority
                                               BuzzerTask::xStack,        //stack object
                                               &BuzzerTask::xTaskBuffer); //TCB object
}

TaskHandle_t BuzzerTask::getTaskHandle()
{
    return taskHandle;
}

void BuzzerTask::activity(void *ptr)
{
    uint32_t timer = 0;
    while(true){
        vTaskDelayUntil(&timer, 5000);

        bool pyroA = sys.pyro.getStatus(Pyro::SquibA);
        bool pyroB = sys.pyro.getStatus(Pyro::SquibB);

        FlightState state = sys.tasks.filter.plan.getState();
        
        //One beep: indicates power
        sys.buzzer.set(2500);
        vTaskDelay(300);
        sys.buzzer.set(0);

        vTaskDelay(500);

        switch (state)
        {
        case Waiting:
            //no state beep
            vTaskDelay(300);
            break;
        case Flight:
            //one state beep
            sys.buzzer.set(5000);
            vTaskDelay(300);
            sys.buzzer.set(0);
            break;
        case Falling:
            //two state beeps
            sys.buzzer.set(5000);
            vTaskDelay(100);
            sys.buzzer.set(0);
            vTaskDelay(50);
            sys.buzzer.set(5000);
            vTaskDelay(100);
            sys.buzzer.set(0);
            break;
        case Landed:
            //three state beeps
            sys.buzzer.set(5000);
            vTaskDelay(66);
            sys.buzzer.set(0);
            vTaskDelay(25);
            sys.buzzer.set(5000);
            vTaskDelay(66);
            sys.buzzer.set(0);
            vTaskDelay(25);
            sys.buzzer.set(5000);
            vTaskDelay(66);
            sys.buzzer.set(0);
            break;
        }

        vTaskDelay(500);

        if(pyroA){
            sys.buzzer.set(5000);
            vTaskDelay(100);
            sys.buzzer.set(0);
            vTaskDelay(300);
        }else{
            sys.buzzer.set(800);
            vTaskDelay(300);
            sys.buzzer.set(0);
            vTaskDelay(100);
        }

        if(pyroB){
            sys.buzzer.set(5000);
            vTaskDelay(100);
            sys.buzzer.set(0);
            vTaskDelay(300);
        }else{
            sys.buzzer.set(800);
            vTaskDelay(300);
            sys.buzzer.set(0);
            vTaskDelay(100);
        }
        
    }
}