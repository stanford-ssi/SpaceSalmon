/*******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2006-2009 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
********************************************************************************
*
* $File Name:       MC33797.h$
* @file             MC33797.h
*
* $Date:            Mar 15, 2014
* @date             Mar 15, 2014
*
* $Version:         1.0.2.0$ - version assigned by ClearCase (version control system)
* @version          0.0.0.1 - release version
*
* Release Version:  0.0.1
*
* Design Reference: Airbag_Reference_Design_Spec.doc Rev 0.9
*
* Description:      SQUIB Driver header file
* @brief            SQUIB Driver header file
*
* @author B17364
*
*******************************************************************************/
#ifndef MC33797_H_
#define MC33797_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

typedef enum {
  ARD_COM_OK = 0x00U,
  ARD_COM_NOT_OK = 0xFFU
} Ard_ComReturnType;

#define SQUIB_GUI_COMMUNICATION false
#define STD_ON true
#define SQUIB_DEV_ERROR_DETECTION false
#define SQUIB_IC_CONNECTION false
#define SQUIB_CROSS_COUPLED true


#define SQUIB_E_INVALID_POINTER     0x01U
#define SQUIB_E_INVALID_CHANNEL     0x02U
#define SQUIB_E_INVALID_MEM         0x03U
#define SQUIB_E_INVALID_PARAM       0x04U
#define SQUIB_E_UNINIT              0x05U
#define SQUIB_E_TRANSITION          0x06U
#define SQUIB_E_TIMEOUT             0x07U

#define SQUIB_SID_INIT              0x00U
#define SQUIB_SID_SET_MODE          0x01U

/***************************************************************
*
*           Configuration structures and types
*
****************************************************************/

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


/*********** Masking definition ***********/

#define SQB_MASK_01_BITS 0x03U /* mask bits 0 and 1 */
#define SQB_MASK_23_BITS 0x0CU /* mask bits 2 and 3 */
#define SQB_MASK_45_BITS 0x30U /* mask bits 4 and 5 */
#define SQB_MASK_67_BITS 0xC0U /* mask bits 6 and 7 */

/*********************** DRIVER STATUS *************************/

/*! @brief SPI channel number (not physical SPI channel) */
typedef uint8_t Squib_SpiChannelType;

/*! @brief Return value of the Squib driver API */
typedef enum {
  SQB_OK = 0x00U,
  SQB_NOT_OK = 0xFFU
} Squib_ReturnType;

/*! @brief State which the Squib driver shell be introduced */
typedef enum {
  SQB_UNINIT = 0xFFU,
  SQB_RUNNING = 0x01U
} Squib_DriverModeType;

/*! @brief Squib Diagnostic Delay Time */
#define SQB_MEASUREMENT_DELAY      1 //    3080 /* ~1ms, fBUS = 40MHz */
#define SQB_MEASUREMENT_DELAY_LONG 2 //    6160 /* ~2ms, fBUS = 40MHz */

/******************** PROGRAMMING AND DIAGNOSTICS COMMANDS *********************/

