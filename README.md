# netzplan
Netzplanverwaltung für das Wohnheim Kellnerweg

## Kompilieren
Einfach 'make' ausführen:
    make

## Benutzung

netzplan [kommando [parameter]]

Kommandos: liste, neu, glas, verschiebe, lies, schreib

Syntax:
  netzplan
  netzplan liste [<haus> [<pos>] | <geraetename> | <geraetetyp>]
  netzplan [neu|glas] <haus> <pos> <port> [<haus>] <pos> <port> 
        [[<haus>] <pos> <port> [[<haus>] <pos> <port> … ]]
  netzplan verschiebe <geraetename> <pos>
  netzplan lies <datei>
  netzplan schreib

Beschreibung:
  netzplan
  netzplan liste
      Allgmeine Übersicht anzeigen
  netzplan liste <haus>
      Geräte im Haus anzeigen
  netzplan liste <haus> <pos>
      Verbindungen vom Gerät anzeigen
  netzplan liste <geraetename>
      Verbindungen vom Gerät anzeigen
  netzplan liste <geraetetyp>
      Alle Gerät vom Gerätetyp anzeigen
  netzplan neu <haus> <pos> <port> <pos> <port>
      Neue Patchverbindung in Haus
  netzplan neu <haus> <pos> <port> <haus> <pos> <port>
      Neue Kabelverbindung von Haus zu Haus
      Für Verbindungen zur Dose im gleichen Haus, zweimal das 
        gleiche Haus angeben
  netzplan neu <haus> <pos> <port> [<haus>] <pos> <port> …
      Mehrere zusammenhängende Kabel- und Patchverbindungen
  netzplan glas …
      Wie 'netzplan neu', setzt Typ auf Glasfaser
  netzplan verschiebe <geraetename> [<haus>] <pos>
      Verschiebt das Gerät an die Position
  netzplan lies <datei>
      Liest Netzkonfiguration aus Datei
  netzplan schreib
      Gibt gesamte Netzkonfiguration auf dem Terminal aus

Hinweise zur Position:
  -1: MhunAP
   0: Dose
   1: 1. Gerät von oben
   2: 2. Gerät von oben
      ...
