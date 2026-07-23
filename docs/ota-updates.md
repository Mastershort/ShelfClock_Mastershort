# OTA-Updates über GitHub Releases

Die Uhr kann selbstständig auf GitHub nach neuen Versionen suchen und sie
per Klick installieren - ohne dass du firmware.bin/littlefs.bin manuell
hochladen musst.

## Wie es funktioniert

- Prüfung: täglich automatisch (60s nach Boot, danach alle 24h) und per
  Knopf auf der Settings-Seite ("Firmware Update").
- Quelle: `https://api.github.com/repos/Mastershort/ShelfClock_Mastershort/releases/latest`
- Verglichen wird der Release-Tag (z.B. `v2.1.0`) mit der einkompilierten
  `softwareVersion` in [src/ShelfClock.cpp](../src/ShelfClock.cpp).
- Installation lädt die zum Release hochgeladenen Assets `firmware.bin`
  bzw. `littlefs.bin` direkt herunter und flasht sie - technisch identisch
  zum manuellen Weg über `/update`/`/updatefs`, nur automatisiert.
- Sichtbar auch in Home Assistant als Sensor "Firmware Status" ("Aktuell"
  oder "Update verfügbar: X.Y.Z").

## Release veröffentlichen (damit die Uhr etwas findet)

Nach jedem Batch an Änderungen, den du auf die Uhren "ausrollen" willst:

```bash
# 1. Version in src/ShelfClock.cpp anheben (softwareVersion = "version-X.Y.Z")
# 2. Beide Images bauen
pio run
pio run --target buildfs

# 3. Release veröffentlichen (Tag MUSS mit "v" beginnen)
gh release create vX.Y.Z \
  .pio/build/espwroom32/firmware.bin \
  .pio/build/espwroom32/littlefs.bin \
  --title "vX.Y.Z" \
  --notes "Was sich geändert hat..."
```

Danach zeigt jede Uhr im Netz das Update spätestens beim nächsten
täglichen Check an (oder sofort per "Nach Updates suchen"-Button).

## Bekannte Grenzen

- **HTTPS ohne Zertifikatsprüfung**: Die Uhr nutzt `WiFiClientSecure` mit
  `setInsecure()` - die Verbindung ist verschlüsselt, aber die Uhr prüft
  nicht, ob sie wirklich mit dem echten GitHub spricht. Ein Angreifer mit
  Kontrolle über dein lokales Netz (z.B. DNS-Spoofing) könnte theoretisch
  gefälschte Firmware unterschieben. Für ein Heimnetz-Gerät ist das
  ein akzeptables Risiko - Root-CA-Pinning würde das schließen, bräuchte
  aber laufende Pflege (Zertifikate laufen ab/wechseln).
- **Kein Rollback für das Dateisystem**: Die Firmware-Partition hat ein
  A/B-Schema (bei Fehlschlag bleibt die alte Version bootfähig), die
  LittleFS-Partition nicht. Schlägt ein Dateisystem-Update mitten im
  Schreiben fehl (WLAN-Aussetzer o.ä.), bleibt nur der manuelle Reflash
  per USB.
- **Flash-Verbrauch**: HTTPS/TLS-Unterstützung kostet ca. 10 Prozentpunkte
  Flash-Speicher (Stand v2.1.0: ~90% belegt). Für zukünftige Features ist
  entsprechend weniger Puffer vorhanden.
