#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "strutture.h"
#include "io_file.h"
#include "motore_bancario.h"
#include "previsione.h"
#include "ordinamento.h"

#define FILE_DB "database_banca.txt"
CodaBonifici codaCentrale;
Data dataSimulata;

void pulisciBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pulisciSchermo() {
    system("clear || cls");
}

void attendiInvio() {
    printf("\nPremi INVIO per continuare...");
    getchar();
}

void stampaIntestazione(const char* titolo) {
    pulisciSchermo();
    printf(ANSI_COLOR_CYAN "+=================================================+\n");
    printf("| %-47s |\n", titolo);
    printf("+=================================================+\n" ANSI_COLOR_RESET);
}

// Sottocaso: Acquisizione stringhe  (evita buffer overflow e spazi non voluti)
void leggiStringa(char* buffer, int size) {
    bool valido = false;
    while (!valido) {
        fgets(buffer, size, stdin);
        int len = strlen(buffer);

        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
            len--;
        }

        if (len > 0) {
            valido = true;
            // Sanitizzazione per il DB: converto gli spazi in underscore
            // affinché fscanf in io_file.c continui a leggere il record come stringa singola.
            for (int i = 0; i < len; i++) {
                if (buffer[i] == ' ') buffer[i] = '_';
            }
        } else {
            printf(ANSI_COLOR_RED" [X] Errore: Inserimento vuoto non ammesso. Riprova: "ANSI_COLOR_RESET);
        }
    }
}

int leggiIntero(bool consentiNegativo) {
    char buffer[50];
    bool valido = false;

    while (!valido) {
        valido = true;
        scanf("%49s", buffer);
        pulisciBuffer();

        int start = 0;
        if (consentiNegativo && buffer[0] == '-')
            start = 1;

        if (buffer[start] == '\0')
            valido = false;

        for (int i = start; buffer[i] != '\0'; i++) {
            if (buffer[i] < '0' || buffer[i] > '9') {
                valido = false;
                break;
            }
        }
        if (!valido) printf(ANSI_COLOR_RED" [X] Errore: Ammessi SOLO numeri INTERI. Riprova: "ANSI_COLOR_RESET);
    }
    return atoi(buffer);
}

double leggiDouble(bool consentiNegativo) {
    char buffer[50];
    bool valido = false;

    while (!valido) {
        valido = true;
        scanf("%49s", buffer);
        pulisciBuffer();

        int start = 0;
        if (consentiNegativo && buffer[0] == '-') start = 1;
        if (buffer[start] == '\0') valido = false;

        int dotCount = 0;
        int decimalPlaces = 0;
        bool countingDecimals = false;
        int digitCount = 0;

        for (int i = start; buffer[i] != '\0'; i++) {
            if (buffer[i] == ',') buffer[i] = '.';

            if (buffer[i] == '.') {
                dotCount++;
                countingDecimals = true;
                if (dotCount > 1)
                    { valido = false;
                       break;
                }
            }
            else if (buffer[i] >= '0' && buffer[i] <= '9') {
                digitCount++;
                if (countingDecimals)
                    decimalPlaces++;
            } else {
                valido = false;
                break;
            }
        }

        if (digitCount == 0) valido = false;
        if (valido && decimalPlaces > 2) {
            printf(ANSI_COLOR_RED" [X] Errore: Ammesse al massimo 2 cifre decimali.\n"ANSI_COLOR_RESET);
            valido = false;
        } else if (!valido) {
            printf(ANSI_COLOR_RED" [X] Errore: Formato non valido (es. 10.50).\n"ANSI_COLOR_RESET);
        }

        if (!valido) printf(" Riprova: ");
    }
    return atof(buffer);
}

bool validaIBAN(const char* iban) {
    if (strlen(iban) != 9) return false;
    if (iban[0] != 'I' || iban[1] != 'T' || iban[2] != '-') return false;
    for (int i = 3; i < 9; i++) {
        bool isDigit = (iban[i] >= '0' && iban[i] <= '9');
        bool isUpperHex = (iban[i] >= 'A' && iban[i] <= 'F');
        if (!isDigit && !isUpperHex) return false;
    }
    return true;
}

