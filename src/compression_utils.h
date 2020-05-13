#pragma once
#include "Arduino.h"

double_t map_float(double_t x, double_t in_min, double_t in_max, double_t out_min, double_t out_max);

uint32_t compressFloat(float value, float min, float max, uint8_t bits);

uint32_t trimBits(uint32_t value, uint8_t bits);

uint32_t fitToBits(uint32_t value, uint8_t bits, uint32_t min, uint32_t max);

double_t unpackBits(uint32_t value, uint8_t bits, double_t min, double_t max);

double_t expandFloat(uint32_t value, double_t min, double_t max, uint8_t bits);