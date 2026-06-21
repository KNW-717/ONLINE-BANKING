#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io_file.h"

// --- Salva il Database -> (username, passsword, iban, saldo, storico, movimenti programmati) ---
void salvaDatabase(ListaUtenti lista, const char* nomeFile) {
    FILE *f = fopen(nomeFile, "w");
    if (f == NULL) return; //errore nell'apertura/creazione del file

    // Per ogni utente nella lista
    Utente *cu = lista;
    while (cu != NULL) {
        fprintf(f, "USER %s %s %s %lf\n", cu->username, cu->password, cu->iban, cu->saldo);

        // Per ogni transazione nello storico
        Transazione *ct = cu->storico;
        while (ct != NULL) {
            fprintf(f, "TRANS %d %d %d %d %d %d %s %s %lf\n",
                ct->dataOraOperazione.anno, ct->dataOraOperazione.mese, ct->dataOraOperazione.giorno,
                ct->dataOraOperazione.ora, ct->dataOraOperazione.minuto, ct->dataOraOperazione.secondo,
                ct->tipo, ct->controparte, ct->importo);
            ct = ct->next;
        }

        // Per ogni movimento programmato
        Movimento *cm = cu->programmati;
        while (cm != NULL) {
            fprintf(f, "MOV %d %d %d %s %lf %d %d\n",
                cm->dataInizio.anno, cm->dataInizio.mese, cm->dataInizio.giorno,
                cm->descrizione, cm->importo, cm->giornoRipetizione, cm->intervalloMesi);
            cm = cm->next;
        }
        cu = cu->next; // utente successivo
    }
    fclose(f); // chiusura del file
} 

// --- Carica dal Database -> (username, passsword, iban, saldo, storico, movimenti programmati) ---
ListaUtenti caricaDatabase(const char* nomeFile) {
    FILE *f = fopen(nomeFile, "r");
  
    if (f == NULL) return; //errore nell'lettura del file
  
    //INIZIALIZZAZIONE PUNTATORI
    ListaUtenti lista = NULL;
    Utente *uTail = NULL;
    Utente *uCorrente = NULL;
  
    Transazione *tTail = NULL;
    Movimento *mTail = NULL;

    char tipoRecord[10];

    // fino a END of Line
    while (fscanf(f, "%s", tipoRecord) != EOF) {
        
        // Se Rileva "USER" --> Legge: user,pass,iban,saldo
        if (strcmp(tipoRecord, "USER") == 0) {
            char user[50], pass[50], iban[15];
            double saldo;
            fscanf(f, "%s %s %s %lf", user, pass, iban, &saldo);

            Utente* nuovo = creaUtente(user, pass, iban);
            nuovo->saldo = saldo;

            // se la lista è vuota, allora diventa primo elemento
            if (lista == NULL) {
                lista = nuovo;
                uTail = nuovo;
            } 
            else // altrimenti mette in coda 
            {
                uTail->next = nuovo;
                uTail = nuovo;
            }
            uCorrente = nuovo;
            tTail = NULL;
            mTail = NULL;
        }
        // Altrimenti Se Rileva "TRANS" --> Alloca spazio in memoria e Legge: anno,mese,giorno,ora,minuto,secondo,tipo,controparte e importo della Transazione
        else if (strcmp(tipoRecord, "TRANS") == 0 && uCorrente != NULL) {
            Transazione *t = (Transazione*)malloc(sizeof(Transazione));
            fscanf(f, "%d %d %d %d %d %d %s %s %lf",
                &t->dataOraOperazione.anno, &t->dataOraOperazione.mese, &t->dataOraOperazione.giorno,
                &t->dataOraOperazione.ora, &t->dataOraOperazione.minuto, &t->dataOraOperazione.secondo,
                t->tipo, t->controparte, &t->importo);
            t->next = NULL;
          
            // se lo storico è vuoto, allora diventa primo elemento
            if (uCorrente->storico == NULL) {
                uCorrente->storico = t;
                tTail = t;
            } 
            else // altrimenti mette in coda 
            {
                tTail->next = t;
                tTail = t;
            }
        }
        // Altrimenti Se Rileva "MOV" --> Alloca spazio in memoria e Legge: anno,mese,giorno,descrizione,importo,giorno in cui si ripete e la frequenza del Movimento Programmato
        else if (strcmp(tipoRecord, "MOV") == 0 && uCorrente != NULL) {
            Movimento *m = (Movimento*)malloc(sizeof(Movimento));
            // Aggiunto &m->intervalloMesi alla lettura
            fscanf(f, "%d %d %d %s %lf %d %d",
                &m->dataInizio.anno, &m->dataInizio.mese, &m->dataInizio.giorno,
                m->descrizione, &m->importo, &m->giornoRipetizione, &m->intervalloMesi);
            m->next = NULL;
          
            // se la lista è vuota, allora diventa primo elemento
            if (uCorrente->programmati == NULL) {
                uCorrente->programmati = m;
                mTail = m;
            } 
            else // altrimenti mette in coda 
            {
                mTail->next = m;
                mTail = m;
            }
        }
    }
    fclose(f); // chiudi file
    return lista; //ritorna la lista (il database)
}
