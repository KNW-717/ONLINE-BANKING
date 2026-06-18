#include <stdio.h>
#include <stdbool.h>
#include "previsione.h"

// --- Funzione per restituire quanti giorni in un mese, compreso Febbraio (con funzione per calcolo anno bisestile)
int giorniInMese(int mese, int anno) {
    if (mese == 2) 
      return ((anno % 4 == 0 && anno % 100 != 0) || (anno % 400 == 0)) ? 29 : 28;
    if (mese == 4 || mese == 6 || mese == 9 || mese == 11) 
      return 30;
    return 31;
}

// --- Funzione per far avanzare i giorni nella simulazione (con controllo mese e anno) ---
void avanzaGiorno(Data* d) {
    d->giorno++;
  // Controllo se è cambiato il mese 
    if (d->giorno > giorniInMese(d->mese, d->anno)) {
        d->giorno = 1;
        d->mese++;
  // Controlla se è cambiato l'anno
        if (d->mese > 12) { d->mese = 1; d->anno++; }
    }
}

// --- Funzione per confrontare 2 Date -> (se d1 è maggiore o uguale di d2) ((serve come funzione di supporto)) --- 
bool dataMaggioreOUguale(Data d1, Data d2) {
    if (d1.anno != d2.anno) return d1.anno > d2.anno;
    if (d1.mese != d2.mese) return d1.mese > d2.mese;
    return d1.giorno >= d2.giorno;
}
