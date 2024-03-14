#include "inputArm.h"


Status_t armTrainerInit(armTrainer_t * this, char * device, char * config) {

  Status_t retval;

  if (SerOpen (&(this->port), device, 9600) != OK)
    return(CantOpen);
  
  if ((retval = transInitFile(&(this->buffer), 8, config)) != OK) 
    return(retval);
    
  return SerStartNotifier(&(this->port), armTrainerCharReciever, (void*)(&this->buffer)) ;  // ssls3! YEA!


}




void armTrainerCharReciever(unsigned char * data, void * anchor) {

  unsigned char byte;
  static unsigned char  byte1, byte2 ;
  static int state;
  transBuffer_t  * buffer;
  
  unsigned int   value;
  unsigned int   channel;

  buffer = (transBuffer_t  *) anchor;
  byte = *data;

  if (state == 0) {
    if ((byte & 0x80) != 0) { /*printf("x%X", byte);*/ return;} // reject
    byte1 = byte; 
    state = 1;    
  } else {
    if ((byte & 0x80) == 0) { state = 0; /*printf("X%X", byte); */return; } // reject
    byte2 = byte;

    channel = (byte1 >> 3);
    value = ((unsigned int)byte2 & 0x7F) | (((unsigned int)byte1 & 0x07)<<7);    
    
    buffer->in[channel] = value;

    state = 0;
  }

  return;

}


/*

int main(void) {

   armTrainer_t  INP;
   unsigned int i;
   
   printf("Initializing\n");
   if (armTrainerInit(&INP, "/dev/ttyUSB0", "inputarm.tf") != OK) {
     printf("port open error\n");
     return 0;
   }
   clrscr();
        
   while(1) {
     transCalc(&(INP.buffer));
     gotoxy(2,1);
     for (i = 0; i < 8; i++) {
       printf("Channel %d = %f\n", i, INP.buffer.out[i]);
     }
   
   }
  

  return 0;
}

*/
