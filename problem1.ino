#include <avr/io.h>
#include <avr/interrupt.h>

volatile bool buttonPressed = false;
volatile bool frequency_1 = false; //500Hz (1ms ON, 1ms OFF)
volatile bool frequency_2 = false; //5Hz (100ms ON, 100ms OFF)
void btn_init() {
  DDRD &= ~(1 << 2);
  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0); // Enable port D2 as external interrupt
  sei(); //Enable global interrupt bit
}

void timer0_init() {
  TCCR0B |= ((1 << CS01) | (1 << CS00)); // Enable to the 64 prescaler
  TCCR0A |= (1 << WGM01); //Set up TCT mode for timer 0
  TIMSK0 |= (1 << OCIE0A); //Enable Compare Match A Interrupt
  OCR0A = 249;
}

void timer1_init() {
   // For TC1
  TCCR1B |= (1 << WGM12); // Setup CTC mode for timer 1
  TCCR1B |= ((1 << CS10) | (1 << CS11)); // Set prescaler for timer 1
  // TCCR1B |= ((1 << CS12) | (1 << CS10));
  TIMSK1 |= (1 << OCIE1A); // Enable Compare A interrupt
  OCR1A = 24999;
}

int main(void) {
  DDRB |= (1 << 5); //PORTB5 Output
  btn_init();
  timer0_init();
  timer1_init();
  while(1) {
    while(buttonPressed) { //buttonPressed true - 500Hz
      if(frequency_2) {
        PORTB ^= (1 << 5);
        frequency_2 = false;
    }
  }
  //buttonPressed false - 5Hz
    if(frequency_1) { 
    PORTB ^= (1 << 5);
    frequency_1 = false;
    }
  }
}

ISR( INT0_vect ) {
  if(buttonPressed) {
    buttonPressed = false;
  }
  else {
    buttonPressed = true;
  }
}

ISR( TIMER1_COMPA_vect ) {
  frequency_2 = true;
}

ISR( TIMER0_COMPA_vect ) {
  frequency_1 = true;
}