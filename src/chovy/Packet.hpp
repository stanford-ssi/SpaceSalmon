#pragma once

typedef struct __attribute__((__packed__)) rf_down_t
{
    unsigned pad : 4;

    unsigned time : 18;
    
    unsigned batt_srad_A : 8;
    unsigned batt_srad_B : 8;
    unsigned batt_cots : 8;

    unsigned state : 3; 
    unsigned armed : 1;

    unsigned filter_alt : 15;
    unsigned filter_vel : 11;

    unsigned gps_lock : 1;
    unsigned lat : 18;
    unsigned lon : 18;
    unsigned gps_alt: 15;

    unsigned pyroA_continuity : 1;
    unsigned pyroB_continuity : 1;
    unsigned pyroC_continuity : 1;
    unsigned pyroD_continuity : 1;

    unsigned pyroA_fired: 1;
    unsigned pyroB_fired: 1;
    unsigned pyroC_fired: 1;
    unsigned pyroD_fired: 1;
} rf_down_t;

typedef struct __attribute__((__packed__)) rf_up_t
{ //should be 15 bytes
    unsigned arm_payload : 1;
    unsigned charge1 : 1;
    unsigned charge2 : 1;
    unsigned charge3 : 1;
    unsigned charge4 : 1;
    unsigned charge1_parity : 1;
    unsigned charge2_parity : 1;
    unsigned charge3_parity : 1;
    unsigned charge4_parity : 1;
} rf_up_t;