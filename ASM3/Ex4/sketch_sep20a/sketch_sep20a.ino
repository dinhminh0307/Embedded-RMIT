#ifndef F_CPU
#define F_CPU 16000000UL  // Set 16 MHz clock speed
#endif

#include <avr/io.h>
#define USART_BAUDRATE 9600
//Take the formula from datasheet 
#define BAUD_PRESCALE ((( F_CPU / 16) / ( USART_BAUDRATE ))- 1)
#define MIN_READ 0x85
#define SCLK 3
#define CE 1
#define DAT 5
volatile bool buttonPressed = false;
void CE_Enable();
void sendCommandByte(uint8_t commandByte);
void SCLK_Triggered();
void CE_Disable();
void sendBit(int tmp);
int main(void) {
  DDRB |= (1 << CE);
  DDRB |= (1 << SCLK);
  DDRD &= ~(1 << 2); //Set port d2 as input
  EICRA |= (1 << ISC10); // Detect the falling edge
  EIMSK |= (1 << INT0); // enable exint in portd2
  sei(); // Enalble global interupt
  while(1) {
    if (buttonPressed) {
      CE_Enable();
      sendCommandByte(MIN_READ);
      CE_Disable();
    }
  }
}

void sendCommandByte(uint8_t commandByte) {
  for(int i = 0; i < 8;i++) {
    int singleBit = (commandByte & (1 << i));
    sendBit(singleBit);
    SCLK_Triggered();
  }
}
void SCLK_Triggered() {
  PORTB |= (1 << SCLK);
  PORTB ^= (1 << SCLK);
}
void sendBit(int tmp) {
  if(tmp != 0) {
    PORTB |= (1 << DAT);
  }
  else {
    PORTB &= ~(1 << DAT);
  }
}
void CE_Enable() {
  PORTB |= (1 << CE);
}

void CE_Disable() {
  PORTB &= ~(1 << CE);
}
ISR(INT0_vect) {
  buttonPressed = true;
}