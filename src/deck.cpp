#include <filesystem>

#include "../headers/deck.h"
#include "../headers/UI.h"
#include "../headers/card_text.h"
#include "../headers/loader.h"

const Resource Resource::coins = {0,"coins",CardTextFont::coinChar};
const Resource Resource::damage = {0,"damage",CardTextFont::damageChar};
const Resource Resource::food = {0,"food",CardTextFont::foodChar};

BaseCard::BaseCard(std::string name_,std::string spritePath) : name(name_)
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


Sprite* BaseCard::getSprite() const
{
    return sprite.get();
}

std::string BaseCard::getName() const
{
    return name;
}

void ResourceBody::renderCardText(const glm::vec4& pos, float angle, int z) const
{
    glm::vec2 center = {pos.x + pos.z/2, pos.y + pos.a/2};
    glm::vec4 textRect = rotateRect(CardRenderer::getCardTextRect(pos),center,angle); //rect where we render card text

    CardUI::cardTextFont->requestWrite({CardTextFont::getCardResourceString(stats,false),
                                textRect,
                                -1.f,{1,1,1,1},angle,z,CENTER,VERTCENTER},*CardUI::cardTextShader);
}

PlayerCard::PlayerCard(std::string name_, std::string spritePath, const ResourceStats& stats_) :  Card(name_,spritePath,*(new ResourceBody{stats_}))
{

}


ResourceStats PlayerCard::getStats() const
{
    return static_cast<ResourceBody*>(body.get())->stats;
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

