#include <string>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "mysql_driver.h"

extern sql::Connection *con;

enum connection_type { Patchung, Kabel };

class wohnung {
public:
    int haus;
    int apartment;
};

void verbinde_db();
void trenne_db();


unsigned geraete_id_von_ort(wohnung wohn, int pos);
unsigned geraetetyp_id_von_typname(std::string &typname);

void loesche_geraete(std::string &);
void loesche_geraet(wohnung wohn, int pos);

void loesche_verbindungen(std::string &, connection_type typ);
void loesche_verbindungen(std::string &, int pos, connection_type typ);
void loesche_verbindungen(wohnung wohn, int pos, unsigned port,
                          connection_type typ);

bool geraet_existiert(wohnung wohn, int pos);
bool geraetetyp_existiert(std::string &name);

void erzeuge_geraet(wohnung wohn, std::string &typname, int pos,
                    std::string &name);
void erzeuge_verbindung(wohnung w1, int pos1, unsigned port1, wohnung w2,
                        int pos2, unsigned port2, connection_type typ,
                        std::string &material);

void zeige_geraete_in_wohnung(wohnung wohn);
void zeige_kabelstrippe(unsigned geraet, std::string typ, int port = -1);
void zeige_geraet(wohnung wohn, int pos);
void zeige_geraet(std::string &name, int skip = -1);
void zeige_geraetetyp(std::string &name);
void zeige_alle_geraetetypen();
void zeige_alle_geraetetypen(std::string &delim);


void dump_all();
void setup_db();
