#pragma once

// LEDS
#define ACT_LED 1
#define ADC_LED 2
#define PWR_LED 3
#define LOG_LED 4


// Squibs
#define NUM_EM_CHANNELS 4
#define UPDATE_SQUIBS 0b01

typedef enum{
    FIRED = 1,
    UNFIRED = 0,
} ematch_fire_state_t; // state indicators for EMs to keep the nomenclature consistent

typedef enum{
    ARMED = 1,
    UNARMED = 0
} ematch_arm_state_t;

// Valves
#define UPDATE_VALVES 0b01
#define NUM_SOLENOIDS 12

typedef enum{
    OPEN = 1,
    CLOSED = 0,
} solenoid_state_t; // state indicators for SVs to keep the nomenclature consistent

typedef enum{
    NORMALLY_CLOSED = OPEN,
    NORMALLY_OPEN = CLOSED,
} solenoid_normal_t; // defines when the solenoid ought to be powered

typedef enum {
    REAL_SMALL = 10,
    P_SMALL = 50,
    SMALL = 130,
    MEDIUM = 220, // Edelbrook
    LARGE = 180, // Pro BigShot
    ABORT_VALVE = 255 // don't pwm, just digital write
} solenoid_pwm_t; // making solenoid pwm values more readable, values taken from old quail

// sensing
#define ERROR_THRESHOLD 10
#define READ_TIMOUT 100
#define PSI_TO_PA 6894.7572931783
#define UNCONFIGURED UINT8_MAX
#define READ_TIMEOUT 100
#define WINDOW_SIZE 10

typedef enum {
    NEED_CONFIG = 0b01,
    DATA_READY = 0b10
} sensor_tasks_states_t;


// sequence
#define UPDATE_SEQS 0b01
#define MAWP 700 * PSI_TO_PA
#define FSM_PTM 100
#define FSM_FREQ 10
#define FIRE_DELAY 4000

typedef enum {
    ENGINE_ABORT,
    ENGINE_IDLE,
    ENGINE_FILL,
    ENGINE_FULL,
    ENGINE_FIRE,
    MAIN_ACTUATION
} EngineState;

typedef enum {
    TANK_IDLE_EMPTY,
    TANK_IDLE_PRESS,
    TANK_EMPTY,
    TANK_DRAIN,
    TANK_FILL,
    TANK_FULL,
    TANK_BLEED,
    TANK_READY
} TankState;


//slate 
#define NO_QUAIL_ID ""

// errors
 enum ErrorType {
   none = 0,
   corrupt_file = 1,
   failed_to_mount_disk = 2,
   failed_to_open_file = 3
 };

 // TX RX
 #define ETHERNET_TXRX
 // #define RADIO_TXRX
 #define COMM_RESET 1800000

 // networking
 #define MY_SLATE_PORT 1000
 #define CLIENT_SLATE_PORT 8000
 #define MY_CMD_PORT 1002
 #define META_PCKT_LEN 16384
 #define DATA_PCKT_LEN 8192
 #define NETWORKING_DELAY 2000
 #define MAX_CMD_LENGTH 512 // maximum length of a command, in bytes (based on radio command max size)

 // shortcuts
 #define STRINGIFY(x) #x
 #define PRINT(x) sys.tasks.txtask.writeUSB(STRINGIFY(x))