/*! @brief Programming and diagnostics commands */
typedef enum {
  SQB_CMD_NOP = 0x00U, /* NOP */
  SQB_1A_CURR_REG_RESET = 0x31U, /* Current measurement register reset command for Squib�1A */
  SQB_1B_CURR_REG_RESET = 0x32U, /* Current measurement register reset command for Squib�1B */
  SQB_2A_CURR_REG_RESET = 0x34U, /* Current measurement register reset command for Squib�2A */
  SQB_2B_CURR_REG_RESET = 0x38U, /* Current measurement register reset command for Squib�2B */
  SQB_UNLOCK_FEN1_COUNTER_REG = 0x80U, /* Unlock for FEN1 counter registers programming */
  SQB_UNLOCK_FEN2_COUNTER_REG = 0x81U, /* Unlock for FEN2 counter registers programming */
  SQB_UNLOCK_TO_TEST_HIGH_SQUIBS = 0x82U, /* Unlock to test High-Squib drivers 1A, 1B, 2A, 2B */
  SQB_1A_HS_TRANSISTOR_TEST = 0x11U, /* High-Side driver transistor 1A test command */
  SQB_1B_HS_TRANSISTOR_TEST = 0x12U, /* High-Side driver transistor 1B test command */
  SQB_2A_HS_TRANSISTOR_TEST = 0x14U, /* High-Side driver transistor 2A test command */
  SQB_2B_HS_TRANSISTOR_TEST = 0x18U, /* High-Side driver transistor 2B test command */ 
  SQB_UNLOCK_TO_TEST_LOW_SQUIBS = 0x83U, /* Unlock to test Low-Squib drivers 1A, 1B, 2A, and 2B */
  SQB_1A_LS_TRANSISTOR_TEST = 0x21U, /* Low-Side driver transistor 1A test command */
  SQB_1B_LS_TRANSISTOR_TEST = 0x22U, /* Low-Side driver transistor 1B test command */
  SQB_2A_LS_TRANSISTOR_TEST = 0x24U, /* Low-Side driver transistor 2A test command */
  SQB_2B_LS_TRANSISTOR_TEST = 0x28U, /* Low-Side driver transistor 2B test command */
  SQB_RFU_NVM_LOW = 0x90U, /* Reserved for freescale read NVM Low */
  SQB_RFU_NVM_HIGH = 0x91U, /* Reserved for freescale read NVM High */
  SQB_RFU_NVM_ENAB = 0x92U, /* Reserved for freescale NVM enable */
  SQB_RFU_TEST_MODE = 0x93U, /* Reserved for freescale test mode enable */
  SQB_CMD_SPI_CHECK = 0x96U, /* SPI integrity check */
  SQB_CMD_SHORT_STAT = 0xC1U, /* Squib Short-to-Ground/Short-to-Battery diagnostics */
  SQB_CMD_1A_CURR_TIME = 0x70U, /* Squib�1A current measurement time */
  SQB_CMD_1B_CURR_TIME = 0x71U, /* Squib�1B current measurement time */
  SQB_CMD_2A_CURR_TIME = 0x72U, /* Squib�2A current measurement time */
  SQB_CMD_2B_CURR_TIME = 0x73U, /* Squib�2B current measurement time */
  SQB_CMD_CURR_LIMIT_STAT = 0x79U, /* Squib�1A, 1B, 2A, 2B current status */
  SQB_CMD_THERM_STAT = 0x7FU, /* Thermal shutdown status */
  SQB_CMD_VDIAG_AND_HS_SAFING_STAT = 0xC0U, /* VDIAG and High-Side safing sensor diagnostics */
  SQB_CMD_LS_CONTINUITY_STAT = 0xC2U, /* Low-Side driver continuity status */
  SQB_CMD_SHORT_GND_BAT_STAT = 0xC3U, /* Squib Harness Short-to-Ground/Short-to-Battery with Squib open (no squib present) */
  SQB_CMD_VFIRE_1B_2B_STAT = 0xC5U, /* VFIRE_1B and VFIRE_2B voltage */
  SQB_CMD_VDIAG = 0xC6U, /* VDIAG_1 and VDIAG_2 diagnostics */
  SQB_CMD_FEN_RES_STAT = 0xC8U, /* FEN Status, R_LIMIT_X, R_DIAG status, IC type */
  SQB_CMD_VFIRE_RTN_STAT = 0xC9U, /* VFIRE_RTN Status (open ground) */
  SQB_CMD_RES_1A_STAT = 0xD0U, /* Squib�1A resistance */
  SQB_CMD_RES_1B_STAT = 0xD1U, /* Squib�1B resistance */
  SQB_CMD_RES_2A_STAT = 0xD2U, /* Squib�2A resistance */
  SQB_CMD_RES_2B_STAT = 0xD3U, /* Squib�2B resistance */
  SQB_CMD_SHORTS_1A_STAT = 0xE0U, /* Shorts between Squib loops, Squib�1A */
  SQB_CMD_SHORTS_1B_STAT = 0xE1U, /* Shorts between Squib loops, Squib�1B */
  SQB_CMD_SHORTS_2A_STAT = 0xE2U, /* Shorts between Squib loops, Squib�2A */
  SQB_CMD_SHORTS_2B_STAT = 0xE3U, /* Shorts between Squib loops, Squib�2B */
  SQB_CMD_SHORT_LOOPS = 0xE8U /* Shorts between Squib loops, for additional ICs */
} Squib_ProgCmdType;

/************************** FIRING *****************************/

/*! @brief Arming commands - for arming squibs pairs or for separate Low/High sides */
#define CMD_ARM_1_LOW_HIGH_SIDE     0xA0U /* ARM squib drivers 1A nad 1B */
#define CMD_ARM_2_LOW_HIGH_SIDE     0xA1U /* ARM squib drivers 2A nad 2B */
#define CMD_ARM_1_AND_2_SQUIB_PAIRS 0xA2U /* ARM squib drivers 1A, 1B, 2A and 2B */

