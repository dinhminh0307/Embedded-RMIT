#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define CE 0
#define SCLK 1
#define DAT 2
#define TEST 5


int half_cycle = 0;
volatile int buttonPressed = 0;
int i = 0;

void writeCommand(int i);

void pinSetup() {
  DDRB |= (1 << TEST);	// Set PORTB5 as output for LED - for TESTING
  DDRD &= ~(1 << 2); // Set PD2 pin (INT0) as input to receive interrupt re-quest
  DDRB |= (1 << CE) | (1 << SCLK); //PORTB0 (CE) & PORTB1 (SCLK) as output
	EICRA |= (1 << ISC01); // set INT0 to trigger on Falling edge
	EIMSK  |= (1 << INT0); // Turns on INT0
}

void timer1Setup() {
  TCCR1B |= (1 << WGM12); //CTC Mode On
  TCCR1B |= (1 << CS11) | (1 << CS10); //Prescaler of 64
  OCR1A = 24999; //Duration: 0.05s
  TIMSK1 |= (1 << OCIE1A); //Output Compare Match A Interrupt On
	sei(); // Turn on the Global Interrupt Enable Bit
}

int main(void) {
  pinSetup();

  timer1Setup();
  
	while (1)
	{
	}
}

ISR ( TIMER1_COMPA_vect ) {
  if (buttonPressed == 1) { //button is pressed
    DDRB |= (1 << DAT); //Output to DS1302 
    half_cycle++;

    if (half_cycle == 1) { //PORTB0 ON
      PORTB |= (1 << CE);
    } else if (half_cycle >= 2 && half_cycle <= 16) { //PORTB1 toggles
      if (half_cycle % 2 == 0) { //rising-edge
        writeCommand(i);
        i++;
      }
      _delay_ms(10); //wait 10ms for DAT toggles, then toggles SCLK
      PORTB ^= (1 << SCLK);
    } else if (half_cycle >= 17 && half_cycle <= 31) {
      DDRB &= ~(1 << DAT); //Input to DS1302 
      _delay_ms(10);
      PORTB ^= (1 << SCLK);
      
      //PLUG OSC AND READ THIS PIN: PIND0
      // if ( !(PORTB & (1 << SCLK)) ) {
      //   readData();
      // }
      
    } else if (half_cycle > 31) { //PORTB1 toggles full 15 cycles
      half_cycle = 0; //reset cycle to 0 for next button pressed
      i = 0;
      buttonPressed = 0; //reset button

      PORTB &= ~(1 << CE); //PORTB0 OFF
    }
  }
}
  

ISR ( INT0_vect )	// ISR for the INT0
{ 
  buttonPressed = 1;
}

void writeCommand(int i) {
    switch (i) {
      case 0:
        PORTB |= (1 << DAT); //1
        break;
      case 1:
        PORTB &= ~(1 << DAT); //0
        break;
      case 2:
        PORTB |= (1 << DAT); //1
        break;
      case 3:
        PORTB &= ~(1 << DAT); //0
        break;
      case 4:
        PORTB &= ~(1 << DAT); //0
        break;
      case 5:
        PORTB &= ~(1 << DAT); //0
        break;
      case 6:
        PORTB &= ~(1 << DAT); //0
        break;
      case 7:
        PORTB |= (1 << DAT); //1
        break;
      default:
        break;
  }
}


void readData() {
  if ( PINB & (1 << DAT) ) { //1
    PORTB |= (1 << DAT);
  } else { //0
    PORTB &= ~(1 << DAT);
  }
}


