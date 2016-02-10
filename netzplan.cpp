#include <iostream>
#include <fstream>

#include "middleside.h"

std::string kupfer("kupfer");
std::string glas("glas");

void parse_arguments(int, char **);
void parse_datei(std::string &);
void function_that_does_sth();

int main(int argc, char **argv) {
    try {
        verbinde_db();

        parse_arguments(argc, argv);

        trenne_db();
    } catch (sql::SQLException &e) {
        std::cerr << "# ERR: " << e.what()
                  << " (MySQL error code: " << e.getErrorCode()
                  << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    } catch (std::string message) {
        std::cerr << "  Fehler: " << message << " Hilfe mit 'netzplan hilfe'" << std::endl;
    } catch (...) {
        std::cerr << "  Something unforseen happened ... Farewell, "
                     "cruel world!" << std::endl;
    }
}


void parse_arguments(int argc, char **argv) {
    if (argc == 1) {
        show_overview();
    }

    if (argc > 1) {
        if (argv[1] == std::string("hilfe") || argv[1] == std::string("help") ||
            argv[1] == std::string("--help") || argv[1] == std::string("-h")) {
            std::ifstream helpfile("/usr/share/netzplan/README");
            while (helpfile.good()) {
                std::string line;
                std::getline(helpfile, line);
                std::cout << line << std::endl;
            }
        } else if (argv[1] == std::string("liste") || argv[1] == std::string("zeige")) {
            if (argc == 2) {
                show_overview();
                // std::cout << "Zu wenig Parameter. Hilfe mit 'netzplan hilfe'"
                // << std::endl;
            }

            if (argc > 2) {
                if (std::string(argv[2]).find("/") != std::string::npos) {
                    std::string haus = argv[2];
                    if (argc == 3) {
                        show_haus(haus);
                    }
                    if (argc > 3) {
                        std::string pos = argv[3];
                        show_geraet_bei_pos(haus, pos);
                    }
                } else { // no '/' in argv[2] found
                    std::string name = argv[2];
                    show_geraet_oder_geraetetyp(name);
                }
            }
        } else if (argv[1] == std::string("neu") ||
                   argv[1] == std::string("glas")) {
            if (argc < 7) {
                throw std::string("Zu wenig Parameter.");
            } else if (std::string(argv[2]).find("/") == std::string::npos) {
                throw std::string("Bitte Haus angeben.");
            } else {
                unsigned zaehl_patch = 0;
                unsigned zaehl_kabel = 0;

                std::string vonhaus = argv[2];
                std::string vonpos = argv[3];
                std::string vonport = argv[4];

                for (int i = 5; i < argc; i++) {
                    if (std::string(argv[i]).find("/") != std::string::npos) {
                        if (argc < i + 3) {
                            throw std::string("Ich bin verwirrt (1).");
                            break;
                        }
                        std::string zuhaus = argv[i];
                        std::string zupos = argv[i + 1];
                        std::string zuport = argv[i + 2];
                        i += 2;
                        zaehl_kabel++;
                        erzeuge_verbindung(vonhaus, vonpos, vonport, zuhaus,
                                           zupos, zuport, Kabel,
                                           argv[1] == glas ? glas : kupfer);
                    } else {
                        if (argc < i + 2) {
                            throw std::string("Ich bin verwirrt (2).");
                            break;
                        }
                        std::string zupos = argv[i];
                        std::string zuport = argv[i + 1];
                        i += 1;
                        zaehl_patch++;
                        erzeuge_verbindung(vonhaus, vonpos, vonport, vonhaus,
                                           zupos, zuport, Patchung,
                                           argv[1] == glas ? glas : kupfer);
                    }
                }
                std::cout << zaehl_kabel + zaehl_patch
                          << " Verbindungen erstellt (" << zaehl_kabel
                          << " Kabel, " << zaehl_patch << " Patch)."
                          << std::endl;
            }
        } else if (argv[1] == std::string("verschiebe")) {
            if (argc < 4) {
                std::cout << "Zu wenig Parameter. Hilfe mit 'netzplan hilfe'"
                          << std::endl;
            }
            std::string geraet = argv[2];
            if (std::string(argv[3]).find("/") != std::string::npos) {
                if (argc == 4) {
                    std::cout
                        << "Bitte Position angeben. Hilfe mit 'netzplan hilfe'"
                        << std::endl;
                }
                if (argc > 4) {
                    std::string haus = argv[3];
                    std::string pos = argv[4];
                    verschiebe(geraet, haus, pos);
                }
            } else {
                std::string pos = argv[3];
                verschiebe(geraet, pos);
            }
        } else if (argv[1] == std::string("lies")) {
            if (argc < 3) {
                std::cout << "Zu wenig Parameter. Hilfe mit 'netzplan hilfe'"
                          << std::endl;
            }
            std::string dateiname = argv[2];
            parse_datei(dateiname);
        } else if (argv[1] == std::string("schreib")) {
            dump_all();
        } else if (argv[1] == std::string("init_db")) {
                setup_db();   /* initialisiere Datenbank */
        }
        else {
            std::cout << "Unbekannter Befehl" << std::endl;
        }
    }
}


void parse_datei(std::string &dateiname) {
    std::ifstream datei(dateiname.c_str());

    try {
        while (true) {
            std::string haus;
            datei >> haus;
            if (!datei.good()) {
                throw 0;
            }
            if (haus.find("/") == std::string::npos) {
                throw 2;
            }

            std::string command;
            datei >> command;
            if (!datei.good()) {
                throw 1;
            }
            if (command.find("/") != std::string::npos || command == "patch") {
                std::string haus2;
                connection_type typ;
                if (command == "patch") {
                    haus2 = haus;
                    typ = Patchung;
                } else {
                    haus2 = command;
                    typ = Kabel;
                }
                std::string material;
                datei >> material;
                if (!datei.good()) {
                    throw 1;
                }
                while (true) {
                    std::string vonpos;
                    datei >> vonpos;
                    if (!datei.good()) {
                        throw 1;
                    }
                    if (vonpos == ";") {
                        break;
                    }

                    std::string vonport;
                    datei >> vonport;
                    if (!datei.good()) {
                        throw 1;
                    }

                    std::string zupos;
                    datei >> zupos;
                    if (!datei.good()) {
                        throw 1;
                    }

                    std::string zuport;
                    datei >> zuport;
                    if (!datei.good()) {
                        throw 1;
                    }

                    erzeuge_verbindung(haus, vonpos, vonport, haus2, zupos,
                                       zuport, typ, material);
                }

            } else if (command == "geraet") {
                while (true) {
                    std::string geraetename;
                    datei >> geraetename;
                    if (!datei.good()) {
                        throw 1;
                    }
                    if (geraetename == ";") {
                        break;
                    }

                    std::string pos;
                    datei >> pos;
                    if (!datei.good()) {
                        throw 1;
                    }

                    std::string name;
                    datei >> name;
                    if (!datei.good()) {
                        throw 1;
                    }

                    erzeuge_geraet(haus, geraetename, pos, name);
                }
            } else {
                throw 3;
            }
        }
    } catch (int i) {
        std::cout << " " << i << " ";
    }
}

/*
void function_that_does_sth() {
    sql::ResultSet *res = 0;
    res = stmt->executeQuery(
        "SELECT id, ort_haus, ort_apartment, ort_position FROM geraet ORDER BY id ASC");

    while (res->next()) {
        // std::cout << ", " << res->getInt("id") << ": " <<
        // res->getString("ort") << " " << res->getInt("position") << std::endl;
    }
    delete res;
    std::cout << std::endl;
}
*/
