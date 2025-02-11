#include "vanilla.h"

#include "../headers/enemyCards.h"
#include "../headers/gamestate.h"
#include "../headers/UI.h"
#include "../headers/loader.h"

std::unordered_map<std::string, ChoiceTypes>  Choice::stringToChoice = {
        {"trade", ChoiceTypes::TRADE},
        {"attack", ChoiceTypes::ATTACK},
        {"hurt", ChoiceTypes::HURT},
        {"text", ChoiceTypes::TEXT}
    };

void Choice::render(const glm::vec4& space, ZType z)
{
    CardUI::cardTextFont->requestWrite({getMessage(),
                       space,
                       -2.f,{1,1,1,1},0,z,CENTER, VERTCENTER
                       },*ViewPort::basicProgram);
}

void Choice::setReshuffle(bool var)
{
    reshuffle = var;
}

TextChoice::TextChoice(std::string message_) : Choice(), message(message_)
{

}

std::string TextChoice::getMessage()
{
    return message;
}

HurtChoice::HurtChoice(int damage_) : damage(damage_), Choice()
{

}

void HurtChoice::choose()
{
    MasterCardsUI::getUI()->playerTakeDamage(damage);
}

StarveChoice::StarveChoice() : HurtChoice(STARVE_DAMAGE)
{

}

void StarveChoice::choose()
{
    HurtChoice::choose();
    GameState::getGameState()->playerChangeHunger(PlayerState::MAX_HUNGER);
}

std::string HurtChoice::getMessage()
{
    return "take " + std::to_string(damage) + " damage!";
}

GenericChoice::GenericChoice(const ResourceStats& give_, ChoiceResult result_) : give(give_), result(result_)
{

}

bool GenericChoice::isValid()
{
    return getOffer() <= (GameState::getGameState()->getTracker<BoardState>())->getResources();
}

void GenericChoice::choose()
{
    result.result();
    MasterCardsUI::getUI()->clearBoard();
}

ResourceStats GenericChoice::getOffer()
{
    return give;
}

Sprite GenericChoice::arrow;

std::string GenericChoice::getMessage()
{
    return CardTextFont::getCardResourceString(give,true) + " -> " + result.label;
}

void GenericChoice::render(const glm::vec4& space, ZType z)
{
    CardUI::cardTextFont->requestWrite({CardTextFont::getCardResourceString(give,false),
                       glm::vec4(space.x,space.y,space.z/3,space.a),
                       -2.f,{1,1,1,1},0,z,CENTER, VERTCENTER
                       },*ViewPort::basicProgram);
     /*CardUI::cardTextFont->requestWrite({"->",
                       glm::vec4(space.x + space.z/3,space.y,space.z/3,space.a),
                       -2.f,{1,1,1,1},0,z,CENTER, VERTCENTER
                       },*ViewPort::basicProgram);*/
    SpriteManager::requestSprite({*ViewPort::basicProgram,&arrow}, glm::vec4(space.x + space.z/3,space.y,space.z/3,space.a),z);

    std::string message = result.label + (reshuffle ? ("\n" + std::string(1,CardTextFont::reshuffleChar)) : "");
    CardUI::cardTextFont->requestWrite({message,
                        glm::vec4(space.x + space.z/3*2, space.y, space.z/3, space.a),
                        -2.f,{1,1,1,1},0,z,CENTER, VERTCENTER
                        }, *ViewPort::basicProgram);
}

std::string Trade::getRewardsString(const CardRewards& get)
{
    std::string getString = "";
    for (int i = 0; i < get.size(); i ++)
    {
        getString += CardTextFont::getCardResourceString(get[i]->getStats(),false);
    }
    return getString;
}

Trade::Trade(const ResourceStats& give_, const CardRewards& get_) : GenericChoice(give_,{getRewardsString(get_),std::bind(&giveRewards,this)}), get(get_)
{

}


void Trade::giveRewards()
{
    glm::vec4 enemyRect = GameUI::getEnemyRect();

    for (auto card : get)
    {
        MasterCardsUI::getUI()->addCardToHand(card.get(),{enemyRect});
    }
}


