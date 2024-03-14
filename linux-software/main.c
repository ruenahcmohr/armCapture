#include <stdio.h>
#include "inputArm.h"
#include "servoArm.h"
#include "transformer.h"
#include "returnValues.h"
#include <stdint.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

#include "colours.h"


/****

realtime mode is used to make a 1:1 playback file
non-realtime is used to create waypoints


*/

void setupTerm() {

    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &new_term_attr);
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

}


void targetingColour( float this, float that) {
	 if (0) {
	 } else if ((this - that) > 4) {
	   textcolor(CC_RED);
	 } else if ((this - that) < -4) {
	   textcolor(CC_BLUE);
	 } else if ((this - that) > 2) {
	   textcolor(CC_YELLOW);
	 } else if ((this - that) < -2) {
	   textcolor(CC_YELLOW);  	   
	 } else {
	   textcolor(CC_GREEN);
	 } 
}

void selectColour( char this, char that) {
  	 if (this == that) {
	   textbackground(CC_YELLOW); 
	 } else {
	   textbackground(CC_BLACK);
	 } 
}


void addDisplayHistory(float * data) {
  static unsigned int count = 0;
  static float Buffer[4][6];
  unsigned int i, j;
  
  for(i = 3; i >0; i--) {
    for(j = 0; j < 6; j++) Buffer[i][j] = Buffer[i-1][j];      
  }
  
  for(j = 0; j < 6; j++) Buffer[0][j] = data[j]; 
  
  for(i = 0; i < 4; i++)  printf(" %d:  %-7.2f, %-7.2f, %-7.2f, %-7.2f, %-7.2f, %-7.2f              \n", count-i, Buffer[i][0], Buffer[i][1], Buffer[i][2], Buffer[i][3], Buffer[i][4], Buffer[i][5]);
  count++;

}