/*! @brief Firing commands - for squibs pairs or for separate Low/High side */
typedef enum {
  CMD_FIRE_NO_SQUIBS = 0x50U, /* no firing Squibs required */
  CMD_FIRE_1A = 0xF1U, /* firing Squib pair 1A required */
  CMD_FIRE_1B = 0xF2U, /* firing Squib pair 1B required */
  CMD_FIRE_1A1B = 0xF3U, /* firing Squib pairs 1A and 1B required */
  CMD_FIRE_2A = 0xF4U, /* firing Squib pair 2A required */
  CMD_FIRE_1A2A = 0xF5U, /* firing Squib pairs 1A and 2A required */
  CMD_FIRE_1B2A = 0xF6U, /* firing Squib pairs 2A and 1B required */
  CMD_FIRE_1A2A1B = 0xF7U, /* firing Squib pairs 1A, 2A and 1B required */
  CMD_FIRE_2B = 0xF8U, /* firing Squib pair 2B required */
  CMD_FIRE_1A2B = 0xF9U, /* firing Squib pairs 1A and 2B required */
  CMD_FIRE_1B2B = 0xFAU, /* firing Squib pairs 1B and 2B required */
  CMD_FIRE_1A1B2B = 0xFBU, /* firing Squib pairs 1A, 1B and 2B required */
  CMD_FIRE_2A2B = 0xFCU, /* firing Squib pairs 2A and 2B required */
  CMD_FIRE_1A2A2B = 0xFDU, /* firing Squib pairs 1A, 2A and 2B required */
  CMD_FIRE_2A1B2B = 0xFEU, /* firing Squib pairs 2A, 1B and 2B required */
  CMD_FIRE_1A2A1B2B = 0xFFU, /* firing Squib pairs 1A, 2A, 1B and 2B required */ 
  CMD_FIRE_1ALS = 0x51U, /* switch Squib "1A" Low Side */
  CMD_FIRE_1AHS = 0x52U, /* switch Squib "1A" High Side */
  CMD_FIRE_1ALSHS = 0x53U, /* switch Squib "1A" Low and High Sides */
  CMD_FIRE_1BLS = 0x54U, /* switch Squib "1B" Low Side */
  CMD_FIRE_1ALSBLS = 0x55U, /* switch Squib "1A" Low Side and Squib "1B" Low Side */
  CMD_FIRE_1AHSBLS = 0x56U, /* switch Squib "1A" High Side and Squib "1B" Low Side */
  CMD_FIRE_1AHSLSBLS = 0x57U, /* switch Squib "1A" Low&High Sides and Squib "1B" Low Side */
  CMD_FIRE_1BHS = 0x58U, /* switch Squib "1B" High Side */
  CMD_FIRE_1ALSBHS = 0x59U, /* switch Squib "1A" Low Side and Squib "1B" High Side */
  CMD_FIRE_1AHSBHS = 0x5AU, /* switch Squib "1A" High Side and Squib "1B" High Side */
  CMD_FIRE_1AHSLSBHS = 0x5BU, /* switch Squib "1A" High&Low Sides and Squib "1B" High Side */
  CMD_FIRE_1BHSLS = 0x5CU, /* switch Squib "1B" High&Low Sides */
  CMD_FIRE_1ALSBHSLS = 0x5DU, /* switch Squib "1A" Low Side and Squib "1B" High&Low Sides */
  CMD_FIRE_1AHSBHSLS = 0x5EU, /* switch Squib "1A" High Side and Squib "1B" High&Low Sides */
  CMD_FIRE_1AHSLSBHSLS = 0x5FU, /* switch Squib "1A" High&Low Side and Squib "1B" High&Low Sides */
  CMD_FIRE_2ALS = 0xD1U, /* switch Squib "2A" Low Side */
  CMD_FIRE_2AHS = 0xD2U, /* switch Squib "2A" High Side */
  CMD_FIRE_2ALSHS = 0xD3U, /* switch Squib "2A" Low and High Sides */
  CMD_FIRE_2BLS = 0xD4U, /* switch Squib "2B" Low Side */
  CMD_FIRE_2ALSBLS = 0xD5U, /* switch Squib "2A" Low Side and Squib "2B" Low Side */
  CMD_FIRE_2AHSBLS = 0xD6U, /* switch Squib "2A" High Side and Squib "2B" Low Side */
  CMD_FIRE_2AHSLSBLS = 0xD7U, /* switch Squib "2A" Low&High Sides and Squib "2B" Low Side */
  CMD_FIRE_2BHS = 0xD8U, /* switch Squib "2B" High Side */
  CMD_FIRE_2ALSBHS = 0xD9U, /* switch Squib "2A" Low Side and Squib "2B" High Side */
  CMD_FIRE_2AHSBHS = 0xDAU, /* switch Squib "2A" High Side and Squib "2B" High Side */
  CMD_FIRE_2AHSLSBHS = 0xDBU, /* switch Squib "2A" High&Low Sides and Squib "2B" High Side */
  CMD_FIRE_2BHSLS = 0xDCU, /* switch Squib "2B" High&Low Sides */
  CMD_FIRE_2ALSBHSLS = 0xDDU, /* switch Squib "2A" Low Side and Squib "2B" High&Low Sides */
  CMD_FIRE_2AHSBHSLS = 0xDEU, /* switch Squib "2A" High Side and Squib "2B" High&Low Sides */
  CMD_FIRE_2AHSLSBHSLS = 0xDFU /* switch Squib "2A" High&Low Side and Squib "2B" High&Low Sides */
} Squib_FireType;

