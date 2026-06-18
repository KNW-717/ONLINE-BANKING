#include <stdio.h>
#include <stdbool.h>
#include "ordinamento.h"
#include <string.h>

// --- Scambia Transazioni  ---
void scambiaTransazioni(Transazione* a, Transazione* b) {
    Transazione temp = *a;
    Transazione* nextA = a->next;
    Transazione* nextB = b->next;

    *a = *b; a->next = nextA;
    *b = temp; b->next = nextB;
}

// --- Scambia Utenti ---
void scambiaUtenti(Utente* a, Utente* b) {
    Utente temp = *a;
    Utente* nextA = a->next;
    Utente* nextB = b->next;

    *a = *b; a->next = nextA;
    *b = temp; b->next = nextB;
}

// --- Compara Timestamp per tutte le operazioni in cui è necessario ---
int comparaTimestamp(Timestamp t1, Timestamp t2) {
    if (t1.anno != t2.anno) return t1.anno > t2.anno ? 1 : -1;
    if (t1.mese != t2.mese) return t1.mese > t2.mese ? 1 : -1;
    if (t1.giorno != t2.giorno) return t1.giorno > t2.giorno ? 1 : -1;
    if (t1.ora != t2.ora) return t1.ora > t2.ora ? 1 : -1;
    if (t1.minuto != t2.minuto) return t1.minuto > t2.minuto ? 1 : -1;
    if (t1.secondo != t2.secondo) return t1.secondo > t2.secondo ? 1 : -1;
    return 0;
}

// --- Ordina Storico in base al Criterio scelto nel main ---
void ordinaStorico(Utente* u, int criterio) {
    if (u->storico == NULL || u->storico->next == NULL) 
        return;

    bool scambiato = true; // ci serve per entrare nel while
    Transazione* ptr1;
    Transazione* lptr = NULL;

    while (scambiato) {
        scambiato = false;
        ptr1 = u->storico;
        while (ptr1->next != lptr) {
            bool condizioneSwap = false;
            int cmp = comparaTimestamp(ptr1->dataOraOperazione, ptr1->next->dataOraOperazione);
            
            // si prevede la scelta del criterio in base al numero selezionato nel main (2 criteri)
            if (criterio == 1 && cmp > 0) condizioneSwap = true;
            else if (criterio == 2 && cmp < 0) condizioneSwap = true;

            // swap
            if (condizioneSwap) {
                scambiaTransazioni(ptr1, ptr1->next);
                scambiato = true;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
}

// --- Ordina Utenti in base al Criterio scelto nel main ---
void ordinaUtenti(ListaUtenti* lista, int criterio) {
    if (*lista == NULL || (*lista)->next == NULL) return;

    bool scambiato = true;
    Utente* ptr1;
    Utente* lptr = NULL;

    while (scambiato) {
        scambiato = false;
        ptr1 = *lista;
        while (ptr1->next != lptr) {
            bool condizioneSwap = false;
            
            // si prevede la scelta del criterio in base al numero selezionato nel main (2 criteri)
            if (criterio == 1 && ptr1->saldo < ptr1->next->saldo) condizioneSwap = true;
            else if (criterio == 2 && ptr1->saldo > ptr1->next->saldo) condizioneSwap = true;

            // swap
            if (condizioneSwap) {
                scambiaUtenti(ptr1, ptr1->next);
                scambiato = true;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
}
