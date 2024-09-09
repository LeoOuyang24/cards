#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED

#include <list>

#include "render.h"

class Card
{
    std::shared_ptr<Sprite> sprite;
    std::string text = "";
public:
    //create the card
    //if no sprite is given, get a random one
    Card(std::string spritePath = "", std::string text_ = "Deal \x80 Damage");
    Sprite* getSprite();
    std::string getText();
};

//data structure that represents a deck
//top-most card is at the end of the vector
//this makes popping it a lot more efficient
typedef std::shared_ptr<Card> CardPtr;
typedef std::vector<CardPtr> DeckType;
class Deck
{
    DeckType deck;
public:
    Deck();
    //return top most card, null if empty
    CardPtr getTop();
    void pop();
};

typedef std::list<CardPtr> HandType;
class Hand
{
    HandType hand;
public:
    //return the card(s) that we drew
    std::vector<CardPtr> draw(Deck& deck, int amount = 1);
    HandType& getHand();
};

#endif // DECK_H_INCLUDED
