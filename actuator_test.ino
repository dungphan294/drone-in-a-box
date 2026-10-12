// Test sketch: one linear actuator on ZK-5AD channel B (motor 2), ESP32-S3
// Wiring: GPIO4 -> D2, GPIO5 -> D3, ESP32 GND -> driver GND
// Open Serial Monitor (115200 baud) and type:
//   e = extend, r = retract, s = stop
//   1..9 = speed 10%..90%, 0 = full speed (100%)

const int PIN_A = 4;          // to D2
const int PIN_B = 5;          // to D3
int speedPct = 100;           // current speed setting in %
char direction = 's';         // 'e', 'r' or 's'

int duty() { return map(speedPct, 0, 100, 0, 255); }

void apply() {
  if (direction == 'e')      { analogWrite(PIN_A, duty()); analogWrite(PIN_B, 0); }
  else if (direction == 'r') { analogWrite(PIN_A, 0);      analogWrite(PIN_B, duty()); }
  else                       { analogWrite(PIN_A, 0);      analogWrite(PIN_B, 0); }
}

void setup() {
  analogWriteFrequency(PIN_A, 20000);   // 20 kHz: above hearing, no motor whine
  analogWriteFrequency(PIN_B, 20000);
  apply();                              // motor off right after boot
  Serial.begin(115200);
  Serial.println("Ready: e/r/s, speed 1-9 (10-90%), 0 = 100%");
}

void loop() {
  if (!Serial.available()) return;
  char c = Serial.read();
  if (c == 'e' || c == 'r' || c == 's') {
    direction = c;
  } else if (c >= '0' && c <= '9') {
    speedPct = (c == '0') ? 100 : (c - '0') * 10;
  } else {
    return;
  }
  apply();
  Serial.printf("Direction: %c, speed: %d%%\n", direction, speedPct);
}
