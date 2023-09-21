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
// void CE_Enable();
// void sendCommandByte(uint8_t commandByte);
// void toggleData(int tmp)
// // void SCLK_Triggered();
// void CE_Disable();
// void readByte();
// void sendBit(int tmp);
int main(void) {
   UBRR0H = ( BAUD_PRESCALE >> 8); // Load upper 8- bits of the baud rate value into the high byte of the UBRR0H register
   UBRR0L = BAUD_PRESCALE ; // Load lower 8- bits of the baud rate value into the low byte of the UBRR0L register
   //Configure data format for transimission
   UCSR0C |= (1 << UCSZ00 ) | (1 << UCSZ01 ); // Use 8- bit character sizes
   UCSR0B |= (1 << RXEN0 ) | (1 << TXEN0 );

  DDRB |= (1 << CE);
  DDRB |= (1 << SCLK);
  DDRB |= (1 << DAT);
  DDRB |= (1 << 4);
  DDRD &= ~(1 << 2); //Set port d2 as input
  EICRA |= (1 << ISC01); // Detect the falling edge
  EIMSK |= (1 << INT0); // enable exint in portd2
  sei(); // Enalble global interupt
  while(1) {
    if (buttonPressed) {
      sendCommandByte(MIN_READ);
      buttonPressed = false;
    }
    
  }
}

// void readByte() {
//   while (( UCSR0A & (1 << RXC0 )) == 0) {};
//   int recievedByte = UDR0;
//   for(int i = 0; i < 8; i ++) {
//     int singleBit = (recievedByte & (1 << i));
//     toggleData(singleBit);
//   }
// }
// void toggleData(int tmp) {
//   PORTB |= (1 << 4);
//   PORTB ^= (1 << 4);
// }


void sendCommandByte(uint8_t commandByte) {
  CE_Enable();
  for(int i = 0; i < 8;i++) {
    int singleBit = (commandByte | (1 << i));
    sendBit(singleBit);
    SCLK_Triggered();
  }
  CE_Disable();
}
void SCLK_Triggered() {
  PORTB |= (1 << SCLK);
  PORTB |= (1 << 4);
  PORTB ^= (1 << SCLK);
  PORTB ^= (1 << 4);
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