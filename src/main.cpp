#include <Arduino.h>

constexpr int motorGatePin = 3;
constexpr int moistureSensor1 = A0;



void setup() {
    Serial.begin(9600);
    Serial.println("Booted");

    pinMode(motorGatePin, OUTPUT);

}

void checkSoilMoisture() {
    const int rawSoilMoisture = analogRead(moistureSensor1);
    const double soilMoisture = constrain(map(rawSoilMoisture, 600, 200, 0, 100), 0, 100) / 100.0;
    }
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
