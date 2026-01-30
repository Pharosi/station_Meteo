<script>
  const URL_WEBSOCKET = "ws://localhost:8080";
  const TIMEOUT_OFFLINE = 60000; // 60 secondes sans données

  // Mapping deviceId -> lieu
  const LIEUX = {
    "esp32-01": "Paris",
    "esp32-02": "Lyon",
    "esp32-03": "Marseille",
    "esp32-04": "Bordeaux",
    "esp32-05": "Toulouse",
    "esp32-06": "Hetic",
  };

  // État de la connexion WebSocket
  let statutConnexion = "déconnecté";

  // Stockage des données de chaque station
  let stations = $state({});

  // Connexion au WebSocket
  function connecter() {
    statutConnexion = "connexion...";

    const ws = new WebSocket(URL_WEBSOCKET);

    ws.onopen = () => {
      statutConnexion = "connecté";
      console.log("Connecté au serveur");
    };

    ws.onerror = () => {
      statutConnexion = "erreur";
    };

    ws.onclose = () => {
      statutConnexion = "déconnecté";
      // Tenter de reconnecter après 3 secondes
      setTimeout(connecter, 3000);
    };

    ws.onmessage = (event) => {
      try {
        const message = JSON.parse(event.data);

        const deviceId = message.deviceId ?? message.data?.device_id ?? "unknown";
        const donnees = message.data ?? {};

        // ✅ Unité envoyée par l’ESP32 ("C" ou "F")
        const unit = donnees.unit === "F" || donnees.unit === "C" ? donnees.unit : "C";

        // ✅ Températures possibles
        const tempC = donnees.tempC !== undefined ? Number(donnees.tempC) : null;
        const tempF = donnees.tempF !== undefined ? Number(donnees.tempF) : null;

        // ✅ Fallback si certains payloads utilisent "temp"
        const tempGeneric = donnees.temp !== undefined ? Number(donnees.temp) : null;

        // ✅ On choisit la bonne température selon l’unité
        const temperature = unit === "F" ? tempF ?? tempGeneric : tempC ?? tempGeneric;

        // ✅ Humidité (supporte humPct ou hum)
        const hum = donnees.humPct ?? donnees.hum;
        const humidite = hum !== undefined ? Number(hum) : null;

        // ✅ Batterie optionnelle
        const batterie = donnees.batteryPct !== undefined ? Number(donnees.batteryPct) : 100;

        stations[deviceId] = {
          lieu: LIEUX[deviceId] || deviceId,
          unit,
          temperature,
          humidite,
          batterie,
          derniereMAJ: Date.now(),
          deviceId
        };

        console.log(`${stations[deviceId].lieu}: ${temperature ?? "?"}°${unit}`);
      } catch (error) {
        console.error("Erreur parsing:", error);
      }
    };
  }

  // Vérifier si une station est en ligne
  function estEnLigne(station) {
    if (!station || !station.derniereMAJ) return false;
    return Date.now() - station.derniereMAJ < TIMEOUT_OFFLINE;
  }

  // Calculer la moyenne de température (sur les stations en ligne)
  function moyenneTemperature() {
    const stationsEnLigne = Object.values(stations).filter(estEnLigne);
    if (stationsEnLigne.length === 0) return null;

    const stationsAvecTemp = stationsEnLigne.filter((s) => typeof s.temperature === "number");
    if (stationsAvecTemp.length === 0) return null;

    const somme = stationsAvecTemp.reduce((acc, s) => acc + s.temperature, 0);
    return (somme / stationsAvecTemp.length).toFixed(1);
  }

  // Unité de la moyenne (si tout le monde a la même unité)
  function moyenneUnit() {
    const stationsEnLigne = Object.values(stations).filter(estEnLigne);
    if (stationsEnLigne.length === 0) return "C";

    const units = new Set(stationsEnLigne.map((s) => s.unit || "C"));
    return units.size === 1 ? [...units][0] : "C";
  }

  // Calculer la moyenne d'humidité
  function moyenneHumidite() {
    const stationsEnLigne = Object.values(stations).filter(estEnLigne);
    if (stationsEnLigne.length === 0) return null;

    const stationsAvecHum = stationsEnLigne.filter((s) => typeof s.humidite === "number");
    if (stationsAvecHum.length === 0) return null;

    const somme = stationsAvecHum.reduce((acc, s) => acc + s.humidite, 0);
    return (somme / stationsAvecHum.length).toFixed(1);
  }

  function classeBatterie(pct) {
    if (pct > 60) return "batterie-ok";
    if (pct > 30) return "batterie-moyen";
    return "batterie-faible";
  }

  $effect(() => {
    connecter();
  });
</script>

