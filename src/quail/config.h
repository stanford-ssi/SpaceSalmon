#pragma once

// Squibs
#define NUM_EM_CHANNELS 8
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
#define NUM_SOLENOIDS 8

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
    ABORT = 255 // don't pwm, just digital write
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

enum SEQUENCE_STATE {
    RUNNING = 1,
    SUSPEND = 0,
    DELETE = -1,
} typedef SEQUENCE_STATE;

//slate 
#define NO_QUAIL_ID ""