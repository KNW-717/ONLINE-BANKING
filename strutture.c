#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "strutture.h"

// Generazione del TIMESTAMP
Timestamp generaTimestamp(Data dataSimulata) {
    Timestamp ts;
    ts.anno = dataSimulata.anno;
    ts.mese = dataSimulata.mese;
    ts.giorno = dataSimulata.giorno;

    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    ts.ora = timeinfo->tm_hour; ts.minuto = timeinfo->tm_min; ts.secondo = timeinfo->tm_sec;
    return ts;
}

// Creazione dell'UTENTE
Utente* creaUtente(const char* username, const char* password, const char* iban) {
    Utente* nuovo = (Utente*)malloc(sizeof(Utente));
    if (nuovo != NULL) {
        strcpy(nuovo->username, username); strcpy(nuovo->password, password); strcpy(nuovo->iban, iban);

        // --- INIZIALIZZAZIONE ---
        nuovo->saldo = 0.0; nuovo->storico = NULL; nuovo->programmati = NULL;
        nuovo->notifiche = NULL; 
        nuovo->next = NULL;
    }
    return nuovo;
}

// Inserimento dell'Utente nella ListaUtenti
void inserisciUtente(ListaUtenti* lista, Utente* nuovoUtente) {
    if (nuovoUtente == NULL) return;
    nuovoUtente->next = *lista; *lista = nuovoUtente;
}


// Ricerca Ricorsiva per Username
Utente* cercaUtentePerUsername(ListaUtenti lista, const char* username) {
    // Caso Base 1: Lista finita o vuota
    if (lista == NULL) 
        return NULL;
    
    // Caso Base 2: Trovato
    if (strcmp(lista->username, username) == 0) 
        return lista;
    
    // Passo Ricorsivo: Cerca nel resto della lista
    return cercaUtentePerUsername(lista->next, username);
}

// Ricerca Ricorsiva per IBAN
Utente* cercaUtentePerIBAN(ListaUtenti lista, const char* iban) {
    // Caso Base 1: Lista finita o vuota
    if (lista == NULL) 
        return NULL;
  
    // Caso Base 2: Trovato
    if (strcmp(lista->iban, iban) == 0) 
        return lista;
  
    // Passo Ricorsivo: Cerca nel resto della lista
    return cercaUtentePerIBAN(lista->next, iban);
}

// Creazione Transazione
void aggiungiTransazione(Utente* u, Data dataSimulata, const char* tipo, const char* controparte, double importo) {
    Transazione* nuova = (Transazione*)malloc(sizeof(Transazione));
    if (nuova == NULL) return;
    
    nuova->dataOraOperazione = generaTimestamp(dataSimulata);
    strcpy(nuova->tipo, tipo); 
    strcpy(nuova->controparte, controparte); 
    nuova->importo = importo;
    nuova->next = u->storico;
    u->storico = nuova;
}

// Creazione Movimento Programmato
void aggiungiMovimentoProgrammato(Utente* u, Data dataInizio, const char* descrizione, double importo, int giornoRipetizione, int intervalloMesi) {
    Movimento* nuovo = (Movimento*)malloc(sizeof(Movimento));
    if (nuovo == NULL) return;
    
    nuovo->dataInizio = dataInizio;
    strcpy(nuovo->descrizione, descrizione);
    nuovo->importo = importo;
    nuovo->giornoRipetizione = giornoRipetizione;
    nuovo->intervalloMesi = intervalloMesi; // <-- Frequenza Mensile
    nuovo->next = u->programmati;
    u->programmati = nuovo;
}

// Visualizzazione Movimenti Programmati
void stampaMovimentiProgrammati(Utente* u) {
    Movimento* curr = u->programmati;
    if (curr == NULL) {
        printf(ANSI_COLOR_YELLOW "    Nessun movimento programmato attivo.\n" ANSI_COLOR_RESET);
        return;
    }
    while (curr != NULL) {
        const char* colore = (curr->importo >= 0) ? ANSI_COLOR_GREEN : ANSI_COLOR_RED; //Formattazione compatta if-else
        char segno = (curr->importo >= 0) ? '+' : ' '; // Il meno è già incluso nei negativi
        
        printf("    -> %-15s | %s%c%8.2lf EUR%s | Ogni %2d mesi il %02d | Dal: %02d/%04d\n",
               curr->descrizione, colore, segno, curr->importo, ANSI_COLOR_RESET,
               curr->intervalloMesi, curr->giornoRipetizione,
               curr->dataInizio.mese, curr->dataInizio.anno);
        curr = curr->next;
    }
}

