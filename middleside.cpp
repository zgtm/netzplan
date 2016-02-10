#include "middleside.h"

#include "strnumconv.h"

#include <utility>

std::pair<int, int> split_wohnung(std::string &wohn)
{
    unsigned int pos = wohn.find("/"); //== std::string::npos)
    if (pos == std::string::npos)
        throw new std::string("Ungueltiger Wohnungsstring. ");
    std::string haus = wohn.substr(0, pos);
    std::string app  = wohn.substr(pos + 1, std::string::npos);
    return {StringToNumber<int>(haus), StringToNumber<int>(app)};
}

wohnung str_wohnung(std::string &wohn)
{
    std::pair<int, int> ha = split_wohnung(wohn);
    return {ha.first, ha.second};
}

void show_overview() {
    geraeteliste();
    std::cout << std::endl;
    std::string n = "";
    zeige_geraet(n, 5);
}

void show_haus(int hausnummer) {}

void show_haus(std::string &haus) {
    zeige_geraete_in_wohnung(str_wohnung(haus));
}

void show_wohnung(int haus, int app);


void show_geraet_bei_pos(std::string &wohn_str, std::string &pos_str) {
    wohnung wohn = str_wohnung(wohn_str);
    unsigned pos = StringToNumber<unsigned>(pos_str);
    zeige_geraet(wohn, pos);
}

void show_geraet_oder_geraetetyp(std::string &name) {
    zeige_geraet(name);
    std::cout << std::endl;
    zeige_geraetetyp(name);
}

void geraeteliste() {
    std::string delim(", ");
    std::cout << "[";
    zeige_alle_geraetetypen(delim);
    std::cout << "]" << std::endl;
}

void erzeuge_verbindung(std::string &wohn1_str, std::string &pos1_str,
                        std::string &port1_str, std::string &wohn2_str,
                        std::string &pos2_str, std::string &port2_str,
                        connection_type typ, std::string &material) {
    wohnung wohn1 = str_wohnung(wohn1_str);
    wohnung wohn2 = str_wohnung(wohn2_str);
    unsigned pos1 = StringToNumber<unsigned>(pos1_str);
    unsigned pos2 = StringToNumber<unsigned>(pos2_str);
    unsigned port1 = StringToNumber<unsigned>(port1_str);
    unsigned port2 = StringToNumber<unsigned>(port2_str);
    
    loesche_verbindungen(wohn2, pos1, port1, typ);
    loesche_verbindungen(wohn2, pos2, port2, typ);

    if (geraet_existiert(wohn1, pos1) && geraet_existiert(wohn2, pos2)) {
        erzeuge_verbindung(wohn1, pos1, port1, wohn2, pos2, port2, typ,
                           material);
    }
}

void erzeuge_geraet(std::string &wohn_str, std::string &typ, std::string &pos_str,
                    std::string &name) {
    unsigned pos = StringToNumber<unsigned>(pos_str);
    wohnung wohn = str_wohnung(wohn_str); 
    
    loesche_geraet(wohn, pos);
    if (geraetetyp_existiert(typ)) {
        erzeuge_geraet(wohn, typ, pos, name);
    }
}

void verschiebe(std::string &geraet, std::string &pos) {}

void verschiebe(std::string &geraet, std::string &haus, std::string &pos) {}
