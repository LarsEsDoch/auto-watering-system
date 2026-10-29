#include <Arduino.h>

const int motorGatePin = 3;

void setup() {
    Serial.begin(9600);
    Serial.println("Booted");

    pinMode(motorGatePin, OUTPUT);
}

void loop() {
    Serial.println(analogRead(A0));
    Serial.print("Timestamp on:");
    Serial.println(millis());
    digitalWrite(motorGatePin, HIGH);

    delay(5000);
    Serial.print("Timestamp off:");
    Serial.println(millis());

    digitalWrite(motorGatePin, LOW);

    delay(5000);
}
