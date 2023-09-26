#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define CE 0
#define SCLK 1
#define DAT 2


int cycle = 0;
volatile int buttonPressed = 0;
int hour;
int i = 0;
int j = 0;
uint8_t temp = 0x00;

void writeCommand(int i);

int main(void)
{
  // Init UART
  Serial.begin(9600);

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
  OCR1A = 2499; //Duration: 0.05s
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
    DDRB |= (1 << DAT); //Output to DS1302 
    cycle++;

    if (cycle == 1) { //PORTB0 ON
      PORTB |= (1 << CE);
    } else if (cycle >= 2 && cycle <= 16) { //PORTB1 toggles
      PORTB ^= (1 << SCLK);

      if (PORTB & (1 << SCLK)) { //rising-edge
        writeCommand(i);
        i++;
      }
    } else if (cycle >= 17 && cycle <= 31) {
      PORTB ^= (1 << SCLK);
      DDRB &= ~(1 << DAT); //Input to DS1302 
      //PLUG OSC AND READ THIS PIN: PIND0
      for(int k = 0; k <=7; k++) {
        if(readData() == 1) {
          temp |= (1 << k);
        }
        else {
          temp &= ~(1 << k);
        }
      }
      
    } else if (cycle > 31) { //PORTB1 toggles full 15 cycles
      cycle = 0; //reset cycle to 0 for next button pressed
      buttonPressed = 0; //reset button

      PORTB &= ~(1 << CE); //PORTB0 OFF
      temp = 0x00;
    }
  }
  Serial.println(temp);
}
  

ISR ( INT0_vect )	// ISR for the INT0
{ 
  buttonPressed = 1;
  _delay_ms(1000);
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


int readData() { //NOT DONE
  for (i = 0; i <= 7; i++) {
    if ( PINB & (1 << DAT) ) { //1
      return (1 << i);
    } else { //0
      return (1 << i);
    }
  }
}

