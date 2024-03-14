#ifndef __servoArray
#define __servoArray


#include "genServo.h"

typedef struct servoArray_s {
  servo_t  *   servo;
  unsigned int servoCount;
} servoArray_t;


#endif
