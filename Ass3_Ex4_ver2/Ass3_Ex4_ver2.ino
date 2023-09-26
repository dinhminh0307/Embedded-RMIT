#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#define CE 0
#define SCLK 1
#define DAT 2

int cycle = 0;
volatile int buttonPressed = 0;
int i = 0;

void writeCommand();


int main(void)
{
  //Setup UART
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  /* Enable UART transmitter/receiver */
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    /* 8 data bits, 1 stop bit */
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	DDRB |= (1 << 5);	// Set PORTB5 as output for LED - for TESTING
	DDRD &= ~(1 << 2); // Set PD2 pin (INT0) as input to receive interrupt re-quest
  DDRB |= (1 << CE) | (1 << SCLK); //PORTB0 - CE & PORTB1 - SCLK as output
	EICRA |= (1 << ISC01); // set INT0 to trigger on Falling edge
	EIMSK  |= (1 << INT0); // Turns on INT0

  TCCR1B |= (1 << WGM12); //CTC Mode On
  TCCR1B |= (1 << CS11) | (1 << CS10); //Prescaler of 64
  OCR1A = 62499; //Duration: NOT 0.1s
  TIMSK1 |= (1 << OCIE1A); //Output Compare Match A Interrupt On
	sei(); // Turn on the Global Interrupt Enable Bit

	while (1)
	{
	}
}

ISR ( TIMER1_COMPA_vect ) {
  if (buttonPressed == 1) { //button is pressed
    DDRB |= (1 << DAT); //Output to DS1302 
    cycle++;

    if (cycle == 1) { //PORTB0 ON
      PORTB |= (1 << CE);
    } else if (cycle >= 2 && cycle <= 16) { //PORTB1 toggles
      if (cycle % 2 == 0) { //rising-edge
        PORTB ^= (1 << SCLK);
        writeCommand(i);
        i++;
      } else { 
        PORTB ^= (1 << SCLK);
      }
    } else if (cycle >= 17 && cycle <= 31) {
      PORTB ^= (1 << SCLK);
      DDRB &= ~(1 << DAT); //Input to DS1302 
      //PLUG OSC AND READ THIS PIN: PIND0
    } else if (cycle > 31) { //PORTB1 toggles full 15 cycles
      cycle = 0; //reset cycle to 0 for next button pressed
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
  while (i <= 7) {
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
    }
  }
}

/*
int readData() { //NOT DONE
  for (i = 0; i <= 7; i++) {
    if ( PINB & (1 << DAT) ) { //1
      return (1 << i);
    } else { //0
      return (1 << i);
    }
  }
}
*/
