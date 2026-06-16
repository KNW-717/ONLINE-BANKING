// Responsabilità: Calcolo matematico, alterazione saldi e gestione transazionale.

#ifndef MOTORE_BANCARIO_H_INCLUDED
#define MOTORE_BANCARIO_H_INCLUDED
#include "strutture.h"
#include <stdbool.h>

bool effettuaRicarica(Utente* u, double importo, Data dataSimulata);
bool effettuaPrelievo(Utente* u, double importo, Data dataSimulata);
bool effettuaP2P(Utente* mittente, ListaUtenti listaSistema, const char* usernameBeneficiario, double importo, Data dataSimulata);
bool effettuaBonifico(Utente* mittente, ListaUtenti listaSistema, const char* ibanBeneficiario, double importo, Data dataSimulata, CodaBonifici* coda);
void elaboraCodaBonifici(ListaUtenti listaSistema, Data dataSimulata, CodaBonifici* coda);

#endif // MOTORE_BANCARIO_H_INCLUDED
