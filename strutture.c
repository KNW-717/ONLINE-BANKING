#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "strutture.h"

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


