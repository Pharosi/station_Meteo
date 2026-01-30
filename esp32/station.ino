#include <DHT.h>

#define DHTPIN 3
#define DHTTYPE DHT22

const int LEDPIN_5 = 5;
const int LEDPIN_4 = 4;
const int BUTTONPIN_12 = 12;

bool modeForTempC = false;  

int lastReading = HIGH;
int stableState = HIGH;
unsigned long lastChange = 0;
const unsigned long debounceMs = 30;

unsigned long lastDhtRead = 0;
const unsigned long dhtInterval = 2000; 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(LEDPIN_5, OUTPUT);
  pinMode(LEDPIN_4, OUTPUT);
  pinMode(BUTTONPIN_12, INPUT_PULLUP);
  dht.begin();
  Serial.begin(115200);


  digitalWrite(LEDPIN_5, HIGH);
  digitalWrite(LEDPIN_4, LOW);
}

void loop() {

  int reading = digitalRead(BUTTONPIN_12);

  if (reading != lastReading) {
    lastChange = millis();
    lastReading = reading;
  }

  if (millis() - lastChange > debounceMs) {
    if (stableState != reading) {
      stableState = reading;

      if (stableState == LOW) {
        modeForTempC = !modeForTempC;

        digitalWrite(LEDPIN_5, modeForTempC ? LOW : HIGH);
        digitalWrite(LEDPIN_4, modeForTempC ? HIGH : LOW); 

        Serial.print("Mode changé : ");
        Serial.println(modeForTempC ? "Celsius (°C)" : "Fahrenheit (°F)");
      }
    }
  }

  if (millis() - lastDhtRead >= dhtInterval) {
    lastDhtRead = millis();

    float tempC = dht.readTemperature();       
    float tempF = dht.readTemperature(true);   
    float hum = dht.readHumidity();

    Serial.print("Humidité: ");
    Serial.print(hum);
    Serial.print(" %  | Température: ");

    if (modeForTempC) {
      Serial.print(tempC);
      Serial.println(" °C");
    } else {
      Serial.print(tempF);
      Serial.println(" °F");
    }
  }
}