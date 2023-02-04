#pragma once

typedef enum {
    ENGINE_ABORT,
    ENGINE_IDLE,
    ENGINE_PREPOX,
    ENGINE_OXPREPPED,
    ENGINE_PREPFUEL,
    ENGINE_PREPPED,
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