CardRewards&& Trade::getRewards()
{
    return std::move(get);
}

std::string Trade::getMessage()
{
    return CardTextFont::getCardResourceString(give,true);
}

/*void Trade::render(const glm::vec4& space, ZType z)
{
    CardUI::cardTextFont->requestWrite({getMessage(),
                       glm::vec4(space.x,space.y,space.z/3,space.a),
                       -2.f,{1,1,1,1},0,z,CENTER, VERTCENTER
                       },*ViewPort::basicProgram);
     CardUI::cardTextFont->requestWrite({"->",
                       glm::vec4(space.x + space.z/3,space.y,space.z/3,space.a),
                       -2.f,{1,1,1,1},0,z,CENTER, VERTCENTER
                       },*ViewPort::basicProgram);
    std::string getString = "";
    for (int i = 0; i < get.size(); i ++)
    {
        getString += CardTextFont::getCardResourceString(get[i]->getStats(),false);
    }
    CardUI::cardTextFont->requestWrite({getString,
                        glm::vec4(space.x + space.z/3*2, space.y, space.z/3, space.a),
                        -2.f,{1,1,1,1},0,z,CENTER, VERTCENTER
                        }, *ViewPort::basicProgram);
}*/

Attack::Attack(int attack, CardRewards&& get, Consequence&& consequence_) : Trade({0,attack,0},get), consequence(consequence_)
{

}

void Attack::choose()
{
    Trade::choose();
   // MasterCardsUI::killEnemy(*this);
}

void Attack::inflictConsequence()
{
    consequence();
}

void ChoicesBody::renderCardText(const glm::vec4& rect, float angle, int z) const
{
    glm::vec2 mousePos = pairtoVec(MouseManager::getMousePos());
    for (int i = 0; i < choices.size(); i++)
    {
        glm::vec4 choiceRect = CardRenderer::getChoiceRect(rect,i);

        choices.at(i)->render(choiceRect,z+2); //+2 so the highlight rectangle will not cover it
    }
}

EnemyCard::EnemyCard(std::string name, std::string spritePath, std::initializer_list<Choice*> choices_) : EnemyCard(name,spritePath,choices_.begin(),choices_.end())
{

}

EnemyCard::EnemyCard(std::string jsonPath) : EnemyCard(static_cast<EnemyCard&&>(*loadCard(jsonPath)))
{

}

const Choices& EnemyCard::getChoices() const
{
    return static_cast<ChoicesBody*>(body.get())->choices;
}

Deck::Deck(Sprite* cardBack_) : cardBack(cardBack_)
{
    for (int i = 0; i < 2; i++)
    {
        /*EnemyCard* enemy = new EnemyCard("Grandma","sprites/cardfaces/grandma.png",{(new Trade({0,2,2},{ptr})),
                                        (new Attack(3,{ptr},[](){
                                        GameState::getGameState()->addEnemyCardToDeck(new EnemyCard("Guard","sprites/cardfaces/guard.png",
                                                                                    {new Attack(10,{},{}),new HurtChoice(1)}
                                                                                                    ));
                                                    }))});*/
        EnemyCard* enemy = new EnemyCard("grandma");//static_cast<EnemyCard*>(loadCard("grandma"));
        deck.emplace_back(enemy);

    }
}


EnemyPtr Deck::getTop()
{
    if (deck.size() > 0)
    {
        return deck.front();
    }
    else
    {
        return EnemyPtr(nullptr);
    }
}

Sprite const* Deck::getCardBack() const
{
    return cardBack.get();
}

void Deck::pop()
{
    if (deck.size() > 0)
        deck.pop_front();
}

size_t Deck::size() const
{
    return deck.size();
}

void Deck::addCard(const EnemyPtr& card)
{
    auto it = deck.begin();
    if (deck.size() >0)
    {
        std::advance(it, (rand()%deck.size()));
    }
    deck.insert(it,card);
}