/************************ DIAGNOSTIC ***************************/

/*! @brief Common responses */
#define SQB_RESISTANCE_OPEN        0xFFU /* reports open squib */
#define SQB_RESISTANCE_SHORT       0x00U /* reports short squib */
#define SQB_1A_HS_ON               0x0EU /* programming response - High-Side driver transistor 1A test response */
#define SQB_1B_HS_ON               0x0DU /* programming response - High-Side driver transistor 1B test response */
#define SQB_2A_HS_ON               0x0BU /* programming response - High-Side driver transistor 2A test response */
#define SQB_2B_HS_ON               0x07U /* programming response - High-Side driver transistor 2B test response */ 
#define SQB_1A_LS_ON               0x0EU /* programming response - Low-Side driver transistor 1A test response */
#define SQB_1B_LS_ON               0x0DU /* programming response - Low-Side driver transistor 1B test response */
#define SQB_2A_LS_ON               0x0BU /* programming response - Low-Side driver transistor 2A test response */
#define SQB_2B_LS_ON               0x07U /* programming response - Low-Side driver transistor 2B test response */
#define SQB_1A_LS_ON_CROSS         0xFDU /* programming response - Low-Side driver transistor 1A test response for cross-coupled SQUIBs */
#define SQB_1B_LS_ON_CROSS         0xF7U /* programming response - Low-Side driver transistor 1B test response for cross-coupled SQUIBs */
#define SQB_2A_LS_ON_CROSS         0xDFU /* programming response - Low-Side driver transistor 2A test response for cross-coupled SQUIBs */
#define SQB_2B_LS_ON_CROSS         0x7FU /* programming response - Low-Side driver transistor 2B test response for cross-coupled SQUIBs */

/*! @brief FET driver current limit measurement status */
typedef enum {
  SQB_1A_CURRENT_EXCEED = 0x01U,
  SQB_1B_CURRENT_EXCEED = 0x02U,
  SQB_2A_CURRENT_EXCEED = 0x03U,
  SQB_2B_CURRENT_EXCEED = 0x04U,
  SQB_CURRENT_0K = 0x00U
} Squib_StatCurrentType;

/*! @brief Low-Side Squib driver thermal shutdown */
typedef enum {
  SQB_1A_LS_THER_SHUT = 0x02U,
  SQB_1B_LS_THER_SHUT = 0x08U,
  SQB_2A_LS_THER_SHUT = 0x20U,
  SQB_2B_LS_THER_SHUT = 0x80U,
  SQB_LS_THERM_0K = 0x00U
} Squib_StatLowSideThermalShutType;

/*! @brief High-Side Squib driver thermal shutdown */
typedef enum {
  SQB_1A_HS_THER_SHUT = 0x01U,
  SQB_1B_HS_THER_SHUT = 0x04U,
  SQB_2A_HS_THER_SHUT = 0x10U,
  SQB_2B_HS_THER_SHUT = 0x40U,
  SQB_HS_THERM_0K = 0x00U
} Squib_StatHighSideThermalShutType;

/*! @brief High-Side Safing sensor diagnostics - monitors the VFIRE_XX pin connection to the VDIAG_1 pin */
typedef enum {
  SQB_VFIREXX_SHORT_TO_VDIAG1 = 0x00U,
  SQB_VFIREXX_NO_CONECT_TO_VDIAG1 = 0x0CU,
  SQB_NORMAL_SAFING1 = 0x08U
} Squib_Stat1HSSafingSensType;

/*! @brief Firing supply voltage (VDIAG_1) diagnostics - monitors the voltage on the VDIAG_1 pin */
typedef enum {
  SQB_1_VDIAG_ABOVE = 0x03U,
  SQB_1_VDIAG_BETWEEN = 0x01U,
  SQB_1_VDIAG_BELOW = 0x00U
} Squib_Stat1VdiagResultType;

/*! @brief High-Side Safing sensor diagnostics - monitors the VFIRE_XX pin connection to the VDIAG_2 pin */
typedef enum {
  SQB_VFIREXX_SHORT_TO_VDIAG2 = 0x00U,
  SQB_VFIREXX_NO_CONECT_TO_VDIAG2 = 0xC0U,
  SQB_NORMAL_SAFING2 = 0x80U
} Squib_Stat2HSSafingSensType;

