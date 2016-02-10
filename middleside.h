#include <string>

#include "backside.h"

void show_overview();
void show_haus(std::string &);
void show_geraet_bei_pos(std::string &, std::string &);
void show_geraet_oder_geraetetyp(std::string &);
void geraeteliste();
void erzeuge_geraet(std::string &, std::string &, std::string &, std::string &);
void erzeuge_verbindung(std::string &, std::string &, std::string &,
                        std::string &, std::string &, std::string &,
                        connection_type, std::string &);
void verschiebe(std::string &, std::string &);
void verschiebe(std::string &, std::string &, std::string &);
