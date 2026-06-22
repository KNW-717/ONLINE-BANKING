#ifndef CONSOLE_UTILS_H_INCLUDED
#define CONSOLE_UTILS_H_INCLUDED

#include <stdbool.h>
#include "strutture.h" // Per i tipi Data e Utente

void pulisciBuffer();
void pulisciSchermo();
void attendiInvio();
void stampaIntestazione(const char* titolo);
void leggiStringa(char* buffer, int size);
int leggiIntero(bool consentiNegativo);
double leggiDouble(bool consentiNegativo);
bool validaIBAN(const char* iban);
void generaIBAN(char* buffer);
void impostaDataSimulata(Data* dataAttuale);

#endif // CONSOLE_UTILS_H_INCLUDED
