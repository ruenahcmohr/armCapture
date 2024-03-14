#include "genServo.h"
#include "returnValues.h"
#include "Stuff.h"

Status_t ServoInit(servo_t *this){
  this->target   = 0;
  this->update   = 1;           // do update on startup
  this->zero     = 128;         // hobby servo center
  this->scale    = 127.0/45.0;  // hobby servo guess
  this->limL     = -65535;      // generic safe?
  this->limH     = 65535;       // generic safe?
  
  return OK;
}

Status_t ServoFini(servo_t *this){
  return OK;
}

Status_t ServoSetTarget(servo_t *this, float target){

  target = Limit(target,this->limL,this->limH);

  if (this->target != target) {
   this->target = target;
   this->update++;
  }
  return OK;
}

Status_t servoGetTarget(servo_t *this, float * target){
  *target = this->target;
  return OK;
}

Status_t servoSetLimits(servo_t *this, float highLim, float lowLim){
  this->limL = lowLim;
  this->limH = highLim;
}


Status_t servoDump(servo_t *this){
  printf("Target: %f\n", this->target);
  printf("Update: %d\n", this->update);
  printf("Zero  : %f\n", this->zero);
  printf("Scale : %f\n", this->scale);
  printf("Limits: %f to %f\n", this->limL, this->limH);
  return OK;
}
