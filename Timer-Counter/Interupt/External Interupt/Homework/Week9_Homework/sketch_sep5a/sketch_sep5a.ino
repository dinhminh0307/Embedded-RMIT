#include <avr/io.h>
#define NUMBER_5 0b1101101
#define NUMBER_1 0b0000110
int main(void) {
  DDRB |= 0b11111111;
  DDRD &= ~(1 << 2);
  TCCR1B |= (1 << WGM12); // Enable TCT mode for TCCR1B
  TCCR1B |= (1 << CS12); // set prescaler to 256
  EICRA |= (1 << ISC11) | (1 << ISC01); // set INT0, INT1 to trigger on falling edge
  EIMSK |= (1 << INT0); // Set interupt frag on POrt D2
  seil(); // Enable global interupt frag

  while(1) {
    // When button is not pressed, always display number 5
    PORTB |= ~NUMBER_5;
  }
}
ISR (INT0_vect) { // when  the button is pressed, display number 1 and blink number 1 with nested interupt
  PORTB |= ~NUMBER_1;
  TIMSK1 |= (1 << OCIEA); // enalbel CTC with interupt to blink number 1
  seil(); // enalbler global interupt frag
}

ISR(TIMER1_COMPA_vect) { // set time delay for number 1 with interupt, then go back to INT0 interupt
  PORTB |= NUMBER_1;
}