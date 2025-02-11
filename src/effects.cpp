#include "resourcesMaster.h"

#include "../headers/effects.h"
#include "../headers/gamestate.h"
#include "../headers/UI.h"

const glm::vec4 EffectsUI::bloodColor = {0.3,0,0,1};

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

Sequencer* EffectsUI::shuffleCard(EnemyCard& card)
{

    glm::vec2 screenDimen = ViewPort::getScreenDimen();
    glm::vec4 enemyRect = GameUI::getEnemyRect();
    glm::vec4 start = glm::vec4(0.5f*(screenDimen),0,0);
    glm::vec4 end = glm::vec4(0.5f*(screenDimen - glm::vec2(enemyRect.z,enemyRect.a)),enemyRect.z,enemyRect.a);
    Sequencer* sequence = new Sequencer([&card,start,end](int runtime)
                        {
                            CardUIOrient startO = {start ,0,GameUI::effectsZ};
                            CardUIOrient endO = {end,0,GameUI::effectsZ};
                            CardRenderer::render(card,squerp(startO,endO,runtime/1000.0,true,5));
                            return runtime >= 1000;
                        },
                        [&card,end](int runtime)
                        {
                            CardUIOrient startO = {end,0,GameUI::effectsZ,0};
                            CardUIOrient endO = {GameUI::getDeckRect(),0,GameUI::effectsZ,M_PI};
                            CardRenderer::render(card,squerp(startO,endO,runtime/1000.0f,true,3));
                            return runtime>=1000;
                        }
                        );
    return sequence;
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

