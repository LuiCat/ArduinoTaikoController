#include <Keyboard.h>

//#define DEBUG_OUTPUT

const int min_threshold = 20;
const long cd_length = 8000;
const long cd_antireso_length = 8000;
const float k_antireso = 0.85;
const float k_decay = 0.96;

const int pin[4] = {A3, A0, A2, A1};
const int key[4] = {'d', 'f', 'j', 'k'};

const int key_next[4] = {1, 3, 0, 2};

const long cd_stageselect = 200000;

bool stageselect = false;
bool stageresult = false;

float threshold[4] = {20, 20, 20, 20};
int raw[4] = {0, 0, 0, 0};
int level[4] = {0, 0, 0, 0};
long cd[4] = {0, 0, 0, 0};
bool pressed[4] = {false, false, false, false};
int t0 = 0;
int dt = 0, sdt = 0;

void sample() {
  int prev[4] = {raw[0], raw[1], raw[2], raw[3]};
  raw[0] = analogRead(pin[0]);
  raw[1] = analogRead(pin[1]);
  raw[2] = analogRead(pin[2]);
  raw[3] = analogRead(pin[3]);
  for (int i=0; i<4; ++i)
    level[i] = abs(raw[i] - prev[i]);
}

void sampleSingle(int i) {
  int prev = raw[i];
  raw[i] = analogRead(pin[i]);
  level[i] = abs(raw[i] - prev);
}

void setup() {
  analogReference(DEFAULT); // use internal 1.1v as reference voltage
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Keyboard.begin();
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

void loop() {
  static int si = 0;

  parseSerial();
  
  int t1 = micros();
  dt = t1 - t0;
  sdt += dt;
  t0 = t1;
  
  while (sdt >= 1000) {
    sdt -= 1000;
    for (int i = 0; i != 4; ++i)
      threshold[i] *= k_decay;
  }

  for (int i = 0; i != 4; ++i) {
    if (cd[i] > 0) {
      cd[i] -= dt;
      if (cd[i] <= 0) {
        cd[i] = 0;
        if (pressed[i]) {
#ifndef DEBUG_OUTPUT
          Keyboard.release(stageresult ? KEY_ESC : key[i]);
#endif
          pressed[i] = false;
        }
      }
    }
  }
  
  int i_max = 0;
  int level_max = 0;
  
  for (int i = 0; i != 4; ++i) {
    if (level[i] > level_max && level[i] > threshold[i]) {
      level_max = level[i];
      i_max = i;
    }
  }

  if (i_max == si && level_max >= min_threshold) {
    if (cd[i_max] == 0) {
      if (!pressed[i_max]) {
#ifndef DEBUG_OUTPUT
        if (stageresult) {
          Keyboard.press(KEY_ESC);
        } else {
          Keyboard.press(key[i_max]);
        }
#endif
        pressed[i_max] = true;
      }
      for (int i = 0; i != 4; ++i)
        cd[i] = cd_antireso_length;
      cd[i_max] = (stageselect ? cd_stageselect : cd_length);
    }
    float level_antireso = level_max * k_antireso;
    for (int i = 0; i != 4; ++i)
      threshold[i] = max(threshold[i], level_antireso);
    threshold[i_max] = (cd[i_max] == 0 ? level_max : level_max * 1.5);
    sdt = 0;
  }

#ifdef DEBUG_OUTPUT
  static bool printing = false;
  if (si == 0) {
    if (level[0]+level[1]+level[2]+level[3] >= min_threshold || cd[0] || cd[1] || cd[2] || cd[3]){
      Serial.print(level[0]);
      Serial.print("\t");
      Serial.print(level[1]);
      Serial.print("\t");
      Serial.print(level[2]);
      Serial.print("\t");
      Serial.print(level[3]);
      Serial.print("\t| ");
      Serial.print(cd[0] == 0 ? "  " : pressed[0] ? "# " : "* ");
      Serial.print(cd[1] == 0 ? "  " : pressed[1] ? "# " : "* ");
      Serial.print(cd[2] == 0 ? "  " : pressed[2] ? "# " : "* ");
      Serial.print(cd[3] == 0 ? "  " : pressed[3] ? "# " : "* ");
      Serial.print("|\t");
      Serial.print((int)threshold[0]);
      Serial.print("\t");
      Serial.print((int)threshold[1]);
      Serial.print("\t");
      Serial.print((int)threshold[2]);
      Serial.print("\t");
      Serial.print((int)threshold[3]);
      Serial.println();
      printing = true;
    }else if(printing){
      Serial.println("=============================================================================");
      printing = false;
    }
  }
#endif

  sampleSingle(si);
  si = key_next[si];
  
}