/*! @brief Firing supply voltage (VDIAG_2) diagnostics - monitors the voltage on the VDIAG_2 pin */
typedef enum {
  SQB_2_VDIAG_ABOVE = 0x30U,
  SQB_2_VDIAG_BETWEEN = 0x10U,
  SQB_2_VDIAG_BELOW = 0x00U
} Squib_Stat2VdiagResultType;

/*! @brief Squib short-to-battery diagnostics - monitors the voltage on the SENSE_XX pins */
typedef enum {
  SQB_1A_SH_BATT_OK = 0x01U,
  SQB_1B_SH_BATT_OK = 0x04U,
  SQB_2A_SH_BATT_OK = 0x10U,
  SQB_2B_SH_BATT_OK = 0x40U,
  SQB_SH_TO_BATT_FAULT = 0x00U
} Squib_ShBattType;

/*! @brief Squib short-to-ground diagnostics - monitors the voltage on the SENSE_XX pins */
typedef enum {
  SQB_1A_SH_GND_OK = 0x02U,
  SQB_1B_SH_GND_OK = 0x08U,
  SQB_2A_SH_GND_OK = 0x20U,
  SQB_2B_SH_GND_OK = 0x80U,
  SQB_SH_TO_GND_FAULT = 0x00U
} Squib_ShGndType;

/*! @brief Continuity test for the Low-Side driver SQB_LO_XX connection - LS driver continuity is checked during the continuity test diagnostics */
typedef enum {
  SQB_1A_LS_OK = 0x01U,
  SQB_1B_LS_OK = 0x02U,
  SQB_2A_LS_OK = 0x04U,
  SQB_2B_LS_OK = 0x08U,
  SQB_LS_OPEN_FAULT = 0x00U
} Squib_StatLowSideContType;

/*! @brief Squib harness short-to-battery diagnostics with an open Squib */
typedef enum {
  SQB_1A_OPN_SH_BATT_OK = 0x01U,
  SQB_1B_OPN_SH_BATT_OK = 0x04U,
  SQB_2A_OPN_SH_BATT_OK = 0x10U,
  SQB_2B_OPN_SH_BATT_OK = 0x40U,
  SQB_OPN_SH_TO_BATT_FAULT = 0x00U
} Squib_OpnShBattType;

/*! Squib harness short-to-ground diagnostics with an open Squib */
typedef enum {
  SQB_1A_OPN_SH_GND_OK = 0x02U,
  SQB_1B_OPN_SH_GND_OK = 0x08U,
  SQB_2A_OPN_SH_GND_OK = 0x20U,
  SQB_2B_OPN_SH_GND_OK = 0x80U,
  SQB_OPN_SH_TO_GND_FAULT = 0x00U
} Squib_OpnShGndType;

/*! @brief Shorts between Squib 1A loops */
typedef enum {
  SQB_1A_SHORT_OK = 0x01U,
  SQB_1A1B_SHORT_FAULT = 0x03U,
  SQB_1A2A_SHORT_FAULT = 0x05U,
  SQB_1A2B_SHORT_FAULT = 0x09U,
  SQB_1A1B_1A2A_SHORT_FAULT = 0x07U,
  SQB_1A1B_1A2B_SHORT_FAULT = 0x0BU,
  SQB_1A2A_1A2B_SHORT_FAULT = 0x0DU,  
  SQB_1A1B_1A2A_1A2B_SHORT_FAULT = 0x0FU,
  SQB_1A_MORE_SHORTS_FAULT = 0x00U
} Squib_Stat1ALoopsShortsType;

/*! @brief Shorts between Squib 1B loops */
typedef enum {
  SQB_1B_SHORT_OK = 0x02U,
  SQB_1B1A_SHORT_FAULT = 0x03U,
  SQB_1B2A_SHORT_FAULT = 0x06U,
  SQB_1B2B_SHORT_FAULT = 0x0AU,
  SQB_1B1A_1B2A_SHORT_FAULT = 0x07U,
  SQB_1B1A_1B2B_SHORT_FAULT = 0x0BU,
  SQB_1B2A_1B2B_SHORT_FAULT = 0x0EU,  
  SQB_1B1A_1B2A_1B2B_SHORT_FAULT = 0x0FU,
  SQB_1B_MORE_SHORTS_FAULT = 0x00U
} Squib_Stat1BLoopsShortsType;

