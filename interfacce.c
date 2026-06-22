#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "interfacce.h"
#include "funzioni_di_supporto.h"
#include "io_file.h"
#include "motore_bancario.h"
#include "previsione.h"
#include "ordinamento.h"

// static perché non vogliamo venga esposta al main
// --- Sotto menù Programmati --> (Gestione Entrate/Uscite Movimenti Programmati) ---
static void sottomenuProgrammati(Utente* u, Data* dataAttuale) {
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

            printf("\n [+] Nome operazione (es. Spesa Coop): "); leggiStringa(desc, 50);
            printf(" [+] Importo (+ Introito, - Uscita) [Accetta centesimi]: "); imp = leggiDouble(true);

            printf(" [+] Frequenza (es. 1 = mensile, 3 = trimestrale, 12 = annuale): ");
            bool intValido = false;
            while (!intValido) {
                intervallo = leggiIntero(false);
                if (intervallo < 1) printf(ANSI_COLOR_RED" [X] L'intervallo deve essere almeno 1. Riprova: "ANSI_COLOR_RESET);
                else intValido = true;
            }

            bool dataValida = false;
            while (!dataValida) {
                printf(" [+] Giorno di esecuzione nel mese (1-31): "); rip = leggiIntero(false);
                printf(" [+] A partire da quale Mese (1-12): "); inizioMov.mese = leggiIntero(false);
                printf(" [+] A partire da quale Anno (es. 2026): "); inizioMov.anno = leggiIntero(false);
                inizioMov.giorno = rip;

                long dataInizioGrezza = inizioMov.anno * 10000 + inizioMov.mese * 100 + inizioMov.giorno;
                long dataAttualeGrezza = dataAttuale->anno * 10000 + dataAttuale->mese * 100 + dataAttuale->giorno;

                if (dataInizioGrezza < dataAttualeGrezza) {
                    printf(ANSI_COLOR_RED"\n [X] ERRORE: Non puoi programmare un movimento nel passato!\n"ANSI_COLOR_RESET);
                    printf(ANSI_COLOR_YELLOW"     La data deve essere >= %02d/%02d/%04d.\n\n"ANSI_COLOR_RESET, dataAttuale->giorno, dataAttuale->mese, dataAttuale->anno);
                } else if (rip < 1 || rip > 31 || inizioMov.mese < 1 || inizioMov.mese > 12) {
                    printf(ANSI_COLOR_RED"\n [X] ERRORE: Formato del calendario non valido.\n\n"ANSI_COLOR_RESET);
                } else dataValida = true;
            }
            aggiungiMovimentoProgrammato(u, inizioMov, desc, imp, rip, intervallo);
            printf(ANSI_COLOR_GREEN"\n [V] Movimento '%s' programmato con successo dal %02d/%02d/%04d.\n"ANSI_COLOR_RESET, desc, inizioMov.giorno, inizioMov.mese, inizioMov.anno);
            attendiInvio();
        } else if (scelta == 2) {
            char descr[50];
            printf("\n [-] Inserisci il nome ESATTO del movimento da eliminare: "); leggiStringa(descr, 50);
            eliminaMovimentoProgrammato(u, descr); attendiInvio();
        }
    }
}

// --- Menù Utente in Rosso -->(un menù specifico che blocca le operazioni di Prelievo,Bonifico e P2P per Utenti con il Conto in Rosso) ---
static void menuUtenteInRosso(Utente* u, Data* dataAttuale) {
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
            effettuaRicarica(u, importo, *dataAttuale);
            if (u->saldo >= 0) {
                printf(ANSI_COLOR_GREEN" [V] Debito saldato! Ritorno al menu standard...\n"ANSI_COLOR_RESET);
                attendiInvio(); return;
            } else attendiInvio();
        }
        else if (scelta == 2) sottomenuProgrammati(u, dataAttuale);
    }
}

