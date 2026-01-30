# 🌡️ Station Météo ESP32 avec DHT22

## 📋 Matériel requis

- **ESP32** (ou ESP8266)
- **Capteur DHT22** (température et humidité)
- **2 LEDs** (pour indiquer le mode C/F)
- **1 Bouton poussoir** (pour changer de mode)
- Résistances (220Ω pour LEDs, 10kΩ pour bouton)

## 🔌 Branchements

```
DHT22:
  - VCC → 3.3V
  - GND → GND
  - DATA → GPIO 3

LED Celsius (rouge):
  - Anode → GPIO 5
  - Cathode → GND (via résistance 220Ω)

LED Fahrenheit (bleue):
  - Anode → GPIO 4
  - Cathode → GND (via résistance 220Ω)

Bouton:
  - Pin 1 → GPIO 12
  - Pin 2 → GND
  - Pull-up interne activé
```

## 📚 Bibliothèques Arduino requises

Installez ces bibliothèques via le gestionnaire de bibliothèques Arduino IDE:

1. **DHT sensor library** by Adafruit
2. **Adafruit Unified Sensor**
3. **PubSubClient** by Nick O'Leary
4. **ArduinoJson** by Benoit Blanchon

### Installation:
```
Outils → Gérer les bibliothèques → Rechercher et installer:
- DHT sensor library
- Adafruit Unified Sensor
- PubSubClient
- ArduinoJson
```

## ⚙️ Configuration

Modifiez dans `station.ino` :

```cpp
// Configuration WiFi
const char* SSID = "VOTRE_WIFI";
const char* PASSWORD = "VOTRE_MOT_DE_PASSE";

// Configuration MQTT (déjà configuré pour votre broker)
const char* MQTT_BROKER = "captain.dev0.pandor.cloud";
const int MQTT_PORT = 1884;
const char* DEVICE_ID = "genders-01";
```

## 🚀 Déploiement

1. **Ouvrir Arduino IDE**
2. **Sélectionner la carte**: Outils → Type de carte → ESP32 Dev Module
3. **Sélectionner le port**: Outils → Port → COMx (votre port série)
4. **Téléverser le code**: Cliquer sur le bouton Upload (→)

## 📊 Fonctionnement

### Au démarrage:
- ✅ Connexion au WiFi
- ✅ Connexion au broker MQTT
- ✅ LED Celsius allumée (mode par défaut)

### En cours d'exécution:
- 📤 Envoi des données toutes les 2 secondes
- 🔘 Appuyer sur le bouton pour basculer entre °C et °F
- 💡 Les LEDs indiquent le mode actif

### Format des données envoyées:
```json
{
  "temp": 24.5,
  "hum": 65.2,
  "unit": "°C",
  "mode": "C"
}
```

## 🔍 Débogage

Ouvrez le **Moniteur Série** (115200 baud) pour voir:
- État de connexion WiFi
- Adresse IP obtenue
- Connexion MQTT
- Données du capteur
- Envoi MQTT

## ⚠️ Dépannage

| Problème | Solution |
|----------|----------|
| Erreur de compilation | Installer toutes les bibliothèques requises |
| Pas de connexion WiFi | Vérifier SSID et mot de passe |
| Pas de connexion MQTT | Vérifier l'adresse du broker et le port |
| Lectures NaN | Vérifier le branchement du DHT22 |
| LEDs ne s'allument pas | Vérifier GPIO et résistances |

## 📡 Topic MQTT

Les données sont publiées sur:
```
classroom/genders-01/telemetry
```

## 🌐 Visualisation

Les données apparaîtront automatiquement sur le frontend à l'adresse:
```
http://localhost:5173
```

(Assurez-vous que le bridge Node.js est en cours d'exécution)
