#include "../headers/gamestate.h"

void BoardState::addCard(const CardWeakPtr& card)
{
    if (Card* ptr = card.lock().get())
    {
        HashCardPtr weakptr(card);
        if (cards.insert(weakptr).second) //if the card was actually inserted (it didn't exist beforehand), update resources
        {
            curResources += ptr->getStats();
        }
    }
}

void BoardState::removeCard(const CardWeakPtr& card)
{
    if (Card* ptr = card.lock().get())
    {
        HashCardPtr weakptr(card);
        if (cards.erase(weakptr) == 1) //erase the card. If it actually was removed (it was in the set) also update our board resources
        {
            curResources -= ptr->getStats();
        }
    }
}

void BoardState::clear()
{
    cards.clear();
    curResources = {};
}

ResourceStats BoardState::getResources() const
{
    return curResources;
}

void HandState::addCard(const CardWeakPtr& card)
{
    hand.push_back(card);
}

void HandState::removeCard(const CardWeakPtr& card)
{
    for (auto it = hand.begin(); it != hand.end(); ++it)
    {
        if (it->lock().get() == card.lock().get())
        {
            hand.erase(it);
            return;
        }
    }
}

void HandState::clear()
{
    hand.clear();
}

const HandType& HandState::getHand() const
{
    return hand;
}

void EnemyState::setEnemy(EnemyCard* card)
{
    enemy.reset(card);
}

void EnemyState::setNextEnemy()
{
    if (deck.size() > 0)
    {
        enemy = std::move(deck.getTop());
        deck.pop();
    }
}

const EnemyPtr& EnemyState::getEnemy() const
{
    return enemy;
}


std::unique_ptr<GameState> GameState::curState;


GameState* GameState::getGameState()
{
    return curState.get();
}

void GameState::init()
{
    curState.reset(new GameState());
}

const CardPtr& GameState::addCard(Card* card, CardSpots spots )
{
    if (cards.find(card) == cards.end()) //card is new, add it
    {
        cards[card] = {CardPtr(card),spots};
        if (spots != NOWHERE)
        {
            states[spots]->addCard(cards[card].first);
        }
    }
    else
    {
        auto pear =&cards[card];
        if (pear->second != spots) //if card is being moved to a new spot ...
        {
            if (pear->second != NOWHERE) //remove the card from its old spot
            {
                states[pear->second]->removeCard(pear->first);
            }
            if (spots != NOWHERE)
            {
                states[spots]->addCard(pear->first);
            }
            pear->second = spots;
        }
    }
    return cards[card].first;

}

const CardPtr& GameState::getCard(Card* card)
{
    if (cards.find(card) == cards.end())
    {
        return CardPtr();
    }
    return cards[card].first;
}

const EnemyState& GameState::getEnemyState() const
{
    return enemy;
}

CardSpots GameState::getCardSpot(Card* card)
{
    if (cards.find(card) == cards.end())
    {
        return NOWHERE;
    }
    return cards[card].second;
}

void GameState::removeCard(Card* card)
{
    if (cards.find(card) != cards.end())
    {
        if (cards[card].second != NOWHERE)
        {
            states[cards[card].second].get()->removeCard(cards[card].first); //remove card from spot
        }
        cards.erase(card); //delete the card from the master base
    }
}

void GameState::clear(CardSpots spot)
{
    if (spot != NOWHERE)
    {
        states[spot].get()->clear();
    }
    else
    {
        for (auto it = states.begin(); it != states.end(); ++it)
        {
            it->second->clear();
        }
    }
}

void GameState::newTurn()
{
    enemy.setNextEnemy();
}
