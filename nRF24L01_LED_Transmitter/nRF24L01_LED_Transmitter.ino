// nRF24L01 LED Transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);
const byte address[6] = "00001";

void setup() {
  Serial.begin(115200);

  radio.begin();
  radio.setChannel(76);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_LOW);

  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("Type 1=ON, 0=OFF");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if (c == '1') {
      const char msg[] = "ON";
      bool ok = radio.write(&msg, sizeof(msg));
      Serial.println(ok ? "Sent ON OK" : "Sent ON FAIL");
    }

    if (c == '0') {
      const char msg[] = "OFF";
      bool ok = radio.write(&msg, sizeof(msg));
      Serial.println(ok ? "Sent OFF OK" : "Sent OFF FAIL");
    }
  }
}