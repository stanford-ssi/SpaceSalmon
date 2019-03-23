#include "MC33797.h"

/*********** Arming Commands & Masks ***********/

#define CMD_ARM_1A_1B 0xA0U            /* ARM Squib drivers 1A and 1B */
#define CMD_ARM_2A_2B 0xA1U            /* ARM Squib drivers 2A and 2B */
#define CMD_ARM_1A_1B_2A_2B 0xA2U      /* ARM Squib drivers 1A, 1B, 2A and 2B */
#define MASK_ARM_1A_1B_2A_2B 0xF0U     /* mask SQUIB 1A, 1B, 2A and 2B */
#define MASK_ARM_1_LOW_HIGH_SIDE 0x50U /* mask SQUIB 1A and 1B */
#define MASK_ARM_2_LOW_HIGH_SIDE 0xD0U /* mask SQUIB 2A and 2B */
#define SQUIB_ARM_MASK 0xF0U           /* mask arming command */
#define MASK_CMD_FIRE 0x0FU            /* mask firing command */
#define CMD_FIRE 0x50U                 /* four most significant bits of the firing commands */

/*********** Responses ***********/

#define RESP_SPI_CHECK 0x69U /* response to SPI integrity check */

struct spi_m_sync_descriptor *SquibSPI;
uint8_t SquibCS;

uint8_t SquibCmdResp;

/*********** Macros ***********/

#define SQUIB_GET_BOOL_STATUS(data, bit, ret, type) (ret = (type)(data & (1 << bit)))
#define SQUIB_SPI_ERROR(spi_ret, func_ret) \
  if (spi_ret != ARD_COM_OK)               \
  func_ret = SQB_NOT_OK

#define debug(text) //printf_("%s\n",text) //do nothing
#define debug_hex(num) //printf_("0x%x\n",num)//do nothing
#define delay(ms) delay_ms(ms)

Ard_ComReturnType Ard_ComSendByte(uint8_t b, uint8_t *c)
{
	struct spi_xfer data = {&b, c, 1};

	gpio_set_pin_level(SquibCS, false);
	spi_m_sync_transfer(SquibSPI, &data);
	gpio_set_pin_level(SquibCS, true);

  delay_ms(1);

  return ARD_COM_OK; //lol this could be better
}

/*********** Masking definition ***********/

#define SQB_MASK_01_BITS 0x03U /* mask bits 0 and 1 */
#define SQB_MASK_23_BITS 0x0CU /* mask bits 2 and 3 */
#define SQB_MASK_45_BITS 0x30U /* mask bits 4 and 5 */
#define SQB_MASK_67_BITS 0xC0U /* mask bits 6 and 7 */

/*********** Global variables ***********/

Squib_DriverModeType SquibDriverStatus = SQB_UNINIT; /* status of the SQUIB1 driver - default value is uninitialized */

/***************************************************************************/ /*!
*
*    @squib_function    Squib_Init
*    @squib_service_id  0x00U
*    @squib_sync_async  Synchronous
*    @squib_reentrancy  Non Reentrant
*    @squib_detailed_description
*                   - check FENx status
*                   - check correct SQUIB device type
*                   - set driver state machine to SQUIB_RUNNING
*                   - return result from function
*
*    @param[in]     Spi_Channel - virtual SPI channel number (not physical SPI channel)
*    @param[out]    None
*    @return        Squib_ReturnType - return value of the SQUIB driver API (SQB_OK / SQB_NOT_OK)
*    @brief         The function initializes the SQUIB module
*    @attention     N/A
*
*******************************************************************************/

