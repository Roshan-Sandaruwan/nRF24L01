// nRF24L01 Module Test

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Starting radio...");
  if (!radio.begin()) {
    Serial.println("radio.begin() FAILED (wiring/power/module dead)");
    while (1) {}
  }

  Serial.println("radio.begin() OK");
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(76);

  radio.printDetails(); // shows config, helps detect SPI issues
}

void loop() {}