#define BLYNK_TEMPLATE_ID "TMPL3D4eKNtDF"
#define BLYNK_TEMPLATE_NAME "food adulteration detection"
#define BLYNK_AUTH_TOKEN "IG8-D29wpT6MTdbHLlYgnqM2FtQwBOYX"

#include "SensorManager.h"
#include <ColorName.h>
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <Wire.h>
#include <EEPROM.h>
#include <algorithm> // For std::sort

// ------ Calibration values (UPDATE these after buffer calibration!) ------
int SensorManager::raw7 = 2450;   // <--- Set this to your pH7 buffer ADC value
int SensorManager::raw4 = 1600;   // <--- Set this to your pH4 buffer ADC value

// ------ Baselines for MQ sensors (update after warm-up in clean air) ------
const int MQ4_BASELINE = 1259;    // Clean air value for MQ4 (methane)
const int MQ3_BASELINE = 2650;    // Clean air value for MQ3 (alcohol)

// ------ Unsafe color list ------
const char* SensorManager::unsafeColors[6] = {
  "Brown", "Black", "Gray", "Pale", "Dark Red", "Dark Green"
};
const int SensorManager::numUnsafeColors = 6;

// ------ WiFi and Blynk credentials ------
const char* ssid = "TestESP";
const char* password = "12345678";
char auth[] = "IG8-D29wpT6MTdbHLlYgnqM2FtQwBOYX";

BlynkTimer timerr;

SensorManager::SensorManager()
    : calibration_value(20.24 - 0.7), menuDisplayed(false) {}

void SensorManager::begin() {
    Serial.begin(9600);
    delay(1000);

    WiFi.begin(ssid, password);
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 30) {
        Serial.print(".");
        delay(500);
        retry++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi. Check credentials and 2.4GHz band.");
    }

    Blynk.begin(auth, ssid, password);
    if (Blynk.connected()) {
        Serial.println("Connected to Blynk server!");
    } else {
        Serial.println("Failed to connect to Blynk server. Check auth token and internet.");
    }

    pinMode(mq4Pin, INPUT);
    pinMode(phSensorPin, INPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(sensorOut, INPUT);

    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);

    timerr.setInterval(5000L, [this]() { this->sendToBlynk(); }); // Changed to 5 seconds
}

void SensorManager::sendToBlynk() {
    if (!Blynk.connected()) {
        Serial.println("Blynk not connected. Skipping data send.");
        return;
    }
    float phsensor = readPHSensor();
    const char* color = readColorSensor();
    float methane = readMethaneSensor();
    float alcohol = readAlcSensor();

    Serial.print("pH Sensor: ");
    Serial.println(phsensor);
    Serial.print("Color Sensor: ");
    Serial.println(color);
    Serial.print("Methane Sensor: ");
    Serial.println(methane);
    Serial.print("Alcohol Sensor: ");
    Serial.println(alcohol);

    // Send data to Blynk with debug
    Blynk.virtualWrite(V1, phsensor);
    Serial.println("Sent pH to V1");
    Blynk.virtualWrite(V3, color);
    Serial.println("Sent color to V3");
    Blynk.virtualWrite(V5, methane); // Changed from V32 to V5
    Serial.println("Sent methane to V5");
    Blynk.virtualWrite(V2, alcohol);
    Serial.println("Sent alcohol to V2");

    // Food safety logic
    bool isSafe = true;
    if(phsensor < 3.0 || phsensor > 7.0) isSafe = false;
    if(methane > 1000) isSafe = false;
    if(alcohol > 2000) isSafe = false;
    for(int i=0; i<numUnsafeColors; i++) {
        if(strcmp(color, unsafeColors[i]) == 0) {
            isSafe = false;
            break;
        }
    }
    Blynk.virtualWrite(V4, isSafe ? 1 : 0);
    Serial.println("Sent safety status to V4");
    Serial.println("Data sent to Blynk!");
}

// ---- pH Sensor ----
float SensorManager::readPHSensor() {
    return calculatePHValue();
}

