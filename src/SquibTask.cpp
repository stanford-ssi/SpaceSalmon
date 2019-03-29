#include "SquibTask.hpp"

SquibTask::SquibTask()
{
    //TODO: xTaskCreateStatic()
    if (xTaskCreate(activity, "Squib", 256, NULL, TASK_EXAMPLE_STACK_PRIORITY, &taskHandle) != pdPASS) {
        printf_("Sensor Task Creation Failed!");
	}
    
}

TaskHandle_t SquibTask::getTaskHandle(){
    return taskHandle;
}

void SquibTask::activity(void * ptr){
    while(true){
        vTaskDelay(1000);
        testSquibSpi(&SPI_SQUIB);
        //TODO: do things. (figure out a data flow model)
    }
}