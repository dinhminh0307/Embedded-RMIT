#include <avr/io.h>
/*Generate a square wave with the frequency of 1 Hz given the
input clock is 16 MHz. We will use normal mode (without interrupt)
and consider hardware prescaler

Target Timer COunter = 31249*/
int main(void) {
  DDRB |= (1 << 5); // Set port 5 as output
  TTCR1B |= (1 << CS12); // Set up timer to 256 prescaling

  while (1) {
    if (TCNT1 >= 31249) { // CHeck if the counter 1 count above the target timer count thn toggle the port 5 and reset the counter then toggle again when the condition again mathces
      PORTB ^= (1 << 5); // Toggle port 5
      TCNT1 = 0; // reset the counter

    }
  }
}