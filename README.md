<div align="center">

<img src="https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExMjE0ZWNhYmVlODk5MjcwMzExZGUzYTM1YWUwNTdlZTMyMTE1YzVjNiZlcD12MV9pbnRlcm5hbF9naWZzX2dpZklkJmN0PWc/xT9IgzoZoKBvdH1VNC/giphy.gif" width="400" alt="Coding Banking System">

# 🏦 DATASHEET DEFINITIVO - SISTEMA BANCARIO AVANZATO C 🏦

![Lingua](https://img.shields.io/badge/Lingua-C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Architettura](https://img.shields.io/badge/Architettura-Modulare-FF6F00?style=for-the-badge)
![Paradigma](https://img.shields.io/badge/Struttura-Dinamica-8A2BE2?style=for-the-badge)
![Status](https://img.shields.io/badge/WORK_IN_PROGRESS-25%25-orange?style=for-the-badge)

</div>

**AUTORE/PROGETTO:** Gallina Giovanni Antonio / Pacenza Fortunato  
**LINGUAGGIO:** C  
**ARCHITETTURA:** Modulare (Header/Source separation)  
**PARADIGMI IMPLEMENTATI:** Strutture Dinamiche (Liste, Pila, Coda), Ricorsione, Ricerca.  

> ⚠️ **NOTA:** PARADIGMI da INSERIRE:
   *  Ordinamento (BubbleSort ottimizzato)
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
### 🧠 3. Struttura Primaria e Variabili Globali:
*   **Utente:** `{char username[50], char password[50], char iban[15], double saldo, Transazione *storico, Movimento *programmati, PilaNotifiche notifiche, struct utente *next}`  
    *Ruolo*: Nodo master (Chiavi Primarie: username, iban). Contiene le proprie liste.
    
*   **Data dataSimulata (Globale):** Variabile di controllo dello "spazio-tempo" simulato.
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

#### 🔍 -> SOTTO-LOGICHE E DETTAGLI IMPLEMENTATIVI (`strutture.c`):
*   **`time_t rawtime` / `struct tm * timeinfo`:** Variabili della libreria `<time.h>` usate in `generaTimestamp` per estrarre ora, minuto e secondo reali dal SO.
*   **Logica di Sgancio Liste (`eliminaMovimentoProgrammato`):** Utilizza due puntatori (`curr` e `prec`).
  Se prec == NULL`, significa che il nodo da eliminare è la testa, quindi si aggiorna direttamente il puntatore principale alla lista (es. `u->programmati = curr->next`).