void generaIBAN(char* buffer) {
    const char charset[] = "0123456789ABCDEF";
    strcpy(buffer, "IT-");
    for (int i = 0; i < 6; i++) buffer[3 + i] = charset[rand() % 16];
    buffer[9] = '\0';
}

void impostaDataSimulata() {
    stampaIntestazione("IMPOSTAZIONE DATA SIMULATA DI SISTEMA");
    printf(" Data attuale di simulazione: %02d/%02d/%04d\n\n", dataSimulata.giorno, dataSimulata.mese, dataSimulata.anno);

    Data nuovaData;
    bool dataValida = false;

    while (!dataValida) {
        printf(" Inserisci nuovo Giorno (1-31): "); nuovaData.giorno = leggiIntero(false);
        printf(" Inserisci nuovo Mese (1-12)  : "); nuovaData.mese = leggiIntero(false);
        printf(" Inserisci nuovo Anno         : "); nuovaData.anno = leggiIntero(false);

        long oldDataGrezza = dataSimulata.anno * 10000 + dataSimulata.mese * 100 + dataSimulata.giorno;
        long newDataGrezza = nuovaData.anno * 10000 + nuovaData.mese * 100 + nuovaData.giorno;

        if (newDataGrezza < oldDataGrezza) {
            printf(ANSI_COLOR_RED"\n [X] ERRORE: Violazione temporale! Non puoi tornare indietro nel tempo.\n"ANSI_COLOR_RESET);
            printf(ANSI_COLOR_YELLOW"     La data deve essere >= %02d/%02d/%04d.\n\n"ANSI_COLOR_RESET, dataSimulata.giorno, dataSimulata.mese, dataSimulata.anno);
        } else if (nuovaData.giorno < 1 || nuovaData.giorno > 31 || nuovaData.mese < 1 || nuovaData.mese > 12) {
            printf(ANSI_COLOR_RED"\n [X] ERRORE: Formato del calendario non valido.\n\n"ANSI_COLOR_RESET);
        } else {
            dataValida = true;
        }
    }

    dataSimulata = nuovaData;
    printf(ANSI_COLOR_GREEN"\n [V] Il sistema e' avanzato temporalmente al: %02d/%02d/%04d\n"ANSI_COLOR_RESET, dataSimulata.giorno, dataSimulata.mese, dataSimulata.anno);
    attendiInvio();
}

