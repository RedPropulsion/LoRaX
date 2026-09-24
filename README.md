# LoRaX - risponditore LoRa per NUCLEO-WL55JC1

Firmware Zephyr per la seconda radio della demo ObelICS / Bright Night.
Riceve quattro byte ASCII `PING`, attende 200 ms, trasmette quattro byte `PONG`
e torna in ricezione.

La comunicazione bidirezionale con lo shield SX1261 di ObelICS è stata verificata
sulle schede. I comandi LED tra Idefix e ObelICS viaggiano separatamente su
Ethernet/MAVLink, nel progetto Panoramix.

## Sorgenti

```text
wl55-responder/
├── CMakeLists.txt
├── prj.conf
├── nucleo_wl55jc.overlay
└── src/main.c
```

- `src/main.c`: ricezione PING, risposta PONG, log RSSI/SNR e contatori.
- `prj.conf`: configurazione Zephyr con backend radio LoRaMac-node.
- `nucleo_wl55jc.overlay`: abilita lo switch RF tramite FE_CTRL3/PC3.
  Il devicetree della board Zephyr gestisce FE_CTRL1/2, TCXO e percorso RF HP.
- `CMakeLists.txt`: compilazione dell'applicazione Zephyr.

## Parametri radio

| Parametro | Valore |
|---|---|
| Frequenza | 868 MHz |
| Larghezza di banda | 250 kHz |
| Spreading factor | SF8 |
| Coding rate | 4/5 |
| Preambolo | 12 simboli |
| Potenza TX richiesta | 14 dBm |
| Header | Esplicito, lunghezza variabile |
| CRC payload | Abilitato |
| Rete | Privata |
| IQ | Normale |
| Payload | PING / PONG, 4 byte senza terminatore NUL |

Il LED verde cambia stato dopo ogni PONG trasmesso. I log RSSI/SNR della WL55
misurano il PING ricevuto; quelli di ObelICS misurano il PONG ricevuto.
I pacchetti diversi da quattro byte `PING` vengono ignorati.

## Compilare

Sostituire i percorsi con quelli del proprio ambiente di lavoro e compilare con:

```bash
# Attiva gli strumenti Python già installati
source /percorso/workspace-panoramix/.venv/bin/activate
# Indica dove si trova Zephyr
export ZEPHYR_BASE=/percorso/workspace-panoramix/zephyr
# Entra nella repository LoRaX (se non ci sei già)
cd /percorso/LoRaX
# Nel caso in cui la compilazione dia errore perchè non riesce a determinare la board dal CMakeList.txt
west config build.board nucleo_wl55jc
# Compila l'applicazione per la WL55
west build wl55-responder
```

## Programmare la WL55

Dopo una compilazione riuscita, restando nella radice di LoRaX e nello stesso ambiente attivo, collegare la scheda NUCLEO-WL55JC1 al PC e programmare il firmware con:

```bash
west flash
```

Durante questo flash collegare al PC solo lo ST-LINK della WL55, per evitare
che venga selezionata la H723. Dopo il flash possono essere ricollegate entrambe.

## Verifica

Montare le antenne adatte alla banda di 868 MHz e alimentare entrambe le schede.
Aprire la console seriale ST-LINK della WL55 a **115200 baud, 8N1**.

```text
WL55 responder: 868 MHz, BW250, SF8, CR4/5, private, CRC on
Waiting for four bytes PING; response PONG after 200 ms
PING #1: RSSI <misura> dBm, SNR <misura> dB
PONG #1 sent
```

Sulla console ObelICS deve comparire `LoRa PING/PONG OK`.
In assenza di PING la WL55 resta in ascolto e non trasmette periodicamente.
Il test non contiene numeri di sequenza e presuppone una coppia di nodi in prova.
