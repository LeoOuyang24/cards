#include <cmath>

#include "sequencer.h"
#include "FreeTypeHelper.h"
#include "vanilla.h"
#include "resourcesMaster.h"

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
    return glm::vec4(screenDimen.x/2 - dimen.x/2,0.05*screenDimen.y,dimen);
}

glm::vec4 GameUI::getRewardsRect()
{
    glm::vec4 playRect = getPlayRect();
    int height = 0.1*ViewPort::getScreenDimen().y;
    return {playRect.x, playRect.y - height, playRect.z, height};
}

glm::vec4 GameUI::getDeckRect()
{
    glm::vec4 playRect = getPlayRect();
    glm::vec2 dimens = 1.2f*CardUI::CARD_DIMENS;
    return glm::vec4(playRect.x + playRect.z - dimens.x/2,playRect.y - dimens.y/2,dimens);
}

glm::vec4 GameUI::getHealthRect()
{
    glm::vec4 handRect = getHandRect();
    float height = 100;
    return glm::vec4(handRect.x,handRect.y - height/2, handRect.z/3,height);
}

glm::vec4 GameUI::getHungerRect()
{
    glm::vec4 health = getHealthRect();
    return glm::vec4(health.x,health.y + health.a, health.z, health.a);
}

CardUIOrient CardUIOrient::operator+(const CardUIOrient& b) const
{
    return {this->rect + b.rect, this->angle + b.angle, this->z, this->flip + b.flip};
}
CardUIOrient CardUIOrient::operator*(float val) const
{
    return {val*this->rect,val*this->angle,this->z,val*this->flip};
}

CardUIOrient CardUIOrient::operator-(const CardUIOrient& b) const
{
    return *this + b*-1.0f;
}

bool CardUIOrient::isFacedown() const
{
    return flip > M_PI/2;
}

 std::unique_ptr<CardTextFont> CardUI::cardTextFont;
 std::unique_ptr<BasicRenderPipeline> CardUI::cardTextShader;
 Sprite CardUI::blankCard;
void CardUI::onHover()
{
    glm::vec2 dimen = 1.75f*CardUI::CARD_DIMENS;
    render({glm::vec4(getRect().x,getRect().y - .5f*dimen.y,dimen),0,GameUI::cardHoverZ});
}

CardUI::CardUI(const CardPtr& card_,const CardUIOrient& orient_) : card(card_),orient(orient_)
{

}


Card* CardUI::getCard() const
{
    return card.lock().get();
}

CardWeakPtr& CardUI::getCardPtr()
{
    return card;
}

glm::vec4 CardUI::getRect()
{
    return orient.rect;
}

float CardUI::getAngle()
{
    return orient.angle;
}

int CardUI::getZ()
{
    return orient.z;
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
    orient.rect = rect;
}

void CardUI::setAngle(float angle)
{
    orient.angle = angle;
}

void CardUI::setOrient(const CardUIOrient& orientation)
{
    orient = orientation;
}

glm::vec4 CardUI::getCardTextRect(const glm::vec4& pos)
{
    return glm::vec4(pos.x + .1*pos.z,pos.y+135.0/278*pos.a,.8*pos.z,pos.a*.4f);
}

void CardUI::renderCardText(const glm::vec4& pos, float angle, int z)
{
    glm::vec2 center = {pos.x + pos.z/2, pos.y + pos.a/2};
    glm::vec4 textRect = rotateRect(getCardTextRect(pos),center,angle); //rect where we render card text

    cardTextFont->requestWrite({getCard()->getText(),
                                textRect,
                                -1.f,{1,1,1,1},angle,z,CENTER,DOWN},*cardTextShader);
}

void CardUI::render()
{
    render(orient);
}

