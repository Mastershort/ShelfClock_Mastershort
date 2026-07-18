# ShelfClock Notify - Nachrichten auf der Uhr anzeigen

Die Uhr kann Textnachrichten als Laufschrift anzeigen, ausgeloest per MQTT
oder HTTP. Der aktuelle Anzeigemodus wird kurz unterbrochen und danach
automatisch fortgesetzt.

## Payload

JSON (empfohlen) oder einfacher Text:

```json
{"text": "Waschmaschine fertig", "color": "00FF00", "repeat": 2}
```

| Feld     | Pflicht | Beschreibung                                    |
|----------|---------|-------------------------------------------------|
| `text`   | ja      | Nachricht (A-Z, a-z, 0-9 und . , - : ' % ^)     |
| `color`  | nein    | Hex-Farbe `RRGGBB` oder `#RRGGBB`, Default weiss |
| `repeat` | nein    | 1-5 Wiederholungen, Default 1                    |

Wird nur Text (kein JSON) geschickt, scrollt er einmal in Weiss.

## MQTT

Topic: **`shelfclock/notify`**

Test mit mosquitto:

```bash
mosquitto_pub -h <broker> -u <user> -P <pass> -t shelfclock/notify \
  -m '{"text": "Hallo Regal", "color": "FF8800"}'
```

## Home-Assistant-Automationen

### Waschmaschine fertig

```yaml
automation:
  - alias: "ShelfClock: Waschmaschine fertig"
    trigger:
      - platform: state
        entity_id: sensor.waschmaschine_status
        to: "fertig"
    action:
      - service: mqtt.publish
        data:
          topic: shelfclock/notify
          payload: '{"text": "WASCHMASCHINE FERTIG", "color": "00FF00", "repeat": 2}'
```

### Tuerklingel

```yaml
automation:
  - alias: "ShelfClock: Tuerklingel"
    trigger:
      - platform: state
        entity_id: binary_sensor.tuerklingel
        to: "on"
    action:
      - service: mqtt.publish
        data:
          topic: shelfclock/notify
          payload: '{"text": "DING DONG", "color": "FF0000", "repeat": 3}'
```

### Als Skript mit Variablen (wiederverwendbar)

```yaml
script:
  shelfclock_notify:
    alias: "ShelfClock Nachricht"
    fields:
      message:
        description: "Text der Nachricht"
      color:
        description: "Hex-Farbe RRGGBB"
        default: "FFFFFF"
    sequence:
      - service: mqtt.publish
        data:
          topic: shelfclock/notify
          payload: >-
            {"text": "{{ message }}", "color": "{{ color | default('FFFFFF') }}"}
```

Aufruf aus jeder Automation:

```yaml
- service: script.shelfclock_notify
  data:
    message: "Essen ist fertig"
    color: "FFAA00"
```

## HTTP (zum Testen ohne MQTT)

```bash
curl -X POST http://shelfclock/notify \
  -H "Content-Type: application/json" \
  -d '{"text": "Test 123", "color": "00AAFF"}'
```

## Hinweise

- Der Zeichensatz der 7-Segment-Anzeige ist begrenzt; Umlaute gibt es nicht
  (AE/OE/UE ausschreiben). Unbekannte Zeichen werden als Leerzeichen gezeigt.
- Waehrend eine Nachricht scrollt, werden weitere Notifies verworfen bzw.
  ueberschreiben die Warteschlange (es gibt genau einen Warteplatz).
- Payload-Limit: ca. 400 Zeichen (MQTT-Puffer 512 Bytes).
