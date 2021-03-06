# KW Netzplan
Netzplanverwaltung für das Wohnheim Kellnerweg

## Kompilieren
Einfach 'make' ausführen:

    make

## Benutzung

    netzplan [kommando [parameter]]

Kommandos: liste, neu, glas, verschiebe, lies, schreib

Syntax:

    netzplan
    netzplan liste [<wohnung> [<pos>] | <geraetename> | <geraetetyp>]
    netzplan [neu|glas] <wohnung> <pos> <port> [<wohnung>] <pos> <port> 
        [[<wohnung>] <pos> <port> [[<wohnung>] <pos> <port> … ]]
    netzplan verschiebe <geraetename> <pos>
    netzplan lies <datei>
    netzplan schreib

Wobei

    <wohnung> := <haus>/<apartment>

Beschreibung:

    netzplan
    netzplan liste
      Allgmeine Übersicht anzeigen
    netzplan liste <wohnung>
      Geräte in Wohnung anzeigen
    netzplan liste <wohnung> <pos>
      Verbindungen vom Gerät anzeigen
    netzplan liste <geraetename>
      Verbindungen vom Gerät anzeigen
    netzplan liste <geraetetyp>
      Alle Gerät vom Gerätetyp anzeigen
    netzplan neu <wohnung> <pos> <port> <pos> <port>
      Neue Patchverbindung in Wohnung
    netzplan neu <wohnung> <pos> <port> <wohnung> <pos> <port>
      Neue Kabelverbindung von Wohnung zu Wohnung
      Für Verbindungen zur Dose im gleichen Wohnung, zweimal die 
        gleiche Wohnung angeben
    netzplan neu <wohnung> <pos> <port> [<wohnung>] <pos> <port> …
      Mehrere zusammenhängende Kabel- und Patchverbindungen
    netzplan glas …
      Wie 'netzplan neu', setzt Typ auf Glasfaser
    netzplan verschiebe <geraetename> [<wohnung>] <pos>
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


## API

    enum Medium {
        kupfer,
        glas
    };
    
    enum VTyp {
        verkabelung,
        patchung
    };
    
    class Geraet {
    public:
        int house();
        int apt();
        int pos();
        str name();
        str mac();
        str serial();
        str comment();
        Geraetetyp* type();
    
        vec<Verbindung> patches();
        vec<Verbindung> cables();
    
        void delete(); /* Not implemented yet */
        void removeAllPatches(); /* Not implemented yet */
        void removeAllCables(); /* Not implemented yet */
    };
    
    class Geraetetyp {
    public:
        str name();
        int n_ports();
        str brand();
        str model();
        str comment();
    };
    
    class Verbindung {
    public:
        VTyp type();
        Medium medium();
        
        std::shared_ptr<Geraet> loc_dev();
        unsigned loc_port();
        std::shared_ptr<Geraet> rem_dev();
        unsigned rem_port();
    
        void delete(); /* Not implemented yet */
    };
    
    class NetzplanDatenbank {
    public:
        NetzplanDatenbank(str db_user, str db_pw, str db_dbname);
    
        vec<Geraet> wohnung(int house, int apt);
        shared_ptr<Geraet> geraet(int house, int apt, int pos);
    
        void newDevice(int house, int apt, int pos, Geraetetyp *typ);  /* Not implemented yet */
        void newDeviceType(str name, int n_ports, str brand, str model, str comment = "");  /* Not implemented yet */
        void newCable(Geraet* from, Geraet* to, unsigned from_port, unsigned to_port);  /* Not implemented yet */
        void newPatch(Geraet* from, Geraet* to, unsigned from_port, unsigned to_port);  /* Not implemented yet */
    };
