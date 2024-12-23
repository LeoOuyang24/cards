#include "../headers/player.h"
#include "../headers/gamestate.h"
#include "../headers/UI.h"

PlayerUI::PlayerUI()
{
    HeartIcon.reset((new Sprite("sprites/icons/heart.png")));
    HungerIcon.reset((new Sprite("sprites/icons/hunger.png")));
}

void PlayerUI::render()
{
    glm::vec4 healthRect = GameUI::getHealthRect();
    glm::vec4 hungerRect = GameUI::getHungerRect();
    for (int i = 0; i < GameState::getGameState()->getPlayerState().getHealth(); i ++)
    {
        int maxWidth = 30;
        int margin = 5;
        glm::vec2 center = {healthRect.x + margin*(i+1) + (maxWidth/2)*i, healthRect.y + healthRect.a};
        int width = lerp(maxWidth,maxWidth*9/10,SDL_GetTicks()%10000 >= 9000);
        SpriteManager::requestSprite({*ViewPort::basicProgram,HeartIcon.get()},glm::vec4(center.x - width/2,healthRect.y,width,width*2),1);
    }

    for (int i = 0; i < GameState::getGameState()->getPlayerState().getHunger(); i ++)
    {
        int maxWidth = 40;
        int margin = 5;
        glm::vec2 center = {hungerRect.x + margin*(i+1) + (maxWidth/2)*i, hungerRect.y + hungerRect.a};
        int width = lerp(maxWidth,maxWidth*9/10,SDL_GetTicks()%10000 >= 9000);
        SpriteManager::requestSprite({*ViewPort::basicProgram,HungerIcon.get()},glm::vec4(center.x - width/2,healthRect.y + 60,width,width),1);
    }

    PolyRender::requestRect(GameUI::getHealthRect(),glm::vec4(1,0,0.5,1),false,0,1);
}

int PlayerState::getHealth() const
{
    return health;
}

void PlayerState::changeHealth(int health_)
{
    health = health_;
}

int PlayerState::getHunger() const
{
    return hunger;
}

int PlayerState::getMaxHunger() const
{
    return MAX_HUNGER;
}

void PlayerState::changeHunger(int hunger_)
{
    MasterCardsUI::getUI()->playerChangeHunger(hunger_);
    hunger = hunger_;
}

void PlayerState::decrementHunger(int rate)
{
    changeHunger(getHunger() - rate);
}

bool PlayerState::isDead() const
{
    return health <= 0;
}
