# 🔄 Synchronisation Bidirectionnelle Frontend ↔ Arduino

## Architecture de communication

```
┌─────────────┐        ┌─────────────┐        ┌─────────────┐
│   Frontend  │◄──────►│   Bridge    │◄──────►│   Arduino   │
│  (Svelte)   │   WS   │  (Node.js)  │  MQTT  │   (ESP32)   │
└─────────────┘        └─────────────┘        └─────────────┘
```

## 📡 Topics MQTT

### 1. **Télémétrie** (Arduino → Frontend)
- **Topic**: `classroom/genders-01/telemetry`
- **Direction**: Arduino → MQTT → Bridge → WebSocket → Frontend
- **Contenu**: 
```json
{
  "temp": 24.5,
  "hum": 65.2,
  "unit": "°C",
  "mode": "C"
}
```
- **Fréquence**: Toutes les 2 secondes

### 2. **Commandes** (Frontend → Arduino)
- **Topic**: `classroom/genders-01/command`
- **Direction**: Frontend → WebSocket → Bridge → MQTT → Arduino
- **Contenu**:
```json
{
  "mode": "F"
}
```
- **Déclenchement**: Clic sur le bouton de changement de mode

## 🔀 Flux de synchronisation

### Scénario 1: Clic sur le bouton du frontend

```
1. Utilisateur clique sur "🌡️ Celsius" (veut passer en Fahrenheit)
   ↓
2. Frontend change modeAffichage = 'F'
   ↓
3. Frontend envoie via WebSocket: 
   { type: "command", command: "changeMode", mode: "F" }
   ↓
4. Bridge reçoit et publie sur MQTT:
   Topic: classroom/genders-01/command
   Payload: { "mode": "F" }
   ↓
5. Arduino reçoit la commande
   ↓
6. Arduino:
   - Change modeForTempC = false
   - Allume LED bleue (GPIO 4)
   - Éteint LED rouge (GPIO 5)
   ↓
7. Au prochain envoi de données (2s), Arduino envoie:
   { "temp": 76.1, "hum": 65.2, "unit": "°F", "mode": "F" }
   ↓
8. Frontend reçoit et confirme la synchronisation ✅
```

### Scénario 2: Appui sur le bouton physique de l'Arduino

```
1. Utilisateur appuie sur le bouton physique (GPIO 12)
   ↓
2. Arduino:
   - Change modeForTempC = true
   - Allume LED rouge (GPIO 5)
   - Éteint LED bleue (GPIO 4)
   ↓
3. Arduino envoie immédiatement les données:
   { "temp": 24.5, "hum": 65.2, "unit": "°C", "mode": "C" }
   ↓
4. Bridge relaie vers le frontend via WebSocket
   ↓
5. Frontend détecte le changement de mode:
   modeAffichage = donnees.mode  // 'C'
   ↓
6. Interface frontend se met à jour:
   - Bouton affiche "🌡️ Celsius"
   - Températures converties en °C
   ✅ Synchronisé !
```

## 🎯 Fonctionnalités

### ✅ Ce qui fonctionne:

1. **Double contrôle**:
   - Bouton frontend → Change l'Arduino
   - Bouton physique → Change le frontend

2. **Synchronisation automatique**:
   - Les deux interfaces restent toujours alignées
   - Pas de désynchronisation possible

3. **Feedback visuel**:
   - LEDs sur Arduino
   - Bouton animé sur frontend
   - Console logs pour debug

4. **Gestion d'erreurs**:
   - Vérification de connexion WebSocket
   - Logs d'erreurs MQTT
   - Reconnexion automatique

## 🔧 Variables de synchronisation

### Frontend (Svelte)
```javascript
let modeAffichage = $state('C');  // État réactif
let wsConnection = null;           // Référence WebSocket
```

### Arduino (C++)
```cpp
bool modeForTempC = false;        // true = Celsius, false = Fahrenheit
const char* MQTT_TOPIC_COMMAND = "classroom/genders-01/command";
```

### Bridge (Node.js)
```javascript
const COMMAND_TOPIC = `classroom/${DEVICE_ID}/command`;
// Relaye les commandes WebSocket → MQTT
```

## 🧪 Tester la synchronisation

### Test 1: Frontend → Arduino
1. Ouvrir le frontend (http://localhost:5173)
2. Ouvrir le moniteur série de l'Arduino (115200 baud)
3. Cliquer sur le bouton "🌡️ Celsius" dans le frontend
4. Observer sur le moniteur série:
   ```
   📩 Message reçu sur [classroom/genders-01/command]: {"mode":"F"}
   ✅ Mode changé vers Fahrenheit (°F)
   ```
5. Vérifier que la LED bleue (GPIO 4) s'allume

### Test 2: Arduino → Frontend
1. Appuyer sur le bouton physique de l'Arduino (GPIO 12)
2. Observer sur le moniteur série:
   ```
   Mode changé : Celsius (°C)
   📤 Données envoyées sur MQTT: {"temp":24.5,"hum":65.2,"unit":"°C","mode":"C"}
   ```
3. Vérifier dans le frontend:
   - Le bouton affiche "🌡️ Celsius"
   - Les températures sont en °C
   - La console browser montre la synchronisation

### Test 3: Synchronisation en temps réel
1. Ouvrir 2 onglets du frontend
2. Cliquer sur le bouton dans l'onglet 1
3. Vérifier que l'onglet 2 se synchronise aussi
4. Appuyer sur le bouton physique
5. Vérifier que les 2 onglets changent

## 📊 Messages de debug

### Console Frontend
```
📨 Message WebSocket reçu: {...}
✅ Station mise à jour: Station Météo DHT22 - 24.5°C - 65.2%
📤 Commande envoyée: changement de mode vers F
```

### Console Bridge
```
📩 classroom/genders-01/telemetry | device=genders-01 | temp=24.5°C | hum=65.2% | mode=C
📤 Commande envoyée: classroom/genders-01/command | {"mode":"F"}
```

### Moniteur Série Arduino
```
📩 Message reçu sur [classroom/genders-01/command]: {"mode":"F"}
✅ Mode changé vers Fahrenheit (°F)
📤 Données envoyées sur MQTT: {"temp":76.1,"hum":65.2,"unit":"°F","mode":"F"}
```

## ⚠️ Points d'attention

1. **Toujours démarrer dans l'ordre**:
   - Arduino (connexion WiFi + MQTT)
   - Bridge (`node index.js`)
   - Frontend (`npm run dev`)

2. **Vérifier les connexions**:
   - Arduino connecté au WiFi
   - Arduino abonné au topic de commandes
   - WebSocket connecté (🟢 dans le frontend)

3. **En cas de désynchronisation**:
   - Redémarrer l'Arduino
   - Les données avec le mode actuel seront envoyées
   - Le frontend se synchronisera automatiquement
