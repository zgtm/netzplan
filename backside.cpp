#include "backside.h"

sql::Connection *con = 0;
sql::PreparedStatement *pstmt = 0;
sql::ResultSet *res = 0;


void verbinde_db() {
    try {
        // INIT:
        sql::mysql::MySQL_Driver *driver = 0;
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://localhost", "netzplan", "netzplan");
        con->setSchema("netzplan");

    } catch (sql::SQLException &e) {
        std::cerr << "# ERR: " << e.what()
                  << " (MySQL error code: " << e.getErrorCode()
                  << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }
}

void trenne_db() {
    if (con) {
        delete con;
    }
}

void loesche_geraete(std::string &) { throw std::string("Not Implemented"); }
void loesche_geraet(wohnung wohn, int pos) {
    pstmt = con->prepareStatement("DELETE FROM geraet WHERE ort_haus = ? AND "
                                  "ort_apartment = ? AND ort_position = ?;");
    pstmt->setInt(1, wohn.haus);
    pstmt->setInt(2, wohn.apartment);
    pstmt->setInt(3, pos);
    pstmt->executeUpdate();
    delete pstmt;
    pstmt = 0;
}

void loesche_verbindungen(std::string &, connection_type typ) {
    throw std::string("Not Implemented.");
}
void loesche_verbindungen(std::string &, unsigned, connection_type typ) {
    throw std::string("Not Implemented.");
}
void loesche_verbindungen(wohnung wohn, int pos, unsigned port,
                          connection_type typ) {
    unsigned geraet_id = geraete_id_von_ort(wohn, pos);

    pstmt = con->prepareStatement(
        "DELETE FROM " +
        std::string(typ == Patchung ? "patchung" : "verkabelung") +
        " WHERE (von_geraet = ? AND von_port = ?) OR (zu_geraet = ? AND "
        "zu_port = ?);");
    pstmt->setUInt(1, geraet_id);
    pstmt->setUInt(2, port);
    pstmt->setUInt(3, geraet_id);
    pstmt->setUInt(4, port);
    pstmt->executeUpdate();
    delete pstmt;
    pstmt = 0;
}

