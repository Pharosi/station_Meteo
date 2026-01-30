#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Configuration WiFi
const char* SSID = "Redmi Note 13";
const char* PASSWORD = "0987654321";

// Configuration MQTT
const char* MQTT_BROKER = "captain.dev0.pandor.cloud";
const int MQTT_PORT = 1884;
const char* DEVICE_ID = "genders-01";
const char* MQTT_TOPIC_TELEMETRY = "classroom/genders-01/telemetry";
const char* MQTT_TOPIC_COMMAND = "classroom/genders-01/command";

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
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message reçu sur [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  
  StaticJsonDocument<100> doc;
  DeserializationError error = deserializeJson(doc, message);
  
  if (!error && doc.containsKey("mode")) {
    String mode = doc["mode"];
    
    if (mode == "C") {
      modeForTempC = true;
      digitalWrite(LEDPIN_5, LOW);
      digitalWrite(LEDPIN_4, HIGH);
      Serial.println("Mode changé vers Celsius (°C)");
    } else if (mode == "F") {
      modeForTempC = false;
      digitalWrite(LEDPIN_5, HIGH);
      digitalWrite(LEDPIN_4, LOW);
      Serial.println("Mode changé vers Fahrenheit (°F)");
    }
  }
}

void setup() {
  pinMode(LEDPIN_5, OUTPUT);
  pinMode(LEDPIN_4, OUTPUT);
  pinMode(BUTTONPIN_12, INPUT_PULLUP);
  
  Serial.begin(115200);
  dht.begin();
  
  Serial.println("\n🔌 Connexion au WiFi...");
  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\n Connexion WiFi réussie !");
  Serial.print("📍 Adresse IP: ");
  Serial.println(WiFi.localIP());

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  
  digitalWrite(LEDPIN_5, HIGH);
  digitalWrite(LEDPIN_4, LOW);
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connexion au broker MQTT...");
    
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println(" Connecté !");
      mqttClient.subscribe(MQTT_TOPIC_COMMAND);
      Serial.print("Abonné au topic: ");
      Serial.println(MQTT_TOPIC_COMMAND);
    } else {
      Serial.print(" Échec, code: ");
      Serial.print(mqttClient.state());
      Serial.println(" → Nouvelle tentative dans 5s");
      delay(5000);
    }
  }
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

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
    if (isnan(tempC) || isnan(hum)) {
      Serial.println("Erreur de lecture du capteur DHT22");
      return;
    }

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

    StaticJsonDocument<200> doc;
    doc["temp"] = modeForTempC ? tempC : tempF;
    doc["hum"] = hum;
    doc["unit"] = modeForTempC ? "°C" : "°F";
    doc["mode"] = modeForTempC ? "C" : "F";
    
    char jsonBuffer[200];
    serializeJson(doc, jsonBuffer);

    if (mqttClient.publish(MQTT_TOPIC_TELEMETRY, jsonBuffer)) {
      Serial.print("Données envoyées sur MQTT: ");
      Serial.println(jsonBuffer);
    } else {
      Serial.println("Échec de l'envoi MQTT");
    }
  }
}