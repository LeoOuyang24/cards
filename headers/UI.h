#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "geometry.h"

#include "deck.h"

struct GameUI
{
    static glm::vec4 getHandRect();
    static glm::vec4 getPlayRect(); //
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
    bool inHand = false;
    void onHover();
public:
    static constexpr glm::vec2 CARD_DIMENS = {100,140};
    CardUI(CardPtr& card_,const glm::vec4& pos_ = glm::vec4(0,0,CARD_DIMENS), float radians_ = 0);

    Card* getCard();
    glm::vec4 getRect();
    float getAngle();
    bool getInHand();

    void setInHand(bool val);
    void setRect(const glm::vec4& rect);
    void setAngle(float angle);
    void orient(const CardUIOrient&);

    void render();
    void render(const glm::vec4& pos, float angle,int z = 0);

    //handles input
    //return true if input was handled, false if nothing to do
    bool handleInput();
};

typedef std::shared_ptr<CardUI> CardUIPtr;

class HandUI
{
    std::list<CardUIPtr> cardUIs;

    //returns what should be the UI position and angle of a card, given its index and hand size
    static CardUIOrient getCardUIRect(int index, int handSize);

public:
    //adds a card and maybe orient it
    void addCard(CardUIPtr& card, bool orient = false);
    //reorients the whole hand
    void reorient();
    void removeCard(CardUIPtr& card);
    void render();
    void drawCards(std::vector<CardUIPtr>& cards);
    //renders the cards, most notably renders them from left->right
};

class BoardUI
{
    //handles cards on the board.
    //pretty simple really, since the cards can be in any order

    std::list<CardUIPtr> cardUIs;
public:
    void addCard(CardUIPtr& ptr);
    void removeCard(CardUIPtr& card);
    void render();
};

class MasterCardsUI
{
    //handles UI elements regarding cards in the hand, cards in play, enemy cards

    //checks if cardui input has already been handled. Ensures only one card ui is being handled at a time
    bool inputHandled = false;

    CardUIPtr heldCard; //card currently being dragged by the player

    //all player cards, whether they are in hand or board
    std::list<CardUIPtr> playerCards;
    HandUI handUI;
    BoardUI boardUI;
public:
    //add a card. Literally as raw as possible; doesn't add to hand or anything
    CardUIPtr addCard(CardPtr& card);

    //add some drawn cards. also adds them to handui, which plays a little drawing animation
    //handui also handles orienting the cards
    void drawCards(std::vector<CardPtr>& cards);
    void update();
};
#endif // UI_H_INCLUDED
