#include "transBuffer.h"


/*

  use a call to transformSetLoadFrom or otherwise to set up tf
  
*/
Status_t transInit(transBuffer_t *this, unsigned int count, transformSet_t * tf) {
    
   this->tf    = tf;
   this->count = count;
  
   return transCreateBuffers(this, count);
   
}


/*

  or just use this and pass a filename
  
*/
Status_t transInitFile(transBuffer_t *this, unsigned int count, char * filename) {

  Status_t retval;
  
  this->tf = malloc(sizeof(transformSet_t));
  if (this->tf == NULL) 
    return ErrMalloc;    
    
  if ((retval = transformSetInit(this->tf, 1)) != OK) 
    return retval;
    
  if ((retval = transformSetLoadFrom(this->tf, filename)) != OK) 
    return retval;
  
  return transCreateBuffers(this, count);

}


Status_t transFiniFile(transBuffer_t *this) {
  if (this->tf) {
    free(this->tf);
    this->tf = NULL;
  }
  return OK;
}



Status_t transCalc(transBuffer_t *this) {

  unsigned int i;
  
  for(i = 0; i < this->count; i++) 
    this->out[i] = 0.0;
  
  transformSetDo( this->tf, this->in, this->out);  

  return OK;
}


Status_t transCreateBuffers(transBuffer_t *this, unsigned int count) {

  this->in = malloc(sizeof(float) * count);   
  if (this->in == NULL) 
     return ErrMalloc;
  
  this->out = malloc(sizeof(float) * count);   
  if (this->out == NULL) 
     return ErrMalloc;         
     
  this->count = count;        

  return OK;
}


Status_t transDump(transBuffer_t *this ) {

  unsigned int i;
  
  for (i = 0; i < this->count; i++) 
    printf("%d = % 7.2f              \n", i, this->out[i]);

  return OK;
}


Status_t transZeroInput(transBuffer_t *this ) {

  unsigned int i;
  
  for (i = 0; i < this->count; i++) 
   this->in[i] = 0;

  return OK;
}

Status_t transZeroOutput(transBuffer_t *this ) {

  unsigned int i;
  
  for (i = 0; i < this->count; i++) 
   this->out[i] = 0;

  return OK;
}


/*


int main(void) {

  transBuffer_t  tb;
  unsigned int i;
  
  printf("initializing from file test.tf...\n");
  transInitFile(&tb, 8, "test.tf");  
  
  transformSetDump(tb.tf);  
  
  printf("setting all inputs to 1.0...\n");
  for(i = 0; i < tb.count; i++) 
    tb.in[i] = 1.0;
  
  
  printf("doing calculation...\n");
  transCalc(&tb);
  printf("dumping results...\n");
  transDump(&tb) ;
  
  

  return 0;
}

*/