void sottomenuProgrammati(Utente* u) {
    int scelta = -1;
    while (scelta != 0) {
        stampaIntestazione("GESTIONE ENTRATE / USCITE PROGRAMMATE");
        printf(" Lista Movimenti di %s:\n", u->username);
        stampaMovimentiProgrammati(u);
        printf(ANSI_COLOR_CYAN"+=================================================+\n");
        printf("| 1. Aggiungi nuovo Introito / Uscita             |\n");
        printf("| 2. Elimina un Movimento esistente               |\n");
        printf("| 0. Torna al menu principale                     |\n");
        printf("+=================================================+\n"ANSI_COLOR_RESET);
        printf(" Scelta: ");
        scelta = leggiIntero(false);

        if (scelta == 1) {
            char desc[50];
            double imp; int rip; Data inizioMov; int intervallo;

            printf("\n [+] Nome operazione (es. Spesa Coop): ");
            leggiStringa(desc, 50);

            printf(" [+] Importo (+ Introito, - Uscita) [Accetta centesimi]: "); imp = leggiDouble(true);

            // --- Acquisizione nuova periodicità ---
            printf(" [+] Frequenza (es. 1 = mensile, 3 = trimestrale, 12 = annuale): ");
            bool intValido = false;
            while (!intValido) {
                intervallo = leggiIntero(false);
                if (intervallo < 1) {
                    printf(ANSI_COLOR_RED" [X] L'intervallo deve essere almeno 1. Riprova: "ANSI_COLOR_RESET);
                } else {
                    intValido = true;
                }
            }

            bool dataValida = false;
            while (!dataValida) {
                printf(" [+] Giorno di esecuzione nel mese (1-31): "); rip = leggiIntero(false);
                printf(" [+] A partire da quale Mese (1-12): "); inizioMov.mese = leggiIntero(false);
                printf(" [+] A partire da quale Anno (es. 2026): "); inizioMov.anno = leggiIntero(false);
                inizioMov.giorno = rip;

                long dataInizioGrezza = inizioMov.anno * 10000 + inizioMov.mese * 100 + inizioMov.giorno;
                long dataAttualeGrezza = dataSimulata.anno * 10000 + dataSimulata.mese * 100 + dataSimulata.giorno;

                if (dataInizioGrezza < dataAttualeGrezza) {
                    printf(ANSI_COLOR_RED"\n [X] ERRORE: Non puoi programmare un movimento nel passato!\n"ANSI_COLOR_RESET);
                    printf(ANSI_COLOR_YELLOW"     La data deve essere >= al %02d/%02d/%04d (Data di Simulazione).\n\n"ANSI_COLOR_RESET,
                           dataSimulata.giorno, dataSimulata.mese, dataSimulata.anno);
                } else if (rip < 1 || rip > 31 || inizioMov.mese < 1 || inizioMov.mese > 12) {
                    printf(ANSI_COLOR_RED"\n [X] ERRORE: Formato del calendario non valido.\n\n"ANSI_COLOR_RESET);
                } else {
                    dataValida = true;
                }
            }

            // Aggiunto "intervallo" alla chiamata
            aggiungiMovimentoProgrammato(u, inizioMov, desc, imp, rip, intervallo);
            printf(ANSI_COLOR_GREEN"\n [V] Movimento '%s' programmato con successo dal %02d/%02d/%04d.\n"ANSI_COLOR_RESET,
                   desc, inizioMov.giorno, inizioMov.mese, inizioMov.anno);
            attendiInvio();
        } else if (scelta == 2) {
            char descr[50];
            printf("\n [-] Inserisci il nome ESATTO del movimento da eliminare: ");
            leggiStringa(descr, 50);
            eliminaMovimentoProgrammato(u, descr);
            attendiInvio();
        }
    }
}

void menuUtenteInRosso(Utente* u) {
    int scelta = -1;
    while (scelta != 0) {
        stampaIntestazione(ANSI_COLOR_RED "!!! ATTENZIONE - CONTO IN ROSSO !!!" ANSI_COLOR_RESET);
        printf(" Causa debito, le normali operazioni sono bloccate.\n");
        printf(" Saldo Attuale: " ANSI_COLOR_RED "%.2lf EUR\n" ANSI_COLOR_RESET, u->saldo);
        printf(ANSI_COLOR_CYAN"+=================================================+\n");
        printf("| 1. Effettua Ricarica (Ripiana Debito)           |\n");
        printf("| 2. Gestisci Entrate/Uscite Programmate          |\n");
        printf("| 0. Logout                                       |\n");
        printf("+=================================================+\n"ANSI_COLOR_RESET);
        printf(" Scelta: ");
        scelta = leggiIntero(false);

        if (scelta == 1) {
            double importo;
            printf("\n Importo ricarica [SOLO INTERI]: "); importo = (double)leggiIntero(false);
            effettuaRicarica(u, importo, dataSimulata);
            if (u->saldo >= 0) {
                printf(ANSI_COLOR_GREEN" [V] Debito saldato! Ritorno al menu standard...\n"ANSI_COLOR_RESET);
                attendiInvio();
                return;
            }
            else
            {
                attendiInvio();
                }
        }
        else if (scelta == 2) {
            sottomenuProgrammati(u);
        }
    }
}

