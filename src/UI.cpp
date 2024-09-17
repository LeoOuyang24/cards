#include <cmath>

#include "sequencer.h"
#include "FreeTypeHelper.h"

#include "../headers/UI.h"

glm::vec4 GameUI::getHandRect()
{
    glm::vec2 screenDimen = ViewPort::getScreenDimen();
    float width = screenDimen.x*0.7;
    float height = screenDimen.y*0.2;

    return glm::vec4(0.5*(screenDimen.x - width),0.65*screenDimen.y,width,height);
}

glm::vec4 GameUI::getPlayRect()
{
    glm::vec2 screenDimen = ViewPort::getScreenDimen();
    float width = screenDimen.x*0.7;
    float height = screenDimen.y*0.5;

    glm::vec4 handRect = getHandRect();

    return glm::vec4(handRect.x,handRect.y - height,width,height);
}

glm::vec4 GameUI::getEnemyRect()
{
    glm::vec2 screenDimen = ViewPort::getScreenDimen();
    glm::vec2 dimen = 2.0f*CardUI::CARD_DIMENS;
    return glm::vec4(screenDimen.x/2 - dimen.x/2,0.1*screenDimen.y,dimen);
}

glm::vec4 GameUI::getDeckRect()
{
    glm::vec4 playRect = getPlayRect();
    glm::vec2 dimens = 1.2f*CardUI::CARD_DIMENS;
    return glm::vec4(playRect.x + playRect.z - dimens.x/2,playRect.y - dimens.y/2,dimens);
}

CardUIOrient CardUIOrient::operator+(const CardUIOrient& b) const
{
    return {this->rect + b.rect, this->angle + b.angle};
}
CardUIOrient CardUIOrient::operator*(float val) const
{
    return {val*this->rect,val*this->angle};
}

CardUIOrient CardUIOrient::operator-(const CardUIOrient& b) const
{
    return *this + b*-1.0f;
}

 std::unique_ptr<CardTextFont> CardUI::cardTextFont;
 Sprite CardUI::blankCard;
void CardUI::onHover()
{
    glm::vec2 dimen = 1.75f*CardUI::CARD_DIMENS;
    render(glm::vec4(getRect().x,getRect().y - .5f*dimen.y,dimen),0,1);
}

CardUI::CardUI(CardPtr& card_,const glm::vec4& pos_, float radians_) : card(card_), rect(pos_), radians(radians_)
{

}

Card* CardUI::getCard()
{
    return card.lock().get();
}

glm::vec4 CardUI::getRect()
{
    return rect;
}

float CardUI::getAngle()
{
    return radians;
}

bool CardUI::getInHand()
{
    return inHand;
}

void CardUI::setInHand(bool val)
{
    inHand = val;
}

void CardUI::setRect(const glm::vec4& rect)
{
    this->rect = rect;
}

void CardUI::setAngle(float angle)
{
    this->radians = angle;
}

void CardUI::orient(const CardUIOrient& orientation)
{
    setRect(orientation.rect);
    setAngle(orientation.angle);
}

void CardUI::render()
{
    render(rect,radians);
}

void CardUI::render(const glm::vec4& pos, float angle, int z)
{
    if (Card* ptr = getCard())
    {
        SpriteManager::requestSprite({*ViewPort::basicProgram,&blankCard},
                             pos,z,angle);

        glm::vec2 center = {pos.x + pos.z/2, pos.y + pos.a/2};

        glm::vec4 spriteRect = rotateRect(glm::vec4(pos.x + .1*pos.z,pos.y + .03*pos.a  ,.8*pos.z,pos.a/2),center,angle); //rect where we render the sprite
        glm::vec4 nameRect = rotateRect(glm::vec4(pos.x + .26*pos.z,pos.y + 153.0/278*pos.a,0.48*pos.z,.06*pos.a),center,angle); //rect where we render the name
        glm::vec4 textRect = rotateRect(glm::vec4(pos.x + .1*pos.z,pos.y+174.0/278*pos.a,.8*pos.z,pos.a*.3f),center,angle); //rect where we render card text

        SpriteManager::requestSprite({*ViewPort::basicProgram,ptr->getSprite()},
                                     spriteRect,z,angle);

        cardTextFont->requestWrite({ptr->getName(),
                                   nameRect,
                                   -1.f,{1,1,1,1},angle,z,CENTER, VERTCENTER
                                   },*ViewPort::basicProgram);

        cardTextFont->requestWrite({ptr->getText(),
                                   textRect,
                                   -1.f,{0,0,0,1},angle,z,CENTER,VERTCENTER},*ViewPort::basicProgram);
    }
}

bool CardUI::handleInput()
{
    glm::vec2 mousePos = pairtoVec(MouseManager::getMousePos());
    if (pointInVec(getRect(),mousePos,getAngle()))
    {
        if (MouseManager::isPressed(SDL_BUTTON_LEFT))
        {
            glm::vec2 dimen = {getRect().z,getRect().a};
            setRect(glm::vec4(mousePos - 0.5f*dimen,dimen));
        }
        else
        {
            onHover();
        }
        return true;
    }
    return false;
}

CardUIOrient HandUI::getCardUIRect(int index, int handSize)
{
    //set up a bunch of constants
    glm::vec4 handRect = GameUI::getHandRect();
    glm::vec2 centerPoint = {handRect.x + handRect.z/2, handRect.y + handRect.a*2};
    float radius = handRect.z/2;
    float angle = -M_PI/2 + M_PI/16*(index-handSize/2);

    //in order to ensure the cards overlap each other correctly, each card has a higher z
    glm::vec4 cardRect = glm::vec4(centerPoint + radius*glm::vec2(cos(angle),sin(angle)),CardUI::CARD_DIMENS);
    float cardAngle = angle + M_PI/2;

    return {cardRect,cardAngle};

}


