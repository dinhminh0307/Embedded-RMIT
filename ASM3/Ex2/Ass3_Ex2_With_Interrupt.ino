#include <avr/io.h>
#include <avr/interrupt.h>

volatile int interval = 1;
volatile int duration = 1; //1: 0.125s; 2: 0.25s; 3: 0.375s
volatile int direction = 1; //1: PORTB0 to PORTB2; 2: PORTB2 to PORTB0; 
int noCount = 0;

int main(void)
{
  DDRD |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7); //Output 4 Testing LEDs
  PORTD |= (1 << 4);

	DDRB |= (1 << 5); // Set PORTB5 as output for LED - for TESTING BUTTON
	DDRD &= ~(1 << 2); //PD2 (INT0) - Gear button
  DDRD &= ~(1 << 3); //PD3 (INT1) - Direction button
  DDRB |= (1 << 0) | (1 << 1) | (1 << 2); //3 LEDs - PORTB0 to PORTB2
	EICRA |= (1 << ISC01) | (1 << ISC11);   // set INT0 to trigger on ANY logic change
	EIMSK  |= (1 << INT0) | (1 << INT1);   // Turns on INT0

  //CTC Timer 1 with Interrupt Mode
  TCCR1B |= (1 << WGM12); //CTC Mode On
  TCCR1B |= (1 << CS11) | (1 << CS10); //Prescaler of 64
  OCR1A = 31249; //Duration: 0.125s
  TIMSK1 |= (1 << OCIE1A); //Output Compare Match A Interrupt On
	sei();                   // Turn on the Global Interrupt Enable Bit
	while (1)
	{
	}
}

ISR ( TIMER1_COMPA_vect) {
  noCount++; //Software Prescaler. Explanation: duration 2 = 2 * duration 1; duration = 3 * duration 1

  if (noCount >= duration) {
    noCount = 0;

    if (direction == 1) {
      //LED sweeping PORTB0 to PORTB2
      switch (interval) {
        case 1:
          PORTB |= (1 << 0); //PORTB0 ON
          PORTB &= ~((1 << 1) | (1 << 2)); //PORTB1 & PORTB2 OFF
          interval++;
          break;
        case 2:
          PORTB |= (1 << 1); //PORTB0 ON
          PORTB &= ~((1 << 0) | (1 << 2)); //PORTB1 & PORTB2 OFF
          interval++;
          break;
        case 3:
          PORTB |= (1 << 2); //PORTB0 ON
          PORTB &= ~((1 << 0) | (1 << 1)); //PORTB1 & PORTB2 OFF
          interval = 1; //reset to interval 1
          break;
      }
    } else {
      //LED sweeping PORTB0 to PORTB2
      switch (interval) {
        case 1:
          PORTB |= (1 << 2); //PORTB0 ON
          PORTB &= ~((1 << 0) | (1 << 1)); //PORTB1 & PORTB2 OFF
          interval++;
          break;
        case 2:
          PORTB |= (1 << 1); //PORTB0 ON
          PORTB &= ~((1 << 0) | (1 << 2)); //PORTB1 & PORTB2 OFF
          interval++;
          break;
        case 3:
          PORTB |= (1 << 0); //PORTB0 ON
          PORTB &= ~((1 << 1) | (1 << 2)); //PORTB1 & PORTB2 OFF
          interval = 1; //reset to interval 1
          break;
      }
    }
    
  }
}

ISR(INT0_vect)	// ISR for the INT0
{
	PORTB ^= (1<<PORTB5);	//Toggle the PB5 connecting to the LED

  duration++;

  if (duration > 3) { //loop through all possible values, return to the start
    duration = 1; //reset duration back to duration 1
  }
}

ISR(INT1_vect)	// ISR for the INT0
{
	PORTB ^= (1<<PORTB5);	//Toggle the PB5 connecting to the LED

  direction++; 

  if (direction > 2) { //loop through all possible values, return to the start
    direction = 1; //reset direction back direction 1
  }
}
