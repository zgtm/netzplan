#include "orm.h"

std::shared_ptr<Geraet> Verbindung::rem_dev() {
    if (!devices[1].get())        
        devices[1] = db->geraetbyid(devids[1]);
    return devices[1];
}

NetzplanDatenbank::NetzplanDatenbank(str db_user, str db_pw, str db_dbname) : driver(0), con(0) {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://localhost", db_user, db_pw);
        con->setSchema(db_dbname);
    } catch (sql::SQLException &e) {
        std::cerr << "# ERR: " << e.what()
                  << " (MySQL error code: " << e.getErrorCode()
                  << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        throw str("SQL connection not possible");
    }
}

Geraetetyp* NetzplanDatenbank::findType(unsigned id) {
    auto it = geraetetypen.find(id);
    if (it != geraetetypen.end())
        return it->second;

    sql::PreparedStatement *pstmt = 0;
    pstmt = con->prepareStatement(
        "SELECT * "
        "FROM geraetetyp "
        "WHERE id = ? ;");
    pstmt->setInt(1, id);

    sql::ResultSet *res = 0;
    res = pstmt->executeQuery();
        
    if (res->next()) {
        geraetetypen[id] = new Geraetetyp(res->getUInt("id"),
                                          res->getString("typname"),
                                          res->getUInt("n_ports"),
                                          res->getString("hersteller"),
                                          res->getString("modell"),
                                          res->getString("kommentar"),
                                          this);
    }
    else {
        geraetetypen[id] = 0;
    }
    delete res;
    delete pstmt;

    return geraetetypen[id];
}

std::shared_ptr<Geraet> NetzplanDatenbank::geraetbyid(unsigned id) {
    std::shared_ptr<Geraet> g(0);
    
    sql::PreparedStatement *pstmt = 0;
    pstmt = con->prepareStatement(
        "SELECT * "
        "FROM geraet "
        "WHERE id = ? ;");
    pstmt->setUInt(1, id);

    sql::ResultSet *res = 0;
    res = pstmt->executeQuery();
        
    if (res->next()) {
        g = std::make_shared<Geraet>(Geraet(id,
                                    res->getInt("ort_haus"),
                                    res->getInt("ort_apartment"),
                                    res->getInt("ort_position"),
                                    res->getString("name"),
                                    res->getString("mac"),
                                    res->getString("seriennummer"),
                                    res->getString("kommentar"),
                                    findType(res->getUInt("typ")),
                                    this));
    }
    delete res;
    delete pstmt;
    
    return g;
}


vec<Geraet> NetzplanDatenbank::wohnung(int haus, int apt) {
    vec<Geraet> v;

    sql::PreparedStatement *pstmt = 0;
    pstmt = con->prepareStatement(
        "SELECT * "
        "FROM geraet "
        "WHERE ort_haus = ? AND ort_apartment = ? "
        "ORDER BY ort_position ;");
    pstmt->setInt(1, haus);
    pstmt->setInt(2, apt);

    sql::ResultSet *res = 0;
    res = pstmt->executeQuery();
        
    while (res->next()) {
        auto g = std::make_shared<Geraet>(
                 Geraet(res->getUInt("id"),
                        haus,
                        apt,
                        res->getInt("ort_position"),
                        res->getString("name"),
                        res->getString("mac"),
                        res->getString("seriennummer"),
                        res->getString("kommentar"),
                        findType(res->getUInt("typ")),
                        this));
        g->setself(g);
        v.push_back(g);
    }
    delete res;
    delete pstmt;

    return v;
}
