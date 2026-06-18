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

// --- Cerca utente nel Sistema, Verifica Disponibilità, Effettua P2P e Aggiorna sald su conti utente ---
bool effettuaP2P(Utente* mittente, ListaUtenti listaSistema, const char* usernameBeneficiario, double importo, Data dataSimulata) {
    if (importo <= 0) return false;

    // Ricerca Utente nel sistema
    Utente* beneficiario = cercaUtentePerUsername(listaSistema, usernameBeneficiario);
    if (beneficiario == NULL) {
        printf("ERRORE: Utente '%s' inesistente.\n", usernameBeneficiario);
        return false;
    }
    
    // Pointer Aliasing --> Mittente deve essere diverso da Beneficiario
    if (mittente == beneficiario) {
        printf("OPERAZIONE NEGATA: Non puoi inviare un P2P a te stesso.\n");
        return false;
    }
    
    if (!verificaDisponibilita(mittente, importo)) 
        return false;
    // Aggiorna Saldi
    mittente->saldo -= importo;
    beneficiario->saldo += importo;
    aggiungiTransazione(mittente, dataSimulata, "P2P_Inviato", usernameBeneficiario, importo);
    aggiungiTransazione(beneficiario, dataSimulata, "P2P_Ricevuto", mittente->username, importo);

    // Notifica per il mittente
    char logTx[150];
    sprintf(logTx, "Hai inviato %.2lf EUR a %s tramite P2P", importo, usernameBeneficiario);
    pushNotifica(&(mittente->notifiche), logTx);

    // Notifica per il destinatario
    char logRx[150];
    sprintf(logRx, "Hai ricevuto %.2lf EUR da %s tramite P2P", importo, mittente->username);
    pushNotifica(&(beneficiario->notifiche), logRx);

    printf("Trasferimento P2P completato.\n");
    return true;
}