void menuUtente(Utente* u, ListaUtenti* db) {
    int scelta = -1;
    while (scelta != 0) {
        if (u->saldo < 0) {
            menuUtenteInRosso(u);
            if (u->saldo < 0) return;
        }

        char titolo[100];
        sprintf(titolo, "BENTORNATO %s", u->username);
        stampaIntestazione(titolo);
        printf(" IBAN: %-25s Saldo: %.2lf EUR\n", u->iban, u->saldo);
        printf(ANSI_COLOR_CYAN"+=================================================+\n");
        printf(ANSI_COLOR_RESET"| 1. Operazioni (Ricarica/Prelievo/P2P/Bonifico)  |\n");
        printf("| 2. Gestione Entrate/Uscite Programmate          |\n");
        printf("| 3. Storico (Ordinamento e Filtro)               |\n");
        printf("| 4. Previsione Futura (Forecasting Andamento)    |\n");
        printf("| 5. Leggi Notifiche (Pop dalla Pila LIFO)        |\n");
        printf("| 0. Logout                                       |\n");
        printf("+=================================================+\n");
        printf(" Scelta: ");
        scelta = leggiIntero(false);

        if (scelta == 1) {
            stampaIntestazione(ANSI_COLOR_CYAN"OPERAZIONI BANCARIE");
            printf(ANSI_COLOR_RESET" 1. Ricarica\n 2. Prelievo\n 3. P2P (tramite Username)\n 4. Bonifico (tramite IBAN)\n");
            printf("\n Seleziona Operazione: ");
            int op = leggiIntero(false);

            double importo; char stringa[50];

            if (op == 1 || op == 2) {
                printf("\n Importo [SOLO INTERI]: ");
                importo = (double)leggiIntero(false);
                if (op == 1) effettuaRicarica(u, importo, dataSimulata);
                else effettuaPrelievo(u, importo, dataSimulata);
            }
            else if (op == 3) {
                bool p2pRisolto = false;
                while (!p2pRisolto) {
                    printf("\n Identificativo (Username Beneficiario): ");
                    leggiStringa(stringa, 50);
                    Utente* beneficiario = cercaUtentePerUsername(*db, stringa);

                    if (beneficiario != NULL) {
                        printf(" Importo da trasferire [Accetta centesimi]: ");
                        importo = leggiDouble(false);
                        effettuaP2P(u, *db, stringa, importo, dataSimulata);
                        p2pRisolto = true;
                    } else {
                        printf(ANSI_COLOR_YELLOW"\n [X] NOTIFICA: L'utente '%s' NON e' registrato nel sistema.\n"ANSI_COLOR_RESET, stringa);
                        printf(" -------------------------------------------------\n");
                        printf(" 1. Inserisci un nuovo Username (Riprova)\n");
                        printf(" 2. Passa al Bonifico Esterno (tramite IBAN)\n");
                        printf(" 0. Annulla Operazione\n");
                        printf(" Scelta: ");
                        int fallback = leggiIntero(false);

                        if (fallback == 1) continue;
                        else if (fallback == 2) { op = 4; break; }
                        else { printf(ANSI_COLOR_RED"\n Operazione annullata.\n"ANSI_COLOR_RESET); p2pRisolto = true; }
                    }
                }
            }

            if (op == 4) {
                bool ibanValido = false;
                while (!ibanValido) {
                    printf("\n Identificativo (IBAN Beneficiario): ");
                    leggiStringa(stringa, 50);

                    if (!validaIBAN(stringa)) {
                        printf(ANSI_COLOR_YELLOW"\n [X] NOTIFICA: L'IBAN '%s' non rispetta il formato (es. IT-0A1B2C).\n"ANSI_COLOR_RESET, stringa);
                        printf(" -------------------------------------------------\n");
                        printf(" 1. Inserisci un nuovo IBAN (Riprova)\n");
                        printf(ANSI_COLOR_RED" 0. Annulla Operazione\n"ANSI_COLOR_RESET);
                        printf(" Scelta: ");
                        int fallback = leggiIntero(false);

                        if (fallback == 1) continue;
                        else { printf(ANSI_COLOR_RED"\n Operazione annullata.\n"ANSI_COLOR_RESET); ibanValido = true; op = 0; }
                    } else {
                        ibanValido = true;
                    }
                }

                if(op != 0) {
                    printf(" Importo da trasferire [Accetta centesimi]: ");
                    importo = leggiDouble(false);
                    effettuaBonifico(u, *db, stringa, importo, dataSimulata, &codaCentrale);
                }
            }
            attendiInvio();
        }
        else if (scelta == 2) {
            sottomenuProgrammati(u);
        }
        else if (scelta == 3) {
            stampaIntestazione("STORICO OPERAZIONI");
            printf(" 1. Ordina Meno Recente\n 2. Ordina Più Recente\n 3. Filtra per Mese\n");
            printf("\n Scelta: "); int sub = leggiIntero(false);
            printf("\n");

            if (sub == 1 || sub == 2) {
                printf(" Quanti movimenti vuoi visualizzare? (0 per vederli tutti): ");
                int limite = leggiIntero(false);
                printf("\n");

                ordinaStorico(u, sub);
                Transazione* t = u->storico;
                int conteggio = 0;
                if (t == NULL) printf(ANSI_COLOR_RED" [!] Nessuna operazione in archivio.\n"ANSI_COLOR_RESET);
                    while(t != NULL && (limite == 0 || conteggio < limite)) {
                    bool isUscita = (strstr(t->tipo, "Prelievo") || strstr(t->tipo, "Inviato") || strstr(t->tipo, "Esterno"));
                    const char* cColor = isUscita ? ANSI_COLOR_RED : ANSI_COLOR_GREEN;
                    char segno = isUscita ? '-' : '+';

                    printf(" [%02d/%02d/%04d %02d:%02d:%02d] %-15s | %s%c%8.2lf EUR%s\n",
                        t->dataOraOperazione.giorno, t->dataOraOperazione.mese, t->dataOraOperazione.anno,
                        t->dataOraOperazione.ora, t->dataOraOperazione.minuto, t->dataOraOperazione.secondo,
                        t->tipo, cColor, segno, t->importo, ANSI_COLOR_RESET);
                    t = t->next;
                    conteggio++;
                }
            } else if (sub == 3) {
                printf(" Mese: ");
                int m = leggiIntero(false);
                printf(" Anno: "); int y = leggiIntero(false);
                printf(" Quanti movimenti vuoi visualizzare? (0 per vederli tutti): ");
                int limite = leggiIntero(false);
                filtraStoricoPerMese(u, m, y, limite);
            }
            attendiInvio();
        }
        else if (scelta == 4) {
            stampaIntestazione(ANSI_COLOR_CYAN"MOTORE DI PREVISIONE (FORECASTING)");
            printf(ANSI_COLOR_RESET" Quanti mesi nel futuro vuoi simulare?: "); int mesi = leggiIntero(false);
            generaPrevisione(u, dataSimulata, mesi);
            attendiInvio();
        }
        else if (scelta == 5) {
    stampaIntestazione("NOTIFICHE DI SISTEMA");
    popNotifica(&(u->notifiche));
    attendiInvio();
}

        salvaDatabase(*db, FILE_DB);
    }
}

