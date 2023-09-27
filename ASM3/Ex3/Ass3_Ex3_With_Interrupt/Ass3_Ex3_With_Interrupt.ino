#include <avr/io.h>
#include <avr/interrupt.h>

int half_cycle = 0;
volatile int buttonPressed = 0;

int main(void)
{
	DDRD &= ~(1 << 2); // Set PD2 pin (INT0) as input to receive interrupt re-quest
  DDRB |= (1 << 0) | (1 << 1); //PORTB0 & PORTB1 as output
	EICRA |= (1 << ISC01); // set INT0 to trigger on Falling edge
	EIMSK  |= (1 << INT0); // Turns on INT0

  TCCR1B |= (1 << WGM12); //CTC Mode On
  TCCR1B |= (1 << CS11) | (1 << CS10); //Prescaler of 64
  OCR1A = 24999; //Duration: NOT 0.1s
  TIMSK1 |= (1 << OCIE1A); //Output Compare Match A Interrupt On
	sei(); // Turn on the Global Interrupt Enable Bit

	while (1)
	{
	}
}

ISR ( TIMER1_COMPA_vect ) {
  if (buttonPressed == 1) { //button is pressed 
    half_cycle++;

    if (half_cycle == 1) { //PORTB0 ON
      PORTB |= (1 << 0);
    } else if (half_cycle > 33) { //PORTB1 toggles full 16 cycles
      half_cycle = 0; //reset cycle to 0 for next button pressed
      buttonPressed = 0; //reset button

      PORTB &= ~(1 << 0); //PORTB0 OFF
    } else { //PORTB1 toggles
      PORTB ^= (1 << 1);
    }
  }
  
} 

ISR ( INT0_vect )	// ISR for the INT0
{ 
  buttonPressed = 1;
	PORTB ^= (1<<PORTB5);	//Toggle the PB5 connecting to the LED
}

