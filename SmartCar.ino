/* SmartCar - reconstructed reference firmware, not the original project source.
 * ESP32 Classic Bluetooth SPP + four DC motors + two L298N + HC-SR04.
 * Review docs/WIRING.md before applying power. No PWM: ENA/ENB jumpers fitted.
 * No rear/side sensors: automatic reverse/turn requires a clear test area.
 */
#include <Arduino.h>
#include "BluetoothSerial.h"
#include "config.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED) || !defined(CONFIG_BT_SPP_ENABLED)
#error This firmware requires an original ESP32 with Bluetooth Classic SPP.
#endif

BluetoothSerial SerialBT;
enum class Mode { MANUAL, AUTO };
enum class Phase { CRUISE, STOP_BEFORE_REVERSE, REVERSE, STOP_BEFORE_TURN, TURN, STOP_AFTER_TURN };
Mode mode = Mode::MANUAL;
Phase phase = Phase::CRUISE;
char manualCommand = 'S';
bool connectedBefore = false;
bool validDistance = false;
float distanceCm = 0;
uint32_t lastRx = 0, lastManual = 0, lastSonar = 0, lastValidSonar = 0;
uint32_t phaseStarted = 0, lastTelemetry = 0;
int previousLeft = 0, previousRight = 0;

void motor(const MotorPins &p, int direction) {
  if (p.inverted) direction = -direction;
  digitalWrite(p.in1, direction > 0 ? HIGH : LOW);
  digitalWrite(p.in2, direction < 0 ? HIGH : LOW);
}
void rawDrive(int left, int right) {
  motor(LEFT_FRONT, left); motor(LEFT_REAR, left);
  motor(RIGHT_FRONT, right); motor(RIGHT_REAR, right);
}
void drive(int left, int right) {
  // Coast briefly before reversing a side's polarity.
  if (left * previousLeft < 0 || right * previousRight < 0) {
    rawDrive(0, 0); delay(20);
  }
  rawDrive(left, right);
  previousLeft = left; previousRight = right;
}
void stopCar() { drive(0, 0); }
void resetControl() {
  mode = Mode::MANUAL; manualCommand = 'S'; phase = Phase::CRUISE;
  digitalWrite(AUTO_LED_PIN, LOW); stopCar();
}
void startPhase(Phase next, uint32_t now) {
  phase = next; phaseStarted = now;
}
void sampleDistance(uint32_t now) {
  if (uint32_t(now - lastSonar) < SONAR_INTERVAL_MS) return;
  lastSonar = now;
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
  distanceCm = duration * 0.0343f / 2.0f;
  validDistance = duration != 0 && distanceCm >= MIN_DISTANCE_CM && distanceCm <= MAX_DISTANCE_CM;
  if (validDistance) lastValidSonar = millis();
}
bool sonarReady(uint32_t now) {
  return validDistance && uint32_t(now - lastValidSonar) <= SONAR_STALE_MS;
}
void handleCommand(char c, uint32_t now) {
  if (c >= 'a' && c <= 'z') c = char(c - 'a' + 'A');
  switch (c) {
    case 'S': resetControl(); lastRx = now; break;
    case 'M': resetControl(); lastRx = now; break;
    case 'A':
      stopCar(); mode = Mode::AUTO; manualCommand = 'S';
      startPhase(Phase::CRUISE, now); lastRx = now;
      digitalWrite(AUTO_LED_PIN, HIGH); break;
    case 'K': lastRx = now; break; // Heartbeat does not renew manual motion.
    case 'F': case 'B': case 'L': case 'R':
      if (mode == Mode::MANUAL) {
        manualCommand = c; lastManual = now; lastRx = now;
      }
      break;
    default: break; // Whitespace/unknown bytes do not keep motion alive.
  }
}
void runManual(uint32_t now) {
  if (uint32_t(now - lastManual) > MANUAL_TIMEOUT_MS) manualCommand = 'S';
  switch (manualCommand) {
    case 'F':
      if (sonarReady(now) && distanceCm > OBSTACLE_CM) drive(1, 1);
      else stopCar();
      break;
    case 'B': drive(-1, -1); break;
    case 'L': drive(-1, 1); break;
    case 'R': drive(1, -1); break;
    default: stopCar(); break;
  }
}
void runAuto(uint32_t now) {
  if (!sonarReady(now)) {
    stopCar(); startPhase(Phase::CRUISE, now); return;
  }
  uint32_t elapsed = uint32_t(now - phaseStarted);
  switch (phase) {
    case Phase::CRUISE:
      if (distanceCm > OBSTACLE_CM) drive(1, 1);
      else { stopCar(); startPhase(Phase::STOP_BEFORE_REVERSE, now); }
      break;
    case Phase::STOP_BEFORE_REVERSE:
      stopCar();
      if (elapsed >= STOP_BEFORE_REVERSE_MS) startPhase(Phase::REVERSE, now);
      break;
    case Phase::REVERSE:
      if (elapsed >= REVERSE_MS) { stopCar(); startPhase(Phase::STOP_BEFORE_TURN, now); }
      else drive(-1, -1);
      break;
    case Phase::STOP_BEFORE_TURN:
      stopCar();
      if (elapsed >= STOP_BEFORE_TURN_MS) startPhase(Phase::TURN, now);
      break;
    case Phase::TURN:
      if (elapsed >= TURN_MS) { stopCar(); startPhase(Phase::STOP_AFTER_TURN, now); }
      else drive(1, -1);
      break;
    case Phase::STOP_AFTER_TURN:
      stopCar();
      if (elapsed >= STOP_AFTER_TURN_MS) startPhase(Phase::CRUISE, now);
      break;
  }
}
void telemetry(uint32_t now) {
  if (uint32_t(now - lastTelemetry) < 500) return;
  lastTelemetry = now;
  SerialBT.print("MODE="); SerialBT.print(mode == Mode::AUTO ? "AUTO" : "MANUAL");
  SerialBT.print(";DIST_CM=");
  if (sonarReady(now)) SerialBT.print(distanceCm, 1); else SerialBT.print("INVALID");
  SerialBT.print(";MOTION="); SerialBT.print(previousLeft);
  SerialBT.print(','); SerialBT.println(previousRight);
}
void setup() {
  Serial.begin(115200);
  const MotorPins motors[] = {LEFT_FRONT, LEFT_REAR, RIGHT_FRONT, RIGHT_REAR};
  for (const auto &p : motors) {
    digitalWrite(p.in1, LOW); digitalWrite(p.in2, LOW);
    pinMode(p.in1, OUTPUT); pinMode(p.in2, OUTPUT);
  }
  pinMode(TRIG_PIN, OUTPUT); digitalWrite(TRIG_PIN, LOW);
  pinMode(ECHO_PIN, INPUT); pinMode(AUTO_LED_PIN, OUTPUT);
  resetControl();
  if (!SerialBT.begin(BT_NAME)) {
    Serial.println("Bluetooth failed; motors remain stopped.");
    while (true) { stopCar(); delay(100); }
  }
  Serial.println("SMART_CAR ready. Stopped in MANUAL mode.");
}
void loop() {
  bool connected = SerialBT.hasClient();
  if (!connected) {
    resetControl(); connectedBefore = false;
    // Discard queued commands on loss of link.
    while (SerialBT.available()) SerialBT.read();
    delay(5); return;
  }
  if (!connectedBefore) {
    resetControl(); connectedBefore = true; lastRx = millis();
  }
  // Check timeouts before accepting fresh bytes, so a late heartbeat cannot
  // resume an expired motion/automatic session.
  uint32_t now = millis();
  if (uint32_t(now - lastRx) > LINK_TIMEOUT_MS) resetControl();
  if (mode == Mode::MANUAL && uint32_t(now - lastManual) > MANUAL_TIMEOUT_MS) {
    manualCommand = 'S'; stopCar();
  }
  // Bounded processing keeps sensors and timeout checks serviced.
  for (int count = 0; count < 32 && SerialBT.available(); ++count) {
    char c = char(SerialBT.read());
    handleCommand(c, millis());
    if (c == 'S' || c == 's' || c == 'M' || c == 'm') {
      while (SerialBT.available()) SerialBT.read();
      break;
    }
  }
  sampleDistance(millis());
  now = millis();
  if (uint32_t(now - lastRx) > LINK_TIMEOUT_MS) resetControl();
  else if (mode == Mode::AUTO) runAuto(now);
  else runManual(now);
  telemetry(millis());
  delay(2);
}
