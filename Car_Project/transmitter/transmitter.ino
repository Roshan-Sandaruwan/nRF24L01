// Transmitter
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

void setup() {
  Serial.begin(9600);

  if (!radio.begin()) {
    Serial.println("NRF init failed");
    while (1);
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  data.x = analogRead(A0);
  data.y = analogRead(A1);

  bool ok = radio.write(&data, sizeof(data));

  Serial.print("X="); Serial.print(data.x);
  Serial.print(" Y="); Serial.print(data.y);
  Serial.print("  TX="); Serial.println(ok ? "OK" : "FAIL");

  delay(20); // ~50Hz
}