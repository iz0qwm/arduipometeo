# arduipometeo
Stazione meteorologica per meteorologia ipogea

## Introduzione
sul perchè sia importante monitorare alcuni parametri meteorologici anche all'interno delle grotte (ambiente _ipogeo_),
vi rimando a questa pagina del Gruppo Grotte di Gallarate: http://www.gruppogrottegallarate.it/meteorologia-ipogea/

Un mio amico, Casadei Federico, grande appassionato di speleogia nonchè membro attivo del CAI, mi ha chiesto se potevo consigliargli qualche sensore per il minimo monitoraggio ambientale di una grotta.
Così è iniziata l'avventura con Arduino per risparmiare il più possibile visto che la stazione sarebbe rimasta abbandonata per un tempo imprecisato, nel sottosuolo.

## Sensori e PCB
Ecco i componenti utilizzati: 
- Arduino UNO
- DHT22 - sensore di temperatura e umidità 
- BMP280 - sensore di pressione (temperatura come parametro secondario)
- Anemometro Inspeed Vortex
- RTC DS3231 - Real Time Clock (temperatura come parametro secondario)
- Micro SD Card module - Modulo per scheda SD
 
