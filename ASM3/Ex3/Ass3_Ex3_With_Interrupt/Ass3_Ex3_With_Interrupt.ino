#include <avr/io.h>
#include <avr/interrupt.h>
#define GPIO_1 0
#define GPIO_2 1

int half_cycle = 0;
volatile int buttonPressed = 0;

int main(void)
{
  /* PINs SETUP*/
	DDRD &= ~(1 << 2); // Set PD2 pin (INT0) as input to receive interrupt re-quest
  DDRB |= (1 << GPIO_1) | (1 << GPIO_2); //PORTB0 (GPIO_1) & PORTB1 (GPIO_2) as output

  /* EXTERNAL INTERRUPT SETUP */
	EICRA |= (1 << ISC01); // set INT0 to trigger on Falling edge
	EIMSK  |= (1 << INT0); // Turns on INT0

  /* TIMER1 SETUP */
  TCCR1B |= (1 << WGM12); //CTC Mode On
  TCCR1B |= (1 << CS11) | (1 << CS10); //Prescaler of 64
  OCR1A = 24999; //Duration: 0.1s
  TIMSK1 |= (1 << OCIE1A); //Output Compare Match A Interrupt On
	sei(); // Turn on the Global Interrupt Enable Bit

	while (1)
	{
	}
}

ISR ( TIMER1_COMPA_vect ) {
  if (buttonPressed == 1) { //button is pressed 
    half_cycle++;

    if (half_cycle == 1) { //GPIO_1 ON
      PORTB |= (1 << GPIO_1);
    } else if (half_cycle > 13) { //PORTB1 toggles full 6 cycles
      half_cycle = 0; //reset cycle to 0 for next button pressed
      buttonPressed = 0; //reset button

      PORTB &= ~(1 << GPIO_1); //GPIO_1 OFF
    } else { //PORTB1 toggles
      PORTB ^= (1 << GPIO_2);
    }
  }
  
} 

ISR ( INT0_vect )	// ISR for the INT0
{ 
  buttonPressed = 1;
}

