const WebSocket = require("ws");
const mqtt = require("mqtt");

// ================= CONFIG =================
const MQTT_BROKER = "mqtt://captain.dev0.pandor.cloud:1884";
const WEBSOCKET_PORT = 8080;

// Définition DEVICE_ID
const DEVICE_ID = "genders-01";

// Convention TP : classroom/<device_id>/telemetry
const TELEMETRY_TOPIC = `classroom/${DEVICE_ID}/telemetry`;
const COMMAND_TOPIC = `classroom/${DEVICE_ID}/command`;
// ==========================================

const clientMQTT = mqtt.connect(MQTT_BROKER, {
  clientId: `bridge-${DEVICE_ID}-${Math.random().toString(16).slice(2)}`,
  reconnectPeriod: 2000,
  connectTimeout: 10000,
});

// Clients WebSocket connectés
const clientsWebSocket = [];

function broadcastToWebClients(payload) {
  const msg = JSON.stringify(payload);

  clientsWebSocket.forEach((ws) => {
    if (ws.readyState === WebSocket.OPEN) {
      ws.send(msg);
    }
  });
}

// ================= MQTT =================
clientMQTT.on("connect", () => {
  console.log("✅ Connecté au broker MQTT");
  console.log(`📡 Abonnement au topic : ${TELEMETRY_TOPIC}\n`);

  clientMQTT.subscribe(TELEMETRY_TOPIC, (err) => {
    if (err) {
      console.error("❌ Erreur d'abonnement MQTT:", err.message);
    } else {
      console.log("✅ Abonnement OK. En attente de messages...\n");
    }
  });
});

clientMQTT.on("error", (err) => {
  console.error("❌ MQTT error:", err.message);
});

clientMQTT.on("message", (topic, message) => {
  const raw = message.toString();

  let data;
  try {
    data = JSON.parse(raw);
  } catch (_) {
    console.error("❌ Message non-JSON reçu:", raw);
    return;
  }

  // Log "tolérant" (ne dépend pas de tempC/humPct)
  const temp = data.temp ?? data.tempC ?? null;
  const hum = data.hum ?? data.humPct ?? null;
  const unit = data.unit ?? "";
  const mode = data.mode ?? "";

  console.log(
    `📩 ${topic} | device=${DEVICE_ID} | temp=${temp ?? "?"}${unit} | hum=${hum ?? "?"}% | mode=${mode}`
  );

  // Payload envoyé au navigateur
  const messageAvecTopic = {
    topic,
    deviceId: DEVICE_ID,
    data,
  };

  broadcastToWebClients(messageAvecTopic);
});

// ================= WEBSOCKET =================
const serveurWebSocket = new WebSocket.Server({ port: WEBSOCKET_PORT });

serveurWebSocket.on("connection", (ws) => {
  console.log("🌐 Nouveau client web connecté");
  clientsWebSocket.push(ws);

  // Message de bienvenue (utile pour debug front)
  ws.send(
    JSON.stringify({
      type: "info",
      msg: "WS connected",
      telemetryTopic: TELEMETRY_TOPIC,
      deviceId: DEVICE_ID,
      at: Date.now(),
    })
  );

  // Gestion des messages du client WebSocket (frontend)
  ws.on("message", (message) => {
    try {
      const data = JSON.parse(message.toString());
      
      // Si c'est une commande de changement de mode
      if (data.type === "command" && data.command === "changeMode") {
        const payload = JSON.stringify({ mode: data.mode });
        
        clientMQTT.publish(COMMAND_TOPIC, payload, (err) => {
          if (err) {
            console.error("❌ Erreur d'envoi de commande MQTT:", err.message);
          } else {
            console.log(`📤 Commande envoyée: ${COMMAND_TOPIC} | ${payload}`);
          }
        });
      }
    } catch (error) {
      console.error("❌ Erreur de parsing du message WebSocket:", error.message);
    }
  });

  ws.on("close", () => {
    const index = clientsWebSocket.indexOf(ws);
    if (index > -1) clientsWebSocket.splice(index, 1);
    console.log("🌐 Client web déconnecté");
  });
});

console.log(`🚀 Serveur WebSocket démarré sur ws://localhost:${WEBSOCKET_PORT}`);
console.log("En attente de connexions...\n");