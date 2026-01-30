const mqtt = require("mqtt");

const MQTT_BROKER = "mqtt://captain.dev0.pandor.cloud:1884";
const DEVICE_ID = "genders-01";
const TELEMETRY_TOPIC = `classroom/${DEVICE_ID}/telemetry`;

const client = mqtt.connect(MQTT_BROKER, {
  clientId: `test-publisher-${Math.random().toString(16).slice(2)}`,
});

client.on("connect", () => {
  console.log("✅ Connecté au broker MQTT");
  console.log(`📤 Envoi de données de test sur ${TELEMETRY_TOPIC}\n`);

  // Envoyer des données de test toutes les 3 secondes
  setInterval(() => {
    const data = {
      temp: (20 + Math.random() * 10).toFixed(1),
      hum: (50 + Math.random() * 30).toFixed(1),
      unit: "°C",
      mode: "C",
    };

    client.publish(TELEMETRY_TOPIC, JSON.stringify(data));
    console.log(`📤 Envoyé: temp=${data.temp}°C, hum=${data.hum}%`);
  }, 3000);
});

client.on("error", (err) => {
  console.error("❌ Erreur MQTT:", err.message);
});
