#include "AnalogReadNow.h"

//#define DEBUG_OUTPUT
//#define DEBUG_OUTPUT_LIVE
//#define DEBUG_TIME
//#define DEBUG_DATA

//#define ENABLE_KEYBOARD
#define ENABLE_NS_JOYSTICK

//#define HAS_BUTTONS

#ifdef ENABLE_KEYBOARD
#include <Keyboard.h>
#endif

#ifdef ENABLE_NS_JOYSTICK
#include "Joystick.h"
const int led_pin[4] = {8, 9, 10, 11};
const int sensor_button[4] = {SWITCH_BTN_ZL, SWITCH_BTN_LCLICK, SWITCH_BTN_RCLICK, SWITCH_BTN_ZR};
#endif

#ifdef HAS_BUTTONS
int button_state[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int button_cd[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#ifdef ENABLE_KEYBOARD
const int button_key[16] = {
  KEY_UP_ARROW, KEY_RIGHT_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW,
  'k', 'j', 'f', 'd',
  KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_ESC, ' ',
  KEY_F1, 'q', 0 /*Fn1*/, 0 /*Fn2*/
};
#endif
#ifdef ENABLE_NS_JOYSTICK
const int button[16] = {
  0 /*SWITCH_HAT_U*/, 0 /*SWITCH_HAT_R*/, 0 /*SWITCH_HAT_D*/, 0 /*SWITCH_HAT_L*/,
  SWITCH_BTN_X, SWITCH_BTN_A, SWITCH_BTN_B, SWITCH_BTN_Y,
  SWITCH_BTN_L, SWITCH_BTN_R, SWITCH_BTN_SELECT, SWITCH_BTN_START,
  SWITCH_BTN_CAPTURE, SWITCH_BTN_HOME, 0 /*Fn1*/, 0 /*Fn2*/
};
const int hat_mapping[16] = {
  SWITCH_HAT_CENTER, SWITCH_HAT_U, SWITCH_HAT_R, SWITCH_HAT_UR,
  SWITCH_HAT_D, SWITCH_HAT_CENTER, SWITCH_HAT_DR, SWITCH_HAT_R,
  SWITCH_HAT_L, SWITCH_HAT_UL, SWITCH_HAT_CENTER, SWITCH_HAT_U,
  SWITCH_HAT_DL, SWITCH_HAT_L, SWITCH_HAT_D, SWITCH_HAT_CENTER,
};
#endif
#endif

const int min_threshold = 15;
const long cd_length = 10000;
const float k_threshold = 1.5;
const float k_decay = 0.97;

const int pin[4] = {A0, A3, A1, A2};
const int key[4] = {'d', 'f', 'j', 'k'};
const float sens[4] = {1.0, 1.0, 1.0, 1.0};

const int key_next[4] = {3, 2, 0, 1};

const long cd_stageselect = 200000;

bool stageselect = false;
bool stageresult = false;

float threshold = 20;
int raw[4] = {0, 0, 0, 0};
float level[4] = {0, 0, 0, 0};
long cd[4] = {0, 0, 0, 0};
bool down[4] = {false, false, false, false};
#ifdef ENABLE_NS_JOYSTICK
uint8_t down_count[4] = {0, 0, 0, 0};
#endif

typedef unsigned long time_t;
time_t t0 = 0;
time_t dt = 0, sdt = 0;

void sample() {
  int prev[4] = {raw[0], raw[1], raw[2], raw[3]};
  raw[0] = analogRead(pin[0]);
  raw[1] = analogRead(pin[1]);
  raw[2] = analogRead(pin[2]);
  raw[3] = analogRead(pin[3]);
  for (int i=0; i<4; ++i)
    level[i] = abs(raw[i] - prev[i]) * sens[i];
}

void sampleSingle(int i) {
  int prev = raw[i];
  raw[i] = analogReadNow();
  level[i] = abs(raw[i] - prev) * sens[i];
  analogSwitchPin(pin[key_next[i]]);
}

void setup() {
  analogReference(DEFAULT);
  analogSwitchPin(pin[0]);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#ifdef ENABLE_NS_JOYSTICK
  for (int i = 0; i < 8; ++i) pinMode(i, INPUT_PULLUP);
  for (int i = 0; i < 4; ++i) {  digitalWrite(led_pin[i], HIGH); pinMode(led_pin[i], OUTPUT); }
#endif
#ifdef ENABLE_KEYBOARD
  Keyboard.begin();
#endif
  t0 = micros();
  Serial.begin(9600);
}

void parseSerial() {
  static char command = -1;
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (command == -1)
      command = c;
    else {
      switch (command) {
      case 'C':
        Serial.write('C');
        Serial.write(c);
        Serial.flush();
        break;
      case 'S':
        stageselect = (c == '1');
        digitalWrite(LED_BUILTIN, stageselect ? HIGH : LOW);
        break;
      case 'R':
        stageresult = (c == '1');
        digitalWrite(LED_BUILTIN, stageresult ? HIGH : LOW);
        break;
      }
      command = -1;
    }
  }
}

void loop_test() {
  sampleSingle(0);
  Serial.print(level[0]);
  Serial.print("\t");
  delayMicroseconds(500);
  sampleSingle(1);
  Serial.print(level[1]);
  Serial.print("\t");
  delayMicroseconds(500);
  sampleSingle(2);
  Serial.print(level[2]);
  Serial.print("\t");
  delayMicroseconds(500);
  sampleSingle(3);
  Serial.print(level[3]);
  Serial.println();
  delayMicroseconds(500);
}

void loop_test2() {
  Serial.print(analogRead(pin[0]));
  Serial.print("\t");
  delayMicroseconds(500);
  Serial.print(analogRead(pin[1]));
  Serial.print("\t");
  delayMicroseconds(500);
  Serial.print(analogRead(pin[2]));
  Serial.print("\t");
  delayMicroseconds(500);
  Serial.print(analogRead(pin[3]));
  Serial.println();
  delayMicroseconds(500);
}

void loop() {
  //loop_test2(); return;
  
  static int si = 0;

#ifdef ENABLE_KEYBOARD
  parseSerial();
#endif
  
  time_t t1 = micros();
  dt = t1 - t0;
  sdt += dt;
  t0 = t1;
  
  float prev_level = level[si];
  sampleSingle(si);
  float new_level = level[si];
  level[si] = (level[si] + prev_level * 2) / 3;
  
  threshold *= k_decay;

  for (int i = 0; i != 4; ++i) {
    if (cd[i] > 0) {
      cd[i] -= dt;
      if (cd[i] <= 0) {
        cd[i] = 0;
        if (down[i]) {
#ifdef ENABLE_KEYBOARD
          Keyboard.release(stageresult ? KEY_ESC : key[i]);
#endif
          down[i] = false;
        }
      }
    }
  }
  
  int i_max = 0;
  int level_max = 0;
  
  for (int i = 0; i != 4; ++i) {
    if (level[i] > level_max && level[i] > threshold) {
      level_max = level[i];
      i_max = i;
    }
  }

  if (i_max == si && level_max >= min_threshold) {
    if (cd[i_max] == 0) {
      if (!down[i_max]) {
#ifdef DEBUG_DATA
        Serial.print(level[0], 1);
        Serial.print("\t");
        Serial.print(level[1], 1);
        Serial.print("\t");
        Serial.print(level[2], 1);
        Serial.print("\t");
        Serial.print(level[3], 1);
        Serial.print("\n");
#endif
#ifdef ENABLE_KEYBOARD
        if (stageresult) {
          Keyboard.press(KEY_ESC);
        } else {
          Keyboard.press(key[i_max]);
        }
#endif
        down[i_max] = true;
#ifdef ENABLE_NS_JOYSTICK
        if (down_count[i_max] <= 2) down_count[i_max] += 2;
#endif
      }
      for (int i = 0; i != 4; ++i)
        cd[i] = cd_length;
#ifdef ENABLE_KEYBOARD
      if (stageselect)
        cd[i_max] = cd_stageselect;
#endif
    }
    sdt = 0;
  }

  if (cd[i_max] > 0) {
    threshold = max(threshold, level_max * k_threshold);
  }
  
  static time_t ct = 0;
  static int cc = 0;
  ct += dt;
  cc += 1;

#ifdef HAS_BUTTONS
  // 4x4 button scan, one row per cycle
  static int bi = 3;
  pinMode(bi+4, INPUT_PULLUP);
  bi = ((bi+1)&3);
  pinMode(bi+4, OUTPUT);
  digitalWrite(bi+4, LOW);
    
  int state;
  int* bs = button_state + (bi << 2);
  int* bc = button_cd + (bi << 2);
  for (int i = 0; i < 4; ++i) {
    state = (digitalRead(i) == LOW);
    //digitalWrite(led_pin[i], state ? LOW : HIGH);
    if (bc[i] != 0) {
      bc[i] -= ct;
      if (bc[i] < 0) bc[i] = 0;
    }
    if (state != bs[i] && bc[i] == 0) {
      bs[i] = state;
      bc[i] = 15000;
#ifdef ENABLE_KEYBOARD
      if (state) {
        Keyboard.press(button_key[(bi << 2) + i]);
      } else {
        Keyboard.release(button_key[(bi << 2) + i]);
      }
#endif
    }
#ifdef ENABLE_NS_JOYSTICK
    Joystick.Button |= (bs[i] ? button[(bi << 2) + i] : SWITCH_BTN_NONE);
#endif
  }
#endif
  
#ifdef ENABLE_NS_JOYSTICK
  if (ct > 32000 || (ct > 8000 && (down_count[0] || down_count[1] || down_count[2] || down_count[3]))) {
    for (int i = 0; i < 4; ++i) { // Sensors
      bool state = (down_count[i] & 1);
      Joystick.Button |= (state ? sensor_button[i] : SWITCH_BTN_NONE);
      down_count[i] -= !!down_count[i];
      digitalWrite(led_pin[i], state ? LOW : HIGH);
    }
    state = 0;
    for (int i = 0; i < 4; ++i) { // Buttons for hats
      state |= (button_state[i] ? 1 << i : 0);
    }
    Joystick.HAT = hat_mapping[state]; 
    Joystick.sendState();
    Joystick.Button = SWITCH_BTN_NONE;
#ifdef DEBUG_TIME
    if (cc > 0)
      Serial.println((float)ct/cc);
#endif
    ct = 0;
    cc = 0;
  }
#endif

#ifdef DEBUG_OUTPUT
  static bool printing = false;
#ifdef DEBUG_OUTPUT_LIVE
  if (true)
#else
  if (printing || (/*down[0] &&*/ threshold > 10))
#endif
  {
    printing = true;
    Serial.print(level[0], 1);
    Serial.print("\t");
    Serial.print(level[1], 1);
    Serial.print("\t");
    Serial.print(level[2], 1);
    Serial.print("\t");
    Serial.print(level[3], 1);
    Serial.print("\t| ");
    Serial.print(cd[0] == 0 ? "  " : down[0] ? "# " : "* ");
    Serial.print(cd[1] == 0 ? "  " : down[1] ? "# " : "* ");
    Serial.print(cd[2] == 0 ? "  " : down[2] ? "# " : "* ");
    Serial.print(cd[3] == 0 ? "  " : down[3] ? "# " : "* ");
    Serial.print("|\t");
    Serial.print(threshold, 1);
    Serial.println();
    if(threshold <= 5){
      Serial.println();
      printing = false;
    }
  } 
#endif

  level[si] = new_level;
  si = key_next[si];

  long ddt = 300 - (micros() - t0);
  if(ddt > 3) delayMicroseconds(ddt);
  
}
