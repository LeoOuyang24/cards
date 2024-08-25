#include <filesystem>

#include "../headers/deck.h"
#include "../headers/UI.h"

Card::Card(std::string spritePath)
{
    if (spritePath.size() == 0)
    {
        std::string folderPath = "./sprites/cards";

        //reservoir sample our files, picking a random sprite
        //https://stackoverflow.com/questions/58400066/how-to-quickly-pick-a-random-file-from-a-folder-tree
        int n = 1;
        for (const auto & entry : std::filesystem::directory_iterator(folderPath))
            if (rand()%n == 0)
            {
                spritePath = entry.path().string();
                n++;
            }
    }
    sprite.reset(new Sprite(spritePath));
}

Sprite* Card::getSprite()
{
    return sprite.get();
}

Deck::Deck()
{
    for (int i = 0; i < 52; i++)
    {
        deck.emplace_back(new Card());
    }
}

CardPtr Deck::getTop()
{
    if (deck.size() > 0)
    {
        return deck.back();
    }
    else
    {
        return CardPtr(nullptr);
    }
}

void Deck::pop()
{
    deck.pop_back();
}

HandType& Hand::getHand()
{
    return hand;
}

std::vector<CardPtr> Hand::draw(Deck& deck, int amount)
{
    std::vector<CardPtr> cards;
    for (int i = 0; i < amount; i++)
    {
        CardPtr ptr = deck.getTop();
        hand.push_back(ptr);
        cards.push_back(ptr);
        deck.pop();
    }
    return cards;

}

