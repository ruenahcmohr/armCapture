#include "transformer.h"
#include <stdio.h>

/*
 file format is:   destination_channel,  source_channel,  scale,  offset
 
 if a destination is defined more than once, the defintions are added.



*/

Status_t transformSetLoadFrom(transformSet_t * this, char * filename) {

  FILE * input;
  unsigned int i;  

  if ((input = fopen(filename, "r")) == NULL) {  //open text file 'param 1' w/ err chk
      return CantOpen;
  }

  fflush(stdout);
  if (this->count != 0) {
    this->transform = realloc(this->transform, sizeof(transform_t)); // down to 1 element.
    this->count = 1;
  }

  // we have to realloc the array as we find lines.
  i = 0;
  while(fscanf(input, "%d, %d, %f, %f\n", &(this->transform[i].destChan), &(this->transform[i].srcChan), &(this->transform[i].scale), &(this->transform[i].translate)) != EOF) {
    i++;
    this->transform = realloc(this->transform, sizeof(transform_t)*(i+1));
  }
  this->count = i;
  
  
  fclose(input);
  
  return OK;
}



Status_t transformSetSaveTo(transformSet_t * this, char * filename) {
  FILE * output;
  unsigned int i;
  
  if ((output = fopen(filename, "w")) == NULL) {  //open text file 'param 1' w/ err chk
      return CantOpen;
  }

  for( i = 0; i < this->count; i++) {
    fprintf(output, "%d, %d, %f, %f\n", this->transform[i].destChan, this->transform[i].srcChan,  this->transform[i].scale, this->transform[i].translate);    
  }
  fclose(output);
  return OK;
}

/*
 results are accumulative, you need to zero dst before calling this.
 I cant do it because I dont know how many dst elements there are.
*/
Status_t transformSetDo(transformSet_t * this, float * src, float * dst) {

  unsigned int i;
  
  for (i = 0; i < this->count; i++) 
    dst[this->transform[i].destChan] += (src[this->transform[i].srcChan] * this->transform[i].scale) + this->transform[i].translate;  
  
  return OK;

}

Status_t transformSetClear(transformSet_t * this, float * src, float * dst) {

  unsigned int i;
  
  for (i = 0; i < this->count; i++) 
    dst[this->transform[i].srcChan] = 0;
  
  return OK;

}

Status_t transformSetInit(transformSet_t * this, unsigned int elements) {
   unsigned int i;
   
   this->count = elements;
   this->transform = malloc(sizeof(transform_t) * elements);
   
   if (this->transform == NULL) 
     return ErrMalloc;
     
   for( i = 0; i < elements; i++) {
     this->transform[i].srcChan    = i;
     this->transform[i].destChan   = i;
     this->transform[i].scale      = 1;
     this->transform[i].translate  = 0;
   }
     
   return OK;
}

void transformSetDump(transformSet_t * this) {

  unsigned int i;
  
  for(i = 0; i < this->count; i++ )
    printf("Rule %d: Dst(%d) += (Src(%d) x %0.2f) + %0.2f\n",i,  this->transform[i].destChan, this->transform[i].srcChan, this->transform[i].scale, this->transform[i].translate); 
  return;
}

/*


void showList(float * this, unsigned int count) {

  unsigned int i;
  for (i = 0; i < count; i++) 
    printf("%d = %f\n", i, this[i]);

}

int main(void) {

  unsigned int    i;
  float           in[8], out[8];
  transformSet_t  T;
  
  for( i = 0; i < 8; i++ ) {
    in[i]  = i;
    out[i] = 0;
  }
  transformSetInit(&T, 8);
  printf("Before transform: \n");
  showList(in, 8);
  
  T.transform[3].srcChan   = 2;
  T.transform[3].scale     = 3;
  T.transform[3].translate = 4;
  
  T.transform[5].destChan   = 6;
  
  transformSetDo(&T, in, out);
  printf("After transform: \n");
  showList(out, 8);
  
  transformSetSaveTo(&T, "testset2.tf");
  printf("As saved:\n");
  transformSetDump(&T);
  
  transformSetLoadFrom(&T, "testset.tf");
  printf("As loaded: \n");
  transformSetDump(&T);
  
 return 0;
}


*/



