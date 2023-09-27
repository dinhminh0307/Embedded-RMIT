#include <avr/io.h>
#include <avr/interrupt.h>

volatile bool buttonPressed = false;
volatile bool frequency_1 = false; //500Hz (1ms ON, 1ms OFF)
volatile bool frequency_2 = false; //5Hz (100ms ON, 100ms OFF)
void btn_init() {
  DDRD &= ~(1 << 2);
  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0); // enbale port D2 as external interupt
  sei(); //Enable globale interupt bit
}

void timer0_init() {
  // TCCR0B |= ((1 << CS01) | (1 << CS00)); // Enable to the 64 prescaler
  // TCCR0A |= (1 << WGM01); //Set up TCT mode for timer 0
  // TIMSK0 |= (1 << OCIE0A); //ENbale COmparematch A interupt
  // OCR0A = 249;

  /*Change from 500 to 220 Hz*/
   TCCR0B |= ((1 << CS02)); // Enable to the 256 prescaler
   TCCR0A |= (1 << WGM01); //Set up TCT mode for timer 0
   TIMSK0 |= (1 << OCIE0A); //ENbale COmparematch A interupt
   OCR0A = 141;
}

void timer1_init() {
  //  // For TC1
  // TCCR1B |= (1 << WGM12); // Setup CTC mode for timer 1
  // TCCR1B |= ((1 << CS10) | (1 << CS11)); // Set prescaler for timer 1
  // // TCCR1B |= ((1 << CS12) | (1 << CS10));
  // TIMSK1 |= (1 << OCIE1A); // Enable COmp A interupt
  // OCR1A = 24999;

  /*Change from 5 to 3 Hz*/
  TCCR1B |= (1 << WGM12); // Setup CTC mode for timer 1
  TCCR1B |= ((1 << CS10) | (1 << CS11)); // Set prescaler for timer 1
  TIMSK1 |= (1 << OCIE1A); // Enable COmp A interupt
  OCR1A = 41665;
}

int main(void) {
  DDRB |= (1 << 5);
  btn_init();
  timer0_init();
  timer1_init();
  while(1) {
    while(buttonPressed) {
      if(frequency_2) {
        PORTB ^= (1 << 5);
        frequency_2 = false;
    }
  }
    if(frequency_1) {
    PORTB ^= (1 << 5);
    frequency_1 = false;
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
  frequency_2 = true;
}
ISR(TIMER0_COMPA_vect) {
  frequency_1 = true;
}
