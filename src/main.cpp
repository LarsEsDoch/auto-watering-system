#include <Arduino.h>

constexpr int motorGatePin = 3;
constexpr int moistureSensor1 = A0;

static constexpr int moistureSensorMin = 460;
static constexpr int moistureSensorMax = 210;

static constexpr float potHeight = 8.0;
static constexpr float potDiameter = 10.0;
// Small pots (under 15cm diameter): 1 outlet (placed 3-4cm from sensor)
// Medium pots (15cm to 24cm diameter): 2 outlets (opposite sides)
// Large pots (25cm to 35cm diameter): 3 to 4 outlets (distributed in a ring)
// Extra large pots (over 35cm diameter): Drip ring or 4+ outlets required

static float radius = potDiameter / 2.0;
static float potVolumeMl = 3.14159 * radius * radius * potHeight * 0.85;

static float wateringVolume = constrain(0.022 + (1.25 / sqrt(potVolumeMl)), 0.035, 0.08);

static float mlPerMs = 1600.0 / 60000.0;
static constexpr unsigned long minWateringTime = 800.0;

static unsigned long currentWateringDuration = 0;
static unsigned long wateringDuration = max(((potVolumeMl * wateringVolume) / mlPerMs), minWateringTime);
static unsigned long wateringTime = 0;

static double waterThreshold = 0.3;
static bool isWatering = false;

static unsigned long checkInterval = 60 * 1000.0;
static unsigned long wateringInterval = (4.0 + (potHeight * 1.0) + (potDiameter * 0.35)) * 60.0 * 1000.0;

static bool isFaulted = false;
static int wateringFailureCount = 0;
static float soilMoistureBeforeWatering = 0.0;
static bool pendingVerification = false;

void setup() {
    Serial.begin(9600);
    Serial.println("Booted");

    digitalWrite(motorGatePin, LOW); //TODO TEST if it makes a difference
    pinMode(motorGatePin, OUTPUT);
}

static void checkSoilMoisture() {
    const int rawSoilMoisture = analogRead(moistureSensor1);
    const float soilMoisture = constrain(map(rawSoilMoisture, moistureSensorMin, moistureSensorMax, 0, 100), 0, 100) / 100.0;
    const bool needsWater = soilMoisture < waterThreshold;

    if (!needsWater && (wateringFailureCount > 0 || isFaulted)) {
        Serial.println("Manual watering noticed. Starting automatic watering again!");
        wateringFailureCount = 0;
        pendingVerification = false;
    }

    static unsigned long last_watering = millis() - wateringInterval;
    static unsigned long last_verification = millis() - wateringInterval;
    const bool soakTimeElapsed = millis() - last_watering >= wateringInterval;
    const bool verificationTimeElapsed = millis() - last_verification >= wateringInterval / 3;

    if (pendingVerification && verificationTimeElapsed) {
        if (soilMoisture - soilMoistureBeforeWatering < 0.01) {
            wateringFailureCount++;
            Serial.print("No moisture increase detected. Fail count: ");
            Serial.println(wateringFailureCount);
        } else {
            if (wateringFailureCount > 0) {
                Serial.println("Moisture increase confirmed. Continue with normal operation!");
            }
            wateringFailureCount = 0;
            pendingVerification = false;
        }
        last_verification = millis();
    }

    if (!isFaulted && soakTimeElapsed && needsWater && !isWatering) {
        soilMoistureBeforeWatering = soilMoisture;
        pendingVerification = true;

        float deficitFactor = (waterThreshold - soilMoisture) / waterThreshold;
        deficitFactor = constrain(deficitFactor, 0.35, 1.0);
        currentWateringDuration = max((unsigned long)(wateringDuration * deficitFactor), minWateringTime);

        Serial.print("Watering started for ");
        Serial.print(currentWateringDuration);
        Serial.println(" ms");

        digitalWrite(motorGatePin, HIGH);
        isWatering = true;
        wateringTime = millis();
        last_watering = millis();
        last_verification = millis();
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
    if (millis() - last_check >= checkInterval) {
        checkSoilMoisture();
        last_check = millis();
    }

    if (isWatering && (millis() - wateringTime >= currentWateringDuration)) {
        Serial.println("Watering finished!");
        digitalWrite(motorGatePin, LOW);
        isWatering = false;
    }
}

void loop() {
    wateringLoop();
    delay(100);
}