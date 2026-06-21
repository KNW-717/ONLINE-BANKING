//Responsabilità: Persistenza del database da Memoria Volatile a Disco fisso.
#ifndef IO_FILE_H_INCLUDED
#define IO_FILE_H_INCLUDED
#include "strutture.h"

void salvaDatabase(ListaUtenti lista, const char* nomeFile);
ListaUtenti caricaDatabase(const char* nomeFile);

void salvaBackupBinario(ListaUtenti lista, const char* nomeFileBin);
void esploraBackupBinarioConFseek(const char* nomeFileBin, int indiceBersaglio);
#endif // IO_FILE_H_INCLUDED
