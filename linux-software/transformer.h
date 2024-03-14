#ifndef __transformer
 #define __transformer
 
  #include <stdio.h>
  #include <malloc.h>
  #include "returnValues.h"



  typedef struct transform_s {

     unsigned int  srcChan;
     float         scale;
     float         translate;
     unsigned int  destChan;

  } transform_t;


  typedef struct transformSet_s {

     unsigned int      count;
     transform_t  *    transform;     

  } transformSet_t;


  Status_t transformSetLoadFrom(transformSet_t * this, char * filename); 
  Status_t transformSetSaveTo(transformSet_t * this, char * filename) ;
  Status_t transformSetDo(transformSet_t * this, float * src, float * dst);
  Status_t transformSetInit(transformSet_t * this, unsigned int elements) ;
  void transformSetDump(transformSet_t * this) ;







 
 
#endif
