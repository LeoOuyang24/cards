#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include "deck.h"
#include "player.h"
#include "enemyCards.h"


//a weak_ptr wrapper class that is hashable
//it basically stores and hangs on to the initial hash, never changing even if the weak ptr is invalidated
class HashCardPtr
{
    size_t brown; //the hash (it's a hashbrown)
    PlayerWeakPtr ptr;
public:
    //this article says that you shouldn't make these from weak pointers https://stackoverflow.com/a/70132547/6947131
    //I don't really get why not, but whatever
    HashCardPtr(const PlayerWeakPtr& ptr_) : brown(std::hash<PlayerCard*>{}(ptr_.lock().get())), ptr(ptr_)
    {

    }
    bool operator ==(const HashCardPtr& other) const
    {
        return getHash() == other.getHash();
    }
    size_t getHash() const
    {
        return brown;
    }
    PlayerCard* get()
    {
        return ptr.lock().get();
    }
};

struct HasherIHardlyKnowHer
{
    size_t operator()(const HashCardPtr& ptr) const
    {
        return ptr.getHash();
    }
};
typedef std::unordered_set<HashCardPtr,HasherIHardlyKnowHer> CardSet;

//tracks various cards
class StateTracker
{
public:
    virtual void addCard(const PlayerWeakPtr& card) = 0; //add a non-owning pointer to a card
    virtual void removeCard(const PlayerWeakPtr& card) = 0;
    virtual void clear() = 0; //remove all cards
};

//tracks cards in board
class BoardState : public StateTracker
{
    ResourceStats curResources;
    CardSet cards;
public:
    void addCard(const PlayerWeakPtr& card);
    void removeCard(const PlayerWeakPtr& card);
    void clear();
    ResourceStats getResources() const;
};

//represents hand state
class HandState : public StateTracker
{
    HandType hand;
public:
    void addCard(const PlayerWeakPtr& card);
    void removeCard(const PlayerWeakPtr& card);
    void clear();
    const HandType& getHand() const;
};

class EnemyState
{
    EnemyPtr restEvent; //the rest event scenario
    EnemyPtr enemy;
    Deck deck;
public:
    EnemyState(std::string cardBackSpritePath);
    //set enemy, taking ownership of it. Usually used for enemies that are not in the deck
    void setEnemy(EnemyCard* card);
    void setNextEnemy(); //replace the current enemy with the next card on the deck. Slightly more efficient than using setEnemy() by using moves
    void restTime(); //replace the current enemy with the rest time event
    const EnemyPtr& getEnemy() const;
    const Deck& getDeck() const;
    void addCardToDeck(const EnemyPtr& ptr);

};

enum CardSpots //places a card can be
{
    NOWHERE, //nowhere, just exists in GameState
    HAND, //in the hand,
    BOARD //on the board, ready to be traded
};


class GameState
{
    GameState() : enemy("sprites/card_back.png")//private for that singleton structure
    {
        states[HAND] = GlobalMount<HandState>::getSharedPtr();
        states[BOARD] = GlobalMount<BoardState>::getSharedPtr();
        //states[]
    }
    std::unordered_map<PlayerCard*,std::pair<PlayerCardPtr,CardSpots>> cards; //master list of player cards and their locations
    std::unordered_map<CardSpots,std::shared_ptr<StateTracker>> states;
    static std::unique_ptr<GameState> curState;

    EnemyState enemy;
    PlayerState player;
public:
    static GameState* getGameState();
    static void init();
    //add a card, potentially moving it from a previous location
    const PlayerCardPtr& addCard(PlayerCard* card, CardSpots);
    const PlayerCardPtr& getCard(PlayerCard* card);
    CardSpots getCardSpot(PlayerCard* card); //returns NOWHERE if card cannot be found
    EnemyState& getEnemyState();
    const PlayerState& getPlayerState() const;
    StateTracker const* getTracker(CardSpots spots);
    template<typename T>
    T const* getTracker()
    {
        return GlobalMount<T>::getPtr();
    }

    void removeCard(PlayerCard* card); //kill a card permanently
    void clear(CardSpots spot); //remove all cards from a spot. If NOWHERE is passed, ALL cards are removed
    void newTurn();

    void addEnemyCardToDeck(EnemyCard* card);
    void playerTakeDamage(int damage);
    void playerChangeHunger(int amount);

};

#endif // GAMESTATE_H_INCLUDED