Squib_ReturnType Squib_Init(struct spi_m_sync_descriptor *spi, uint8_t cspin)
{
  SquibSPI = spi;
  SquibCS = cspin;

  Squib_ReturnType ret_val;
  Ard_ComReturnType ret_com;
  Squib_DeviceChannelType Device = SQB_FOUR_CHANNEL_DEVICE;

  ret_val = SQB_OK;
  //printf_("ret_val: %d",ret_val);
  ret_com = ARD_COM_NOT_OK;

  if (SquibDriverStatus != SQB_UNINIT)
  {
    debug("Already Initalized");
    ret_val = SQB_NOT_OK;
    
  }
  else
  {
    /* SPI integrity check - Command 0x96 */
    ret_com = Ard_ComSendByte(SQB_CMD_SPI_CHECK, &SquibCmdResp);
    SQUIB_SPI_ERROR(ret_com, ret_val); /* if SPI communication error? */
    /* Get FEN Status - Command 0xC8 */
    ret_com = Ard_ComSendByte(SQB_CMD_FEN_RES_STAT, &SquibCmdResp); /* SPI command - FEN Status, R_LIMIT_X, R_DIAG Status, IC Type */
    SQUIB_SPI_ERROR(ret_com, ret_val);                              /* if SPI communication error? */
    debug_hex(SquibCmdResp);
    if (SquibCmdResp == RESP_SPI_CHECK)
    { /* if the SPI integrity check response is correct */
      /* NOP command - Command 0x00 */
      debug("Good SPI Response");
      ret_com = Ard_ComSendByte(SQB_CMD_NOP, &SquibCmdResp);                   /* read last response from the SQUIB driver in this Init function */
      debug_hex(SquibCmdResp);
      SQUIB_SPI_ERROR(ret_com, ret_val);                                       /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 7, Device, Squib_DeviceChannelType); /* bit 7 - response to the previous command 0xC8 */
    }
    else
    {
      debug("Bad SPI Response");
    }

    /* Check Squib device type */
    if (Device != SQB_FOUR_CHANNEL_DEVICE)
    {
      ret_val = SQB_NOT_OK;
    }

    /* Go to running (normal) mode - if everything is OK */
    if (ret_val == SQB_OK)
    {
      SquibDriverStatus = SQB_RUNNING;
      debug("Running");
    }
    else
    {
      SquibDriverStatus = SQB_UNINIT;
      debug("Not Running");
    }
  }
  return (ret_val);
}

/***************************************************************************/ /*!
* 
*    @squib_function    Squib_Fire
*    @squib_service_id  0x01U
*    @squib_sync_async  Synchronous
*    @squib_reentrancy  Non Reentrant
*    @squib_detailed_description
*                   - check firing command
*                   - arm selected group of the squibs
*                   - fire selected squib or group of the squibs
*                   - check SPI communication
*                   - return result from function
*
*    @param[in]     Spi_Channel - virtual SPI channel number (not physical SPI channel)
*    @param[in]     Squib_Fire - Squib fire command
*    @param[out]    None
*    @return        Squib_ReturnType - return value of the SQUIB driver API (SQB_OK / SQB_NOT_OK)
*    @brief         This function provide explosion of the selected Squib driver
*    @attention     N/A
*
*******************************************************************************/

Squib_ReturnType Squib_Fire(Squib_FireType Squib_Fire)
{
  Squib_ReturnType ret_val;
  Ard_ComReturnType ret_com;

  ret_val = SQB_OK;
  ret_com = ARD_COM_NOT_OK;

  if (SquibDriverStatus == SQB_UNINIT)
  {
    debug("Not Initialized!");
    ret_val = SQB_NOT_OK;
  }
  else
  {
    if ((Squib_Fire != CMD_FIRE_NO_SQUIBS) && (Squib_Fire != CMD_FIRE_1A) && (Squib_Fire != CMD_FIRE_1B) &&
        (Squib_Fire != CMD_FIRE_1A1B) && (Squib_Fire != CMD_FIRE_2A) && (Squib_Fire != CMD_FIRE_1A2A) &&
        (Squib_Fire != CMD_FIRE_1B2A) && (Squib_Fire != CMD_FIRE_1A2A1B) && (Squib_Fire != CMD_FIRE_2B) &&
        (Squib_Fire != CMD_FIRE_1A2B) && (Squib_Fire != CMD_FIRE_1B2B) && (Squib_Fire != CMD_FIRE_1A1B2B) &&
        (Squib_Fire != CMD_FIRE_2A2B) && (Squib_Fire != CMD_FIRE_1A2A2B) && (Squib_Fire != CMD_FIRE_2A1B2B) &&
        (Squib_Fire != CMD_FIRE_1A2A1B2B))
    {
      debug("Not a valid command!");
      ret_val = SQB_NOT_OK;
    }
    else
    {
      /* Arming Squibs command - for arming squibs pairs */
      debug("Firing!");
      ret_com = Ard_ComSendByte(CMD_ARM_1_AND_2_SQUIB_PAIRS, &SquibCmdResp);                        /* ARM squib drivers (ARM command "CMD_ARM_1_AND_2_SQUIB_PAIRS" is echoed back on the next command) */
      ret_com = Ard_ComSendByte((uint8_t)((Squib_Fire & MASK_CMD_FIRE) | CMD_FIRE), &SquibCmdResp); /* fire selected squibs (fire command "Squib_Fire" is echoed back on the next command) */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                            /* if SPI communication error? */
      debug("Fired!");
    }
  }
  return (ret_val);
}

