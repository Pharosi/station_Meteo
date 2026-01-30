#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Configuration WiFi
const char* SSID = "Redmi Note 13";
const char* PASSWORD = "0987654321";
<<<<<<< HEAD

// Configuration MQTT
const char* MQTT_BROKER = "captain.dev0.pandor.cloud";
const int MQTT_PORT = 1884;
const char* DEVICE_ID = "genders-01";
const char* MQTT_TOPIC_TELEMETRY = "classroom/genders-01/telemetry";
const char* MQTT_TOPIC_COMMAND = "classroom/genders-01/command";
=======
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))

// Configuration MQTT
const char* MQTT_BROKER = "captain.dev0.pandor.cloud";
const int MQTT_PORT = 1884;
const char* DEVICE_ID = "Mazafakas-01";
const char* MQTT_TOPIC = "Mazafakas-01/telemetry";

#define DHTPIN 14
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
<<<<<<< HEAD

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
=======
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))

void setup() {
  pinMode(LEDPIN_5, OUTPUT);
  pinMode(LEDPIN_4, OUTPUT);
  pinMode(BUTTONPIN_12, INPUT_PULLUP);
  
  Serial.begin(115200);
  dht.begin();
  
<<<<<<< HEAD
=======
  // Connexion WiFi
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))
  Serial.println("\n🔌 Connexion au WiFi...");
  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

<<<<<<< HEAD
  Serial.println("\n Connexion WiFi réussie !");
  Serial.print("📍 Adresse IP: ");
  Serial.println(WiFi.localIP());

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
=======
  Serial.println("\n✅ Connexion WiFi réussie !");
  Serial.print("📍 Adresse IP: ");
  Serial.println(WiFi.localIP());

  // Configuration MQTT
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))
  
  digitalWrite(LEDPIN_5, HIGH);
  digitalWrite(LEDPIN_4, LOW);
}

<<<<<<< HEAD
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
=======
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))

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
  
  if (!error && doc.containsKey("unit")) {
    String unit = doc["unit"];
    
    if (unit == "°C") {
      modeForTempC = true;
      digitalWrite(LEDPIN_5, LOW);
      digitalWrite(LEDPIN_4, HIGH);
      Serial.println("Unité changé vers Celsius (°C)");
    } else if (unit == "°F") {
      modeForTempC = false;
      digitalWrite(LEDPIN_5, HIGH);
      digitalWrite(LEDPIN_4, LOW);
      Serial.println("Unité changé vers Fahrenheit (°F)");
    }
  }
}


void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("------> Connexion au broker MQTT...");
    
    String clientId = "ESP32-" + DEVICE_ID + "-";
    clientId += String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println(" ------> Connecté !");
      mqttClient.subscribe(MQTT_TOPIC, mqttCallback);
    } else {
      Serial.print(" ------> Échec, code: ");
      Serial.print(mqttClient.state());
      Serial.println(" ------> Nouvelle tentative dans 5s");
      delay(5000);
    }
  }
}

void loop() {
  // Vérifier la connexion MQTT
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // Gestion du bouton pour changer de mode
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

  // Lecture du capteur DHT22 et envoi MQTT
  if (millis() - lastDhtRead >= dhtInterval) {
    lastDhtRead = millis();

    float tempC = dht.readTemperature();       
    float tempF = dht.readTemperature(true);   
    float hum = dht.readHumidity();
    if (isnan(tempC) || isnan(hum)) {
      Serial.println("Erreur de lecture du capteur DHT22");
      return;
    }

    // Vérifier que les données sont valides
    if (isnan(tempC) || isnan(hum)) {
      Serial.println("------> Erreur de lecture du capteur DHT22");
      return;
    }

    // Affichage sur le moniteur série
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

<<<<<<< HEAD
=======
    // Créer le payload JSON
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))
    StaticJsonDocument<200> doc;
    doc["temp"] = modeForTempC ? tempC : tempF;
    doc["hum"] = hum;
    doc["unit"] = modeForTempC ? "°C" : "°F";
<<<<<<< HEAD
    doc["mode"] = modeForTempC ? "C" : "F";
=======
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))
    
    char jsonBuffer[200];
    serializeJson(doc, jsonBuffer);

<<<<<<< HEAD
    if (mqttClient.publish(MQTT_TOPIC_TELEMETRY, jsonBuffer)) {
      Serial.print("Données envoyées sur MQTT: ");
      Serial.println(jsonBuffer);
    } else {
      Serial.println("Échec de l'envoi MQTT");
=======
    // Publier sur MQTT
    if (mqttClient.publish(MQTT_TOPIC, jsonBuffer)) {
      Serial.print("Données envoyées sur MQTT -> ");
      Serial.println(jsonBuffer);
    } else {
      Serial.println("Échec de l'envoi MQTT ->");
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))
    }
  }
}
