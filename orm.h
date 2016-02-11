#include <string>
#include <map>
#include <vector>
#include <memory>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "mysql_driver.h"

typedef std::string str;
template<typename T>
    using vec = std::vector<std::shared_ptr<T>>;


enum Medium {
    kupfer,
    glas
};

enum VTyp {
    verkabelung,
    patchung
};

str medtostr(Medium m) { return str(m == Medium::glas ? "glas" : "kupfer"); }
Medium strtomed(str s) { return (s == "glas" ? Medium::glas : Medium::kupfer); }

str typtostr(VTyp m) { return str(m == VTyp::patchung ? "patchung" : "verkabelung"); }
VTyp strtotyp(str s) { return (s == "patchung" ? VTyp::patchung : VTyp::verkabelung); }

class NetzplanDatenbank;
class Geraetetyp {
private:
    friend class NetzplanDatenbank; 
    Geraetetyp(unsigned id, str name, unsigned nports, str brand, str model, str comment, NetzplanDatenbank *db) : id(id), _name(name), _n_ports(nports), _brand(brand), _model(model), _comment(comment), db(db) {};
    
    unsigned id;
    str _name;
    unsigned _n_ports;
    str _brand;
    str _model;
    str _comment;
    NetzplanDatenbank *db;

public:
    str name() {return _name;}
    int n_ports() {return _n_ports;}
    str brand() {return _brand;}
    str model() {return _model;}
    str comment() {return _comment;}
};

class Geraet;
class Verbindung {
private:
    friend class NetzplanDatenbank; 
    Verbindung(VTyp type, Medium medium, unsigned dev1, unsigned dev2, unsigned port1, unsigned port2, std::shared_ptr<Geraet> local, NetzplanDatenbank* db) : _type(type), _medium(medium), devids{dev1, dev2}, ports{port1, port2}, devices{local, 0}, db(db) {};
    
    VTyp _type;
    Medium _medium;
    unsigned devids[2];
    unsigned ports[2];
    std::shared_ptr<Geraet> devices[2];
    NetzplanDatenbank* db;

public:
    VTyp type() { return _type; }
    Medium medium() { return _medium; }
    std::shared_ptr<Geraet> loc_dev() { return devices[0]; }
    int loc_port() { return ports[0]; }
    std::shared_ptr<Geraet> rem_dev();
    int rem_port() { return ports[1]; }
};

class Geraet;
class NetzplanDatenbank {
private:
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;
    std::map<unsigned, Geraetetyp*> geraetetypen;

public:
    NetzplanDatenbank(str, str, str);
    vec<Geraet> wohnung(int, int);
    
    /* Should not be used */
    Geraetetyp* findType(unsigned);
    std::shared_ptr<Geraet> geraetbyid(unsigned);
    vec<Verbindung> connection(unsigned, VTyp, std::shared_ptr<Geraet>);
};


class Geraet {
private:
    friend class NetzwerkDatenbank;
    Geraet(unsigned id, int house, int apt, int pos, str name, str mac, str serial, str comment, Geraetetyp *type, NetzplanDatenbank *db) : id(id), _house(house), _apt(apt), _pos(pos), _name(name), _mac(mac), _serial(serial), _comment(comment), _type(type), db(db) {};
    void setself(std::weak_ptr<Geraet> s) {self = s;}
    
    unsigned id;
    int _house;
    int _apt;
    int _pos;
    str _name;
    str _mac;
    str _serial;
    str _comment;
    Geraetetyp *_type;
    NetzplanDatenbank *db;
    std::weak_ptr<Geraet> self;

public:
    int house() {return _house;}
    int apt() {return _apt;}
    int pos() {return _pos;}
    str name() {return _name;}
    str mac() {return _mac;}
    str serial() {return _serial;}
    str comment() {return _comment;}
    Geraetetyp* type() {return _type;}

    vec<Verbindung> patches() { return db->connection(id, VTyp::patchung, self.lock()); }
    vec<Verbindung> cables() { return db->connection(id, VTyp::verkabelung, self.lock()); }
};

vec<Verbindung> NetzplanDatenbank::connection(unsigned gid, VTyp typ, std::shared_ptr<Geraet> origin) {
    vec<Verbindung> v;

    sql::PreparedStatement *pstmt = 0;
    pstmt = con->prepareStatement(
        "SELECT * "
        "FROM " + typtostr(typ) + " "
        "WHERE von_geraet = ? "
        "ORDER BY von_port ;");
    pstmt->setInt(1, gid);

    sql::ResultSet *res = 0;
    res = pstmt->executeQuery();
        
    while (res->next()) {
        v.push_back(std::make_shared<Verbindung>(
                Verbindung(typ,
                           strtomed(res->getString("material")),
                           gid,
                           res->getUInt("zu_geraet"),
                           res->getUInt("von_port"),
                           res->getUInt("zu_port"),
                           origin,
                           this)));
    }
    delete res;
    delete pstmt;

    return v;
}
