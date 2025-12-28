#define BLYNK_TEMPLATE_ID "TMPL30VVtr6hr"
#define BLYNK_TEMPLATE_NAME "iot project food adulteration"
#define BLYNK_AUTH_TOKEN "eQew5r1i4SIPeg_mPWaA1T_pX-3v9d6M"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Updated Sensor Pins according to your requirements
#define PH_PIN 35         // GPIO35 for pH sensor
#define MQ3_PIN 32        // GPIO32 for Alcohol (MQ3)
#define MQ4_PIN 33        // GPIO33 for Methane (MQ4)
#define S0 18             // GPIO18 for TCS3200
#define S1 19             // GPIO19 for TCS3200
#define S2 21             // GPIO21 for TCS3200
#define S3 22             // GPIO22 for TCS3200
#define COLOR_OUT 25      // GPIO25 for TCS3200 output

// WiFi Credentials
char ssid[] = "Kirtigami....";
char pass[] = "Kirti.gami";

// Blynk Auth Token
char auth[] = "eQew5r1i4SIPeg_mPWaA1T_pX-3v9d6M";

// Color Reading Variables
int redValue, greenValue, blueValue;

void setupColorSensor() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(COLOR_OUT, INPUT);

  // Setting frequency-scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

int readColor(String color) {
  if (color == "Red") {
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
  } else if (color == "Green") {
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
  } else if (color == "Blue") {
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
  }
  delay(50);
  return pulseIn(COLOR_OUT, LOW);
}

String getColorName(int r, int g, int b) {
  if (r < g && r < b) return "Red";
  else if (g < r && g < b) return "Green";
  else return "Blue";
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  Blynk.begin(auth, ssid, pass);
  setupColorSensor();
}

void loop() {
  Blynk.run();

  // pH Sensor Reading (GPIO35)
  int rawPH = analogRead(PH_PIN);
  float voltagePH = rawPH * (3.3 / 4095.0);
  float pHValue = 7 + ((2.5 - voltagePH) / 0.18);
  pHValue = constrain(pHValue, 0, 14);

  // Alcohol Sensor Reading (GPIO32)
  int alcoholRaw = analogRead(MQ3_PIN);
  float alcoholLevel = alcoholRaw * (3.3 / 4095.0);

  // Methane Sensor Reading (GPIO33)
  int methaneRaw = analogRead(MQ4_PIN);
  float methaneLevel = methaneRaw * (3.3 / 4095.0);

  // Color Sensor Reading
  redValue = readColor("Red");
  greenValue = readColor("Green");
  blueValue = readColor("Blue");
  String colorDetected = getColorName(redValue, greenValue, blueValue);

  // Food Status Logic
  String foodStatus = "Safe to Eat";
  if (pHValue < 6.5 || pHValue > 7.5 || 
      alcoholLevel > 1.0 || methaneLevel > 1.0 || 
      colorDetected == "Red") {
    foodStatus = "Adulterated / Not Safe";
  }

  // Serial Monitor Output
  Serial.print("pH: "); Serial.println(pHValue);
  Serial.print("Alcohol: "); Serial.println(alcoholLevel);
  Serial.print("Methane: "); Serial.println(methaneLevel);
  Serial.print("Color: "); Serial.println(colorDetected);
  Serial.print("R: "); Serial.print(redValue);
  Serial.print(" G: "); Serial.print(greenValue);
  Serial.print(" B: "); Serial.println(blueValue);
  Serial.print("Food Status: "); Serial.println(foodStatus);

  // Blynk Updates
  Blynk.virtualWrite(V1, pHValue);
  Blynk.virtualWrite(V2, alcoholLevel);
  Blynk.virtualWrite(V3, colorDetected);
  Blynk.virtualWrite(V32, methaneLevel);
  Blynk.virtualWrite(V4, foodStatus);

  delay(2000);
}
