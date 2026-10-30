#include <Arduino.h>

constexpr int motorGatePin = 3;
constexpr int moistureSensor1 = A0;


static constexpr float potHeight = 8.0;
static constexpr float potDiameter = 10.0;

static float radius = potDiameter / 2;
static float potVolumeMl = 3.14159 * radius * radius * potHeight * 0.85;

static float wateringVolume = 0.025;

static float mlPerMs = 1600.0 / 60000.0;
static unsigned long wateringDuration = max(((potVolumeMl * wateringVolume) / mlPerMs), 1000.0);
static unsigned long wateringTime = 0;

static double waterThreshold = 0.3;
static bool isWatering = false;

static unsigned long checkInterval = 60 * 1000.0;
static unsigned long wateringInterval = (15 + (potHeight * 1.5)) * 60.0 * 1000.0;

static bool isFaulted = false;
static int wateringFailureCount = 0;
static float soilMoistureBeforeWatering = 0.0;
static bool pendingVerification = false;

void setup() {
    Serial.begin(9600);
    Serial.println("Booted");

    pinMode(motorGatePin, OUTPUT);

}

static void checkSoilMoisture() {
    const int rawSoilMoisture = analogRead(moistureSensor1);
    const float soilMoisture = constrain(map(rawSoilMoisture, 600, 200, 0, 100), 0, 100) / 100.0;
    const bool needsWater = soilMoisture < waterThreshold;

    if (pendingVerification) {
        if (soilMoisture - soilMoistureBeforeWatering < 0.02) {
            wateringFailureCount++;
            Serial.println("Encountered no change in moisture after watering!");
        } else {
            wateringFailureCount = 0;
            Serial.println("Moisture increased successfully.");
        }
        pendingVerification = false;
    }

    static unsigned long last_watering = millis() - wateringInterval;
    if (millis() - last_watering >= wateringInterval) {
        if (needsWater && (needsWater != isWatering)) {
            soilMoistureBeforeWatering = soilMoisture;
            pendingVerification = true;

            Serial.println("Watering started!");
            digitalWrite(motorGatePin, HIGH);
            isWatering = true;
            wateringTime = millis();
            last_watering = millis();
        }
    }

    Serial.print("Soil Moisture: ");
    Serial.println(soilMoisture);
    Serial.print("Needs Watering: ");
    Serial.println(needsWater);
}

static void wateringLoop() {
    if (wateringFailureCount >= 3) {
        if (!isFaulted) {
            Serial.println("There are problems in watering system!");
            Serial.println("Please check tubes, water reservoir and moisture sensor!");
        }
        digitalWrite(motorGatePin, LOW);
        isFaulted = true;
    } else {
        if (isFaulted) {
            Serial.println("Problems got solved!");
            Serial.println("Continue watering plants!");
        }
        isFaulted = false;
    }

    static unsigned long last_check = millis() - checkInterval;
    if (!isFaulted && millis() - last_check >= checkInterval) {
        checkSoilMoisture();
        last_check = millis();
    }

    if (isWatering && millis() - wateringTime >= wateringDuration) {
        Serial.println("Watering finished!");
        digitalWrite(motorGatePin, LOW);
        isWatering = false;
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
