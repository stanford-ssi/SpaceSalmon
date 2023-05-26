#include "BuzzerTask.hpp"

BuzzerTask::BuzzerTask(uint8_t priority) : Task(priority, "Buzzer"){};

void BuzzerTask::activity()
{
    if(sys.silent){ 
        vTaskSuspend(getTaskHandle());
    }
    
    uint32_t timer = 0; // use for timing delays
    while (true) // main activity loop
    {
        vTaskDelayUntil(&timer, 5000); // a delay of 5 seconds

        bool pyroA = sys.pyro.getStatus(Pyro::SquibA);
        bool pyroB = sys.pyro.getStatus(Pyro::SquibB);

        FlightState state = sys.tasks.filter.plan.p_state;

        //One beep: indicates power
        sys.buzzer.set(2500);
        vTaskDelay(300);
        sys.buzzer.set(0);
        vTaskDelay(500);

        // If time exceeded, scary beep
        bool timeExceeded;
        sys.timeExceeded.get(timeExceeded);
        if (timeExceeded) {
            for (int i = 0; i< 20; i++) {
                sys.buzzer.set(3500);
                vTaskDelay(100);
                sys.buzzer.set(0);
                vTaskDelay(100);
            }
            
        }

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

        if (pyroA) // true if actual pyros connected to board
        {
            sys.buzzer.set(5000);
            vTaskDelay(100);
            sys.buzzer.set(0);
            vTaskDelay(300);
        }
        else
        {
            sys.buzzer.set(800);
            vTaskDelay(300);
            sys.buzzer.set(0);
            vTaskDelay(100);
        }

        if (pyroB)
        {
            sys.buzzer.set(5000);
            vTaskDelay(100);
            sys.buzzer.set(0);
            vTaskDelay(300);
        }
        else
        {
            sys.buzzer.set(800);
            vTaskDelay(300);
            sys.buzzer.set(0);
            vTaskDelay(100);
        }
    }
}