int main(int argc, char * argv[]) {

   servoArm_t    *OUT;
   unsigned int  outCount;
   armTrainer_t  INP;
   transformSet_t BRIDGE;
   float last[6];
   unsigned int  i;
   unsigned char operate;
   FILE *save;
   char pause = 1, select = 0, realtime = 0;
   char c;
   unsigned long long t, lt;
   struct timeval tv;
   
   //---------------------------------------------------------------------------------------
   
   if (argc < 5) {
     printf("Usage %s devicein configin deviceout configout [ deviceout configout ]...\n", argv[0]);
     return 0;
   }   
   
   //-- set up input files --
   printf("Initializing with %d params.\n", argc);
   if (armTrainerInit(&INP, argv[1], argv[2]) != OK) {
     printf("Input port open error %s\n", argv[1]);
     return 0;
   }
   printf("Input source open OK.\n");            
   
   //-- set up output files --
   OUT = malloc(sizeof(servoArm_t)*4);
   printf("OUT: %X\n", OUT);
   for(outCount = 1 ; (argc-1) >= ((outCount*2)+2) ; outCount++) {
   
   //  OUT = realloc(OUT, sizeof(servoArm_t)*outCount);
     printf("OUT: %X\n", OUT);
     if (servoArmInit(&OUT[outCount-1], argv[1+(outCount*2)], argv[2+(outCount*2)]) != OK) {
       printf("Output arm port open error %s\n", argv[1+(outCount*2)]);
       return 0;
     }
     printf("Output arm port open OK. %s (%d)\n", argv[1+(outCount*2)], OUT[outCount-1].axii.serial.fd);
     transZeroInput(&(OUT[outCount-1].buffer)) ;   

   }
   outCount--;
   printf("%d arms initialized.\n", outCount);
   
   if ((save = fopen("saved.motion", "wt")) == NULL) {
     printf("Cannot open save out file\n");
     return 0;
   }   
   printf("Save out file opened OK.\n");
   
   
   //-- initialize data bridge --        
   if (transformSetInit(&BRIDGE, 1) != OK) {
     printf("bridge init err\n");
     return 0;
   }   
   if ( transformSetLoadFrom(&BRIDGE, "bridge.tf") != OK) {
     printf("bridge init err, does bridge.tf exist?\n");
     return 0;
   }
   printf("Bridge initialized OK.\n");   
   
   sleep(1);
   
   clrscr();
   gotoxy(1,1);
   printf(" Space or s to save position to file\n");
   printf(" p to toggle pause\n");
   printf(" r to toggle realtime\n");
   printf(" q to quit\n");
   printf(" = during pause to reset offsets\n");
   printf(" arrows during pause to adust position\n");
   
   setupTerm();
       
   operate = 1;
   while(operate) {               
   
   /*    [in INP out]  [in BRIDGE out] [in OUT out]
         ADC     DEG    DEG       DEG   DEG    SERVO_COUNT   */
      
     transCalc(&(INP.buffer)); // transform input values to standard degress.
      
     i = 0;  
     for(i = 0; i < outCount; i++ ) {  //!!!???!!! this would be faster if we just do teh bridge once and transfer the numbers to the other arms
       
       transZeroInput( &OUT[i].buffer );                    
       transformSetDo(&BRIDGE, INP.buffer.out, OUT[i].buffer.in); // force a calc between the ends of the two adjacent buffers using the bridge transform     
       transCalc(&(OUT[i].buffer)); // transform output values from standard values.
     
     }
     
     
     
     if (!pause)  {
       
       for(i = 0; i < outCount; i++ )  servoArmSync(&OUT[i]); 
     
       last[0] = OUT[0].buffer.in[0]; 
       last[1] = OUT[0].buffer.in[1]; 
       last[2] = OUT[0].buffer.in[2]; 
       last[3] = OUT[0].buffer.in[3]; 
       last[4] = OUT[0].buffer.in[4];  
       last[5] = OUT[0].buffer.in[5];
           
     } else {
         gotoxy(8, 1);
	 textcolor(CC_WHITE);
	 printf("-- PAUSED --\n");
	 	 
         selectColour(select, 0); printf(" %-7.2f,",  last[0]);  
	 selectColour(select, 1); printf(" %-7.2f,",  last[1]);  
	 selectColour(select, 2); printf(" %-7.2f,",  last[2]);  
	 selectColour(select, 3); printf(" %-7.2f,",  last[3]); 
	 selectColour(select, 4); printf(" %-7.2f,",  last[4]);  
	 selectColour(select, 5); printf(" %-7.2f\n", last[5]); 
	 selectColour(1, 2);	 
	   
	// printf(" %-7.2f, %-7.2f, %-7.2f, %-7.2f, %-7.2f, %-7.2f              \n", last[0], last[1], last[2], last[3], last[4], last[5]);	 	 
	 
	 targetingColour( OUT[0].buffer.in[0], last[0]);  printf(" %-7.2f,", OUT[0].buffer.in[0]);	 
	 targetingColour( OUT[0].buffer.in[1], last[1]);  printf(" %-7.2f,", OUT[0].buffer.in[1]);	 
	 targetingColour( OUT[0].buffer.in[2], last[2]);  printf(" %-7.2f,", OUT[0].buffer.in[2]);
	 targetingColour( OUT[0].buffer.in[3], last[3]);  printf(" %-7.2f,", OUT[0].buffer.in[3]);
	 targetingColour( OUT[0].buffer.in[4], last[4]);  printf(" %-7.2f,", OUT[0].buffer.in[4]);
	 targetingColour( OUT[0].buffer.in[5], last[5]);  printf(" %-7.2f",  OUT[0].buffer.in[5]);
	 printf("           \r");	 
	 
     }
        
      if ( read(fileno(stdin),&c,1) == 1) {
 //    if ((c = fgetc(stdin)) != (-10)) {
 //    if (1) {
 //      c = getchar();
      // printf("=================================================================> %X \n", c) ;
       switch (c) {
         case 's':
	 case ' ':
	   gotoxy(20,1); textcolor(CC_WHITE);
	   printf("------------------------| FILE |----------------------------\n");
	   addDisplayHistory(last);
	   
	   if (!realtime) {
	     fprintf(save, "1, %f, %f, %f, %f, %f, %f\n", last[0], last[1], last[2], last[3], last[4], last[5]);
	   } else {
	     /*  gettimeofday(&tv,NULL);
	       t = tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
	       fprintf(save, "%f, %f, %f, %f, %f, %f, %f\n", (float)(t-lt)/(float)1000000, last[0], last[1], last[2], last[3], last[4], last[5]);
	       lt = t;
	       usleep(60000); 
	       fflush(stdin);*/
	   }
	   
	   fflush(save);
	 break;
	 
	 case 'q':
	   operate = 0;
	 break;
	 
	 case 'r':
	    realtime = 1 - realtime;
	    gotoxy(11,1);
	    if (realtime) {
	      printf("realtime mode.\n");
	    } else {
	      printf("1s points mode.\n");
	    }
	 break;
	 
	 case 'p':
	   pause = 1 - pause;
	   if (!pause) 	lt = tv.tv_sec*(uint64_t)1000000+tv.tv_usec;   
	   
	 break;
	 
	 case '=':
	   if (!pause) break;
	   
	   gotoxy(11,1);
	   BRIDGE.transform[0].translate  =  last[0] - INP.buffer.out[0] ;
	   BRIDGE.transform[1].translate  =  last[1] - INP.buffer.out[1] ;	
	   BRIDGE.transform[2].translate  =  last[2] - INP.buffer.out[2] ;
	   BRIDGE.transform[3].translate  =  last[3] - INP.buffer.out[3] ;
	   BRIDGE.transform[4].translate  =  last[4] - INP.buffer.out[4] ;
	   BRIDGE.transform[5].translate  =  last[5] - INP.buffer.out[5] ;
	
	   printf("offsets applied.\n");
	   
	 break;
	 
	 // cursor control
	 case '\e':
	   gotoxy(19,1);
	   if ((c = fgetc(stdin)) == 0x5B) {	   
	     c = fgetc(stdin);
	     if (0) {
	     } else if (c == 0x41) {
	      last[select] += 0.5;
	     } else if (c == 0x42) {
	      last[select] -= 0.5;;
	     } else if (c == 0x43) {
	      if (select < 5) select++;
	     } else if (c == 0x44) {
	      if (select > 0) select--;
	     }
	     for(i = 0; i < outCount; i++ ) { 
               OUT[i].buffer.in[0] = last[0]; 
	       OUT[i].buffer.in[1] = last[1]; 
	       OUT[i].buffer.in[2] = last[2]; 
	       OUT[i].buffer.in[3] = last[3]; 
	       OUT[i].buffer.in[4] = last[4];  
	       OUT[i].buffer.in[5] = last[5];

               transCalc(&(OUT[i].buffer));
	       servoArmSync(&OUT[i]); 
             }
	     
	   }
	 break;
	 
       }      
     }
      if (!realtime) {
       usleep(20000);
      } else {
       if (!pause) {
         gettimeofday(&tv,NULL);
	 t = tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
	 fprintf(save, "%f, %f, %f, %f, %f, %f, %f\n", (float)(t-lt)/(float)1000000, last[0], last[1], last[2], last[3], last[4], last[5]);
	 lt = t;
	 usleep(60000); 
	 fflush(stdin);    
	 gotoxy(20,1); textcolor(CC_WHITE);
	 printf("------------------------| FILE |----------------------------\n");
	 addDisplayHistory(last);   
       
       }
      }
     
   
   }
   
   fclose(save);
  

  return 0;
}

