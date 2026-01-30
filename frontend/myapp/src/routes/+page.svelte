<script>
    const URL_WEBSOCKET = "ws://localhost:8080";
    const TIMEOUT_OFFLINE = 60000;
    
    const LIEUX = {
        'genders-01': 'Station Météo DHT22',
        'esp32-01': 'Paris',
        'esp32-02': 'Lyon',
        'esp32-03': 'Marseille',
        'esp32-04': 'Bordeaux',
        'esp32-05': 'Toulouse',
        'esp32-06': 'Hetic'
    };

    let statutConnexion = $state("déconnecté");
    
<<<<<<< HEAD
    let modeAffichage = $state('C'); // 'C' ou 'F'
=======
    let modeAffichage = $state('C');
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))
    
    let stations = $state({});
    
    let wsConnection = null;
    
<<<<<<< HEAD
    // Connexion au WebSocket
=======
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))
    function connecter() {
        statutConnexion = "connexion...";

        const ws = new WebSocket(URL_WEBSOCKET);
        wsConnection = ws;

        ws.onopen = () => {
            statutConnexion = "connecté";
            console.log("Connecté au serveur");
        }

        ws.onerror = () => {
            statutConnexion = "erreur";
        }

        ws.onclose = () => {
            statutConnexion = "déconnecté";
            setTimeout(connecter, 3000);
        }

        ws.onmessage = (event) => {
            try {
                    const message = JSON.parse(event.data);
                    console.log("Message WebSocket reçu:", message);

                    const deviceId = message.deviceId ?? message.data?.device_id ?? "unknown";
                    const donnees = message.data ?? {};

<<<<<<< HEAD
=======
                    if (donnees.source === "button" && donnees.mode) {
                        modeAffichage = donnees.mode;
                        console.log(`🔘 Bouton physique pressé - Mode changé vers ${donnees.mode === 'C' ? 'Celsius' : 'Fahrenheit'}`);
                        return;
                    }

>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))
                    const temp = donnees.tempC ?? donnees.temp;
                    const hum = donnees.humPct ?? donnees.hum;

                    const temperature = temp !== undefined ? Number(temp) : null;
                    const humidite = hum !== undefined ? Number(hum) : null;

<<<<<<< HEAD
                    const unit = donnees.unit ?? "°C";
                    const mode = donnees.mode ?? "C";

                    if (mode) {
                        modeAffichage = mode;
                    }

=======
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))
                    const batterie = donnees.batteryPct !== undefined ? Number(donnees.batteryPct) : 100;

                    stations[deviceId] = {
                    lieu: LIEUX[deviceId] || deviceId,
                    temperature,
                    humidite,
                    batterie,
                    derniereMAJ: Date.now(),
                    deviceId
                };

                console.log(`Station mise à jour: ${stations[deviceId].lieu} - ${temperature ?? "?"}${unit} - ${humidite ?? "?"}%`);
                console.log("Stations actuelles:", $state.snapshot(stations));
            } catch (error) {
                console.error("Erreur parsing:", error);
            }
        };
    }

    function estEnLigne(station) {
        if (!station || !station.derniereMAJ) return false;
        return (Date.now() - station.derniereMAJ) < TIMEOUT_OFFLINE;
    }

    function moyenneTemperature() {
        const stationsEnLigne = Object.values(stations).filter(estEnLigne);
        if (stationsEnLigne.length === 0) return null;
        
        const somme = stationsEnLigne.reduce((acc, s) => acc + (s.temperature || 0), 0);
        const moyenneC = somme / stationsEnLigne.length;
        return convertirTemp(moyenneC);
    }

    function moyenneHumidite() {
        const stationsEnLigne = Object.values(stations).filter(estEnLigne);
        if (stationsEnLigne.length === 0) return null;
        
        const somme = stationsEnLigne.reduce((acc, s) => acc + (s.humidite || 0), 0);
        return (somme / stationsEnLigne.length).toFixed(1);
    }

    function getUniteActuelle() {
        return modeAffichage === 'C' ? '°C' : '°F';
    }

    function convertirTemp(tempC) {
        if (tempC === null || tempC === undefined) return null;
        if (modeAffichage === 'F') {
            return (tempC * 9/5 + 32).toFixed(1);
        }
        return Number(tempC).toFixed(1);
    }

    function toggleMode() {
        const nouveauMode = modeAffichage === 'C' ? 'F' : 'C';
        modeAffichage = nouveauMode;
        
        if (wsConnection && wsConnection.readyState === WebSocket.OPEN) {
            const commande = {
                type: "command",
                command: "changeMode",
                mode: nouveauMode
            };
            wsConnection.send(JSON.stringify(commande));
<<<<<<< HEAD
            console.log(`Commande envoyée: changement de mode vers ${nouveauMode}`);
        } else {
            console.warn("WebSocket non connecté, impossible d'envoyer la commande");
=======
            console.log(`📤 Commande envoyée à l'Arduino: mode ${nouveauMode}`);
        } else {
            console.warn("⚠️ WebSocket non connecté, impossible d'envoyer la commande");
>>>>>>> 8b455b6 (Test Finaux avec button qui marche dans un sens (X Front -> Material))
        }
    }

    function classeBatterie(pct) {
        if (pct > 60) return 'batterie-ok';
        if (pct > 30) return 'batterie-moyen';
        return 'batterie-faible';
    }

    $effect(() => {
        connecter();
    });
