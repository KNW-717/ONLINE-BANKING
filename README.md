<div align="center">

<img src="https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExMjE0ZWNhYmVlODk5MjcwMzExZGUzYTM1YWUwNTdlZTMyMTE1YzVjNiZlcD12MV9pbnRlcm5hbF9naWZzX2dpZklkJmN0PWc/xT9IgzoZoKBvdH1VNC/giphy.gif" width="400" alt="Coding Banking System">

# 🏦 DATASHEET (NON) DEFINITIVO - SISTEMA BANCARIO AVANZATO C 🏦

![Lingua](https://img.shields.io/badge/Lingua-C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Architettura](https://img.shields.io/badge/Architettura-Modulare-FF6F00?style=for-the-badge)
![Paradigma](https://img.shields.io/badge/Struttura-Dinamica-8A2BE2?style=for-the-badge)
![Status](https://img.shields.io/badge/WORK_IN_PROGRESS-35%25-orange?style=for-the-badge)

</div>

**AUTORE/PROGETTO:** Gallina Giovanni Antonio / Pacenza Fortunato  
**LINGUAGGIO:** C  
**ARCHITETTURA:** Modulare (Header/Source separation)  
**PARADIGMI IMPLEMENTATI:** Strutture Dinamiche (Liste, Pila, Coda), Ricorsione, Ricerca, Ordinamento (BubbleSort ottimizzato), Filtraggio Dati


> ⚠️ **NOTA:** PARADIGMI da INSERIRE:
   *  File Binari / Database
   *  Gestione persistente dei file

  
---

## 🗃️ SEZIONE 1: STRUTTURE DATI E VARIABILI 

### 🕒 1. Strutture Temporali:
*   **Timestamp:** `{int anno, mese, giorno, ora, minuto, secondo}` -> Data esatta.
*   **Data:** `{int giorno, mese, anno}` -> Rappresentazione di calendario semplificata.

### 🔗 2. Nodi e Strutture Lineari Dinamiche:
*   **Transazione:** `{Timestamp dataOraOperazione, char tipo[30], char controparte[50], double importo, struct transazione *next}`  
    *Ruolo*: Nodo della Lista Concatenata per lo storico movimenti.

*   **Movimento:** `{Data dataInizio, char descrizione[50], double importo, int giornoRipetizione, int intervalloMesi, struct movimento *next}`  
    *Ruolo*: Nodo della Lista Concatenata per entrate/uscite ricorrenti.

*   **NodoPila:** `{char messaggio[150], struct nodoPila *next}`  
    *Ruolo*: Nodo della struttura LIFO (Last-In-First-Out) per le notifiche asincrone.
    
*   **NodoCodaBonifico:** `{char mittente[50], char ibanBeneficiario[15], double importo, Data dataInserimento, struct nodoCoda *next}`  
    *Ruolo*: Nodo della struttura FIFO (First-In-First-Out) per l'elaborazione bonifici.
    
*   **CodaBonifici:** `{NodoCodaBonifico *front, NodoCodaBonifico *rear}`  
    *Ruolo*: Contenitore dei puntatori di testa e coda per accodamento.

### 🧠 3. Struttura Primaria e Variabili Globali:
*   **Utente:** `{char username[50], char password[50], char iban[15], double saldo, Transazione *storico, Movimento *programmati, PilaNotifiche notifiche, struct utente *next}`  
    *Ruolo*: Nodo master (Chiavi Primarie: username, iban). Contiene le proprie liste.
    
*   **Data dataSimulata (Globale):** Variabile di controllo dello "spazio-tempo" simulato.
  
*   **CodaBonifici codaCentrale (Globale):** Coda di sistema per la Gestione dei Bonifici.
---
## ⚙️ SEZIONE 2: MODULI E FUNZIONI IMPLEMENTATE (CON DETTAGLI ARCHITETTURALI)
### 🏗️ --- MODULO: STRUTTURE (`strutture.h` / `strutture.c`) ---
**Responsabilità:** Allocazione/Deallocazione RAM e manipolazione liste/pile/code.

*   **generaTimestamp:** Restituisce l'ora di sistema agganciata alla Data simulata.
*   **creaUtente:** Usa `malloc()` e inizializza i puntatori e il saldo a 0.
*   **inserisciUtente:** Head-Insertion (inserimento in testa, asintotica O(1)).
*   **cercaUtentePerUsername:** Funzione RICORSIVA (Divide et Impera). Ritorna il puntatore.
*   **cercaUtentePerIBAN:** Funzione RICORSIVA per il controllo dell'IBAN.
*   **aggiungiTransazione:** Head-Insertion nello storico dell'utente.
*   **aggiungiMovimentoProgrammato:** Head-Insertion nei piani ricorrenti.
*   **stampaMovimentiProgrammati:** Scorrimento con applicazione di codici Colore ANSI.
*   **eliminaMovimentoProgrammato:** Attraversamento lista con sgancio nodo e `free()`.
*   **pushNotifica:** Inserimento in cima alla Pila (LIFO).
*   **popNotifica:** Estrazione e deallocazione (`free`) del messaggio in cima alla Pila.
*   **inizializzaCoda:** Imposta front e rear a NULL.
*   **enqueueBonifico:** Inserimento rapido in coda (`rear->next = nuovo`).
*   **dequeueBonifico:** Estrazione rapida dalla testa (`front = front->next`) per l'Admin.
*   **liberaDatabase:** Scorre l'albero Utente->Transazione+Movimento+Pila e distrugge ogni nodo con `free()`. Previene memory leak.
*   **eliminaUtente:** Scorre la ListaUtenti, sgancia i puntatori prev/next e chiama la deallocazione a cascata (Transazioni, Movimenti, Notifiche) prima di eseguire la `free()` sul nodo radice.

#### 🔍 -> SOTTO-LOGICHE E DETTAGLI IMPLEMENTATIVI (`strutture.c`):
*   **`time_t rawtime` / `struct tm * timeinfo`:** Variabili della libreria `<time.h>` usate in `generaTimestamp` per estrarre ora, minuto e secondo reali dal SO.
*   **Logica di Sgancio Liste (`eliminaMovimentoProgrammato` / `eliminaUtente`):** Utilizza due puntatori (`curr` e `prec`). prec == NULL,`significa che il nodo da eliminare è la testa`, quindi si aggiorna direttamente il puntatore principale alla lista (es. `u->programmati = curr->next`).
  
### 🏦 --- MODULO: MOTORE BANCARIO (`motore_bancario.h` / `motore_bancario.c`) ---
**Responsabilità:** Calcolo matematico, alterazione saldi e gestione transazionale.

*   **verificaDisponibilita:** Clausola di Guardia per prevenire che il conto vada in rosso.
*   **effettuaRicarica:** Incrementa il saldo e chiama `aggiungiTransazione`.
*   **effettuaPrelievo:** Controlla fondi, decrementa e chiama `aggiungiTransazione`.
*   **effettuaBonifico:** Modello "Pre-Autorizzazione". Scala il saldo (blocco fondi) e prenota Bonifico chiamando `enqueueBonifico()`. Avvisa il mittente.
*   **elaboraCodaBonifici:** Chiama `dequeueBonifico()`. Trova mittente e destinatario (se interno). Accredita fondi, traccia log storico e invia notifiche asincrone.

#### 🔍 -> SOTTO-LOGICHE E DETTAGLI IMPLEMENTATIVI (`motore_bancario.c`):
*   **Pointer Aliasing P2P (`if mittente == beneficiario`):** Misura di sicurezza in `effettuaP2P`. Impedisce che il sistema processi le operazioni `+=` e `-=` sulla stessa area di memoria simultaneamente, annullando la validità del saldo.
*   **Pointer Aliasing BONIFICO(`if (strcmp(mittente->iban,ibanBeneficiario)==0)`):** Misura di sicurezza in `EffettuaBonifico`. Impedisce che il sistmea processi le operazioni `+=` e `-=` sulla stessa area di memoria simultaneamente, annullando la validità del saldo.
*   **Buffer di Log (`char log[150]`):** Stringhe temporanee pre-formattate tramite `sprintf()` utilizzate per iniettare i valori float (`%.2lf`) e le stringhe `%s` nelle code delle PilaNotifiche.

### 🔀 --- MODULO: ORDINAMENTO E FILTRI (`ordinamento.h` / `ordinamento.c`) ---
**Responsabilità:** Algoritmi di Sorting logico (Bubble Sort ottimizzato) e filtraggio data.

*   **scambiaTransazioni / scambiaUtenti:** Funzioni helper di swap dei dati.
*   **comparaTimestamp:** Verifica priorità tra date e tempi (anno, mese, ecc).
*   **ordinaStorico:** Bubble Sort pre-condizionato (con 2 criteri). Ordina le transazioni dell'utente.
*   **ordinaUtenti:** Bubble Sort sulle stringhe e i double. Ordina gli utenti per saldo.
*   **filtraStoricoPerMese:** Analizza le struct e usa `strstr()` (libreria `string.h`) per decifrare l'output visivo e stampare i colori.
#### 🔍 -> SOTTO-LOGICHE E DETTAGLI IMPLEMENTATIVI (`ordinamento.c`):
*   **`bool scambiato`:** Variabile di flag usata nei Bubble Sort per uscire dal ciclo while prematuramente se l'array risulta già ordinato (Ottimizzazione Caso Migliore).
*   **Puntatore `lptr`:** Limite destro del Bubble Sort. A ogni iterazione del ciclo principale, `lptr` viene arretrato all'ultimo nodo scambiato, restringendo il campo di ricerca così da dover analizzare meno nodi.
*   **`strstr()` in `filtraStoricoPerMese`:** Sottofunzione di libreria (<string.h>) che cerca substringhe (es. "Prelievo") per dedurre se applicare ANSI_COLOR_RED o ANSI_COLOR_GREEN e "-" o "+" alla stampa su termiinale.