void HandUI::addCard(CardUIPtr& card,bool orient)
{
    card->setInHand(true);
    if (orient)
    {
        auto it = cardUIs.begin();
        for (; it != cardUIs.end(); ++it)
        {
            if ((*it)->getRect().x > card->getRect().x)
            {
                break;
            }
        }
        cardUIs.insert(it,card);
        reorient();
    }
    else
    {
        cardUIs.push_back(card);
    }
}

void HandUI::reorient()
{
    int i = 0;
    for (auto it = cardUIs.begin(); it != cardUIs.end(); ++it,i++)
    {
        (*it)->orient(getCardUIRect(i,cardUIs.size()));
    }
}

void HandUI::removeCard(CardUIPtr& card)
{
    auto found = std::find(cardUIs.begin(),cardUIs.end(),card);
    if (found != cardUIs.end())
    {
        cardUIs.erase(found);
    }
    reorient();
}

void HandUI::render()
{
    for (auto it = cardUIs.begin(); it != cardUIs.end(); ++it)
    {
        (*it)->render();
    }
}

template<typename T>
T lerp(T a1, T a2, float t)
{
    return a1 + (a2 - a1)*t;
}



void HandUI::drawCards(std::vector<CardUIPtr>& cards)
{
    glm::vec2 screenDimen = ViewPort::getScreenDimen();

    glm::vec4 start = {screenDimen.x,screenDimen.y,0,0};
    glm::vec4 end = glm::vec4(0.8f*screenDimen.x,0.5f*screenDimen.y,2.0f*CardUI::CARD_DIMENS);

    int i = 0;
    //reorient all existing cards
    for (auto it = cardUIs.begin(); it != cardUIs.end(); ++it,++i)
    {
        CardUIOrient orient = getCardUIRect(i,cards.size() + cardUIs.size());
        (*it)->orient(orient);
    }

    //draw the new cards
    //adding them to the handUI
    //and showing a draw animation
    int size = cardUIs.size();
    float time = 250.0f; //ms it takes for each "leg" of the animation to run
    //build the animation sequence
    Sequencer* sequencer = new Sequencer();
    for (int i = 0; i < cards.size(); i++)
    {
        //add the card to our ui
        auto ptr = cards[i];
        addCard(ptr);

        auto result = getCardUIRect(size + i,size + cards.size());
        sequencer->addUnits(([ptr,start,end,time](int runtime){
                            ptr->setRect(lerp(start,end,runtime/time));
                            return (runtime>=time);
                            }),
                            ([ptr,end,result,time](int runtime){
                             ptr->orient(lerp({end,0},result,runtime/time));
                             return (runtime >= time);
                             })
                            );
    }
    SequenceManager::request(*sequencer);

}

void BoardUI::addCard(CardUIPtr& ptr)
{
    ptr->setInHand(false);
    cardUIs.push_back(ptr);
    if (ptr.get())
    {
        ptr->setAngle(0);
    }
}

void BoardUI::removeCard(CardUIPtr& card)
{
    //you may notice that this is literally the same as HandUI
    //the reason that I haven't made both boardui and handui descendants of a parent class yet is
    //1) there's not really any reason for polymorphism yet
    //2) boardUI::removeCard may change in the future. Especially with how BoardUI will be responsible for playing cards, the two may need to be separated
    //for now though, they are literally identical
    auto found = std::find(cardUIs.begin(),cardUIs.end(),card);
    if (found != cardUIs.end())
    {
        cardUIs.erase(found);
    }
}

void BoardUI::render()
{
    for (auto it = cardUIs.begin(); it != cardUIs.end(); ++it)
    {
        (*it)->render();
    }
}

CardUIPtr MasterCardsUI::addCard(CardPtr& card)
{
    CardUIPtr ptr(new CardUI(card));
    playerCards.push_back(ptr);
    return ptr;
}

void MasterCardsUI::drawCards(std::vector<CardPtr>& drawn)
{
    std::vector<CardUIPtr> drawnUIs;
    for (int i = 0; i < drawn.size(); i++)
    {
        drawnUIs.push_back(addCard(drawn[i])); //add card to both masterui and handui
    }
    handUI.drawCards(drawnUIs);
}

void MasterCardsUI::update()
{
    if (MouseManager::getJustReleased() == SDL_BUTTON_LEFT)
    {
        if (heldCard.get())
        {
            if (!heldCard->getInHand() && heldCard->getRect().y >= GameUI::getHandRect().y)
            {
                handUI.addCard(heldCard,true);
                boardUI.removeCard(heldCard);
            }
            else if (heldCard->getInHand() && heldCard->getRect().y < GameUI::getHandRect().y)
            {
                handUI.removeCard(heldCard);
                boardUI.addCard(heldCard);
            }
            heldCard.reset();
        }
    }
    //check for input for all cards. Only do this if we are not dragging a card
    if (!heldCard.get())
    {
        for (auto it = playerCards.begin(); it != playerCards.end(); ++it)
        {
            //if no cardui has handled input
            if (!inputHandled)
            {
                //check if this card can handle input
                inputHandled = (*it)->handleInput();

                //check if the mouse clicked, and thus is dragging a card
                //honestly not a huge fan of how this is handled rn
                //may want to consider adding the dragging logic to MasterCardsUI away from CardUI::handleInput()
                if (inputHandled && MouseManager::getJustClicked() == SDL_BUTTON_LEFT)
                {
                    heldCard = *it;
                }

            }
        }
    }
    else
    {
        heldCard->handleInput();
    }


    handUI.render();
    boardUI.render();

    //reset "inputHandled"
    inputHandled = false;
}
