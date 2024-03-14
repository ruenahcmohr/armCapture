#ifndef __inputarm
  #define __inputarm

  #include "returnValues.h"
  #include "transBuffer.h"
  #include "ssls3.h"

   


  typedef struct armTrainer_s {

   SSLS_t        port;
   transBuffer_t buffer;

  } armTrainer_t;

  Status_t armTrainerInit(armTrainer_t * this, char * device, char * config) ;  
  void armTrainerCharReciever(unsigned char * data, void * anchor) ;
  
  
  

#endif