</script>

<main>
    <header>
        <h1>🌦️ Stations Météo IoT</h1>
        <div class="header-controls">
            <div class="statut-connexion {statutConnexion === 'connecté' ? 'connecte' : ''}">
                {statutConnexion === 'connecté' ? '🟢' : '🔴'} {statutConnexion}
            </div>
            <button class="toggle-mode" onclick={toggleMode}>
                {modeAffichage === 'C' ? '🌡️ Celsius' : '🌡️ Fahrenheit'}
                <span class="toggle-icon">⇄</span>
            </button>
        </div>
    </header>

    {#if Object.keys(stations).length > 0}
        <section class="moyennes">
            <div class="moyenne-item">
                <span class="icone">🌡️</span>
                <div>
                    <div class="label">Température {Object.keys(stations).length > 1 ? 'moyenne' : 'actuelle'}</div>
                    <div class="valeur">{moyenneTemperature() ?? '--'}{getUniteActuelle()}</div>
                </div>
            </div>
            <div class="moyenne-item">
                <span class="icone">💧</span>
                <div>
                    <div class="label">Humidité {Object.keys(stations).length > 1 ? 'moyenne' : 'actuelle'}</div>
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
                    <div class="badges">
                        {#if station.mode}
                            <span class="badge-mode">
                                {station.mode === 'C' ? '🌡️ Celsius' : '🌡️ Fahrenheit'}
                            </span>
                        {/if}
                        <span class="badge-statut">
                            {enLigne ? '🟢 En ligne' : '🔴 Hors ligne'}
                        </span>
                    </div>
                </div>
                
                <div class="mesures">
                    <div class="mesure">
                        <span class="icone">🌡️</span>
                        <div>
                            <div class="label">Température</div>
                            <div class="valeur-principale">{convertirTemp(station.temperature) ?? '--'}{getUniteActuelle()}</div>
                        </div>
                    </div>
                    
                    <div class="mesure">
                        <span class="icone">💧</span>
                        <div>
                            <div class="label">Humidité</div>
                            <div class="valeur">{station.humidite ?? '--'}%</div>
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
        text-shadow: 2px 2px 4px rgba(0,0,0,0.2);
    }

    .header-controls {
        display: flex;
        justify-content: center;
        align-items: center;
        gap: 1rem;
        flex-wrap: wrap;
    }

    .statut-connexion {
        display: inline-block;
        padding: 0.5rem 1.5rem;
        background: rgba(255,255,255,0.2);
        border-radius: 2rem;
        backdrop-filter: blur(10px);
        font-weight: 500;
        text-transform: uppercase;
        font-size: 0.9rem;
    }

    .statut-connexion.connecte {
        background: rgba(76, 175, 80, 0.3);
    }

    .toggle-mode {
        padding: 0.6rem 1.5rem;
        background: linear-gradient(135deg, #ffd89b 0%, #19547b 100%);
        color: white;
        border: none;
        border-radius: 2rem;
        font-weight: 600;
        font-size: 0.9rem;
        cursor: pointer;
        transition: all 0.3s ease;
        display: flex;
        align-items: center;
        gap: 0.5rem;
        box-shadow: 0 4px 6px rgba(0,0,0,0.2);
    }

    .toggle-mode:hover {
        transform: translateY(-2px);
        box-shadow: 0 6px 12px rgba(0,0,0,0.3);
    }

    .toggle-mode:active {
        transform: translateY(0);
    }

    .toggle-icon {
        font-size: 1.2rem;
        display: inline-block;
        transition: transform 0.3s ease;
    }

    .toggle-mode:hover .toggle-icon {
        transform: rotate(180deg);
    }

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
        box-shadow: 0 4px 6px rgba(0,0,0,0.1);
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

    .grille-stations {
        display: grid;
        grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
        gap: 1.5rem;
    }

    .carte-station {
        background: white;
        border-radius: 1rem;
        padding: 1.5rem;
        box-shadow: 0 8px 16px rgba(0,0,0,0.1);
        transition: transform 0.3s ease, box-shadow 0.3s ease;
    }

    .carte-station:hover {
        transform: translateY(-5px);
        box-shadow: 0 12px 24px rgba(0,0,0,0.15);
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

    .badges {
        display: flex;
        gap: 0.5rem;
        align-items: center;
        flex-wrap: wrap;
    }

    .badge-statut {
        font-size: 0.75rem;
        padding: 0.4rem 0.8rem;
        border-radius: 1rem;
        background: #e8f5e9;
        color: #2e7d32;
        font-weight: 600;
    }

    .badge-mode {
        font-size: 0.75rem;
        padding: 0.4rem 0.8rem;
        border-radius: 1rem;
        background: #e3f2fd;
        color: #1565c0;
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