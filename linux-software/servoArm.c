#include "servoArm.h"


Status_t servoArmInit(servoArm_t * this, char * device, char * config) {

  SSC8SpoolerInit(&(this->axii));                      // initialize system
  SSC8SpoolerOpenDevice(&(this->axii), device);        // open serial port
  SSC8SpoolerStartTransmitter(&(this->axii));          // start transmitter
  
  return transInitFile(&(this->buffer), 8, config);

}


Status_t servoArmSync(servoArm_t * this) {

  unsigned int i;
  
  // do calculations
  transCalc(&(this->buffer));
  
  // set servos  
  for (i = 0; i < 8; i++) {
    ServoSetTarget( &(this->axii.servos.servo[i]), this->buffer.out[i] );
  }

}


/*

int main(void) {

   servoArm_t   OUT;
   unsigned int i;
   
   printf("Initializing\n");
   if (servoArmInit(&OUT, "/dev/ttyUSB0", "arm7.tf") != OK) {
     printf("port open error\n");
     return 0;
   }

   transZeroInput(&(OUT.buffer)) ;  

   printf("Running update loop\n");     
   while(1) {
     transCalc(&(OUT.buffer));
     servoArmSync(&OUT);   
     
     transDump(&(OUT.buffer));
     
   }
    
  return 0;
}


*/