void sottomenuAdminUtente(Utente* u) {
    int scelta = -1;
    while (scelta != 0) {
        char titolo[100];
        sprintf(titolo, ANSI_COLOR_CYAN "GESTIONE UTENTE: %s"ANSI_COLOR_RESET, u->username);
        stampaIntestazione(titolo);
        printf("| 1. Modifica Username                            |\n");
        printf("| 2. Modifica Password                            |\n");
        printf("| 3. Modifica Saldo d'Ufficio                     |\n");
        printf("| 4. Visione Previsione Mesi                      |\n");
        printf("| 0. Indietro                                     |\n");
        printf(ANSI_COLOR_CYAN"+=================================================+\n"ANSI_COLOR_RESET);
        printf(" Scelta: ");
        scelta = leggiIntero(false);

        if (scelta == 1) { printf(" Nuovo Username: "); leggiStringa(u->username, 50); attendiInvio(); }
        else if (scelta == 2) { printf(" Nuova Password: "); leggiStringa(u->password, 50); attendiInvio(); }
        else if (scelta == 3) { printf(" Nuovo Saldo: "); u->saldo = leggiDouble(true); attendiInvio(); }
        else if (scelta == 4) { generaPrevisione(u, dataSimulata, 6); attendiInvio(); }
    }
}
void menuAdmin(ListaUtenti* db) {
    int scelta = -1;
    while (scelta != 0) {
        stampaIntestazione(ANSI_COLOR_CYAN  "PANNELLO DI CONTROLLO AMMINISTRATORE");
        printf("| 1. Registra Nuovo Utente                        |\n");
        printf("| 2. Visualizza Utenti (Ordinati per Saldo)       |\n");
        printf("| 3. Gestisci Singolo Utente                      |\n");
        printf("| 4. Elimina Utente dal Sistema                   |\n");
        printf("| 5. Elabora Coda Bonifici Pendenti (FIFO)        |\n");
        printf("| 6. Genera Dump Sicurezza (Binario)              |\n");
        printf("| 7. Accesso Diretto Backup Binario (fseek)       |\n");
        printf("| 0. Logout                                       |\n");
        printf("+=================================================+\n" ANSI_COLOR_RESET);
        printf(" Scelta: ");
        scelta = leggiIntero(false);

        if (scelta == 1) {
            char u[50], p[50], i[15];
            bool usernameValido = false;

            while (!usernameValido) {
                printf("\n Username: ");
                leggiStringa(u, 50);

                if (cercaUtentePerUsername(*db, u) != NULL) {
                    printf(ANSI_COLOR_RED "[X] ERRORE: L'username '%s' e' gia' in uso nel sistema.\n"ANSI_COLOR_RESET, u);
                    printf(ANSI_COLOR_YELLOW"     Scegli un identificativo differente.\n"ANSI_COLOR_RESET);
                } else {
                    usernameValido = true;
                }
            }

            printf(" Password: ");
            leggiStringa(p, 50);

            bool ibanUnico = false;
            while (!ibanUnico) {
                generaIBAN(i);
                if (cercaUtentePerIBAN(*db, i) == NULL) {
                    ibanUnico = true;
                }
            }

            inserisciUtente(db, creaUtente(u, p, i));
            salvaDatabase(*db, FILE_DB);
            printf(ANSI_COLOR_GREEN" [V] Utente registrato con successo! IBAN Assegnato: %s\n"ANSI_COLOR_RESET, i);
            attendiInvio();

        } else if (scelta == 2) {
            printf("\n 1. Saldo Alto -> Basso \n 2. Saldo Basso -> Alto \n Scelta: ");
            int cr = leggiIntero(false);
            ordinaUtenti(db, cr);
            Utente* curr = *db;
            printf("\n");
            while(curr != NULL) {
                printf(" %-15s | IBAN: %-15s | %9.2lf EUR\n", curr->username, curr->iban, curr->saldo);
                curr = curr->next;
            }
            attendiInvio();

        } else if (scelta == 3) {
            char search[50];
            printf("\n Inserisci Username da gestire: "); leggiStringa(search, 50);
            Utente* target = cercaUtentePerUsername(*db, search);
            if (target) sottomenuAdminUtente(target);
            else { printf(ANSI_COLOR_RED" [X] Utente non trovato.\n"ANSI_COLOR_RESET); attendiInvio(); }
            salvaDatabase(*db, FILE_DB);

        } else if (scelta == 4) {
            char targetDel[50];
            printf("\n Inserisci Username da ELIMINARE definitivamente: ");
            leggiStringa(targetDel, 50);

            if (strcmp(targetDel, "admin") == 0) {
                printf(ANSI_COLOR_RED" [X] Operazione negata: impossibile eliminare l'amministratore di sistema.\n"ANSI_COLOR_RESET);
            } else {
                printf(ANSI_COLOR_YELLOW" ATTENZIONE: Sei sicuro di voler eliminare '%s'? (1=SI, 0=NO): "ANSI_COLOR_RESET, targetDel);
                int conferma = leggiIntero(false);

                if (conferma == 1) {
                    if (eliminaUtente(db, targetDel)) {
                        salvaDatabase(*db, FILE_DB);
                        printf(" [V] Utente '%s' e tutto il suo storico sono stati radiati dal sistema.\n", targetDel);
                    } else {
                        printf(ANSI_COLOR_RED" [X] ERRORE: Utente '%s' non trovato nel sistema.\n"ANSI_COLOR_RESET, targetDel);
                    }
                } else {
                    printf(ANSI_COLOR_YELLOW" [!] Operazione di eliminazione annullata.\n"ANSI_COLOR_RESET);
                }
            }
            attendiInvio();
        }
        else if (scelta == 5) {
    elaboraCodaBonifici(*db, dataSimulata, &codaCentrale);
    salvaDatabase(*db, FILE_DB);
    attendiInvio();
}
else if (scelta == 6) {
    salvaBackupBinario(*db, "backup_sicurezza.dat");
    attendiInvio();
}
else if (scelta == 7) {
    printf("\n Inserisci l'indice del record da leggere (es. 0 per il primo): ");
    int id = leggiIntero(false);
    esploraBackupBinarioConFseek("backup_sicurezza.dat", id);
    attendiInvio();
}
    }
}

