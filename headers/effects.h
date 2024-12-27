#ifndef EFFECTS_H_INCLUDED
#define EFFECTS_H_INCLUDED

#include "render.h"
#include "FreeTypeHelper.h"
#include "sequencer.h"

#include "deck.h"
#include "enemyCards.h"

//handles visual effects
class EffectsUI
{
    typedef std::unique_ptr<BasicRenderPipeline> EffectsPipeline;
    //a shader for effects
    //rn it's basically just a glorified PolyRenderer that follows z sorting
    EffectsPipeline effectsProgram;

    //used for rendering a bleeding card
    EffectsPipeline bleedingCorpse;
    std::unique_ptr<Sprite> bloodSplatter;
    std::unique_ptr<Font> EffectsUIFont;

    Sequencer* floatingText(std::string text, const glm::vec3& color, const glm::vec2& point, Font& font) const; //renders some floating text
public:
    EffectsUI();
    Sequencer* shuffleCard(EnemyCard& card); //shuffle a card into the deck
    Sequencer* killAMfer() const;
    Sequencer* takeDamage() const;
    Sequencer* addHunger(int amount) const;
    Sequencer* die() const;
};

#endif // EFFECTS_H_INCLUDED
