#include <avr/io.h>

// CTC mode demo
int main(void) {

  /*Old way:
  DDRB |= (1 << 5); //Set port B5 as output
  TCCR1B |= (1 << CS12); // TTCR1B used to set mode for timer, setup timer1 (16 bit) since 
  Serial.begin(9600); 
  while (1) {
    // Serial.println(TCNT1);
    if (TCNT1 >= 31248) { // TCNT1 will hold the value count incrementingly, if it counts up over the target time, toggle the port and reset timer
      //if the TTC above the prescaler, the LED dont blink
      PORTB ^= (1 << 5); // toggle the port B5
      TCNT1 = 0; // Reset timer coutner
    }
  }*/

  
  DDRB |= (1 << 5);
  TCCR1B |= (1 << CS12); // Set up the prescaler to 256
  // Enable CTC mode:
  TCCR1B |= (1 << WGM12); // TUrn the CTC mode for TCCR1B (wgm13 and wgm12)
  OCR1A = 31248; // Load TTC to OCR1A top value
  while (1) {
    // Check if the flag is set
    if (TIFR1 & (1 << OCF1A)) { // check if bit is set
      PORTB ^= (1 << 5); // toggle
      TIFR1 |= (1 << OCF1A); // clear the flag by writing 1 to this flag
    }
  }
}