/*! @brief Shorts between Squib 2A loops */
typedef enum {
  SQB_2A_SHORT_OK = 0x04U,
  SQB_2A1A_SHORT_FAULT = 0x05U,
  SQB_2A1B_SHORT_FAULT = 0x06U,
  SQB_2A2B_SHORT_FAULT = 0x0CU,
  SQB_2A1A_2A1B_SHORT_FAULT = 0x07U,
  SQB_2A1A_2A2B_SHORT_FAULT = 0x0DU,
  SQB_2A1B_2A2B_SHORT_FAULT = 0x0EU,  
  SQB_2A1A_2A1B_2A2B_SHORT_FAULT = 0x0FU,
  SQB_2A_MORE_SHORTS_FAULT = 0x00U
} Squib_Stat2ALoopsShortsType;

/*! @brief Shorts between Squib 2B loops */
typedef enum {
  SQB_2B_SHORT_OK = 0x08U,
  SQB_2B1A_SHORT_FAULT = 0x09U,
  SQB_2B1B_SHORT_FAULT = 0x0AU,
  SQB_2B2A_SHORT_FAULT = 0x0CU,
  SQB_2B1A_2B1B_SHORT_FAULT = 0x0BU,
  SQB_2B1A_2B2A_SHORT_FAULT = 0x0DU,
  SQB_2B1B_2B2A_SHORT_FAULT = 0x0EU,  
  SQB_2B1A_2B1B_2B2A_SHORT_FAULT = 0x0FU,
  SQB_2B_MORE_SHORTS_FAULT = 0x00U
} Squib_Stat2BLoopsShortsType;

/*! @brief VFIRE test finished */
typedef enum {
  SQB_VFIREB_TESTED = 0x10U,
  SQB_VFIREB_UNTESTED = 0x00U
} Squib_StatVfireBTestedType;

/*! @brief Firing supply voltage diagnostics - monitors the voltage on the VFIRE_XX pin */
typedef enum {
  SQB_VFIRE_ABOVE = 0x03U,
  SQB_VFIRE_BETWEEN = 0x01U,
  SQB_VFIRE_BELOW = 0x00U
} Squib_StatVfireType;

/*! @brief Firing supply voltage diagnostics - the VDIAG_X (V1, V2, V3, V4) voltages on the VDIAG1 pin */
typedef enum {
  SQB_VDIAG1_V1_ABOVE = 0x01U,
  SQB_VDIAG1_V2_ABOVE = 0x02U,
  SQB_VDIAG1_V3_ABOVE = 0x04U,
  SQB_VDIAG1_V4_ABOVE = 0x08U,
  SQB_VDIAG1_VX_BELOW = 0x00U
} Squib_StatV1diagType;

/*! @brief Firing supply voltage diagnostics - the VDIAG_X (V1, V2, V3, V4) voltages on the VDIAG2 pin */
typedef enum {
  SQB_VDIAG2_V1_ABOVE = 0x10U,
  SQB_VDIAG2_V2_ABOVE = 0x20U,
  SQB_VDIAG2_V3_ABOVE = 0x40U,
  SQB_VDIAG2_V4_ABOVE = 0x80U,
  SQB_VDIAG2_VX_BELOW = 0x00U
} Squib_StatV2diagType;

/*! @brief The FEN1 and FEN2 status bits are a reflection of the FEN_1 and FEN_2 pins */
typedef enum {
  SQB_1_FEN_HIGH = 0x01U,
  SQB_2_FEN_HIGH = 0x02U,
  SQB_FEN_LOW = 0x00U
} Squib_StatFenType;

/*! @brief FEN1 and FEN2 latch status */
typedef enum {
  SQB_1_FEN_LATCH_OK = 0x04U,
  SQB_2_FEN_LATCH_OK = 0x08U,
  SQB_FEN_LATCH_FAULT = 0x00U
} Squib_StatFenLatchType;

/*! @brief R_DIAG resistor diagnostics - reference currents derived by the R_DIAG resistor */
typedef enum {
  SQB_R_DIAG_OK = 0x10U,
  SQB_R_DIAG_FAULT = 0x00U
} Squib_StatRdiagType;

/*! @brief R_LIMIT_X resistor diagnostics - reference currents derived by the R_LIMIT_1 and R_LIMIT_2 resistors */
typedef enum {
  SQB_1_R_LIMIT_OK = 0x20U,
  SQB_2_R_LIMIT_OK = 0x40U,
  SQB_R_LIMIT_FAULT = 0x00U
} Squib_StatRlimitType;

/*! @brief Identifier the squib IC as a four- or two-channel squib driver IC */
typedef enum {
  SQB_TWO_CHANNEL_DEVICE = 0x00U,
  SQB_FOUR_CHANNEL_DEVICE = 0x80U
} Squib_DeviceChannelType;

/*! @brief VFIRE_RTN diagnostics - monitors the resistance on the VFIRE_RTN pin for open pin connections */
typedef enum {
  SQB_1_RTN_OPEN_FAULT = 0x01U,
  SQB_2_RTN_OPEN_FAULT = 0x02U,
  SQB_RTN_SHORT_OK = 0x00U
} Squib_StatVfireRtnType;

