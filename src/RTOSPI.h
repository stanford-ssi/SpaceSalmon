#pragma once

#include <SPI.h>
#include "Mutex.hpp"
#include "Semaphore.hpp"

class RTOSPI : public SPIClass
{
    public:
        using SPIClass::SPIClass;
        
        void begin(){
            SPIClass::begin();
            int8_t num = getSercomIndex();
            table[num] = this; //register in table
        }

        // obtain mutex lock, start transfer, and block thread until finished
        void dmaTransfer(void *txbuf, void *rxbuf, uint32_t count)
        {
            lock.take(NEVER);
            SPIClass::transfer(txbuf, rxbuf, count, &spi_cb);
            wait.take(NEVER);
            lock.give();
        }
        
    private:
        Mutex lock;
        
        Semaphore wait;

        static RTOSPI *table[SERCOM_INST_NUM];
        
        static void spi_cb(uint8_t sercom_num)
        {
            table[sercom_num]->wait.giveFromISR();
        }
};