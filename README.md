# 🌦️ Station Météo Connectée — HETIC WEB3

TP de station météo connectée utilisant un ESP32, MQTT et une interface web en temps réel.

---

# 🎯 Objectif

- Mesurer ou simuler la température et l’humidité
- Publier les données via MQTT (JSON)
- Afficher les données en temps réel sur une interface web
- Basculer entre °C / °F avec un bouton physique
- Indiquer l’unité active avec 2 LEDs

---

# 🔌 MQTT

- Broker : captain.dev0.pandor.cloud
- Port : 1884
- Authentification : aucune

## Topic utilisé

classroom/<device_id>/telemetry

Exemple :

classroom/genders-01/telemetry

---

# 📦 Structure du projet

station-meteo/
├── esp32/        # Code ESP32 (simulation + DHT22)
├── bridge/       # Bridge MQTT → WebSocket (Node.js)
├── frontend/     # Interface web (SvelteKit)
├── docs/         # Photos, TinkerCAD, schéma de câblage
└── README.md

---

# 🚀 Lancement

## Bridge (Node.js)

### Installation des dépendances (une seule fois)

```bash
cd bridge  
npm install mqtt ws
``` 

### Démarrage du bridge

```bash
node index.js  
```

WebSocket disponible sur :

ws://localhost:8080

---

## Frontend (SvelteKit)

### Installation des dépendances (une seule fois)

```bash
cd frontend/myapp  
npm install
```

### Démarrage du frontend

```bash
npm run dev  
```

Interface web accessible sur :

http://localhost:5173

⚠️ Le bridge doit être lancé avant le frontend.

---

## ESP32

- Ouvrir esp32/station.ino dans Arduino IDE
- Configurer le Wi-Fi
- Flasher l’ESP32
- Ouvrir le Serial Monitor (115200 bauds)

---

# 🧪 Mode simulation

Le projet fonctionne sans capteur grâce à un mode simulation.

Exemple de message MQTT :

{
  "device_id": "genders-01",
  "unit": "C",
  "temp": 22.4,
  "hum": 45.2,
  "batteryPct": 18
}

---

# 📄 Documentation

Le dossier docs/ contient :

- captures TinkerCAD
- photos du montage réel
- schéma de câblage

---

# 👥 Équipe
Raphael Paes
Kelian Halleray
Ian N'Da
Owen Lambert
Modestin Hounga

Projet pédagogique — HETIC WEB3
