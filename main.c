#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "strutture.h"
#include "io_file.h"
#include "funzioni_di_supporto.h"
#include "interfacce.h"

#define FILE_DB "database_banca.txt"

Data dataSimulata;
CodaBonifici codaCentrale;

int main() {
    srand((unsigned)time(NULL));
    
    // Inizializzazione dello stato di partenza
    ListaUtenti db = caricaDatabase(FILE_DB);
    eseguiControlloIntegrita(&db, "backup_sicurezza.dat");
    
    dataSimulata.giorno = 26; dataSimulata.mese = 6; dataSimulata.anno = 2026;
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
            char usr[50], pwdRaw[50], pwdHash[50];
            
            printf("\n Username: "); leggiStringa(usr, 50);
            printf(" Password: "); leggiStringa(pwdRaw, 50); // <-- Sostituito leggiPassword con leggiStringa

            // Controlliamo prima se è l'admin 
            if (strcmp(usr, "admin") == 0 && strcmp(pwdRaw, "adminpass") == 0) {
                menuAdmin(&db, &dataSimulata, &codaCentrale);
            } 
            else 
            {
                Utente* found = cercaUtentePerUsername(db, usr);
                
                // Generazione Hash e match con il database
                generaHash(pwdRaw, pwdHash);
                
                if (found && strcmp(found->password, pwdHash) == 0) {
                    menuUtente(found, &db, &dataSimulata, &codaCentrale);
                } 
                else 
                {
                    printf(ANSI_COLOR_RED" [X] Credenziali errate o utente inesistente.\n"ANSI_COLOR_RESET);
                    attendiInvio();
                }
            }
        }
        else if (cmd == 2) {
            impostaDataSimulata(&dataSimulata);
        }     
        salvaDatabase(db, FILE_DB);
    }

    printf(ANSI_COLOR_RED"\nSpegnimento del sistema in corso...\n"ANSI_COLOR_RESET);
    liberaDatabase(db);
    return 0;
}
