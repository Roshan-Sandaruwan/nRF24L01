// Reciver esp32 arduino R4

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "CAR01";

struct Payload {
  uint16_t x;   // 0..1023
  uint16_t y;   // 0..1023
};

Payload data;

const int ENA = 5;   // PWM
const int IN1 = 7;
const int IN2 = 8;

const int ENB = 6;   // PWM
const int IN3 = 4;
const int IN4 = 3;

unsigned long lastRecv = 0;
const unsigned long FAILSAFE_MS = 400;

uint16_t applyDeadband(uint16_t v, uint16_t center = 512, uint16_t band = 20) {
  if (v > center - band && v < center + band) return center;
  return v;
}

// 0..1023 -> -255..+255 (with deadband)
int16_t joyToSigned(uint16_t v) {
  long out = map(v, 0, 1023, -255, 255);
  if (abs(out) < 25) out = 0;
  return (int16_t)out;
}

void setMotor(int en, int inA, int inB, int speedSigned) {
  int spd = abs(speedSigned);
  spd = constrain(spd, 0, 255);

  if (speedSigned > 0) {
    digitalWrite(inA, HIGH);
    digitalWrite(inB, LOW);
  } else if (speedSigned < 0) {
    digitalWrite(inA, LOW);
    digitalWrite(inB, HIGH);
  } else {
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
  }

  analogWrite(en, spd);
}

void stopAll() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  stopAll();

  if (!radio.begin()) {
    Serial.println("NRF init failed");
    while (1);
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);
  radio.openReadingPipe(1, address);
  radio.startListening();

  Serial.println("Receiver ready");
}

void loop() {
  // Receive newest packet
  if (radio.available()) {
    while (radio.available()) {
      radio.read(&data, sizeof(data));
    }
    lastRecv = millis();
  }

  // Failsafe: stop if no signal
  if (millis() - lastRecv > FAILSAFE_MS) {
    stopAll();
    // Optional debug
    // Serial.println("Failsafe STOP");
    return;
  }

  // Clean + convert
  uint16_t xRaw = applyDeadband(data.x);
  uint16_t yRaw = applyDeadband(data.y);

  int16_t steer    = joyToSigned(xRaw); // left/right
  int16_t throttle = joyToSigned(yRaw); // forward/back

  // Mix (tank style)
  int left  = constrain(throttle + steer, -255, 255);
  int right = constrain(throttle - steer, -255, 255);

  setMotor(ENA, IN1, IN2, left);
  setMotor(ENB, IN3, IN4, right);

  // Debug
  Serial.print("X="); Serial.print(xRaw);
  Serial.print(" Y="); Serial.print(yRaw);
  Serial.print(" L="); Serial.print(left);
  Serial.print(" R="); Serial.println(right);

  delay(10);
}