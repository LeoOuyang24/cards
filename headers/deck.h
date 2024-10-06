#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED

#include <list>

#include "render.h"

//represents how much resource each card provides
struct ResourceStats
{
    int coins = 0;
    int damage = 0;
    int food = 0;

    std::string toString()
    {
        return "[Coins: " + std::to_string(coins) + ", Damage: " + std::to_string(damage) + ", Food: " + std::to_string(food) + "]";
    }
    ResourceStats operator+(const ResourceStats& b) const
    {
        return {coins + b.coins,damage + b.damage, food + b.food};
    }
    ResourceStats operator*(int num) const    {
        return {coins*num, damage*num, food*num};
    }
    ResourceStats operator-(const ResourceStats& b) const
    {
        return (*this ) + b*-1;
    }
    void operator += (const ResourceStats& a)
    {
        (*this) = (*this) + a;
    }
    void operator -= (const ResourceStats& a)
    {
        (*this) = (*this) - a;
    }
    bool operator == (const ResourceStats& a)
    {
        return coins == a.coins && damage == a.damage && food == a.food;
    }
    bool operator >= (const ResourceStats& a)
    {
        return (coins >= a.coins && damage >= a.damage && food >= a.food);
    }
    bool operator <= (const ResourceStats& a)
    {
        return  (coins <= a.coins && damage <= a.damage && food <= a.food);
    }

};

class Card
{
    std::shared_ptr<Sprite> sprite;
    std::string text = "";
    std::string name = "";

    ResourceStats stats;
public:
    //create the card
    //if no sprite is given, get a random one
    Card(std::string name, std::string spritePath = "", std::string text_ = "Deal \x80 Damage");
    Card(std::string name, std::string spritePath, const ResourceStats& stats);
    Sprite* getSprite() const;
    std::string getText() const;
    std::string getName() const;
    ResourceStats getStats() const;
};

//data structure that represents a deck
//top-most card is at the end of the vector
//this makes popping it a lot more efficient
typedef std::shared_ptr<Card> CardPtr;
typedef std::weak_ptr<Card> CardWeakPtr;


typedef std::list<CardWeakPtr> HandType;

/*class Hand
{
    HandType hand;
public:
    //return the card(s) that we drew
    Hand()
    {

    }

    //provide a starting hand. Make sure the starting hand container is very shortlived because we will be std::moving it
    Hand(const HandType& starting);
    std::vector<CardPtr> draw(Deck& deck, int amount = 1);
    HandType& getHand();
};*/

#endif // DECK_H_INCLUDED
