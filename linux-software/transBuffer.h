#ifndef __transbuffer
  #define __transbuffer

  #include "transformer.h"
  #include "returnValues.h"
  #include <stdio.h>

  typedef struct transBuffer_s {

  float *         in;
  float *         out;
  unsigned int    count;
  transformSet_t  *tf;

} transBuffer_t; 

Status_t transInit(transBuffer_t *this, unsigned int count, transformSet_t * tf);
Status_t transInitFile(transBuffer_t *this, unsigned int count, char * filename);
Status_t transCalc(transBuffer_t *this);
Status_t transCreateBuffers(transBuffer_t *this, unsigned int count);
Status_t transDump(transBuffer_t *this ) ;
Status_t transZeroInput(transBuffer_t *this ) ;
Status_t transZeroOutput(transBuffer_t *this ) ;


#endif
