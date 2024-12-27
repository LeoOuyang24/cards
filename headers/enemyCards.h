#ifndef ENEMYCARDS_H_INCLUDED
#define ENEMYCARDS_H_INCLUDED

#include "deck.h"

//for cards you'll encounter in the deck

typedef std::vector<PlayerCardPtr> CardRewards;

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


enum ChoiceTypes
{
    TRADE, //trade
    ATTACK,
    HURT,
    TEXT
};


//represents a choice posed by a card
struct Choice
{
    //map of string to trade type
     static std::unordered_map<std::string, ChoiceTypes> stringToChoice;

    virtual std::string getMessage() = 0;
    virtual bool isValid() = 0; //return true if this choice can be selected
    virtual void choose() = 0; //what to do if chosen
    virtual bool isAttack() //if this choice is an attack. only true for Attack. Not a huge fan of this implementation but it works!
    {
        return false;
    }
    virtual void render(const glm::vec4& space);
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

//a choice that is usually free and requires the user take damage
class HurtChoice : public Choice
{
    int damage = 0;
public:
    HurtChoice(int damage);
    bool isValid() //always valid, player is always allowed to kill themselves
    {
        return true;
    }
    void choose();
    std::string getMessage();
};

//the option to starve every time the player doesn't have enough food
class StarveChoice : public HurtChoice
{
    static constexpr int STARVE_DAMAGE = 1;
public:
    StarveChoice();
    void choose();
};

typedef std::function<void()> Consequence; //a function that is run after a choice is made

struct ChoiceResult //represents a result of picking a choice
{
    std::string label = ""; //the text to render, describing the choice
    Consequence result; //what to run
};

class GenericChoice : public Choice
{
    ResourceStats give;
    ChoiceResult result;
public:
    GenericChoice(const ResourceStats& give, ChoiceResult result_);
    bool isValid();
    void choose(); //what to do when selected
    ResourceStats getOffer();
    std::string getMessage();
    void render(const glm::vec4& space);
};

class Trade : public Choice
{
    ResourceStats give; //what to give for this trade
    CardRewards get; //what you (the player) receives for this trade
public:
    Trade(const ResourceStats& give, const CardRewards& get);
    bool isValid();
    void choose(); //what to do when selected
    ResourceStats getOffer();
    CardRewards&& getRewards(); //return r value so you can move it
    std::string getMessage();
    void render(const glm::vec4& space);

};

class Attack : public Trade
{
    Consequence consequence;
public:
    Attack(int attack,CardRewards&& get, Consequence&& consequence);
    void choose();
    void inflictConsequence();
    virtual bool isAttack()
    {
        return true;
    }
};

//literally only shared_ptr because unique_ptrs can't be copied and are annoying to work with since push_back copies
typedef std::vector<std::shared_ptr<Choice>> Choices;

struct ChoicesBody : public CardBody
{
    Choices choices;
    void renderCardText(const glm::vec4& pos, float angle, int z) const;
};
//deck cards
 class EnemyCard : public Card
{
public:
    static constexpr int maxChoices = 3; //max number of choices including the attack
    template<typename It>
    EnemyCard(std::string name, std::string spritePath,  It first, It last) : Card(name,spritePath,*(new ChoicesBody())) //pass start and end iterators of a container containing Choice*
    {
        for (auto it = first; it != last; ++it)
        {
            static_cast<ChoicesBody*>(body.get())->choices.emplace_back(*it);
        }
    }
    EnemyCard(std::string name, std::string spritePath, std::initializer_list<Choice*> choices_);
    const Choices& getChoices() const;
};

typedef std::shared_ptr<EnemyCard> EnemyPtr;
//top most card is in FRONT, bottom most card is in the back
typedef std::list<EnemyPtr> DeckType;
class Deck
{
    DeckType deck;
    std::unique_ptr<Sprite> cardBack;
public:
    Deck(Sprite* cardBack);
    //return top most card, null if empty
    EnemyPtr getTop();
    Sprite const* getCardBack() const;
    void pop();
    size_t size() const;

    void addCard(const EnemyPtr& ptr); //add card to a random spot
};

#endif // ENEMYCARDS_H_INCLUDED