/***************************************************************************/ /*!
*
*    @squib_function    Squib_GetStatus
*    @squib_service_id  0x02U
*    @squib_sync_async  Synchronous
*    @squib_reentrancy  Non Reentrant
*    @squib_detailed_description
*                   - read status of the Squibs Current Measurement Time
*                   - read status of the Squibs Current Limit Status
*                   - read status of the Squibs Thermal Shutdown Status
*                   - read the Short-to-Ground/Short-to-Battery Diagnostics
*                   - read status of the VFIRE_1B, VFIRE_2B, VDIAG_1 and VDIAG_2 Voltage
*                   - read status of the Low-Side Driver Continuity Status
*                   - read status of the FEN status, R_LIMIT_X, R_DIAG and device Type
*                   - read status of the Squib 1X resistance
*                   - read shorts between Squibs loops
*                   - check SPI communication
*                   - return result from function
*
*    @param[in]     Spi_Channel - virtual SPI channel number (not physical SPI channel)
*    @param[out]    *Status - status of the SQUIB device
*    @return        Squib_ReturnType - return value of the SQUIB driver API (SQB_OK / SQB_NOT_OK)
*    @brief         This function returns the status of the squib drivers (1A, 1B, 2A and 2B) and
*                   common status of the SQUIB device
*    @attention     N/A
*
*******************************************************************************/

