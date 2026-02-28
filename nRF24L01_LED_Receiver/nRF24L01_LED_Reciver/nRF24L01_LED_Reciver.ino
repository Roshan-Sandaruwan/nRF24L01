// nRF24L01 LED Receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);
const byte address[6] = "00001";
const int ledPin = 3;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

  radio.begin();
  radio.setChannel(76);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_LOW);

  radio.openReadingPipe(1, address);
  radio.startListening();

  Serial.println("Receiver ready...");
}

void loop() {
  if (radio.available()) {
    char msg[32] = {0};
    radio.read(&msg, sizeof(msg));
    Serial.print("Received: ");
    Serial.println(msg);

    if (!strcmp(msg, "ON"))  digitalWrite(ledPin, HIGH);
    if (!strcmp(msg, "OFF")) digitalWrite(ledPin, LOW);
  }
}