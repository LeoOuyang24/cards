#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <memory>

#include "render.h"

class PlayerUI
{
    std::unique_ptr<Sprite> HeartIcon;
    std::unique_ptr<Sprite> HungerIcon;
public:
    PlayerUI();
    void render();
};


class PlayerState
{
    int health = 3;
    int hunger = 3;
public:
    constexpr static int MAX_HUNGER = 3;
    constexpr static int MAX_HEALTH = 3;
    int getHealth() const;
    void changeHealth(int health);

    int getHunger() const;
    int getMaxHunger() const; //for now, just returns MAX_HUNGER. In the future maxhunger may be modified
    void changeHunger(int hunger_);
    void decrementHunger(int rate = 1);

    bool isDead() const;
};

#endif // PLAYER_H_INCLUDED