// Eliminazione Movimento Programmato
void eliminaMovimentoProgrammato(Utente* u, const char* descrizione) {
    Movimento* curr = u->programmati;
    Movimento* prec = NULL;
    while (curr != NULL) {
        if (strcmp(curr->descrizione, descrizione) == 0) {
            if (prec == NULL) 
                u->programmati = curr->next;
            else 
                prec->next = curr->next;       
            
            free(curr);            //liberazione della memoria    
            printf(" [V] Movimento '%s' eliminato con successo!\n", descrizione);
            return;
        }
        // swap
        prec = curr; 
        curr = curr->next;
    }
    printf(" [X] Errore: Nessun movimento trovato con il nome '%s'.\n", descrizione);
}

// --- Implementazione NOTIFICHE (Pila) ---
void pushNotifica(PilaNotifiche *pila, const char* msg) {
    NodoPila *nuovo = (NodoPila*)malloc(sizeof(NodoPila));
    if(nuovo != NULL) {
        strcpy(nuovo->messaggio, msg);
        nuovo->next = *pila;
        *pila = nuovo;
    }
}

void popNotifica(PilaNotifiche *pila) {
    if (*pila == NULL) {
        printf(" Nessuna nuova notifica da leggere.\n");
        return;
    }
    NodoPila *temp = *pila;
    printf(" NOTIFICA RECENTE: %s\n", temp->messaggio);
    *pila = (*pila)->next;
    free(temp);
}

// --- Implementazione BONIFICI (Coda) ---
void inizializzaCoda(CodaBonifici *coda) {
    coda->front = NULL;
    coda->rear = NULL;
}

// --- Creazione Bonifico in Coda ---
void enqueueBonifico(CodaBonifici *coda, const char* mittente, const char* iban, double importo, Data data) {
    NodoCodaBonifico *nuovo = (NodoCodaBonifico*)malloc(sizeof(NodoCodaBonifico));
    if (nuovo == NULL) return;
    
    strcpy(nuovo->mittente, mittente);
    strcpy(nuovo->ibanBeneficiario, iban);
    nuovo->importo = importo;
    nuovo->dataInserimento = data;
    nuovo->next = NULL;

    if (coda->rear == NULL) {
        coda->front = coda->rear = nuovo;
        return;
    }
    coda->rear->next = nuovo;
    coda->rear = nuovo;
}

// --- Elaborazione Bonifico ---
NodoCodaBonifico* dequeueBonifico(CodaBonifici *coda) {
    if (coda->front == NULL) return NULL;
    NodoCodaBonifico *temp = coda->front;
    coda->front = coda->front->next;
    if (coda->front == NULL) 
        coda->rear = NULL;
    return temp;
}

// --- Prevenzione Memory Leak: Deallocazione completa dell'albero dei dati ---
void liberaDatabase(ListaUtenti db) { 
    Utente* currU = db;

    //Seleziona tutti gli utenti di volta in volta
    while (currU != NULL) {
        Utente* nextU = currU->next;

        //Libera Memoria Transazioni
        Transazione* currT = currU->storico;
        while (currT != NULL) {
            Transazione* nextT = currT->next;
            free(currT);
            currT = nextT;
        }
        
        //Libera Memoria Movimenti
        Movimento* currM = currU->programmati;
        while (currM != NULL) {
            Movimento* nextM = currM->next;
            free(currM);
            currM = nextM;
        }

        //Libera Memoria Notifiche
        NodoPila* currP = currU->notifiche;
        while (currP != NULL) {
            NodoPila* nextP = currP->next;
            free(currP);
            currP = nextP;
        }

        //Elimina per ogni Utente
        free(currU);
        currU = nextU;
    }
}

// ---Eliminazione Utente da Database (serve a admin) ---
bool eliminaUtente(ListaUtenti* lista, const char* username) {
    Utente* curr = *lista;
    Utente* prec = NULL;

    while (curr != NULL) {
        if (strcmp(curr->username, username) == 0) {
            
            // Fase 1: Sgancio dalla Lista Principale
            if (prec == NULL) {
                *lista = curr->next; // Era il primo elemento
            } else {
                prec->next = curr->next; // Era nel mezzo o in fondo
            }

            // Fase 2: Deallocazione dello Storico Transazioni
            Transazione* t = curr->storico;
            while (t != NULL) {
                Transazione* nextT = t->next;
                free(t);
                t = nextT;
            }

            // Fase 3: Deallocazione dei Movimenti Programmati
            Movimento* m = curr->programmati;
            while (m != NULL) {
                Movimento* nextM = m->next;
                free(m);
                m = nextM;
            }
            // Fae 4: Pulizia NOtifiche 
            NodoPila* p = curr->notifiche;
            while (p != NULL) {
            NodoPila* nextP = p->next;
            free(p);
            p = nextP;
        }

            // Fase 5: Deallocazione del Nodo Utente
            free(curr);
            return true; // Eliminazione completata con successo
        }
        prec = curr;
        curr = curr->next;
    }

    return false; // Utente non trovato
}



