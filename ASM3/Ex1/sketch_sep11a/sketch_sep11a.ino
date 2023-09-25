#include <avr/io.h>
#include <avr/interrupt.h>

volatile bool buttonPressed = false;
volatile bool quarterFlag = false;
volatile bool halfFlag = false;
void btn_init() {
  DDRD &= ~(1 << 2);
  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0); // enbale port D2 as external interupt
  sei(); //Enable globale interupt bit
}

void timer0_init() {
  TCCR0B |= ((1 << CS01) | (1 << CS00)); // Enable to the 64 prescaler
  TCCR0A |= (1 << WGM01); //Set up TCT mode for timer 0
  TIMSK0 |= (1 << OCIE0A); //ENbale COmparematch A interupt
  OCR0A = 249;
}

void timer1_init() {
   // For TC1
  TCCR1B |= (1 << WGM12); // Setup CTC mode for timer 1
  TCCR1B |= ((1 << CS10) | (1 << CS11)); // Set prescaler for timer 1
  // TCCR1B |= ((1 << CS12) | (1 << CS10));
  TIMSK1 |= (1 << OCIE1A); // Enable COmp A interupt
  OCR1A = 24999;
}
int main(void) {
  DDRB |= (1 << 5);
  btn_init();
  timer0_init();
  timer1_init();
  while(1) {
    while(buttonPressed) {
      if(quarterFlag) {
        PORTB ^= (1 << 5);
        quarterFlag = false;
    }
  }
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
}

ISR(TIMER1_COMPA_vect) {
  quarterFlag = true;
}
ISR(TIMER0_COMPA_vect) {
  halfFlag = true;
}
