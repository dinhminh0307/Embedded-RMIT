#include <avr/io.h>

int main(void) {
  DDRB |= (1 << 5);
  DDRD &= ~(1  << 2); // Use INT0 bit so must be exactly on the portD2
  TCCR1B |= (1 << WGM12); // Enable CTC  mode
  TCCR1B |= (1 << CS12); // set prescaler
  OCR1A = 15624;
  // Scale 1: Enable interupt bit output compare A match
  EIMSK |= (1 << INT0); // Set the external interupt input bit in port D2
  sei(); // Enable the global interupt bit 
  while (1) { // If the button is not pressed, blink with frequecny of 2 Hz
    if(TIFR1 & (1 << OCF1A)) {
      PORTB ^= ( 1 << 5);
      TIFR1 |= (1 << OCF1A);
    }
  }

}

ISR(INT0_vect) { // When button is pressed to 1 Hz
  OCR1A = 31249; // set top value to 31249 pulse = 1 HZ
  if(TIFR1 & (1 << OCF1A)) {
    PORTB ^= (1 <<5);
    TIFR1 |= (1 << OCF1A);
  }
}