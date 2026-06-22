#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include "funzioni_di_supporto.h"

void pulisciBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pulisciSchermo() {
    int ret = system("clear || cls");
    (void)ret;
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

void leggiStringa(char* buffer, int size) {
    bool valido = false;
    while (!valido) {
        if(fgets(buffer, size, stdin) != NULL) {
            int len = strlen(buffer);
            if (len > 0 && buffer[len-1] == '\n') {
                buffer[len-1] = '\0';
                len--;
            }
            if (len > 0) {
                valido = true;
                for (int i = 0; i < len; i++) {
                    if (buffer[i] == ' ') buffer[i] = '_';
                }
            } else {
                printf(ANSI_COLOR_RED" [X] Errore: Inserimento vuoto non ammesso. Riprova: "ANSI_COLOR_RESET);
            }
        }
    }
}

int leggiIntero(bool consentiNegativo) {
    char buffer[50];
    bool valido = false;
    while (!valido) {
        valido = true;
        if(scanf("%49s", buffer) == 1) {
            pulisciBuffer();
            int start = 0;
            if (consentiNegativo && buffer[0] == '-') start = 1;
            if (buffer[start] == '\0') valido = false;

            for (int i = start; buffer[i] != '\0'; i++) {
                if (buffer[i] < '0' || buffer[i] > '9') {
                    valido = false; break;
                }
            }
            if (!valido) printf(ANSI_COLOR_RED" [X] Errore: Ammessi SOLO numeri INTERI. Riprova: "ANSI_COLOR_RESET);
        } else {
            valido = false; pulisciBuffer();
        }
    }
    return atoi(buffer);
}

double leggiDouble(bool consentiNegativo) {
    char buffer[50];
    bool valido = false;
    while (!valido) {
        valido = true;
        if(scanf("%49s", buffer) == 1) {
            pulisciBuffer();
            int start = 0;
            if (consentiNegativo && buffer[0] == '-') start = 1;
            if (buffer[start] == '\0') valido = false;

            int dotCount = 0, decimalPlaces = 0, digitCount = 0;
            bool countingDecimals = false;

            for (int i = start; buffer[i] != '\0'; i++) {
                if (buffer[i] == ',') buffer[i] = '.';
                if (buffer[i] == '.') {
                    dotCount++; countingDecimals = true;
                    if (dotCount > 1) { valido = false; break; }
                } else if (buffer[i] >= '0' && buffer[i] <= '9') {
                    digitCount++;
                    if (countingDecimals) decimalPlaces++;
                } else {
                    valido = false; break;
                }
            }

            if (digitCount == 0) valido = false;
            if (valido && decimalPlaces > 2) {
                printf(ANSI_COLOR_RED" [X] Errore: Ammesse max 2 cifre decimali.\n"ANSI_COLOR_RESET);
                valido = false;
            } else if (!valido) {
                printf(ANSI_COLOR_RED" [X] Errore: Formato non valido.\n"ANSI_COLOR_RESET);
            }
            if (!valido) printf(" Riprova: ");
        } else {
            valido = false; pulisciBuffer();
        }
    }
    return atof(buffer);
}

bool validaIBAN(const char* iban) {
    regex_t regex;
    int ret = regcomp(&regex, "^IT-[0-9A-F]{6}$", REG_EXTENDED);
    if (ret != 0) return false;
    ret = regexec(&regex, iban, 0, NULL, 0);
    regfree(&regex);
    return (ret == 0);
}

void generaIBAN(char* buffer) {
    const char charset[] = "0123456789ABCDEF";
    strcpy(buffer, "IT-");
    for (int i = 0; i < 6; i++) buffer[3 + i] = charset[rand() % 16];
    buffer[9] = '\0';
}

void impostaDataSimulata(Data* dataAttuale) {
    stampaIntestazione("IMPOSTAZIONE DATA SIMULATA DI SISTEMA");
    printf(" Data attuale di simulazione: %02d/%02d/%04d\n\n", dataAttuale->giorno, dataAttuale->mese, dataAttuale->anno);

    Data nuovaData;
    bool dataValida = false;

    while (!dataValida) {
        printf(" Inserisci nuovo Giorno (1-31): "); nuovaData.giorno = leggiIntero(false);
        printf(" Inserisci nuovo Mese (1-12)  : "); nuovaData.mese = leggiIntero(false);
        printf(" Inserisci nuovo Anno         : "); nuovaData.anno = leggiIntero(false);

        long oldDataGrezza = dataAttuale->anno * 10000 + dataAttuale->mese * 100 + dataAttuale->giorno;
        long newDataGrezza = nuovaData.anno * 10000 + nuovaData.mese * 100 + nuovaData.giorno;

        if (newDataGrezza < oldDataGrezza) {
            printf(ANSI_COLOR_RED"\n [X] ERRORE: Non puoi tornare indietro nel tempo.\n"ANSI_COLOR_RESET);
            printf(ANSI_COLOR_YELLOW"     La data deve essere >= %02d/%02d/%04d.\n\n"ANSI_COLOR_RESET, dataAttuale->giorno, dataAttuale->mese, dataAttuale->anno);
        } else if (nuovaData.giorno < 1 || nuovaData.giorno > 31 || nuovaData.mese < 1 || nuovaData.mese > 12) {
            printf(ANSI_COLOR_RED"\n [X] ERRORE: Formato del calendario non valido.\n\n"ANSI_COLOR_RESET);
        } else {
            dataValida = true;
        }
    }

    *dataAttuale = nuovaData;
    printf(ANSI_COLOR_GREEN"\n [V] Sistema avanzato al: %02d/%02d/%04d\n"ANSI_COLOR_RESET, dataAttuale->giorno, dataAttuale->mese, dataAttuale->anno);
    attendiInvio();
}
