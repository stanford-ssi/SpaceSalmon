#pragma once
#include <stdint.h>

typedef uint8_t result_t; //0 is success, > is fail

enum RET : result_t { OK = 0, FAIL = 1 };