// --- Menù Utente ---> Operazioni: Ricarica,Prelievo,P2P,Bonifico,Gestione Entrate/Uscite programmate,Storico,Previsione Futura, Leggi Notifiche,Logout) ---
void menuUtente(Utente* u, ListaUtenti* db, Data* dataAttuale, CodaBonifici* coda) {
    int scelta = -1;
    while (scelta != 0) {
        if (u->saldo < 0) {
            menuUtenteInRosso(u, dataAttuale);
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
            printf(ANSI_COLOR_RESET" 1. Ricarica\n 2. Prelievo\n 3. P2P (Username)\n 4. Bonifico (IBAN)\n\n Seleziona Operazione: ");
            int op = leggiIntero(false);
            double importo; 
            char stringa[50];

            if (op == 1 || op == 2) {
                printf("\n Importo [SOLO INTERI]: "); 
                importo = (double)leggiIntero(false);
                if (op == 1) 
                effettuaRicarica(u, importo, *dataAttuale);
                  else 
                    effettuaPrelievo(u, importo, *dataAttuale);
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
                        effettuaP2P(u, *db, stringa, importo, *dataAttuale); 
                        p2pRisolto = true;
                    } else {
                        printf(ANSI_COLOR_YELLOW"\n [X] NOTIFICA: L'utente '%s' NON e' registrato.\n"ANSI_COLOR_RESET, stringa);
                        printf(" -------------------------------------------------\n 1. Inserisci nuovo Username\n 2. Passa al Bonifico Esterno\n 0. Annulla\n Scelta: ");
                        int fallback = leggiIntero(false);
                        if (fallback == 1) continue;
                        else if (fallback == 2) { 
                          op = 4; 
                          break; 
                        }
                        else 
                        { 
                          printf(ANSI_COLOR_RED"\n Operazione annullata.\n"ANSI_COLOR_RESET); 
                          p2pRisolto = true; 
                        }
                    }
                }
            }
            if (op == 4) {
                bool ibanValido = false;
                while (!ibanValido) {
                    printf("\n Identificativo (IBAN Beneficiario): "); 
                    leggiStringa(stringa, 50);
                    if (!validaIBAN(stringa)) {
                        printf(ANSI_COLOR_YELLOW"\n [X] NOTIFICA: L'IBAN '%s' non rispetta il formato.\n"ANSI_COLOR_RESET, stringa);
                        printf(" -------------------------------------------------\n 1. Riprova\n"ANSI_COLOR_RED" 0. Annulla\n"ANSI_COLOR_RESET" Scelta: ");
                        int fallback = leggiIntero(false);
                        if (fallback == 1) continue;
                        else { printf(ANSI_COLOR_RED"\n Operazione annullata.\n"ANSI_COLOR_RESET); 
                              ibanValido = true; 
                              op = 0; }
                    } 
                    else ibanValido = true;
                }
                if(op != 0) {
                    printf(" Importo da trasferire [Accetta centesimi]: "); 
                    importo = leggiDouble(false);
                    effettuaBonifico(u, *db, stringa, importo, *dataAttuale, coda);
                }
            }
            attendiInvio();
        }
        else if (scelta == 2) 
          sottomenuProgrammati(u, dataAttuale);
        else if (scelta == 3) {
            stampaIntestazione("STORICO OPERAZIONI");
            printf(" 1. Ordina Meno Recente\n 2. Ordina Più Recente\n 3. Filtra per Mese\n\n Scelta: ");
            int sub = leggiIntero(false);
            if (sub == 1 || sub == 2) {
                printf(" Quanti movimenti visualizzare? (0 per tutti): "); 
                int limite = leggiIntero(false);
                ordinaStorico(u, sub); 
                Transazione* t = u->storico; int conteggio = 0;
                if (t == NULL) printf(ANSI_COLOR_RED" [!] Nessuna operazione in archivio.\n"ANSI_COLOR_RESET);
                while(t != NULL && (limite == 0 || conteggio < limite)) {
                    bool isUscita = (strstr(t->tipo, "Prelievo") || strstr(t->tipo, "Inviato") || strstr(t->tipo, "Esterno"));
                    const char* cColor = isUscita ? ANSI_COLOR_RED : ANSI_COLOR_GREEN;
                    char segno = isUscita ? '-' : '+';
                    printf(" [%02d/%02d/%04d %02d:%02d:%02d] %-15s | %s%c%8.2lf EUR%s\n",
                        t->dataOraOperazione.giorno, t->dataOraOperazione.mese, t->dataOraOperazione.anno,
                        t->dataOraOperazione.ora, t->dataOraOperazione.minuto, t->dataOraOperazione.secondo,
                        t->tipo, cColor, segno, t->importo, ANSI_COLOR_RESET);
                    t = t->next; conteggio++;
                }
            } else if (sub == 3) {
                printf(" Mese: "); int m = leggiIntero(false);
                printf(" Anno: "); int y = leggiIntero(false);
                printf(" Quanti visualizzare? (0 per tutti): "); int limite = leggiIntero(false);
                filtraStoricoPerMese(u, m, y, limite);
            }
            attendiInvio();
        }
        else if (scelta == 4) {
            stampaIntestazione(ANSI_COLOR_CYAN"MOTORE DI PREVISIONE (FORECASTING)");
            printf(ANSI_COLOR_RESET" Quanti mesi nel futuro vuoi simulare?: "); int mesi = leggiIntero(false);
            generaPrevisione(u, *dataAttuale, mesi); attendiInvio();
        }
        else if (scelta == 5) {
            stampaIntestazione("NOTIFICHE DI SISTEMA");
            popNotifica(&(u->notifiche)); attendiInvio();
        }
    }
}

