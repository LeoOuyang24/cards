#include <filesystem>

#include "../headers/deck.h"
#include "../headers/UI.h"
#include "../headers/card_text.h"

Card::Card(std::string name_,std::string spritePath, std::string text_) : text(text_), name(name_)
{
    if (spritePath.size() == 0)
    {
        std::string folderPath = "./sprites/cardfaces";

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

Card::Card(std::string name_, std::string spritePath, const ResourceStats& stats_) :  Card(name_,spritePath,CardTextFont::getCardResourceString(stats_))
{
    stats = stats_;
}

Sprite* Card::getSprite() const
{
    return sprite.get();
}

std::string Card::getName() const
{
    return name;
}

std::string Card::getText() const
{
    return text;
}

ResourceStats Card::getStats() const
{
    return stats;
}




/*Hand::Hand(const HandType& startingHand) : hand(std::move(startingHand))
{

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

}*/