<main>
  <header>
    <h1>🌦️ Stations Météo IoT</h1>
    <div class="statut-connexion {statutConnexion === 'connecté' ? 'connecte' : ''}">
      {statutConnexion === 'connecté' ? '🟢' : '🔴'} {statutConnexion}
    </div>
  </header>

  {#if Object.keys(stations).length > 0}
    <section class="moyennes">
      <div class="moyenne-item">
        <span class="icone">🌡️</span>
        <div>
          <div class="label">Température moyenne</div>
          <div class="valeur">{moyenneTemperature() ?? '--'}°{moyenneUnit()}</div>
        </div>
      </div>
      <div class="moyenne-item">
        <span class="icone">💧</span>
        <div>
          <div class="label">Humidité moyenne</div>
          <div class="valeur">{moyenneHumidite() ?? '--'}%</div>
        </div>
      </div>
    </section>
  {/if}

  <section class="grille-stations">
    {#each Object.values(stations) as station (station.deviceId)}
      {@const enLigne = estEnLigne(station)}
      <article class="carte-station {enLigne ? 'online' : 'offline'}">
        <div class="en-tete">
          <h2>{station.lieu}</h2>
          <span class="badge-statut">
            {enLigne ? '🟢 En ligne' : '🔴 Hors ligne'}
          </span>
        </div>

        <div class="mesures">
          <div class="mesure">
            <span class="icone">🌡️</span>
            <div>
              <div class="label">Température</div>
              <div class="valeur-principale">
                {station.temperature ?? '--'}°{station.unit ?? 'C'}
              </div>
            </div>
          </div>

          <div class="mesure">
            <span class="icone">💧</span>
            <div>
              <div class="label">Humidité</div>
              <div class="valeur">{station.humidite ?? '--'}%</div>
            </div>
          </div>

          <div class="mesure">
            <span class="icone {classeBatterie(station.batterie)}">🔋</span>
            <div>
              <div class="label">Batterie</div>
              <div class="valeur">{station.batterie ?? 100}%</div>
            </div>
          </div>
        </div>

        <div class="pied">
          <small>
            📡 {station.deviceId} •
            Mis à jour il y a {Math.floor((Date.now() - station.derniereMAJ) / 1000)}s
          </small>
        </div>
      </article>
    {:else}
      <div class="aucune-donnee">
        <p>⏳ En attente de données des stations...</p>
        <p><small>Vérifiez que le bridge est bien démarré</small></p>
      </div>
    {/each}
  </section>
</main>

<style>
  /* styliser grâce à l'ia */
  :global(body) {
    margin: 0;
    padding: 0;
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    min-height: 100vh;
  }

  main {
    max-width: 1400px;
    margin: 0 auto;
    padding: 2rem;
  }

  header {
    text-align: center;
    margin-bottom: 3rem;
    color: white;
  }

  header h1 {
    font-size: 3rem;
    margin: 0 0 1rem 0;
    text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.2);
  }

  .statut-connexion {
    display: inline-block;
    padding: 0.5rem 1.5rem;
    background: rgba(255, 255, 255, 0.2);
    border-radius: 2rem;
    backdrop-filter: blur(10px);
    font-weight: 500;
    text-transform: uppercase;
    font-size: 0.9rem;
  }

  .statut-connexion.connecte {
    background: rgba(76, 175, 80, 0.3);
  }

  /* Moyennes globales */
  .moyennes {
    display: flex;
    gap: 1.5rem;
    justify-content: center;
    margin-bottom: 2rem;
  }

  .moyenne-item {
    background: white;
    padding: 1.5rem 2rem;
    border-radius: 1rem;
    display: flex;
    align-items: center;
    gap: 1rem;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  }

  .moyenne-item .icone {
    font-size: 2.5rem;
  }

  .moyenne-item .label {
    font-size: 0.85rem;
    color: #666;
    margin-bottom: 0.25rem;
  }

  .moyenne-item .valeur {
    font-size: 1.8rem;
    font-weight: bold;
    color: #333;
  }

  /* Grille des stations */
  .grille-stations {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
    gap: 1.5rem;
  }

  .carte-station {
    background: white;
    border-radius: 1rem;
    padding: 1.5rem;
    box-shadow: 0 8px 16px rgba(0, 0, 0, 0.1);
    transition: transform 0.3s ease, box-shadow 0.3s ease;
  }

  .carte-station:hover {
    transform: translateY(-5px);
    box-shadow: 0 12px 24px rgba(0, 0, 0, 0.15);
  }

  .carte-station.offline {
    opacity: 0.6;
    background: #f5f5f5;
  }

  .en-tete {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 1.5rem;
    padding-bottom: 1rem;
    border-bottom: 2px solid #f0f0f0;
  }

  .en-tete h2 {
    margin: 0;
    font-size: 1.5rem;
    color: #333;
  }

  .badge-statut {
    font-size: 0.75rem;
    padding: 0.4rem 0.8rem;
    border-radius: 1rem;
    background: #e8f5e9;
    color: #2e7d32;
    font-weight: 600;
  }

  .carte-station.offline .badge-statut {
    background: #ffebee;
    color: #c62828;
  }

  .mesures {
    display: flex;
    flex-direction: column;
    gap: 1rem;
    margin-bottom: 1rem;
  }

  .mesure {
    display: flex;
    align-items: center;
    gap: 1rem;
  }

  .mesure .icone {
    font-size: 2rem;
  }

  .mesure .label {
    font-size: 0.85rem;
    color: #666;
  }

  .mesure .valeur {
    font-size: 1.3rem;
    font-weight: bold;
    color: #333;
  }

  .mesure .valeur-principale {
    font-size: 2rem;
    font-weight: bold;
    color: #667eea;
  }

  .batterie-ok {
    filter: grayscale(0);
  }

  .batterie-moyen {
    filter: hue-rotate(45deg);
  }

  .batterie-faible {
    filter: hue-rotate(0deg) saturate(2);
  }

  .pied {
    margin-top: 1rem;
    padding-top: 1rem;
    border-top: 1px solid #f0f0f0;
  }

  .pied small {
    color: #999;
    font-size: 0.75rem;
  }

  .aucune-donnee {
    grid-column: 1 / -1;
    text-align: center;
    padding: 4rem 2rem;
    background: white;
    border-radius: 1rem;
    color: #666;
  }

  .aucune-donnee p {
    margin: 0.5rem 0;
    font-size: 1.2rem;
  }
</style>