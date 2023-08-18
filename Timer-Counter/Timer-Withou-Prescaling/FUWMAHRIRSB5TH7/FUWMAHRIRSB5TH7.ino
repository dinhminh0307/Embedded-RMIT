// GPIO as output
 /*Masking technique, using bit manipulation*/
#ifndef F_CPU
#define F_CPU 16000000UL 
#endif
#include <avr/io.h> // Thi s header def i nes al l t he I nput / Out put Regi st er s f or al l AVR
#include <util/delay.h> /// Thi s header f i l e def i nes t wo f unct i ons, _del ay_ms ( mi l l i second

int main(void) {
  DDRB |= (1 << 5); //Set port B5 as output
  TCCR1B |= (1 << CS10); // TTCR1B used to set mode for timer, setup timer1 (16 bit) since 
  Serial.begin(9600); 
  while (1) {
    _delay_ms(10000);
    // Serial.println(TCNT1);
    if (TCNT1 >= 7999) { // TCNT1 will hold the value count incrementingly, if it counts up over the target time, toggle the port and reset timer
      
      PORTB ^= (1 << 5); // toggle the port B5
      TCNT1 = 0; // Reset timer coutner
    }
  }
}
