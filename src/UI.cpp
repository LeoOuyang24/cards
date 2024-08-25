#include <cmath>

#include "sequencer.h"

#include "../headers/UI.h"

glm::vec4 GameUI::getHandRect()
{
    glm::vec2 screenDimen = ViewPort::getScreenDimen();
    float width = screenDimen.x*0.7;
    float height = screenDimen.y*0.2;

    return glm::vec4(0.5*(screenDimen.x - width),0.65*screenDimen.y,width,height);
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
        SpriteManager::requestSprite({*ViewPort::basicProgram,ptr->getSprite()},
                             pos,z,angle);

    }
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

void HandUI::render(CardUIPtr& ptr)
{
    ptr->render();
}

void HandUI::onHover(CardUIPtr& ptr)
{
    ptr->render(glm::vec4(ptr->getRect().x,GameUI::getHandRect().y - CardUI::CARD_DIMENS.y,2.0f*CardUI::CARD_DIMENS),0,1);
}

HandUI::CardUIPtr HandUI::addCard(CardPtr& card, const CardUIOrient& orient)
{

    CardUIPtr ptr(new CardUI(card,orient.rect,orient.angle));
    cardUIs.push_back(ptr);
    return ptr;
}

void HandUI::resetHand(HandType& hand)
{
    cardUIs.clear();
    int size = hand.size();
    int i = 0;
    for (auto it = hand.begin(); it != hand.end(); ++it)
    {
        addCard(*it,getCardUIRect(i,size));
        i ++;
    }
    //SpriteManager::render();
    inputHandled = false;
}

template<typename T>
T lerp(T a1, T a2, float t)
{
    return a1 + (a2 - a1)*t;
}



void HandUI::drawCards(std::vector<CardPtr>& cards)
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
        //the orientation here doesn't matter, we're just gonna replace it anyway
        CardUIPtr ptr = addCard(cards[i],{});



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


void HandUI::update()
{
    bool hovered = false;
    glm::vec2 mousepos = pairtoVec(MouseManager::getMousePos());
    for (auto it = cardUIs.begin(); it != cardUIs.end(); ++it)
    {
        CardUIPtr ptr = (*it);
        if (!hovered && pointInVec(ptr->getRect(),mousepos,ptr->getAngle()))
        {
            onHover(ptr);
            hovered = true;
        }
        render(ptr);
    }
}

