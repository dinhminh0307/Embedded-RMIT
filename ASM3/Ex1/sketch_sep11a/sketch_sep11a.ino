#include <avr/io.h>
#include <avr/interrupt.h>

volatile bool buttonPressed = false;
volatile bool quarterFlag = false;
volatile bool halfFlag = false;
int main(void) {
  DDRB |= (1 << 5);
  DDRD &= ~(1 << 2);
  TCCR0B |= ((1 << CS01) | (1 << CS00)); // Enable to the 64 prescaler
  TCCR0A |= (1 << WGM01); //Set up TCT mode for timer 0
  
  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0); // enbale port D2 as external interupt
  TIMSK0 |= (1 << OCIE0A); //ENbale COmparematch A interupt
  OCR0A = 249;

  // For TC1
  TCCR1A |= (1 << WGM12); // Setup CTC mode for timer 1
  TCCR1B |= ((1 << CS10) | (1 << CS11)); // Set prescaler for timer 1
  TIMSK1 |= (1 << OCIE1A); // Enable COmp A interupt
  OCR1A = 24999;

  
  sei(); //Enable globale interupt bit

  while(1) {
    while(buttonPressed) {
      // PORTB |= (1 << 5);
      // _delay_ms(2000);
      // PORTB &= ~(1 << 5);
      // _delay_ms(2000);
      
      if(quarterFlag) {
        PORTB ^= (1 << 5);
        quarterFlag = false;
    }
  }
    // PORTB |= (1 << 5);
    // _delay_ms(1);
    // PORTB &= ~(1 << 5);
    // _delay_ms(1);
    if(halfFlag) {
    PORTB ^= (1 << 5);
    halfFlag = false;
    }
    
  }
}

ISR(INT0_vect) {
  if(buttonPressed) {
    buttonPressed = false;
  }
  else {
    buttonPressed = true;
  }
  //  PORTB |= (1 << 5);
  //     _delay_ms(2000);
  //     PORTB &= ~(1 << 5);
  //     _delay_ms(2000);
}

ISR(TIMER1_COMPA_vect) {
  quarterFlag = true;
}
ISR(TIMER0_COMPA_vect) {
  halfFlag = true;
}