/*! @brief Shorts between squib loops (for additional ICs) check all squibs for shorted squib lines */
typedef enum {
  SQB_1A_SHORT_FAULT = 0x01U,
  SQB_1B_SHORT_FAULT = 0x02U,
  SQB_2A_SHORT_FAULT = 0x04U,
  SQB_2B_SHORT_FAULT = 0x08U,
  SQB_LOOPS_NO_SHORT = 0x00U
} Squib_StatLoopsShortsAddICType;

/*! @brief Data structure containing the status of the Squib device */
typedef struct { 
  uint8_t Squib_Stat1ACurrTime; /* SPI command 0x70 - firing current in 1A squib line and records the �ON� time in which the IMEAS is above the threshold for 1A squib */
  uint8_t Squib_Stat1BCurrTime; /* SPI command 0x71 - firing current in 1B squib line and records the �ON� time in which the IMEAS is above the threshold for 1B squib */
  uint8_t Squib_Stat2ACurrTime; /* SPI command 0x72 - firing current in 2A squib line and records the �ON� time in which the IMEAS is above the threshold for 2A squib */
  uint8_t Squib_Stat2BCurrTime; /* SPI command 0x73 - firing current in 2B squib line and records the �ON� time in which the IMEAS is above the threshold for 2B squib */
  Squib_StatCurrentType Squib_Stat1ACurrent; /* SPI command 0x79 - 1A current limit status */
  Squib_StatCurrentType Squib_Stat1BCurrent; /* SPI command 0x79 - 1B current limit status */
  Squib_StatCurrentType Squib_Stat2ACurrent; /* SPI command 0x79 - 2A current limit status */
  Squib_StatCurrentType Squib_Stat2BCurrent; /* SPI command 0x79 - 2B current limit status */
  Squib_StatLowSideThermalShutType Squib_Stat1ALowSideThermalShut; /* SPI command 0x7F - 1A LS thermal shutdown status */
  Squib_StatHighSideThermalShutType Squib_Stat1AHighSideThermalShut; /* SPI command 0x7F - 1A HS thermal shutdown status */
  Squib_StatLowSideThermalShutType Squib_Stat1BLowSideThermalShut; /* SPI command 0x7F - 1B LS thermal shutdown status */
  Squib_StatHighSideThermalShutType Squib_Stat1BHighSideThermalShut; /* SPI command 0x7F - 1B HS thermal shutdown status */
  Squib_StatLowSideThermalShutType Squib_Stat2ALowSideThermalShut; /* SPI command 0x7F - 2A LS thermal shutdown status */
  Squib_StatHighSideThermalShutType Squib_Stat2AHighSideThermalShut; /* SPI command 0x7F - 2A HS thermal shutdown status */
  Squib_StatLowSideThermalShutType Squib_Stat2BLowSideThermalShut; /* SPI command 0x7F - 2B LS thermal shutdown status */
  Squib_StatHighSideThermalShutType Squib_Stat2BHighSideThermalShut; /* SPI command 0x7F - 2B HS thermal shutdown status */
  Squib_Stat1VdiagResultType Squib_Stat1VdiagResult; /* SPI command 0xC0 */
  Squib_Stat1HSSafingSensType Squib_Stat1HSSafingSens; /* SPI command 0xC0 */
  Squib_Stat2VdiagResultType Squib_Stat2VdiagResult; /* SPI command 0xC0 */
  Squib_Stat2HSSafingSensType Squib_Stat2HSSafingSens; /* SPI command 0xC0 */
  Squib_ShBattType Squib_1AShBatt; /* SPI command 0xC1 */
  Squib_ShGndType Squib_1AShGnd; /* SPI command 0xC1 */
  Squib_ShBattType Squib_1BShBatt; /* SPI command 0xC1 */
  Squib_ShGndType Squib_1BShGnd; /* SPI command 0xC1 */
  Squib_ShBattType Squib_2AShBatt; /* SPI command 0xC1 */
  Squib_ShGndType Squib_2AShGnd; /* SPI command 0xC1 */
  Squib_ShBattType Squib_2BShBatt; /* SPI command 0xC1 */
  Squib_ShGndType Squib_2BShGnd; /* SPI command 0xC1 */
  Squib_StatLowSideContType Squib_Stat1ALowSideCont; /* SPI command 0xC2 */
  Squib_StatLowSideContType Squib_Stat1BLowSideCont; /* SPI command 0xC2 */
  Squib_StatLowSideContType Squib_Stat2ALowSideCont; /* SPI command 0xC2 */
  Squib_StatLowSideContType Squib_Stat2BLowSideCont; /* SPI command 0xC2 */      
  Squib_OpnShBattType Squib_1AOpnShBatt; /* SPI command 0xC3 */
  Squib_OpnShGndType Squib_1AOpnShGnd; /* SPI command 0xC3 */
  Squib_OpnShBattType Squib_1BOpnShBatt; /* SPI command 0xC3 */
  Squib_OpnShGndType Squib_1BOpnShGnd; /* SPI command 0xC3 */
  Squib_OpnShBattType Squib_2AOpnShBatt; /* SPI command 0xC3 */
  Squib_OpnShGndType Squib_2AOpnShGnd; /* SPI command 0xC3 */
  Squib_OpnShBattType Squib_2BOpnShBatt; /* SPI command 0xC3 */
  Squib_OpnShGndType Squib_2BOpnShGnd; /* SPI command 0xC3 */ 
  Squib_StatVfireBTestedType Squib_StatVfireBTested; /* SPI command 0xC5 */
  Squib_StatVfireType Squib_StatVfire; /* SPI command 0xC5 */
  Squib_StatV1diagType Squib_StatV1diagV1; /* SPI command 0xC6 */
  Squib_StatV1diagType Squib_StatV1diagV2; /* SPI command 0xC6 */
  Squib_StatV1diagType Squib_StatV1diagV3; /* SPI command 0xC6 */
  Squib_StatV1diagType Squib_StatV1diagV4; /* SPI command 0xC6 */
  Squib_StatV2diagType Squib_StatV2diagV1; /* SPI command 0xC6 */
  Squib_StatV2diagType Squib_StatV2diagV2; /* SPI command 0xC6 */
  Squib_StatV2diagType Squib_StatV2diagV3; /* SPI command 0xC6 */
  Squib_StatV2diagType Squib_StatV2diagV4; /* SPI command 0xC6 */
  Squib_StatFenType Squib_StatFen1; /* SPI command 0xC8 */
  Squib_StatFenType Squib_StatFen2; /* SPI command 0xC8 */
  Squib_StatFenLatchType Squib_StatFen1Latch; /* SPI command 0xC8 */
  Squib_StatFenLatchType Squib_StatFen2Latch; /* SPI command 0xC8 */
  Squib_StatRdiagType Squib_StatRdiag; /* SPI command 0xC8 */
  Squib_StatRlimitType Squib_StatRlimit1; /* SPI command 0xC8 */
  Squib_StatRlimitType Squib_StatRlimit2; /* SPI command 0xC8 */
  Squib_DeviceChannelType Squib_DeviceType; /* SPI command 0xC8 */
  Squib_StatVfireRtnType Squib_StatVfireRtn1; /* SPI command 0xC9 */
  Squib_StatVfireRtnType Squib_StatVfireRtn2; /* SPI command 0xC9 */
  uint8_t Squib_Stat1AResistance; /* SPI command 0xD0 - monitors squib 1A resistance */
  uint8_t Squib_Stat1BResistance; /* SPI command 0xD1 - monitors squib 1B resistance */
  uint8_t Squib_Stat2AResistance; /* SPI command 0xD2 - monitors squib 2A resistance */
  uint8_t Squib_Stat2BResistance; /* SPI command 0xD3 - monitors squib 2B resistance */
  Squib_Stat1ALoopsShortsType Squib_Stat1ALoopsShorts; /* SPI command 0xE0 - monitors conditions that have shorts between 1A squib lines (firing loops) */
  Squib_Stat1BLoopsShortsType Squib_Stat1BLoopsShorts; /* SPI command 0xE1 - monitors conditions that have shorts between 1B squib lines (firing loops) */
  Squib_Stat2ALoopsShortsType Squib_Stat2ALoopsShorts; /* SPI command 0xE2 - monitors conditions that have shorts between 2A squib lines (firing loops) */
  Squib_Stat2BLoopsShortsType Squib_Stat2BLoopsShorts; /* SPI command 0xE3 - monitors conditions that have shorts between 2B squib lines (firing loops) */
  Squib_StatLoopsShortsAddICType Squib_Stat1ALoopsShortsAddIC; /* SPI command 0xE8 */
  Squib_StatLoopsShortsAddICType Squib_Stat1BLoopsShortsAddIC; /* SPI command 0xE8 */
  Squib_StatLoopsShortsAddICType Squib_Stat2ALoopsShortsAddIC; /* SPI command 0xE8 */
  Squib_StatLoopsShortsAddICType Squib_Stat2BLoopsShortsAddIC; /* SPI command 0xE8 */
} Squib_StatusType;

#ifdef __cplusplus
}
#endif

#endif /* MC33797_H_ */

