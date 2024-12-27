#ifndef LOADER_H_INCLUDED
#define LOADER_H_INCLUDED

#include <fstream>

//...I hardly know her!!

#include "deck.h"

struct CardInfo //bare info needed to load a card
{
    std::string name;
    std::string spritePath;
    ResourceStats stats;
};

BaseCard* loadCard(std::string json);
BaseCard* loadCard(const CardInfo& info);

std::vector<PlayerCard*> loadHand();


#endif // LOADER_H_INCLUDED
