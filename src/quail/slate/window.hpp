#pragma once

#include "../config.h"
#include <FreeRTOS.h>

class Window {
    //TODO: make reference to ticks 
    public:
        Window(){};

        void push(float val) { 
            curr = (curr + 1) % WINDOW_SIZE;
            arr[curr] = val; 

            lastAvg = currAvg;
            currAvg = _avg();
        }

        void operator+=(float val) { push(val); }

        float avg() { return currAvg; }

        float peek() { return arr[curr]; }

        float delta() { return currAvg - lastAvg; }

    private:
        float arr[WINDOW_SIZE];
        uint8_t curr = -1;

        float lastAvg = 0;
        float currAvg = 0;

        float _avg() {
            float sum = 0;
            
            for (uint8_t i = 0; i < WINDOW_SIZE; i++) {
                sum += arr[i];
            }
            return sum / WINDOW_SIZE;
        }
};