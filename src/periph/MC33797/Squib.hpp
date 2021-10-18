#pragma once

#include "MC33797.h"
#include "Arduino.h"
#include "SPI.h"

class Squib
{
  public:
    typedef Squib_ReturnType Return_t;
    typedef Squib_FireType Fire_t;
    typedef Squib_DriverModeType DriverMode_t;
    typedef Squib_StatusType Status_t;
    typedef Squib_ProgCmdType ProgCmd_t;
    //contructor, provide SPI interface and cspin
    Squib(SPIClass &spi, int8_t cspin);
    //fire some squibs!
    Return_t fire(Fire_t fire);
    //read the status
    Return_t getStatus();
    //here is the status
    Status_t status;

    Return_t SingleCmd(void);

    Return_t GetStatus(Status_t *Status);

    Return_t ProgramCmd(ProgCmd_t Command, uint8_t Data, uint8_t Delay, uint8_t *SpiResponse);

    Ard_ComReturnType Ard_ComSendByte(uint8_t b, uint8_t *c);

    Return_t Init();

    SPIClass &SquibSPI;
    uint8_t SquibCS;
    uint8_t SquibCmdResp;
    DriverMode_t SquibDriverStatus = SQB_UNINIT; /* status of the SQUIB1 driver - default value is uninitialized */
};
