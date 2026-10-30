#include <Arduino.h>

const int motorGatePin = 3;

void setup() {
    Serial.begin(9600);
    Serial.println("Booted");

    pinMode(motorGatePin, OUTPUT);
}

void loop() {
    unsigned long last_millis = 0;
    Serial.print("Timestamp on:  ");
    Serial.println(millis());
    last_millis = millis();
    digitalWrite(motorGatePin, HIGH);

    delay(5000);
    Serial.print("Timestamp off: ");
    Serial.println(millis());
    Serial.print("On time: ");
    Serial.println(millis() - last_millis);


    digitalWrite(motorGatePin, LOW);

    delay(5000);
}
