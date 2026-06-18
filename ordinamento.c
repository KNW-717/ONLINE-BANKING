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
