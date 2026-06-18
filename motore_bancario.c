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

// --- Cerca utente nel Sistema, Verifica Disponibilità, Effettua P2P e Aggiorna saldi su conti utente ---
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

// --- Cerca Iban nel Sistema, Verifica Disponibilità, Congela Saldo mittente, Prenota il BONIFICO ---
bool effettuaBonifico(Utente* mittente, ListaUtenti listaSistema, const char* ibanBeneficiario, double importo, Data dataSimulata, CodaBonifici* coda) {
    if (importo <= 0) return false;

    // POinter ALiasing -->Iban Mittente deve essere DIVERSO da Iban Beneficiario
    if (strcmp(mittente->iban, ibanBeneficiario) == 0) {
        printf("OPERAZIONE NEGATA: Non puoi disporre un bonifico verso il tuo stesso IBAN.\n");
        return false;
    }
    
    if (!verificaDisponibilita(mittente, importo)) 
        return false;

    mittente->saldo -= importo; // Congela Saldo Mittente
    enqueueBonifico(coda, mittente->username, ibanBeneficiario, importo, dataSimulata);

    // Notifica privata solo per il mittente
    char logTx[150];
    sprintf(logTx, "Bonifico (%.2lf EUR) in coda. Saldo disponibile aggiornato: %.2lf EUR.", importo, mittente->saldo);
    pushNotifica(&(mittente->notifiche), logTx);

    printf(" [V] Bonifico accodato per l'elaborazione interbancaria.\n");
    printf("     Fondi bloccati preventivamente. Nuovo saldo disponibile: %.2lf EUR\n", mittente->saldo);

    return true;
}

// --- Elaborazione Coda Bonifici (ZONA ADMIN) ---
void elaboraCodaBonifici(ListaUtenti listaSistema, Data dataSimulata, CodaBonifici* coda) {
    NodoCodaBonifico *b = dequeueBonifico(coda);
    if (b == NULL) {
        printf(" [!] La coda bonifici e' vuota. Nessuna transazione pendente.\n");
        return;
    }

    printf(" Elaborazione Bonifico: %s -> %s (%.2lf EUR)\n", b->mittente, b->ibanBeneficiario, b->importo);

    // Ricerca Utenti nel sistema (per username -> Mittente e per iban -> Beneficiario) 
    Utente* mittente = cercaUtentePerUsername(listaSistema, b->mittente);
    Utente* beneficiarioInterno = cercaUtentePerIBAN(listaSistema, b->ibanBeneficiario);

    // Aggiorna ANCHE il saldo del BENEFICIARIO se presente nel sistema
    if (mittente != NULL) {
        if (beneficiarioInterno != NULL) {
            beneficiarioInterno->saldo += b->importo;
            aggiungiTransazione(mittente, dataSimulata, "Bonifico_Inviato", b->ibanBeneficiario, b->importo);
            aggiungiTransazione(beneficiarioInterno, dataSimulata, "Bonifico_Ricevuto", mittente->iban, b->importo);

            // Notifica per il destinatario
            char logRx[150];
            sprintf(logRx, "ACCREDITO: Ricevuto bonifico di %.2lf EUR da %s.", b->importo, mittente->username);
            pushNotifica(&(beneficiarioInterno->notifiche), logRx);
        } 
        else // Aggiorna SOLO saldo Mittente 
        {
            aggiungiTransazione(mittente, dataSimulata, "Bonifico_Esterno", b->ibanBeneficiario, b->importo);
        }

        // Notifica di conferma per il mittente
        char logTx[150];
        sprintf(logTx, "ELABORATO: Il tuo bonifico di %.2lf EUR verso %s e' andato a buon fine.", b->importo, b->ibanBeneficiario);
        pushNotifica(&(mittente->notifiche), logTx);

        printf(" [V] Bonifico accreditato con successo!\n");
    }
    free(b); //Deallocazione memoria riservata 
}