// --- Sotto menù Admin Utente --> (Modifica Username/Password,Saldo ; Visione Previsione Mesi, Indietro) ---
static void sottomenuAdminUtente(Utente* u, Data* dataAttuale) {
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
            else if (scelta == 2) {
            printf(" Nuova Password: ");
            char passRaw[50];
            leggiStringa(passRaw, 50);
            generaHash(passRaw, u->password);
            printf(ANSI_COLOR_GREEN" [V] Password modificata e cifrata con successo.\n"ANSI_COLOR_RESET);
            attendiInvio();
        }        else if (scelta == 3) { printf(" Nuovo Saldo: "); u->saldo = leggiDouble(true); attendiInvio(); }
        else if (scelta == 4) { generaPrevisione(u, *dataAttuale, 6); attendiInvio(); }
    }
}

// --- Menuù Admin --> (Registra Nuovo Utente, Visualizza Utenti Registrati, Gestione Utente, Eliminazione Utente dal sistema, Elaborazione dei BOnifici Puntiformi, Generazione e Consultazione del Dump di Sicurezza (file binario)) ---
void menuAdmin(ListaUtenti* db, Data* dataAttuale, CodaBonifici* coda) {
    int scelta = -1;
    while (scelta != 0) {
        stampaIntestazione(ANSI_COLOR_CYAN "PANNELLO DI CONTROLLO AMMINISTRATORE");
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
            char u[50], p[50], i[15]; bool usernameValido = false;
            while (!usernameValido) {
                printf("\n Username: "); leggiStringa(u, 50);
                if (cercaUtentePerUsername(*db, u) != NULL) {
                    printf(ANSI_COLOR_RED "[X] ERRORE: L'username '%s' e' gia' in uso nel sistema.\n"ANSI_COLOR_RESET, u);
                    printf(ANSI_COLOR_YELLOW"     Scegli un identificativo differente.\n"ANSI_COLOR_RESET);
                } else usernameValido = true;
            }
           printf(" Password: ");
            char passRaw[50];
            leggiStringa(passRaw, 50);

            char passHash[50];
            generaHash(passRaw, passHash);

            bool ibanUnico = false;
            while (!ibanUnico) { generaIBAN(i); if (cercaUtentePerIBAN(*db, i) == NULL) ibanUnico = true; }
            inserisciUtente(db, creaUtente(u, p, i));
            printf(ANSI_COLOR_GREEN" [V] Utente registrato con successo! IBAN Assegnato: %s\n"ANSI_COLOR_RESET, i);
            attendiInvio();
        } else if (scelta == 2) {
            printf("\n 1. Saldo Alto -> Basso \n 2. Saldo Basso -> Alto \n Scelta: ");
            ordinaUtenti(db, leggiIntero(false));
            Utente* curr = *db; printf("\n");
            while(curr != NULL) {
                printf(" %-15s | IBAN: %-15s | %9.2lf EUR\n", curr->username, curr->iban, curr->saldo);
                curr = curr->next;
            }
            attendiInvio();
        } else if (scelta == 3) {
            char search[50]; printf("\n Inserisci Username da gestire: "); leggiStringa(search, 50);
            Utente* target = cercaUtentePerUsername(*db, search);
            if (target) sottomenuAdminUtente(target, dataAttuale);
            else { printf(ANSI_COLOR_RED" [X] Utente non trovato.\n"ANSI_COLOR_RESET); attendiInvio(); }
        } else if (scelta == 4) {
            char targetDel[50]; printf("\n Inserisci Username da ELIMINARE: "); leggiStringa(targetDel, 50);
            if (strcmp(targetDel, "admin") == 0) {
                printf(ANSI_COLOR_RED" [X] Impossibile eliminare l'amministratore di sistema.\n"ANSI_COLOR_RESET);
            } else {
                printf(ANSI_COLOR_YELLOW" Sicuro di voler eliminare '%s'? (1=SI, 0=NO): "ANSI_COLOR_RESET, targetDel);
                if (leggiIntero(false) == 1) {
                    if (eliminaUtente(db, targetDel)) printf(" [V] Utente '%s' radiato dal sistema.\n", targetDel);
                    else printf(ANSI_COLOR_RED" [X] ERRORE: Utente non trovato.\n"ANSI_COLOR_RESET);
                } else printf(ANSI_COLOR_YELLOW" [!] Operazione annullata.\n"ANSI_COLOR_RESET);
            }
            attendiInvio();
        } else if (scelta == 5) {
            elaboraCodaBonifici(*db, *dataAttuale, coda); attendiInvio();
        } else if (scelta == 6) {
            salvaBackupBinario(*db, "backup_sicurezza.dat"); attendiInvio();
        } else if (scelta == 7) {
            printf("\n Inserisci l'indice del record (es. 0): ");
            esploraBackupBinarioConFseek("backup_sicurezza.dat", leggiIntero(false));
            attendiInvio();
        }
    }
}

    free(backup); attendiInvio();
}
