#ifndef INTERFACCE_H_INCLUDED
#define INTERFACCE_H_INCLUDED

#include "strutture.h"

void menuUtente(Utente* u, ListaUtenti* db, Data* dataAttuale, CodaBonifici* coda);
void menuAdmin(ListaUtenti* db, Data* dataAttuale, CodaBonifici* coda);
void eseguiControlloIntegrita(ListaUtenti* db, const char* nomeFileBin);

#endif // INTERFACCE_H_INCLUDED
