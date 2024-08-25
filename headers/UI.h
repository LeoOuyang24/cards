#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "geometry.h"

#include "deck.h"

struct GameUI
{
    static glm::vec4 getHandRect();
};

//orientation of a cardui
struct CardUIOrient
{
    glm::vec4 rect = {};
    float angle = 0;
    CardUIOrient operator+(const CardUIOrient& b) const; //apparently passing left hand value by value helps optimize?
    CardUIOrient operator-(const CardUIOrient& b) const ;
    CardUIOrient operator*(float val) const;
};

class CardUI
{
    float radians = 0;
    glm::vec4 rect = glm::vec4(0);
    std::weak_ptr<Card> card;
public:
    static constexpr glm::vec2 CARD_DIMENS = {100,140};
    CardUI(CardPtr& card_,const glm::vec4& pos_, float radians_);

    Card* getCard();
    glm::vec4 getRect();
    float getAngle();

    void setRect(const glm::vec4& rect);
    void setAngle(float angle);
    void orient(const CardUIOrient&);


    void render();
    void render(const glm::vec4& pos, float angle,int z = 0);



};

class HandUI
{
    //if mouse is already hovering over a card
    //makes it so only the left-most card is magnified if our mouse happens to overlap with multiple cards
    bool inputHandled = false;
    typedef std::shared_ptr<CardUI> CardUIPtr;
    std::list<CardUIPtr> cardUIs;
    //returns what should be the UI position and angle of a card, given its index and hand size
    static CardUIOrient getCardUIRect(int index, int handSize);
    CardUIPtr addCard(CardPtr& card,const CardUIOrient& orient);
    void render(CardUIPtr& ptr);
    void onHover(CardUIPtr& ptr);
public:
    //reset the hand ui, replacing all cards with the cards in "hand"
    void resetHand(HandType& hand);
    void drawCards(std::vector<CardPtr>& cards);
    void handleInput();
    void update();
};

#endif // UI_H_INCLUDED
