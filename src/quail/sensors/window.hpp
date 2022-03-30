#pragma once

#include "../config.h"
#include <FreeRTOS.h>

class Window {
    public:
        Window(){};

        void push(float val) { 
            curr = (curr + 1) % WINDOW_SIZE;
            arr[curr] = val; 
        }
        
        void operator+=(float val) { push(val); }

        float peek() { return arr[curr]; }

        float avg() {
            float sum = 0;
            
            for (uint8_t i = 0; i < WINDOW_SIZE; i++) {
                sum += arr[i];
            }
            return sum / WINDOW_SIZE;
        }

        float delta() { return arr[curr] - arr[(curr + WINDOW_SIZE - 1) % WINDOW_SIZE]; }

    private:
        float arr[WINDOW_SIZE];
        uint8_t curr = -1;
};