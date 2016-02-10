#include <iostream>
#include "orm.h"

int main() {
    NetzplanDatenbank np("netzplan", "netzplan", "netzplan");

    std::cout << "Alle Geraete in 12/01: \n";
    for (auto g : np.wohnung(12,1)) {
        std::cout << g->pos() << ": " << g->type()->name() << " (" << g->name() << ")\n";
        for (auto p : g->patches()) {
            std::cout << " - " << p->rem_dev()->name() << " (" << p->rem_dev()->house()  << ", " <<  p->rem_dev()->apt()  << ")\n";
        }
    }
    
}
