#include <AceButton.h>
using namespace ace_button;

// pins
#define LED_GREEN 8
#define LED_RED 9
#define BUT1_PIN 2
#define BUT2_PIN 3
#define BUT3_PIN 4
#define BUT4_PIN 5
#define BUT5_PIN 6

//buttons id
#define BUT1_ID 0
#define BUT2_ID 1
#define BUT3_ID 2
#define BUT4_ID 3
#define BUT5_ID 4

AceButton button1(BUT1_PIN, HIGH, BUT1_ID);
AceButton button2(BUT2_PIN, HIGH, BUT2_ID);
AceButton button3(BUT3_PIN, HIGH, BUT3_ID);
AceButton button4(BUT4_PIN, HIGH, BUT4_ID);
AceButton button5(BUT5_PIN, HIGH, BUT5_ID);

#define KEY_LEFT_ARROW  0x50
#define KEY_RIGHT_ARROW 0x4F
#define KEYPAD_SUBTRACT 0x56
#define KEY_SPACE       44
#define KEYPAD_ENTER    0x58
#define KEYPAD_0        0x62
#define KEYPAD_ADD      0x57
#define KEY_RIGHT_SHIFT 0xE5
#define KEY_M           16
#define KEY_B           5
#define KEY_COMMA       54
#define KEYPAD_MULTIPLY 0x55
#define KEY_Y           28
#define KEY_LEFT_ALT    0xE2

#define PRESS_DELAY 10

enum speed_t : uint8_t {
  P100 = 0,
  P75  = 1,
  P50  = 2
};
speed_t speed = P100; // 0 100%, 1 75%, 2 50%

bool playing = false;
bool recording = false;

enum preset_t : uint8_t {
  YOUTUBE = 0,
  CUBASE = 1
};

preset_t preset = YOUTUBE;

void handleButtons(AceButton*, uint8_t, uint8_t);

void setup() {
 
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUT1_PIN, INPUT_PULLUP);
  pinMode(BUT2_PIN, INPUT_PULLUP);
  pinMode(BUT3_PIN, INPUT_PULLUP);
  pinMode(BUT4_PIN, INPUT_PULLUP);
  pinMode(BUT5_PIN, INPUT_PULLUP);

  button1.setEventHandler(handleButtons);
  button2.setEventHandler(handleButtons);
  button3.setEventHandler(handleButtons);
  button4.setEventHandler(handleButtons);

  // button5 have long press to change preset
  ButtonConfig* buttonConfig = button5.getButtonConfig();
  buttonConfig->setEventHandler(handleButtons);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);

  // Start the Serial which is connected with the USB MCU.
  Serial.begin(115200);

  // default youtube green
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, LOW); 

}

void loop() {
  button1.check();
  button2.check();
  button3.check();
  button4.check();
  button5.check();
}

void press(char key) {
  Serial.write('K');
  Serial.write(key);
}

void releaseAll(void) {
  delay(PRESS_DELAY);
  Serial.write('R');
}

#define DBG_BUF_LEN 20

void serialdebug(char *buffer) {
  size_t l = strlen(buffer);
  if (l > DBG_BUF_LEN)
    l = DBG_BUF_LEN;
  
  Serial.write('D');
  Serial.write(l);
  Serial.write(buffer, l);  
}

void handleButtons(AceButton* button, uint8_t eventType, uint8_t /* buttonState */) {
  switch (eventType) {
    case AceButton::kEventPressed:
      switch(button->getId()) {        
        case BUT1_ID:   // BACK
          switch(preset) {
            case YOUTUBE:
              press(KEY_LEFT_ARROW);
              releaseAll();
              serialdebug("BACK");
              break;
            case CUBASE:
              press(KEYPAD_SUBTRACT);
              releaseAll();
              serialdebug("BACK");
              break;
          }          
          break;
        case BUT2_ID:   // PLAY/PAUSE PLAY/STOP
          switch(preset) {
            case YOUTUBE:
              press(KEY_SPACE);
              releaseAll();
              serialdebug("PLAY/PAUSE");
              break;
            case CUBASE:
              if (!playing && !recording) {
                press(KEYPAD_ENTER);
                releaseAll();
                serialdebug("PLAY");
                playing = true;
              } else {
                press(KEYPAD_0);
                releaseAll();
                serialdebug("STOP");
                if (playing)
                  playing = false;
                if (recording)
                  recording = false;
              }
              break;
          }          
          break;
        case BUT3_ID:   // FWD
          switch(preset) {
            case YOUTUBE:
              press(KEY_RIGHT_ARROW);
              releaseAll();
              serialdebug("FWD");
              break;
            case CUBASE:
              press(KEYPAD_ADD);
              releaseAll();
              serialdebug("FWD");
              break;
          }
          break;        
        case BUT4_ID:  // SPEED RECORD
          switch(preset) {
            case YOUTUBE:
              speed = speed + 1;
              if (speed > P50)
                speed = P100; //reset          
              if (speed == P75) {                        
                press(KEY_RIGHT_SHIFT);
                press(KEY_M );
                releaseAll();
                serialdebug("SPD75");
              } else if (speed == P50) {
                press(KEY_RIGHT_SHIFT);
                press(KEY_M );
                releaseAll();
                serialdebug("SPD50");
              } else if (speed == P100) {        
                press(KEY_RIGHT_SHIFT);
                press(KEY_COMMA);
                releaseAll();
                press(KEY_RIGHT_SHIFT);
                press(KEY_COMMA);
                releaseAll();
                serialdebug("SPD100");                
              }
              break;
            case CUBASE:
              press(KEYPAD_MULTIPLY);
              releaseAll();
              serialdebug("RECORD");
              recording = true;
              break;
          }
          break;        
      }
      break;
    case AceButton::kEventClicked:
      switch(button->getId()) {
        case BUT5_ID:   // LOOP REWIND
          switch(preset) {
            case YOUTUBE:
              press(KEY_Y);
              releaseAll();
              serialdebug("LOOP");
              break;
            case CUBASE:
              press(KEY_RIGHT_SHIFT);
              press(KEY_B);
              releaseAll();
              serialdebug("REWIND");
              break;
          }
          break;
      }
      break;
    case AceButton::kEventLongPressed:
      switch(button->getId()) {
        case BUT5_ID:  // MODE
          if (preset == YOUTUBE) {
            preset = CUBASE;
            playing = false;
            recording = false;
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_RED, HIGH); 
            serialdebug("PRESET CUBASE");
          } else {
            preset = YOUTUBE;
            speed = P100;
            digitalWrite(LED_GREEN, HIGH);
            digitalWrite(LED_RED, LOW); 
            serialdebug("PRESET YOUTUBE");
          }
          break;
      }
      break;
  }
  Serial.flush();
  delay(PRESS_DELAY);
}