Squib_ReturnType Squib_GetStatus(Squib_StatusType *Status)
{
  Squib_ReturnType ret_val;
  Ard_ComReturnType ret_com;

  ret_val = SQB_OK;
  ret_com = ARD_COM_NOT_OK;

  if (SquibDriverStatus == SQB_UNINIT)
  {
    ret_val = SQB_NOT_OK;
  }
  else
  {
    if (Status == NULL)
    {
      ret_val = SQB_NOT_OK;
    }
    else
    {
      /* VDIAG_1 and VDIAG_2 diagnostics - command 0xC6 */
      ret_com = Ard_ComSendByte(SQB_CMD_VDIAG, &SquibCmdResp); /* SPI command - VDIAG_1 and VDIAG_2 diagnostics */
      delay(SQB_MEASUREMENT_DELAY_LONG);                       /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                       /* if SPI communication error? */

      /* FEN status, R_LIMIT_X, R_DIAG status, IC type - command 0xC8 */
      ret_com = Ard_ComSendByte(SQB_CMD_FEN_RES_STAT, &SquibCmdResp);                           /* SPI command - FEN Status, R_LIMIT_X, R_DIAG Status, IC type */
      delay(SQB_MEASUREMENT_DELAY_LONG);                                                        /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                        /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 0, Status->Squib_StatV1diagV1, Squib_StatV1diagType); /* bit 0 - response to the previous command 0xC6 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 1, Status->Squib_StatV1diagV2, Squib_StatV1diagType); /* bit 1 - response to the previous command 0xC6 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 2, Status->Squib_StatV1diagV3, Squib_StatV1diagType); /* bit 2 - response to the previous command 0xC6 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 3, Status->Squib_StatV1diagV4, Squib_StatV1diagType); /* bit 3 - response to the previous command 0xC6 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 4, Status->Squib_StatV2diagV1, Squib_StatV2diagType); /* bit 4 - response to the previous command 0xC6 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 5, Status->Squib_StatV2diagV2, Squib_StatV2diagType); /* bit 5 - response to the previous command 0xC6 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 6, Status->Squib_StatV2diagV3, Squib_StatV2diagType); /* bit 6 - response to the previous command 0xC6 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 7, Status->Squib_StatV2diagV4, Squib_StatV2diagType); /* bit 7 - response to the previous command 0xC6 */

      /* Squib 1A current measurement time - commands 0x70 */
      ret_com = Ard_ComSendByte(SQB_CMD_1A_CURR_TIME, &SquibCmdResp);                              /* SPI command - squib 1A current measurement time */
      delay(SQB_MEASUREMENT_DELAY);                                                                /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                           /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 0, Status->Squib_StatFen1, Squib_StatFenType);           /* bit 0 - response to the previous command 0xC8 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 1, Status->Squib_StatFen2, Squib_StatFenType);           /* bit 1 - response to the previous command 0xC8 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 2, Status->Squib_StatFen1Latch, Squib_StatFenLatchType); /* bit 2 - response to the previous command 0xC8 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 3, Status->Squib_StatFen2Latch, Squib_StatFenLatchType); /* bit 3 - response to the previous command 0xC8 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 4, Status->Squib_StatRdiag, Squib_StatRdiagType);        /* bit 4 - response to the previous command 0xC8 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 5, Status->Squib_StatRlimit1, Squib_StatRlimitType);     /* bit 5 - response to the previous command 0xC8 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 6, Status->Squib_StatRlimit2, Squib_StatRlimitType);     /* bit 6 - response to the previous command 0xC8 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 7, Status->Squib_DeviceType, Squib_DeviceChannelType);   /* bit 7 - response to the previous command 0xC8 */

      /* Squib 1B current measurement time - commands 0x71 */
      ret_com = Ard_ComSendByte(SQB_CMD_1B_CURR_TIME, &SquibCmdResp); /* SPI command - squib 1B current measurement time */
      delay(SQB_MEASUREMENT_DELAY);                                   /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                              /* if SPI communication error? */
      Status->Squib_Stat1ACurrTime = SquibCmdResp;                    /* whole byte - response to the previous command 0x70 */

      /* Squib 2A current measurement time - commands 0x72 */
      ret_com = Ard_ComSendByte(SQB_CMD_2A_CURR_TIME, &SquibCmdResp); /* SPI command - squib 2A current measurement time */
      delay(SQB_MEASUREMENT_DELAY);                                   /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                              /* if SPI communication error? */
      Status->Squib_Stat1BCurrTime = SquibCmdResp;                    /* whole byte - response to the previous command 0x71 */

      /* Squib 2B current measurement time - commands 0x73 */
      ret_com = Ard_ComSendByte(SQB_CMD_2B_CURR_TIME, &SquibCmdResp); /* SPI command - squib 2B current measurement time */
      delay(SQB_MEASUREMENT_DELAY);                                   /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                              /* if SPI communication error? */
      Status->Squib_Stat2ACurrTime = SquibCmdResp;                    /* whole byte - response to the previous command 0x72 */

      /* Squib 1X current limit status - command 0x79 */
      ret_com = Ard_ComSendByte(SQB_CMD_CURR_LIMIT_STAT, &SquibCmdResp); /* SPI command - squib 1A, 1B, 2A nad 2B current limit statuses */
      delay(SQB_MEASUREMENT_DELAY);                                      /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                 /* if SPI communication error? */
      Status->Squib_Stat2BCurrTime = SquibCmdResp;                       /* whole byte - response to the previous command 0x73 */

      /* Thermal shutdown status - command 0x7F */
      ret_com = Ard_ComSendByte(SQB_CMD_THERM_STAT, &SquibCmdResp);                               /* SPI command - thermal shutdown status */
      delay(SQB_MEASUREMENT_DELAY);                                                               /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                          /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 0, Status->Squib_Stat1ACurrent, Squib_StatCurrentType); /* bit 0 - response to the previous command 0x79 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 1, Status->Squib_Stat1BCurrent, Squib_StatCurrentType); /* bit 1 - response to the previous command 0x79 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 2, Status->Squib_Stat2ACurrent, Squib_StatCurrentType); /* bit 2 - response to the previous command 0x79 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 3, Status->Squib_Stat2BCurrent, Squib_StatCurrentType); /* bit 3 - response to the previous command 0x79 */

      /* Vdiag and High-side safing sensor diagnostics - command 0xC0 */
      ret_com = Ard_ComSendByte(SQB_CMD_VDIAG_AND_HS_SAFING_STAT, &SquibCmdResp);                                         /* SPI command - Vdiag and High-side Safing sensor diagnostics */
      delay(SQB_MEASUREMENT_DELAY);                                                                                       /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                                                  /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 0, Status->Squib_Stat1AHighSideThermalShut, Squib_StatHighSideThermalShutType); /* bit 0 - response to the previous command 0x7F */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 1, Status->Squib_Stat1ALowSideThermalShut, Squib_StatLowSideThermalShutType);   /* bit 1 - response to the previous command 0x7F */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 2, Status->Squib_Stat1BHighSideThermalShut, Squib_StatHighSideThermalShutType); /* bit 2 - response to the previous command 0x7F */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 3, Status->Squib_Stat1BLowSideThermalShut, Squib_StatLowSideThermalShutType);   /* bit 3 - response to the previous command 0x7F */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 4, Status->Squib_Stat2AHighSideThermalShut, Squib_StatHighSideThermalShutType); /* bit 4 - response to the previous command 0x7F */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 5, Status->Squib_Stat2ALowSideThermalShut, Squib_StatLowSideThermalShutType);   /* bit 5 - response to the previous command 0x7F */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 6, Status->Squib_Stat2BHighSideThermalShut, Squib_StatHighSideThermalShutType); /* bit 6 - response to the previous command 0x7F */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 7, Status->Squib_Stat2BLowSideThermalShut, Squib_StatLowSideThermalShutType);   /* bit 7 - response to the previous command 0x7F */

      /* Squib short-to-ground/short-to-battery diagnostics - command 0xC1 */
      ret_com = Ard_ComSendByte(SQB_CMD_SHORT_STAT, &SquibCmdResp);                                     /* SPI command - Squib short-to-ground/short-to-battery diagnostics */
      delay(SQB_MEASUREMENT_DELAY);                                                                     /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                                /* if SPI communication error? */
      Status->Squib_Stat1VdiagResult = (Squib_Stat1VdiagResultType)(SquibCmdResp & SQB_MASK_01_BITS);   /* bit 0&1 - response to the previous command 0xC0 */
      Status->Squib_Stat1HSSafingSens = (Squib_Stat1HSSafingSensType)(SquibCmdResp & SQB_MASK_23_BITS); /* bit 2&3 - response to the previous command 0xC0 */
      Status->Squib_Stat2VdiagResult = (Squib_Stat2VdiagResultType)(SquibCmdResp & SQB_MASK_45_BITS);   /* bit 4&5 - response to the previous command 0xC0 */
      Status->Squib_Stat2HSSafingSens = (Squib_Stat2HSSafingSensType)(SquibCmdResp & SQB_MASK_67_BITS); /* bit 6&7 - response to the previous command 0xC0 */

      /* NOP command - command 0x00 */
      ret_com = Ard_ComSendByte(SQB_CMD_NOP, &SquibCmdResp);                            /* only to read response from the SQUIB driver */
      delay(SQB_MEASUREMENT_DELAY_LONG);                                                /* wait 2 ms */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 0, Status->Squib_1AShBatt, Squib_ShBattType); /* bit 0 - response to the previous command 0xC1 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 1, Status->Squib_1AShGnd, Squib_ShGndType);   /* bit 1 - response to the previous command 0xC1 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 2, Status->Squib_1BShBatt, Squib_ShBattType); /* bit 2 - response to the previous command 0xC1 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 3, Status->Squib_1BShGnd, Squib_ShGndType);   /* bit 3 - response to the previous command 0xC1 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 4, Status->Squib_2AShBatt, Squib_ShBattType); /* bit 4 - response to the previous command 0xC1 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 5, Status->Squib_2AShGnd, Squib_ShGndType);   /* bit 5 - response to the previous command 0xC1 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 6, Status->Squib_2BShBatt, Squib_ShBattType); /* bit 6 - response to the previous command 0xC1 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 7, Status->Squib_2BShGnd, Squib_ShGndType);   /* bit 7 - response to the previous command 0xC1 */

      /* Low-Side driver continuity status sent to SQUIB - command 0xC2 */
      ret_com = Ard_ComSendByte(SQB_CMD_LS_CONTINUITY_STAT, &SquibCmdResp); /* SPI command sent - Low-Side driver continuity status */
      delay(SQB_MEASUREMENT_DELAY);                                         /* measurement waiting */
      /* NOP command - command 0x00 */
      ret_com = Ard_ComSendByte(SQB_CMD_NOP, &SquibCmdResp);                                              /* only to read response from the SQUIB driver */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                                  /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 0, Status->Squib_Stat1ALowSideCont, Squib_StatLowSideContType); /* bit 0 - response to the previous command 0xC2 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 1, Status->Squib_Stat1BLowSideCont, Squib_StatLowSideContType); /* bit 1 - response to the previous command 0xC2 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 2, Status->Squib_Stat2ALowSideCont, Squib_StatLowSideContType); /* bit 2 - response to the previous command 0xC2 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 3, Status->Squib_Stat2BLowSideCont, Squib_StatLowSideContType); /* bit 3 - response to the previous command 0xC2 */

      /* Harness short-to-ground / short-to-battery with Squib open (no Squib present) - command 0xC3 */
      ret_com = Ard_ComSendByte(SQB_CMD_SHORT_GND_BAT_STAT, &SquibCmdResp); /* SPI command - Harness short-to-ground / short-to-battery with Squib open (no squib present) */
      delay(SQB_MEASUREMENT_DELAY);                                         /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                    /* if SPI communication error? */

      /* VFIRE_1B and VFIRE_2B voltage - command 0xC5 */
      ret_com = Ard_ComSendByte(SQB_CMD_VFIRE_1B_2B_STAT, &SquibCmdResp);                     /* SPI command - VFIRE_1B and VFIRE_2B voltage */
      delay(SQB_MEASUREMENT_DELAY);                                                           /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                      /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 0, Status->Squib_1AOpnShBatt, Squib_OpnShBattType); /* bit 0 - response to the previous command 0xC3 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 1, Status->Squib_1AOpnShGnd, Squib_OpnShGndType);   /* bit 1 - response to the previous command 0xC3 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 2, Status->Squib_1BOpnShBatt, Squib_OpnShBattType); /* bit 2 - response to the previous command 0xC3 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 3, Status->Squib_1BOpnShGnd, Squib_OpnShGndType);   /* bit 3 - response to the previous command 0xC3 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 4, Status->Squib_2AOpnShBatt, Squib_OpnShBattType); /* bit 4 - response to the previous command 0xC3 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 5, Status->Squib_2AOpnShGnd, Squib_OpnShGndType);   /* bit 5 - response to the previous command 0xC3 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 6, Status->Squib_2BOpnShBatt, Squib_OpnShBattType); /* bit 6 - response to the previous command 0xC3 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 7, Status->Squib_2BOpnShGnd, Squib_OpnShGndType);   /* bit 7 - response to the previous command 0xC3 */

      /* VFIRE_RTN Status (Open Ground) - command 0xC9 */
      ret_com = Ard_ComSendByte(SQB_CMD_VFIRE_RTN_STAT, &SquibCmdResp);                                   /* SPI command - VFIRE_RTN Status (open ground) */
      delay(SQB_MEASUREMENT_DELAY);                                                                       /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                                  /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 4, Status->Squib_StatVfireBTested, Squib_StatVfireBTestedType); /* bit 4 - response to the previous command 0xC5 */
      Status->Squib_StatVfire = (Squib_StatVfireType)(SquibCmdResp & SQB_MASK_01_BITS);                   /* bits 0 and 1 */

      /* Squib 1A resistance - commands 0xD0 */
      ret_com = Ard_ComSendByte(SQB_CMD_RES_1A_STAT, &SquibCmdResp);                               /* SPI command - Squib 1A resistance */
      delay(SQB_MEASUREMENT_DELAY);                                                                /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                           /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 0, Status->Squib_StatVfireRtn1, Squib_StatVfireRtnType); /* bit 0 - response to the previous command 0xC9 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 1, Status->Squib_StatVfireRtn2, Squib_StatVfireRtnType); /* bit 1 - response to the previous command 0xC9 */

      /* Squib 1B resistance - commands 0xD1 */
      ret_com = Ard_ComSendByte(SQB_CMD_RES_1B_STAT, &SquibCmdResp); /* SPI command - Squib 1B resistance */
      delay(SQB_MEASUREMENT_DELAY);                                  /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                             /* if SPI communication error? */
      Status->Squib_Stat1AResistance = SquibCmdResp;                 /* whole byte - response to the previous command 0xD0 */

      /* Squib 2A resistance - commands 0xD2 */
      ret_com = Ard_ComSendByte(SQB_CMD_RES_2A_STAT, &SquibCmdResp); /* SPI command - Squib 2A resistance */
      delay(SQB_MEASUREMENT_DELAY);                                  /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                             /* if SPI communication error? */
      Status->Squib_Stat1BResistance = SquibCmdResp;                 /* whole byte - response to the previous command 0xD1 */

      /* Squib 2B resistance - commands 0xD3 */
      ret_com = Ard_ComSendByte(SQB_CMD_RES_2B_STAT, &SquibCmdResp); /* SPI command - Squib 2B resistance */
      delay(SQB_MEASUREMENT_DELAY);                                  /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                             /* if SPI communication error? */
      Status->Squib_Stat2AResistance = SquibCmdResp;                 /* whole byte - response to the previous command 0xD2 */

      /* Shorts between Squib loops, Squib 1A - command 0xE0 */
      ret_com = Ard_ComSendByte(SQB_CMD_SHORTS_1A_STAT, &SquibCmdResp); /* SPI command - shorts between Squib loops, Squib 1A */
      delay(SQB_MEASUREMENT_DELAY);                                     /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                /* if SPI communication error? */
      Status->Squib_Stat2BResistance = SquibCmdResp;                    /* whole byte - response to the previous command 0xD3 */

      /* Shorts between Squib loops, Squib 1B - command 0xE1 */
      ret_com = Ard_ComSendByte(SQB_CMD_SHORTS_1B_STAT, &SquibCmdResp); /* SPI command - shorts between Squib loops, Squib 1B */
      delay(SQB_MEASUREMENT_DELAY);                                     /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                /* if SPI communication error? */
      Status->Squib_Stat1ALoopsShorts = SquibCmdResp; /* whole byte - response to the previous command 0xE0 */

      /* Shorts between Squib loops, Squib 2A - command 0xE2 */
      ret_com = Ard_ComSendByte(SQB_CMD_SHORTS_2A_STAT, &SquibCmdResp); /* SPI command - shorts between Squib loops, Squib 2A */
      delay(SQB_MEASUREMENT_DELAY);                                     /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                /* if SPI communication error? */
      Status->Squib_Stat1BLoopsShorts = SquibCmdResp; /* whole byte - response to the previous command 0xE1 */

      /* Shorts between Squib loops, Squib 2B - command 0xE3 */
      ret_com = Ard_ComSendByte(SQB_CMD_SHORTS_2B_STAT, &SquibCmdResp); /* SPI command - shorts between Squib loops, Squib 2B */
      delay(SQB_MEASUREMENT_DELAY);                                     /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                /* if SPI communication error? */
      Status->Squib_Stat2ALoopsShorts = SquibCmdResp; /* whole byte - response to the previous command 0xE2 */

      /* NOP command - command 0x00 */
      ret_com = Ard_ComSendByte(SQB_CMD_NOP, &SquibCmdResp); /* read last response from the SQUIB driver and stop diagnostics */
      SQUIB_SPI_ERROR(ret_com, ret_val);                     /* if SPI communication error? */
      Status->Squib_Stat2BLoopsShorts = SquibCmdResp; /* whole byte - response to the previous command 0xE3 */

      /* Shorts between Squib loops for additional ICs - command 0xE8 */

      /* Shorts between Squib loops, for additional ICs - command 0xE8 */
      ret_com = Ard_ComSendByte(SQB_CMD_SHORT_LOOPS, &SquibCmdResp); /* SPI command - shorts between Squib loops, for additional ICs */
      delay(SQB_MEASUREMENT_DELAY);                                  /* measurement waiting */
      SQUIB_SPI_ERROR(ret_com, ret_val);                             /* if SPI communication error? */
      /* NOP command - command 0x00 */
      ret_com = Ard_ComSendByte(SQB_CMD_NOP, &SquibCmdResp);                                                        /* read last response from the SQUIB driver and stop diagnostics */
      SQUIB_SPI_ERROR(ret_com, ret_val);                                                                            /* if SPI communication error? */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 0, Status->Squib_Stat1ALoopsShortsAddIC, Squib_StatLoopsShortsAddICType); /* bit 0 - response to the previous command 0xE8 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 1, Status->Squib_Stat1BLoopsShortsAddIC, Squib_StatLoopsShortsAddICType); /* bit 1 - response to the previous command 0xE8 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 2, Status->Squib_Stat2ALoopsShortsAddIC, Squib_StatLoopsShortsAddICType); /* bit 2 - response to the previous command 0xE8 */
      SQUIB_GET_BOOL_STATUS(SquibCmdResp, 3, Status->Squib_Stat2BLoopsShortsAddIC, Squib_StatLoopsShortsAddICType); /* bit 3 - response to the previous command 0xE8 */
    }
  }
  return (ret_val);
}

