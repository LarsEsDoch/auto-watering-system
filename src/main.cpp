#include <Arduino.h>

constexpr int motorGatePin = 3;
constexpr int moistureSensor1 = A0;


const float potHeight = 8.0;
const float potDiameter = 10.0;

float radius = potDiameter / 2;
float potVolumeMl = 3.14159 * radius * radius * potHeight * 0.85;

float wateringVolume = 0.025;

float mlPerMs = 1600.0 / 60000.0;
long wateringDuration = (potVolumeMl * wateringVolume) / mlPerMs;
unsigned long wateringTime = 0;

double waterTreshold = 0.3;
boolean isWatering = false;

long checkInterval = 600 * 1000.0;
long wateringInterval = (15 + (potHeight * 1.5)) * 60.0 * 1000.0;


void setup() {
    Serial.begin(9600);
    Serial.println("Booted");

    pinMode(motorGatePin, OUTPUT);

}

void checkSoilMoisture() {
    const int rawSoilMoisture = analogRead(moistureSensor1);
    const double soilMoisture = constrain(map(rawSoilMoisture, 600, 200, 0, 100), 0, 100) / 100.0;
    const bool needsWater = soilMoisture < waterTreshold;

    static unsigned long last_watering = millis() - wateringInterval;
    if (millis() - last_watering >= wateringInterval) {
        if (needsWater && (needsWater != isWatering)) {
            Serial.println("Watering started!");
            digitalWrite(motorGatePin, HIGH);
        }
    }
void wateringLoop() {
    static unsigned long last_check = millis() - checkInterval;
    if (millis() - last_check >= checkInterval) {
        checkSoilMoisture();
    }
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