// --- ENTRY POINT ---
int main() {
    srand((unsigned)time(NULL));
    ListaUtenti db = caricaDatabase(FILE_DB);

    dataSimulata.giorno = 4;
    dataSimulata.mese = 1; dataSimulata.anno = 2026;
    inizializzaCoda(&codaCentrale);
    int cmd = -1;
    while (cmd != 0) {
        stampaIntestazione("SISTEMA BANCARIO AVANZATO - SCHERMATA LOGIN");
        printf(ANSI_COLOR_GREEN" Data di Riferimento Attuale: %02d/%02d/%04d\n" ANSI_COLOR_RESET, dataSimulata.giorno, dataSimulata.mese, dataSimulata.anno);
        printf("+=================================================+\n");
        printf("| 1. Effettua Login                               |\n");
        printf("| 2. Avanza la Data Simulata                      |\n");
        printf("| 0. Esci dal sistema                             |\n");
        printf("+=================================================+\n");
        printf(" Scelta: ");
        cmd = leggiIntero(false);

        if (cmd == 1) {
            char usr[50], pwd[50];

            printf("\n Username: ");
            leggiStringa(usr, 50);

            printf(" Password: ");
            leggiStringa(pwd, 50);

            if (strcmp(usr, "admin") == 0 && strcmp(pwd, "adminpass") == 0) {
                menuAdmin(&db);
            } else {
                Utente* found = cercaUtentePerUsername(db, usr);
                if (found && strcmp(found->password, pwd) == 0) {
                    menuUtente(found, &db);
                } else {
                    printf(ANSI_COLOR_RED" [X] Credenziali errate o utente inesistente.\n"ANSI_COLOR_RESET);
                    attendiInvio();
                }
            }
        } else if (cmd == 2) {
            impostaDataSimulata();
        }
    }

    salvaDatabase(db, FILE_DB);
    printf(ANSI_COLOR_RED"\nSpegnimento del sistema in corso...\n"ANSI_COLOR_RESET);

    // Sottocaso : Prevenzione del memory leak prima del rientro al sistema operativo
    liberaDatabase(db);
    return 0;
}
