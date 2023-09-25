#ifndef F_CPU
#define F_CPU 16000000UL  // Set 16 MHz clock speed
#endif

#include <avr/io.h>
#include <avr/delay.h>
#define USART_BAUDRATE 9600
//Take the formula from datasheet 
#define BAUD_PRESCALE ((( F_CPU / 16) / ( USART_BAUDRATE ))- 1)
#define HRS_READ_REG 0x85
#define SCLK 3
#define CE 1
#define DAT 5
#define OSC 7
#define TIME_VAL 0x24
volatile bool buttonPressed = false;

//Initialize the external interupt
void btn_init() {
  DDRD &= ~(1 << 2); //Set port d2 as input
  EICRA |= (1 << ISC01); // Detect the falling edge
  EIMSK |= (1 << INT0); // enable exint in portd2
  sei(); // Enalble global interupt
}

// Algorithm to read byte for the DS1302
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

// Function to display the signal to OSC
void OSC_ReadData(uint8_t ctr) {
  DDRB |= (1 << OSC);
  for(int i = 0; i < 8; i++) {
        if((ctr >> i) & 0x01) {
          PORTD |= (1 << OSC);
        }
        else {
          PORTD &= ~(1 << OSC);
        }
      }
}

// Function to get data from the DS1302
uint8_t RTC_ReadResponseByte() {
  DDRB &= ~(1 << DAT);
  uint8_t byte = readByte();
  return byte;
}

// THis algorithm used to send byte to DS1302
void writeByte(uint8_t dat) {
  for(int i = 0; i < 8; i++) {
    if((dat >> i) & 0x01) {
      PORTB |= (1 << DAT);
    }
    else {
      PORTB &= ~(1 << DAT);
    }
    //TRigger CLK on Rising Edge
    PORTB |= (1 << SCLK);
    _delay_ms(1);
    PORTB &= ~(1 << SCLK);
  }
}

// This function used to send command byte to DS1302
void RTC_WriteCommandByte(uint8_t dat) {
  PORTB &= ~(1 << DAT);
  writeByte(dat);
}

// This function used to initialize the DS1302
void RTC_init() {
  // Set all port as output
  DDRB |= (1 << CE);
  PORTB &= ~(1 << CE);

  DDRB |= (1 << DAT);
  PORTB &= ~(1 << DAT);

  DDRB |= (1 << SCLK);
  PORTB &= ~(1 << SCLK);
}

//THis function used to enable the CE pin of the DS1302 when sending data
void CE_Enable() {
  PORTB |= (1 << CE);
}
//This function used to disable the CE pin after getting data
void CE_Disable() {
  PORTB &= ~(1 << CE);
}
// This function to convert 4 bit to Dec to calculate the Decimal value of the hour register
int convert4BitToDec(uint8_t temp) {
  int counter = 0;
  for(int i = 0; i < 4; i++) {
    if((temp >> i) & 0x01) {
      if(i == 0) {
        counter += 1;
      }
      else if(i == 1) {
        counter += 2;
      }
      else if(i == 2) {
        counter += 4;
      }
      else if(i == 3) {
        counter += 8;
      }
    }
    else {
      continue;
    }
  }
  return counter;
}

// Function to convert BCD to Decimal
int convertBCDtoDec(uint8_t ctr) {
  uint8_t tmp1, tmp2;
  for(int i = 0; i < 8; i++) { // this alogrithm used to split byte to 2 temp of 4 bit
    if(i <= 4) {
      tmp1 |= (ctr >> i);
    }
    if(i >= 4) {
      tmp2 |= (ctr >> i);
    }
  }
  return convert4BitToDec(tmp1) + convert4BitToDec(tmp2);
}
int main(void) {
  DDRD |= (1 << OSC);
  Serial.begin(9600);
  RTC_init();
  btn_init();
  while(1) {
    uint8_t ctr;
    if (buttonPressed) {
      CE_Enable(); // Enable CE before writing command byte
      RTC_WriteCommandByte(HRS_READ_REG); // Send command byte to trigger Hour register
      ctr = RTC_ReadResponseByte(); // get hour data form DS1302 to variable
      OSC_ReadData(ctr); // Display the receive data to OSC
      CE_Disable(); // Disable the CE pin
      Serial.println(convertBCDtoDec(ctr)); // Print the data from hour to the serial monitor
    }
    PORTD &= ~(1 << OSC);
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