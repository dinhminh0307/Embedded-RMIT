#include <avr/io.h>
#include <avr/interrupt.h>
#define CE 0
#define SCLK 1
#define DAT 0

int cycle = 0;
volatile int buttonPressed = 0;
int hour;
int i = 0;

void writeCommand();

int main(void)
{
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

  UCSR0B = (1 << RXEN0 ) | (1 << TXEN0 ); // Turn on the transmission and reception circuitry
  UCSR0C = (1 << UCSZ00 ) | (1 << UCSZ01 ); // Use 8- bit character sizes

  UCSR0B = (1 << RXCIE0) | (1 << TXCIE0); //Enable RX and TX Complete Interrupt

	while (1)
	{
	}
}

ISR ( TIMER1_COMPA_vect ) {
  if (buttonPressed == 1) { //button is pressed
    DDRD |= (1 << DAT); //Output to DS1302 
    cycle++;

    if (cycle == 1) { //PORTB0 ON
      PORTB |= (1 << CE);
    } else if (cycle >= 2 && cycle <= 16) { //PORTB1 toggles
      PORTB ^= (1 << SCLK);

      if (PORTB & (1 << SCLK)) { //rising-edge
        writeCommand();
      }
    } else if (cycle >= 17 && cycle <= 31) {
      DDRD &= ~(1 << DAT); //Input to DS1302 
      //PLUG OSC AND READ THIS PIN: PIND0
      /*
      while (( UCSR0A & (1 << RXC0 )) == 0) { // Wait till data received and ready to be read from UDR0
        hour = UDR0 ; // Read the received byte value
      }
      */
      /*
      if ( !(PORTB & (1 << SCLK)) ) { //falling-edge
        readData();
      }
      */
    } else if (cycle > 31) { //PORTB1 toggles full 15 cycles
      cycle = 0; //reset cycle to 0 for next button pressed
      buttonPressed = 0; //reset button

      PORTB &= ~(1 << CE); //PORTB0 OFF
    }
  }
}
  
} 

ISR ( INT0_vect )	// ISR for the INT0
{ 
  buttonPressed = 1;
}

void writeCommand() {
  for (int i = 0; i <= 7; i++) {
    switch (i) {
      case 0:
        PORTD |= (1 << DAT); //1
        break;
      case 1:
        PORTD &= ~(1 << DAT); //0
        break;
      case 2:
        PORTD |= (1 << DAT); //1
        break;
      case 3:
        PORTD &= ~(1 << DAT); //0
        break;
      case 4:
        PORTD &= ~(1 << DAT); //0
        break;
      case 5:
        PORTD &= ~(1 << DAT); //0
        break;
      case 6:
        PORTD &= ~(1 << DAT); //0
        break;
      case 7:
        PORTD |= (1 << DAT); //1
        break;
    }
  }
}

int readData(int i) { //NOT DONE
  for (i = 0; i <= 7; i++) {
    if ( PIND & (1 << DAT) ) { //1
      return (1 << i);
    } else { //0
      return (1 << i);
    }
  }
}