/***************************************************************************/ /*!
*
*    @squib_function    Squib_ProgramCmd
*    @squib_service_id  0x03U
*    @squib_sync_async  Synchronous
*    @squib_reentrancy  Non Reentrant
*    @squib_detailed_description
*                   - send other special SQUIB command to device driver
*                   - read and return its response
*                   - check SPI communication
*                   - return result from function
*
*    @param[in]     Spi_Channel - virtual SPI channel number (not physical SPI channel)
*    @param[in]     Command - other programming command
*    @param[out]    Response - response from command
*    @return        Squib_ReturnType - return value of the SQUIB driver API (SQB_OK / SQB_NOT_OK)
*    @brief         This function 
*    @attention     N/A
*
*******************************************************************************/

Squib_ReturnType Squib_ProgramCmd(Squib_ProgCmdType Command, uint8_t Data, uint8_t Delay, uint8_t *SpiResponse)
{
  Squib_ReturnType ret_val;
  Ard_ComReturnType ret_com;

  ret_val = SQB_OK;
  ret_com = ARD_COM_NOT_OK;

  if (SquibDriverStatus == SQB_UNINIT)
  {
    ret_val = SQB_NOT_OK;
  }
  else
  {
      if ((Command != SQB_CMD_NOP) && (Command != SQB_1A_CURR_REG_RESET) && (Command != SQB_1B_CURR_REG_RESET) && (Command != SQB_2A_CURR_REG_RESET) && (Command != SQB_2B_CURR_REG_RESET) &&
          (Command != SQB_UNLOCK_FEN1_COUNTER_REG) && (Command != SQB_UNLOCK_FEN2_COUNTER_REG) && (Command != SQB_UNLOCK_TO_TEST_HIGH_SQUIBS) &&
          (Command != SQB_1A_HS_TRANSISTOR_TEST) && (Command != SQB_1B_HS_TRANSISTOR_TEST) && (Command != SQB_2A_HS_TRANSISTOR_TEST) && (Command != SQB_2B_HS_TRANSISTOR_TEST) &&
          (Command != SQB_UNLOCK_TO_TEST_LOW_SQUIBS) && (Command != SQB_1A_LS_TRANSISTOR_TEST) && (Command != SQB_1B_LS_TRANSISTOR_TEST) && (Command != SQB_2A_LS_TRANSISTOR_TEST) && (Command != SQB_2B_LS_TRANSISTOR_TEST) &&
          (Command != SQB_RFU_NVM_LOW) && (Command != SQB_RFU_NVM_HIGH) && (Command != SQB_RFU_NVM_ENAB) && (Command != SQB_RFU_TEST_MODE) && (Command != SQB_CMD_SHORT_STAT) && (Command != SQB_CMD_SPI_CHECK) &&
          (Command != SQB_CMD_1A_CURR_TIME) && (Command != SQB_CMD_1B_CURR_TIME) && (Command != SQB_CMD_2A_CURR_TIME) && (Command != SQB_CMD_2B_CURR_TIME) && (Command != SQB_CMD_CURR_LIMIT_STAT) && (Command != SQB_CMD_THERM_STAT) &&
          (Command != SQB_CMD_VDIAG_AND_HS_SAFING_STAT) && (Command != SQB_CMD_LS_CONTINUITY_STAT) && (Command != SQB_CMD_SHORT_GND_BAT_STAT) && (Command != SQB_CMD_VFIRE_1B_2B_STAT) && (Command != SQB_CMD_VDIAG) && (Command != SQB_CMD_FEN_RES_STAT) &&
          (Command != SQB_CMD_VFIRE_RTN_STAT) && (Command != SQB_CMD_RES_1A_STAT) && (Command != SQB_CMD_RES_1B_STAT) && (Command != SQB_CMD_RES_2A_STAT) && (Command != SQB_CMD_RES_2B_STAT) && (Command != SQB_CMD_SHORTS_1A_STAT) &&
          (Command != SQB_CMD_SHORTS_1B_STAT) && (Command != SQB_CMD_SHORTS_2A_STAT) && (Command != SQB_CMD_SHORTS_2B_STAT) && (Command != SQB_CMD_SHORT_LOOPS))
      {
        ret_val = SQB_NOT_OK;
      }
      else
      {

        /* Command programing */
        ret_com = Ard_ComSendByte((uint8_t)Command, &SquibCmdResp); /* send programming command "Command" */
        SQUIB_SPI_ERROR(ret_com, ret_val);                          /* if SPI communication error? */
        *SpiResponse = SquibCmdResp;                                /* response to SPI command */
        /* If FEN UNLOCK command is sent to Squib driver - counter delay time data followed */
        if ((Command == SQB_UNLOCK_FEN1_COUNTER_REG) || (Command == SQB_UNLOCK_FEN2_COUNTER_REG))
        {
          switch (Command)
          {
          case SQB_UNLOCK_FEN1_COUNTER_REG:
            ret_com = Ard_ComSendByte(Data, &SquibCmdResp); /* send the programming information "SQUIB_FEN1_COUNTER" to set the required counter delay time (0 - 255 ms) */
            break;
          case SQB_UNLOCK_FEN2_COUNTER_REG:
            ret_com = Ard_ComSendByte(Data, &SquibCmdResp); /* send the programming information "SQUIB_FEN2_COUNTER" to set the required counter delay time (0 - 255 ms) */
            break;
          default:
            break;
          }
          SQUIB_SPI_ERROR(ret_com, ret_val); /* if SPI communication error? */
          /* NOP command - Command 0x00 */
          ret_com = Ard_ComSendByte(SQB_CMD_NOP, &SquibCmdResp); /* read last response from the SQUIB driver in this Init function */
          SQUIB_SPI_ERROR(ret_com, ret_val);                     /* if SPI communication error? */
          *SpiResponse = SquibCmdResp;                           /* response to SPI command */
        }
      }
    
  }
  if (Delay == true)
    delay(SQB_MEASUREMENT_DELAY); /* measurement waiting */
  return (ret_val);
}

Squib_ReturnType Squib_SingleCmd(void)
{
  Squib_ReturnType ret_val;
  Ard_ComReturnType ret_com;

  ret_val = SQB_OK;
  ret_com = ARD_COM_NOT_OK;

  /* SPI integrity check - Command 0x96 */
  ret_com = Ard_ComSendByte(SQB_CMD_SPI_CHECK, &SquibCmdResp);
  if (ret_com != ARD_COM_OK)
    ret_val = SQB_NOT_OK; /* if SPI communication error? */

  return (ret_val);
}
