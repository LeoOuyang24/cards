#ifndef ENEMYCARDS_H_INCLUDED
#define ENEMYCARDS_H_INCLUDED

#include "deck.h"

//for cards you'll encounter in the deck

typedef std::vector<CardPtr> CardRewards;

/*template<typename It>
std::vector<Card const*> minCardsNeeded(const It& start, const It& end, ResourceStats& stats, ResourceStats& target)
{
    if (Card const* card = start->lock().get())
    {
        if (card->getStats() > ResourceStats())
        {

        }
    }
    return minCardsNeeded(++start,end,stats,target);
}


//pass in start and end iterators
template<typename It>
std::vector<Card const*> minCardsNeeded(const It& start, const It& end, ResourceStats& target)
{
    ResourceStats stats;
}
*/



//represents a choice posed by a card
struct Choice
{
    virtual std::string getMessage() = 0;
    virtual bool isValid() = 0; //return true if this choice can be selected
    virtual void choose() = 0; //what to do if chosen
};

class TextChoice : public Choice
{
    std::string message = "";
public:
    TextChoice(std::string message);
    bool isValid() //temporary, will probably need to change this run a lambda or something later
    {
        return true;
    }
    void choose() //temporary
    {
        return;
    }
    std::string getMessage();
};

class Trade : public Choice
{
    ResourceStats give; //what to give for this trade
    CardRewards get; //what you (the player) receives for this trade
public:
    Trade(const ResourceStats& give, const CardRewards& get);
    bool isValid();
    void choose();
    ResourceStats getOffer();
    CardRewards&& getRewards(); //return r value so you can move it
    std::string getMessage();
};

//literally only shared_ptr because unique_ptrs can't be copied and are annoying to work with since push_back copies
typedef std::vector<std::shared_ptr<Choice>> Choices;
//deck cards
class EnemyCard : public Card
{
    Choices choices;
public:
    static constexpr int maxChoices = 4;
    EnemyCard(std::string name, std::string spritePath, std::initializer_list<Choice*> choices_);
    EnemyCard(std::string name, std::string spritePath, std::initializer_list<std::string> choices_);
    const Choices& getChoices() const;
};

typedef std::shared_ptr<EnemyCard> EnemyPtr;
//top most card is in FRONT, bottom most card is in the back
typedef std::list<EnemyPtr> DeckType;
class Deck
{
    DeckType deck;
public:
    Deck();
    //return top most card, null if empty
    EnemyPtr getTop();
    void pop();
    size_t size();
};

#endif // ENEMYCARDS_H_INCLUDED
