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

// ---Generazione Previsione per n mesi, con: saldo virtuale, data iterativa, controlli su saldo in rosso e rilevamento mese in cui saldo va in rosso, report finale ---
void generaPrevisione(Utente* u, Data dataSimulataCorrente, int mesiPrevisione) {
    double saldoVirtuale = u->saldo;
    Data dataIterativa = dataSimulataCorrente;
    Data dataFine = dataSimulataCorrente;
    dataFine.mese += mesiPrevisione;
    
    // Check cambio anno
    while (dataFine.mese > 12) 
        {
         dataFine.mese -= 12; 
         dataFine.anno++; 
        }

    printf(ANSI_COLOR_CYAN "\n=== PREVISIONE A %d MESI ===\n" ANSI_COLOR_RESET, mesiPrevisione);

    // Variabili per riportare se il conto è andato in rosso e dopo quanti mesi è successo 
    bool primoRossoRilevato = false;
    int mesiAlPrimoRosso = 0;

    // Finché non arriva alla data prevista per la fine della simulazione
    while (!(dataIterativa.anno == dataFine.anno && dataIterativa.mese == dataFine.mese && dataIterativa.giorno == dataFine.giorno)) {
        Movimento* m = u->programmati;
        while (m != NULL) {
            //calcolo dei giorni presenti in ogni mese simulato
            int giorniMeseCorrente = giorniInMese(dataIterativa.mese, dataIterativa.anno);
            int giornoScadenza = (m->giornoRipetizione > giorniMeseCorrente) ? giorniMeseCorrente : m->giornoRipetizione;

            // calcolo della differenza di mesi dalla data simulata alla data in cui è stato programmato il movimento
            int deltaMesi = (dataIterativa.anno - m->dataInizio.anno) * 12 + (dataIterativa.mese - m->dataInizio.mese);

            // verifica sulla data del movimento programmato, se siamo nel giorno e nel mese e con la periodicità corretta del movimento programmato, allora bisogna aggiornare il saldo Virtuale
            if (dataMaggioreOUguale(dataIterativa, m->dataInizio) && dataIterativa.giorno == giornoScadenza && (deltaMesi % m->intervalloMesi == 0)) {
                saldoVirtuale += m->importo;
                if (m->importo >= 0) {
                    printf("[%02d/%02d/%04d] Introito '%s' (" ANSI_COLOR_GREEN "+%.2lf EUR" ANSI_COLOR_RESET "). Saldo: %.2lf\n", dataIterativa.giorno, dataIterativa.mese, dataIterativa.anno, m->descrizione, m->importo, saldoVirtuale);
                } 
                else 
                {
                    printf("[%02d/%02d/%04d] Uscita '%s' (" ANSI_COLOR_RED "%.2lf EUR" ANSI_COLOR_RESET "). Saldo: %.2lf\n",
                           dataIterativa.giorno, dataIterativa.mese, dataIterativa.anno, m->descrizione, m->importo, saldoVirtuale);

                    //controllo saldo va in rosso
                    if (saldoVirtuale < 0) {
                        printf(ANSI_COLOR_RED "\n[!!!] COLLISIONE RILEVATA! Impossibile pagare '%s' [!!!]\n" ANSI_COLOR_RESET, m->descrizione);
                        printf(ANSI_COLOR_YELLOW "Azione Consigliata: Spostare date o ricaricare almeno %.2lf EUR.\n\n" ANSI_COLOR_RESET, (saldoVirtuale * -1));
                        if (!primoRossoRilevato) {
                            //salvataggio dopo quanti mesi da inizio simulazione è stato rilevato il primo rosso
                            mesiAlPrimoRosso = (dataIterativa.anno - dataSimulataCorrente.anno) * 12 + (dataIterativa.mese - dataSimulataCorrente.mese);
                            primoRossoRilevato = true;
                        }
                    }
                }
            }
            m = m->next; ///passaggio a successivo movimento programmato
        }
        avanzaGiorno(&dataIterativa); // finché data simulata != data fine simulazione
    }

    const char* coloreSaldo = (saldoVirtuale >= 0) ? ANSI_COLOR_GREEN : ANSI_COLOR_RED;

    printf(ANSI_COLOR_CYAN "===================================================\n");
    printf("=== SALDO FINALE STIMATO: %s%.2lf EUR" ANSI_COLOR_CYAN " ===\n" ANSI_COLOR_RESET, coloreSaldo, saldoVirtuale);

    if (primoRossoRilevato) {
        if (mesiAlPrimoRosso == 0) {
            printf(ANSI_COLOR_YELLOW "[!] RESOCONTO: Il conto andra' in negativo gia' nel mese corrente di simulazione.\n" ANSI_COLOR_RESET);
        } else {
            printf(ANSI_COLOR_YELLOW "[!] RESOCONTO: Il conto andra' in negativo esattamente dopo %d mesi dall'inizio della simulazione.\n" ANSI_COLOR_RESET, mesiAlPrimoRosso);
        }
    } else {
        printf(ANSI_COLOR_GREEN "[V] RESOCONTO: Il conto rimarra' stabile e in positivo per tutti i %d mesi simulati.\n" ANSI_COLOR_RESET, mesiPrevisione);
    }
    printf(ANSI_COLOR_CYAN "===================================================\n\n" ANSI_COLOR_RESET);
}
// .
// . .
// . . .
// . . . . qui giacono le gocce di sudore versate per pensare,realizzare e, soprattutto, far funzionare questo pezzetto di codice . . . .
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
