//Responsabilità: Persistenza del database da Memoria Volatile a Disco fisso.
#ifndef IO_FILE_H_INCLUDED
#define IO_FILE_H_INCLUDED
#include "strutture.h"

void salvaDatabase(ListaUtenti lista, const char* nomeFile);
ListaUtenti caricaDatabase(const char* nomeFile);

#endif // IO_FILE_H_INCLUDED
