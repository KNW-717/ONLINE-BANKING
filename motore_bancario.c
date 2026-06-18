#include <stdio.h>
#include <string.h>
#include "motore_bancario.h"
#include <stdlib.h>

// --- IMPORTANTE: Verifica Disponibiilità Fondi prima di ogni Operazione ---
bool verificaDisponibilita(Utente* u, double importo) {
    if (u->saldo < 0 || (u->saldo - importo) < 0) {
        printf("OPERAZIONE NEGATA: Fondi insufficienti o conto in rosso.\n");
        return false;
    }
    return true;
}

// --- Effettua Ricarica e Aggiorna saldo su conto utente ---
 bool effettuaRicarica(Utente* u, double importo, Data dataSimulata) {
    if (importo <= 0) return false;
    u->saldo += importo;
    aggiungiTransazione(u, dataSimulata, "Ricarica", "Se_Stesso", importo);
    printf("Ricarica di %.2lf EUR effettuata.\n", importo);
    return true;
}

// ---Verifica Disponibilità, Effettua Prelievo e Aggiorna saldo su conto utente ---
bool effettuaPrelievo(Utente* u, double importo, Data dataSimulata) {
    if (importo <= 0 || !verificaDisponibilita(u, importo)) return false;
    u->saldo -= importo;
    aggiungiTransazione(u, dataSimulata, "Prelievo", "Se_Stesso", importo);
    printf("Prelievo di %.2lf EUR effettuato.\n", importo);
    return true;
}
