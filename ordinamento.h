//Responsabilità: Algoritmi di Sorting logico (Bubble Sort ottimizzato) e filtraggio data.
#ifndef ORDINAMENTO_H_INCLUDED
#define ORDINAMENTO_H_INCLUDED
#include "strutture.h"

void ordinaStorico(Utente* u, int criterio);
void ordinaUtenti(ListaUtenti* lista, int criterio);
void filtraStoricoPerMese(Utente* u, int meseScelto, int annoScelto, int limite);

#endif // ORDINAMENTO_H_INCLUDED