void CardUI::render(const CardUIOrient& o)
{
    if (Card const * ptr = getCard())
    {
        glm::vec2 center = {o.rect.x + o.rect.z/2, o.rect.y + o.rect.a/2};

        glm::vec4 spriteRect = rotateRect(glm::vec4(o.rect.x + .1*o.rect.z,o.rect.y + .03*o.rect.a  ,.8*o.rect.z,110.0/278*o.rect.a),center,o.angle); //rect where we render the sprite
        glm::vec4 nameRect = rotateRect(glm::vec4(o.rect.x + .26*o.rect.z,o.rect.y + 120.0/278*o.rect.a,0.48*o.rect.z,.06*o.rect.a),center,o.angle); //rect where we render the name


        SpriteManager::requestSprite({*MasterCardsUI::CardShader,&blankCard},
                             o.rect,o.z,o.angle, o.flip);

        SpriteManager::requestSprite({*MasterCardsUI::CardShader,ptr->getSprite()},
                                     spriteRect,o.z,o.angle, o.flip);

        renderCardText(o.rect,o.angle,o.z);

        cardTextFont->requestWrite({ptr->getName(),
                                   nameRect,
                                   -1.f,{0,0,0,1},o.angle,o.z,CENTER, VERTCENTER
                                   },*MasterCardsUI::CardShader);

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


void HandUI::addCard(CardUIPtr card,bool orient)
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

void HandUI::addCard(CardUIPtr card, const CardUIOrient& origin)
{
    const float totalTime = 250.0f;
    addCard(card);
    CardUIOrient handRect = getCardUIRect(cardUIs.size(),cardUIs.size());
    SequenceManager::request(*(new Sequencer([origin,handRect,this,card,totalTime](int runtime)
                  {
                    card->setOrient(lerp(origin,handRect,runtime/totalTime));
                    return (runtime >= totalTime);
                  })));
}

void HandUI::reorient()
{
    int i = 0;
    for (auto it = cardUIs.begin(); it != cardUIs.end(); ++it,i++)
    {
        (*it)->setOrient({getCardUIRect(i,cardUIs.size())});
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
        (*it)->setOrient(orient);
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
                             ptr->setOrient(lerp({end,0},result,runtime/time));
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

glm::vec4 EnemyCardUI::getChoiceRect(const glm::vec4& fullCardRect, int index)
{
    glm::vec4 textRect = getCardTextRect(fullCardRect);
    const int choiceHeight = textRect.a*(1.0/EnemyCard::maxChoices); //height of each choice
    return glm::vec4(textRect.x,textRect.y + choiceHeight*index,textRect.z,choiceHeight);
}


void EnemyCardUI::renderCardText(const glm::vec4& pos, float angle, int z)
{
    if ( EnemyCard const* card = static_cast<EnemyCard const *>(getCard()))
    {
        Choices const* choices = &card->getChoices();
        glm::vec2 mousePos = pairtoVec(MouseManager::getMousePos());
        float angle = getAngle();
        for (int i = 0; i < choices->size(); i++)
        {
            glm::vec4 choiceRect = getChoiceRect(pos,i);

            choices->at(i)->render(choiceRect);

        }
    }
}

EnemyCardUI::EnemyCardUI(const std::shared_ptr<EnemyCard>& card_) : CardUI(card_,{GameUI::getEnemyRect(),0}) //by default, enemy cards are locked to a position and angle
{

}

bool EnemyCardUI::handleInput()
{
    if ( EnemyCard const* card = static_cast<EnemyCard const *>(getCard()))
    {
        Choices const* choices = &card->getChoices();
        glm::vec4 ourRect = getRect();
        glm::vec2 mousePos = pairtoVec(MouseManager::getMousePos());
        float angle = getAngle();

        for (int i = 0; i < choices->size(); i++)
        {
            glm::vec4 choiceRect = getChoiceRect(ourRect,i);
            if (pointInVec(choiceRect,mousePos,angle))
            {
                PolyRender::requestRect(choiceRect,glm::vec4(1,0,0,0.5),true,angle,getZ()+1);
                if (MouseManager::getJustClicked() == SDL_BUTTON_LEFT && choices->at(i)->isValid())
                {
                    MasterCardsUI::getUI()->choseChoice(*choices->at(i));
                }
                return true;
            }
        }
    }
    return false;
}

void EnemyCardUI::render(const CardUIOrient& o)
{
    if (o.isFacedown()) //if face down, render the deck's cardback
    {
        SpriteManager::requestSprite({*MasterCardsUI::CardShader,GameState::getGameState()->getEnemyState().getDeck().getCardBack()},o.rect,o.z,o.angle,o.flip);
    }
    else
    {
        CardUI::render(o);
    }
}

void EnemyUI::setEnemyCard(const std::shared_ptr<EnemyCard>& card)
{
    currentEnemy.reset(new EnemyCardUI(card));
}

const EnemyCardUI& EnemyUI::getCurrentEnemy()
{
    return *currentEnemy;
}

void EnemyUI::draw(const std::shared_ptr<EnemyCard>& card)
{
    setEnemyCard(card);
    CardUIOrient last = {GameUI::getEnemyRect()};
    SequenceManager::request(*(new Sequencer([this,last](int runtime){
                            currentEnemy->setOrient(lerp({GameUI::getDeckRect(),0,0,M_PI},last,runtime/500.0f,true));
                             return (runtime >= 500);
                             })));
}

void EnemyUI::update()
{
    if (CardUI* ui = currentEnemy.get())
    {
        ui->render();
        ui->handleInput();
    }
    SpriteManager::requestSprite({*ViewPort::basicProgram,GameState::getGameState()->getEnemyState().getDeck().getCardBack()},GameUI::getDeckRect(),1);
}

bool RewardCardUI::handleInput()
{
    glm::vec2 mousePos = pairtoVec(MouseManager::getMousePos());
    if (pointInVec(getRect(),mousePos,getAngle()) && MouseManager::getJustClicked() == SDL_BUTTON_LEFT)
    {
       // MasterCardsUI::CardsUI.handUI.addCard(CardUIPtr(getCard())) //need a way to add a card
    }
}

Sequencer* EffectsUI::floatingText(std::string text, const glm::vec3& color, const glm::vec2& point, Font& font) const
{
    return new Sequencer([&font,color,text,point](int time){
                            font.requestWrite({text,glm::vec4(point - glm::vec2(0,time/100.0f),0,0),1.5f,glm::vec4(color,time/1000.0f),0,GameUI::effectsZ
                                   },*CardUI::cardTextShader);
                            return time >= 1000;
                         });
}

EffectsUI::EffectsUI()
{
    effectsProgram.reset(new RenderProgram("./shaders/polygonVertex.h",
                                        ResourcesConfig::config[ResourcesConfig::RESOURCES_DIR] + "/shaders/fragment/simpleFragment.h"));
    bleedingCorpse.reset(new RenderProgram({LoadShaderInfo{templateShader(stripComments(readFile("./shaders/cardVertexShader.h").first), true,
                                                          {"int time"},
                                                          {"int ms" },
                                                          {"ms = time"}
                                                          ),GL_VERTEX_SHADER,false},
                                           {"./shaders/bleedingFragment.h",GL_FRAGMENT_SHADER}}
                                           ));

    bloodSplatter.reset(new Sprite("sprites/effects/blood.png"));
    EffectsUIFont.reset(new Font("fonts/Cryptik.ttf"));
}

Sequencer* EffectsUI::killAMfer() const
{
    Sequencer* sequencer = (new Sequencer([this](int runtime){
                    SpriteManager::request(RenderRequest{*effectsProgram},GameUI::effectsZ,GameUI::effectsZ,glm::vec4(1,0,0,1-runtime/1000.0f));
                    SpriteManager::requestSprite({*ViewPort::basicProgram,bloodSplatter.get()},GameUI::getEnemyRect(),GameUI::effectsZ);
                    SpriteManager::request(RenderRequest{*bleedingCorpse},GameUI::effectsZ-1,GameUI::getEnemyRect(),GameUI::effectsZ - 1,0,0,runtime);
                    return runtime >= 1500;
                                             }
                                             ));
    return sequencer;
}

Sequencer* EffectsUI::takeDamage() const
{
    return new Sequencer([this](int runtime){
                        SpriteManager::request(RenderRequest{*effectsProgram},GameUI::effectsZ,GameUI::effectsZ,glm::vec4(1,0,0,1-runtime/1000.0f));
                         return runtime >= 1000;
                         });
}

Sequencer* EffectsUI::addHunger(int amount) const
{
    glm::vec4 hungerRect = GameUI::getHungerRect();
    return floatingText((amount >= 0 ? "+" : "") + std::to_string(amount),
                        glm::vec4(0,1,0,1),
                        glm::vec2(hungerRect.x, GameUI::getHealthRect().y),
                        *CardUI::cardTextFont
                        );
}

Sequencer* EffectsUI::die() const
{
    return new Sequencer([this](int runtime){
                         SpriteManager::request({*effectsProgram},GameUI::effectsZ,GameUI::effectsZ,
                                                glm::vec4(0,0,0,std::min(1.0f,runtime/1000.0f)));
                        return runtime >= 1000;
                         },
                         [this](int runtime){
                        SpriteManager::request({*effectsProgram},GameUI::effectsZ,GameUI::effectsZ,
                            glm::vec4(0,0,0,1));

                        EffectsUIFont->requestWrite({"Game Over",glm::vec4(100,100,500,200),1,glm::vec4(1,0,0,1),0,GameUI::effectsZ+1});

                        return false;
                         }

                         );
}

std::unique_ptr<MasterCardsUI> MasterCardsUI::CardsUI;
std::unique_ptr<BasicRenderPipeline> MasterCardsUI::CardShader;
void MasterCardsUI::init()
{
    CardShader.reset(new BasicRenderPipeline("shaders/cardVertexShader.h","shaders/cardFragmentShader.h"));

    CardsUI.reset(new MasterCardsUI());
}

void MasterCardsUI::newTurn()
{
    GameState::getGameState()->newTurn();
    enemyUI.draw(GameState::getGameState()->getEnemyState().getEnemy());
}

MasterCardsUI* MasterCardsUI::getUI()
{
    return CardsUI.get();
}

CardUIPtr MasterCardsUI::addCard(const CardPtr& card)
{
    CardUIPtr ptr(new CardUI(card, {}));
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

CardUIPtr MasterCardsUI::addCardToHand(Card* card, const CardUIOrient& origin) //create a card and add it to hand.
{
    CardPtr ptr = GameState::getGameState()->addCard(card,HAND);
    CardUIPtr UIptr = addCard(ptr);
    handUI.addCard(UIptr,origin);

    return UIptr;
}

void MasterCardsUI::removeCard(CardUIPtr& card)
{
    if (Card* cardPtr = card->getCard())
    {
        auto found = std::find(playerCards.begin(), playerCards.end(), card);
        if (found != playerCards.end())
        {
            switch (GameState::getGameState()->getCardSpot(cardPtr)) //remove the card from the appropriate UI
            {
                case HAND:
                    handUI.removeCard(card);
                    break;
                case BOARD:
                    boardUI.removeCard(card);
                    break;
                default:
                    break;
            }
            playerCards.erase(found);
            GameState::getGameState()->removeCard(cardPtr);
        }
    }
}

void MasterCardsUI::clearBoard()
{
    //REFACTOR: THIS SUCKS. Maybe organize all the carduis depending on their location rather than just linear searching??
    for (auto& cardUI: playerCards)
    {
        if (GameState::getGameState()->getCardSpot(cardUI->getCard()) == BOARD) //if this card is on the board, remove it
            {
                removeCard(cardUI);
            }
    }
}

void MasterCardsUI::choseChoice(Choice& choice)
{
    Sequencer* sequencer = choice.isAttack() ? effectsUI.killAMfer() : new Sequencer();

    sequencer->addUnits([&choice](int runtime){
                        choice.choose();
                        return true;
                       },
                       [&choice,this](int runtime){
                        if (choice.isAttack())
                        {
                            static_cast<Attack&>(choice).inflictConsequence();
                        }
                        newTurn();
                        return true;
                       }
                       );

    SequenceManager::request(*sequencer);
}

void MasterCardsUI::killEnemy(Attack& attack)
{
    /*->addUnits([](int runtime){
                attack.Trade::choose();
                                     });*/
}

void MasterCardsUI::playerTakeDamage(int damage)
{
    GameState::getGameState()->playerTakeDamage(damage);

    Sequencer* seq = nullptr;
    if (GameState::getGameState()->getPlayerState().isDead())
    {
        seq = effectsUI.die();
    }
    else
    {
        seq = effectsUI.takeDamage();
    }
    SequenceManager::request(*seq);
}

void MasterCardsUI::playerChangeHunger(int hunger)
{
    SequenceManager::request(*effectsUI.addHunger(hunger - GameState::getGameState()->getPlayerState().getHunger()));
}

void MasterCardsUI::moveCard(CardUIPtr& card, bool toHand) //move a card to the hand or board
{
    if (toHand)
    {
        handUI.addCard(card,true);
        boardUI.removeCard(card);
        GameState::getGameState()->addCard(card->getCard(),HAND);

    }
    else
    {
        handUI.removeCard(card);
        boardUI.addCard(card);
        GameState::getGameState()->addCard(card->getCard(),BOARD);
    }
}

void MasterCardsUI::loadHand( const HandType& hand)
{
    for (auto it = hand.begin(); it != hand.end(); ++it)
    {
        handUI.addCard(addCard(GameState::getGameState()->getCard(it->lock().get())),true);
    }
}


void MasterCardsUI::update()
{
    if (MouseManager::getJustReleased() == SDL_BUTTON_LEFT)
    {
        if (heldCard.get())
        {
            if (!heldCard->getInHand() && heldCard->getRect().y >= GameUI::getHandRect().y)
            {
                moveCard(heldCard,true);
            }
            else if (heldCard->getInHand() && heldCard->getRect().y < GameUI::getHandRect().y)
            {
                moveCard(heldCard,false);
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
                //may want to consider adding the dragging logic from MasterCardsUI to CardUI::handleInput()
                if (inputHandled && MouseManager::isPressed(SDL_BUTTON_LEFT))
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
    enemyUI.update();

    //reset "inputHandled"
    inputHandled = false;
}
