#ifndef UI_H_INCLUDED
#define UI_H_INCLUDED

#include "sequencer.h"
#include "geometry.h"

#include "deck.h"
#include "card_text.h"
#include "enemyCards.h"
#include "gamestate.h"

struct GameUI
{
    static glm::vec4 getHandRect();
    static glm::vec4 getPlayRect();
    static glm::vec4 getEnemyRect();
    static glm::vec4 getRewardsRect();
    static glm::vec4 getDeckRect();

    static constexpr int backgroundZ = 0;
};

//orientation of a cardui
struct CardUIOrient
{
    glm::vec4 rect = {};
    float angle = 0;
    int z = 0; //unlike the other two, doesn't really get impacted by mathematical operations. You should set this manually
    float flip = 0; //angle along the y axis

    //math operators that allow for easy transforms
    CardUIOrient operator+(const CardUIOrient& b) const; //apparently passing left hand value by value helps optimize?
    CardUIOrient operator-(const CardUIOrient& b) const ;
    CardUIOrient operator*(float val) const;

    bool isFacedown() const; //true if card is face down

};

class CardUI
{
    CardUIOrient orient;
    CardWeakPtr card;
    bool inHand = false;
    void onHover();
protected:
    glm::vec4 getCardTextRect(const glm::vec4& pos);

    //renders text, may be different from cards in hand vs enemy cards
    //I should probably make this for each aspect of the card rendering (sprite, name, etc) but I'm gonna do that as time comes
    //if it's not necessary I'm not gonna bother making more functions
    virtual void renderCardText(const glm::vec4& pos, float angle, int z);
public:
    static constexpr glm::vec2 CARD_DIMENS = {120,168};
    static std::unique_ptr<CardTextFont> cardTextFont;
    static Sprite blankCard; //sprite for a blank card

    CardUI(const CardPtr& card_,const CardUIOrient& orient_);

    Card* getCard();
    CardWeakPtr& getCardPtr();
    glm::vec4 getRect();
    float getAngle();
    int getZ();
    bool getInHand();

    void setInHand(bool val);
    void setRect(const glm::vec4& rect);
    void setAngle(float angle);
    void setOrient(const CardUIOrient&);

    void render();
    virtual void render(const CardUIOrient&);

    //handles input
    //return true if input was handled, false if nothing to do
    virtual bool handleInput();
};

typedef std::shared_ptr<CardUI> CardUIPtr;

class HandUI
{
    std::list<CardUIPtr> cardUIs;

    //returns what should be the UI position and angle of a card, given its index and hand size
    static CardUIOrient getCardUIRect(int index, int handSize);

public:
    //adds a card and maybe orient it
    void addCard(CardUIPtr card, bool orient = false);
    //add a card and play an animation of it coming from a position
    void addCard(CardUIPtr card, const CardUIOrient& origin);
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

//renders an enemy card
class EnemyCardUI : public CardUI
{
protected:
    glm::vec4 getChoiceRect(const glm::vec4& fullCardRect, int index);
    virtual void renderCardText(const glm::vec4& pos, float angle, int z);
public:
    EnemyCardUI(const std::shared_ptr<EnemyCard>& card_);
    bool handleInput();
    void render(const CardUIOrient& o);
};

//renders the enemy cards and rewards
class EnemyUI
{
public:
    void setEnemyCard(const std::shared_ptr<EnemyCard>& card);
    void draw(const std::shared_ptr<EnemyCard>& card); //set enemy card and play a drawing animation
    void update();
private:
    std::shared_ptr<EnemyCardUI> currentEnemy;
};

//renders a card that can be picked as a reward
class RewardCardUI : public CardUI
{
    //void onClick();
public:
    virtual bool handleInput();
};

class MasterCardsUI
{
    //handles UI elements regarding cards in the hand, cards in play, enemy cards

    //checks if cardui input has already been handled. Ensures only one card ui is being handled at a time
    bool inputHandled = false;

    CardUIPtr heldCard; //card currently being dragged by the player

    //all player cards, whether they are in hand or board
    std::list<CardUIPtr> playerCards;
    MasterCardsUI() //singleton constructor. it is assumed gamestate has been initialized by this point
    {
        loadHand(GameState::getGameState()->getTracker<HandState>()->getHand());

        enemyUI.setEnemyCard(GameState::getGameState()->getEnemyState().getEnemy());
    }
    static std::unique_ptr<MasterCardsUI> CardsUI;

public:
    HandUI handUI;
    BoardUI boardUI;
    EnemyUI enemyUI;

    static void init();
    static MasterCardsUI* getUI();
    static std::unique_ptr<BasicRenderPipeline> CardShader;//standard card shader
    void newTurn();


    //add a card. Literally as raw as possible; doesn't add to hand or anything
    CardUIPtr addCard(const CardPtr& card);


    //add some drawn cards. also adds them to handui, which plays a little drawing animation
    //handui also handles orienting the cards
    void drawCards(std::vector<CardPtr>& cards);


    void moveCard(CardUIPtr& card, bool toHand); //move a card to the hand or board

    //REFACTOR: I'm not sure if functions that involve the UIs should be in MasterCardsUI or their respective UIs (HandUI for addCardToHand, for example).
    //For now I'm doing it here because it allows me to add/remove the card to/from MasterCardsUI as well as the respective UI
    CardUIPtr addCardToHand(Card* card,const CardUIOrient& rect); //create a card and add it to hand. Origin rect is an animation
    void clearBoard(); //clear all cards from teh board, removing them completely from teh game.

    void removeCard(CardUIPtr& card);

    //load a hand into ui
    void loadHand(const HandType& hand);
    void update();
};
#endif // UI_H_INCLUDED
