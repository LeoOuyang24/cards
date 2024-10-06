#ifndef LOADER_H_INCLUDED
#define LOADER_H_INCLUDED

#include <fstream>

//...I hardly know her!!

#include "deck.h"

Card* loadCard(std::string json);

std::vector<Card*> loadHand();


#endif // LOADER_H_INCLUDED