float SensorManager::calculatePHValue() {
    // Take multiple readings for better accuracy
    int sum = 0;
    for(int i=0; i<10; i++) {
        sum += analogRead(phSensorPin);
        delay(10);
    }
    int rawValue = sum / 10;
    
    // Debug print
    Serial.print("pH Raw: "); Serial.println(rawValue);
    
    // Simple solution for your presentation:
    // Based on your readings, create ranges for different liquids
    float pH = 7.0; // Default to neutral
    
    if (rawValue > 4100) {
        pH = 2.5; // Very acidic (lemon juice)
    } 
    else if (rawValue > 3850) {
        pH = 3.0; // Acidic (vinegar)
    }
    else if (rawValue > 3650) {
        pH = 4.0; // Acidic (milk)
    }
    else if (rawValue > 3450) {
        pH = 6.5; // Slightly acidic (milk)
    }
    else if (rawValue > 3000) {
        pH = 7.0; // Neutral (water)
    }
    else {
        pH = 8.0; // Basic
    }
    
    Serial.print("Calculated pH: "); Serial.println(pH);
    return pH;
}

// ---- Color Sensor ----
const char* SensorManager::readColorSensor() {
    int red = getRed();
    int redValue = map(red, 280, 30, 255, 0); // Map the Red Color Value
    delay(200);

    int green = getGreen();
    int greenValue = map(green, 330, 30, 255, 0); // Map the Green Color Value
    delay(200);

    int blue = getBlue();
    int blueValue = map(blue, 320, 30, 255, 0); // Map the Blue Color Value
    delay(200);

    Serial.print("Red = ");
    Serial.print(red);
    Serial.print("    ");
    Serial.print("Green = ");
    Serial.print(green);
    Serial.print("    ");
    Serial.print("Blue = ");
    Serial.println(blue);

    return ColorNameString(redValue, greenValue, blueValue);
}

// ---- Methane Sensor (MQ4) ----
float SensorManager::readMethaneSensor() {
    const int NUM_READINGS = 15;
    int readings[NUM_READINGS];

    for (int i = 0; i < NUM_READINGS; i++) {
        readings[i] = analogRead(mq4Pin);
        delay(20);
    }
    std::sort(readings, readings + NUM_READINGS);
    long sum = 0;
    for (int i = 3; i < NUM_READINGS - 3; i++) sum += readings[i];
    int avg = sum / (NUM_READINGS - 6);

    int methaneValue = avg - MQ4_BASELINE;
    if (methaneValue < 0) methaneValue = 0;

    Serial.print("MQ4 Raw: "); Serial.print(avg);
    Serial.print(" | Methane (calibrated): "); Serial.println(methaneValue);
    return methaneValue;
}

// ---- Alcohol Sensor (MQ3) ----
float SensorManager::readAlcSensor() {
    const int NUM_READINGS = 15;
    int readings[NUM_READINGS];

    for (int i = 0; i < NUM_READINGS; i++) {
        readings[i] = analogRead(32); // MQ3 analog pin
        delay(20);
    }
    std::sort(readings, readings + NUM_READINGS);
    long sum = 0;
    for (int i = 3; i < NUM_READINGS - 3; i++) sum += readings[i];
    int avg = sum / (NUM_READINGS - 6);

    int alcoholValue = avg - MQ3_BASELINE;
    if (alcoholValue < 0) alcoholValue = 0;

    Serial.print("MQ3 Raw: "); Serial.print(avg);
    Serial.print(" | Alcohol (calibrated): "); Serial.println(alcoholValue);
    return alcoholValue;
}

// ---- Color Sensor Helper Functions ----
int SensorManager::getRed() {
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    return pulseIn(sensorOut, LOW);
}

int SensorManager::getGreen() {
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    return pulseIn(sensorOut, LOW);
}

int SensorManager::getBlue() {
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    return pulseIn(sensorOut, LOW);
}

SensorManager sensorManager;

void setup() {
    sensorManager.begin();
}

void loop() {
    Blynk.run();
    timerr.run();
}
