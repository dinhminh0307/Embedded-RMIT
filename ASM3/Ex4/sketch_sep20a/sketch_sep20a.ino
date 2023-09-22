#ifndef F_CPU
#define F_CPU 16000000UL  // Set 16 MHz clock speed
#endif

#include <avr/io.h>
#include <avr/delay.h>
#define USART_BAUDRATE 9600
//Take the formula from datasheet 
#define BAUD_PRESCALE ((( F_CPU / 16) / ( USART_BAUDRATE ))- 1)
#define MIN_READ 0x85
#define SCLK 3
#define CE 1
#define DAT 5
#define OSC 4
volatile bool buttonPressed = false;

void btn_init() {
  DDRD &= ~(1 << 2); //Set port d2 as input
  EICRA |= (1 << ISC01); // Detect the falling edge
  EIMSK |= (1 << INT0); // enable exint in portd2
  sei(); // Enalble global interupt
}

uint8_t readByte() {
  uint8_t tmp;
  for(int i = 0; i < 8; i++) {
    if(PINB & (1 << DAT)) {
      tmp |= (1 << i);
    }
    // Falling Edge of the CLK
    PORTB |= (1 << SCLK);
    _delay_ms(1);
    PORTB &= ~(1 << SCLK);
  }
  return tmp;
}

void OSC_ReadData(uint8t_t ctr) {
  DDRB |= (1 << OSC);
  for(int i = 0; i < 8; i++) {
        if((ctr >> i) & 0x01) {
          PORTB |= (1 << OSC);
        }
        else {
          PORTB &= ~(1 << OSC);
        }
      }
}

uint8_t RTC_ReadResponseByte() {
  DDRB &= ~(1 << DAT);
  uint8_t byte = readByte();
  return byte;
}
void writeByte(uint8_t dat) {
  for(int i = 0; i < 8; i++) {
    if((dat >> i) & 0x01) {
      PORTB |= (1 << DAT);
    }
    else {
      PORTB &= ~(1 << DAT);
    }
    //TRigger CLK on Rising Edge
    PORTB &= ~(1 << SCLK);
    _delay_ms(1);
    PORTB |= (1 << SCLK);
  }
}

void RTC_WriteCommandByte(uint8_t dat) {
  PORTB &= ~(1 << DAT);
  writeByte(dat);
}


void RTC_init() {
  DDRB |= (1 << CE);
  PORTB &= ~(1 << CE);

  DDRB |= (1 << DAT);
  PORTB &= ~(1 << DAT);

  DDRB |= (1 << SCLK);
  PORTB &= ~(1 << SCLK);
}

void CE_Enable() {
  PORTB |= (1 << CE);
}

void CE_Disable() {
  PORTB &= ~(1 << CE);
}

int main(void) {
   UBRR0H = ( BAUD_PRESCALE >> 8); // Load upper 8- bits of the baud rate value into the high byte of the UBRR0H register
   UBRR0L = BAUD_PRESCALE ; // Load lower 8- bits of the baud rate value into the low byte of the UBRR0L register
   //Configure data format for transimission
   UCSR0C |= (1 << UCSZ00 ) | (1 << UCSZ01 ); // Use 8- bit character sizes
   UCSR0B |= (1 << RXEN0 ) | (1 << TXEN0 );

  RTC_init();
  btn_init();
  while(1) {
    uint8_t ctr;
    if (buttonPressed) {
      CE_Enable();
      RTC_WriteCommandByte(MIN_READ);
      ctr = RTC_ReadResponseByte();
      OSC_ReadData(ctr);
      CE_Disable();
    }
  }
}


ISR(INT0_vect) {
  buttonPressed = true;
}