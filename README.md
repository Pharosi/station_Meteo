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

## Démarrage

### Lancer le bridge (Terminal 1)
```bash
cd bridge
node index.js
```



### Lancer le front (Terminal 2)
```bash
cd front/myapp
npm run dev
```

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
  "mode": "sim",
  "unit": "C",
  "temp": 22.4,
  "hum": 45.2,
  "ts": 1769698000
}

---

# 📄 Documentation

Le dossier docs/ contient :

- captures TinkerCAD
- photos du montage réel
- schéma de câblage

---

# 👥 Équipe

- Nom Prénom
- Nom Prénom
- Nom Prénom

---

Projet pédagogique — HETIC WEB3