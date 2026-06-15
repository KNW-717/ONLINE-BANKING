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
