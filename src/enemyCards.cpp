#include "vanilla.h"

#include "../headers/enemyCards.h"
#include "../headers/gamestate.h"
#include "../headers/UI.h"
#include "../headers/loader.h"



TextChoice::TextChoice(std::string message_) : Choice(), message(message_)
{

}

std::string TextChoice::getMessage()
{
    return message;
}

Trade::Trade(const ResourceStats& give_, const CardRewards& get_) : give(give_), get(get_)
{

}

bool Trade::isValid()
{
    return getOffer() <= (GameState::getGameState()->getTracker<BoardState>())->getResources();
}

void Trade::choose()
{
    Sequencer* sequence = new Sequencer();
    glm::vec4 enemyRect = GameUI::getEnemyRect();
    glm::vec4 cardRect = GameUI::getHandRect();
    float totalTime = 250;
    for (auto card : get)
    {
        MasterCardsUI::getUI()->addCardToHand(card.get(),{enemyRect});
    }
    SequenceManager::request(*sequence);
}

ResourceStats Trade::getOffer()
{
    return give;
}

CardRewards&& Trade::getRewards()
{
    return std::move(get);
}

std::string Trade::getMessage()
{
    return give.toString() + " --> ";
}

EnemyCard::EnemyCard(std::string name, std::string spritePath, std::initializer_list<Choice*> choices_) : Card(name,spritePath,"")
{
        for (auto choice : choices_)
        {
            choices.emplace_back(choice);
        }
}

EnemyCard::EnemyCard(std::string name, std::string spritePath, std::initializer_list<std::string> choices_) : Card(name,spritePath,"")
{
        for (auto str : choices_)
        {
            choices.emplace_back(new TextChoice(str));
        }
}

const Choices& EnemyCard::getChoices() const
{
    return choices;
}

Deck::Deck(Sprite* cardBack_) : cardBack(cardBack_)
{
    for (int i = 0; i < 52; i++)
    {
        CardPtr ptr(loadCard("card_jsons/stale_potatoes.json"));
        deck.emplace_back(new EnemyCard("Grandma","sprites/cardfaces/grandma.png",{(new Trade({0,0,2},{ptr}))}));
    }
}


EnemyPtr Deck::getTop()
{
    if (deck.size() > 0)
    {
        return deck.front();
    }
    else
    {
        return EnemyPtr(nullptr);
    }
}

Sprite const* Deck::getCardBack() const
{
    return cardBack.get();
}

void Deck::pop()
{
    if (deck.size() > 0)
        deck.pop_front();
}

size_t Deck::size() const
{
    return deck.size();
}
