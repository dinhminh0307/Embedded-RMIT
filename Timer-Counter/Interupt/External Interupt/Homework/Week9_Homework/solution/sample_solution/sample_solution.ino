#include <avr/io.h>
#include <avr/interrupt.h>

volatile bool halfsec_elapsed = false;
volatile bool button_pressed = false;

int main(void) 
{
  DDRB = 0xFF;
  DDRC |= (1<<0);
  DDRD &= ~(1<<2); // INT0 external interrupt
  
  //Display number 5  
  PORTC |= (1<<0);    
  PORTB = B00101101;
  
  //enable interrupts
  EIMSK |= (1<<INT0); //enable PORTD2 as external interrupt
  EICRA |= (1<<ISC01); // Detect on falling edge
  EIFR |= (1<<INTF0); // Clear flag

  TCCR1B |= (1 << WGM12); // Turn on the CTC mode for Timer 1
  TCCR1B |= (1 << CS12 ); // Set up Timer 1 with the prescaler of 256
  OCR1A = 31249;    // Set CTC compare value to 1Hz at 16 MHz AVR clock , with a prescaler of 256
  sei();        //Enable the Global Interrupt Bit
  
  int count = 0; //initialize couting value
  
  while (1){
    if (button_pressed)      { // button_pressed 1 pressed
    PORTB = 0x00;
    PORTC &= ~(1<<0);        
        while (count < 20) { // pattern 2 flashes as 1Hz frequency 10 times
        if (halfsec_elapsed)
        { 
          if (PORTB == B00000110) 
          { 
            PORTB = 0x00;
            PORTC &= ~(1<<0);
          }
          else 
          {
            PORTB = B00000110;
            PORTC |= ~(1<<0);            
          }
          count++;
          halfsec_elapsed = false;
        }
      }    
    count = 0; // reset count value
    // change to default pattern - Number 5
    PORTC |= (1<<0);    
    PORTB = B00101101;
    button_pressed = false;
    }     
  }
}

ISR (INT0_vect) 
{ 
  button_pressed = true;
  TCNT1 = 0;  
  TIMSK1 = 1<<OCIE1A;     // Enable Output Compare A Match Interrupt
}
  
ISR ( TIMER1_COMPA_vect )
{
  
  halfsec_elapsed = true; 
}
