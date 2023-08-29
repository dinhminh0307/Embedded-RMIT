#include <avr/io.h>
#include <avr/interrupt.h>
volatile int tick = 1;
volatile bool half_flag = false;
volatile bool quarter_flag = false;

int main(void)
{
  DDRB |= (1 << DDB5); // Set LED as output
  DDRD &= ~(1<<DDB2);  //Button connected to PORTD2 (pull-up)
  TCCR1B |= (1 << WGM12); // Turn on the CTC mode for Timer 1
  TCCR1B |= (1 << CS12 ); // Set up Timer 1 with the prescaler of 256
  //Assumption - CTC, prescaler 256, target f = 2 Hz
  OCR1A = 15624;    // Set CTC compare value to 1Hz at 16 MHz AVR clock , with a prescaler of 256
  TIMSK1 = 1<<OCIE1A;     // Enable Output Compare A Match Interrupt
  sei();        //Enable the Global Interrupt Bit
  while (1)
  {
      if (!(PIND & (1<<PIND2)))   //if button is pressed
      {
         if(half_flag)  //When 0.5 seconds
           {
            PORTB ^= (1 << 5); // Toggle the LED to have 1Hz
            half_flag = false;
           }
      }
      else  //Button is not pressed
      {
         if (quarter_flag)
        {
          PORTB ^= (1<<5);      //Toggle the LED to have 2 Hz
          quarter_flag = false;
        }         
      } 
  }
}

ISR ( TIMER1_COMPA_vect )  //When Compare Match - 0.25s elapsed
{
  quarter_flag = true;   
  if (tick == 2) // 2 x 0.25 = 0.5 second
  {
    half_flag = true;    // Indicate has 0.5s
    tick =1;
  }
  else 
  {
    tick++;
  }
}
