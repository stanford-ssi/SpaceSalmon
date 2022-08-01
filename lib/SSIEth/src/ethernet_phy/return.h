#pragma once
#include <stdint.h>

typedef uint8_t return_t; //0 is success, > is fail

enum RET : return_t { OK = 0, FAIL = 1 };