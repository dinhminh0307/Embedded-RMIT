#include <avr/io.h>

// CTC mode demo
int main(void) {

  DDRB |= (1 << 5);
  TCCR1B |= (1 << CS12); // Set up the prescaler to 256
  // Enable CTC mode:
  TCCR1B |= (1 << WGM12); // TUrn the CTC mode for TCCR1B (wgm13 and wgm12)
  OCR1A = 31248; // Load TTC to OCR1A top value

  TIFR1 |= ( 1 << OCFA);
  // Enable CTC interupt mode
  // Level 1 particular type
  TIMSK1 |= (1 << OCEIA);
  // Level 2 master interupt bit
  seil();
  while (1) {
    // Check if the flag is set
    if (TIFR1 & (1 << OCF1A)) { // check if bit is set
      PORTB ^= (1 << 5); // toggle
      TIFR1 |= (1 << OCF1A); // clear the flag by writing 1 to this flag
    }
  }
}

ISR (TIMER1_COMPA vect) {
  PORTB ^= (1 << 5);
}