bool geraet_existiert(wohnung wohn, int pos) {
    bool result = false;

    pstmt =
        con->prepareStatement("SELECT id FROM geraet WHERE ort_haus = ? AND "
                              "ort_apartment = ? AND ort_position = ?;");
    pstmt->setInt(1, wohn.haus);
    pstmt->setInt(2, wohn.apartment);
    pstmt->setInt(3, pos);
    res = pstmt->executeQuery();
    if (res->next()) {
        result = true;
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;

    return result;
}

bool geraetetyp_existiert(std::string &name) {
    bool result = false;

    pstmt =
        con->prepareStatement("SELECT id FROM geraetetyp WHERE typname = ?;");
    pstmt->setString(1, name);
    res = pstmt->executeQuery();
    if (res->next()) {
        result = true;
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;

    return result;
}

unsigned geraetetyp_id_von_typname(std::string &typname) {
    unsigned typ = 0;
    pstmt =
        con->prepareStatement("SELECT id FROM geraetetyp WHERE typname = ?;");
    pstmt->setString(1, typname);
    res = pstmt->executeQuery();
    if (res->next()) {
        typ = res->getUInt("id");
    } else {
        throw std::string("Geraete-Typ nicht gefunden (Fehler 110)");
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;
    return typ;
}

void erzeuge_geraet(wohnung wohn, std::string &typname, int pos,
                    std::string &name) {
    unsigned typid = geraetetyp_id_von_typname(typname);

    pstmt = con->prepareStatement("INSERT INTO geraet (typ, ort_haus, "
                                  "ort_apartment, ort_position, name) VALUES "
                                  "(?, ?, ?, ?, ?);");
    pstmt->setUInt(1, typid);
    pstmt->setInt(2, wohn.haus);
    pstmt->setInt(3, wohn.apartment);
    pstmt->setInt(4, pos);
    pstmt->setString(5, name);
    pstmt->executeUpdate();
    delete pstmt;
    pstmt = 0;
    std::cout << "*";
}

unsigned geraete_id_von_ort(wohnung wohn, int pos) {
    unsigned geraet_id = 0;
    pstmt =
        con->prepareStatement("SELECT id FROM geraet WHERE ort_haus = ? AND "
                              "ort_apartment = ? AND ort_position = ?;");
    pstmt->setInt(1, wohn.haus);
    pstmt->setInt(2, wohn.apartment);
    pstmt->setInt(3, pos);
    res = pstmt->executeQuery();
    if (res->next()) {
        geraet_id = res->getUInt("id");
    } else {
        throw std::string("Geraet nicht gefunden (Fehler 145)");
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;

    return geraet_id;
}

void erzeuge_verbindung(wohnung wohn1, int pos1, unsigned port1,
                        wohnung wohn2, int pos2, unsigned port2,
                        connection_type typ, std::string &medium) {
    unsigned geraet_id1 = geraete_id_von_ort(wohn1, pos1);
    unsigned geraet_id2 = geraete_id_von_ort(wohn2, pos2);

    pstmt = con->prepareStatement(
        "INSERT INTO " +
        std::string(typ == Patchung ? "patchung" : "verkabelung") +
        "(von_geraet, von_port, zu_geraet, zu_port, medium) VALUES "
        "(?,?,?,?,?);");
    pstmt->setUInt(1, geraet_id1);
    pstmt->setUInt(2, port1);
    pstmt->setUInt(3, geraet_id2);
    pstmt->setUInt(4, port2);
    pstmt->setString(5, medium);
    pstmt->executeUpdate();
    pstmt->setUInt(1, geraet_id2);
    pstmt->setUInt(2, port2);
    pstmt->setUInt(3, geraet_id1);
    pstmt->setUInt(4, port1);
    pstmt->setString(5, medium);
    pstmt->executeUpdate();
    delete pstmt;
    pstmt = 0;
    std::cout << "-";
}

void zeige_kabelstrippe(unsigned geraet, std::string typ, int port) {
    sql::PreparedStatement *pstmt2 = con->prepareStatement(
        "SELECT von_port, zu_port, " + typ +
        ".medium, geraet.name, zu_geraet "
        "FROM " + typ + " " +
        "JOIN geraet ON geraet.id = zu_geraet "
        "WHERE von_geraet = ?" +
        (port < 0 ? ";" : " AND von_port = ?;"));
    pstmt2->setUInt(1, geraet);
    if (port >= 0) {
        pstmt2->setUInt(2, port);
    }

    sql::ResultSet *res2 = pstmt2->executeQuery();
    while (res2->next()) {
        std::cout << " p" << res2->getUInt("von_port")
                  << (res2->getString("medium") == "kupfer" ? " --- "
                                                              : " ... ") << "p"
                  << res2->getUInt("zu_port") << " " << res2->getString("name");
        if (port < 0) {
            std::cout << std::endl;
        }


        std::string typ2 = (typ == "verkabelung") ? "patchung" : "verkabelung";
        zeige_kabelstrippe(res2->getUInt("zu_geraet"), typ2,
                           res2->getUInt("zu_port"));
    }
    delete res2;
    delete pstmt2;
}

void zeige_alle_geraetetypen() {
    std::string delim("\n");
    zeige_alle_geraetetypen(delim);
    std::cout << std::endl;
}

void zeige_alle_geraetetypen(std::string &delim) {

    pstmt = con->prepareStatement("SELECT typname, n_ports FROM geraetetyp;");
    res = pstmt->executeQuery();
    bool first = true;
    while (res->next()) {
        std::cout << (first ? "" : delim) << res->getString("typname");
        first = false;
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;
}

void zeige_geraet(wohnung wohn, int pos) {
    pstmt = con->prepareStatement(
        "SELECT name, ort_haus, ort_apartment, ort_position, typname, "
        "geraet.kommentar, typ AS kommentar "
        "FROM geraet "
        "JOIN geraetetyp ON geraet.typ = geraetetyp.id "
        "WHERE ort_haus = ? AND ort_apartment = ? AND ort_position = ?;");
    pstmt->setInt(1, wohn.haus);
    pstmt->setInt(2, wohn.apartment);
    pstmt->setInt(3, pos);
    res = pstmt->executeQuery();
    if (res->next()) {
        std::cout << "Geraet " << res->getString("name") << ": "
                  << res->getString("typname") << " in "
                  << res->getInt("ort_haus") << "/"
                  << res->getInt("ort_apartment") << " (pos "
                  << res->getInt("ort_position") << ")"
                  << ", Kommentar: " << res->getString("kommentar")
                  << std::endl;


        std::string typ = "patchung";
        zeige_kabelstrippe(res->getUInt("typ"), typ);
        std::cout << std::endl;

        typ = "verkabelung";
        zeige_kabelstrippe(res->getUInt("typ"), typ);
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;
}


void zeige_geraet(std::string &name, int skip) {
    pstmt = con->prepareStatement(
        "SELECT name, typname, ort_haus, ort_apartment, ort_position, "
        "geraet.kommentar AS kommentar "
        "FROM geraet "
        "JOIN geraetetyp ON geraet.typ = geraetetyp.id "
        "WHERE INSTR(geraet.name, ?)>0 AND (typ > ?)"
        "ORDER BY typ DESC, ort_haus, ort_apartment, ort_position;");
    pstmt->setString(1, name);
    pstmt->setInt(2, skip);
    res = pstmt->executeQuery();
    while (res->next()) {
        std::cout << "Geraet " << res->getString("name") << " ("
                  << res->getString("typname") << ") in "
                  << res->getInt("ort_haus") << "/"
                  << res->getInt("ort_apartment") << " (pos "
                  << res->getInt("ort_position") << ")"
                  << "  \"" << res->getString("kommentar") << "\""
                  << std::endl;
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;
}

void zeige_geraete_in_wohnung(wohnung wohn) {
        pstmt = con->prepareStatement(
        "SELECT name, typname, ort_haus, ort_apartment, ort_position, "
        "geraet.kommentar AS kommentar "
        "FROM geraet "
        "JOIN geraetetyp ON geraet.typ = geraetetyp.id "
        "WHERE ort_haus = ? AND ort_apartment = ? "
        "ORDER BY ort_position ;");
    pstmt->setInt(1, wohn.haus);
    pstmt->setInt(2, wohn.apartment);
    res = pstmt->executeQuery();
    std::cout << "In " << wohn.haus << "/" << wohn.apartment << ":\n";
    while (res->next()) {
        std::cout << "Geraet " << res->getString("name") << " ("
                  << res->getString("typname") << ") an Pos "
                  << res->getInt("ort_position")
                  << "  \"" << res->getString("kommentar") << "\""
                  << std::endl;
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;
}

void zeige_geraetetyp(std::string &name) {
    pstmt = con->prepareStatement(
        "SELECT * FROM geraetetyp WHERE INSTR(typname, ?)>0;");
    pstmt->setString(1, name);
    res = pstmt->executeQuery();
    while (res->next()) {
        std::cout << "Typ " << res->getString("typname") << ": "
                  << res->getUInt("n_ports") << "-Port "
                  << res->getString("hersteller") << " ("
                  << res->getString("modell")
                  << ")  \"" << res->getString("kommentar") << "\""
                  << std::endl;

        sql::PreparedStatement *pstmt2 =
            con->prepareStatement("SELECT * FROM geraet WHERE typ = ?");
        pstmt2->setUInt(1, res->getUInt("id"));
        sql::ResultSet *res2 = pstmt2->executeQuery();
        while (res2->next()) {
            std::cout << "   Geraet " << res2->getString("name") << " in "
                      << res2->getInt("ort_haus") << "/"
                      << res2->getInt("ort_apartment") << " (pos "
                      << res2->getInt("ort_position") << ")" << std::endl;
        }
        delete res2;
        delete pstmt2;
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;
}

void dump_all() {
    pstmt = con->prepareStatement(
        "SELECT ort_haus, ort_apartment, ort_position, name, typname "
        "FROM geraet "
        "JOIN geraetetyp ON geraet.typ = geraetetyp.id "
        "ORDER BY ort_haus, ort_apartment, ort_position;");
    res = pstmt->executeQuery();
    while (res->next()) {
        std::string typ = res->getString("typname");
        std::cout << res->getInt("ort_haus") << "/"
                  << res->getInt("ort_apartment") << "\tgeraet\t" << typ
                  << (typ.length() > 7 ? "\t" : "\t\t")
                  << res->getInt("ort_position") << "\t"
                  << res->getString("name") << " ;" << std::endl;
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;

    std::cout << std::endl;

    pstmt = con->prepareStatement(
        "SELECT g.ort_haus, g.ort_apartment, g.ort_position, g2.ort_haus AS "
        "ort_haus2, g2.ort_apartment AS ort_apartment2, g2.ort_position AS "
        "ort_position2, von_port, zu_port, medium "
        "FROM verkabelung "
        "JOIN geraet AS g ON verkabelung.von_geraet = g.id "
        "JOIN geraet AS g2 ON verkabelung.zu_geraet = g2.id "
        "WHERE g.id < g2.id "
        "OR (g.id = g2.id AND von_port < zu_port);");
    res = pstmt->executeQuery();
    while (res->next()) {
        std::cout << res->getInt("ort_haus") << "/"
                  << res->getInt("ort_apartment") << "\t"
                  << res->getInt("ort_haus2") << "/"
                  << res->getInt("ort_apartment2") << "\t"
                  << res->getString("medium") << "\t"
                  << res->getInt("ort_position") << "\t"
                  << res->getUInt("von_port") << "\t"
                  << res->getInt("ort_position2") << "\t"
                  << res->getUInt("zu_port") << " ;" << std::endl;
    }
    delete res;
    res = 0;
    delete pstmt;
    pstmt = 0;

    std::cout << std::endl;

    for (int i = 0; i < 2; i++) {
        std::string medium = i ? "glas" : "kupfer";
        pstmt = con->prepareStatement(
            "SELECT DISTINCT ort_haus, ort_apartment FROM geraet;");
        res = pstmt->executeQuery();
        while (res->next()) {
            wohnung wohn = {res->getInt("ort_haus"),
                            res->getInt("ort_apartment")};

            //            std::cerr << wohn.haus << "/" << wohn.apartment <<
            //            std::endl;


            sql::PreparedStatement *pstmt2 = con->prepareStatement(
                "SELECT g.ort_haus, g.ort_apartment, g.ort_position, "
                "g2.ort_position AS ort_position2, von_port, zu_port "
                "FROM patchung "
                "JOIN geraet AS g ON patchung.von_geraet = g.id "
                "JOIN geraet AS g2 ON patchung.zu_geraet = g2.id "
                "WHERE g.ort_haus = ? AND g.ort_apartment = ? "
                "AND (g.id < g2.id OR (g.id = g2.id AND von_port < zu_port)) "
                "AND patchung.medium = ?;");
            pstmt2->setInt(1, wohn.haus);
            pstmt2->setInt(2, wohn.apartment);
            pstmt2->setString(3, medium);
            sql::ResultSet *res2 = pstmt2->executeQuery();
            bool first = true;
            while (res2->next()) {
                if (first) {
                    std::cout << wohn.haus << "/" << wohn.apartment
                              << "\tpatch " << medium;
                    first = false;
                }

                std::cout << std::endl << res2->getInt("ort_position") << "\t"
                          << res2->getUInt("von_port") << "\t"
                          << res2->getInt("ort_position2") << "\t"
                          << res2->getUInt("zu_port");
            }
            if (!first) {
                std::cout << " ;\n";
            }
            delete res2;
            delete pstmt2;
        }
        delete res;
        res = 0;
        delete pstmt;
        pstmt = 0;
    }
}


void setup_db() {
    /* WARNING:
       Before you call this funktion, make sure to have a backup of the database
       */
    using namespace std;
    string table_geraetetyp_string = "CREATE TABLE geraetetyp ("
                                     "id INT UNSIGNED NOT NULL AUTO_INCREMENT, "
                                     "typname VARCHAR(250), "
                                     "n_ports INT UNSIGNED, "
                                     "hersteller VARCHAR(250), "
                                     "modell VARCHAR(250), "
                                     "kommentar TEXT, "
                                     "PRIMARY KEY (id));";

    string table_geraete_string = "CREATE TABLE geraet ("
                                  "id INT UNSIGNED NOT NULL AUTO_INCREMENT, "
                                  "typ INT UNSIGNED, "
                                  "ort_haus INT, "
                                  "ort_apartment INT, "
                                  "ort_position INT, "
                                  "name VARCHAR(250), "
                                  "mac VARCHAR(250), "
                                  "seriennummer VARCHAR(250), "
                                  "kommentar TEXT, "
                                  "PRIMARY KEY (id));";

    string table_patchung_string = "CREATE TABLE patchung ("
                                   "von_geraet INT UNSIGNED, "
                                   "von_port INT UNSIGNED, "
                                   "zu_geraet INT UNSIGNED, "
                                   "zu_port INT UNSIGNED, "
                                   "medium VARCHAR(250), "
                                   "kommentar VARCHAR(250));";

    string table_verkabelung_string = "CREATE TABLE verkabelung ("
                                      "von_geraet INT UNSIGNED, "
                                      "von_port INT UNSIGNED, "
                                      "zu_geraet INT UNSIGNED, "
                                      "zu_port INT UNSIGNED, "
                                      "medium VARCHAR(255), "
                                      "kommentar VARCHAR(255));";

    string create_list[] = {table_geraetetyp_string, table_geraete_string,
                            table_patchung_string, table_verkabelung_string};

    string delete_list[] = {"patchung", "verkabelung", "geraet", "geraetetyp"};

    class geraetetyp_record {
    public:
        string name;
        uint n_ports;
        string hersteller;
        string modell;
    };

    geraetetyp_record geraetetypen[] = {
        {"dose", 2, "", ""},
        {"mhunap", 5, "", ""},
        {"pp24", 24, "", ""},
        {"pp32", 32, "", ""},
        {"pp48", 48, "", ""},
        {"hp2530", 10, "hp", "2530-8G (J9777A)"},
        {"hp2620-24", 28, "hp", "2620-24 (J9623A)"},
        {"hp2620-48", 52, "hp", "2620-48 (J9626A)"},
        {"hp2626", 28, "hp", "2626 (J4900A)"},
        {"3com4200", 26, "3com", "4228G (3C17304)"},
        {"3com4500", 50, "3com", "4500 (3CR17562-91)"},
        {"3com4800", 24, "3com", "4800G (3CRS48G-24S-91)"}};


    string geraete_insert =
        "INSERT INTO geraetetyp (typname, n_ports, hersteller, "
        "modell) VALUES (?, ?, ?, ?)";

    for (string table : delete_list) {
        pstmt = con->prepareStatement("DROP TABLE IF EXISTS " + table + ";");
        pstmt->execute();
        delete pstmt;
    }

    for (string statement : create_list) {
        pstmt = con->prepareStatement(statement);
        pstmt->execute();
        delete pstmt;
    }

    pstmt = con->prepareStatement(geraete_insert);
    for (geraetetyp_record geraetetyp : geraetetypen) {
        pstmt->setString(1, geraetetyp.name);
        pstmt->setUInt(2, geraetetyp.n_ports);
        pstmt->setString(3, geraetetyp.hersteller);
        pstmt->setString(4, geraetetyp.modell);
        pstmt->execute();
    }
    delete pstmt;
}
