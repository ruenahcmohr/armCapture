/*


 +-------------------------------------+
 |                          Sig Vd Gnd |
 |  +---------+   5V O     PB0 [o o o] | baud test input
 |  | 7805  O |   Vd O     PB1 [o o o] | 
 |  +---------+   V+ .     PB2 [o o o] | 
 |                         PB3 [o o o] | chan0 pwm
 |                         PB4 [o o o] | chan0 dir
 |                         PB5 [o o o] | chan1 dir
 |                         PB6 [o o o] | chan2 dir
 |                         PB7 [o o o] | chan3 dir
 |                         PA0 [o o o] | chan0 current sense
 |                         PA1 [o o o] | chan0 feedback voltage
 |        +----------+     PA2 [o o o] | chan1 current sense
 |        |O         |     PA3 [o o o] | chan1 feedback voltage
 |        |          |     PA4 [o o o] | chan2 current sense
 |        |          |     PA5 [o o o] | chan2 feedback voltage
 |        |          |     PA6 [o o o] | chan3 current sense
 |        |          |     PA7 [o o o] | chan3 feedback voltage
 |        |          |     PC7 [o o o] | chan3 status -
 |        |          |     PC6 [o o o] | chan3 status +
 |        |          |     PC5 [o o o] | chan2 status -
 |        | ATMEGA32 |     PC4 [o o o] | chan2 status +
 |        |          |     PC3 [o o o] | chan1 status -
 |        |          |     PC2 [o o o] | chan1 status +
 |        |          |     PC1 [o o o] | chan0 status -
 |        |          |     PC0 [o o o] | chan0 status +
 |        |          |     PD7 [o o o] | chan3 pwm
 |        |          |     PD2 [o o o] |
 |        |          |     PD3 [o o o] |
 |        |          |     PD4 [o o o] | chan2 pwm
 |        |          |     PD5 [o o o] | chan1 pwm
 |        +----------+     PD6 [o o o] | Debug
 |      E.D.S BABYBOARD III            |
 +-------------------------------------+



*/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"


#define IsHigh(BIT, PORT)    ((PORT & (1<<BIT)) != 0)
#define IsLow(BIT, PORT)     ((PORT & (1<<BIT)) == 0)
#define SetBit(BIT, PORT)     PORT |= (1<<BIT)
#define ClearBit(BIT, PORT)   PORT &= ~(1<<BIT)

#define Kp  1
#define Ki  4

#define ABS(a)                ((a) < 0 ? -(a) : (a))
#define SIGN(x)               (x)==0?0:(x)>0?1:-1
#define NOP()                 asm volatile ("nop"::)

/*****************************| DEFINIATIONS |********************************/

#define OUTPUT             1
#define INPUT              0


/*****************************| VARIABLES |********************************/
 
volatile int AdcValues[8];


 
/************************| FUNCTION DECLARATIONS |*************************/
 
void AnalogInit (void);
int  Analog (int n);


/****************************| CODE... |***********************************/

int main (void) {
 
  unsigned int i;
  unsigned int temp;
  unsigned char packet1, packet2;

  // set up directions 
//  DDRA = (INPUT << PA0 | INPUT << PA1 |INPUT << PA2 |INPUT << PA3 |INPUT << PA4 |INPUT << PA5 |INPUT << PA6 |INPUT << PA7);
  DDRB = (INPUT << PB0 | INPUT << PB1 |INPUT << PB2 |INPUT << PB3 |INPUT << PB4 |INPUT << PB5 |INPUT << PB6 |INPUT << PB7);
  DDRC = (INPUT << PC0 | INPUT << PC1 |INPUT << PC2 |INPUT << PC3 |INPUT << PC4 |INPUT << PC5 |INPUT << PC6 );
  DDRD = (INPUT << PD0 | INPUT << PD1 |INPUT << PD2 |INPUT << PD3 |INPUT << PD4 |INPUT << PD5 |INPUT << PD6 |INPUT << PD7);         
  
  USART_Init( 103 ); // 9600 @ 16Mhz
  AnalogInit();
  sei();
 
  while(1){
    
    for(i = 0; i < 8; i++) {   
    
    // 00cccddd  1ddddddd
    //  while(i == (ADMUX&7)); // dont go on values about to be worked on.
      
      temp = AdcValues[i];
      
      
      
      packet1 = (temp>>7)|(i<<3);
      packet2 = (temp&0xFF)|(1<<7);      
      USART_Transmit( packet1);
      USART_Transmit( packet2);
      NOP();
    }                
  }     
}

//------------------------| FUNCTIONS |------------------------



void AnalogInit (void) {

  int i;

  // Activate ADC with Prescaler 
  ADCSRA =  1 << ADEN  |
            1 << ADSC  | /* start a converstion, irq will take from here */
            0 << ADATE |
            0 << ADIF  |
            1 << ADIE  | /* enable interrupt */
            1 << ADPS2 |
            1 << ADPS1 |
            1 << ADPS0 ;
                        
            
  for (i = 0; i < 8; AdcValues[i++] = 0);
  
}


 
int Analog (int n) {
  return AdcValues[n & 7];
}


//SIGNAL(SIG_ADC) {
ISR(ADC_vect) {
  int i;

  i = ADMUX & 7;       // what channel we on?
  AdcValues[i] = ADC;  // save value
  i++;                 // next value
  ADMUX = (i & 7) | (1<<REFS0);       // select channel
  ADCSRA |= _BV(ADSC); // start next conversion

  return;
}


