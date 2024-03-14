
#include "SSC8Spooler.h" 


Status_t SSC8SpoolerStartTransmitter(SSC8Spooler_t *this) {
    
   if( (pthread_create( &(this->xmitter), NULL, &SSC8SpoolerTransmittLoop, this)) ){
      printf("Failed to start motion control thread: \n");
      return ExecFail;
   }
   return OK;
}


void *SSC8SpoolerTransmittLoop(void *that) {
   char   packet[4];
   float  position;
   unsigned int positionBuff;
   unsigned int i, u;
   SSC8Spooler_t *this;
   this = that;
   
   packet[0] = 0xFF;
   i = 0;
   u = 0;
   
   printf("Spooler started for port: %s with FD: %d\n", this->serial.filename, this->serial.fd);

   while(1) {  
     if (this->servos.servo[i].update != 0) { 
       position = (this->servos.servo[i].target * this->servos.servo[i].scale) + this->servos.servo[i].zero;
     //  printf("Position: %f, Scale: %f, Zero: %f  Result %f \n", this->servos.servo[i].target, this->servos.servo[i].scale, this->servos.servo[i].zero, position);
       position = Limit(position, 0, 32000);
       packet[1] = i;
       positionBuff = position;
       packet[2] = (positionBuff>>8);
       packet[3] = (positionBuff & 0xFF);
       this->servos.servo[i].update = 0;  // reset updater
     //  printf("%02X %02X %02X \n", packet[0], packet[1], packet[2]);
       SerWrite(&(this->serial), packet, 4);  // do blocking write
       u++;
     } 
     i++;
     if (i > (SERVOCOUNT-1)) {
       i = 0;
       if ( u == 0) usleep(5000); // if there were no updates, sleep for a bit
       u = 0;
     }
   }

}


Status_t SSC8SpoolerInit(SSC8Spooler_t *this){
  int i;
  Status_t retval;
  
  if ((retval = SerInit(&(this->serial))) != OK)  return retval;

  this->servos.servoCount = SERVOCOUNT;
  this->servos.servo      = malloc(sizeof(servo_t) * this->servos.servoCount); //!!!???!!!
  
  for (i = 0; i < SERVOCOUNT; i++){
    ServoInit(&(this->servos.servo[i]));
    // zero is 12000
    // scale is 24000/180
    this->servos.servo[i].zero = 16000;
    this->servos.servo[i].scale = 32000/180;
  }
  
  return OK;
}



Status_t SSC8SpoolerFini(SSC8Spooler_t *this){
  pthread_cancel(this->xmitter);
  free(this->servos.servo);
  return SerFini(&(this->serial)); 
}



Status_t SSC8SpoolerOpenDevice(SSC8Spooler_t *this, char * filename){
  return SerOpen(&(this->serial), filename, 9600);  
}


/*


#include "SSC8Spooler.h"



int main(void) {

  SSC8Spooler_t  axii;
   
  SSC8SpoolerInit(&axii);                      // initialize system
  SSC8SpoolerOpenDevice(&axii, "/dev/ttyS0");  // open serial port
  SSC8SpoolerStartTransmitter(&axii);          // start transmitter
  
  
  ServoSetTarget(&(axii.servos.servo[0]), -22);   // move servos around
  sleep(1);                                     
  ServoSetTarget(&(axii.servos.servo[0]),  22);   
  sleep(1);                                     
  ServoSetTarget(&(axii.servos.servo[0]),  0); 
  sleep(1);
  
  SSC8SpoolerFini(&axii);                      // shutdown everything

  return 0;
}


*/
