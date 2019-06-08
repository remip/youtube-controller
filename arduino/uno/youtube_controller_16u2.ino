#include "HID-Project.h"
#include <avr/pgmspace.h>

//#define DBG

void setup() {
  // Start the Serial1 which is connected with the IO MCU.
  Serial1.begin(115200);

  // Sends a clean report to the host. This is important on any Arduino type.
  Keyboard.begin();

  // Start the USB Serial for debugging
  Serial.begin(115200);

}

#define READ_WAIT 10

char readbyte() {
  while(true) {
    if (Serial1.available() > 0)
      return Serial1.read();    
    delay(READ_WAIT);
  }
}

#define DBG_BUF_LEN 20
char buffer[DBG_BUF_LEN+1] = {'\0'};  // max msg length


void readbuffer(uint8_t l) {
  uint8_t c = 0;
  if (l > DBG_BUF_LEN)
    l = DBG_BUF_LEN;
  while(true) {
    if (Serial1.available() >= l) {
      Serial1.readBytes(buffer, l);
      buffer[l] = 0;
      return;
    }
    if(c++ == 10) {
      Serial.print(" ERROR x10 need=");
      Serial.print(l);
      Serial.print(" Available=");
      Serial.println(Serial1.available());      
    }

    delay(READ_WAIT);
  }
}


void getmsg() {
  char op;
  uint8_t k;
    
  op = readbyte();
#ifdef DBG 
  Serial.print("OP ");
  Serial.print(op);
#endif
  switch(op) {
    case 'K':
      // press key      
      k = readbyte();
#ifdef DBG       
      Serial.print(" Press key ");
      Serial.println(k);
#endif  
      Keyboard.press((KeyboardKeycode ) k);
      break;
    case 'R':
      // release all key
#ifdef DBG       
      Serial.println(" Release key");
#endif      
      Keyboard.releaseAll();
      break;
    case 'D':
      // serial debug
      k = readbyte();
#ifdef DBG       
      Serial.print(" DBGMSG len ");
      Serial.print(k);
      Serial.print(" ");
#endif
      readbuffer(k);
      Serial.println(buffer);      
      break;
  }
}

unsigned long last_time = millis();

void loop() {
  // read messages from the 328

  if (Serial1.available() > 0)
    getmsg();

  delay(READ_WAIT);
}
