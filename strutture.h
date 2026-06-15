//TUTTE LE STRUTTURE CHE VERRANNO UTILIZZATE ED INTEGRATE NEL PROGETTO

#ifndef STRUTTURE_H_INCLUDED
#define STRUTTURE_H_INCLUDED
#include <stdbool.h>

//--- COLORI ---
#define ANSI_COLOR_RED     "\033[31m"
#define ANSI_COLOR_GREEN   "\033[32m"
#define ANSI_COLOR_YELLOW  "\033[33m"
#define ANSI_COLOR_CYAN    "\033[36m"
#define ANSI_COLOR_RESET   "\033[0m"

//--- UTENTE ---
typedef struct utente {
    char username[50];
    char password[50];
    char iban[15];
    double saldo;
    Transazione *storico;
    Movimento *programmati;
    PilaNotifiche notifiche; // casella postale privata
    struct utente *next;
} Utente;

typedef Utente* ListaUtenti;

//--- TIMESTAMP MOVIMENTI ---
typedef struct {
    int anno;
    int mese;
    int giorno;
    int ora;
    int minuto;
    int secondo;
} Timestamp;

//--- DATA ---
typedef struct {
    int giorno;
    int mese;
    int anno;
} Data;

//--- TRANSAZIONE ---
typedef struct transazione {
    Timestamp dataOraOperazione;
    char tipo[30];
    char controparte[50];
    double importo;
    struct transazione *next;
} Transazione;

//--- MOVIMENTO ---
typedef struct movimento {
    Data dataInizio;
    char descrizione[50];
    double importo;
    int giornoRipetizione;
    int intervalloMesi; // <-- Frequenza del Movimento
    struct movimento *next;
} Movimento;

//--- NOTIFICHE aka nodoPila ---
typedef struct nodoPila {
    char messaggio[150];
    struct nodoPila *next;
} NodoPila;
typedef NodoPila* PilaNotifiche;

// --- nodoCoda per i Bonifici in sospeso ---
typedef struct nodoCoda {
    char mittente[50];
    char ibanBeneficiario[15];
    double importo;
    Data dataInserimento;
    struct nodoCoda *next;
} NodoCodaBonifico;

// --- BONIFICI IN CODA ---
typedef struct {
    NodoCodaBonifico *front;
    NodoCodaBonifico *rear;
} CodaBonifici;

// Gestione Utente
Utente* creaUtente(const char* username, const char* password, const char* iban);
void inserisciUtente(ListaUtenti* lista, Utente* nuovoUtente);
Utente* cercaUtentePerUsername(ListaUtenti lista, const char* username);
Utente* cercaUtentePerIBAN(ListaUtenti lista, const char* iban);

// Gestione Bonifici
void inizializzaCoda(CodaBonifici *coda);
void enqueueBonifico(CodaBonifici *coda, const char* mittente, const char* iban, double importo, Data data);
NodoCodaBonifico* dequeueBonifico(CodaBonifici *coda);

// Gestione Notifiche
void pushNotifica(PilaNotifiche *pila, const char* msg);
void popNotifica(PilaNotifiche *pila);

// Gestione Transazioni
void aggiungiTransazione(Utente* u, Data dataSimulata, const char* tipo, const char* controparte, double importo);

// Gestione  Moivimenti Programmati
void aggiungiMovimentoProgrammato(Utente* u, Data dataInizio, const char* descrizione, double importo, int giornoRipetizione, int intervalloMesi);
void eliminaMovimentoProgrammato(Utente* u, const char* descrizione);
void stampaMovimentiProgrammati(Utente* u);

// Gestione Memoria ed Eliminazione
void liberaDatabase(ListaUtenti db);
bool eliminaUtente(ListaUtenti* lista, const char* username);

#endif // STRUTTURE_H_INCLUDED
